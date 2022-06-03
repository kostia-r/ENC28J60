/*
 * net.c
 *
 *  Created on: Jun 1, 2022
 *      Author: U30JC
 */
#include "net.h"
//--------------------------------------------------
extern UART_HandleTypeDef huart1;
//--------------------------------------------------
extern uint8_t macaddr[6];
uint8_t net_buf[ENC28J60_MAXFRAME];
uint8_t ipaddr[4] = IP_ADDR;
char str1[60] = { 0 };
extern char str[20];
uint32_t clock_cnt = 0;
USART_prop_ptr usartprop;
//--------------------------------------------------
uint8_t arp_read(enc28j60_frame_ptr *frame, uint16_t len);
void arp_send(enc28j60_frame_ptr *frame);
void eth_read(enc28j60_frame_ptr *frame, uint16_t len);
void eth_send(enc28j60_frame_ptr *frame, uint16_t len);
uint8_t icmp_read(enc28j60_frame_ptr *frame, uint16_t len);
//--------------------------------------------------
void net_ini(void)
{
	usartprop.usart_buf[0] = 0;
	usartprop.usart_cnt = 0;
	usartprop.is_ip = 0;
	HAL_UART_Transmit(&huart1, (uint8_t*) "123456\r\n", 8, 0x1000);
	enc28j60_ini();
}

//--------------------------------------------------
uint16_t checksum(uint8_t *ptr, uint16_t len, uint8_t type)
{
	uint32_t sum = 0;

	if (type == 1)
	{
		sum += IP_UDP;
		sum += len - 8;
	}

	while (len > 0)
	{
		sum += (uint16_t) (((uint32_t) *ptr << 8) | *(ptr + 1));
		ptr += 2;
		len -= 2;
	}

	if (len)
		sum += ((uint32_t) *ptr) << 8;

	while (sum >> 16)
		sum = (uint16_t) sum + (sum >> 16);

	return ~be16toword((uint16_t )sum);
}
//--------------------------------------------------
uint8_t ip_read(enc28j60_frame_ptr *frame, uint16_t len)
{
	uint8_t res = 0;
	ip_pkt_ptr *ip_pkt = (void*) (frame->data);

	if ((ip_pkt->verlen == 0x45) && (!memcmp(ip_pkt->ipaddr_dst, ipaddr, 4)))
	{
		len = be16toword(ip_pkt->len) - sizeof(ip_pkt_ptr);

		if (ip_pkt->prt == IP_ICMP)
		{
			icmp_read(frame, len);
		}
		else if (ip_pkt->prt == IP_TCP)
		{
		}
		else if (ip_pkt->prt == IP_UDP)
		{
			udp_read(frame, len);
		}
	}
	return res;
}

//--------------------------------------------------
uint8_t ip_send(enc28j60_frame_ptr *frame, uint16_t len)
{
	uint8_t res = 0;
	ip_pkt_ptr *ip_pkt = (void*) frame->data;

	//Fill in the IP packet header
	ip_pkt->len = be16toword(len);
	ip_pkt->fl_frg_of = 0;
	ip_pkt->ttl = 128;
	ip_pkt->cs = 0;
	memcpy(ip_pkt->ipaddr_dst, ip_pkt->ipaddr_src, 4);
	memcpy(ip_pkt->ipaddr_src, ipaddr, 4);
	ip_pkt->cs = checksum((void*) ip_pkt, sizeof(ip_pkt_ptr), 0);

	//sending frame
	eth_send(frame, len);
	return res;
}

//-----------------------------------------------
uint16_t port_extract(char *ip_str, uint8_t len)
{
	uint16_t port = 0;
	int ch1 = ':';
	char *ss1;
	uint8_t offset = 0;
	ss1 = strchr(ip_str, ch1);
	offset = ss1 - ip_str + 1;
	ip_str += offset;
	port = atoi(ip_str);
	return port;
}
//-----------------------------------------------
//Convert string IP-value to 32-bits integer
void ip_extract(char *ip_str, uint8_t len, uint8_t *ipextp)
{
	uint8_t offset = 0;
	uint8_t i;
	char ss2[5] = { 0 };
	char *ss1;
	int ch1 = '.';
	int ch2 = ':';

	for (i = 0; i < 3; i++)
	{
		ss1 = strchr(ip_str, ch1);
		offset = ss1 - ip_str + 1;
		strncpy(ss2, ip_str, offset);
		ss2[offset] = 0;
		ipextp[i] = atoi(ss2);
		ip_str += offset;
		len -= offset;
	}

	ss1 = strchr(ip_str, ch2);

	if (ss1 != NULL)
	{
		offset = ss1 - ip_str + 1;
		strncpy(ss2, ip_str, offset);
		ss2[offset] = 0;
		ipextp[3] = atoi(ss2);
		return;
	}

	strncpy(ss2, ip_str, len);
	ss2[len] = 0;
	ipextp[3] = atoi(ss2);
}

//--------------------------------------------------
void eth_read(enc28j60_frame_ptr *frame, uint16_t len)
{
	uint8_t res = 0;
	if (len > sizeof(enc28j60_frame_ptr))
	{
		if (frame->type == ETH_ARP)
		{
			res = arp_read(frame, len - sizeof(enc28j60_frame_ptr));
			if (res == 1)
			{
				arp_send(frame);
			}
			else if (res == 2)
			{
				arp_table_fill(frame);
			}
			if (usartprop.is_ip == 3) //UDP packet sending status
			{
				memcpy(frame->addr_dest, frame->addr_src, 6);
				net_cmd();
			}
		}
		if (frame->type == ETH_IP)
		{
			ip_read(frame, len - sizeof(ip_pkt_ptr));
		}
		else
		{
			sprintf(str1, "%02X:%02X:%02X:%02X:%02X:%02X-%02X:%02X:%02X:%02X:%02X:%02X; %d; %04X", frame->addr_src[0],
					frame->addr_src[1], frame->addr_src[2], frame->addr_src[3], frame->addr_src[4], frame->addr_src[5],
					frame->addr_dest[0], frame->addr_dest[1], frame->addr_dest[2], frame->addr_dest[3],
					frame->addr_dest[4], frame->addr_dest[5], len, be16toword(frame->type));
			HAL_UART_Transmit(&huart1, (uint8_t*) str1, strlen(str1), 0x1000);
			HAL_UART_Transmit(&huart1, (uint8_t*) "\r\n", 2, 0x1000);
		}
	}
}

//--------------------------------------------------
void eth_send(enc28j60_frame_ptr *frame, uint16_t len)
{
	memcpy(frame->addr_src, macaddr, 6);
	enc28j60_packetSend((void*) frame, len + sizeof(enc28j60_frame_ptr));
}

//--------------------------------------------------
void net_poll(void)
{
	uint16_t len;
	//uint8_t ip[4] = { 0 };

	enc28j60_frame_ptr *frame = (void*) net_buf;

	while ((len = enc28j60_packetReceive(net_buf, sizeof(net_buf))) > 0)
	{
		eth_read(frame, len);
	}
}

//-----------------------------------------------
void net_cmd(void)
{
	static uint8_t ip[4] = { 0 };
	static uint16_t port = 0;

	if (usartprop.is_ip == 1)	//ARP request sending status
	{
		ip_extract((char*) usartprop.usart_buf, usartprop.usart_cnt, ip);
		arp_request(ip);
		usartprop.is_ip = 0;
		usartprop.usart_cnt = 0;
	}
	else if (usartprop.is_ip == 2)	//attempt status to send UDP-packet
	{
		ip_extract((char*) usartprop.usart_buf, usartprop.usart_cnt, ip);
		usartprop.is_ip = 3;	//attempt status to send UDP-packet
		usartprop.usart_cnt = 0;
		arp_request(ip);	//find out mac-address
	}
	else if (usartprop.is_ip == 3)	//attempt status to send UDP-packet
	{
		port = port_extract((char*) usartprop.usart_buf, usartprop.usart_cnt);
		udp_send(ip, port);
		usartprop.is_ip = 0;
	}
}

//--------------------------------------------------
uint8_t icmp_read(enc28j60_frame_ptr *frame, uint16_t len)
{
	uint8_t res = 0;
	ip_pkt_ptr *ip_pkt = (void*) (frame->data);
	icmp_pkt_ptr *icmp_pkt = (void*) (ip_pkt->data);

	//Filter the packet by message length and type - echo request
	if ((len >= sizeof(icmp_pkt_ptr)) && (icmp_pkt->msg_tp == ICMP_REQ))
	{
		icmp_pkt->msg_tp = ICMP_REPLY;
		icmp_pkt->cs = 0;
		icmp_pkt->cs = checksum((void*) icmp_pkt, len, 0);

		memcpy(frame->addr_dest, frame->addr_src, 6);
		ip_send(frame, len + sizeof(ip_pkt_ptr));
		sprintf(str1, "%d.%d.%d.%d-%d.%d.%d.%d icmp request\r\n", ip_pkt->ipaddr_dst[0], ip_pkt->ipaddr_dst[1],
				ip_pkt->ipaddr_dst[2], ip_pkt->ipaddr_dst[3], ip_pkt->ipaddr_src[0], ip_pkt->ipaddr_src[1],
				ip_pkt->ipaddr_src[2], ip_pkt->ipaddr_src[3]);
		HAL_UART_Transmit(&huart1, (uint8_t*) str1, strlen(str1), 0x1000);
	}

	return res;
}
//--------------------------------------------------
void UART1_RxCpltCallback(void)
{
	uint8_t b;
	b = str[0];

	//check if the buffer length is exceeded
	if (usartprop.usart_cnt > 25)
	{
		usartprop.usart_cnt = 0;
	}
	else if (b == 'a')
	{
		usartprop.is_ip = 1; //ARP-request sending status
		net_cmd();
	}
	else if (b == 'u')
	{
		usartprop.is_ip = 2; //attempt status to send UDP-packet
		net_cmd();
	}
	else
	{
		usartprop.usart_buf[usartprop.usart_cnt] = b;
		usartprop.usart_cnt++;
	}
	HAL_UART_Receive_IT(&huart1, (uint8_t*) str, 1);
}

//--------------------------------------------------
void TIM_PeriodElapsedCallback(void)
{
	clock_cnt++;
}
