/*
 * message_parser.c
 *
 *  Created on: Feb 17, 2021
 *      Author: Marco
 */

/*
 Packet format:
 [0x00][Checksum LSB][Checksum MSB][0x??]..[0x??][0x00]

 Checksum is a simple addition of the original data as a uint16 (wrapping around).
 Minimum packet length is (1 [start] + 2 [checksum] + 1 [end]) = 4.
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "cmsis_os2.h"
#include "stm32f1xx_hal.h"
#include "main.h"
#include "status_message_parser.h"
#include "status.pb.h"
#include "pb_decode.h"
#include "cobs.h"

#define BUFFER_SIZE 1024

static bool check_start(void);
static bool check_end(size_t dma_count);
static size_t decode_cobs(size_t dma_count);
static bool check_checksum(size_t cobs_buffer_length);
static void decode_protobuf(size_t length);

extern UART_HandleTypeDef huart1;
extern osMessageQueueId_t status_message_queueHandle;
extern osSemaphoreId_t status_message_pendingHandle;

static uint8_t dma_rx_buffer[BUFFER_SIZE];
static uint8_t cobs_buffer[BUFFER_SIZE];
static size_t message_count = 0;

void status_message_parser__idle_callback(void) {
	osSemaphoreRelease(status_message_pendingHandle);
}

void status_message_parser__main(void) {
	if (osSemaphoreAcquire(status_message_pendingHandle, osWaitForever) != osOK) {
		Error_Handler();
	}

	while (true) {
		message_count++;
		memset(dma_rx_buffer, 0xFF, BUFFER_SIZE);
		memset(cobs_buffer, 0xFF, BUFFER_SIZE);

		if (HAL_UART_Receive_DMA(&huart1, &dma_rx_buffer[0], BUFFER_SIZE) != HAL_OK) {
			Error_Handler();
		}

		if (osSemaphoreAcquire(status_message_pendingHandle, osWaitForever) != osOK) {
			Error_Handler();
		}

		if (HAL_UART_DMAStop(&huart1) != HAL_OK) {
			Error_Handler();
		}

		size_t dma_count = BUFFER_SIZE - __HAL_DMA_GET_COUNTER(huart1.hdmarx);

		if (dma_count < 4) {
			printf("Error [%u]: No enough data in packet\n", message_count);
			continue;
		}

		if (!check_start()) {
			continue;
		}

		if (!check_end(dma_count)) {
			continue;
		}

		size_t cobs_buffer_length = decode_cobs(dma_count);
		if (cobs_buffer_length == -1) {
			continue;
		}

		if (!check_checksum(cobs_buffer_length)) {
			continue;
		}

		decode_protobuf(cobs_buffer_length);
	}
}

static bool check_start(void) {
	bool status = dma_rx_buffer[0] == 0x00;

	if (!status) {
		printf("Error [%u]: Packet start check failed\n", message_count);
	}

	return status;
}

static bool check_end(size_t dma_count) {
	bool status = dma_rx_buffer[dma_count - 1] == 0x00;

	if (!status) {
		printf("Error [%u]: Packet end check failed\n", message_count);
	}

	return status;
}

static size_t decode_cobs(size_t dma_count) {
	uint8_t *dma_rx_buffer_ptr = &dma_rx_buffer[3];
	size_t encoded_length = dma_count - 4;

	cobs_decode_result result = cobs_decode(&cobs_buffer[0], BUFFER_SIZE, dma_rx_buffer_ptr, encoded_length);
	if (result.status != COBS_DECODE_OK) {
		printf("Error [%u]: COBS decode failed (code: %u)\n", message_count, (uint16_t) result.status);
		return -1;
	}

	return result.out_len;
}

static bool check_checksum(size_t cobs_buffer_length) {
	uint16_t checksum = ((uint16_t) dma_rx_buffer[1]) | ((uint16_t) dma_rx_buffer[2] << 8);
	uint16_t buffer_checksum = 0;

	for (size_t i = 0; i < cobs_buffer_length; i++) {
		buffer_checksum += cobs_buffer[i];
	}

	bool status = buffer_checksum == checksum;

	if (!status) {
		printf("Error [%u]: Checksum failed\n", message_count);
	}

	return status;
}

static void decode_protobuf(size_t length) {
	Status message = Status_init_default;
	pb_istream_t stream = pb_istream_from_buffer(&cobs_buffer[0], length);

	if (pb_decode(&stream, &Status_msg, &message)) {
		switch (osMessageQueuePut(status_message_queueHandle, &message, 0, 0)) {
		case osOK:
			printf("Info [%u]: Got message OK\n", message_count);
			break;
		case osErrorResource:
			printf("Error [%u]: Message queue is full\n", message_count);
			break;
		default:
			Error_Handler();
		}
	} else {
		printf("Error [%u]: Failed to decode protobuf message\n", message_count);
	}
}
