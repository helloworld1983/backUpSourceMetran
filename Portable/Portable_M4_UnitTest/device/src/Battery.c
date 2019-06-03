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
#include "TaskCommon.h"
#include "string.h"
#include "timers.h"
#include "stdlib.h"
#include "I2CInterface.h"
#include "Gpio.h"
#define MSB_CRC8    (0x07)      // 1000 0111 x8 + x2 + x1 + x0
enum
{
	eGetBatteryRemainingPercentage,
	eGetBatteryStatus,
	eGetBatteryRemainingCapacity
};
//#define DEBUG_BATTERY_EN
#ifdef DEBUG_BATTERY_EN
#define DEBUG_BATTERY(...) printf(__VA_ARGS__)
#else
#define DEBUG_BATTERY(...)
#endif

#define DEBUGBATTERY(...) printf(__VA_ARGS__)

#define BAT_ST1_PORT_H_PININT_INDEX   0


#define NUMBER_OF_RISING_EDGE_LOW 10
#define NUMBER_OF_RISING_EDGE_HIGH 10
#define NUMBER_OF_RISING_EDGE_ERROR_CHARGING 2

#define MAX_DISTANCE_VALUE 65535
#define FILTER_SIZE 3
#define BAT_100MS_TICKS_TIME 100
#define BAT_1S_CNT 10
#define CLEAR_ALARM_MODE_RESEND_TIME 40000/BATTERY_DELAY_TIME
#define START_UP_TIMEOUT 5000/BATTERY_DELAY_TIME
#define TWELVE_SECONDS_TIMEOUT 12000/BATTERY_DELAY_TIME
#define BAT_START_TIMER_ID 0

#define BATTERY_AUTHENTICATION_CMD_LENGTH 24
#define BATTERY_AUTHENTICATION_RESPONSE_LENGTH 22
#define BATTERY_AUTHENTICATION_RESPONSE_CHALLENGE_BUFF_START_BYTE 1
#define BATTERY_I2C_SEND_ADDR 0x16
#define BATTERY_I2C_RECEIVE_ADDR 0x17
#define BATTERY_AUTHENTICATION_CMD 0x2f
#define BATTERY_AUTHENTICATION_CMD_ADDR_BYTE 0
#define BATTERY_AUTHENTICATION_CMD_CMD_BYTE 1
#define BATTERY_AUTHENTICATION_CMD_CHALLENGE_BUFF_START_BYTE 2
#define BATTERY_AUTHENTICATION_CHALLENGE_BUFF_LENGTH 0x14
#define BATTERY_AUTHENTICATION_CMD_PEC_BYTE 23
#define BATTERY_NUMBER_OF_PEC_BYTE 1
#define BATTERY_NUMBER_OF_ADDR_BYTE 1
#define BATTERY_AUTHENTICATION_REQUEST_CMD_LENGTH 1
#define BATTERY_AUTHENTICATION_MAX_NUMBER_OF_FALSE_RESULT 3
#define BATTERY_AUTHENTICATION_ERROR_THREE_TIMES	3

#define BATTERY_GET_STATUS_CMD 0x16
#define BATTERY_GET_STATUS_LENGTH 3
#define BATTERY_GET_STATUS_RESPONSE_LENGTH 3
#define BATTERY_GET_STATUS_CMD_ADDR_BYTE 0
#define BATTERY_GET_STATUS_CMD_CMD_BYTE 1
#define BATTERY_GET_STATUS_CMD_PEC_BYTE 2
#define BATTERY_H_BYTE 1
#define BATTERY_L_BYTE 0
#define BATTERY_MAX_VALUE_OF_CNT 2
#define BATTERY_FIRST_SAMPLE 0
#define BATTERY_SECOND_SAMPLE 1
#define BATTERY_THIRD_SAMPLE 2

#define BATTERY_GET_REMAIN_CAPACITY_CMD 0x0f
#define BATTERY_GET_REMAIN_CAPACITY_LENGTH 3
#define BATTERY_GET_REMAIN_CAPACITY_RESPONSE_LENGTH 3
#define BATTERY_GET_REMAIN_CAPACITY_CMD_ADDR_BYTE 0
#define BATTERY_GET_REMAIN_CAPACITY_CMD_CMD_BYTE 1
#define BATTERY_GET_REMAIN_CAPACITY_CMD_PEC_BYTE 2

#define BATTERY_GET_RELATIVE_STATE_CHARGE_CMD 0x0d
#define BATTERY_GET_RELATIVE_STATE_CHARGE_LENGTH 3
#define BATTERY_GET_RELATIVE_STATE_CHARGE_RESPONSE_LENGTH 3
#define BATTERY_GET_RELATIVE_STATE_CHARGE_CMD_ADDR_BYTE 0
#define BATTERY_GET_RELATIVE_STATE_CHARGE_CMD_CMD_BYTE 1
#define BATTERY_GET_RELATIVE_STATE_CHARGE_CMD_PEC_BYTE 2

#define BATTERY_SET_MODE_CMD 0x03
#define BATTERY_SET_MODE_CMD_LENGTH 5
#define BATTERY_SET_MODE_CMD_ADDR_BYTE 0
#define BATTERY_SET_MODE_CMD_CMD_BYTE 1
#define BATTERY_SET_MODE_CMD_H_BYTE 3
#define BATTERY_SET_MODE_CMD_L_BYTE 2
#define BATTERY_SET_MODE_CMD_PEC_BYTE 4

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
#define MIN_VALUE_BATTERY_PERCENTAGE 0
#define MAX_VALUE_BATTERY_PERCENTAGE 100
#define MAXOFBUFFER 12

static const char gs_expectedResponseHMAC[20] =
{
		0x9c, 0xab, 0x30, 0x9d,
		0xa4, 0x66, 0x67, 0xcd,
		0x75, 0xf6, 0x00, 0x3c,
		0x78, 0x4e, 0x62, 0xb6,
		0x03, 0x4a, 0xeb, 0xf3
};


//static uint8_t gs_numberOfRisingEdge = 0;

static bool gs_isBatteryCharging = false;
static bool gs_isBatteryConnect = false;
static bool gs_isPrevBatteryConnect = false;
static bool gs_isBatteryChargingError = false;
static bool gs_isInvalidBatteryInserted = false; // By default this is true, so we need to do authentication

static uint16_t gs_PreBatteryPercentage = 0;
static uint16_t gs_PreBatteryCapacity = 0;
static uint16_t gs_BatteryRemainingPercentage = 0;
static uint16_t gs_BatteryRemainingCapacity = 0;
static uint16_t gs_BatteryStatus = 0;


static E_ChargeType gs_currentChargeMethod = eNoBatteryChargeType;

static E_ChargeSource gs_previousChargeSource = eBatterySource;
static E_ChargeSource gs_currentPowerSource = eBatterySource;

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	battery_Init							              				*/
/*                                                                                      */
/* Details:     Initialize Battery function by create timer and initialize interrupt 	*/
/*				pin 																	*/
/*                                                                                      */
/* Arguments:	None																	*/
/*                                                                                      */
/* ReturnValue : None			                                                        */
/*                                                                                      */
/****************************************************************************************/
void battery_Init()
{
	Chip_SCU_PinMuxSet(0x1, 17, (SCU_MODE_FUNC0));
	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT,0, 12);
	Chip_GPIO_WritePortBit(LPC_GPIO_PORT,0, 12,false);

	Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT, BAT_ST1_PORT_NUM, BAT_ST1_BIT_NUM);
}

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
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	battery_DisableAlarmMode				              				*/
/*                                                                                      */
/* Details:    Disable alarm mode by set ALARM_MODE bit in battery						*/
/*                                                                                      */
/* Arguments:	None																	*/
/*                                                                                      */
/* ReturnValue : None			                                                        */
/*                                                                                      */
/****************************************************************************************/


static void battery_DisableAlarmMode()
{
	uint8_t writeCmd[BATTERY_SET_MODE_CMD_LENGTH]={'\0'};
	writeCmd[BATTERY_SET_MODE_CMD_ADDR_BYTE]=BATTERY_I2C_SEND_ADDR;
	writeCmd[BATTERY_SET_MODE_CMD_CMD_BYTE]=BATTERY_SET_MODE_CMD;
	writeCmd[BATTERY_SET_MODE_CMD_L_BYTE]=0x01;
	writeCmd[BATTERY_SET_MODE_CMD_H_BYTE]=0x20;
	writeCmd[BATTERY_SET_MODE_CMD_PEC_BYTE]=battery_GetCRC8( &writeCmd[0], BATTERY_SET_MODE_CMD_LENGTH-BATTERY_NUMBER_OF_PEC_BYTE );
	I2CM_XFER_T i2cData;
	i2cData.slaveAddr = (BATTERY_I2C_SEND_ADDR >> 1);
	i2cData.options = 0;
	i2cData.status = 0;
	i2cData.txBuff = &writeCmd[BATTERY_SET_MODE_CMD_CMD_BYTE];
	i2cData.txSz = BATTERY_SET_MODE_CMD_LENGTH-BATTERY_NUMBER_OF_ADDR_BYTE;
	i2cData.rxBuff = NULL;
	i2cData.rxSz = 0;
	if(i2CInterface_XferBlockingTimeout(LPC_I2C0, &i2cData) == 0)
	{
		DEBUG_TASK("\nSend to I2C Error******************");
	}
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	battery_ChangeInputCurrentLimit			              				*/
/*                                                                                      */
/* Details:    Refer to specification													*/
/*                                                                                      */
/* Arguments:	(I)E_ChargeType chargeType												*/
/*                                                                                      */
/* ReturnValue : None			                                                        */
/*                                                                                      */
/****************************************************************************************/
static void battery_ChangeInputCurrentLimit(E_ChargeType chargeType)
{
	I2CM_XFER_T i2cData;
	uint8_t writeCmd[CHARGER_SET_CHARGE_CURRENT_LIMIT_LENGTH]={"\0"};
	writeCmd[CHARGER_SET_CHARGE_CURRENT_LIMIT_CMD_ADDR_BYTE]=CHARGER_I2C_SEND_ADDR;
	writeCmd[CHARGER_SET_CHARGE_CURRENT_LIMIT_CMD_CMD_BYTE]=CHARGER_SET_CHARGE_INPUT_CURRENT_LIMIT_CMD;
	writeCmd[CHARGER_SET_CHARGE_CURRENT_LIMIT_CMD_BYTE_LENGTH_BYTE]=CHARGER_SET_CHARGE_CURRENT_LIMIT_BYTE_LENGTH;
	writeCmd[CHARGER_SET_CHARGE_CURRENT_LIMIT_CMD_CONFIGURATION_BYTE] = CHARGER_SET_CHARGE_CURRENT_LIMIT_CONFIGURATION_LIMIT_TO_RAM;
	switch(chargeType)
	{
	case eCarChargeType:
		DEBUG_BATTERY("Change to Car charge\n");
		writeCmd[CHARGER_SET_CHARGE_CURRENT_LIMIT_CMD_CMD_BYTE]=CHARGER_SET_CHARGE_CURRENT_LIMIT_CMD;
		writeCmd[CHARGER_SET_CHARGE_CURRENT_LIMIT_CMD_H_BYTE] = 0x0E;//4000 = 0x0FA0
		writeCmd[CHARGER_SET_CHARGE_CURRENT_LIMIT_CMD_L_BYTE] = 0xD8;//3000= 0x0BB8
		writeCmd[CHARGER_SET_CHARGE_CURRENT_LIMIT_CMD_PEC_BYTE]=battery_GetCRC8( &writeCmd[0], CHARGER_SET_CHARGE_CURRENT_LIMIT_LENGTH-BATTERY_NUMBER_OF_PEC_BYTE );
		i2cData.slaveAddr = (CHARGER_I2C_SEND_ADDR>>1);
		i2cData.options = 0;
		i2cData.status = 0;
		i2cData.txBuff = &writeCmd[CHARGER_SET_CHARGE_CURRENT_LIMIT_CMD_CMD_BYTE];
		i2cData.txSz = CHARGER_SET_CHARGE_CURRENT_LIMIT_LENGTH-BATTERY_NUMBER_OF_ADDR_BYTE;
		i2cData.rxBuff = NULL;
		i2cData.rxSz = 0;
		if(i2CInterface_XferBlocking(LPC_I2C0, &i2cData) == 0)
		{
			DEBUG_TASK("\nSend to I2C Error******************");
		}
		writeCmd[CHARGER_SET_CHARGE_CURRENT_LIMIT_CMD_CMD_BYTE]=CHARGER_SET_CHARGE_INPUT_CURRENT_LIMIT_CMD;
		writeCmd[CHARGER_SET_CHARGE_CURRENT_LIMIT_CMD_H_BYTE] = 0x0E;//3800 = 0x0ED8
		writeCmd[CHARGER_SET_CHARGE_CURRENT_LIMIT_CMD_L_BYTE] = 0xD8;//1000 = 0x03E8
		break;
	case eACChargeType:
		DEBUG_BATTERY("Change to AC charge\n");
		writeCmd[CHARGER_SET_CHARGE_CURRENT_LIMIT_CMD_CMD_BYTE]=CHARGER_SET_CHARGE_CURRENT_LIMIT_CMD;
		writeCmd[CHARGER_SET_CHARGE_CURRENT_LIMIT_CMD_H_BYTE] = 0x11;
		writeCmd[CHARGER_SET_CHARGE_CURRENT_LIMIT_CMD_L_BYTE] = 0x30;
		writeCmd[CHARGER_SET_CHARGE_CURRENT_LIMIT_CMD_PEC_BYTE]=battery_GetCRC8( &writeCmd[0], CHARGER_SET_CHARGE_CURRENT_LIMIT_LENGTH-BATTERY_NUMBER_OF_PEC_BYTE );
		i2cData.slaveAddr = (CHARGER_I2C_SEND_ADDR>>1);
		i2cData.options = 0;
		i2cData.status = 0;
		i2cData.txBuff = &writeCmd[CHARGER_SET_CHARGE_CURRENT_LIMIT_CMD_CMD_BYTE];
		i2cData.txSz = CHARGER_SET_CHARGE_CURRENT_LIMIT_LENGTH-BATTERY_NUMBER_OF_ADDR_BYTE;
		i2cData.rxBuff = NULL;
		i2cData.rxSz = 0;
		if(i2CInterface_XferBlocking(LPC_I2C0, &i2cData) == 0)
		{
			DEBUG_TASK("\nSend to I2C Error******************");
		}
		writeCmd[CHARGER_SET_CHARGE_CURRENT_LIMIT_CMD_CMD_BYTE]=CHARGER_SET_CHARGE_INPUT_CURRENT_LIMIT_CMD;
		writeCmd[CHARGER_SET_CHARGE_CURRENT_LIMIT_CMD_H_BYTE] = 0x11;//4000 = 0x0FA0//4400 = 0x1130
		writeCmd[CHARGER_SET_CHARGE_CURRENT_LIMIT_CMD_L_BYTE] = 0x30;//5000 = 0x1388//4800 = 0x12C0
		break;
	case eNoBatteryChargeType:
		DEBUG_BATTERY("Change to Stop charge\n");
		writeCmd[CHARGER_SET_CHARGE_CURRENT_LIMIT_CMD_CMD_BYTE] = CHARGER_SET_CHARGE_CURRENT_LIMIT_CMD;
		writeCmd[CHARGER_SET_CHARGE_CURRENT_LIMIT_CMD_H_BYTE] = 0x00;
		writeCmd[CHARGER_SET_CHARGE_CURRENT_LIMIT_CMD_L_BYTE] = 0x32;
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
	if(i2CInterface_XferBlocking(LPC_I2C0, &i2cData) == 0)
	{
		DEBUG_TASK("\nSend to I2C Error******************");
	}
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	battery_UpdateChargingType				              				*/
/*                                                                                      */
/* Details: Check current input voltage type to set appropriate input current limit for */
/*			smart charger																*/
/* Arguments:	(I) void																*/
/*              (O) void                                                                */
/* ReturnValue : None			                                                        */
/*                                                                                      */
/****************************************************************************************/
static void battery_UpdateChargingType(E_ChargeSource chargeSource)
{
	DEBUG_BATTERY("update charging type \n");
	if(chargeSource == eCarSource)
	{
		DEBUG_BATTERY("\tbattery update charging type - eCarCharge\n");
		gs_currentChargeMethod = eCarChargeType;
		battery_ChangeInputCurrentLimit(gs_currentChargeMethod);
	}
	else if(chargeSource == eACSource)
	{
		DEBUG_BATTERY("\tbattery update charging type - eACCharging\n");
		gs_currentChargeMethod = eACChargeType;
		battery_ChangeInputCurrentLimit(gs_currentChargeMethod);

	}
	else
	{
		//Doing nothing here if this is battery case
	}
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	battery_UpdateChargingType				              				*/
/*                                                                                      */
/* Details: Compare 2 response HMAC														*/
/* Arguments:	(I) const char* input													*/
/*              (I) const char* expectedHMAC                                            */
/*				(I) uint8_t len															*/
/* ReturnValue : bool			                                                        */
/*                                                                                      */
/****************************************************************************************/
static bool battery_CompareHMAC(const char* input,const char* expectedHMAC,uint8_t len)
{
	for(int i=0;i<len;i++)
	{
		if(input[i]!=expectedHMAC[i])
			return false;
	}
	return true;
}


/****************************************************************************************/
/*                                                                                      */
/* Function name: 	battery_SendAuthenCommand					              			*/
/*                                                                                      */
/* Details: Send I2C command to request battery prepare data for authentication         */
/*																						*/
/* Arguments:	(O) None				                                                */
/*																						*/
/* ReturnValue : return true if send OK			                                        */
/*                                                                                      */
/****************************************************************************************/

static bool battery_SendAuthenticationCommand()
{
	uint8_t writeCmd[BATTERY_AUTHENTICATION_CMD_LENGTH]={'\0'};

	I2CM_XFER_T i2cData;

	writeCmd[BATTERY_AUTHENTICATION_CMD_ADDR_BYTE] = BATTERY_I2C_SEND_ADDR;
	writeCmd[BATTERY_AUTHENTICATION_CMD_CMD_BYTE] = BATTERY_AUTHENTICATION_CMD;
	writeCmd[BATTERY_AUTHENTICATION_CMD_CHALLENGE_BUFF_START_BYTE] = BATTERY_AUTHENTICATION_CHALLENGE_BUFF_LENGTH;
	writeCmd[BATTERY_AUTHENTICATION_CMD_PEC_BYTE] = battery_GetCRC8( &writeCmd[0],BATTERY_AUTHENTICATION_CMD_LENGTH-BATTERY_NUMBER_OF_PEC_BYTE);

	i2cData.slaveAddr = (BATTERY_I2C_SEND_ADDR>>1);
	i2cData.options = 0;
	i2cData.status = 0;
	i2cData.txBuff = &writeCmd[BATTERY_AUTHENTICATION_CMD_CMD_BYTE];
	i2cData.txSz = BATTERY_AUTHENTICATION_CMD_LENGTH-BATTERY_NUMBER_OF_ADDR_BYTE;
	i2cData.rxBuff = NULL;
	i2cData.rxSz = 0;

	if(i2CInterface_XferBlockingTimeout(LPC_I2C0, &i2cData) == 0)
	{
		DEBUG_TASK("\nSend to I2C Error******************");
		return false;
	}
	else
		return true;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	battery_DoAuthentication					              			*/
/*                                                                                      */
/* Details: Send I2C to get response HMAC to compare with expected 					    */
/*			HMAC to know whether this is valid battery									*/
/* Arguments:	(O) None    			                                                */
/*                                                                                      */
/* ReturnValue : E_ResultOfAuthen			                                            */
/*                                                                                      */
/****************************************************************************************/
static E_ResultOfAuthen battery_DoAuthentication()
{
	E_ResultOfAuthen result = eOK;
	uint8_t data[BATTERY_AUTHENTICATION_RESPONSE_LENGTH]={'\0'};
	uint8_t readCmd = BATTERY_AUTHENTICATION_CMD;

	I2CM_XFER_T i2cDataRead;
	i2cDataRead.slaveAddr = (BATTERY_I2C_RECEIVE_ADDR >> 1);
	i2cDataRead.options = 0;
	i2cDataRead.status = 0;
	i2cDataRead.txBuff = &readCmd;
	i2cDataRead.txSz = BATTERY_AUTHENTICATION_REQUEST_CMD_LENGTH;
	i2cDataRead.rxBuff = NULL;
	i2cDataRead.rxSz = 0;

	if(i2CInterface_XferBlockingTimeout(LPC_I2C0, &i2cDataRead) == 0)
	{
		DEBUG_BATTERY("\nSend to I2C Error******************");
		result = eReadAuthenDataError;
	}
	else
	{
		I2C0_ReadTimeout((BATTERY_I2C_RECEIVE_ADDR >> 1) , data, BATTERY_AUTHENTICATION_RESPONSE_LENGTH);

		uint8_t testPECArray[BATTERY_AUTHENTICATION_RESPONSE_LENGTH];
		testPECArray[0] = BATTERY_I2C_RECEIVE_ADDR;
//		for (uint8_t i = 0 ; i < (BATTERY_AUTHENTICATION_RESPONSE_LENGTH-1); i++)
//		{
//			testPECArray[i+1] = data[i];
//			DEBUGOUT("%x  ",data[i]);
//		}
//		DEBUGOUT("%x  ",data[21]);
		memcpy(&testPECArray[1],&data[0], BATTERY_AUTHENTICATION_RESPONSE_LENGTH-1);
		if(data[BATTERY_AUTHENTICATION_RESPONSE_LENGTH-1] == battery_GetCRC8(&testPECArray[0], BATTERY_AUTHENTICATION_RESPONSE_LENGTH) )
		{
//			DEBUGOUT("CRC OK.....................\n");
			bool checkResp = battery_CompareHMAC((char*)&data[BATTERY_AUTHENTICATION_RESPONSE_CHALLENGE_BUFF_START_BYTE],
					gs_expectedResponseHMAC,BATTERY_AUTHENTICATION_CHALLENGE_BUFF_LENGTH);

			if(checkResp == false)
			{
				result = eAuthenticationCodeError;
			}
			else
			{
				result = eOK;
			}
		}
		else
		{
			result = eCheckCRCError;
//			DEBUGOUT("CRC ERROR.....................\n");
		}
	}
	return result;
}




/****************************************************************************************/
/*                                                                                      */
/* Function name: 	bat_ScanStatusBatteryPort				              				*/
/*                                                                                      */
/* Details: This function runs when gs_batteryStartTimer time out for every 2.5 second.	*/
/* 			This one is used for counting the rising edge of Battery status pin 1 to 	*/
/*          detect whether the smart battery is charging	                            */
/* Arguments:	(I) TimerHandle_t xTimer												*/
/*                                                                                      */
/* ReturnValue : None			                                                        */
/*                                                                                      */
/****************************************************************************************/
static void battery_ScanStatusBatteryPort()
{
	uint8_t count_giop;
	//get state of Pin's ECharging board
	count_giop = (uint8_t)Chip_GPIO_ReadPortBit(LPC_GPIO_PORT, BAT_ST1_PORT_NUM, BAT_ST1_BIT_NUM);
	uint8_t MaxHighPulse = 0;
	uint8_t MaxLowPulse = 0;
	uint8_t TempHighPulse = 0;
	uint8_t TempLowPulse = 0;

	static uint8_t uNumberOfTimerCall = 0;
	const uint8_t NUMBEROFELEMENTUPDATE = 1;
	const uint8_t FIRSTELEMENT = 0;

	static uint8_t StateOfBattery[MAXOFBUFFER];
	uint8_t TempBuffer[MAXOFBUFFER - NUMBEROFELEMENTUPDATE];

	//add state of Pin's ECharging board to array
	if(uNumberOfTimerCall > MAXOFBUFFER){
		uNumberOfTimerCall = MAXOFBUFFER - 1;
		//when the number of element is more than buffer, begin shift to left
		memcpy(&TempBuffer[FIRSTELEMENT],&StateOfBattery[NUMBEROFELEMENTUPDATE], MAXOFBUFFER - NUMBEROFELEMENTUPDATE);
		memcpy(&StateOfBattery[FIRSTELEMENT],&TempBuffer[FIRSTELEMENT],MAXOFBUFFER - NUMBEROFELEMENTUPDATE);
		StateOfBattery[uNumberOfTimerCall] = count_giop;
	}else{
		StateOfBattery[uNumberOfTimerCall] = count_giop;
	}

	//check the stage of Pin basing on maximum of high or low pulse occur
	for(int i = 0 ; i < MAXOFBUFFER - 1; ++i){
		if(StateOfBattery[i] == StateOfBattery[i+1]){
			if(StateOfBattery[i] == 1){
				++TempHighPulse;
			}else{
				++TempLowPulse;
			}
			if(MaxHighPulse < TempHighPulse){
				MaxHighPulse = TempHighPulse;
			}else{
				//do nothing
			}
			if(MaxLowPulse < TempLowPulse){
				MaxLowPulse = TempLowPulse;
			}else{
				//do nothing
			}
		}else{
			TempHighPulse = 0;
			TempLowPulse = 0;
		}
	}

//	DEBUGOUT("MaxHighPulse(%d) MaxLowPulse(%d) NewPulse(%d)\n",MaxHighPulse, MaxLowPulse,count_giop);
//	DEBUGOUT("StateOfBattery = ");
	for(int i = 0; i < MAXOFBUFFER; ++i){
//		DEBUGOUT("%d ",StateOfBattery[i]);
	}
//	DEBUGOUT("\n");

	gs_isBatteryConnect = true;

	//every pulse is low(0) -> Low stage
	if(MaxLowPulse > NUMBER_OF_RISING_EDGE_LOW){
//		DEBUGOUT("Low Stage\n");
		gs_isBatteryCharging = false;
		if(Chip_GPIO_ReadPortBit(LPC_GPIO_PORT,BAT_ST1_PORT_NUM,BAT_ST1_BIT_NUM) == false)
		{
			gs_isBatteryConnect = false;
		}
		else
		{
			gs_isBatteryConnect = true;
		}

		if(gs_currentPowerSource == eBatterySource)
		{
			gs_isBatteryConnect = true;
		}

	}else{
		//every pulse is high(1) -> High stage
		if(MaxHighPulse > NUMBER_OF_RISING_EDGE_HIGH){
//			DEBUGOUT("High Stage\n");
			gs_isBatteryCharging = false;
		}else{
			//if low pulse and high pulse is NUMBER_OF_RISING_EDGE_ERROR_CHARGING(2) -> Blinking 2.5Hz
			if((MaxLowPulse == NUMBER_OF_RISING_EDGE_ERROR_CHARGING) && (MaxHighPulse == NUMBER_OF_RISING_EDGE_ERROR_CHARGING)){
//				DEBUGOUT("Blinking 2.5Hz \n");
				gs_isBatteryChargingError = true;
			}else{
				//another case is Blinking 0.5
//				DEBUGOUT("Blinking 0.5 Hz\n");
				gs_isBatteryCharging = true;
			}
		}
	}

	++uNumberOfTimerCall;
}



/****************************************************************************************/
/*                                                                                      */
/* Function name: 	battery_ReadBatteryStatus					              			*/
/*                                                                                      */
/* Details:    Refer to specification													*/
/*                                                                                      */
/* Arguments:	None																	*/
/*                                                                                      */
/* ReturnValue : uint16_t		                                                        */
/*                                                                                      */
/****************************************************************************************/
static uint16_t battery_ReadBatteryStatus(void)
{
	static uint16_t s_batteryStatusBuff[3];
	uint16_t returnValue = 0;
	uint8_t writeCmd[BATTERY_GET_STATUS_LENGTH]={'\0'};
	uint8_t data[BATTERY_GET_STATUS_RESPONSE_LENGTH]={'\0'};
	writeCmd[BATTERY_GET_STATUS_CMD_ADDR_BYTE]=BATTERY_I2C_SEND_ADDR;
	writeCmd[BATTERY_GET_STATUS_CMD_CMD_BYTE]=BATTERY_GET_STATUS_CMD;
	writeCmd[BATTERY_GET_STATUS_CMD_PEC_BYTE]=battery_GetCRC8( &writeCmd[0], BATTERY_GET_STATUS_LENGTH-BATTERY_NUMBER_OF_PEC_BYTE);
	I2CM_XFER_T i2cData;
	i2cData.slaveAddr = (BATTERY_GET_STATUS_CMD >> 1);
	i2cData.options = 0;
	i2cData.status = 0;
	i2cData.txBuff = &writeCmd[BATTERY_GET_STATUS_CMD_CMD_BYTE];
	i2cData.txSz = BATTERY_GET_STATUS_LENGTH-BATTERY_NUMBER_OF_ADDR_BYTE;
	i2cData.rxBuff = NULL;
	i2cData.rxSz = 0;
	if(i2CInterface_XferBlockingTimeout(LPC_I2C0, &i2cData) == 0)
	{
		DEBUG_BATTERY("\nSend to I2C Error******************");
	}
	else
	{
		I2C0_ReadTimeout((BATTERY_I2C_RECEIVE_ADDR >> 1)  ,data,BATTERY_GET_STATUS_RESPONSE_LENGTH);
		uint8_t testPECArray[BATTERY_GET_STATUS_RESPONSE_LENGTH];
		testPECArray[0]=BATTERY_I2C_RECEIVE_ADDR;
		testPECArray[1]=data[BATTERY_L_BYTE];
		testPECArray[2]=data[BATTERY_H_BYTE];
		if(data[BATTERY_GET_STATUS_CMD_PEC_BYTE] == battery_GetCRC8( &testPECArray[0], BATTERY_GET_STATUS_RESPONSE_LENGTH ))
		{
			uint16_t value = 0;
			value = (data[BATTERY_H_BYTE]<<8)|data[BATTERY_L_BYTE];
			s_batteryStatusBuff[0] = s_batteryStatusBuff[1];
			s_batteryStatusBuff[1] = s_batteryStatusBuff[2];
			s_batteryStatusBuff[2] = value;

			if (s_batteryStatusBuff[0] == s_batteryStatusBuff[1])
				returnValue = s_batteryStatusBuff[0];

			else if (s_batteryStatusBuff[1] == s_batteryStatusBuff[2])
				returnValue = s_batteryStatusBuff[1];

			else if (s_batteryStatusBuff[0] == s_batteryStatusBuff[2])
				returnValue = s_batteryStatusBuff[2];

			else
				returnValue = 0;
		}
		else
		{
			DEBUG_BATTERY("Battery Status Wrong PEC\n");
		}
	}
	return returnValue;
}


/****************************************************************************************/
/*                                                                                      */
/* Function name: 	battery_ReadBatteryRemainingCapacity				              	*/
/*                                                                                      */
/* Details:    Refer to specification													*/
/*                                                                                      */
/* Arguments:	None																	*/
/*                                                                                      */
/* ReturnValue : uint16_t		                                                        */
/*                                                                                      */
/****************************************************************************************/
static uint16_t battery_ReadBatteryRemainingCapacity()
{

	uint16_t value = 0;
	uint8_t writeCmd[BATTERY_GET_REMAIN_CAPACITY_LENGTH]={'\0'};
	writeCmd[BATTERY_GET_REMAIN_CAPACITY_CMD_ADDR_BYTE]=BATTERY_I2C_SEND_ADDR;
	writeCmd[BATTERY_GET_REMAIN_CAPACITY_CMD_CMD_BYTE]=BATTERY_GET_REMAIN_CAPACITY_CMD;
	writeCmd[BATTERY_GET_REMAIN_CAPACITY_CMD_PEC_BYTE]=battery_GetCRC8( &writeCmd[0], BATTERY_GET_REMAIN_CAPACITY_LENGTH-BATTERY_NUMBER_OF_PEC_BYTE);
	uint8_t data[BATTERY_GET_REMAIN_CAPACITY_RESPONSE_LENGTH]={'\0'};
	I2CM_XFER_T i2cData;
	i2cData.slaveAddr = (BATTERY_I2C_SEND_ADDR >> 1);
	i2cData.options = 0;
	i2cData.status = 0;
	i2cData.txBuff = &writeCmd[BATTERY_GET_REMAIN_CAPACITY_CMD_CMD_BYTE];
	i2cData.txSz = BATTERY_GET_REMAIN_CAPACITY_LENGTH-BATTERY_NUMBER_OF_ADDR_BYTE;
	i2cData.rxBuff = NULL;
	i2cData.rxSz = 0;
	if(i2CInterface_XferBlockingTimeout(LPC_I2C0, &i2cData) == 0)
	{
		DEBUG_BATTERY("\nSend to I2C Error******************");
	}
	else
	{

		I2C0_ReadTimeout((BATTERY_I2C_RECEIVE_ADDR >> 1)  ,data,BATTERY_GET_REMAIN_CAPACITY_RESPONSE_LENGTH);
		uint8_t testPECArray[BATTERY_GET_REMAIN_CAPACITY_RESPONSE_LENGTH];
		testPECArray[0]=BATTERY_I2C_RECEIVE_ADDR;
		testPECArray[1]=data[BATTERY_L_BYTE];
		testPECArray[2]=data[BATTERY_H_BYTE];
		if(data[BATTERY_GET_STATUS_CMD_PEC_BYTE]==battery_GetCRC8( &testPECArray[0], BATTERY_GET_REMAIN_CAPACITY_RESPONSE_LENGTH ))
		{
			value = (data[BATTERY_H_BYTE]<<8)|data[BATTERY_L_BYTE];


			if (gs_PreBatteryCapacity == 0)
			{
				if (value < 6600) //this is first times update, if data OK , we update data
				{
					gs_PreBatteryCapacity = value;
				}
				else
					value = 0;
			}
			else
			{
				static uint8_t countTimesError = 0;
				if (abs(value - gs_PreBatteryCapacity) > 20)
				{
					DEBUGOUT("CRC OK   ");
					DEBUGOUT("%x  ",data[BATTERY_L_BYTE]);
					DEBUGOUT("%x  ",data[BATTERY_H_BYTE]);
					DEBUGOUT("%x  \n",data[BATTERY_GET_STATUS_CMD_PEC_BYTE]);

					DEBUGOUT("RemainingCapacity.....this case is error, so use previous value\n");
					value = gs_PreBatteryCapacity; //this case is error, so we use previous value
					countTimesError++;
					if (countTimesError>= 5)
					{
						countTimesError = 0;
						gs_PreBatteryCapacity = 0;
					}
				}
				else
				{
					countTimesError = 0;
					gs_PreBatteryCapacity = value;//this case is correct, we update previous by new value
				}
			}
		}
		else
		{
			value = gs_PreBatteryCapacity;
			DEBUG_BATTERY("Battery capacity Wrong PEC\n");
			DEBUGOUT("Battery capacity Wrong PEC\n");
		}
	}
	return value;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	battery_ReadBatteryPercentage			              				*/
/*                                                                                      */
/* Details:    Refer to specification													*/
/*                                                                                      */
/* Arguments:	None																	*/
/*                                                                                      */
/* ReturnValue : uint16_t		                                                        */
/*                                                                                      */
/****************************************************************************************/

static uint16_t battery_ReadBatteryPercentage()
{
	uint16_t value = 0;
	uint8_t writeCmd[BATTERY_GET_RELATIVE_STATE_CHARGE_LENGTH]={'\0'};
	writeCmd[BATTERY_GET_RELATIVE_STATE_CHARGE_CMD_ADDR_BYTE]=BATTERY_I2C_SEND_ADDR;
	writeCmd[BATTERY_GET_RELATIVE_STATE_CHARGE_CMD_CMD_BYTE]=BATTERY_GET_RELATIVE_STATE_CHARGE_CMD;
	writeCmd[BATTERY_GET_RELATIVE_STATE_CHARGE_CMD_PEC_BYTE]=battery_GetCRC8( &writeCmd[0], BATTERY_GET_REMAIN_CAPACITY_LENGTH-BATTERY_NUMBER_OF_PEC_BYTE);
	uint8_t data[BATTERY_GET_RELATIVE_STATE_CHARGE_RESPONSE_LENGTH]={'\0'};
	I2CM_XFER_T i2cData;
	i2cData.slaveAddr = (BATTERY_I2C_SEND_ADDR >> 1);
	i2cData.options = 0;
	i2cData.status = 0;
	i2cData.txBuff = &writeCmd[BATTERY_GET_RELATIVE_STATE_CHARGE_CMD_CMD_BYTE];
	i2cData.txSz = BATTERY_GET_REMAIN_CAPACITY_LENGTH-BATTERY_NUMBER_OF_ADDR_BYTE;
	i2cData.rxBuff = NULL;
	i2cData.rxSz = 0;
	if(i2CInterface_XferBlockingTimeout(LPC_I2C0, &i2cData) == 0)
	{
		DEBUG_BATTERY("\nSend to I2C Error******************");
	}
	else
	{
		I2C0_ReadTimeout((BATTERY_I2C_RECEIVE_ADDR >> 1)  ,data,BATTERY_GET_RELATIVE_STATE_CHARGE_RESPONSE_LENGTH);
		uint8_t testPECArray[BATTERY_GET_RELATIVE_STATE_CHARGE_RESPONSE_LENGTH];
		testPECArray[0]=BATTERY_I2C_RECEIVE_ADDR;
		testPECArray[1]=data[BATTERY_L_BYTE];
		testPECArray[2]=data[BATTERY_H_BYTE];
		if(data[BATTERY_GET_STATUS_CMD_PEC_BYTE]==battery_GetCRC8( &testPECArray[0], BATTERY_GET_RELATIVE_STATE_CHARGE_RESPONSE_LENGTH ))
		{
			value = (data[BATTERY_H_BYTE]<<8)|data[BATTERY_L_BYTE];


			if (gs_PreBatteryPercentage == 0)
			{
				if (value <= 100) //this is first times update, if data OK , we update data
				{
					gs_PreBatteryPercentage = value;
				}
				else
					value = 0;
			}
			else
			{
				if (abs(value - gs_PreBatteryPercentage) > 5)
				{
					DEBUGOUT("CRC OK   ");
					DEBUGOUT("%x  ",data[BATTERY_L_BYTE]);
					DEBUGOUT("%x  ",data[BATTERY_H_BYTE]);
					DEBUGOUT("%x  \n",data[BATTERY_GET_STATUS_CMD_PEC_BYTE]);

					DEBUGOUT("Percentage.....this case is error, so use previous value\n");
					value = gs_PreBatteryPercentage; //this case is error, so we use previous value
				}
				else
				{
					gs_PreBatteryPercentage = value;//this case is correct, we update previous by new value
				}
			}
		}
		else
		{
			DEBUG_BATTERY("Battery capacity Wrong PEC\n");
			value = gs_PreBatteryPercentage; //this case is error, so use previous value
		}
	}
	return value;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	battery_ReadPowerSource					              				*/
/*                                                                                      */
/* Details:     Check which source of voltage is using to supply power for the system   */
/*                                                                                      */
/* Arguments:	None																	*/
/*                                                                                      */
/* ReturnValue : None				                                           			*/
/*                                                                                      */
/****************************************************************************************/
static void battery_ReadPowerSource(void)
{
	if((Chip_GPIO_ReadPortBit(LPC_GPIO_PORT,AC_DETECT_PORT_NUM,AC_DETECT_BIT_NUM)==true)
			&& (Chip_GPIO_ReadPortBit(LPC_GPIO_PORT,CAR_DETECT_PORT_NUM,CAR_DETECT_BIT_NUM)==true))
	{
		gs_currentPowerSource = eACSource;
	}
	else
	{
		if((Chip_GPIO_ReadPortBit(LPC_GPIO_PORT,AC_DETECT_PORT_NUM,AC_DETECT_BIT_NUM)==true)
				&& (Chip_GPIO_ReadPortBit(LPC_GPIO_PORT,CAR_DETECT_PORT_NUM,CAR_DETECT_BIT_NUM)==false))
		{
			gs_currentPowerSource = eCarSource;
		}
		else
		{
			gs_currentPowerSource = eBatterySource;
		}
	}
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	battery_Update							              				*/
/*                                                                                      */
/* Details:    Update current battery status to global status variable					*/
/*                                                                                      */
/* Arguments:	None																	*/
/*                                                                                      */
/* ReturnValue : None			                                                        */
/*                                                                                      */
/****************************************************************************************/
static void battery_Update()
{
	static uint8_t s_currentStage = eGetBatteryRemainingPercentage;
	static bool s_checkOneTimes = false;
	if(gs_isInvalidBatteryInserted == true)
	{
		gs_BatteryRemainingPercentage = 0;
		gs_BatteryRemainingCapacity = 0;
		gs_BatteryStatus = 0;
		return;
	}
	if (gs_isBatteryConnect == false)
	{
		gs_PreBatteryCapacity = 0;
		gs_PreBatteryPercentage = 0;
		gs_BatteryRemainingCapacity = 0;
		gs_BatteryStatus = 0;
		if (s_checkOneTimes == false)
		{
			s_checkOneTimes = true;
			gs_BatteryRemainingPercentage = battery_ReadBatteryPercentage();
		}
	}
	else
	{
		s_checkOneTimes = false;
		switch(s_currentStage)
		{
		case eGetBatteryRemainingPercentage:
			gs_BatteryRemainingPercentage = battery_ReadBatteryPercentage();
			if(gs_BatteryRemainingPercentage != 0)
			{
				s_currentStage = eGetBatteryStatus;
			}
			DEBUGOUT("Battery eGetBatteryStatus..............0x %x %x\n",gs_BatteryStatus/256,gs_BatteryStatus%256);
		//		DEBUGOUT("case eGetBatteryRemainingPercentage..............\n");
			break;
		case eGetBatteryStatus:
			gs_BatteryStatus = battery_ReadBatteryStatus();
			s_currentStage = eGetBatteryRemainingCapacity;
		//		DEBUGOUT("case eGetBatteryStatus..............\n");
			break;
		case eGetBatteryRemainingCapacity:
			gs_BatteryRemainingCapacity = battery_ReadBatteryRemainingCapacity();
			s_currentStage = eGetBatteryRemainingPercentage;
		//		DEBUGOUT("case eGetBatteryRemainingCapacity..............\n");
			break;

		default:
			s_currentStage = eGetBatteryRemainingPercentage;
			break;
		}
		//DEBUGOUT("Invalid, Percentage, Status, Capacity..............%d,   %d,   %d,   %d    \n",gs_isInvalidBatteryInserted,gs_BatteryRemainingPercentage,gs_BatteryStatus,gs_BatteryRemainingCapacity);
	}
}




/****************************************************************************************/
/*                                                                                      */
/* Function name: 	battery_Handle							              				*/
/*                                                                                      */
/* Details:        battery handle function is call for update battery status every 3s	*/
/* Arguments:	(I)E_ChargeSource currentChargingState									*/
/*                                                                                      */
/* ReturnValue : None			                                                        */
/*                                                                                      */
/****************************************************************************************/


void battery_Handle()
{
	static uint8_t s_cntBatteryHandle = 0;
	static uint16_t s_timeOutStartTime = START_UP_TIMEOUT;
	static int s_cntStartTime = 0;
	battery_ScanStatusBatteryPort();
	//Always get battery charge source every loop, this call update gs_currentPowerSource variable
	battery_ReadPowerSource();

	if(s_cntBatteryHandle >= BAT_1S_CNT )
	{
		s_cntBatteryHandle = 0;
		static bool s_isStartSequence = true;
		static uint16_t s_cnt = 0;

		if(s_isStartSequence == true)
		{
			gs_BatteryRemainingPercentage = battery_ReadBatteryPercentage();
//			gs_numberOfRisingEdge = 0;
			s_timeOutStartTime = START_UP_TIMEOUT;
			battery_DisableAlarmMode();
			s_isStartSequence = false;
		}

		static int s_cntTimesError;
		static E_AuthenState state = eSendAuthenCommand;

		if (gs_isBatteryConnect == true)
		{
			if(gs_isPrevBatteryConnect == false)
			{
				gs_isInvalidBatteryInserted = false;
				if(state == eSendAuthenCommand)
				{
					if(battery_SendAuthenticationCommand() == true)
						state = eReadAuthenData;
				}
				else if (state == eReadAuthenData)
				{
					// if the batter is invalid (gs_isInvalidBatteryInserted == true), then check authentication again.
					E_ResultOfAuthen result = battery_DoAuthentication();
					switch (result)
					{
					case eAuthenticationCodeError:
						s_cntTimesError++;
						if(s_cntTimesError >= BATTERY_AUTHENTICATION_ERROR_THREE_TIMES)
						{
							gs_isInvalidBatteryInserted = true;
							gs_isPrevBatteryConnect = gs_isBatteryConnect;
						}
						break;

					case eOK:
						s_cntTimesError = 0;
						gs_isPrevBatteryConnect = gs_isBatteryConnect;
						state = eSendAuthenCommand;
						break;

					case eReadAuthenDataError:
					case eCheckCRCError:
						state = eSendAuthenCommand;
						break;

					default:
						break;
					}
//					if(battery_DoAuthentication() == eAuthenticationCodeError)
//					{
//						s_cntTimesError++;
//						if(s_cntTimesError >= BATTERY_AUTHENTICATION_ERROR_THREE_TIMES)
//						{
//							gs_isInvalidBatteryInserted = true;
//							gs_isPrevBatteryConnect = gs_isBatteryConnect;
//						}
//					}
//					else
//					{
//						s_cntTimesError = 0;
//						gs_isPrevBatteryConnect = gs_isBatteryConnect;
//					}
//					state = eSendAuthenCommand;
				}
			}
		}
		else
		{
			s_cntTimesError = 0;
			gs_isPrevBatteryConnect = false;
		}

		// Check if battery inserted is valid, if just newly inserted, then update battery charge type
		if(gs_isInvalidBatteryInserted == true)
		{
//			DEBUGOUT("Invalid battery detected\n");
			if (gs_currentChargeMethod != eNoBatteryChargeType)
			{
				// This is a similar case of No AC and No Car charge.
				gs_currentChargeMethod=eNoBatteryChargeType;
				battery_ChangeInputCurrentLimit(eNoBatteryChargeType);
				DEBUG_BATTERY("Stop charging because of wrong battery\n");
			}
		}
		else
		{	//In this case, battery is detected by authentication code. If battery is valid, change current charge base on power source
			if(gs_currentChargeMethod==eNoBatteryChargeType)
			{
				battery_UpdateChargingType(gs_currentPowerSource);
//				DEBUGOUT("Start charging battery again\n");
			}
		}

		if(gs_currentPowerSource != gs_previousChargeSource) // if not start up sequence check for source change
		{
			DEBUGBATTERY("battery_Handle_gs_currentCharge\n");
			battery_UpdateChargingType(gs_currentPowerSource);
			s_timeOutStartTime = TWELVE_SECONDS_TIMEOUT;
			s_cntStartTime = 0;
		}
		gs_previousChargeSource = gs_currentPowerSource;

		if(s_cntStartTime < s_timeOutStartTime)
		{
			++s_cntStartTime;
			gs_BatteryRemainingPercentage = battery_ReadBatteryPercentage();
		}
		else
		{
			battery_Update();
		}

		if(s_cnt>=CLEAR_ALARM_MODE_RESEND_TIME)
		{
			if(gs_isBatteryConnect==true)
			{
				battery_DisableAlarmMode();
				s_cnt=0;
			}
		}
		else
			s_cnt++;
	}
	else
		++s_cntBatteryHandle;
}


/****************************************************************************************/
/*                                                                                      */
/* Function name: 	battery_isBatteryCharging				              				*/
/*                                                                                      */
/* Details:    Check whether battery is charging										*/
/*                                                                                      */
/* Arguments:	None																	*/
/*                                                                                      */
/* ReturnValue : bool			                                                        */
/*                                                                                      */
/****************************************************************************************/
bool battery_isBatteryCharging()
{
	return gs_isBatteryCharging;
}


/****************************************************************************************/
/*                                                                                      */
/* Function name: 	battery_isBatteryConnected				              				*/
/*                                                                                      */
/* Details:    Check whether battery is connected										*/
/*                                                                                      */
/* Arguments:	None																	*/
/*                                                                                      */
/* ReturnValue : bool			                                                        */
/*                                                                                      */
/****************************************************************************************/
bool battery_isBatteryConnected()
{
	return gs_isBatteryConnect;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	batter_isBatteryChargingError			              				*/
/*                                                                                      */
/* Details:    check whether battery charging is error									*/
/*                                                                                      */
/* Arguments:	None																	*/
/*                                                                                      */
/* ReturnValue : None			                                                        */
/*                                                                                      */
/****************************************************************************************/
bool battery_isBatteryChargingError()
{
	return gs_isBatteryChargingError;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	battery_isInvalidBatteryInserted		              				*/
/*                                                                                      */
/* Details: check whether invalid battery type is inserted								*/
/* Arguments:	(I) None																*/
/*              (O) None					                                            */
/* ReturnValue : bool			                                                        */
/*                                                                                      */
/****************************************************************************************/
bool battery_isInvalidBatteryInserted()
{
	return gs_isInvalidBatteryInserted;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	battery_GetRemainingPercentage			              				*/
/*                                                                                      */
/* Details:    Get remaining capacity percentage										*/
/*                                                                                      */
/* Arguments:	None																	*/
/*                                                                                      */
/* ReturnValue : uint16_t		                                                        */
/*                                                                                      */
/****************************************************************************************/
uint16_t battery_GetRemainingPercentage()
{
//	DEBUGOUT("battery_GetRemainingPercentage:........ %d\n",gs_BatteryRemainingPercentage);
	return gs_BatteryRemainingPercentage;
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	battery_GetRemainingCapacity			              				*/
/*                                                                                      */
/* Details:    Get remaining capacity (mAh)												*/
/*                                                                                      */
/* Arguments:	None																	*/
/*                                                                                      */
/* ReturnValue : uint16_t		                                                        */
/*                                                                                      */
/****************************************************************************************/
uint16_t battery_GetRemainingCapacity()
{
	return gs_BatteryRemainingCapacity;
}


/****************************************************************************************/
/*                                                                                      */
/* Function name: 	battery_getStatus						              				*/
/*                                                                                      */
/* Details:    get status of battery													*/
/*                                                                                      */
/* Arguments:	None																	*/
/*                                                                                      */
/* ReturnValue : uint16_t			                                                    */
/*                                                                                      */
/****************************************************************************************/
uint16_t battery_GetStatus()
{
	return gs_BatteryStatus;
}


/****************************************************************************************/
/*                                                                                      */
/* Function name: 	battery_GetChargeSource				              					*/
/*                                                                                      */
/* Details:     Get charge status													    */
/*                                                                                      */
/* Arguments:	None																	*/
/*                                                                                      */
/* ReturnValue : E_ChargeSource			                                      		    */
/*                                                                                      */
/****************************************************************************************/
E_ChargeSource battery_GetChargeSource(void)
{
	return gs_currentPowerSource;
}

