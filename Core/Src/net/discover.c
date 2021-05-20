#include "main.h"

#include <string.h>
#include "pins.h"
#include "net/discover.h"
#include "net/ports.h"
#include "lwip/ip_addr.h"
#include "lwip/udp.h"
#include "stm32f7xx_hal.h"
#include "print.h"

static struct udp_pcb *server;
static const char *DEVICE_NAME = "Project-A DEMO";

/// The UDP discover packet receive callback.
static err_t Discover_Recv_Callback (void *u, struct udp_pcb *pcb, struct pbuf *packet, ip_addr_t *a, uint16_t port) {
    if (packet == NULL)
      return ERR_OK;

    // Reinterprets the payload as an discovery packet, and checks if the packet is an request
    //  and if the DevideID's match, if not just return.
    DiscoverPkt_t *discoverPkt = (DiscoverPkt_t *) packet->payload;
    if ( !(discoverPkt->Flags & DiscoverPkt_Flag_Request)
    || (discoverPkt->DevID != DiscoverPkt_DevidID_ProjectA)) {
        return ERR_OK;
    }

    // Creates the response packet, which will contain the device name.
    uint16_t deviceNameLength = strlen (DEVICE_NAME) + 1 /* +1: 0 term */;
    struct pbuf *response = pbuf_alloc (PBUF_TRANSPORT, sizeof (DiscoverPkt_t) + sizeof (DiscoverPkt_Response_t) + deviceNameLength, PBUF_RAM);
    if (response == NULL) {
        return ERR_MEM;
    }

    DiscoverPkt_t *responseDiscoverPkt = (DiscoverPkt_t * ) response->payload;
    responseDiscoverPkt->DevID = DiscoverPkt_DevidID_ProjectA;
    responseDiscoverPkt->Flags = DiscoverPkt_Flag_Response;

    DiscoverPkt_Response_t *responseDiscoverRespPacket = (DiscoverPkt_Response_t *) responseDiscoverPkt->Body;
    responseDiscoverRespPacket->Port = CONTROL_PORT;
    responseDiscoverRespPacket->NameLen = deviceNameLength;
    memcpy (responseDiscoverRespPacket->Name, DEVICE_NAME, deviceNameLength);

    // Sends the response packet, after which we clear the used memory.
    if (udp_sendto (pcb, response, a, port) != ERR_OK) {
        pbuf_free (response);
        pbuf_free (packet);
        return ERR_CONN;
    }

    ip_printf (*a, "Discovery Response Sent");

    pbuf_free (response);
    pbuf_free (packet);

    return ERR_OK;
}

/// Initializes the control TCP server.
void Discover_Init (void) {
    // Creates the new UDP server, if this fails call the error handler.
    if ((server = udp_new ()) == NULL) {
        Error_Handler ();
    }

    // Bind the new UDP server to DISCOVER_PORT.
    if (udp_bind (server, IP_ADDR_ANY, DISCOVER_PORT) != ERR_OK) {
        Error_Handler ();
    }

    // Sets the UDP packet callback.
    udp_recv (server,  (udp_recv_fn) &Discover_Recv_Callback, NULL);
}
