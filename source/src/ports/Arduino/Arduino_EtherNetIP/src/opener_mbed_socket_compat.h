#ifndef OPENER_MBED_SOCKET_COMPAT_H_
#define OPENER_MBED_SOCKET_COMPAT_H_

/* C-compatible POSIX-like socket facade used by OpENer.  Do NOT include
 * Arduino/Mbed C++ headers from this file: generic_networkhandler.c is C. */
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned int socklen_t;

struct in_addr { uint32_t s_addr; };
struct sockaddr { uint16_t sa_family; char sa_data[14]; };
struct sockaddr_in {
  uint16_t sin_family;
  uint16_t sin_port;
  struct in_addr sin_addr;
  unsigned char sin_zero[8];
};

typedef struct opener_timeval_t {
  long tv_sec;
  long tv_usec;
} opener_timeval_t;

typedef struct opener_fd_set_t {
  uint64_t bits;
} opener_fd_set_t;

#define FD_ZERO(p) ((p)->bits = 0ULL)
#define FD_SET(fd,p) ((p)->bits |= (1ULL << (fd)))
#define FD_CLR(fd,p) ((p)->bits &= ~(1ULL << (fd)))
#define FD_ISSET(fd,p) (((p)->bits & (1ULL << (fd))) != 0ULL)


/* Prefix the facade symbols so they cannot collide with any POSIX/lwIP symbols
 * already present in the Arduino/Mbed link image. */
#define htons opener_htons
#define ntohs opener_ntohs
#define htonl opener_htonl
#define ntohl opener_ntohl
#define inet_addr opener_inet_addr
#define inet_ntop opener_inet_ntop
#define socket opener_socket
#define bind opener_bind
#define listen opener_listen
#define accept opener_accept
#define connect opener_connect
#define recv opener_recv
#define send opener_send
#define recvfrom opener_recvfrom
#define sendto opener_sendto
#define shutdown opener_shutdown
#define close opener_close
#define fcntl opener_fcntl
#define setsockopt opener_setsockopt
#define getpeername opener_getpeername

#ifndef AF_INET
#define AF_INET 2
#endif
#ifndef SOCK_STREAM
#define SOCK_STREAM 1
#endif
#ifndef SOCK_DGRAM
#define SOCK_DGRAM 2
#endif
#ifndef IPPROTO_IP
#define IPPROTO_IP 0
#endif
#ifndef IPPROTO_TCP
#define IPPROTO_TCP 6
#endif
#ifndef IPPROTO_UDP
#define IPPROTO_UDP 17
#endif
#ifndef SOL_SOCKET
#define SOL_SOCKET 0xFFFF
#endif
#ifndef SO_REUSEADDR
#define SO_REUSEADDR 2
#endif
#ifndef SO_BROADCAST
#define SO_BROADCAST 6
#endif
#ifndef IP_TOS
#define IP_TOS 1
#endif
#ifndef IP_MULTICAST_TTL
#define IP_MULTICAST_TTL 33
#endif
#ifndef IP_MULTICAST_IF
#define IP_MULTICAST_IF 32
#endif
#ifndef O_NONBLOCK
#define O_NONBLOCK 0x800
#endif
#ifndef F_GETFL
#define F_GETFL 3
#endif
#ifndef F_SETFL
#define F_SETFL 4
#endif
#ifndef SHUT_RDWR
#define SHUT_RDWR 2
#endif
#ifndef MSG_NOSIGNAL
#define MSG_NOSIGNAL 0
#endif
#ifndef INADDR_ANY
#define INADDR_ANY 0U
#endif
#ifndef INET_ADDRSTRLEN
#define INET_ADDRSTRLEN 16
#endif
#ifndef EWOULDBLOCK
#define EWOULDBLOCK 11
#endif

uint16_t htons(uint16_t v);
uint16_t ntohs(uint16_t v);
uint32_t htonl(uint32_t v);
uint32_t ntohl(uint32_t v);
uint32_t inet_addr(const char *cp);
const char *inet_ntop(int af, const void *src, char *dst, size_t size);

int socket(int domain, int type, int protocol);
int bind(int handle, const struct sockaddr *addr, socklen_t addrlen);
int listen(int handle, int backlog);
int accept(int handle, struct sockaddr *addr, socklen_t *addrlen);
int connect(int handle, const struct sockaddr *addr, socklen_t addrlen);
int recv(int handle, void *buf, size_t len, int flags);
int send(int handle, const void *buf, size_t len, int flags);
int recvfrom(int handle, void *buf, size_t len, int flags,
             struct sockaddr *from, socklen_t *fromlen);
int sendto(int handle, const void *buf, size_t len, int flags,
           const struct sockaddr *to, socklen_t tolen);
int shutdown(int handle, int how);
int close(int handle);
int fcntl(int handle, int cmd, ...);
int setsockopt(int handle, int level, int optname,
               const void *optval, socklen_t optlen);
int getpeername(int handle, struct sockaddr *addr, socklen_t *addrlen);
int opener_select(int nfds, opener_fd_set_t *readfds, opener_fd_set_t *writefds,
                  opener_fd_set_t *exceptfds, opener_timeval_t *timeout);


#ifdef __cplusplus
}

class NetworkInterface;
extern "C" void opener_set_network(NetworkInterface *net);
extern "C" NetworkInterface *opener_get_network(void);
#endif

#endif
