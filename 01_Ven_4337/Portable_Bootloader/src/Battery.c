/********************************************************************/
/*                                                                  */
/* File Name    : battery.c		                                 	*/
/*                                                                  */
/* General      : battery command to get current status of smart	*/
/* 				battery												*/
/*                 	 						                        */
/*                                                                  */
/* Product Name  : Portable                                         */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*            April 18,2017     Thao Ha(MV)		  	    new file    */
/*                                                                  */
/********************************************************************/
#include "Battery.h"
#include "board.h"
#include "gpio.h"
#define MSB_CRC8    (0x07)      // 1000 0111 x8 + x2 + x1 + x0
#define CHARGER_SET_CHARGE_INPUT_CURRENT_LIMIT_CMD 0x3d
#define CHARGER_SET_CHARGE_CURRENT_LIMIT_CMD 0x3C
#define CHARGER_SET_CHARGE_CURRENT_LIMIT_BYTE_LENGTH 0x03
#define CHARGER_SET_CHARGE_CURRENT_LIMIT_CONFIGURATION_LIMIT_TO_RAM 0x00
#define CHARGER_SET_CHARGE_CURRENT_LIMIT_LENGTH 7
#define CHARGER_SET_CHARGE_CURRENT_LIMIT_CMD_ADDR_BYTE 0
#define CHARGER_SET_CHARGE_CURRENT_LIMIT_CMD_CMD_BYTE 1
#define CHARGER_SET_CHARGE_CURRENT_LIMIT_CMD_BYTE_LENGTH_BYTE 2
#define CHARGER_SET_CHARGE_CURRENT_LIMIT_CMD_CONFIGURATION_BYTE 3
#define CHARGER_SET_CHARGE_CURRENT_LIMIT_CMD_H_BYTE 4
#define CHARGER_SET_CHARGE_CURRENT_LIMIT_CMD_L_BYTE 5
#define CHARGER_SET_CHARGE_CURRENT_LIMIT_CMD_PEC_BYTE 6
#define CHARGER_I2C_SEND_ADDR 0x20
#define BATTERY_NUMBER_OF_PEC_BYTE 1
#define BATTERY_NUMBER_OF_ADDR_BYTE 1
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	battery_GetCRC8							              				*/
/*                                                                                      */
/* Details:        create CRC8 															*/
/* Arguments:	(I)	const void *buff													*/
/* 				(I)	size_t size															*/
/*                                                                                      */
/* ReturnValue : uint8_t			                                                   	*/
/*                                                                                      */
/****************************************************************************************/
static uint8_t battery_GetCRC8( const void *buff, size_t size )
{
	uint8_t *p = (uint8_t *)buff;
	uint8_t crc8;

	for ( crc8 = 0x00 ; size != 0 ; size-- ){
		crc8 ^= *p++;

		for ( int i = 0 ; i < 8 ; i++ ){
			if ( crc8 & 0x80 ){
				crc8 <<= 1; crc8 ^= MSB_CRC8;
			}
			else{
				crc8 <<= 1;
			}
		}
	}
	return crc8;
}
void battery_UpdateChargeInputCurrentLimit(void)
{
	E_ChargingState currentStateCharge = gpio_InputVoltageCheck();
	if(currentStateCharge == eCarCharge)
	{
		battery_ChangeInputCurrentLimit(eCarCharging);
		DEBUGOUT("Change to Car charge \n");
	}
	else if(currentStateCharge == eACCharge)
	{
		battery_ChangeInputCurrentLimit(eACCharging);
		DEBUGOUT("Change to AC charge \n");
	}
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	battery_ChangeInputCurrentLimit			              				*/
/*                                                                                      */
/* Details:    Refer to specification													*/
/*                                                                                      */
/* Arguments:	None																	*/
/*                                                                                      */
/* ReturnValue : None			                                                        */
/*                                                                                      */
/****************************************************************************************/
void battery_ChangeInputCurrentLimit(E_ChargeType chargeType)
{
	I2CM_XFER_T i2cData;
	uint8_t writeCmd[CHARGER_SET_CHARGE_CURRENT_LIMIT_LENGTH]={"\0"};
	writeCmd[CHARGER_SET_CHARGE_CURRENT_LIMIT_CMD_ADDR_BYTE]=CHARGER_I2C_SEND_ADDR;
	writeCmd[CHARGER_SET_CHARGE_CURRENT_LIMIT_CMD_CMD_BYTE]=CHARGER_SET_CHARGE_INPUT_CURRENT_LIMIT_CMD;
	writeCmd[CHARGER_SET_CHARGE_CURRENT_LIMIT_CMD_BYTE_LENGTH_BYTE]=CHARGER_SET_CHARGE_CURRENT_LIMIT_BYTE_LENGTH;
	writeCmd[CHARGER_SET_CHARGE_CURRENT_LIMIT_CMD_CONFIGURATION_BYTE]=CHARGER_SET_CHARGE_CURRENT_LIMIT_CONFIGURATION_LIMIT_TO_RAM;
	switch(chargeType)
	{
	case eCarCharging:
		writeCmd[CHARGER_SET_CHARGE_CURRENT_LIMIT_CMD_CMD_BYTE]=CHARGER_SET_CHARGE_CURRENT_LIMIT_CMD;
		writeCmd[CHARGER_SET_CHARGE_CURRENT_LIMIT_CMD_H_BYTE]=0x03;
		writeCmd[CHARGER_SET_CHARGE_CURRENT_LIMIT_CMD_L_BYTE]=0xE8;
		writeCmd[CHARGER_SET_CHARGE_CURRENT_LIMIT_CMD_PEC_BYTE]=battery_GetCRC8( &writeCmd[0], CHARGER_SET_CHARGE_CURRENT_LIMIT_LENGTH-BATTERY_NUMBER_OF_PEC_BYTE );
		i2cData.slaveAddr = (CHARGER_I2C_SEND_ADDR>>1);
		i2cData.options = 0;
		i2cData.status = 0;
		i2cData.txBuff = &writeCmd[CHARGER_SET_CHARGE_CURRENT_LIMIT_CMD_CMD_BYTE];
		i2cData.txSz = CHARGER_SET_CHARGE_CURRENT_LIMIT_LENGTH-BATTERY_NUMBER_OF_ADDR_BYTE;
		i2cData.rxBuff = NULL;
		i2cData.rxSz = 0;
		if(Chip_I2CM_XferBlocking(LPC_I2C0, &i2cData) == 0)
		{
			DEBUGOUT("\nSend to I2C Error******************");
		}
		writeCmd[CHARGER_SET_CHARGE_CURRENT_LIMIT_CMD_CMD_BYTE]=CHARGER_SET_CHARGE_INPUT_CURRENT_LIMIT_CMD;
		writeCmd[CHARGER_SET_CHARGE_CURRENT_LIMIT_CMD_H_BYTE]=0x0B;//3000 = 0x0BB8
		writeCmd[CHARGER_SET_CHARGE_CURRENT_LIMIT_CMD_L_BYTE]=0xB8;//1000 = 0x03E8
		break;
	case eACCharging:
		writeCmd[CHARGER_SET_CHARGE_CURRENT_LIMIT_CMD_CMD_BYTE]=CHARGER_SET_CHARGE_CURRENT_LIMIT_CMD;
		writeCmd[CHARGER_SET_CHARGE_CURRENT_LIMIT_CMD_H_BYTE] = 0x07;//0x07D0 = 2000
		writeCmd[CHARGER_SET_CHARGE_CURRENT_LIMIT_CMD_L_BYTE] = 0xD0;
		writeCmd[CHARGER_SET_CHARGE_CURRENT_LIMIT_CMD_PEC_BYTE]=battery_GetCRC8( &writeCmd[0], CHARGER_SET_CHARGE_CURRENT_LIMIT_LENGTH-BATTERY_NUMBER_OF_PEC_BYTE );
		i2cData.slaveAddr = (CHARGER_I2C_SEND_ADDR>>1);
		i2cData.options = 0;
		i2cData.status = 0;
		i2cData.txBuff = &writeCmd[CHARGER_SET_CHARGE_CURRENT_LIMIT_CMD_CMD_BYTE];
		i2cData.txSz = CHARGER_SET_CHARGE_CURRENT_LIMIT_LENGTH-BATTERY_NUMBER_OF_ADDR_BYTE;
		i2cData.rxBuff = NULL;
		i2cData.rxSz = 0;
		if(Chip_I2CM_XferBlocking(LPC_I2C0, &i2cData) == 0)
		{
			DEBUGOUT("\nSend to I2C Error******************");
		}
		writeCmd[CHARGER_SET_CHARGE_CURRENT_LIMIT_CMD_CMD_BYTE]=CHARGER_SET_CHARGE_INPUT_CURRENT_LIMIT_CMD;
		writeCmd[CHARGER_SET_CHARGE_CURRENT_LIMIT_CMD_H_BYTE] = 0x07;//4000 = 0x0FA0
		writeCmd[CHARGER_SET_CHARGE_CURRENT_LIMIT_CMD_L_BYTE] = 0xD0;//5000 = 0x1388//4800 = 0x12C0
		break;
	case eStopCharging:
		writeCmd[CHARGER_SET_CHARGE_CURRENT_LIMIT_CMD_CMD_BYTE]=CHARGER_SET_CHARGE_CURRENT_LIMIT_CMD;
		writeCmd[CHARGER_SET_CHARGE_CURRENT_LIMIT_CMD_H_BYTE] = 0;
		writeCmd[CHARGER_SET_CHARGE_CURRENT_LIMIT_CMD_L_BYTE] = 0;
		break;
	default:
		break;
	}
	writeCmd[CHARGER_SET_CHARGE_CURRENT_LIMIT_CMD_PEC_BYTE]=battery_GetCRC8( &writeCmd[0], CHARGER_SET_CHARGE_CURRENT_LIMIT_LENGTH-BATTERY_NUMBER_OF_PEC_BYTE );
	i2cData.slaveAddr = (CHARGER_I2C_SEND_ADDR>>1);
	i2cData.options = 0;
	i2cData.status = 0;
	i2cData.txBuff = &writeCmd[CHARGER_SET_CHARGE_CURRENT_LIMIT_CMD_CMD_BYTE];
	i2cData.txSz = CHARGER_SET_CHARGE_CURRENT_LIMIT_LENGTH-BATTERY_NUMBER_OF_ADDR_BYTE;
	i2cData.rxBuff = NULL;
	i2cData.rxSz = 0;
	if(Chip_I2CM_XferBlocking(LPC_I2C0, &i2cData) == 0)
	{
		DEBUGOUT("\nSend to I2C Error******************");
	}
}

