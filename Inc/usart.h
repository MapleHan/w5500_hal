/**
  ******************************************************************************
  * File Name          : USART.h
  * Description        : This file provides code for the configuration
  *                      of the USART instances.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __usart_H
#define __usart_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */
#include <stdlib.h>
#include "stdio.h"
/* USER CODE END Includes */

extern UART_HandleTypeDef huart1;

/* USER CODE BEGIN Private defines */
#define TX_TIMEOUT  1000
typedef struct
{
	uint8_t init;
  uint8_t recv;
  uint8_t* rx_buff;
  uint8_t* tx_buff;
	uint16_t buff_size;
  uint16_t rx_tail;
  uint16_t tx_head;
  volatile uint16_t rx_head;
  volatile uint16_t tx_tail;
}UART_BufferTypeDef;
extern UART_BufferTypeDef uart1_t;
/* USER CODE END Private defines */

void MX_USART1_UART_Init(void);

/* USER CODE BEGIN Prototypes */
void UART_Begin_IT(UART_HandleTypeDef* huart);
void UART_End_IT(UART_HandleTypeDef *huart);
size_t UART_Write_Block(UART_HandleTypeDef *huart, uint8_t* data, uint16_t size);
size_t UART_Read_Block(UART_HandleTypeDef *huart, uint8_t* data, uint16_t size);
int UART_Peek_IT(UART_BufferTypeDef *obj);
int UART_Read_IT(UART_BufferTypeDef *obj);
size_t UART_Write_IT(UART_BufferTypeDef *obj,uint8_t c);
uint8_t UART_RX_Active(UART_HandleTypeDef *huart);
uint8_t UART_TX_Active(UART_HandleTypeDef *huart);
int UART_RX_Complate(UART_HandleTypeDef *huart,UART_BufferTypeDef *obj);
int UART_TX_Complate(UART_HandleTypeDef *huart,UART_BufferTypeDef *obj);
int UART_Available(UART_BufferTypeDef *obj);
int UART_Available_For_Write(UART_BufferTypeDef *obj);
void UART_Flush(UART_BufferTypeDef *obj);
void UART_Transmit_IT_Init(UART_HandleTypeDef *huart,UART_BufferTypeDef *obj);
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ usart_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
