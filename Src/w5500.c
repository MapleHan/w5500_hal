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

//#include <stdio.h>
#include <string.h>

#include "w5500.h"

void W5500_SPI_initSS()
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOB_CLK_ENABLE();
  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(SPI2_CS_GPIO_Port, SPI2_CS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : PBPin PBPin PBPin */
  GPIO_InitStruct.Pin = SPI2_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}
void W5500_SPI_setSS()
{
	HAL_GPIO_WritePin(SPI2_CS_GPIO_Port,SPI2_CS_Pin,GPIO_PIN_RESET);
}
void W5500_SPI_resetSS()
{
	HAL_GPIO_WritePin(SPI2_CS_GPIO_Port,SPI2_CS_Pin,GPIO_PIN_SET);
}
void W5500_SPI_Begin()
{
	MX_SPI2_Init();
}
void W5500_SPI_Write(uint8_t data)
{
	HAL_SPI_Transmit(&hspi2,&data,1,SPI_TRANSFER_TIMEOUT);
}
void W5500_SPI_Write_Arr(uint8_t* buf,uint16_t len)
{
	for (int i=0;i<len;i++)
		HAL_SPI_Transmit(&hspi2,&buf[i],1,SPI_TRANSFER_TIMEOUT);
}
void W5500_SPI_Read(uint8_t* data)
{
	uint8_t d = 0;
	HAL_SPI_TransmitReceive(&hspi2,&d,data,1,SPI_TRANSFER_TIMEOUT);
}
void W5500_SPI_Read_Arr(uint8_t* buf,uint16_t len)
{
	uint8_t d = 0;
	for (int i=0;i<len;i++)
		HAL_SPI_TransmitReceive(&hspi2,&d,&buf[i],1,SPI_TRANSFER_TIMEOUT);
}


uint8_t W5500_write(uint16_t _addr, uint8_t _cb, uint8_t _data)
{
		#ifndef SPI2_ONLY_W5500
			W5500_SPI_Begin();
		#endif
    W5500_SPI_setSS();
		uint8_t d = _addr >> 8;
		W5500_SPI_Write(d);
	  d = _addr & 0xFF;
		W5500_SPI_Write(d);
		W5500_SPI_Write(_cb);
	  W5500_SPI_Write(_data);
    W5500_SPI_resetSS();
    return 1;
}

uint16_t W5500_write_Arr(uint16_t _addr, uint8_t _cb, const uint8_t *_buf, uint16_t _len)
{
		#ifndef SPI2_ONLY_W5500
			W5500_SPI_Begin();
		#endif
    W5500_SPI_setSS();
		uint8_t d = _addr >> 8;
		W5500_SPI_Write(d);
	  d = _addr & 0xFF;
		W5500_SPI_Write(d);
		W5500_SPI_Write(_cb);
	  W5500_SPI_Write_Arr((uint8_t *)(_buf),_len);
    W5500_SPI_resetSS();
    return _len;
}

uint8_t W5500_read(uint16_t _addr, uint8_t _cb)
{
		#ifndef SPI2_ONLY_W5500
			W5500_SPI_Begin();
		#endif
    W5500_SPI_setSS();
		uint8_t d = _addr >> 8;
		W5500_SPI_Write(d);
	  d = _addr & 0xFF;
		W5500_SPI_Write(d);
		W5500_SPI_Write(_cb);
    uint8_t _data;
		W5500_SPI_Read(&_data);
    W5500_SPI_resetSS();
    return _data;
}

uint16_t W5500_read_Arr(uint16_t _addr, uint8_t _cb, uint8_t *_buf, uint16_t _len)
{ 
		#ifndef SPI2_ONLY_W5500
			W5500_SPI_Begin();
		#endif
    W5500_SPI_setSS();
		uint8_t d = _addr >> 8;
		HAL_SPI_Transmit(&hspi2,&d,1,SPI_TRANSFER_TIMEOUT);
	  d = _addr & 0xFF;
		HAL_SPI_Transmit(&hspi2,&d,1,SPI_TRANSFER_TIMEOUT);
		HAL_SPI_Transmit(&hspi2,&_cb,1,SPI_TRANSFER_TIMEOUT);
	  W5500_SPI_Read_Arr(_buf,_len);
    W5500_SPI_resetSS();
    return _len;
}


uint8_t W5500_readVersion(void)
{
		#ifndef SPI2_ONLY_W5500
			W5500_SPI_Begin();
		#endif
    W5500_SPI_setSS();
	  uint8_t d = 0x00;
		HAL_SPI_Transmit(&hspi2,&d,1,SPI_TRANSFER_TIMEOUT);
	  d = 0x39;
		HAL_SPI_Transmit(&hspi2,&d,1,SPI_TRANSFER_TIMEOUT);
	  d = 0x01;
		HAL_SPI_Transmit(&hspi2,&d,1,SPI_TRANSFER_TIMEOUT);
    uint8_t _data;
		d = 0;
		HAL_SPI_TransmitReceive(&hspi2,&d,&_data,1,SPI_TRANSFER_TIMEOUT);
    W5500_SPI_resetSS();
    return _data;
}

void W5500_read_data(SOCKET s, volatile uint16_t src, volatile uint8_t *dst, uint16_t len)
{
    uint8_t cntl_byte = (0x18+(s<<5));
    W5500_read_Arr((uint16_t)src , cntl_byte, (uint8_t *)dst, len);
}

uint8_t W5500_readSn(SOCKET _s, uint16_t _addr) {
    uint8_t cntl_byte = (_s<<5)+0x08;
    return W5500_read(_addr, cntl_byte);
}

uint8_t W5500_writeSn(SOCKET _s, uint16_t _addr, uint8_t _data) {
    uint8_t cntl_byte = (_s<<5)+0x0C;
    return W5500_write(_addr, cntl_byte, _data);
}

uint16_t W5500_readSn_Arr(SOCKET _s, uint16_t _addr, uint8_t *_buf, uint16_t _len) {
    uint8_t cntl_byte = (_s<<5)+0x08;
    return W5500_read_Arr(_addr, cntl_byte, _buf, _len );
}

uint16_t W5500_writeSn_Arr(SOCKET _s, uint16_t _addr, uint8_t *_buf, uint16_t _len) {
    uint8_t cntl_byte = (_s<<5)+0x0C;
    return W5500_write_Arr(_addr, cntl_byte, _buf, _len);
}

void W5500_getGatewayIp(uint8_t *_addr) {
  W5500_readGAR(_addr);
}

void W5500_setGatewayIp(uint8_t *_addr) {
  W5500_writeGAR(_addr);
}

void W5500_getSubnetMask(uint8_t *_addr) {
  W5500_readSUBR(_addr);
}

void W5500_setSubnetMask(uint8_t *_addr) {
  W5500_writeSUBR(_addr);
}

void W5500_getMACAddress(uint8_t *_addr) {
  W5500_readSHAR(_addr);
}

void W5500_setMACAddress(uint8_t *_addr) {
  W5500_writeSHAR(_addr);
}

void W5500_getIPAddress(uint8_t *_addr) {
  W5500_readSIPR(_addr);
}

void W5500_setIPAddress(uint8_t *_addr) {
  W5500_writeSIPR(_addr);
}

void W5500_setRetransmissionTime(uint16_t _timeout) {
  W5500_writeRTR(_timeout);
}

void W5500_setRetransmissionCount(uint8_t _retry) {
  W5500_writeRCR(_retry);
}

void W5500_setPHYCFGR(uint8_t _val) {
  W5500_writePHYCFGR(_val);
}

uint8_t W5500_getPHYCFGR() {
//  readPHYCFGR();
  return W5500_read(0x002E, 0x00);
}

void W5500_swReset() {
  W5500_writeMR( (W5500_readMR() | 0x80) );
}


//// SPI details
//SPISettings wiznet_SPI_settings(8000000, MSBFIRST, SPI_MODE0);
//uint8_t SPI_CS;

void W5500_init()
{
  W5500_SPI_initSS();
  W5500_SPI_Begin();
  W5500_swReset();
  for (int i=0; i<MAX_SOCK_NUM; i++) {
    uint8_t cntl_byte = (0x0C + (i<<5));
    W5500_write( 0x1E, cntl_byte, 2); //0x1E - Sn_RXBUF_SIZE
    W5500_write( 0x1F, cntl_byte, 2); //0x1F - Sn_TXBUF_SIZE
  }
}

uint16_t W5500_getTXFreeSize(SOCKET s)
{
    uint16_t val=0, val1=0;
    do {
        val1 = W5500_readSnTX_FSR(s);
        if (val1 != 0)
            val = W5500_readSnTX_FSR(s);
    } 
    while (val != val1);
    return val;
}

uint16_t W5500_getRXReceivedSize(SOCKET s)
{
    uint16_t val=0,val1=0;
    do {
        val1 = W5500_readSnRX_RSR(s);
        if (val1 != 0)
            val = W5500_readSnRX_RSR(s);
    } 
    while (val != val1);
    return val;
}

void W5500_send_data_processing(SOCKET s, const uint8_t *data, uint16_t len)
{
  // This is same as having no offset in a call to send_data_processing_offset
  W5500_send_data_processing_offset(s, 0, data, len);

}

void W5500_send_data_processing_offset(SOCKET s, uint16_t data_offset, const uint8_t *data, uint16_t len)
{
    uint16_t ptr = W5500_readSnTX_WR(s);
    uint8_t cntl_byte = (0x14+(s<<5));
    ptr += data_offset;
    W5500_write_Arr(ptr, cntl_byte, data, len);
    ptr += len;
    W5500_writeSnTX_WR(s, ptr);
}

void W5500_recv_data_processing(SOCKET s, uint8_t *data, uint16_t len, uint8_t peek)
{
    uint16_t ptr;
    ptr = W5500_readSnRX_RD(s);

    W5500_read_data(s, ptr, data, len);
    if (!peek)
    {
        ptr += len;
        W5500_writeSnRX_RD(s, ptr);
    }
}



void W5500_execCmdSn(SOCKET s, enum W5500_SockCMD _cmd) {
    // Send command to socket
    W5500_writeSnCR(s, _cmd);
    // Wait for command to complete
    while (W5500_readSnCR(s))
    ;
}
