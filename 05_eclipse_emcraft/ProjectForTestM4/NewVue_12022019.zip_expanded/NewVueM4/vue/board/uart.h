#ifndef BOARD_UART_H_
#define BOARD_UART_H_
#include "stdint.h"
#define TEST_UART 3

void uart_Init(uint32_t channel, uint32_t baud);

int uart_Read(uint32_t channel, uint8_t* buf, uint32_t len);

void uart_Write(uint32_t channel, uint8_t* buf, uint32_t len);

void uart_Deinit(uint32_t channel);

#endif /* BOARD_UART_H_ */
