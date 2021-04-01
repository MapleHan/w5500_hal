#include "EthernetServer.h"

void ETH_Server_init(ETH_ServerHandleTypeDef* ser,uint16_t port)
{
  ser->_port = port;
}

void ETH_Server_begin(ETH_ServerHandleTypeDef* ser)
{
  for (int sock = 0; sock < MAX_SOCK_NUM; sock++) {
    ETH_ClientHandleTypeDef client = {sock};
    if (ETH_Client_status(&client) == W5500_SnSR_CLOSED) {
      SOCKET_socket(sock, W5500_SnMR_TCP, ser->_port, 0);
      SOCKET_listen(sock);
      ETH_server_port[sock] = ser->_port;
      break;
    }
  }  
}

void ETH_Server_accept(ETH_ServerHandleTypeDef* ser )
{
  int listening = 0;

  for (int sock = 0; sock < MAX_SOCK_NUM; sock++) {
    ETH_ClientHandleTypeDef client = {sock};
    if (ETH_server_port[sock] == ser->_port) {
//			DbgPrint("ACC ETH_Client_statu %d:%d\r\n",sock,ETH_Client_status(&client));
      if (ETH_Client_status(&client) == W5500_SnSR_LISTEN) {
        listening = 1;
      } 
      else if (ETH_Client_status(&client) == W5500_SnSR_CLOSE_WAIT && !ETH_Client_available(&client)) {
        ETH_Client_stop(&client);
      }
    } 
  }

  if (!listening) {
    ETH_Server_begin(ser);
  }
}

ETH_ClientHandleTypeDef ETH_Server_available(ETH_ServerHandleTypeDef* ser)
{
  ETH_Server_accept(ser);

  for (int sock = 0; sock < MAX_SOCK_NUM; sock++) {
    ETH_ClientHandleTypeDef client = {sock};
//		DbgPrint("ETH_Client_statu %d:%d\r\n",sock,ETH_Client_status(&client));
    if (ETH_server_port[sock] == ser->_port &&
        (ETH_Client_status(&client) == W5500_SnSR_ESTABLISHED ||
         ETH_Client_status(&client) == W5500_SnSR_CLOSE_WAIT)) {
      if (ETH_Client_available(&client)) {
        // XXX: don't always pick the lowest numbered socket.
        return client;
      }
    }
  }
	ETH_ClientHandleTypeDef client = {MAX_SOCK_NUM};
  return client;
}

size_t ETH_Server_write(ETH_ServerHandleTypeDef* ser,uint8_t b) 
{
  return ETH_Server_write_Arr(ser,&b, 1);
}

size_t ETH_Server_write_Arr(ETH_ServerHandleTypeDef* ser,const uint8_t *buffer, size_t size) 
{
  size_t n = 0;
  ETH_Server_accept(ser);

  for (int sock = 0; sock < MAX_SOCK_NUM; sock++) {
    ETH_ClientHandleTypeDef client = {sock};

    if (ETH_server_port[sock] == ser->_port &&
      ETH_Client_status(&client) == W5500_SnSR_ESTABLISHED) {
      n += ETH_Client_write_Arr(&client,buffer, size);
    }
  }
  
  return n;
}
