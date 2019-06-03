/********************************************************************/
/*                                                                  */
/* File Name    : EEPROM.c                                    		*/
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

#include "board.h"
#include "string.h"
#include "TaskCommon.h"
#include "GuiInterface.h"
#include "UsbMgr.h"
#include "EEPROM.h"

/* Auto programming on/off */
#define AUTOPROG_ON     0


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
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void EEPROM_Init(void)
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

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	- EEPROM_ReadForDump				                                     	*/
/*                                                                                      */
/* Details  -Read data from EEPROM for dump file										*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) uint32_t AddOffset : - address offset				 				*/
/*                   - range : 0-0x4000                                                 */
/*                   - unit : NONE                                                      */
/*                                                                                      */
/*                 uint32_t* ptr : pointer of buffer read						        */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void EEPROM_ReadForDump(uint32_t AddOffset, uint32_t* ptr)
{
	uint32_t *pEepromMem = (uint32_t*)(EEPROM_START+AddOffset);
	for(uint32_t i = 0; i < DUM_SIZE; i++)
	{
		ptr[i] = pEepromMem[i];
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-EEPROM_WriteForUpload					                                 	*/
/*                                                                                      */
/* Details  -write data to EEPROM for upload function				      				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) uint32_t AddOffset : address offset				 					*/
/*                   - range : 0-0x4000                                                 */
/*                   - unit : NONE                                                      */
/*                                                                                      */
/*                 uint32_t* ptr : pointer of buffer read						        */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void EEPROM_WriteForUpload(uint32_t AddOffset, uint8_t* EEPInt)
{
	uint32_t *pEepromMem = (uint32_t*)(AddOffset + EEPROM_START);
	uint32_t *data = (uint32_t*)EEPInt;
	for (int i =0; i<(ONE_LINE_SIZE/sizeof(uint32_t)); i++)
	{
		pEepromMem[i] = data[i];
#if AUTOPROG_ON
		Chip_EEPROM_WaitForIntStatus(LPC_EEPROM, EEPROM_INT_ENDOFPROG);
#endif
	}
#if (AUTOPROG_ON == 0)
	Chip_EEPROM_EraseProgramPage(LPC_EEPROM);
#endif
	return;
}


/****************************************************************************************/
/*                                                                                      */
/* General 	-Read data from EEPROM		                                            	*/
/*                                                                                      */
/* Details  -Read data from EEPROM size must be multiple of 4 bytes      				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) uint32_t pageOffset : - page offset				 					*/
/*                     - range : 0-127                                                  */
/*                     - unit : NONE                                                    */
/*                                                                                      */
/*				   uint32_t pageAddr : page address			                            */
/*                     - range : 0-127                                                  */
/*                     - unit : NONE                                                    */
/*                                                                                      */
/*                 uint32_t* ptr : pointer of buffer read						        */
/*                                                                                      */
/*				   uint32_t size: size of buffer read                                   */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void EEPROM_Read(uint32_t pageOffset, uint32_t pageAddr, uint32_t* ptr, uint32_t size)
{
	uint32_t i = 0;
	uint32_t *pEepromMem = (uint32_t*)EEPROM_ADDRESS(pageAddr,pageOffset);
	for(i = 0; i < size/4; i++) {
		ptr[i] = pEepromMem[i];
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-EEPROM_Erase	                                            				*/
/*                                                                                      */
/* Details  -Erase a page in EEPROM                                      				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) uint32_t pageAddr : page address					 					*/
/*				   		- range : 0-127                           						*/
/*                      - unit : NONE                                                   */
/*				   		                           									    */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void EEPROM_Erase(uint32_t pageAddr)
{
	uint32_t i = 0;
	uint32_t *pEepromMem = (uint32_t*)EEPROM_ADDRESS(pageAddr,0);
	for(i = 0; i < EEPROM_PAGE_SIZE/4; i++) {
		pEepromMem[i] = 0xFFFFFFFF;
#if AUTOPROG_ON
		Chip_EEPROM_WaitForIntStatus(LPC_EEPROM, EEPROM_INT_ENDOFPROG);
#endif
	}
#if (AUTOPROG_ON == 0)
	Chip_EEPROM_EraseProgramPage(LPC_EEPROM);
#endif
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-EEPROM_AllErase			                                            	*/
/*                                                                                      */
/* Details  -Erase all EEPROM       		                               				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE				 													*/
/*				   		                           									    */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void EEPROM_AllErase(void)
{
	for(int i = 0; i < EEPROM_PAGE_NUM-1; i++)
	{
		guiInterface_EraseEEPROM(i);
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-Write data to a page in EEPROM		                                        */
/*                                                                                      */
/* Details  -Write data to a page in EEPROM size must be multiple of 4 bytes    	    */
/*																						*/
/*                                                                                      */
/* Arguments : (I) uint32_t pageOffset : - page offset				 					*/
/*                     - range : 0-127                                                  */
/*                     - unit : NONE                                                    */
/*                                                                                      */
/*				   uint32_t pageAddr : page address			                            */
/*                     - range : 0-127                                                  */
/*                     - unit : NONE                                                    */
/*                                                                                      */
/*                 uint32_t* ptr : pointer of buffer write						        */
/*                                                                                      */
/*				   uint32_t size: size of buffer write                                  */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void EEPROM_Write(uint32_t pageOffset, uint32_t pageAddr, uint32_t* ptr, uint32_t size)
{
	uint32_t i = 0;
	uint32_t *pEepromMem = (uint32_t*)EEPROM_ADDRESS(pageAddr,pageOffset);

	if(size > EEPROM_PAGE_SIZE - pageOffset)
		size = EEPROM_PAGE_SIZE - pageOffset;

	for(i = 0; i < size/4; i++)
	{
		pEepromMem[i] = ptr[i];
#if AUTOPROG_ON
		Chip_EEPROM_WaitForIntStatus(LPC_EEPROM, EEPROM_INT_ENDOFPROG);
#endif
	}

#if (AUTOPROG_ON == 0)
	Chip_EEPROM_EraseProgramPage(LPC_EEPROM);
#endif
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-EEPROM_Checksum	                                       					*/
/*                                                                                      */
/* Details  -Check sum data	   	    													*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) uint32_t startPage : - start page								 	*/
/*                    - range : 0-127                                                   */
/*                    - unit : NONE                                                     */
/*                                                                                      */
/*				   uint32_t endPage : - end page 						                */
/*                    - range : 0-127                                                   */
/*                    - unit : NONE                                                     */
/*                                                                                      */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : uint8_t : byte check sum		                                        */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
uint8_t EEPROM_Checksum(uint32_t startPage, uint32_t endPage)
{
	uint32_t i = 0;
	uint8_t checksum=0;
	uint8_t *pEepromMem = (uint8_t*)EEPROM_ADDRESS(0,startPage);
	uint32_t numberOfByte =(endPage-startPage+1)*128;

	for(i = 0; i < numberOfByte-4; i++)
	{
		checksum^=(*pEepromMem);
		pEepromMem++;
	}
	return checksum;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-EEPROM_ReadChecksum						                				*/
/*                                                                                      */
/* Details  -Read check sum from EEPROM													*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE					 					  							*/
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : uint8_t : byte checksum		                                        */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (O)  NONE														*/
/*					   (I)  NONE														*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
uint8_t EEPROM_ReadChecksum(void)
{
	uint32_t cs=0;
	guiInterface_ReadEEPROM(PAGE_OFFSET_SAVE_CHECKSUM, PAGE_ADDRESS_SAVE_CHECKSUM, (uint32_t*)&cs, SIZE_SAVE_CHECKSUM);
	return (uint8_t)cs;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-EEPROM_WriteChecksum						                				*/
/*                                                                                      */
/* Details  -Write check sum to EEPROM													*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) uint8_t checksum	: - byte checksum				 					*/
/*                   - range : 0-255                                                    */
/*                   - unit : NONE                                                      */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : NONE		                                                        	*/
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (O)  NONE														*/
/*					   (I)  NONE														*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void EEPROM_WriteChecksum(uint8_t checksum)
{
	uint32_t cs = (uint32_t)(0x000000ff & checksum);
	guiInterface_EEPROMWrite(PAGE_OFFSET_SAVE_CHECKSUM, PAGE_ADDRESS_SAVE_CHECKSUM, (uint32_t*)&cs, SIZE_SAVE_CHECKSUM);

	return;
}

/* end of files */

