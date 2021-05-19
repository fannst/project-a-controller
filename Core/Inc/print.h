#ifndef __PRINT_H
#define __PRINT_H

#include "lwip/ip_addr.h"

#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

/// Printf version which is meant for socket connections.
void ip_printf (ip_addr_t addr, const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif
