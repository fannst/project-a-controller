#include "net/control.h"
#include "lwip/tcp.h"
#include "main.h"
#include "net/ports.h"
#include "print.h"
#include "stepper.h"

#define CONTROL_MAX_CONNECTION_COUNT        1

static struct tcp_pcb *server = NULL;
static uint32_t s_ConnectionCount = 0;

extern Stepper_t *g_Steppers[6];
extern uint32_t g_StepperCount;

static void Control_CloseSocket (Control_TCPSession_t *session, struct tcp_pcb *pcb) {
    // Checks if the session is authorized, if so first decrement the connection counter.
    if (session->state == Authorized) {
        ip_printf (pcb->remote_ip, "Authorized Client Disconnected");
        --s_ConnectionCount;
    } else {
        ip_printf (pcb->remote_ip, "Client Disconnected");
    }

    // Free the ring buffer first, after which we free the session.
    RingBuffer_Free (&session->RingBuffer);
    free (session);

    // Close the TCP connection, and return OK.
    tcp_close (pcb);
}

/// Handles an protocol connection request.
static void Control_HandleConnectionRequestPacket (Control_TCPSession_t *session, const ControlPkt_t *packet, struct tcp_pcb *pcb) {
    // Allocates the memory for the response pkt, and sets the size.
    ControlPkt_t *response = (ControlPkt_t *) malloc (sizeof (ControlPkt_t));
    if (response == NULL) {
        // TODO: Handle Error
        return;
    }

    response->TotalLength = sizeof (ControlPkt_t);

    // Checks if we should accept the connection, since there is a limited amount
    //  of masters which may be connected.
    if (s_ConnectionCount >= CONTROL_MAX_CONNECTION_COUNT) {
        // Sets the opcode.
        response->OPCode = ControlPkt_OP_ConnectionRequestRejected;

        // Prints the rejected message.
        ip_printf (pcb->remote_ip, "Client Rejected");
    } else  {
        // Sets the connection priority to normal, and increments the connection count.
        tcp_setprio (pcb, TCP_PRIO_NORMAL);
        ++s_ConnectionCount;

        // Sets the response opcode, and updates the session state to authorized.
        response->OPCode = ControlPkt_OP_ConnectionRequestApproved;
        session->state = Authorized;

        // Prints the authorized message.
        ip_printf (pcb->remote_ip, "Client Authorized");
    }

    // Sends the response packet.
    if (tcp_write (pcb, response, sizeof (ControlPkt_t), TCP_WRITE_FLAG_COPY) != ERR_OK) {
        Control_CloseSocket (session, pcb);
        free (response);
        return;
    }

    // Frees the response packet memory.
    free (response);
}

/// Handles an stepper move to operation.
static void Control_HandleStepperMoveTo (Control_TCPSession_t *session, const ControlPkt_t *packet, struct tcp_pcb *pcb) {
    // If we're not authorized, we cannot move steppers.
    if (session->state != Authorized)
        return;

    // Parses the stepper move to packet.
    ControlPkt_StepperMoveTo_t *move = (ControlPkt_StepperMoveTo_t *) packet->Body;
    
    do {
        // Prints the console update message.
        ip_printf (pcb->remote_ip, "Moving motor %lu to %ld", move->Motor, move->Position);

        // Checks which motor it is, and moves it.

        // Checks if there is an next one, if so set it to the move variable,
        //  else we will set it to null.
        move = (move->HasNext ? (ControlPkt_StepperMoveTo_t *) move->Next : NULL);
    } while (move != NULL);
}


/// Handles an stepper enable/disable to operation.
static void Control_HandleStepperEnableDisable (Control_TCPSession_t *session, const ControlPkt_t *packet, struct tcp_pcb *pcb) {
    // If we're not authorized, we cannot enable/disable steppers.
    if (session->state != Authorized)
        return;

    // Parses the stepper move to packet.
    ControlPkt_EnableDisable_t *enadisa = (ControlPkt_EnableDisable_t *) packet->Body;
    
    do {
        // Prints the console update message.
        ip_printf (pcb->remote_ip, "%s Motor %u", (enadisa->Enable ? "Enabling" : "Disabling"), enadisa->Motor);

        if (enadisa->Motor < g_StepperCount) {
            g_Steppers[enadisa->Motor]->enabled = enadisa->Enable;
        }

        // Checks if there is an next one, if so set it to the move variable,
        //  else we will set it to null.
        enadisa = (enadisa->HasNext ? (ControlPkt_StepperMoveTo_t *) enadisa->Next : NULL);
    } while (enadisa != NULL);
}

/// Handles an stepper info request.
static void Control_HandleStepperInfoRequest (Control_TCPSession_t *session, const ControlPkt_t *packet, struct tcp_pcb *pcb) {
    // If we're not authorized, we cannot deliver stepper info.
    if (session->state != Authorized)
        return;

    // Allocates the memory for the response packet.
    ControlPkt_t *response = (ControlPkt_t *) malloc (sizeof (ControlPkt_t) + (sizeof (ControlPkt_StepperInfo_t) * g_StepperCount));
    if (response == NULL) {
        // TODO: Handle Memory Error
        return;
    }

    // Configures the response.
    response->TotalLength = sizeof (ControlPkt_t) + (sizeof (ControlPkt_StepperInfo_t) * g_StepperCount);
    response->OPCode = ControlPkt_OP_StepperInfoResponse;

    // Adds the motor info to the packet.
    ControlPkt_StepperInfo_t *info = (ControlPkt_StepperInfo_t *) response->Body;
    for (uint32_t i = 0; i < g_StepperCount; ++i) {
        info->CurrentPos = g_Steppers[i]->current_position;
        info->TargetPos = g_Steppers[i]->target_position;

        info->MinumumSpeed = g_Steppers[i]->min_sps;
        info->CurrentSpeed = g_Steppers[i]->current_sps;
        info->TargetSpeed = g_Steppers[i]->max_sps;
        
        info->Flags = (g_Steppers[i]->enabled ? CONTROL_PKT_STEPPER_INFO_FLAG_ENABLED : 0)
            | (g_Steppers[i]->moving ? CONTROL_PKT_STEPPER_INFO_FLAG_MOVING : 0)
            | (g_Steppers[i]->auto_enabled ? CONTROL_PKT_STEPPER_INFO_FLAG_AUTOMATIC : 0);
        info->Motor = (uint8_t) i;

        info->HasNext = (i + 1 < g_StepperCount);

        info = info->Next;
    }

    // Sends the response packet.
    if (tcp_write (pcb, response, response->TotalLength, TCP_WRITE_FLAG_COPY) != ERR_OK) {
        Control_CloseSocket (session, pcb);
        free (response);
        return;
    }

    // Prints the message.
    ip_printf (pcb->remote_ip, "Responded to info.");

    // Frees the response.
    free (response);
}
static err_t Control_RecvHandler (void *u, struct tcp_pcb *pcb, struct pbuf *p, err_t err) {
    Control_TCPSession_t *session = (Control_TCPSession_t *) u;
    
    // ERR_OK and P == NULL means the client closed the connection.
    if (err == ERR_OK && p == NULL) {
        Control_CloseSocket (session, pcb);
        return ERR_OK;
    }

    // Handles if new data is received, and acklowledges it, and writes it to the 
    //  ring buffer.
    if (p != NULL) {
        // Acknowledges the received bytes.
        tcp_recved (pcb, p->tot_len);

        // Loops over the packet chain, and adds the bytes to the ring buffer.
        struct pbuf *current = p;
        do {
            RingBuffer_AddBytes (session->RingBuffer, current->payload, current->len);
            current = current->next;
        } while (current != NULL);

        // Frees the packet buffer.
        pbuf_free (p);

        // Checks if the ring buffer contains any packets we can actually use.
        while (session->RingBuffer->size >= 2) {
            // Reads the total length of the packet in the buffer.
            uint16_t totalPktLength = 0;
            if (RingBuffer_Peek (session->RingBuffer, (uint8_t *) &totalPktLength, 2) != RingBuffer_Err_OK) {
                return;
            }

            // If there is not enough data for the single expected packet, break the loop. 
            if (session->RingBuffer->size < totalPktLength)
                break;

            // Allocates the memory for the packet to be read.
            uint8_t *packet = (uint8_t *) malloc (totalPktLength);
            if (packet == NULL) {
                // TODO: Handle this error
                break;
            }

            // Reads the N bytes from the ring buffer.
            RingBuffer_ReadBytes (session->RingBuffer, packet, totalPktLength);

            // Processes the command packet.
            ControlPkt_t *pkt = (ControlPkt_t *) packet;
            switch (pkt->OPCode) {
                case ControlPkt_OP_ConnectionRequest:
                    Control_HandleConnectionRequestPacket (session, pkt, pcb);
                    break;
                case ControlPkt_OP_StepperInfoRequest:
                    Control_HandleStepperInfoRequest (session, pkt, pcb);
                    break;
                case ControlPkt_OP_StepperMoveTo:
                    Control_HandleStepperMoveTo (session, pkt, pcb);
                    break;
                case ControlPkt_OP_StepperEnableDisable:
                    Control_HandleStepperEnableDisable (session, pkt, pcb);
                    break;
            }

            // Frees the allocated memory.
            free (packet);
        }
    }

    return ERR_OK;
}

/// The accept handler for the control TCP server.
static err_t Control_AcceptHandler (void *u, struct tcp_pcb *pcb, err_t err) {
    // Allocates the memory for the session, if this fails close the connection and return ERR_MEM code.
    Control_TCPSession_t *session = (Control_TCPSession_t *) malloc (sizeof (Control_TCPSession_t));
    if (session == NULL) {
        tcp_close (pcb);
        return ERR_MEM;
    }

    session->state = NonAuthorized;

    // Allocates the memory for the ring buffer, if this fails close the connection and return ERR_MEM code.
    if (RingBuffer_New (&session->RingBuffer, 256) != RingBuffer_Err_OK) {
        tcp_close (pcb);
        return ERR_MEM;
    }

    // Configures the TCP connection, set priority to minimum, set the byte receive handler,
    //  set the error handler, and the polling interval.
    tcp_arg (pcb, (void *) session);
    tcp_setprio (pcb, TCP_PRIO_MIN);
    tcp_recv (pcb, (tcp_recv_fn *) Control_RecvHandler);
    tcp_err (pcb, NULL);
    tcp_poll (pcb, NULL, 4);

    // Prints the update message.
    ip_printf (pcb->remote_ip, "Client Connected");

    // Return OK since everything was successful.
    return ERR_OK;
}

/// Initializes the control TCP server.
void Control_Init (void) {
    // Create the new TCP server.
    if ((server = tcp_new ()) == NULL) {
        Error_Handler ();
    }

    // Bind the TCP socket to port CONTROL_PORT.
    if (tcp_bind (server, IP_ADDR_ANY, CONTROL_PORT) != ERR_OK) {
        Error_Handler ();
    }

    // Starts the TCP listener, and set the new accept handler.
    server = tcp_listen (server);
    tcp_accept (server, (tcp_accept_fn) Control_AcceptHandler);
}
