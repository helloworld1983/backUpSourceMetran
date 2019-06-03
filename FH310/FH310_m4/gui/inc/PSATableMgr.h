#ifndef PSATABLEMGR_H_
#define PSATABLEMGR_H_

/********************************************************************/
/*                                                                  */
/* File Name    : PSATableMgr.h                       			    */
/*                                                                  */
/* General       : Write item psa parameter to EPPROM               */
/* 				   Read item psa parameter from EPPROM				*/
/*                 	 						                        */
/*                                                                  */
/* Product Name  : FH310                                            */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2016/08/22      Viet Le			  	new file        */
/* #0001       2017/09/19	   Viet Le				modify			*/
/*      - Edit define SIZE_BUFFER_READ 60->54 (bug 1844)            */
/********************************************************************/

#include "board.h"

#define SIZE_BUFFER_READ 	54
#define SIZE_BUFFER_WRITE 	60

//Condition O2 concentrator
typedef enum
{
    eNormalO2Type=0,
    eLowO2Type,
    eMaxO2Type
}E_O2ConcentratorType;

//Condition temperature
typedef enum
{
    eHighTempType = 0,
    eNormalTempType,
    eLowTempType,
    eMaxTempType
}E_TemperatureType;

//Psa parameter item
typedef enum
{
    ePSAPress,
	eSolenoid1OnOffsetTime,
	eSolenoid1OffOffsetTime,
	eSolenoid2OnOffsetTime,
	eSolenoid2OffOffsetTime,
	eSolenoid3OnOffsetTime,
	eSolenoid3OffOffsetTime,
	eSolenoid4OnOffsetTime,
	eSolenoid4OffOffsetTime,
	eSolenoid5OnOffsetTimeA,
	eSolenoid5OnOffsetTimeB,
    eSolenoid5OpenTimeA,
    eSolenoid5OpenTimeB,
    eCoolingFanDutyCycle,
	eCompressorRPM,
    eMFCOutput,
    eMaxPSATime,
	eSolenoidOpenTimeOnEnd,
    eMaxItemOfParam
}E_ItemOfParam;

//Init table psa parameter
void PSATableMgr_InitTablePSA(bool erasePSATable);
//Write PSA parameter to EEPROM
void PSATableMgr_WritePSAParam(uint8_t flowRate,E_O2ConcentratorType o2Type);
//Set PSA parameter
void PSATableMgr_SetPSAParam(uint8_t flowRate,E_O2ConcentratorType o2Type, E_TemperatureType tempType,E_ItemOfParam psaId, uint8_t value);
//Get PSA Item
uint8_t PSATableMgr_GetPSAItem(uint8_t flowRate,E_O2ConcentratorType o2Type, E_TemperatureType tempType,E_ItemOfParam item);
//Set PSA parameter if is default select port
void PSATableMgr_SetDefaultSelectPort(void);
//Set PSA parameter if is eeprom select port
void PSATableMgr_SetEEPROMSelectPort(void);

#endif /* PSATABLEMGR_H_ */
/* end of file */

