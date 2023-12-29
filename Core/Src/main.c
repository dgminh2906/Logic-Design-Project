/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "global.h"
#include "button.h"
#include "scheduler.h"
#include "display7SEG.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void blinkydot(){
	//Thiết lập cho đèn chớp tắt
	HAL_GPIO_TogglePin(DOT_GPIO_Port, DOT_Pin);
}

void update_time(){
	sec+=1;
	if (sec >= 60){
		sec = 0;
		min += 1;
	}
	if (min >= 60){
		min = 0;
		hour += 1;
	}
	if (hour >= 24){
		hour = 0;
		day += 1;
		if (day > 31){
			day = 1;
			month += 1;
			if (month > 12) {
				month = 1;
				year += 1;
			}
		}
		else if ((month == 4 || month == 6 || month == 9 || month == 11) && day > 30) {
			day = 1;
			month += 1;
		}
		else if (month == 2) {
			// Check for leap year
			if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
				if (day > 29) {
					day = 1;
					month += 1;
				}
			}
			else {
				if (day > 28) {
					day = 1;
					month += 1;
				}
			}
		}
	}
}

void alarm(){
	//So sánh điều kiện để báo thức xảy ra
	if (hour == hour_alarm && min == min_alarm && sec == sec_alarm){
		HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, GPIO_PIN_RESET);
	}
	//Thiết lập nút nhấn để tắt báo thức
	if (isButtonPressed(2) == 1){
		HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, GPIO_PIN_SET);
	}
}

void fsm_run(){
	switch (status){
		case INIT:
			status = MODE1;
			break;
		//MODE1
		case MODE1:
			//Thiết lập đèn hiển thị đang ở MODE1
			HAL_GPIO_WritePin(MODE1_GPIO_Port, MODE1_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(MODE5_GPIO_Port, MODE5_Pin, GPIO_PIN_SET);
			//Cập nhật để LED hiển thị giờ ,phút, giây
			updateLedBufferMode1();
			if (isButtonPressed(0)) status = MODE2;
			if (isButtonPressed(1)) status = MODE5s;
			break;
		case MODE5s:
			//Cập nhật để LED hiển thị ngày, tháng, năm
			updateLedBufferMode2();
			if (isButtonPressed(0)) status = MODE2;
			dmy5stime++; // Tăng biến đếm thời gian hiển thị MODE5s
			//Nếu hiển thị đủ 5s, chuyển trạng thái về MODE1
			if (dmy5stime >= DMY_DISPLAY_TIME){
				dmy5stime = 0;
				status = MODE1;
			}
			break;
		//MODE2
		case MODE2:
			//Thiết lập đèn hiển thị đang ở MODE2
			HAL_GPIO_WritePin(MODE1_GPIO_Port, MODE1_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(MODE2_GPIO_Port, MODE2_Pin, GPIO_PIN_RESET);
			updateLedBufferMode2();
			if (isButtonPressed(0)) status = MODE3;
			break;
		//MODE3
		case MODE3:
			//Thiết lập đèn hiển thị đang ở MODE3
			HAL_GPIO_WritePin(MODE2_GPIO_Port, MODE2_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(MODE3_GPIO_Port, MODE3_Pin, GPIO_PIN_RESET);
			//Cập nhật để LED hiển thị giờ, phút, giây
			updateLedBufferMode1();
			if (isButtonPressed(0)) status = MODE4;
			if (isButtonPressed(1)) MAX_HOUR = 24; //Chuyển sang hiển thị chế độ giờ 24
			if (isButtonPressed(2)) MAX_HOUR = 12; //Chuyển sang hiển thị chế độ giờ 12
			break;
		//MODE4
		case MODE4:
			//Thiết lập đèn hiển thị đang ở MODE4
			HAL_GPIO_WritePin(MODE3_GPIO_Port, MODE3_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(MODE4_GPIO_Port, MODE4_Pin, GPIO_PIN_RESET);
			//Cập nhật để LED hiển thị giờ, phút, giây của báo thức
			updateLedBufferMode4();
			if (isButtonPressed(0)) status = MODE5;
			if (isButtonPressed(1)) status = INC_HOUR; //Chuyển sang cập nhật giờ báo thức
			break;
		//SET UP ALARM
		case INC_HOUR:
			if (isButtonPressed(1)) status = INC_MIN; //Chuyển sang cập nhật phút báo thức
			//Tăng giờ báo thức lên 1 đơn vị
			if (isButtonPressed(2)){
				hour_alarm += 1;
				if(hour_alarm >= MAX_HOUR) hour_alarm = 0;
				updateLedBufferMode4();
			}
			if (isButtonPressed(0)) status = MODE5;
			break;
		case INC_MIN:
			if (isButtonPressed(1)) status = INC_SEC; //Chuyển sang cập nhật giây báo thức
			//Tăng phút báo thức lên 1 đơn vị
			if (isButtonPressed(2)){
				min_alarm += 1;
				if(min_alarm >= 60) min_alarm = 0;
				updateLedBufferMode4();
			}
			if (isButtonPressed(0)) status = MODE5;
			break;
		case INC_SEC:
			if (isButtonPressed(1)) status = INC_HOUR; //Chuyển sang cập nhật giờ báo thức
			//Tăng giây báo thức lên 1 đơn vị
			if (isButtonPressed(2)){
				sec_alarm += 1;
				if(sec_alarm >= 60) sec_alarm = 0;
				updateLedBufferMode4();
			}
			if (isButtonPressed(0)) status = MODE5;
			break;
		//MODE5
		case MODE5:
			//Thiết lập đèn hiển thị đang ở MODE5
			HAL_GPIO_WritePin(MODE4_GPIO_Port, MODE4_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(MODE5_GPIO_Port, MODE5_Pin, GPIO_PIN_RESET);
			//Cập nhật LED hiển thị giờ, phút, giây
			updateLedBufferMode1();
			if (isButtonPressed(0)) status = MODE1;
			if (isButtonPressed(1)) status = INC_MODE; //Chuyển sang chế độ tăng múi giờ
			break;
		//SET UP TIMEZONE
		case INC_MODE:
			if (isButtonPressed(0)) status = MODE1;
			//Tăng múi giờ lên 1 đơn vị
			if (isButtonPressed(1)){
				hour += 1;
				if (hour >= MAX_HOUR) hour = 0;
				updateLedBufferMode1();
			}
			//Giảm múi giờ đi 1 đơn vị, chuyển sang chế độ giảm múi giờ
			if (isButtonPressed(2)){
				status = DEC_MODE;
				hour -= 1;
				if (hour < 0) hour = MAX_HOUR - 1;
				updateLedBufferMode1();
			}
			break;
		case DEC_MODE:
			if (isButtonPressed(0)) status = MODE1;
			//Tăng múi giờ lên 1 đơn vị, chuyển sang chế độ tăng múi giờ
			if (isButtonPressed(1)){
				status = INC_MODE;
				hour += 1;
				if (hour >= MAX_HOUR) hour = 0;
				updateLedBufferMode1();
			}
			//Giảm múi giờ đi 1 đơn vị
			if (isButtonPressed(2)){
				hour -= 1;
				if (hour < 0) hour = MAX_HOUR - 1;
				updateLedBufferMode1();
			}
			break;
		default:
			break;
	}
}

void scanLED(){
	//Hiển thị led
	update7SEG(index_led);
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
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start_IT (& htim2);
  SCH_Init();
  //Thêm task vào scheduler
  SCH_Add_Task(blinkydot, 10, 500); //Task nhấp nháy dấu 2 chấm
  SCH_Add_Task(update_time, 0, 1200); // Task cập nhật thời gian
  SCH_Add_Task(scanLED, 10, 200); //Task hiển thị LED
  SCH_Add_Task(fsm_run, 0, 10); //Task chính
  SCH_Add_Task(alarm, 10, 1200); //Task báo thức
  //Thiết lập giá trị ban đầu của các LED
  HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(MODE1_GPIO_Port, MODE1_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(MODE2_GPIO_Port, MODE2_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(MODE3_GPIO_Port, MODE3_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(MODE4_GPIO_Port, MODE4_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(MODE5_GPIO_Port, MODE5_Pin, GPIO_PIN_SET);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  SCH_Dispatch_Tasks();
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 7999;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 9;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */
  TIME_CYCLE = 1/(8e6/(htim2.Init.Prescaler + 1)/(htim2.Init.Period + 1)) * 1000; // Time cycle
  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, DOT_Pin|LED_RED_Pin|EN0_Pin|EN1_Pin
                          |EN2_Pin|EN3_Pin|EN4_Pin|EN5_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, SEG0_Pin|SEG1_Pin|SEG2_Pin|MODE4_Pin
                          |MODE5_Pin|SEG3_Pin|SEG4_Pin|SEG5_Pin
                          |SEG6_Pin|MODE1_Pin|MODE2_Pin|MODE3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : DOT_Pin LED_RED_Pin EN0_Pin EN1_Pin
                           EN2_Pin EN3_Pin EN4_Pin EN5_Pin */
  GPIO_InitStruct.Pin = DOT_Pin|LED_RED_Pin|EN0_Pin|EN1_Pin
                          |EN2_Pin|EN3_Pin|EN4_Pin|EN5_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : SEG0_Pin SEG1_Pin SEG2_Pin MODE4_Pin
                           MODE5_Pin SEG3_Pin SEG4_Pin SEG5_Pin
                           SEG6_Pin MODE1_Pin MODE2_Pin MODE3_Pin */
  GPIO_InitStruct.Pin = SEG0_Pin|SEG1_Pin|SEG2_Pin|MODE4_Pin
                          |MODE5_Pin|SEG3_Pin|SEG4_Pin|SEG5_Pin
                          |SEG6_Pin|MODE1_Pin|MODE2_Pin|MODE3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : BUTTON1_Pin BUTTON2_Pin BUTTON3_Pin */
  GPIO_InitStruct.Pin = BUTTON1_Pin|BUTTON2_Pin|BUTTON3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef * htim){
	SCH_Update();
	getKeyInput();
}
/* USER CODE END 4 */

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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
