 /*
 * - 10 Apr. 2015
 * Added support for Arduino Ethernet Shield 2
 * by Arduino.org team
 */
 
#ifndef	_SOCKET_H_
#define	_SOCKET_H_

#include "w5500.h"

extern uint8_t SOCKET_socket(SOCKET s, uint8_t protocol, uint16_t port, uint8_t flag); // Opens a socket(TCP or UDP or IP_RAW mode)
extern void SOCKET_close(SOCKET s); // Close socket
extern uint8_t SOCKET_connect(SOCKET s, uint8_t * addr, uint16_t port); // Establish TCP connection (Active connection)
extern void SOCKET_disconnect(SOCKET s); // disconnect the connection
extern uint8_t SOCKET_listen(SOCKET s);	// Establish TCP connection (Passive connection)
extern uint16_t SOCKET_send(SOCKET s, const uint8_t * buf, uint16_t len); // Send data (TCP)
extern int16_t SOCKET_recv(SOCKET s, uint8_t * buf, int16_t len);	// Receive data (TCP)
extern uint16_t SOCKET_peek(SOCKET s, uint8_t *buf);
extern uint16_t SOCKET_sendto(SOCKET s, const uint8_t * buf, uint16_t len, uint8_t * addr, uint16_t port); // Send data (UDP/IP RAW)
extern uint16_t SOCKET_recvfrom(SOCKET s, uint8_t * buf, uint16_t len, uint8_t * addr, uint16_t *port); // Receive data (UDP/IP RAW)
extern void SOCKET_flush(SOCKET s); // Wait for transmission to complete

extern uint16_t SOCKET_igmpsend(SOCKET s, const uint8_t * buf, uint16_t len);

// Functions to allow buffered UDP send (i.e. where the UDP datagram is built up over a
// number of calls before being sent
/*
  @brief This function sets up a UDP datagram, the data for which will be provided by one
  or more calls to bufferData and then finally sent with sendUDP.
  @return 1 if the datagram was successfully set up, or 0 if there was an error
*/
extern int SOCKET_startUDP(SOCKET s, uint8_t* addr, uint16_t port);
/*
  @brief This function copies up to len bytes of data from buf into a UDP datagram to be
  sent later by sendUDP.  Allows datagrams to be built up from a series of bufferData calls.
  @return Number of bytes successfully buffered
*/
uint16_t SOCKET_bufferData(SOCKET s, uint16_t offset, const uint8_t* buf, uint16_t len);
/*
  @brief Send a UDP datagram built up from a sequence of startUDP followed by one or more
  calls to bufferData.
  @return 1 if the datagram was successfully sent, or 0 if there was an error
*/
int SOCKET_sendUDP(SOCKET s);

#endif
/* _SOCKET_H_ */
