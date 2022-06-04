# Networking. ENC28J60

1. USART terminal command(f.e.PuTTY): plink -serial COM3 -sercfg 115200,8,n,1,N

2. Send ARP-request from MCU to local node in LAN, ro remote node: put in USART terminal {IP}a
   Hint: to check remote node IP, execute in host terminal next command: "ping www.google.com"

3. Send UDP-packet from local node to MCU in LAN using NetCat tool for Windows: nc -u {IP_ADDR} {LOCAL_PORT}

4. Send UDP-packet from MCU to port 57387 of host in LAN: put in USART terminal {IP}:57387u

5. General config defines in user_conf.h:
   #define MAC_ADDR   				{0x00,0x15,0x42,0xBF,0xF0,0x51}
   #define IP_ADDR 				{192,168,0,102}
   #define IP_GATE 				{192,168,0,1}
   #define LOCAL_PORT 				333
   #define ICMP_REQUEST_DATA 		("abcdefghijklmnopqrstuvwabcdefghi")
   #define UDP_CLIENT_REPLY_DATA ("UDP Reply:\r\nHello to UDP Client!!!\r\n")
   #define UDP_SERVER_REPLY_DATA	("UDP Reply:\r\nHello from UDP Server to UDP Client!!!\r\n")
   
6. plink and NetCat look in ""Win_Tools" path.

# Connections
INT - PA2
SO - PA6
SI - PA7
SCK - PA5
CS - PA4
RESET - NRST
VCC - VCC 3.3V
GND - GND