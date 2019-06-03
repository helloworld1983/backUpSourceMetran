#ifndef EEPROM_H_
#define EEPROM_H_

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
/* Product Name  : FH310                                            */
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

#define EEPROM_PAGE_SIZE                (128)
/**The number of EEPROM pages. The last page is not writable.*/
#define EEPROM_PAGE_NUM                 (128)

#define DUM_SIZE 		4
#define SIZE_SAVE_CHECKSUM 4
#define PAGE_OFFSET_SAVE_CHECKSUM 124
#define PAGE_ADDRESS_SAVE_CHECKSUM 126

//Init EEPROM
void EEPROM_Init(void);

//Read EEPROM
void EEPROM_ReadForDump(uint32_t AddOffset, uint32_t* ptr);
void EEPROM_WriteForUpload(uint32_t AddOffset, uint8_t* EEPInt);

void EEPROM_Read(uint32_t pageOffset, uint32_t pageAddr, uint32_t* ptr, uint32_t size);

//Erase EEPROM
void EEPROM_Erase(uint32_t pageAddr);

void EEPROM_AllErase(void);

//Write EEPROM
void EEPROM_Write(uint32_t pageOffset, uint32_t pageAddr, uint32_t* ptr, uint32_t size);

//Check sum
uint8_t EEPROM_Checksum(uint32_t startPage, uint32_t endPage);
uint8_t EEPROM_ReadChecksum(void);
void EEPROM_WriteChecksum(uint8_t checksum);


#endif /* EEPROM_H_ */
