/*
 * stdio.c
 *
 *  Created on: Feb 16, 2021
 *      Author: Marco
 */

#include "stm32f1xx_hal.h"
#include "main.h"

extern UART_HandleTypeDef huart2;

int __io_putchar(int ch)
{
  if (HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, 1000) != HAL_OK)
	Error_Handler();
  return 0;
}
