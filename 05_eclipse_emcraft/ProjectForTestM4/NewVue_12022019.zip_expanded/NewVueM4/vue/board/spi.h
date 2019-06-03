/*
 * spi.h
 *
 *  Created on: Dec 17, 2018
 *      Author: qsbk0
 */

#ifndef BOARD_SPI_H_
#define BOARD_SPI_H_
#include "stdint.h"
//#define TEST_SPI 1

void spi_HwInit(void);

void spi_RtosInit(void);

void spi_RtosDeInit(void);

int16_t spi_Transfer(uint32_t* tx, uint32_t* rx, uint32_t size);

void spi_SendData(uint8_t address, uint32_t datagram);
void spi_ReadData(uint8_t address, uint32_t *datagram);

#endif /* BOARD_SPI_H_ */
