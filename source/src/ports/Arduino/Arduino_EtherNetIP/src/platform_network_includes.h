//platform_network_includes.h replaces the original platform networking headers with the Opta/Mbed socket compatibility layer 
//and provides the required RESTRICT definition.

#ifndef PLATFORM_NETWORK_INCLUDES_H_
#define PLATFORM_NETWORK_INCLUDES_H_

#ifndef RESTRICT
#define RESTRICT __restrict
#endif

#include "opener_mbed_socket_compat.h"

#endif
