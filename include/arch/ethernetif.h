/*
 * ethernetif.h
 *
 *  Created on: 23.03.2017
 *      Author: Stalker1290
 */
#include "chip.h"
#include "lwip/opt.h"
#include "lwip/opt.h"
#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include "lwip/stats.h"
#include "lwip/snmp.h"
#include "lwip/ethip6.h"
#include "lwip/etharp.h"
#include "netif/ppp/pppoe.h"

#ifndef ETHERNETIF_H_
#define ETHERNETIF_H_

#ifdef __cplusplus
extern "C" {
#endif
	
err_t lwip_ethernetif_init(struct netif* netif);
void lwip_ethernetif_input(struct netif* netif, EthernetFrame* Frame_ptr, uint16_t frame_size);

#ifdef __cplusplus
}
#endif

#endif /* ETHERNETIF_H_ */
