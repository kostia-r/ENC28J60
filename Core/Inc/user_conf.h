/*
 * user_conf.h
 *
 *  Created on: Jun 4, 2022
 *      Author: U30JC
 */

#ifndef USER_CONF_H_
#define USER_CONF_H_

#define MAC_ADDR   				{0x00,0x15,0x42,0xBF,0xF0,0x51}
#define IP_ADDR 				{192,168,0,102}
#define IP_GATE 				{192,168,0,1}
#define LOCAL_PORT 				333
#define ICMP_REQUEST_DATA 		("abcdefghijklmnopqrstuvwabcdefghi")
#define UDP_CLIENT_REPLY_DATA 	("UDP Reply:\r\nHello to UDP Client!!!\r\n")
#define UDP_SERVER_REPLY_DATA 	("UDP Reply:\r\nHello from UDP Server to UDP Client!!!\r\n")

#endif /* USER_CONF_H_ */
