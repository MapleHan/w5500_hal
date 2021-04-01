/*
* Copyright (c) 2010 by WIZnet <support@wiznet.co.kr>
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 *
 * - 10 Apr. 2015
 * Added support for Arduino Ethernet Shield 2
 * by Arduino.org team
 */
#include "stdint.h"
#include "main.h"
#include "spi.h"
#ifndef	W5500_H_INCLUDED
#define	W5500_H_INCLUDED
#define NC (int)0xFFFFFFFF
#define MAX_SOCK_NUM 8
typedef struct __IP_AddressTypeDef
{
	uint8_t addr[4];
}IP_AddressTypeDef;

  // could do inline optimizations
  void W5500_SPI_initSS(void);
  void W5500_SPI_setSS(void);
  void W5500_SPI_resetSS(void);
  void W5500_SPI_Begin(void);
  void W5500_SPI_Write(uint8_t data);
  void W5500_SPI_Write_Arr(uint8_t* buf,uint16_t len);
  void W5500_SPI_Read(uint8_t* data);
  void W5500_SPI_Read_Arr(uint8_t* buf,uint16_t len);


	typedef uint8_t SOCKET;

	static const uint8_t W5500_SnMR_CLOSE  = 0x00;
	static const uint8_t W5500_SnMR_TCP    = 0x01;
	static const uint8_t W5500_SnMR_UDP    = 0x02;
	static const uint8_t W5500_SnMR_IPRAW  = 0x03;
	static const uint8_t W5500_SnMR_MACRAW = 0x04;
	static const uint8_t W5500_SnMR_PPPOE  = 0x05;
	static const uint8_t W5500_SnMR_ND     = 0x20;
	static const uint8_t W5500_SnMR_MULTI  = 0x80;

	enum W5500_SockCMD {
		Sock_OPEN      = 0x01,
		Sock_LISTEN    = 0x02,
		Sock_CONNECT   = 0x04,
		Sock_DISCON    = 0x08,
		Sock_CLOSE     = 0x10,
		Sock_SEND      = 0x20,
		Sock_SEND_MAC  = 0x21,
		Sock_SEND_KEEP = 0x22,
		Sock_RECV      = 0x40
	};

	static const uint8_t W5500_SnIR_SEND_OK = 0x10;
	static const uint8_t W5500_SnIR_TIMEOUT = 0x08;
	static const uint8_t W5500_SnIR_RECV    = 0x04;
	static const uint8_t W5500_SnIR_DISCON  = 0x02;
	static const uint8_t W5500_SnIR_CON     = 0x01;

	static const uint8_t W5500_SnSR_CLOSED      = 0x00;
	static const uint8_t W5500_SnSR_INIT        = 0x13;
	static const uint8_t W5500_SnSR_LISTEN      = 0x14;
	static const uint8_t W5500_SnSR_SYNSENT     = 0x15;
	static const uint8_t W5500_SnSR_SYNRECV     = 0x16;
	static const uint8_t W5500_SnSR_ESTABLISHED = 0x17;
	static const uint8_t W5500_SnSR_FIN_WAIT    = 0x18;
	static const uint8_t W5500_SnSR_CLOSING     = 0x1A;
	static const uint8_t W5500_SnSR_TIME_WAIT   = 0x1B;
	static const uint8_t W5500_SnSR_CLOSE_WAIT  = 0x1C;
	static const uint8_t W5500_SnSR_LAST_ACK    = 0x1D;
	static const uint8_t W5500_SnSR_UDP         = 0x22;
	static const uint8_t W5500_SnSR_IPRAW       = 0x32;
	static const uint8_t W5500_SnSR_MACRAW      = 0x42;
	static const uint8_t W5500_SnSR_PPPOE       = 0x5F;

	static const uint8_t W5500_IPPROTO_IP   = 0;
	static const uint8_t W5500_IPPROTO_ICMP = 1;
	static const uint8_t W5500_IPPROTO_IGMP = 2;
	static const uint8_t W5500_IPPROTO_GGP  = 3;
	static const uint8_t W5500_IPPROTO_TCP  = 6;
	static const uint8_t W5500_IPPROTO_PUP  = 12;
	static const uint8_t W5500_IPPROTO_UDP  = 17;
	static const uint8_t W5500_IPPROTO_IDP  = 22;
	static const uint8_t W5500_IPPROTO_ND   = 77;
	static const uint8_t W5500_IPPROTO_RAW  = 255;



  void W5500_init(void);
  uint8_t W5500_readVersion(void);

  /**
   * @brief	This function is being used for copy the data form Receive buffer of the chip to application buffer.
   * 
   * It calculate the actual physical address where one has to read
   * the data from Receive buffer. Here also take care of the condition while it exceed
   * the Rx memory uper-bound of socket.
   */
  void W5500_read_data(SOCKET s, volatile uint16_t  src, volatile uint8_t * dst, uint16_t len);
  
  /**
   * @brief	 This function is being called by send() and sendto() function also. 
   * 
   * This function read the Tx write pointer register and after copy the data in buffer update the Tx write pointer
   * register. User should read upper byte first and lower byte later to get proper value.
   */
  void W5500_send_data_processing(SOCKET s, const uint8_t *data, uint16_t len);
  /**
   * @brief A copy of send_data_processing that uses the provided ptr for the
   *        write offset.  Only needed for the "streaming" UDP API, where
   *        a single UDP packet is built up over a number of calls to
   *        send_data_processing_ptr, because TX_WR doesn't seem to get updated
   *        correctly in those scenarios
   * @param ptr value to use in place of TX_WR.  If 0, then the value is read
   *        in from TX_WR
   * @return New value for ptr, to be used in the next call
   */
  // FIXME Update documentation
  void W5500_send_data_processing_offset(SOCKET s, uint16_t data_offset, const uint8_t *data, uint16_t len);

  /**
   * @brief	This function is being called by recv() also.
   * 
   * This function read the Rx read pointer register
   * and after copy the data from receive buffer update the Rx write pointer register.
   * User should read upper byte first and lower byte later to get proper value.
   */
  void W5500_recv_data_processing(SOCKET s, uint8_t *data, uint16_t len, uint8_t peek);

  inline void W5500_setGatewayIp(uint8_t *_addr);
  inline void W5500_getGatewayIp(uint8_t *_addr);

  inline void W5500_setSubnetMask(uint8_t *_addr);
  inline void W5500_getSubnetMask(uint8_t *_addr);

  inline void W5500_setMACAddress(uint8_t * addr);
  inline void W5500_getMACAddress(uint8_t * addr);

  inline void W5500_setIPAddress(uint8_t * addr);
  inline void W5500_getIPAddress(uint8_t * addr);

  inline void W5500_setRetransmissionTime(uint16_t timeout);
  inline void W5500_setRetransmissionCount(uint8_t _retry);

  inline void W5500_swReset(void);

  inline void W5500_setPHYCFGR(uint8_t _val);
  inline uint8_t W5500_getPHYCFGR(void);

  void W5500_execCmdSn(SOCKET s, enum W5500_SockCMD _cmd);
  
  uint16_t W5500_getTXFreeSize(SOCKET s);
  uint16_t W5500_getRXReceivedSize(SOCKET s);

//  // W5500 Registers
//  // ---------------
//private:
  uint8_t  W5500_write(uint16_t _addr, uint8_t _cb, uint8_t _data);
  uint16_t W5500_write_Arr(uint16_t _addr, uint8_t _cb, const uint8_t *buf, uint16_t len);
  uint8_t  W5500_read(uint16_t _addr, uint8_t _cb );
  uint16_t W5500_read_Arr(uint16_t _addr, uint8_t _cb, uint8_t *buf, uint16_t len);
  
#define __W5500_GP_REGISTER8(name, address)             \
  static inline void W5500_write##name(uint8_t _data) { \
    W5500_write(address, 0x04, _data);                  \
  }                                               \
  static inline uint8_t W5500_read##name() {            \
    return W5500_read(address, 0x00);                   \
  }
#define __W5500_GP_REGISTER16(name, address)            \
  static void W5500_write##name(uint16_t _data) {       \
    W5500_write(address,  0x04, _data >> 8);            \
    W5500_write(address+1, 0x04, _data & 0xFF);         \
  }                                               \
  static uint16_t W5500_read##name() {                  \
    uint16_t res = W5500_read(address, 0x00);           \
    res = (res << 8) + W5500_read(address + 1, 0x00);   \
    return res;                                   \
  }
#define __W5500_GP_REGISTER_N(name, address, size)      \
  static uint16_t W5500_write##name(uint8_t *_buff) {   \
    return W5500_write_Arr(address, 0x04, _buff, size);     \
  }                                               \
  static uint16_t W5500_read##name(uint8_t *_buff) {    \
    return W5500_read_Arr(address, 0x00, _buff, size);      \
  }

  __W5500_GP_REGISTER8 (MR,     0x0000);    // Mode
  __W5500_GP_REGISTER_N(GAR,    0x0001, 4); // Gateway IP address
  __W5500_GP_REGISTER_N(SUBR,   0x0005, 4); // Subnet mask address
  __W5500_GP_REGISTER_N(SHAR,   0x0009, 6); // Source MAC address
  __W5500_GP_REGISTER_N(SIPR,   0x000F, 4); // Source IP address
  __W5500_GP_REGISTER8 (IR,     0x0015);    // Interrupt
  __W5500_GP_REGISTER8 (IMR,    0x0016);    // Interrupt Mask
  __W5500_GP_REGISTER16(RTR,    0x0019);    // Timeout address
  __W5500_GP_REGISTER8 (RCR,    0x001B);    // Retry count
  __W5500_GP_REGISTER_N(UIPR,   0x0028, 4); // Unreachable IP address in UDP mode
  __W5500_GP_REGISTER16(UPORT,  0x002C);    // Unreachable Port address in UDP mode
  __W5500_GP_REGISTER8 (PHYCFGR,     0x002E);    // PHY Configuration register, default value: 0b 1011 1xxx

  
#undef __W5500_GP_REGISTER8
#undef __W5500_GP_REGISTER16
#undef __W5500_GP_REGISTER_N

  // W5500 Socket registers
  // ----------------------
  inline uint8_t W5500_readSn(SOCKET _s, uint16_t _addr);
  inline uint8_t W5500_writeSn(SOCKET _s, uint16_t _addr, uint8_t _data);
  inline uint16_t W5500_readSn_Arr(SOCKET _s, uint16_t _addr, uint8_t *_buf, uint16_t len);
  inline uint16_t W5500_writeSn_Arr(SOCKET _s, uint16_t _addr, uint8_t *_buf, uint16_t len);

  //static const uint16_t CH_BASE = 0x0000;
  //static const uint16_t CH_SIZE = 0x0000;

#define __W5500_SOCKET_REGISTER8(name, address)                    \
  static inline void W5500_write##name(SOCKET _s, uint8_t _data) { \
    W5500_writeSn(_s, address, _data);                             \
  }                                                          \
  static inline uint8_t W5500_read##name(SOCKET _s) {              \
    return W5500_readSn(_s, address);                              \
  }
#if defined(REL_GR_KURUMI) || defined(REL_GR_KURUMI_PROTOTYPE)  
#define __W5500_SOCKET_REGISTER16(name, address)                   \
  static void W5500_write##name(SOCKET _s, uint16_t _data) {       \
    W5500_writeSn(_s, address,   _data >> 8);                      \
    W5500_writeSn(_s, address+1, _data & 0xFF);                    \
  }                                                          \
  static uint16_t W5500_read##name(SOCKET _s) {                    \
    uint16_t res = W5500_readSn(_s, address);                      \
    uint16_t res2 = W5500_readSn(_s,address + 1);                  \
    res = res << 8;                                          \
    res2 = res2 & 0xFF;                                      \
    res = res | res2;                                        \
    return res;                                              \
  }
#else
#define __W5500_SOCKET_REGISTER16(name, address)                   \
  static void W5500_write##name(SOCKET _s, uint16_t _data) {       \
    W5500_writeSn(_s, address,   _data >> 8);                      \
    W5500_writeSn(_s, address+1, _data & 0xFF);                    \
  }                                                          \
  static uint16_t W5500_read##name(SOCKET _s) {                    \
    uint16_t res = W5500_readSn(_s, address);                      \
    res = (res << 8) + W5500_readSn(_s, address + 1);              \
    return res;                                              \
  }
#endif  
#define __W5500_SOCKET_REGISTER_N(name, address, size)             \
  static uint16_t W5500_write##name(SOCKET _s, uint8_t *_buff) {   \
    return W5500_writeSn_Arr(_s, address, _buff, size);                \
  }                                                          \
  static uint16_t W5500_read##name(SOCKET _s, uint8_t *_buff) {    \
    return W5500_readSn_Arr(_s, address, _buff, size);                 \
  }
  
  __W5500_SOCKET_REGISTER8(SnMR,        0x0000)        // Mode
  __W5500_SOCKET_REGISTER8(SnCR,        0x0001)        // Command
  __W5500_SOCKET_REGISTER8(SnIR,        0x0002)        // Interrupt
  __W5500_SOCKET_REGISTER8(SnSR,        0x0003)        // Status
  __W5500_SOCKET_REGISTER16(SnPORT,     0x0004)        // Source Port
  __W5500_SOCKET_REGISTER_N(SnDHAR,     0x0006, 6)     // Destination Hardw Addr
  __W5500_SOCKET_REGISTER_N(SnDIPR,     0x000C, 4)     // Destination IP Addr
  __W5500_SOCKET_REGISTER16(SnDPORT,    0x0010)        // Destination Port
  __W5500_SOCKET_REGISTER16(SnMSSR,     0x0012)        // Max Segment Size
  __W5500_SOCKET_REGISTER8(SnPROTO,     0x0014)        // Protocol in IP RAW Mode
  __W5500_SOCKET_REGISTER8(SnTOS,       0x0015)        // IP TOS
  __W5500_SOCKET_REGISTER8(SnTTL,       0x0016)        // IP TTL
  __W5500_SOCKET_REGISTER16(SnTX_FSR,   0x0020)        // TX Free Size
  __W5500_SOCKET_REGISTER16(SnTX_RD,    0x0022)        // TX Read Pointer
  __W5500_SOCKET_REGISTER16(SnTX_WR,    0x0024)        // TX Write Pointer
  __W5500_SOCKET_REGISTER16(SnRX_RSR,   0x0026)        // RX Free Size
  __W5500_SOCKET_REGISTER16(SnRX_RD,    0x0028)        // RX Read Pointer
  __W5500_SOCKET_REGISTER16(SnRX_WR,    0x002A)        // RX Write Pointer (supported?)
  
#undef __SOCKET_REGISTER8
#undef __SOCKET_REGISTER16
#undef __SOCKET_REGISTER_N

  static const uint8_t  W5500_RST = 7; // Reset BIT
  static const int W5500_SOCKETS = 8;
  static const uint16_t W5500_SSIZE = 2048; // Max Tx buffer size
  static const uint16_t W5500_RSIZE = 2048; // Max Rx buffer size
#endif
