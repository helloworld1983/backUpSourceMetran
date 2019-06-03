#ifndef EEPROM_H_
#define EEPROM_H_
#define EEPROM_MODE_PAGE 0x1c
#define EEPROM_MODE_OFFSET 0
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
/* Product Name  : Portable                                            */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2016/08/14       Viet Le		  	    new file        */
/* #0001       														*/
/*                                                                  */
/********************************************************************/
#include "stdint.h"
typedef enum
{
	eOperationMode,
	eUpdateMode,
	ePowerLostMode,
	eResetWatchdogTimer,
	eVeryLowBatteryMode
}E_OperationMode;

//Init EEPROM
void EEPROM_Init();

//Read EEPROM
void EEPROM_Read(uint32_t pageOffset, uint32_t pageAddr, uint32_t* ptr, uint32_t size);

//Erase EEPROM
void EEPROM_Erase(uint32_t pageAddr);

//Write EEPROM
void EEPROM_Write(uint32_t pageOffset, uint32_t pageAddr, uint32_t* ptr, uint32_t size);

//Check sum
//uint8_t EEPROM_Checksum(uint32_t pageOffset, uint32_t pageAddr, uint32_t numberOfPage);
uint8_t EEPROM_Checksum(uint32_t startPageOffset, uint32_t startPageAddr, uint32_t endPageOffset,uint32_t endPageAddr);
void EEPROM_ClearUpdateFlag(void);
void EEPROM_SetMode(E_OperationMode mode);
E_OperationMode EEPROM_ReadMode(void);
#endif /* EEPROM_H_ */
