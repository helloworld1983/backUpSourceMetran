/********************************************************************/
/*                                                                  */
/* File Name     : ExternalEeprom.h			                        */
/*                                                                  */
/* General       : Contains function for init and communicate with  */
/* 				   external Eeprom									*/
/*                 	 						                        */
/*                                                                  */
/* Product Name  : FH310                                            */
/* 						                                            */
/* Company		 : Metran                                           */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2017/07/12      Quyen Ngo  			new file  	    */
/*	#0001	   2017/08/24      Quyen Ngo			modify			*/
/*			change extEeprom_ResetSW for create the true signal for */
/*			reset sequence (fix bug 1683)							*/
/*	#0002      2017/09/28	   Quyen Ngo			modify			*/
/*			remove function not use, correct the time delay for 	*/
/*			write cycle												*/
/*	#0003      2017/11/17	   Quyen Ngo			modify			*/
/*			change reset sequence for new EEPROM				 	*/
/*                                                                  */
/*	#0004      2017/11/21	   Quyen Ngo			modify			*/
/*			Remove unecessary wait in EEPROM reset sequence			*/
/********************************************************************/

#include <string.h>
#include "board.h"
#include "ExternalEeprom.h"
#include "I2CInterface.h"
#include "Delay.h"
/******************************************************************************
 * Defines and typedefs
 *****************************************************************************/

#define I2C_PORT (LPC_I2C0)

#define EXT_EEPROM_I2C_ADDR(block)  ((0x50) | block)

#define EXT_EEPROM_PAGE_SIZE     	   		256
#define EXT_EEPROM_BLOCK_TOTAL_SIZE    		256*256
#define EXT_EEPROM_NUM_BLOCK		   		2
#define EXT_NUM_OF_START_SEQ_FOR_RESET		9
#define EXT_DELAY_WRITE_CYCLE_MAX_IN_MS		5
#define EXT_DELAY_AFTER_DUMMYCLOCK			10000	//Experience
#define EXT_DELAY_BEFORE_STOP				2000    //Experience

/******************************************************************************
 * External global variables
 *****************************************************************************/

/******************************************************************************
 * Local variables
 *****************************************************************************/


/******************************************************************************
 * Local Functions
 *****************************************************************************/

static Status I2CWrite(uint32_t addr, uint8_t* buf, uint32_t len)
{
	I2CM_XFER_T i2cData;

	i2cData.slaveAddr = addr;
	i2cData.options = 0;
	i2cData.status = 0;
	i2cData.txBuff = buf;
	i2cData.txSz = len;
	i2cData.rxBuff = NULL;
	i2cData.rxSz = 0;

	if (I2CInterfaceTransfer(LPC_I2C0, &i2cData) == ERROR) {
		return ERROR;
	}
	return SUCCESS;
}

static Status I2CRead(uint32_t addr, uint8_t* buf, uint32_t len)
{
	I2CM_XFER_T i2cData;

	i2cData.slaveAddr = addr;
	i2cData.options = 0;
	i2cData.status = 0;
	i2cData.txBuff = NULL;
	i2cData.txSz = 0;
	i2cData.rxBuff = buf;
	i2cData.rxSz = len;

	if (I2CInterfaceTransfer(LPC_I2C0, &i2cData) == ERROR) {
		return ERROR;
	}
	return SUCCESS;
}


/******************************************************************************
 * Public Functions
 *****************************************************************************/

/******************************************************************************/
//COMMON.OPERATION
//    Function Name: extEeprom_Write
//
//    Detail: Write data to EEPROM
//
//    Arguments: (I) uint8_t block,
//					Range {0,1}
//					 uint8_t* buf,
//					 uint16_t offset,
//					 uint16_t len
//					 Range: 0~256
//				 (O)
//
//    Return Values:  int16_t: number of read bytes or -1 in case of an error
//
//
//    Using Global Data : (I)
//						  (O)
//
//
//    Note: None
//
/******************************************************************************/
int16_t extEeprom_Read(uint8_t block, uint8_t* buf, uint8_t page,uint8_t offset, uint8_t len)
{
	int i = 0;
	uint8_t off[2];

	if(block >= EXT_EEPROM_NUM_BLOCK)
	{
		return -1;
	}
	if((offset + len) > EXT_EEPROM_PAGE_SIZE)
	{
		return -1;
	}

	off[0] = page;
	off[1] = offset;

	// 1ms delay needed to prevent misreads from EEPROM
	delay_MS(EEPROM_READ_STABLE_TIME_MS);

	if (I2CWrite((EXT_EEPROM_I2C_ADDR(block)), off, 2) == SUCCESS)
	{
		for ( i = 0; i < 0x2000; i++);//wait a little time
		if (I2CRead((EXT_EEPROM_I2C_ADDR(block)), buf, len) == SUCCESS) {
			return len;
		}
	}
	return -1;
}

/******************************************************************************/
//COMMON.OPERATION
//    Function Name: extEeprom_Write
//
//    Detail: Write data to EEPROM
//
//    Arguments: (I) uint8_t block,
//					Range {0,1}
//					 uint8_t* buf,
//					 uint16_t offset,
//					 uint16_t len
//					 Range: 0~256
//				 (O)
//
//    Return Values:  int16_t: number of written bytes or -1 in case of an error
//
//
//    Using Global Data : (I)
//						  (O)
//
//
//    Note: None
//
/******************************************************************************/
int16_t extEeprom_Write(uint8_t block, uint8_t* buf, uint8_t page, uint8_t offset, uint8_t len)
{
	uint16_t wLen = 0;
	uint16_t off = offset;
	uint8_t tmp[EXT_EEPROM_PAGE_SIZE+2];//tmp buffer data which be wroten to I2C bus
	if(block >= EXT_EEPROM_NUM_BLOCK)
	{
		return -1;
	}
	if((offset + len) > EXT_EEPROM_PAGE_SIZE)
	{
		return -1;
	}

	wLen = EXT_EEPROM_PAGE_SIZE - (off % EXT_EEPROM_PAGE_SIZE);
	wLen = MIN(wLen, len);

	tmp[0] = page;
	tmp[1] = off;
	memcpy(&tmp[2], (void*)buf, wLen);
	if(I2CWrite((EXT_EEPROM_I2C_ADDR(block)), tmp, wLen+2) == SUCCESS)
	{
		/* delay to wait for a write cycle */
		delay_HardDelay(EXT_DELAY_WRITE_CYCLE_MAX_IN_MS);
		return wLen;
	}
	else
	{
		return -1;//error
	}
}

/******************************************************************************/
//COMMON.OPERATION
//    Function Name: extEeprom_ResetSW
//
//    Detail: External Eeprom software reset
//
//    Arguments: (I)
//				 (O)
//
//    Return Values:  NONE
//
//
//    Using Global Data : (I)
//						  (O)
//
//
//    Note: None
//
/******************************************************************************/
void extEeprom_ResetSW(void)
{
	uint8_t buf[1] = {0xFF};
	LPC_I2C0->CONCLR = I2C_CON_AA | I2C_CON_STA | I2C_CON_STO | I2C_CON_SI;
	Chip_I2CM_Write(LPC_I2C0,buf,1);
	for(int i =0; i < EXT_DELAY_AFTER_DUMMYCLOCK; i++);
	Chip_I2CM_ResetControl(LPC_I2C0);
	Chip_I2CM_ClearSI(LPC_I2C0);
	Chip_I2CM_SendStart(LPC_I2C0);
	for(int i =0; i < EXT_DELAY_BEFORE_STOP; i++);
	Chip_I2CM_ClearSI(LPC_I2C0);
	Chip_I2CM_SendStop(LPC_I2C0);
	return;
}

