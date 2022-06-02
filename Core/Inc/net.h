/*
 * net.h
 *
 *  Created on: Jun 1, 2022
 *      Author: U30JC
 */
#ifndef INC_NET_H_
#define INC_NET_H_
//--------------------------------------------------
#include "stm32f4xx_hal.h"
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "enc28j60.h"
//--------------------------------------------------
#define IP_ADDR {192,168,0,102}
//--------------------------------------------------
typedef struct enc28j60_frame
{
	uint8_t addr_dest[6];
	uint8_t addr_src[6];
	uint16_t type;
	uint8_t data[];
} enc28j60_frame_ptr;
//--------------------------------------------------
typedef struct arp_msg
{
	uint16_t net_tp;
	uint16_t proto_tp;
	uint8_t macaddr_len;
	uint8_t ipaddr_len;
	uint16_t op;
	uint8_t macaddr_src[6];
	uint8_t ipaddr_src[4];
	uint8_t macaddr_dst[6];
	uint8_t ipaddr_dst[4];
} arp_msg_ptr;
//--------------------------------------------------
typedef struct ip_pkt
{
	uint8_t verlen; //protocol version and header length
	uint8_t ts; //type of services
	uint16_t len; //length
	uint16_t id; //packet ID
	uint16_t fl_frg_of; //flags and fragment offset
	uint8_t ttl; //lifetime
	uint8_t prt; //protocol type
	uint16_t cs; //header checksum
	uint8_t ipaddr_src[4]; //sender IP-address
	uint8_t ipaddr_dst[4]; //receiver IP-address
	uint8_t data[]; //data
} ip_pkt_ptr;
//--------------------------------------------------
typedef struct icmp_pkt{
uint8_t msg_tp;//service type
uint8_t msg_cd;//message code
uint16_t cs;//header checksum
uint16_t id;//packet ID
uint16_t num;//packet number
uint8_t data[];//data
} icmp_pkt_ptr;
//--------------------------------------------------
#define be16toword(a) ((((a)>>8)&0xff)|(((a)<<8)&0xff00)) //convert big endian to little endian

#define ETH_ARP be16toword(0x0806)
#define ETH_IP be16toword(0x0800)
//--------------------------------------------------
#define ARP_ETH be16toword(0x0001)
#define ARP_IP be16toword(0x0800)
#define ARP_REQUEST be16toword(1)
#define ARP_REPLY be16toword(2)
//--------------------------------------------------
#define IP_ICMP 1
#define IP_TCP 6
#define IP_UDP 17
//--------------------------------------------------
#define ICMP_REQ 8
#define ICMP_REPLY 0
//--------------------------------------------------
void net_ini(void);
void net_pool(void);
//--------------------------------------------------
#endif /* INC_NET_H_ */
