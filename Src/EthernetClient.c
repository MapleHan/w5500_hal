#include "string.h"
#include "EthernetClient.h"
//#include "EthernetServer.h"
//#include "Dns.h"

int ETH_Client_connect(ETH_ClientHandleTypeDef* cli,IP_AddressTypeDef ip, uint16_t port)
{
  if (cli->_sock != MAX_SOCK_NUM)
    return 0;

  for (int i = 0; i < MAX_SOCK_NUM; i++) {
    uint8_t s = W5500_readSnSR(i);
    if (s == W5500_SnSR_CLOSED || s == W5500_SnSR_FIN_WAIT || s == W5500_SnSR_CLOSE_WAIT) {
      cli->_sock = i;
      break;
    }
  }

  if (cli->_sock == MAX_SOCK_NUM)
    return 0;

  ETH_Client_srcport++;
  if (ETH_Client_srcport == 0) ETH_Client_srcport = 1024;
  SOCKET_socket(cli->_sock, W5500_SnMR_TCP, ETH_Client_srcport, 0);

  if (!SOCKET_connect(cli->_sock, ip.addr, port)) {
    cli->_sock = MAX_SOCK_NUM;
    return 0;
  }

  while (ETH_Client_status(cli) != W5500_SnSR_ESTABLISHED) {
    HAL_Delay(1);
    if (ETH_Client_status(cli) == W5500_SnSR_CLOSED) {
      cli->_sock = MAX_SOCK_NUM;
      return 0;
    }
  }

  return 1;
}

size_t ETH_Client_write(ETH_ClientHandleTypeDef* cli,uint8_t b) {
  return ETH_Client_write_Arr(cli,&b, 1);
}

size_t ETH_Client_write_Arr(ETH_ClientHandleTypeDef* cli,const uint8_t *buf, size_t size) {
  if (cli->_sock == MAX_SOCK_NUM) {
//    setWriteError();
    return 0;
  }
  if (!SOCKET_send(cli->_sock, buf, size)) {
//    setWriteError();
    return 0;
  }
  return size;
}

int ETH_Client_available(ETH_ClientHandleTypeDef* cli) {
  if (cli->_sock != MAX_SOCK_NUM)
    return W5500_getRXReceivedSize(cli->_sock);
  return 0;
}

int ETH_Client_read(ETH_ClientHandleTypeDef* cli) {
  uint8_t b;
  if ( SOCKET_recv(cli->_sock, &b, 1) > 0 )
  {
    // recv worked
    return b;
  }
  else
  {
    // No data available
    return -1;
  }
}

int ETH_Client_read_Arr(ETH_ClientHandleTypeDef* cli,uint8_t *buf, size_t size) {
  return SOCKET_recv(cli->_sock, buf, size);
}

int ETH_Client_peek(ETH_ClientHandleTypeDef* cli) {
  uint8_t b;
  // Unlike recv, peek doesn't check to see if there's any data available, so we must
  if (!ETH_Client_available(cli))
    return -1;
  SOCKET_peek(cli->_sock, &b);
  return b;
}

void ETH_Client_flush(ETH_ClientHandleTypeDef* cli) {
  SOCKET_flush(cli->_sock);
}

void ETH_Client_stop(ETH_ClientHandleTypeDef* cli) {
  if (cli->_sock == MAX_SOCK_NUM)
    return;

  // attempt to close the connection gracefully (send a FIN to other side)
  SOCKET_disconnect(cli->_sock);
  unsigned long start = HAL_GetTick();

  // wait a second for the connection to close
  while (ETH_Client_status(cli) != W5500_SnSR_CLOSED && HAL_GetTick() - start < 1000)
    HAL_Delay(1);

  // if it hasn't closed, close it forcefully
  if (ETH_Client_status(cli) != W5500_SnSR_CLOSED)
    SOCKET_close(cli->_sock);

  ETH_server_port[cli->_sock] = 0;
  cli->_sock = MAX_SOCK_NUM;
}

uint8_t ETH_Client_connected(ETH_ClientHandleTypeDef* cli) {
  if (cli->_sock == MAX_SOCK_NUM) return 0;
  
  uint8_t s = ETH_Client_status(cli);
  return !(s == W5500_SnSR_LISTEN || s == W5500_SnSR_CLOSED || s == W5500_SnSR_FIN_WAIT ||
    (s == W5500_SnSR_CLOSE_WAIT && !ETH_Client_available(cli)));
}

uint8_t ETH_Client_status(ETH_ClientHandleTypeDef* cli) {
  if (cli->_sock == MAX_SOCK_NUM) return W5500_SnSR_CLOSED;
  return W5500_readSnSR(cli->_sock);
}

//// the next function allows us to use the client returned by
//// EthernetServer::available() as the condition in an if-statement.

uint8_t ETH_Client_isOK(ETH_ClientHandleTypeDef* cli)
{
	return cli->_sock != MAX_SOCK_NUM;
}
uint8_t ETH_Client_isIdentical(ETH_ClientHandleTypeDef* cli,ETH_ClientHandleTypeDef* cli_rhs)
{
	return cli->_sock == cli_rhs->_sock && cli->_sock != MAX_SOCK_NUM && cli_rhs->_sock != MAX_SOCK_NUM;
}













