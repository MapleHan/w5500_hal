#ifndef ethernetserver_h
#define ethernetserver_h
#include "w5500.h"
#include "socket.h"
#include "string.h"
#include "Ethernet2.h"
#include "EthernetClient.h"

typedef struct EthernetServer
{
	uint16_t _port;
}ETH_ServerHandleTypeDef;

void ETH_Server_accept(ETH_ServerHandleTypeDef* ser);
void ETH_Server_Init(ETH_ServerHandleTypeDef* ser,uint16_t port);
ETH_ClientHandleTypeDef ETH_Server_available(ETH_ServerHandleTypeDef* ser);
void ETH_Server_begin(ETH_ServerHandleTypeDef* ser);
size_t ETH_Server_write(ETH_ServerHandleTypeDef* ser,uint8_t b);
size_t ETH_Server_write_Arr(ETH_ServerHandleTypeDef* ser,const uint8_t *buf, size_t size);

#endif
