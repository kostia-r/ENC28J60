/*
 * udp.h
 *
 *  Created on: Jun 3, 2022
 *      Author: U30JC
 */

#ifndef UDP_H_
#define UDP_H_
//--------------------------------------------------
#include "stm32f4xx_hal.h"
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "enc28j60.h"
#include "net.h"
//--------------------------------------------------
typedef struct udp_pkt
{
	uint16_t port_src; //sender port
	uint16_t port_dst; //receiver port
	uint16_t len; //length
	uint16_t cs; //header checksum
	uint8_t data[]; //data
} udp_pkt_ptr;
//--------------------------------------------------
#endif /* UDP_H_ */