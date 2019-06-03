/******************************************************************************
*
*    File Name: i2c.h
*    @brief Copyright (c) 2018, Metran.  All Rights Reserved.
*
*
*    Revision History:
*
*       Rev:      Date:       	Engineer:         Project:
*        01       11/29/2018    Quyen Ngo         NewVue
*        Description: New file
******************************************************************************/

#ifndef DEVICE_I2C_H_
#define DEVICE_I2C_H_

#include "fsl_i2c.h"

#define TEST_I2C

/*!
 * @brief Init I2C channel based on base address and baudrate
 * This function will set up root clock mux for I2C peripheral
 * and config the clock source and baudrate of corresponding I2C
 * channel
 *
 * @param chanelBase    Base address of I2C channel(see on Memory map)
 * @baudRate            baudRate or clockRate of I2C channel (depend on
 * 						slave device spec)
 *
 * @return nothing
 */
void i2c_Init(I2C_Type* chanelBase, uint32_t baudRate);

status_t i2c_Write(I2C_Type* chanelBase, uint32_t addr, uint8_t* buf, uint32_t len);

status_t i2c_Read(I2C_Type* chanelBase, uint32_t addr, uint8_t* buf, uint32_t len);

#endif /* DEVICE_I2C_H_ */
