/*
 * display.c
 *
 *  Created on: Feb 17, 2021
 *      Author: Marco
 */

#include <stdbool.h>
#include <stdio.h>
#include "cmsis_os2.h"
#include "ssd1306.h"
#include "display.h"
#include "status.pb.h"
#include "main.h"

extern osMessageQueueId_t status_message_queueHandle;

void display__main(void) {
	Status status;
	char str_buffer[32];

	ssd1306_Init();
	printf("Info: SSD1306 display initialized\n");

	while (true) {
		if (osMessageQueueGet(status_message_queueHandle, &status, NULL, osWaitForever) != osOK) {
			printf("Fatal: Failed to get a message from the queue\n");
			Error_Handler();
		}

        ssd1306_Fill(Black);

		sprintf(str_buffer, "CPU: %lu%%", status.cpu);
		ssd1306_SetCursor(0, 0);
		ssd1306_WriteString(str_buffer, Font_7x10, White);

		sprintf(str_buffer, "Power: %luW", status.cpu_power);
		ssd1306_SetCursor(0, 10);
		ssd1306_WriteString(str_buffer, Font_7x10, White);

		sprintf(str_buffer, "Temp: %luoC", status.temperature);
		ssd1306_SetCursor(0, 20);
		ssd1306_WriteString(str_buffer, Font_7x10, White);

		sprintf(str_buffer, "Network: %luMiB/s", status.network);
		ssd1306_SetCursor(0, 30);
		ssd1306_WriteString(str_buffer, Font_7x10, White);

		sprintf(str_buffer, "Users: %lu", status.samba_users_connected);
		ssd1306_SetCursor(0, 40);
		ssd1306_WriteString(str_buffer, Font_7x10, White);

		sprintf(str_buffer, "Files: %lu", status.samba_files_opened);
		ssd1306_SetCursor(0, 50);
		ssd1306_WriteString(str_buffer, Font_7x10, White);

        ssd1306_UpdateScreen();
	}
}
