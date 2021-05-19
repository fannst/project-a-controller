#ifndef __DISCOVER_H
#define __DISCOVER_H

#include <stdint.h>
#include "stm32f767xx.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    DiscoverPkt_Flag_Request = (1 << 0),
    DiscoverPkt_Flag_Response = (1 << 1)
} DiscoverPkt_Flag_t;

typedef enum {
    DiscoverPkt_DevidID_ProjectA = 0x7132
} DiscoverPkt_DevID_t;

typedef struct __attribute__ (( packed )) {
    uint16_t    DevID;          /* DeviceID, for example ProjectA */
    uint8_t     Flags;          /* Discover Packet Flags */
    uint8_t     Body[0];        /* Discover Packet Body */
} DiscoverPkt_t;

typedef struct __attribute__ (( packed )) {
    uint16_t    Port;           /* Control Port */
    uint16_t    NameLen;        /* Device Name Length */
    char        Name[0];        /* Device Name */
} DiscoverPkt_Response_t;

/// Initializes the discover UDP server.
void Discover_Init (void);

#ifdef __cplusplus
}
#endif

#endif
