/*
 * TaskCommon.c
 *
 *  Created on: Dec 22, 2016
 *      Author: ThaoHa
 */
#include <TaskCommon.h>
#include "board.h"
#include "I2CInterface.h"

#define WDT_1S_CNT	3000000 //Timeout=1/Twdclk*4*TCvalue 1/((1/12000000)*4)
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	WDT_init									      				    */
/*                                                                                      */
/* Details:        Initialize Watch dog timer											*/
/*                                                                                      */
/* Arguments:	(I)	None																*/
/*                                                                                      */
/* ReturnValue : void			                                                        */
/*                                                                                      */
/****************************************************************************************/
void WDT_init(void)
{
	Chip_WWDT_Init(LPC_WWDT);

	Chip_WWDT_SetTimeOut(LPC_WWDT, 5*WDT_1S_CNT);
	/* Configure WWDT to reset on timeout */
	Chip_WWDT_SetOption(LPC_WWDT, WWDT_WDMOD_WDRESET);

	/* Clear watchdog warning and timeout interrupts */
	Chip_WWDT_ClearStatusFlag(LPC_WWDT, WWDT_WDMOD_WDTOF | WWDT_WDMOD_WDINT);

	/* Clear watchdog warning and timeout interrupts */
	Chip_WWDT_ClearStatusFlag(LPC_WWDT, WWDT_WDMOD_WDTOF);
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	I2C1_Read									      				    */
/*                                                                                      */
/* Details:        Read data from I2C1													*/
/*                                                                                      */
/* Arguments:	(I)	uint32_t addr														*/
/*              (O) uint8_t* buf														*/
/*              (I) uint32_t len                                                        */
/* ReturnValue : Status			                                                        */
/*                                                                                      */
/****************************************************************************************/
Status I2C1_Read(uint32_t addr, uint8_t* buf, uint32_t len)
{
	I2CM_XFER_T i2cData;

	i2cData.slaveAddr = addr;
	i2cData.options = 0;
	i2cData.status = 0;
	i2cData.txBuff = NULL;
	i2cData.txSz = 0;
	i2cData.rxBuff = buf;
	i2cData.rxSz = len;

	if (i2CInterface_XferBlocking(LPC_I2C1, &i2cData) == 0) {
		return ERROR;
	}
	return SUCCESS;
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	I2C0_Read									      				    */
/*                                                                                      */
/* Details:        Read data from I2C0													*/
/*                                                                                      */
/* Arguments:	(I)	uint32_t addr														*/
/*              (O) uint8_t* buf														*/
/*              (I) uint32_t len                                                        */
/* ReturnValue : Status			                                                        */
/*                                                                                      */
/****************************************************************************************/
Status I2C0_Read(uint32_t addr, uint8_t* buf, uint32_t len)
{
	I2CM_XFER_T i2cData;

	i2cData.slaveAddr = addr;
	i2cData.options = 0;
	i2cData.status = 0;
	i2cData.txBuff = NULL;
	i2cData.txSz = 0;
	i2cData.rxBuff = buf;
	i2cData.rxSz = len;
	if (i2CInterface_XferBlocking(LPC_I2C0, &i2cData) == 0) {
		return ERROR;
	}
	return SUCCESS;
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	I2C0_ReadTimeout							      				    */
/*                                                                                      */
/* Details:        Read data from I2C0 with timeout										*/
/*                                                                                      */
/* Arguments:	(I)	uint32_t addr														*/
/*              (O) uint8_t* buf														*/
/*              (I) uint32_t len                                                        */
/* ReturnValue : Status			                                                        */
/*                                                                                      */
/****************************************************************************************/
Status I2C0_ReadTimeout(uint32_t addr, uint8_t* buf, uint32_t len)
{
	I2CM_XFER_T i2cData;

	i2cData.slaveAddr = addr;
	i2cData.options = 0;
	i2cData.status = 0;
	i2cData.txBuff = NULL;
	i2cData.txSz = 0;
	i2cData.rxBuff = buf;
	i2cData.rxSz = len;

	if (i2CInterface_XferBlockingTimeout(LPC_I2C0, &i2cData) == 0) {
		return ERROR;
	}
	return SUCCESS;
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	WDT_SystemReset								      				    */
/*                                                                                      */
/* Details:        Reset System using Watchdog timer									*/
/*                                                                                      */
/* Arguments:	(I)	None																*/
/*                                                                                      */
/* ReturnValue : void			                                                        */
/*                                                                                      */
/****************************************************************************************/
void WDT_SystemReset(void)
{
	Chip_WWDT_Init(LPC_WWDT);

	Chip_WWDT_SetTimeOut(LPC_WWDT, 0xFF);

	Chip_WWDT_Start(LPC_WWDT);

	while(1);
}
