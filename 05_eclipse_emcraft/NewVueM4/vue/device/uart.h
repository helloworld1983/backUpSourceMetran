/******************************************************************************
*
*    File Name: uart.h
*    @brief Copyright (c) 2018, Metran.  All Rights Reserved.
*
*
*    Revision History:
*
*       Rev:      Date:       	Engineer:         Project:
*        01       11/29/2018    Quyen Ngo         NewVue
*        Description: New file
******************************************************************************/

#ifndef DEVICE_UART_H_
#define DEVICE_UART_H_

//#define TEST_UART 3

void uart_Init(uint32_t channel, uint32_t baud);

int uart_Read(uint32_t channel, uint8_t* buf, uint32_t len);

void uart_Write(uint32_t channel, uint8_t* buf, uint32_t len);

void uart_Deinit(uint32_t channel);

#endif /* DEVICE_UART_H_ */
