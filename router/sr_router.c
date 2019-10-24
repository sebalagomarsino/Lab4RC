/**********************************************************************
 * file:  sr_router.c
 * date:  Mon Feb 18 12:50:42 PST 2002
 * Contact: casado@stanford.edu
 *
 * Description:
 *
 * This file contains all the functions that interact directly
 * with the routing table, as well as the main entry method
 * for routing.
 *
 **********************************************************************/

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "sr_if.h"
#include "sr_rt.h"
#include "sr_router.h"
#include "sr_protocol.h"
#include "sr_arpcache.h"
#include "sr_utils.h"

/*---------------------------------------------------------------------
 * Method: sr_init(void)
 * Scope:  Global
 *
 * Initialize the routing subsystem
 *
 *---------------------------------------------------------------------*/

void sr_init(struct sr_instance* sr)
{
    /* REQUIRES */
    assert(sr);

    /* Initialize cache and cache cleanup thread */
    sr_arpcache_init(&(sr->cache));

    pthread_attr_init(&(sr->attr));
    pthread_attr_setdetachstate(&(sr->attr), PTHREAD_CREATE_JOINABLE);
    pthread_attr_setscope(&(sr->attr), PTHREAD_SCOPE_SYSTEM);
    pthread_attr_setscope(&(sr->attr), PTHREAD_SCOPE_SYSTEM);
    pthread_t thread;

    pthread_create(&thread, &(sr->attr), sr_arpcache_timeout, sr);
    
    /* Add initialization code here! */

} /* -- sr_init -- */

/* Send an ARP request. */
void sr_arp_request_send(struct sr_instance *sr, uint32_t ip) {

}

/* Send an ICMP error. */
void sr_send_icmp_error_packet(uint8_t type,
                              uint8_t code,
                              struct sr_instance *sr,
                              uint32_t ipDst,
                              uint8_t *ipPacket)
{

}

void sr_handle_arp_packet(struct sr_instance *sr,
        uint8_t *packet /* lent */,
        unsigned int len,
        uint8_t *srcAddr,
        uint8_t *destAddr,
        char *interface /* lent */,
        sr_ethernet_hdr_t *eHdr) {
  
    /* Get ARP header and addresses */  
    sr_arp_hdr_t *arp_hdr = (sr_arp_hdr_t *)(packet);
  
    /* add or update sender to ARP cache*/
    sr_arpreq arpreq = sr_arpcache_insert(sr -> cache, (char*) srcAddr, arp_hdr -> sip);
    
    /* check if the ARP packet is for one of my interfaces. */
    sr_if *lista_if = sr -> if_list;
    bool esDeMiInterfaz = false; 
    for (int i = 0; i < sizeof(lista_if) && !esDeMiInterfaz; i++) {
            if (strcmp(interface, lista_if[i].name) != 0) 
                esDeMiInterfaz = true;
    }
    
    if (esDeMiInterfaz) {
        /* check if it is a request or reply*/
        if (arp_hdr -> ar_op == htons(1)) { // Si es request == 1 (broadcast)
            /* if it is a request, construct and send an ARP reply*/ 
            int arpPacketLen = sizeof(eHdr) + sizeof(arp_hdr);
            uint8_t *arpPacket = malloc(arpPacketLen);
            
            sr_ethernet_hdr_t *ethHdr = (struct sr_ethernet_hdr *) arpPacket;
            memcpy(ethHdr -> ether_dhost, arp_hdr -> tha, ETHER_ADDR_LEN);
            
            memcpy(ethHdr -> ether_shost, arp_hdr -> sha, sizeof(uint8_t) * ETHER_ADDR_LEN);
            ethHdr -> ether_type = htons(ethertype_arp);
            
            sr_arp_hdr_t *arpHdr = (sr_arp_hdr_t *) (arpPacket + sizeof(sr_ethernet_hdr_t));
            arpHdr -> ar_hrd = htons(1); // Valor de Ethernet = 1.
            arpHdr -> ar_pro = htons(2048); // Valor de IPv4 = 2048
            arpHdr -> ar_hlen = 6; // Direcciones Ethernet MAC son de 48 bits (6 bytes).
            arpHdr -> ar_pln = 4; // Dirreciones IPv4 addresses son de 32 bits (4 bytes).
            arpHdr -> ar_op = htons(arp_op_request);
            memcpy (arpHdr -> ar_sha, (char *) destAddr, ETHER_ADDR_LEN);
            memcpy (arpHdr -> ar_tha, (char *) srcAddr, ETHER_ADDR_LEN);
            arpHdr -> ar_
            arpHdr -> ar_

            
        } else { 
            if (arp_hdr -> ar_op == htons(2)) {// Si es reply == 2
            /* else if it is a reply, add to ARP cache if necessary and send packets waiting for that reply*/
            } else {
                packet.2Heaven;
            }
        }
        
       
  

        

    } else {
        packet.2Heaven;
    }
}

void sr_handle_ip_packet(struct sr_instance *sr,
        uint8_t *packet /* lent */,
        unsigned int len,
        uint8_t *srcAddr,
        uint8_t *destAddr,
        char *interface /* lent */,
        sr_ethernet_hdr_t *eHdr) {


	/* Get IP header and addresses */

	/* Check if packet is for me or the destination is in my routing table*/

	/* If non of the above is true, send ICMP net unreachable */

	/* Else if for me, check if ICMP and act accordingly*/

	/* Else, check TTL, ARP and forward if corresponds (may need an ARP request and wait for the reply) */

}

/*---------------------------------------------------------------------
 * Method: sr_handlepacket(uint8_t* p,char* interface)
 * Scope:  Global
 *
 * This method is called each time the router receives a packet on the
 * interface.  The packet buffer, the packet length and the receiving
 * interface are passed in as parameters. The packet is complete with
 * ethernet headers.
 *
 * Note: Both the packet buffer and the character's memory are handled
 * by sr_vns_comm.c that means do NOT delete either.  Make a copy of the
 * packet instead if you intend to keep it around beyond the scope of
 * the method call.
 *
 *---------------------------------------------------------------------*/

void sr_handlepacket(struct sr_instance* sr,
        uint8_t * packet/* lent */,
        unsigned int len,
        char* interface/* lent */)
{
  /* REQUIRES */
  assert(sr);
  assert(packet);
  assert(interface);

  printf("*** -> Received packet of length %d \n",len);

  /* Obtain dest and src MAC address */
  sr_ethernet_hdr_t *eHdr = (sr_ethernet_hdr_t *) packet;
  uint8_t *destAddr = malloc(sizeof(uint8_t) * ETHER_ADDR_LEN);
  uint8_t *srcAddr = malloc(sizeof(uint8_t) * ETHER_ADDR_LEN);
  memcpy(destAddr, eHdr->ether_dhost, sizeof(uint8_t) * ETHER_ADDR_LEN);
  memcpy(srcAddr, eHdr->ether_shost, sizeof(uint8_t) * ETHER_ADDR_LEN);
  uint16_t pktType = ntohs(eHdr->ether_type);

  if (is_packet_valid(packet, len)) {
    if (pktType == ethertype_arp) {
      sr_handle_arp_packet(sr, packet, len, srcAddr, destAddr, interface, eHdr);
    } else if (pktType == ethertype_ip) {
      sr_handle_ip_packet(sr, packet, len, srcAddr, destAddr, interface, eHdr);
    }
  }

}/* end sr_ForwardPacket */

