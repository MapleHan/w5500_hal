#ifndef ethernetclient_h
#define ethernetclient_h
#include "w5500.h"
#include "socket.h"
#include "Ethernet2.h"

typedef struct EthernetClient
{
	uint8_t _sock;
}ETH_ClientHandleTypeDef;

uint8_t ETH_Client_status(ETH_ClientHandleTypeDef* cli);
int ETH_Client_connect(ETH_ClientHandleTypeDef* cli,IP_AddressTypeDef ip, uint16_t port);
size_t ETH_Client_write(ETH_ClientHandleTypeDef* cli,uint8_t);
size_t ETH_Client_write_Arr(ETH_ClientHandleTypeDef* cli,const uint8_t *buf, size_t size);
int ETH_Client_available(ETH_ClientHandleTypeDef* cli);
int ETH_Client_read(ETH_ClientHandleTypeDef* cli);
int ETH_Client_read_Arr(ETH_ClientHandleTypeDef* cli,uint8_t *buf, size_t size);
int ETH_Client_peek(ETH_ClientHandleTypeDef* cli);
void ETH_Client_flush(ETH_ClientHandleTypeDef* cli);
void ETH_Client_stop(ETH_ClientHandleTypeDef* cli);
uint8_t ETH_Client_connected(ETH_ClientHandleTypeDef* cli);
uint8_t ETH_Client_isOK(ETH_ClientHandleTypeDef* cli);
uint8_t ETH_Client_isIdentical(ETH_ClientHandleTypeDef* cli,ETH_ClientHandleTypeDef* cli_rhs);

static uint16_t ETH_Client_srcport= 1024;

#endif
