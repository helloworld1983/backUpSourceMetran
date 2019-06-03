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
/*             2017/08/06       Thao Ha		  	    new file        */
/* #0001       														*/
/*                                                                  */
/********************************************************************/

#include "board.h"
#include "string.h"
#include "EEPROM.h"
/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/
/* Auto programming on/off */
#define AUTOPROG_ON     1

/****************************************************************************************/
/*                                                                                      */
/* General 	-Initialize EEPROM		                                            		*/
/*                                                                                      */
/* Details  -Initialize EEPROM	        												*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE									 					  			*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*                                                                                      */
/****************************************************************************************/
void EEPROM_Init()
{
	/* Init EEPROM */
	Chip_EEPROM_Init(LPC_EEPROM);

#if AUTOPROG_ON
	/* Set Auto Programming mode */
	Chip_EEPROM_SetAutoProg(LPC_EEPROM,EEPROM_AUTOPROG_AFT_1WORDWRITTEN);
#else
	/* Set Auto Programming mode */
	Chip_EEPROM_SetAutoProg(LPC_EEPROM,EEPROM_AUTOPROG_OFF);
#endif /*AUTOPROG_ON*/
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-Read data from EEPROM		                                            	*/
/*                                                                                      */
/* Details  -Read data from EEPROM size must be multiple of 4 bytes      				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) uint32_t pageOffset : page offset				 					*/
/*				   uint32_t pageAddr : page address			                            */
/*                 uint32_t* ptr : pointer of buffer read						        */
/*				   uint32_t size: size of buffer read                                   */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*                                                                                      */
/****************************************************************************************/
void EEPROM_Read(uint32_t pageOffset, uint32_t pageAddr, uint32_t* ptr, uint32_t size)
{
	uint32_t i = 0;
	uint32_t *pEepromMem = (uint32_t*)EEPROM_ADDRESS(pageAddr,pageOffset);
	for(i = 0; i < size/4; i++) {
		ptr[i] = pEepromMem[i];
	}
}
/****************************************************************************************/
/*                                                                                      */
/* General 	-Erase a page in EEPROM		                                            	*/
/*                                                                                      */
/* Details  -Erase a page in EEPROM                                      				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) uint32_t pageAddr : page address					 					*/
/*				   		                           									    */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*                                                                                      */
/****************************************************************************************/
void EEPROM_Erase(uint32_t pageAddr)
{
	uint32_t i = 0;
	uint32_t *pEepromMem = (uint32_t*)EEPROM_ADDRESS(pageAddr,0);
	for(i = 0; i < EEPROM_PAGE_SIZE/4; i++) {
		pEepromMem[i] = 0;
#if AUTOPROG_ON
		Chip_EEPROM_WaitForIntStatus(LPC_EEPROM, EEPROM_INT_ENDOFPROG);
#endif
	}
#if (AUTOPROG_ON == 0)
	Chip_EEPROM_EraseProgramPage(LPC_EEPROM);
#endif
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-Write data to a page in EEPROM		                                        */
/*                                                                                      */
/* Details  -Write data to a page in EEPROM size must be multiple of 4 bytes    	    */
/*																						*/
/*                                                                                      */
/* Arguments : (I) uint32_t pageOffset : page offset				 					*/
/*				   uint32_t pageAddr : page address			                            */
/*                 uint32_t* ptr : pointer of buffer write						        */
/*				   uint32_t size: size of buffer write                                  */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*                                                                                      */
/****************************************************************************************/
void EEPROM_Write(uint32_t pageOffset, uint32_t pageAddr, uint32_t* ptr, uint32_t size)
{
	uint32_t i = 0;
	uint32_t *pEepromMem = (uint32_t*)EEPROM_ADDRESS(pageAddr,pageOffset);

	if(size > EEPROM_PAGE_SIZE - pageOffset)
		size = EEPROM_PAGE_SIZE - pageOffset;

	for(i = 0; i < size/4; i++) {
		pEepromMem[i] = ptr[i];
#if AUTOPROG_ON
		Chip_EEPROM_WaitForIntStatus(LPC_EEPROM, EEPROM_INT_ENDOFPROG);
#endif
	}

#if (AUTOPROG_ON == 0)
	Chip_EEPROM_EraseProgramPage(LPC_EEPROM);
#endif
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-Check sum data		                                       					*/
/*                                                                                      */
/* Details  -Check sum data	   	    													*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) uint32_t pageOffset : page offset				 					*/
/*				   uint32_t pageAddr : page address			                            */
/*                 uint32_t numberOfPage : number of page						        */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*                                                                                      */
/****************************************************************************************/
uint8_t EEPROM_Checksum(uint32_t startPageOffset, uint32_t startPageAddr, uint32_t endPageOffset,uint32_t endPageAddr)
{
	uint32_t i = 0;
	uint8_t checksum=0;
	uint32_t *pEepromMem = (uint32_t*)EEPROM_ADDRESS(startPageAddr,startPageOffset);
	uint32_t numberOfByte =(endPageAddr-startPageAddr)*128-(startPageOffset+(128-endPageOffset)) ;
	// for(i = 0; i < (numberOfPage*128-pageOffset); i++) {
	for(i = 0; i < numberOfByte; i++) {
		//  DEBUGOUT("%d = 0x%08x\n",i,pEepromMem[i]);
		checksum+=pEepromMem[i];
	}
	return checksum;
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	EEPROM_SetMode							              				*/
/*                                                                                      */
/* Details:    Write current mode to EEPROM												*/
/*                                                                                      */
/* Arguments:	(I) E_OperationMode mode												*/
/*                                                                                      */
/* ReturnValue : None			                                                        */
/*                                                                                      */
/****************************************************************************************/
void EEPROM_SetMode(E_OperationMode mode)
{
	uint32_t currentMode[4] = {'\0'};
	currentMode[0]=mode;
	switch(mode)
	{
	case eOperationMode:
		DEBUGOUT("Set Operation Mode\n");
		break;
	case eUpdateMode:
		DEBUGOUT("Set Update Mode\n");
		break;
	case ePowerLostMode:
		DEBUGOUT("Set Power Lost Mode\n");
		break;
	case eResetWatchdogTimer:
		DEBUGOUT("Set Reset Watchdog Timer\n");
		break;
	default:
		break;
	}
	EEPROM_Erase(EEPROM_MODE_PAGE);
	EEPROM_Write(EEPROM_MODE_OFFSET,EEPROM_MODE_PAGE,currentMode,4);
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	EEPROM_ReadMode							              				*/
/*                                                                                      */
/* Details:    Read current mode from EEPROM											*/
/*                                                                                      */
/* Arguments:	None																	*/
/*                                                                                      */
/* ReturnValue : E_OperationMode                                                        */
/*                                                                                      */
/****************************************************************************************/
E_OperationMode EEPROM_ReadMode(void)
{
	uint32_t mode = 0;
	EEPROM_Read(EEPROM_MODE_OFFSET,EEPROM_MODE_PAGE,&mode,4);
	return ((E_OperationMode)mode);
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	EEPROM_ClearUpdateFlag					              				*/
/*                                                                                      */
/* Details:    Clear update flag by change current mode to operation mode				*/
/*                                                                                      */
/* Arguments:	None																	*/
/*                                                                                      */
/* ReturnValue : None			                                                        */
/*                                                                                      */
/****************************************************************************************/
void EEPROM_ClearUpdateFlag(void)
{
	EEPROM_SetMode(eOperationMode);
}
