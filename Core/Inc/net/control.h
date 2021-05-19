#ifndef __CONTROL_H
#define __CONTROL_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdarg.h>

#include "ring_buffer.h"
#include "stm32f767xx.h"

#define CONTROL_PKT_STEPPER_INFO_FLAG_ENABLED       (1 << 0)
#define CONTROL_PKT_STEPPER_INFO_FLAG_AUTOMATIC     (1 << 1)
#define CONTROL_PKT_STEPPER_INFO_FLAG_MOVING        (1 << 2)

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    ControlPkt_OP_ConnectionRequest = 0,
    ControlPkt_OP_ConnectionRequestApproved = 1,
    ControlPkt_OP_ConnectionRequestRejected = 2,
    ControlPkt_OP_StepperInfoRequest = 4,
    ControlPkt_OP_StepperMoveTo = 5,
    ControlPkt_OP_StepperEnableDisable = 6,
    ControlPkt_OP_StepperInfoResponse = 7
} ControlPkt_OP_t;

typedef struct __attribute__ (( packed )) {
    uint16_t    TotalLength;
    uint16_t    OPCode;
    uint8_t     Body[0];
} ControlPkt_t;

typedef struct __attribute__ (( packed )) {
    uint8_t     Motor;
    bool        Enable;
    bool        HasNext;
    uint8_t     Next[0];
} ControlPkt_EnableDisable_t;

typedef struct __attribute__ (( packed )) {
    uint8_t     Motor;
    uint8_t     Flags;
    int32_t     TargetPos;
    int32_t     CurrentPos;
    uint16_t    MinumumSpeed;
    uint16_t    CurrentSpeed;
    uint16_t    TargetSpeed;
    bool        HasNext;
    uint8_t     Next[0];  
} ControlPkt_StepperInfo_t;

typedef struct __attribute__ (( packed )) {
    uint8_t     Motor;
    int32_t     Position;
    bool        HasNext;
    uint8_t     Next[0];
} ControlPkt_StepperMoveTo_t;

typedef enum {
    NonAuthorized = 0,
    Authorized
} Control_TCPSession_State_t;

typedef struct {
    RingBuffer_t *RingBuffer;
    Control_TCPSession_State_t state;
} Control_TCPSession_t;

/// Initializes the control TCP server.
void Control_Init (void);

#ifdef __cplusplus
}
#endif

#endif
