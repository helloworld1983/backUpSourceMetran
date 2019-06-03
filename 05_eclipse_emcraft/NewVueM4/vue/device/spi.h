/*
 * spi.h
 *
 *  Created on: Dec 17, 2018
 *      Author: qsbk0
 */

#ifndef DEVICE_SPI_H_
#define DEVICE_SPI_H_

//#define TEST_SPI 1

void spi_HwInit(void);

void spi_RtosInit(void);

void spi_RtosDeInit(void);

int16_t spi_Transfer(uint32_t* tx, uint32_t* rx, uint32_t size);

void SendData(uint8_t address, uint32_t datagram);
void ReadData(uint8_t address, uint32_t *datagram);

#endif /* DEVICE_SPI_H_ */
