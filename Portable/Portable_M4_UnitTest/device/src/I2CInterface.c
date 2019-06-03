/*
 * I2CInterface.c
 *
 *  Created on: Oct 4, 2017
 *      Author: WINDOWS
 */

#include "I2CInterface.h"
#include "TaskCommon.h"

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

uint32_t i2CInterface_XferBlocking(LPC_I2C_T *pI2C, I2CM_XFER_T *xfer)
{
	uint32_t ret = 0;
	vTaskSuspendAll();
	ret = Chip_I2CM_XferBlocking(pI2C, xfer);
	xTaskResumeAll();
	return ret;
}

uint32_t Chip_I2CM_XferBlockingTimeout(LPC_I2C_T *pI2C, I2CM_XFER_T *xfer)
{
    uint32_t ret = 0;
    uint32_t timeCnt1 = 0;
    uint32_t timeCnt2 = 0;

    return ret;
}



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
uint32_t i2CInterface_XferBlockingTimeout(LPC_I2C_T *pI2C, I2CM_XFER_T *xfer)
{
	uint32_t ret = 0;
	vTaskSuspendAll();
	ret = Chip_I2CM_XferBlockingTimeout(pI2C, xfer);
	xTaskResumeAll();
	return ret;
}
