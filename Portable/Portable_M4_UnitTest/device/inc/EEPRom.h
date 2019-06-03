/********************************************************************/
/*                                                                  */
/* File Name    : EEPROM.h                                    		*/
/*                                                                  */
/* General      : EEPROM stands for Electrically Erasable           */
/*  Programmable Read-Only Memory and is a type of non-volatile     */
/*   memory used in computers and other electronic devices to store */
/*    relatively small amounts of data but allowing individual      */
/*     bytes to be erased and reprogrammed.						    */
/* 								                                    */
/*                 	 						                        */
/*                                                                  */
/* Product Name  : Portable                                         */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2017/08/06       Thao Ha(MV)	  	    new file        */
/* #0001       														*/
/*                                                                  */
/********************************************************************/
#ifndef EEPROM_H_
#define EEPROM_H_
#include "stdint.h"

typedef enum
{
	eOperationMode,
	eUpdateMode,
	ePowerLostMode,
	eResetWatchdogTimer,
	eVeryLowBatteryMode
}E_OperationMode;

/****************************************************************************************/
/*                                                                                      */
/* Function name:   EEPROM_Init                                                         */
/*                                                                                      */
/* Details:        Init EEPROM                                                          */
/*                                                                                      */
/* Arguments:   None                                                                    */
/*                                                                                      */
/* ReturnValue : void                                                                   */
/*                                                                                      */
/*                                                                                      */
/****************************************************************************************/
void EEPROM_Init();

/****************************************************************************************/
/*                                                                                      */
/* Function name:   EEPROM_Read                                                         */
/*                                                                                      */
/* Details:        Read EEPROM                                                          */
/*                                                                                      */
/* Arguments:   uint32_t pageOffset                                                     */
/*              uint32_t pageAddr                                                       */
/*              uint32_t* ptr                                                           */
/*              uint32_t size                                                           */
/*                                                                                      */
/* ReturnValue : void                                                                   */
/*                                                                                      */
/*                                                                                      */
/****************************************************************************************/
void EEPROM_Read(uint32_t pageOffset, uint32_t pageAddr, uint32_t* ptr, uint32_t size);

/****************************************************************************************/
/*                                                                                      */
/* Function name:   EEPROM_Erase                                                        */
/*                                                                                      */
/* Details:        Erase EEPROM                                                         */
/*                                                                                      */
/* Arguments:   uint32_t pageAddr                                                       */
/*                                                                                      */
/* ReturnValue : void                                                                   */
/*                                                                                      */
/*                                                                                      */
/****************************************************************************************/
void EEPROM_Erase(uint32_t pageAddr);

/****************************************************************************************/
/*                                                                                      */
/* Function name:   EEPROM_Write                                                         */
/*                                                                                      */
/* Details:        Write EEPROM                                                          */
/*                                                                                      */
/* Arguments:   uint32_t pageOffset                                                     */
/*              uint32_t pageAddr                                                       */
/*              uint32_t* ptr                                                           */
/*              uint32_t size                                                           */
/*                                                                                      */
/* ReturnValue : void                                                                   */
/*                                                                                      */
/*                                                                                      */
/****************************************************************************************/
void EEPROM_Write(uint32_t pageOffset, uint32_t pageAddr, uint32_t* ptr, uint32_t size);

/****************************************************************************************/
/*                                                                                      */
/* Function name:   EEPROM_Checksum                                                     */
/*                                                                                      */
/* Details:        calculate checksume EEPROM                                           */
/*                                                                                      */
/* Arguments:   uint32_t pageOffset                                                     */
/*              uint32_t pageAddr                                                       */
/*              uint32_t* ptr                                                           */
/*              uint32_t size                                                           */
/*                                                                                      */
/* ReturnValue : void                                                                   */
/*                                                                                      */
/*                                                                                      */
/****************************************************************************************/
//uint8_t EEPROM_Checksum(uint32_t pageOffset, uint32_t pageAddr, uint32_t numberOfPage);

/****************************************************************************************/
/*                                                                                      */
/* Function name:   EEPROM_SetMode                                                      */
/*                                                                                      */
/* Details:        set mode for EEPROM                                                  */
/*                                                                                      */
/* Arguments:   E_OperationMode                                                         */
/*                                                                                      */
/* ReturnValue : void                                                                   */
/*                                                                                      */
/*                                                                                      */
/****************************************************************************************/
void EEPROM_SetMode(E_OperationMode);

/****************************************************************************************/
/*                                                                                      */
/* Function name:   EEPROM_ReadMode                                                      */
/*                                                                                      */
/* Details:        read mode of EEPROM                                                  */
/*                                                                                      */
/* Arguments:   E_OperationMode                                                         */
/*                                                                                      */
/* ReturnValue : E_OperationMode                                                                   */
/*                                                                                      */
/*                                                                                      */
/****************************************************************************************/
E_OperationMode EEPROM_ReadMode(void);

#endif /* EEPROM_H_ */
