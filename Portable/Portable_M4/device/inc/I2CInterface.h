/*
 * I2CInterface.h
 *
 *  Created on: Oct 4, 2017
 *      Author: WINDOWS
 */

#ifndef DEVICE_INC_I2CINTERFACE_H_
#define DEVICE_INC_I2CINTERFACE_H_

#include "board.h"

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	i2CInterface_XferBlocking				              				*/
/*                                                                                      */
/* Details:    Transmit and Receive data in master mode									*/
/*                                                                                      */
/* Arguments:	pI2C	: Pointer to selected I2C peripheral							*/
/*              xfer	: Pointer to a I2CM_XFER_T structure 					        */
/*                                                                                      */
/* ReturnValue : Returns non-zero value on successful completion of transfer			*/
/*                                                                                      */
/****************************************************************************************/
uint32_t i2CInterface_XferBlocking(LPC_I2C_T *pI2C, I2CM_XFER_T *xfer);



/****************************************************************************************/
/*                                                                                      */
/* Function name: 	i2CInterface_XferBlockingTimeout				              		*/
/*                                                                                      */
/* Details:    Transmit and Receive data in master mode	and add timeout function		*/
/*                                                                                      */
/* Arguments:	pI2C	: Pointer to selected I2C peripheral							*/
/*              xfer	: Pointer to a I2CM_XFER_T structure 					        */
/*                                                                                      */
/* ReturnValue : Returns non-zero value on successful completion of transfer			*/
/*                                                                                      */
/****************************************************************************************/
uint32_t i2CInterface_XferBlockingTimeout(LPC_I2C_T *pI2C, I2CM_XFER_T *xfer);

#endif /* DEVICE_INC_I2CINTERFACE_H_ */
