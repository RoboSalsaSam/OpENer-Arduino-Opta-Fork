#include <Arduino.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "netsocket/Socket.h"
#include "netsocket/SocketAddress.h"
#include "netsocket/TCPSocket.h"
#include "netsocket/UDPSocket.h"
#include "netsocket/NetworkInterface.h"
#include "netsocket/nsapi_types.h"

#include "opener_mbed_socket_compat.h"


/* The compatibility header maps POSIX names for OpENer C files.  In this
 * C++ implementation we must remove those macros so calls on Mbed Socket
 * objects such as sock->bind() are not rewritten to opener_bind(). */
#undef htons
#undef ntohs
#undef htonl
#undef ntohl
#undef inet_addr
#undef inet_ntop
#undef socket
#undef bind
#undef listen
#undef accept
#undef connect
#undef recv
#undef send
#undef recvfrom
#undef sendto
#undef shutdown
#undef close
#undef fcntl
#undef setsockopt
#undef getpeername

namespace {
constexpr int MAX_SOCKETS = 32;

struct Slot {
  Socket *sock;
  bool udp;
  bool used;
};

Slot slots[MAX_SOCKETS] = {};
NetworkInterface *netif = nullptr;


uint16_t bswap16(uint16_t x) {
  return static_cast<uint16_t>((x << 8) | (x >> 8));
}

uint32_t bswap32(uint32_t x) {
  return ((x & 0x000000ffUL) << 24) |
         ((x & 0x0000ff00UL) << 8) |
         ((x & 0x00ff0000UL) >> 8) |
         ((x & 0xff000000UL) >> 24);
}

void seterr(int e) {
  errno = e > 0 ? e : EIO;
}

int map_error(int e) {
  if (e >= 0) return 0;
  switch (e) {
    case NSAPI_ERROR_WOULD_BLOCK: return EWOULDBLOCK;
    case NSAPI_ERROR_ADDRESS_IN_USE: return EADDRINUSE;
    case NSAPI_ERROR_NO_CONNECTION: return ENETDOWN;
    case NSAPI_ERROR_NO_SOCKET: return EMFILE;
    case NSAPI_ERROR_NO_MEMORY: return ENOMEM;
    case NSAPI_ERROR_PARAMETER: return EINVAL;
    case NSAPI_ERROR_IS_CONNECTED: return EISCONN;
    case NSAPI_ERROR_CONNECTION_LOST: return ECONNRESET;
    case NSAPI_ERROR_CONNECTION_TIMEOUT: return ETIMEDOUT;
    default: return EIO;
  }
}

Slot *getslot(int h) {
  if (h < 0 || h >= MAX_SOCKETS || !slots[h].used) return nullptr;
  return &slots[h];
}

int alloc_slot(Socket *sock, bool udp) {
  for (int i = 0; i < MAX_SOCKETS; ++i) {
    if (!slots[i].used) {
      slots[i].sock = sock;
      slots[i].udp = udp;
      slots[i].used = true;
      return i;
    }
  }
  delete sock;
  seterr(EMFILE);
  return -1;
}

void to_mbed_address(const sockaddr_in *in, SocketAddress &out) {
  const uint32_t h = opener_ntohl(in->sin_addr.s_addr);
  char ip[INET_ADDRSTRLEN];
  snprintf(ip, sizeof(ip), "%u.%u.%u.%u",
           static_cast<unsigned>((h >> 24) & 255),
           static_cast<unsigned>((h >> 16) & 255),
           static_cast<unsigned>((h >> 8) & 255),
           static_cast<unsigned>(h & 255));
  out.set_ip_address(ip);
  out.set_port(opener_ntohs(in->sin_port));
}

void from_mbed_address(const SocketAddress &in, sockaddr_in *out) {
  memset(out, 0, sizeof(*out));
  out->sin_family = AF_INET;
  out->sin_port = opener_htons(in.get_port());
  out->sin_addr.s_addr = opener_inet_addr(in.get_ip_address());
}
}

extern "C" void opener_set_network(NetworkInterface *net) {
  netif = net;
}

extern "C" NetworkInterface *opener_get_network(void) {
  return netif;
}

extern "C" {

uint16_t opener_htons(uint16_t v) { return bswap16(v); }
uint16_t opener_ntohs(uint16_t v) { return bswap16(v); }
uint32_t opener_htonl(uint32_t v) { return bswap32(v); }
uint32_t opener_ntohl(uint32_t v) { return bswap32(v); }

uint32_t opener_inet_addr(const char *cp) {
  unsigned a, b, c, d;
  char tail;
  if (!cp || sscanf(cp, "%u.%u.%u.%u%c", &a, &b, &c, &d, &tail) != 4 ||
      a > 255 || b > 255 || c > 255 || d > 255) {
    return 0xFFFFFFFFUL;
  }
  const uint32_t host = (a << 24) | (b << 16) | (c << 8) | d;
  return opener_htonl(host);
}

const char *opener_inet_ntop(int af, const void *src, char *dst, size_t size) {
  if (af != AF_INET || !src || !dst || size < INET_ADDRSTRLEN) return nullptr;
  const uint32_t h = opener_ntohl(*static_cast<const uint32_t *>(src));
  snprintf(dst, size, "%u.%u.%u.%u",
           static_cast<unsigned>((h >> 24) & 255),
           static_cast<unsigned>((h >> 16) & 255),
           static_cast<unsigned>((h >> 8) & 255),
           static_cast<unsigned>(h & 255));
  return dst;
}

int opener_socket(int domain, int type, int protocol) {
  if (domain != AF_INET || netif == nullptr) {
    seterr(EINVAL);
    return -1;
  }

  const bool want_udp = (type == SOCK_DGRAM || protocol == IPPROTO_UDP);
  const bool want_tcp = (type == SOCK_STREAM || protocol == IPPROTO_TCP);
  if (!want_udp && !want_tcp) {
    seterr(EINVAL);
    return -1;
  }

  if (want_tcp) {
    TCPSocket *tcp = new TCPSocket();
    const nsapi_error_t r = tcp->open(netif);
    if (r < 0) {
      seterr(map_error(r));
      delete tcp;
      return -1;
    }
    tcp->set_blocking(false);
    int h = alloc_slot(tcp, false);
    return h;
  }

  UDPSocket *udp = new UDPSocket();
  const nsapi_error_t r = udp->open(netif);
  if (r < 0) {
    seterr(map_error(r));
    delete udp;
    return -1;
  }
  udp->set_blocking(false);
  int h = alloc_slot(udp, true);
  return h;
}

int opener_bind(int handle, const struct sockaddr *addr, socklen_t) {
  Slot *s = getslot(handle);
  if (!s || !addr) { seterr(EBADF); return -1; }
  SocketAddress a;
  to_mbed_address(reinterpret_cast<const sockaddr_in *>(addr), a);
  const nsapi_error_t r = s->sock->bind(a);
  return 0;
}

int opener_listen(int handle, int backlog) {
  Slot *s = getslot(handle);
  if (!s || s->udp) { seterr(EBADF); return -1; }
  const nsapi_error_t r = s->sock->listen(backlog);
  return 0;
}

int opener_accept(int handle, struct sockaddr *addr, socklen_t *addrlen) {
  Slot *s = getslot(handle);
  if (!s || s->udp) { seterr(EBADF); return -1; }

  nsapi_error_t err = NSAPI_ERROR_OK;
  Socket *child = s->sock->accept(&err);
  if (!child) {
    seterr(map_error(err));
    return -1;
  }

  child->set_blocking(false);
  const int h = alloc_slot(child, false);
  if (h < 0) return -1;

  if (addr && addrlen && *addrlen >= sizeof(sockaddr_in)) {
    SocketAddress peer;
    if (child->getpeername(&peer) == NSAPI_ERROR_OK) {
      from_mbed_address(peer, reinterpret_cast<sockaddr_in *>(addr));
      *addrlen = sizeof(sockaddr_in);
    }
  }
  return h;
}

int opener_connect(int handle, const struct sockaddr *addr, socklen_t) {
  Slot *s = getslot(handle);
  if (!s || !addr) { seterr(EBADF); return -1; }
  SocketAddress a;
  to_mbed_address(reinterpret_cast<const sockaddr_in *>(addr), a);
  const nsapi_error_t r = s->sock->connect(a);
  if (r < 0) { seterr(map_error(r)); return -1; }
  return 0;
}

int opener_recv(int handle, void *buf, size_t len, int) {
  Slot *s = getslot(handle);
  if (!s) { seterr(EBADF); return -1; }
  const nsapi_size_or_error_t r = s->sock->recv(buf, static_cast<nsapi_size_t>(len));
  if (r < 0) {
    seterr(map_error(r));
    return -1;
  }
  return r;
}

int opener_send(int handle, const void *buf, size_t len, int) {
  Slot *s = getslot(handle);
  if (!s) { seterr(EBADF); return -1; }
  const nsapi_size_or_error_t r = s->sock->send(buf, static_cast<nsapi_size_t>(len));
  if (r < 0) {
    seterr(map_error(r));
    return -1;
  }
  return r;
}

int opener_recvfrom(int handle, void *buf, size_t len, int,
             struct sockaddr *from, socklen_t *fromlen) {
  Slot *s = getslot(handle);
  if (!s) { seterr(EBADF); return -1; }
  SocketAddress peer;
  const nsapi_size_or_error_t r = s->sock->recvfrom(&peer, buf, static_cast<nsapi_size_t>(len));
  if (r < 0) {
    seterr(map_error(r));
    return -1;
  }
  if (from && fromlen && *fromlen >= sizeof(sockaddr_in)) {
    from_mbed_address(peer, reinterpret_cast<sockaddr_in *>(from));
    *fromlen = sizeof(sockaddr_in);
  }
  return r;
}

int opener_sendto(int handle, const void *buf, size_t len, int,
           const struct sockaddr *to, socklen_t) {
  Slot *s = getslot(handle);
  if (!s || !to) { seterr(EBADF); return -1; }
  SocketAddress a;
  to_mbed_address(reinterpret_cast<const sockaddr_in *>(to), a);
  const nsapi_size_or_error_t r = s->sock->sendto(a, buf, static_cast<nsapi_size_t>(len));
  if (r < 0) {
    seterr(map_error(r));
    return -1;
  }
  return r;
}

int opener_shutdown(int handle, int) {
  Slot *s = getslot(handle);
  if (!s) { seterr(EBADF); return -1; }
  return 0;
}

int opener_close(int handle) {
  Slot *s = getslot(handle);
  if (!s) {
    seterr(EBADF);
    return -1;
  }
  const nsapi_error_t r = s->sock->close();

  /* Mbed OS owns the lifetime of the object returned by Socket::accept().
   * Its close() operation releases the socket resources and the returned
   * pointer must not be referenced or deleted afterwards.  The Mbed API
   * explicitly documents that no separate delete is required after close().
   *
   * This is why v36 was stable while v35/v37/v39 crashed: those versions
   * attempted to destroy an object whose lifetime had already been consumed
   * by close().  Clear our OpENer handle immediately and never touch the
   * returned pointer again.
   */

  s->sock = nullptr;
  s->used = false;
  return (r < 0) ? -1 : 0;
}

int opener_fcntl(int handle, int cmd, ...) {
  Slot *s = getslot(handle);
  if (!s) { seterr(EBADF); return -1; }
  if (cmd == F_SETFL) {
    va_list ap;
    va_start(ap, cmd);
    (void)va_arg(ap, int);
    va_end(ap);
    s->sock->set_blocking(false);
    return 0;
  }
  if (cmd == F_GETFL) return O_NONBLOCK;
  seterr(EINVAL);
  return -1;
}

int opener_setsockopt(int handle, int level, int optname,
               const void *optval, socklen_t optlen) {
  Slot *s = getslot(handle);
  if (!s) { seterr(EBADF); return -1; }

  /* Map the POSIX names used by OpENer to Mbed's standardized socket options. */
  int mbed_level = level;
  int mbed_opt = optname;
  if (level == SOL_SOCKET) {
    mbed_level = NSAPI_SOCKET;
    if (optname == SO_REUSEADDR) mbed_opt = NSAPI_REUSEADDR;
    else if (optname == SO_BROADCAST) mbed_opt = NSAPI_BROADCAST;
    else return 0;
  } else if (level == IPPROTO_IP) {
    /* OpENer uses these for multicast configuration.  Accept them here because
       lwIP/Mbed's underlying stack handles the normal multicast path. */
    if (optname == IP_TOS || optname == IP_MULTICAST_TTL || optname == IP_MULTICAST_IF) {
      if (optname != IP_TOS) return 0;
      mbed_level = NSAPI_SOCKET;
      mbed_opt = NSAPI_IPTOS;
    } else {
      return 0;
    }
  }

  const nsapi_error_t r = s->sock->setsockopt(mbed_level, mbed_opt, optval,
                                               static_cast<unsigned>(optlen));
  if (r < 0 && r != NSAPI_ERROR_UNSUPPORTED) {
    seterr(map_error(r));
    return -1;
  }
  /* Unsupported QoS/reuse details are non-fatal for the basic EtherNet/IP path. */
  return 0;
}

int opener_getpeername(int handle, struct sockaddr *addr, socklen_t *addrlen) {
  Slot *s = getslot(handle);
  if (!s || !addr || !addrlen) { seterr(EBADF); return -1; }
  if (*addrlen < sizeof(sockaddr_in)) { seterr(EINVAL); return -1; }
  SocketAddress peer;
  const nsapi_error_t r = s->sock->getpeername(&peer);
  if (r < 0) { seterr(map_error(r)); return -1; }
  from_mbed_address(peer, reinterpret_cast<sockaddr_in *>(addr));
  *addrlen = sizeof(sockaddr_in);
  return 0;
}

int opener_select(int nfds, opener_fd_set_t *readfds, opener_fd_set_t *, opener_fd_set_t *, opener_timeval_t *timeout) {
  unsigned long ms = 1;
  if (timeout) {
    ms = static_cast<unsigned long>(timeout->tv_sec * 1000L + timeout->tv_usec / 1000L);
  }
  if (ms > 10) ms = 10;
  delay(ms);

  if (!readfds) return 0;
  const uint64_t requested = readfds->bits;
  int count = 0;
  for (int i = 0; i < nfds && i < 64; ++i) {
    if (requested & (1ULL << i)) ++count;
  }
  /* All sockets are non-blocking.  Returning the requested set lets OpENer
     probe each socket; recv/accept returns EWOULDBLOCK when nothing is there. */
  readfds->bits = requested;
  return count;
}

}
