/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "EthernetServer.h"
#include "EthernetClient.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define CLIENT_MAX_COUNT 2
#define CONTROL_CLIENT_INDEX 0
#define DEBUG_CLIENT_INDEX 1
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
ETH_ServerHandleTypeDef control_server = {23};
ETH_ServerHandleTypeDef dbg_server = {24};
ETH_ClientHandleTypeDef clients[CLIENT_MAX_COUNT] = {MAX_SOCK_NUM,MAX_SOCK_NUM};
unsigned long last_heartbeat[2] = {0,0};
unsigned long heartbeat_timeout = 2000;
extern osMutexId SerialSemaphoreHandle;
extern osMutexId ETHSemaphoreHandle;
volatile bool ETH_Lock = true;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */
void CreateMutex(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_SPI2_Init();
  /* USER CODE BEGIN 2 */
	/* Call init function for freertos objects (in freertos.c) */
  UART_Begin_IT(&huart1);
	HAL_GPIO_WritePin(DBG_LED1_GPIO_Port,DBG_LED1_Pin,GPIO_PIN_SET);
  CreateMutex();
	ethInit();
	DbgPrint("Setup Init Finish!\r\n");
  /* USER CODE END 2 */

  /* Call init function for freertos objects (in freertos.c) */
  MX_FREERTOS_Init(); 
  /* Start scheduler */
  osKernelStart();
 
  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	}
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void DbgPrint(char *format, ...)
{
	char  buf_str[200 + 1]; 
	va_list   v_args; 
	va_start(v_args, format); 
	(void)vsnprintf((char       *)&buf_str[0], 
								 (size_t      ) sizeof(buf_str), 
								 (char const *) format, 
																v_args); 
  va_end(v_args); 
	if((SERIAL_DEBUG) && ( xSemaphoreTake( SerialSemaphoreHandle, ( TickType_t ) 5 ) == pdTRUE ))
  {
		printf("%s", buf_str); 
		xSemaphoreGive( SerialSemaphoreHandle );
	}
	if( (ETH_DEBUG)&&((xSemaphoreTake( ETHSemaphoreHandle, ( TickType_t ) 5 ) == pdTRUE)||(!ETH_Lock)) )
	{
		if(ETH_Client_connected(&clients[DEBUG_CLIENT_INDEX]))
		{
			ETH_Client_write_Arr(&clients[DEBUG_CLIENT_INDEX],(uint8_t*)(buf_str),strlen(buf_str));
		}
		xSemaphoreGive( ETHSemaphoreHandle );
	}
}

void ethInit(void)
{
  static uint8_t MAC[] =  {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEE} ;
  static IP_AddressTypeDef IP = {192,168,0,175};
  do
  {
    ETH_begin(MAC, &IP);
    HAL_Delay(20);
  }while( (ETH_localIP().addr[2]!=IP.addr[2])||(ETH_localIP().addr[3]!=IP.addr[3]) );
  // start listening for clients
  ETH_Server_begin(&control_server);
  if(ETH_DEBUG)
    ETH_Server_begin(&dbg_server);
  DbgPrint("ETH Init Finish, in IP:%d,%d\r\n",ETH_localIP().addr[2],ETH_localIP().addr[3]);
}

void ethEvent()
{
  static uint8_t last_con[CLIENT_MAX_COUNT] = {0,0};
	if( xSemaphoreTake( ETHSemaphoreHandle, ( TickType_t ) 10 ) == pdTRUE )
	{
		ETH_Lock = false;
		for(uint8_t i=0;i<CLIENT_MAX_COUNT;++i) 
		{
			uint8_t s = ETH_Client_connected(&clients[i]);
	//		DbgPrint("Client S:%d,%d,%d\r\n",i,s,ETH_Client_status(&clients[i]));
			if(last_con[i]!=s)
			{
				DbgPrint("clients[%d] state:%d+ To %d,%d\r\n",i,last_con[i],s,ETH_Client_status(&clients[i]));
				if((last_con[i]==1)&&(s==0))
				{
					ETH_Client_stop(&clients[i]);
					DbgPrint("clients[%d] stop\r\n",i);
				}
				last_con[i]=s;
			}
		}
		if(ETH_DEBUG)
		{
			ETH_ClientHandleTypeDef client = ETH_Server_available(&dbg_server);
			if(ETH_Client_isOK(&client))
			{
				if(!ETH_Client_isIdentical(&client,&clients[DEBUG_CLIENT_INDEX]))
				{
					if(ETH_Client_connected(&clients[DEBUG_CLIENT_INDEX]))
					{
						ETH_Client_stop(&clients[DEBUG_CLIENT_INDEX]);
						last_con[DEBUG_CLIENT_INDEX] = 0;
						DbgPrint("DEBUG_CLIENT active stop, reconnect to new client\r\n");
					}
					clients[DEBUG_CLIENT_INDEX] = client;
					DbgPrint("We start have a new debug client message!\r\n");
				}
				const uint8_t n = ETH_Client_available(&client);
				uint8_t rece[n];
				ETH_Client_read_Arr(&client,rece,n);
			}
		}
		ETH_ClientHandleTypeDef client = ETH_Server_available(&control_server);
	//	DbgPrint("Server client:%d,%d,%d\r\n",client._sock,ETH_Client_isOK(&client),ETH_Client_isIdentical(&client,&clients[CONTROL_CLIENT_INDEX]));
		if(ETH_Client_isOK(&client))
		{
			if(!ETH_Client_isIdentical(&client,&clients[CONTROL_CLIENT_INDEX]))
			{
				if(ETH_Client_connected(&clients[CONTROL_CLIENT_INDEX]))
				{
					ETH_Client_stop(&clients[CONTROL_CLIENT_INDEX]);
					last_con[CONTROL_CLIENT_INDEX] = 0;
					DbgPrint("CONTROL_CLIENT active stop, reconnect to new client\r\n");
				}
				clients[CONTROL_CLIENT_INDEX] = client;
				DbgPrint("We start have a new control client message!\r\n");
			}
			uint16_t n = ETH_Client_available(&client);
			if(n>0)
			{
				uint8_t rece[n];
				ETH_Client_read_Arr(&client,rece,n);
				ETH_Client_write_Arr(&client,rece,n);
		//		DbgPrint("ETH Rece:");
		//		for (int i=0;i<n;i++)
		//		  DbgPrint("%d ",rece[i]);
		//		DbgPrint("\r\n");
				// uint8_t h[5]={0xee,0xee,0xee,0xee,0xee};
				// ethDbgWrite(h,5);
				// ethDbgWrite(rece,n);
		//    control_protocol::addData(rece,n);
			}
		}
		ETH_Lock = true;
		xSemaphoreGive( ETHSemaphoreHandle );
	}
}


/* USER CODE END 4 */

 /**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM1 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM1) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
