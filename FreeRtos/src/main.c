/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "dynamixel.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/*#define DXL_ID                1
#define ADDR_TORQUE_ENABLE   64
#define ADDR_GOAL_VELOCITY   104
#define ADDR_GOAL_POSITION   116
#define TORQUE_ENABLE        1
#define BAUDRATE             1000000
*/
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;

/* Definitions for MotorD */
osThreadId_t MotorDHandle;
const osThreadAttr_t MotorD_attributes = {
  .name = "MotorD",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for MotorG */
osThreadId_t MotorGHandle;
const osThreadAttr_t MotorG_attributes = {
  .name = "MotorG",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal1,
};
/* Definitions for MotorTimer */
osTimerId_t MotorTimerHandle;
const osTimerAttr_t MotorTimer_attributes = {
  .name = "MotorTimer"
};
/* USER CODE BEGIN PV */
char msg[64];
int len;
volatile uint8_t rx[64];
uint8_t rx_buf[32];
HAL_StatusTypeDef ret;
HAL_StatusTypeDef rt;

//Valeurs de position
uint16_t CENTER = 2048;
uint16_t FORWARD = 3072;
uint16_t BACKWARD = 1024;
uint16_t TURN = 256;
uint8_t flag=0;
HAL_StatusTypeDef st;
int32_t current_velocity1 = 0;
int32_t current_velocity2 = 0;
int32_t pos1=0;
int32_t pos2=0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_USART1_UART_Init(void);
void MotorDTask(void *argument);
void MotorGTask(void *argument);
void MotorTimCallback(void *argument);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int __io_putchar(int ch)
{
  HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
  return ch;
}
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
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  HAL_UARTEx_ReceiveToIdle_IT(&huart1, rx, sizeof(rx));
  Dxl_Init(&huart1);
  HAL_StatusTypeDef status;
  // Init Motors
  for (uint8_t id = 1; id <= 2; id++)
  {
    printf("Init moteur ID %u...\r\n", id);
    Dxl_Ping(id);
    HAL_Delay(10);

    // Change position Mode
    Dxl_SetOperatingMode(id, 3);
    HAL_Delay(10);

    // Define speed
    Dxl_SetProfileVelocity(id, 10);
    HAL_Delay(10);

  }

  HAL_Delay(2000);

  // conversion rpm
  //float rpm = current_velocity * 0.229f;

//<Test Motor VEL>
/*

  printf("Moteurs prêts !\r\n");
  Dxl_MoveVel(1,150);
  HAL_Delay(100);
  Dxl_MoveVel(2, -70);
  HAL_Delay(100);
  Dxl_ReadVel(1, &current_velocity1);
  Dxl_ReadVel(2, &current_velocity2);
  HAL_Delay(3000);


       //Reception UART
    len= snprintf(msg,sizeof(msg), "Id1 Vit : %ld, Id2 Vit : %ld\r\n",current_velocity1, current_velocity2);
      //Envoie des données de vitesse
     HAL_UART_Transmit(&huart2, (uint8_t *)msg, len, HAL_MAX_DELAY);
     Dxl_MoveVel(1,0);
	 HAL_Delay(100);
	 Dxl_MoveVel(2, 0);
	 Dxl_ReadVel(1, &current_velocity1);
	 Dxl_ReadVel(2, &current_velocity2);
*/

  //<Test Motor Pos>
  Dxl_MovePos(1, FORWARD);
  HAL_Delay(100);
  Dxl_MovePos(2, FORWARD);
  HAL_Delay(1500);
  Dxl_ReadPresentPosition(1, &pos1);
  Dxl_ReadPresentPosition(2, &pos2);
  Dxl_MovePos(1, BACKWARD);
    HAL_Delay(100);
    Dxl_MovePos(2, BACKWARD);
    HAL_Delay(1500);
    Dxl_ReadPresentPosition(1, &pos1);
    Dxl_ReadPresentPosition(2, &pos2);

  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* Create the timer(s) */
  /* creation of MotorTimer */
  MotorTimerHandle = osTimerNew(MotorTimCallback, osTimerPeriodic, NULL, &MotorTimer_attributes);

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of MotorD */
  MotorDHandle = osThreadNew(MotorDTask, NULL, &MotorD_attributes);

  /* creation of MotorG */
  MotorGHandle = osThreadNew(MotorGTask, NULL, &MotorG_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 57600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_HalfDuplex_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 57600;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_HalfDuplex_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size){
	//printf("bon");
	HAL_UARTEx_ReceiveToIdle_IT(&huart1, rx, sizeof(rx));
}

/* USER CODE END 4 */

/* USER CODE BEGIN Header_MotorDTask */
/**
  * @brief  Function implementing the MotorD thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_MotorDTask */
void MotorDTask(void *argument)
{
  /* USER CODE BEGIN 5 */
	osTimerStart(MotorTimerHandle, 1000);
  /* Infinite loop */
  for(;;)
  {
	 /* printf("Avancer\r\n");
	  Dxl_Move(2, FORWARD);
	  	  osDelay(2000);
	  	  Dxl_Move(2, BACKWARD);
	  	osDelay(2000);
	  if(flag==1){
		  HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
	  flag=0;
	  }

    osDelay(100);*/
  }
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_MotorGTask */
/**
* @brief Function implementing the MotorG thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_MotorGTask */
void MotorGTask(void *argument)
{
  /* USER CODE BEGIN MotorGTask */
  /* Infinite loop */
  for(;;)
  {
	/* Dxl_Move(1, BACKWARD);
	 		      osDelay(2000);
	 		      Dxl_Move(1, FORWARD);
	 		     osDelay(1000);*/
  }
  /* USER CODE END MotorGTask */
}

/* MotorTimCallback function */
void MotorTimCallback(void *argument)
{
  /* USER CODE BEGIN MotorTimCallback */
flag=1;
  /* USER CODE END MotorTimCallback */
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM6) {
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
  __disable_irq();
  while (1)
  {
  }
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
