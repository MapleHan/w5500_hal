/**
  ******************************************************************************
  * File Name          : USART.c
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

/* Includes ------------------------------------------------------------------*/
#include "usart.h"

/* USER CODE BEGIN 0 */
UART_BufferTypeDef uart1_t;
//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数
struct __FILE 
{
	int handle; 

}; 

FILE __stdout;
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
}
void _ttywrch(int ch)
{
	ch = ch;
}
//重定义fputc函数 
int fputc(int ch, FILE *f)
{
	int l = UART_Write_IT(&uart1_t,(uint8_t)ch);
	return(l);
}
#endif 
/* USER CODE END 0 */

UART_HandleTypeDef huart1;

/* USART1 init function */

void MX_USART1_UART_Init(void)
{

  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspInit 0 */

  /* USER CODE END USART1_MspInit 0 */
    /* USART1 clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();
  
    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART1 GPIO Configuration    
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART1 interrupt Init */
    HAL_NVIC_SetPriority(USART1_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspInit 1 */

  /* USER CODE END USART1_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspDeInit 0 */

  /* USER CODE END USART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();
  
    /**USART1 GPIO Configuration    
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX 
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10);

    /* USART1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspDeInit 1 */

  /* USER CODE END USART1_MspDeInit 1 */
  }
} 

/* USER CODE BEGIN 1 */
void UART_Begin_IT(UART_HandleTypeDef* huart)
{
	if(huart==(&huart1))
	{
		if(uart1_t.init==1)
		{
			return;
		}
		uart1_t.buff_size = 1024;
		uart1_t.tx_buff = (uint8_t*)malloc(uart1_t.buff_size*sizeof(uint8_t));
		uart1_t.rx_buff = (uint8_t*)malloc(uart1_t.buff_size*sizeof(uint8_t));
		uart1_t.init=1;
		if (UART_RX_Active(&huart1))
		{
			return;
		}
		MX_USART1_UART_Init();
		/* Must disable interrupt to prevent handle lock contention */
		HAL_NVIC_DisableIRQ(USART1_IRQn);
		HAL_UART_Receive_IT(&huart1, &(uart1_t.recv), 1);
		/* USART1 interrupt Init */
		HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(USART1_IRQn);
	}
}

void UART_End_IT(UART_HandleTypeDef *huart)
{
	if(huart==(&huart1))
	{
		if(uart1_t.init!=1)
		{
			return;
		}
		UART_Flush(&uart1_t);
		HAL_UART_DeInit(&huart1);
		free(uart1_t.tx_buff);
		free(uart1_t.rx_buff);
		uart1_t.init=0;
		uart1_t.buff_size = 0;
		uart1_t.rx_head = uart1_t.rx_tail = 0;
	}
}
/**
  * @brief  write the data on the uart
  * @param  obj : pointer to UART_BufferTypeDef structure
  * @param  data : byte to write
  * @param  size : number of data to write
  * @retval The number of bytes written
  */
size_t UART_Write_Block(UART_HandleTypeDef *huart, uint8_t* data, uint16_t size)
{
  if (HAL_UART_Transmit(huart, data, size, TX_TIMEOUT) == HAL_OK) {
    return size;
  } else {
    return 0;
  }
}
/**
  * @brief  Read receive byte from uart
  * @param  obj : pointer to UART_BufferTypeDef structure
  * @retval last character received
  */
size_t UART_Read_Block(UART_HandleTypeDef *huart, uint8_t* data, uint16_t size)
{
  if (HAL_UART_Receive(huart, data, size, TX_TIMEOUT) == HAL_OK) {
    return size;
  } else {
    return 0;
  }
}

int UART_Peek_IT(UART_BufferTypeDef *obj)
{
	if (obj->rx_head == obj->rx_tail) {
    return -1;
  } else {
    return obj->rx_buff[obj->rx_tail];
  }
}
int UART_Read_IT(UART_BufferTypeDef *obj)
{
	if (obj->rx_head == obj->rx_tail) {
    return -1;
  } else {
    unsigned char c = obj->rx_buff[obj->rx_tail];
    obj->rx_tail = (uint16_t)(obj->rx_tail + 1) % obj->buff_size;
    return c;
  }
}
size_t UART_Write_IT(UART_BufferTypeDef* obj,uint8_t c)
{
	UART_HandleTypeDef* huart;
	if(obj==&uart1_t)
	{
		huart=&huart1;
	}
  uint16_t i = (obj->tx_head + 1) % obj->buff_size;
  while (i == obj->tx_tail) {
    // nop, the interrupt handler will free up space for us
  }

  obj->tx_buff[obj->tx_head] = c;
	obj->tx_head = i;
  if (!UART_TX_Active(huart))
	{
    UART_Transmit_IT_Init(huart,obj);
  }
  return 1;
}

/**
 * Attempts to determine if the serial peripheral is already in use for RX
 *
 * @param obj The serial object
 * @return Non-zero if the RX transaction is ongoing, 0 otherwise
 */
uint8_t UART_RX_Active(UART_HandleTypeDef *huart)
{
  return ((HAL_UART_GetState(huart) & HAL_UART_STATE_BUSY_RX) == HAL_UART_STATE_BUSY_RX);
}

/**
 * Attempts to determine if the serial peripheral is already in use for TX
 *
 * @param obj The serial object
 * @return Non-zero if the TX transaction is ongoing, 0 otherwise
 */
uint8_t UART_TX_Active(UART_HandleTypeDef *huart)
{
  return ((HAL_UART_GetState(huart) & HAL_UART_STATE_BUSY_TX) == HAL_UART_STATE_BUSY_TX);
}

/**
  * @brief  Read receive byte from uart
  * @param  obj : pointer to UART_BufferTypeDef structure
  * @retval last character received
  */
int UART_Rx_Complate(UART_HandleTypeDef *huart,UART_BufferTypeDef *obj)
{
  if (huart == NULL) {
    return -1;
  }

  if (UART_RX_Active(huart)) {
    return -1; /* Transaction ongoing */
  }

  unsigned char c = (unsigned char)(obj->recv);
	uint16_t i = (unsigned int)(obj->rx_head + 1) % uart1_t.buff_size;
	if (i != uart1_t.rx_tail)
	{
		obj->rx_buff[obj->rx_head] = c;
		obj->rx_head = i;
	}
  /* Restart RX irq */
  HAL_UART_Receive_IT(huart, &(obj->recv), 1);

  return 0;
}

int UART_TX_Complate(UART_HandleTypeDef *huart,UART_BufferTypeDef *obj)
{
	obj->tx_tail = (obj->tx_tail + 1) % obj->buff_size;
  if (obj->tx_head == obj->tx_tail)
	{
    return -1;
  }
	if (HAL_UART_Transmit_IT(huart, &obj->tx_buff[obj->tx_tail], 1) != HAL_OK)
	{
		return -1;
	}
	return 0;
}
/**
  * @brief  Rx Transfer completed callback
  * @param  UartHandle pointer on the uart reference
  * @retval None
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart==(&huart1))
	{
		UART_Rx_Complate(huart,&uart1_t);
	}
}

/**
  * @brief  Tx Transfer completed callback
  * @param  UartHandle pointer on the uart reference
  * @retval None
  */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart==(&huart1))
	{
		UART_TX_Complate(huart,&uart1_t);
	}
}

int UART_Available(UART_BufferTypeDef *obj)
{
	return ((unsigned int)(obj->buff_size + obj->rx_head - obj->rx_tail)) % obj->buff_size;
}

int UART_Available_For_Write(UART_BufferTypeDef *obj)
{
	uint16_t head = obj->tx_head;
  uint16_t tail = obj->tx_tail;

  if (head >= tail) {
    return obj->buff_size - 1 - head + tail;
  }
  return tail - head - 1;
}
void UART_Flush(UART_BufferTypeDef *obj)
{
	while ((obj->tx_head != obj->tx_tail)) {
    // nop, the interrupt handler will free up space for us
  }
}
void UART_Transmit_IT_Init(UART_HandleTypeDef *huart,UART_BufferTypeDef *obj)
{
	if (huart == NULL) {
    return;
  }
	if(huart->Instance==USART1)
  {
		HAL_NVIC_DisableIRQ(USART1_IRQn);
		HAL_UART_Transmit_IT(huart, &obj->tx_buff[obj->tx_tail], 1);
    HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
  }
  else if(huart->Instance==USART2)
  {
		HAL_NVIC_DisableIRQ(USART2_IRQn);
		HAL_UART_Transmit_IT(huart, &obj->tx_buff[obj->tx_tail], 1);
    HAL_NVIC_SetPriority(USART2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART2_IRQn);
  }
  else if(huart->Instance==USART3)
  {
		HAL_NVIC_DisableIRQ(USART3_IRQn);
		HAL_UART_Transmit_IT(huart, &obj->tx_buff[obj->tx_tail], 1);
    HAL_NVIC_SetPriority(USART3_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART3_IRQn);
  }
}


/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
