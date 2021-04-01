/*
 modified 12 Aug 2013
 by Soohwan Kim (suhwan@wiznet.co.kr)

 - 10 Apr. 2015
 Added support for Arduino Ethernet Shield 2
 by Arduino.org team
 
 */
#ifndef ethernet_h
#define ethernet_h

#include <inttypes.h>
#include "w5500.h"

//#include "EthernetClient.h"
//#include "EthernetServer.h"
//#include "Dhcp.h"


extern IP_AddressTypeDef ETH_dnsServerAddress;
extern char* ETH_dnsDomainName;
extern char* ETH_hostName;
//  extern DhcpClass* _dhcp;

static uint8_t ETH_state[MAX_SOCK_NUM] = {0,};
static uint16_t ETH_server_port[MAX_SOCK_NUM] = {0,};

// Initialize the Ethernet shield to use the provided MAC address and gain the rest of the
// configuration through DHCP.
// Returns 0 if the DHCP configuration failed, and 1 if it succeeded
int ETH_begin_DHCP(uint8_t *mac_address);
void ETH_begin(uint8_t *mac_address, IP_AddressTypeDef* local_ip);
int ETH_maintain(void);

IP_AddressTypeDef ETH_localIP(void);
IP_AddressTypeDef ETH_subnetMask(void);
IP_AddressTypeDef ETH_gatewayIP(void);
IP_AddressTypeDef ETH_dnsServerIP(void);
char* ETH_GetdnsDomainName(void);
char* ETH_GethostName(void);


#endif
