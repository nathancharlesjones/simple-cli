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
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define MAX_CMD_LEN 64
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
bool cmd_received = false;
uint8_t cmd[MAX_CMD_LEN] = {0};
uint8_t * last_char = &cmd[MAX_CMD_LEN];
uint8_t * p_current_char = cmd;
bool led_is_blinking = true;
uint32_t prev_millis = 0;
uint32_t on_time = 500, off_time = 500;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  // Echo the character that was received
  //
  HAL_UART_Transmit(&huart2, p_current_char, (size_t)1, HAL_MAX_DELAY);
  
  // As long as the character received isn't a newline/carriage return AND we haven't
  // reached the end of our cmd buffer, then restart the interrupt to receive
  // another character.
  //
  if( *p_current_char != '\n' && *p_current_char != '\r' && p_current_char != last_char )
  {
    HAL_UART_Receive_IT(huart, ++p_current_char, (size_t)1);
  }

  // Otherwise print out a newline (my computer only sends a carriage 
  // return when the <Enter> key is pressed) and zero out the newline or
  // carriage return that was most recently received. Then set <cmd_received> to
  // <true> so that the main loop can process it.
  //
  else
  {
    uint8_t c = '\n';
    HAL_UART_Transmit(&huart2, &c, (size_t)1, HAL_MAX_DELAY);
    *p_current_char = '\0';
    cmd_received = true;
  }
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
  /* USER CODE BEGIN 2 */

  // Start interrupt-based reception of first character over UART
  //
  HAL_UART_Receive_IT(&huart2, cmd, (size_t)1);
  
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    if( cmd_received )
    {
      // Parse the command. Start by looking for the space that separates
      // "dc" and "freq" from their (optional) values.
      //
      uint8_t * args = NULL;
      uint8_t * index_of_first_space = (uint8_t*)index((char*)cmd, ' ');

      // If a space is found...
      //
      if( index_of_first_space != NULL )
      {
        // ...convert space to NUL and set <args> equal 
        // to the rest of the command (i.e. <val>).
        //
        *index_of_first_space = '\0';
        args = index_of_first_space + 1;
      }

      // Process <cmd> and <args>
      //
      if( strcmp((char*)cmd, "on") == 0 ) led_is_blinking = true;
      else if( strcmp((char*)cmd, "off") == 0 )
      {
        led_is_blinking = false;
        HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
      }
      else if( strcmp((char*)cmd, "dc") == 0 )
      {
        // If <args> is empty, then print out the current value for duty cycle
        //
        if( args == NULL )
        {
          uint8_t buffer[9] = {0};
          float dc = on_time*100.0/(on_time + off_time);

          // Hacky version of printing a floating-point value. Print out the 
          // integer portion first, then subtract the integer portion from
          // the float value to get the fractional portion. Multiply by 10
          // and convert to int to get the first decimal place.
          //
          sprintf((char*)buffer, "%2d.%1d%%\n\r", (int)dc, (int)( ( dc - (int)dc ) * 10 ) );
          
          HAL_UART_Transmit(&huart2, buffer, strlen((char*)buffer), HAL_MAX_DELAY);
        }

        // Otherwise, recompute the duty cycle. <args> has just one value, so we
        // can convert the entire string to a float. (If there were more values, 
        // we would need to further subdivide <args> like we did above, by looking
        // for spaces that separated each value.)
        //
        else
        {
          uint32_t period = on_time + off_time;
          on_time = atof((char*)args) * period / 100.0;
          off_time = period - on_time;
        }
      }
      else if( strcmp((char*)cmd, "freq") == 0 )
      {
        // If <args> is empty, then print out the current value for frequency
        //
        if( args == NULL )
        {
          uint8_t buffer[12] = {0};
          float freq = 1000.0/(on_time + off_time);

          // Hacky version of printing a floating-point value. Print out the 
          // integer portion first, then subtract the integer portion from
          // the float value to get the fractional portion. Multiply by 10
          // and convert to int to get the first decimal place.
          //
          sprintf((char*)buffer, "%3d.%1d Hz\n\r", (int)freq, (int)( ( freq - (int)freq ) * 10 ) );
          
          HAL_UART_Transmit(&huart2, buffer, strlen((char*)buffer), HAL_MAX_DELAY);
        }
        
        // Otherwise, recompute the frequency. <args> has just one value, so we
        // can convert the entire string to a float. (If there were more values, 
        // we would need to further subdivide <args> like we did above, by looking
        // for spaces that separated each value.)
        //
        else
        {
          float freq = 1000.0 / ( on_time + off_time );
          
          on_time = on_time * freq / atof((char*)args);
          if( on_time < 1 ) on_time = 1;
          
          off_time = off_time * freq / atof((char*)args);
          if( off_time < 1 ) off_time = 1;
        }
      }
      else
      {
        uint8_t error_msg[] = "Unknown command\n\r";
        HAL_UART_Transmit(&huart2, error_msg, strlen((char*)error_msg), HAL_MAX_DELAY);
      }

      // Reset variables to receive another command.
      //
      memset(cmd, 0, strlen((char*)cmd));
      p_current_char = cmd;
      cmd_received = false;
      HAL_UART_Receive_IT(&huart2, cmd, (size_t)1);
    }

    // Non-blocking Blinky. If the "Blinky" task is enabled
    // then check if the LED is currently on or off.
    //
    if( led_is_blinking )
    {
      // If the LED is on, and has been on for at least <on_time> 
      // milliseconds, then turn the LED off.
      //
      if( HAL_GPIO_ReadPin(LED_GPIO_Port, LED_Pin) == GPIO_PIN_SET )
      {
        if( HAL_GetTick() - prev_millis > on_time )
        {
          prev_millis = HAL_GetTick();
          HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
        }
      }
      
      // Otherwise check if it has been off for at least <off_time> 
      // milliseconds and, if it has, turn the LED on.
      //
      else
      {
        if( HAL_GetTick() - prev_millis > off_time )
        {
          prev_millis = HAL_GetTick();
          HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
        }
      }
    }
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
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

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
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : LED_Pin */
  GPIO_InitStruct.Pin = LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

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
