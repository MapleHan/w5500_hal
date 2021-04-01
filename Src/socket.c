 /*
 * - 10 Apr. 2015
 * Added support for Arduino Ethernet Shield 2
 * by Arduino.org team
 */
 
#include "w5500.h"
#include "socket.h"

static uint16_t local_port;

/**
 * @brief	This Socket function initialize the channel in perticular mode, and set the port and wait for w5500 done it.
 * @return 	1 for success else 0.
 */
uint8_t SOCKET_socket(SOCKET s, uint8_t protocol, uint16_t port, uint8_t flag)
{
  if ((protocol == W5500_SnMR_TCP) || (protocol == W5500_SnMR_UDP) || (protocol == W5500_SnMR_IPRAW) || (protocol == W5500_SnMR_MACRAW) || (protocol == W5500_SnMR_PPPOE))
  {
    SOCKET_close(s);
    W5500_writeSnMR(s, protocol | flag);
    if (port != 0) {
      W5500_writeSnPORT(s, port);
    } 
    else {
      local_port++; // if don't set the source port, set local_port number.
      W5500_writeSnPORT(s, local_port);
    }
    W5500_execCmdSn(s, Sock_OPEN);
    return 1;
  }
  return 0;
}


/**
 * @brief	This function close the socket and parameter is "s" which represent the socket number
 */
void SOCKET_close(SOCKET s)
{
  W5500_execCmdSn(s, Sock_CLOSE);
  W5500_writeSnIR(s, 0xFF);
}


/**
 * @brief	This function established  the connection for the channel in passive (server) mode. This function waits for the request from the peer.
 * @return	1 for success else 0.
 */
uint8_t SOCKET_listen(SOCKET s)
{
  if (W5500_readSnSR(s) != W5500_SnSR_INIT)
    return 0;
  W5500_execCmdSn(s, Sock_LISTEN);
  return 1;
}


/**
 * @brief	This function established  the connection for the channel in Active (client) mode. 
 * 		This function waits for the untill the connection is established.
 * 		
 * @return	1 for success else 0.
 */
uint8_t SOCKET_connect(SOCKET s, uint8_t * addr, uint16_t port)
{
  if 
    (
  ((addr[0] == 0xFF) && (addr[1] == 0xFF) && (addr[2] == 0xFF) && (addr[3] == 0xFF)) ||
    ((addr[0] == 0x00) && (addr[1] == 0x00) && (addr[2] == 0x00) && (addr[3] == 0x00)) ||
    (port == 0x00) 
    ) 
    return 0;

  // set destination IP
  W5500_writeSnDIPR(s, addr);
  W5500_writeSnDPORT(s, port);
  W5500_execCmdSn(s, Sock_CONNECT);

  return 1;
}



/**
 * @brief	This function used for disconnect the socket and parameter is "s" which represent the socket number
 * @return	1 for success else 0.
 */
void SOCKET_disconnect(SOCKET s)
{
  W5500_execCmdSn(s, Sock_DISCON);
}


/**
 * @brief	This function used to send the data in TCP mode
 * @return	1 for success else 0.
 */
uint16_t SOCKET_send(SOCKET s, const uint8_t * buf, uint16_t len)
{
  uint8_t status=0;
  uint16_t ret=0;
  uint16_t freesize=0;

  if (len > W5500_SSIZE) 
    ret = W5500_SSIZE; // check size not to exceed MAX size.
  else 
    ret = len;

  // if freebuf is available, start.
  do 
  {
    freesize = W5500_getTXFreeSize(s);
    status = W5500_readSnSR(s);
    if ((status != W5500_SnSR_ESTABLISHED) && (status != W5500_SnSR_CLOSE_WAIT))
    {
      ret = 0; 
      break;
    }
  } 
  while (freesize < ret);

  // copy data
  W5500_send_data_processing(s, (uint8_t *)buf, ret);
  W5500_execCmdSn(s, Sock_SEND);

  /* +2008.01 bj */
  while ( (W5500_readSnIR(s) & W5500_SnIR_SEND_OK) != W5500_SnIR_SEND_OK ) 
  {
    /* m2008.01 [bj] : reduce code */
    if ( W5500_readSnSR(s) == W5500_SnSR_CLOSED )
    {
      SOCKET_close(s);
      return 0;
    }
  }
  /* +2008.01 bj */
  W5500_writeSnIR(s, W5500_SnIR_SEND_OK);
  return ret;
}


/**
 * @brief	This function is an application I/F function which is used to receive the data in TCP mode.
 * 		It continues to wait for data as much as the application wants to receive.
 * 		
 * @return	received data size for success else -1.
 */
int16_t SOCKET_recv(SOCKET s, uint8_t *buf, int16_t len)
{
  // Check how much data is available
  int16_t ret = W5500_getRXReceivedSize(s);
  if ( ret == 0 )
  {
    // No data available.
    uint8_t status = W5500_readSnSR(s);
    if ( status == W5500_SnSR_LISTEN || status == W5500_SnSR_CLOSED || status == W5500_SnSR_CLOSE_WAIT )
    {
      // The remote end has closed its side of the connection, so this is the eof state
      ret = 0;
    }
    else
    {
      // The connection is still up, but there's no data waiting to be read
      ret = -1;
    }
  }
  else if (ret > len)
  {
    ret = len;
  }

  if ( ret > 0 )
  {
    W5500_recv_data_processing(s, buf, ret,0);
    W5500_execCmdSn(s, Sock_RECV);
  }
  return ret;
}


/**
 * @brief	Returns the first byte in the receive queue (no checking)
 * 		
 * @return
 */
uint16_t SOCKET_peek(SOCKET s, uint8_t *buf)
{
  W5500_recv_data_processing(s, buf, 1, 1);

  return 1;
}


/**
 * @brief	This function is an application I/F function which is used to send the data for other then TCP mode. 
 * 		Unlike TCP transmission, The peer's destination address and the port is needed.
 * 		
 * @return	This function return send data size for success else -1.
 */
uint16_t SOCKET_sendto(SOCKET s, const uint8_t *buf, uint16_t len, uint8_t *addr, uint16_t port)
{
  uint16_t ret=0;

  if (len > W5500_SSIZE) ret = W5500_SSIZE; // check size not to exceed MAX size.
  else ret = len;

  if
    (
  ((addr[0] == 0x00) && (addr[1] == 0x00) && (addr[2] == 0x00) && (addr[3] == 0x00)) ||
    ((port == 0x00)) ||(ret == 0)
    ) 
  {
    /* +2008.01 [bj] : added return value */
    ret = 0;
  }
  else
  {
    W5500_writeSnDIPR(s, addr);
    W5500_writeSnDPORT(s, port);

    // copy data
    W5500_send_data_processing(s, (uint8_t *)buf, ret);
    W5500_execCmdSn(s, Sock_SEND);

    /* +2008.01 bj */
    while ( (W5500_readSnIR(s) & W5500_SnIR_SEND_OK) != W5500_SnIR_SEND_OK ) 
    {
      if (W5500_readSnIR(s) & W5500_SnIR_TIMEOUT)
      {
        /* +2008.01 [bj]: clear interrupt */
        W5500_writeSnIR(s, (W5500_SnIR_SEND_OK | W5500_SnIR_TIMEOUT)); /* clear SEND_OK & TIMEOUT */
        return 0;
      }
    }

    /* +2008.01 bj */
    W5500_writeSnIR(s, W5500_SnIR_SEND_OK);
  }
  return ret;
}


/**
 * @brief	This function is an application I/F function which is used to receive the data in other then
 * 	TCP mode. This function is used to receive UDP, IP_RAW and MAC_RAW mode, and handle the header as well. 
 * 	
 * @return	This function return received data size for success else -1.
 */
uint16_t SOCKET_recvfrom(SOCKET s, uint8_t *buf, uint16_t len, uint8_t *addr, uint16_t *port)
{
  uint8_t head[8];
  uint16_t data_len=0;
  uint16_t ptr=0;

  if ( len > 0 )
  {
    ptr = W5500_readSnRX_RD(s);
    switch (W5500_readSnMR(s) & 0x07)
    {
    case W5500_SnMR_UDP :
      W5500_read_data(s, ptr, head, 0x08);
      ptr += 8;
      // read peer's IP address, port number.
      addr[0] = head[0];
      addr[1] = head[1];
      addr[2] = head[2];
      addr[3] = head[3];
      *port = head[4];
      *port = (*port << 8) + head[5];
      data_len = head[6];
      data_len = (data_len << 8) + head[7];

      W5500_read_data(s, ptr, buf, data_len); // data copy.
      ptr += data_len;

      W5500_writeSnRX_RD(s, ptr);
      break;

    case W5500_SnMR_IPRAW :
      W5500_read_data(s, ptr, head, 0x06);
      ptr += 6;

      addr[0] = head[0];
      addr[1] = head[1];
      addr[2] = head[2];
      addr[3] = head[3];
      data_len = head[4];
      data_len = (data_len << 8) + head[5];

      W5500_read_data(s, ptr, buf, data_len); // data copy.
      ptr += data_len;

      W5500_writeSnRX_RD(s, ptr);
      break;

    case W5500_SnMR_MACRAW:
      W5500_read_data(s, ptr, head, 2);
      ptr+=2;
      data_len = head[0];
      data_len = (data_len<<8) + head[1] - 2;

      W5500_read_data(s, ptr, buf, data_len);
      ptr += data_len;
      W5500_writeSnRX_RD(s, ptr);
      break;

    default :
      break;
    }
    W5500_execCmdSn(s, Sock_RECV);
  }
  return data_len;
}

/**
 * @brief	Wait for buffered transmission to complete.
 */
void SOCKET_flush(SOCKET s) {
  // TODO
}

uint16_t SOCKET_igmpsend(SOCKET s, const uint8_t * buf, uint16_t len)
{
  uint8_t status=0;
  uint16_t ret=0;

  if (len > W5500_SSIZE) 
    ret = W5500_SSIZE; // check size not to exceed MAX size.
  else 
    ret = len;

  if (ret == 0)
    return 0;

  W5500_send_data_processing(s, (uint8_t *)buf, ret);
  W5500_execCmdSn(s, Sock_SEND);

  while ( (W5500_readSnIR(s) & W5500_SnIR_SEND_OK) != W5500_SnIR_SEND_OK ) 
  {
    status = W5500_readSnSR(s);
    if (W5500_readSnIR(s) & W5500_SnIR_TIMEOUT)
    {
      /* in case of igmp, if send fails, then socket closed */
      /* if you want change, remove this code. */
      SOCKET_close(s);
      return 0;
    }
  }

  W5500_writeSnIR(s, W5500_SnIR_SEND_OK);
  return ret;
}

uint16_t SOCKET_bufferData(SOCKET s, uint16_t offset, const uint8_t* buf, uint16_t len)
{
  uint16_t ret =0;
  if (len > W5500_getTXFreeSize(s))
  {
    ret = W5500_getTXFreeSize(s); // check size not to exceed MAX size.
  }
  else
  {
    ret = len;
  }
  W5500_send_data_processing_offset(s, offset, buf, ret);
  return ret;
}

int SOCKET_startUDP(SOCKET s, uint8_t* addr, uint16_t port)
{
  if
    (
     ((addr[0] == 0x00) && (addr[1] == 0x00) && (addr[2] == 0x00) && (addr[3] == 0x00)) ||
     ((port == 0x00))
    ) 
  {
    return 0;
  }
  else
  {
    W5500_writeSnDIPR(s, addr);
    W5500_writeSnDPORT(s, port);
    return 1;
  }
}

int SOCKET_sendUDP(SOCKET s)
{
  W5500_execCmdSn(s, Sock_SEND);
		
  /* +2008.01 bj */
  while ( (W5500_readSnIR(s) & W5500_SnIR_SEND_OK) != W5500_SnIR_SEND_OK ) 
  {
    if (W5500_readSnIR(s) & W5500_SnIR_TIMEOUT)
    {
      /* +2008.01 [bj]: clear interrupt */
      W5500_writeSnIR(s, (W5500_SnIR_SEND_OK|W5500_SnIR_TIMEOUT));
      return 0;
    }
  }

  /* +2008.01 bj */	
  W5500_writeSnIR(s, W5500_SnIR_SEND_OK);

  /* Sent ok */
  return 1;
}

