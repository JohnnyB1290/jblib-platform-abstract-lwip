/**
 * @file
 * lwIP Ethernet Interface Realization for JbLib
 *
 */

/*
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
 * Copyright © 2019 Evgeniy Ivanov. Contacts: <strelok1290@gmail.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */

// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

/*
 * This file is a skeleton for developing Ethernet network interface
 * drivers for lwIP. Add code to the low_level functions and do a
 * search-and-replace for the word "ethernetif" to replace it with
 * something that better describes your network interface.
 */

#include <string.h>
#include "arch/ethernetif.h"
#include "jbkernel/IVoidEthernet.hpp"

using namespace jblib::jbkernel;

/**
 * This function should do the actual transmission of the packet. The packet is
 * contained in the pbuf that is passed to the function. This pbuf
 * might be chained.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @param p the MAC packet to send (e.g. IP packet including MAC addresses and type)
 * @return ERR_OK if the packet could be sent
 *         an err_t value if the packet couldn't be sent
 *
 * @note Returning ERR_MEM here if a DMA queue of your MAC is full can lead to
 *       strange results. You might consider waiting for space in the DMA queue
 *       to become available since the stack doesn't retry to send a packet
 *       dropped because of memory failure (except for the TCP timers).
 */

static err_t ethernetifOutput(struct netif* netif, struct pbuf* p)
{
	((IVoidEthernet*)netif->state)->addToTxQueue(p);
	LINK_STATS_INC(link.xmit);
	return ERR_OK;
}

/**
 * This function should be called when a packet is ready to be read
 * from the interface. It uses the function low_level_input() that
 * should handle the actual reception of bytes from the network
 * interface. Then the type of the received packet is determined and
 * the appropriate input function is called.
 *
 * @param netif the lwip network interface structure for this ethernetif
 */
void lwipEthernetifInput(struct netif* netif, EthernetFrame* frame, uint16_t size)
{
	struct pbuf* p = pbuf_alloc(PBUF_RAW, (u16_t)size, PBUF_REF);
	if (p == NULL) {
		LWIP_DEBUGF(EMAC_DEBUG | LWIP_DBG_TRACE, ("LW IP low_level_input: could not allocate RX pbuf\n\r"));
		LINK_STATS_INC(link.memerr);
		LINK_STATS_INC(link.drop);
		return;
	}
	p->payload = (void*)frame;
	p->len = p->tot_len = size;
	/* points to packet payload, which starts with an Ethernet header */
	struct eth_hdr* ethhdr = (eth_hdr*)p->payload;
	switch (htons(ethhdr->type))
	{
		case ETHTYPE_IP:
		case ETHTYPE_ARP:
#if PPPOE_SUPPORT
		case ETHTYPE_PPPOEDISC:
		case ETHTYPE_PPPOE:
#endif /* PPPOE_SUPPORT */
		{
			LINK_STATS_INC(link.recv);
			/* full packet send to tcpip_thread to process */
			if (netif->input(p, netif) != ERR_OK) {
				LWIP_DEBUGF(NETIF_DEBUG,("lpc_enetif_input: IP input error\n"));
				/* Free buffer */
				pbuf_free(p);
				p = NULL;
			}
			else
				LINK_STATS_INC(link.recv);
		}
		break;
		default:
		{
			/* Return buffer */
			LINK_STATS_INC(link.proterr);
			pbuf_free(p);
			p = NULL;
		}
		break;
	}
}

/**
 * Should be called at the beginning of the program to set up the
 * network interface. It calls the function low_level_init() to do the
 * actual setup of the hardware.
 *
 * This function should be passed as a parameter to netif_add().
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return ERR_OK if the loopif is initialized
 *         ERR_MEM if private data couldn't be allocated
 *         any other err_t on error
 */
err_t lwipEthernetifInit(struct netif* netif)
{
	static uint8_t* mac;
	((IVoidEthernet*)netif->state)->getParameter(PARAMETER_MAC, (void*)&mac);
	memcpy(netif->hwaddr, mac, 6);
	netif->hwaddr_len = ETHARP_HWADDR_LEN;
	netif->mtu = 1500;
	netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_UP | NETIF_FLAG_ETHERNET;

	/* device capabilities */
#if LWIP_IGMP == 1
	netif->flags = netif->flags | NETIF_FLAG_IGMP;
#endif

#if LWIP_NETIF_HOSTNAME
	/* Initialize interface hostname */
	netif->hostname = "JB_ETH";
#endif /* LWIP_NETIF_HOSTNAME */

	netif->name[0] = 'e';
	netif->name[1] = 'n';
	/* We directly use etharp_output() here to save a function call.
	 * You can instead declare your own function an call etharp_output()
	 * from it if you have to do some checks before sending (e.g. if link
	 * is available...) */
	netif->output = etharp_output;
#if LWIP_IPV6
	netif->output_ip6 = ethip6_output;
#endif /* LWIP_IPV6 */

	netif->linkoutput = ethernetifOutput;

#if LWIP_IPV6 && LWIP_IPV6_MLD
	/*
	 * For hardware/netifs that implement MAC filtering.
	 * All-nodes link-local is handled by default, so we must let the hardware know
	 * to allow multicast packets in.
	 * Should set mld_mac_filter previously. */
	if (netif->mld_mac_filter != NULL)
	{
	  ip6_addr_t ip6_allnodes_ll;
	  ip6_addr_set_allnodes_linklocal(&ip6_allnodes_ll);
	  netif->mld_mac_filter(netif, &ip6_allnodes_ll, NETIF_ADD_MAC_FILTER);
	}
#endif /* LWIP_IPV6 && LWIP_IPV6_MLD */

	return ERR_OK;
}

