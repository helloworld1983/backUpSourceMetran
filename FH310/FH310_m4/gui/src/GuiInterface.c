/********************************************************************/
/*                                                                  */
/* File Name    : GuiInterface.c                          			*/
/*                                                                  */
/* General      : Contains functions which are GUI interface		*/
/*                 	 						                        */
/*                                                                  */
/* Product Name  : FH310                                            */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2017/07/22       Linh Nguyen(MV)	    new file        */
/*  #0001	   2017/09/07      Linh Nguyen(MV)  	    modify	    */
/*    improve get serial number function design						*/
/*  #0002	   2017/09/19      Viet Le			  	    modify	    */
/*    - Change uint8_t->settingID in guiInterface_SetSettingValue() */
/*			and guiInterface_GetSettingValue() function (bug 1851)	*/
/*  #0003	   2017/09/20       Linh Nguyen(MV)  	modify	    	*/
/*   Use the new realTimeClock_SendRequestSetTime function interface*/
/*  #0004	   2017/09/28      Linh Nguyen(MV)  	    modify	    */
/*   Change guiInterface_RTCSettime function 						*/
/*  #0005	   2017/09/28      Viet Le			  	    modify	    */
/*    - Add guiInterface_Init() function (bug 1923)					*/
/*  #0006	   2017/10/03      Viet Le			  	    modify	    */
/*    - Add guiInterface_CheckSettingRange() function to 			*/
/*			check setting range	(bug 1924)							*/
/*  #0007	   2017/10/03      Viet Le			  	    modify	    */
/*    - Add guiInterface_GetChecksumOfSettings() function to 		*/
/*			read check sum	(bug 1925)								*/
/*  #0008	   2017/10/06      Viet Le			  	    modify	    */
/*	  - Add guiInterface_HandlerPowerLost() handle power lost (bug 1957)*/
/*  #0009	   2017/10/06      Viet Le			  	    modify	    */
/*	  - Add guiInterface_WatchDogReset() watch dog reset (bug 1957)	*/
/*  		   2017/10/07      Linh Nguyen(MV)  	    modify	    */
/*  #0010 add function guiInterface_GetStringLogName for bug #1948	*/
/*  #0011 change function guiInterface_RTCSettime for bug #1938		*/
/*  		   2017/10/28      Linh Nguyen(MV)  	    modify	    */
/* #0010 Change RTC design  (bug #2002)								*/
/********************************************************************/
#include "GuiInterface.h"
#include "setting.h"
#include "EEPROM.h"
#include "AudioMgr.h"
#include "ModeMgr.h"
#include "LogDataScreen.h"

extern QueueHandle_t g_guiQueue;
static Real_Time_Clock gs_currentTime;

/****************************************************************************************/
/*                                                                                      */
/* General 	-guiInterface_setCurrentTime	                                            */
/*                                                                                      */
/* Details  -set Current Time		     												*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) Real_Time_Clock currentTime							 			    */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                                                                                      */
/****************************************************************************************/
void guiInterface_setCurrentTime(Real_Time_Clock currentTime)
{
	if(xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED)	//scheduler is not started yet
	{
		gs_currentTime = currentTime;
	}
	else
	{
		vTaskSuspendAll();
		gs_currentTime = currentTime;
		xTaskResumeAll();
	}
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-guiInterface_Init 			                                            	*/
/*                                                                                      */
/* Details  -Initialize gui interface	     											*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE													 			    */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                                                                                      */
/****************************************************************************************/
void guiInterface_Init(void)
{
	realtimeclock_Init();
	realtimeclock_gettime(&gs_currentTime);
	setting_Init();
	audioMgr_InitAudio();

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-guiInterface_GetStringLogName	                                            */
/*                                                                                      */
/* Details  -Get String Log Name	     												*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) LogId ID												 			    */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : int8_t*	                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                                                                                      */
/****************************************************************************************/
int8_t* guiInterface_GetStringLogName(LogId ID)
{
	return logDataScreen_GetStringLogName(ID);
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-guiInterface_WWDTFeed 			                                            */
/*                                                                                      */
/* Details  -Feed watchdog timer	     												*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) LPC_WWDT_T *pWWDT									 			    */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                                                                                      */
/****************************************************************************************/
void guiInterface_WWDTFeed(LPC_WWDT_T *pWWDT)
{
	if(xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED)	//scheduler is not started yet
	{
		Chip_WWDT_Feed(pWWDT);
	}
	else
	{
		vTaskSuspendAll();
		Chip_WWDT_Feed(pWWDT);
		xTaskResumeAll();
	}
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-guiInterface_RTCGettime 			                                        */
/*                                                                                      */
/* Details  -Get date time		        												*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) Real_Time_Clock *rtc									 			    */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : NONE		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                                                                                      */
/****************************************************************************************/
void guiInterface_RTCGettime (Real_Time_Clock *rtc)
{
	if(xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED)	//scheduler is not started yet
	{
		*rtc = gs_currentTime;
	}
	else
	{
		vTaskSuspendAll();
		*rtc = gs_currentTime;
		xTaskResumeAll();
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-guiInterface_RTCSettime 			                                        */
/*                                                                                      */
/* Details  -Set date time		        												*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) Real_Time_Clock *rtc									 			    */
/*                                                                                      */
/*             (O) E_RTC_EVENT_ID id													*/
/*                                                                                      */
/* ReturnValue : bool		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                                                                                      */
/****************************************************************************************/
bool guiInterface_RTCSettime(Real_Time_Clock *rtc, E_RTC_EVENT_ID id)
{
	return realTimeClock_SendRequestSetTime(rtc, id);
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-guiTask_SendEvent                        				    				*/
/*                                                                                      */
/* Details  -Send data to gui task													    */
/*																						*/
/*                                                                                      */
/* Arguments : (I) GUI_EVENT_STRUCT event												*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : bool			                                                    	*/
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*                                                                                      */
/****************************************************************************************/
bool guiInterface_SendGuiEvent(GUI_EVENT_STRUCT event)
{
	if(xQueueSend(g_guiQueue, &event,QUEUE_WAITING) == pdFALSE)
	{
		DEBUGOUT("\n Failure send");
		return false;
	}
	return true;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-Read data from EEPROM for dump file                                      	*/
/*                                                                                      */
/* Details  -														      				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) uint32_t AddOffset : address offset				 					*/
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
/*                                                                                      */
/****************************************************************************************/
void guiInterface_ReadEEPROMForDump(uint32_t AddOffset, uint32_t* ptr)
{
	if(xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED)	//scheduler is not started yet

	{
		EEPROM_ReadForDump(AddOffset, ptr);
	}
	else
	{
		vTaskSuspendAll();
		EEPROM_ReadForDump(AddOffset, ptr);
		xTaskResumeAll();
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-write data to EEPROM for upload function                                  	*/
/*                                                                                      */
/* Details  -														      				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) uint32_t AddOffset : address offset				 					*/
/*                 uint32_t* EEPInt : pointer of buffer read					        */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*                                                                                      */
/****************************************************************************************/
void guiInterface_EEPROMWriteForUpload(uint32_t AddOffset, uint8_t* EEPInt)
{
	if(xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED)	//scheduler is not started yet
	{
		EEPROM_WriteForUpload(AddOffset, EEPInt);
	}
	else
	{
		vTaskSuspendAll();
		EEPROM_WriteForUpload(AddOffset, EEPInt);
		xTaskResumeAll();
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-Erase a page in EEPROM		                                            	*/
/*                                                                                      */
/* Details  -Erase a page in EEPROM                                      				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) uint32_t pageAddr : page address					 					*/
/*             (I) uint32_t pageOffset													*/
/*             (O) uint32_t size														*/
/*             (O) uint32_t* ptr														*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                                                                                      */
/****************************************************************************************/
void guiInterface_ReadEEPROM(uint32_t pageOffset, uint32_t pageAddr, uint32_t* ptr, uint32_t size)
{
	if(xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED)	//scheduler is not started yet
	{
		EEPROM_Read(pageOffset, pageAddr, ptr, size);
	}
	else
	{
		vTaskSuspendAll();
		EEPROM_Read(pageOffset, pageAddr, ptr, size);
		xTaskResumeAll();
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-Erase  EEPROM				                                            	*/
/*                                                                                      */
/* Details  -Erase  EEPROM      	 		                               				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) uint32_t pageAddr 													*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                                                                                      */
/****************************************************************************************/
void guiInterface_EraseEEPROM(uint32_t pageAddr)
{
	if(xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED)	//scheduler is not started yet
	{
		EEPROM_Erase(pageAddr);
	}
	else
	{
		vTaskSuspendAll();
		EEPROM_Erase(pageAddr);
		xTaskResumeAll();
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
/* Arguments : (I) uint32_t pageOffset : page offset				 					*/
/*				   uint32_t pageAddr : page address			                            */
/*                 uint32_t* ptr : pointer of buffer write						        */
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
/* local valiables   : NONE			                                                  	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void guiInterface_EEPROMWrite(uint32_t pageOffset, uint32_t pageAddr, uint32_t* ptr, uint32_t size)
{
	if(xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED)	//scheduler is not started yet
	{
		EEPROM_Write(pageOffset, pageAddr, ptr, size);
	}
	else
	{
		vTaskSuspendAll();
		EEPROM_Write(pageOffset, pageAddr, ptr, size);
		xTaskResumeAll();
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-Check sum data		                                       					*/
/*                                                                                      */
/* Details  -Check sum data	   	    													*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) uint32_t startPage 								 					*/
/*				   uint32_t endPage 						                            */
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                                                                                      */
/****************************************************************************************/
uint8_t guiInterface_EEPROMChecksum(uint32_t startPage, uint32_t endPage)
{
	uint8_t cs = 0;
	if(xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED)	//scheduler is not started yet
	{
		cs = EEPROM_Checksum(startPage, endPage);
	}
	else
	{
		vTaskSuspendAll();
		cs = EEPROM_Checksum(startPage, endPage);
		xTaskResumeAll();
	}
	return cs;
}
/****************************************************************************************/
/*                                                                                      */
/* General 	-guiInterface_SetSerialNumber	                                       		*/
/*                                                                                      */
/* Details  -Set Serial Number	    													*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) int8_t *bufferSerial								 					*/
/*                                                                                      */
/* ReturnValue : 					l                                                   */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                                                                                      */
/****************************************************************************************/
void guiInterface_SetSerialNumber(int8_t *bufferSerial)
{
	if(xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED)	//scheduler is not started yet
	{
		setting_SetSerialNumber(bufferSerial);
	}
	else
	{
		vTaskSuspendAll();
		setting_SetSerialNumber(bufferSerial);
		xTaskResumeAll();
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-guiInterface_GetSerialNumber	                                       		*/
/*                                                                                      */
/* Details  -Get Serial Number	    													*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) int8_t *bufferSerial								 					*/
/*                                                                                      */
/* ReturnValue : 		int8_t*			                                                */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                                                                                      */
/****************************************************************************************/
void guiInterface_GetSerialNumber(int8_t* SerialNumber)
{
	if(xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED)	//scheduler is not started yet
	{
		setting_GetSerialNumber(SerialNumber);
	}
	else
	{
		vTaskSuspendAll();
		setting_GetSerialNumber(SerialNumber);
		xTaskResumeAll();
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-guiInterface_SetNumberOfLogStored	                                       	*/
/*                                                                                      */
/* Details  -Set Number Of Log Stored  													*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) uint32_t numberOfLogStored						 					*/
/*                                                                                      */
/* ReturnValue : 		None			                                                */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                                                                                      */
/****************************************************************************************/
void guiInterface_SetNumberOfLogStored(uint32_t numberOfLogStored)
{

	if(xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED)	//scheduler is not started yet
	{
		setting_SetNumberOfLogStored(numberOfLogStored);
	}
	else
	{
		vTaskSuspendAll();
		setting_SetNumberOfLogStored(numberOfLogStored);
		xTaskResumeAll();
	}
}
/****************************************************************************************/
/*                                                                                      */
/* General 	-guiInterface_GetNumberOfLogStored	                                       	*/
/*                                                                                      */
/* Details  -Get Number Of Log Stored  													*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) None						 											*/
/*                                                                                      */
/* ReturnValue : 		uint32_t		                                                */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                                                                                      */
/****************************************************************************************/
uint32_t guiInterface_GetNumberOfLogStored(void)
{
	uint32_t NumberOfLog=0;
	if(xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED)	//scheduler is not started yet
	{
		NumberOfLog = setting_GetNumberOfLogStored();
	}
	else
	{
		vTaskSuspendAll();
		NumberOfLog = setting_GetNumberOfLogStored();
		xTaskResumeAll();
	}
	return NumberOfLog;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-guiInterface_SetAnypalInfo			                                       	*/
/*                                                                                      */
/* Details  -Set Anypal Info		  													*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) int8_t *equipmentName	 											*/
/*             (I) int8_t* commodityCode                                                */
/*             (I) int8_t* version                                                      */
/*             (I) int8_t* numberID                                                     */
/*                                                                                      */
/* ReturnValue : 		None			                                                */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                                                                                      */
/****************************************************************************************/
void guiInterface_SetAnypalInfo(int8_t* equipmentName, int8_t* commodityCode, int8_t* version, int8_t* numberID)
{
	if(xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED)	//scheduler is not started yet
	{
		setting_SetAnypalInfo(equipmentName, commodityCode, version, numberID);
	}
	else
	{
		vTaskSuspendAll();
		setting_SetAnypalInfo(equipmentName, commodityCode, version, numberID);
		xTaskResumeAll();
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-guiInterface_GetAnypalInfo			                                       	*/
/*                                                                                      */
/* Details  -Get Anypal Info		  													*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) None				                                                    */
/*                                                                                      */
/* ReturnValue : 		AnypalInfor		                                                */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                                                                                      */
/****************************************************************************************/
AnypalInfor guiInterface_GetAnypalInfo(void)
{
	AnypalInfor info;
	if(xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED)	//scheduler is not started yet
	{
		info = setting_GetAnypalInfo();
	}
	else
	{
		vTaskSuspendAll();
		info = setting_GetAnypalInfo();
		xTaskResumeAll();
	}
	return info;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-guiInterface_SetCumulativeHour		                                       	*/
/*                                                                                      */
/* Details  -This operation Set Cumulative hour to an indicated Cumulative id			*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) AllCumulativeHour id		 											*/
/*             (I) uint32_t hour		                                                */
/*             (I) uint8_t min		                                                    */
/*             (I) uint8_t sec		                                                    */
/*                                                                                      */
/* ReturnValue : 		None			                                                */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                                                                                      */
/****************************************************************************************/
void guiInterface_SetCumulativeHour(AllCumulativeHour id, uint32_t hour, uint8_t min, uint8_t sec)
{
	if(xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED)	//scheduler is not started yet
	{
		setting_SetCumulativeHour(id, hour, min, sec);
	}
	else
	{
		vTaskSuspendAll();
		setting_SetCumulativeHour(id, hour, min, sec);
		xTaskResumeAll();
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-guiInterface_GetCumulativeHour			                                    */
/*                                                                                      */
/* Details  -This operation get Cumulative hour to an indicated Cumulative id			*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) AllCumulativeHour id		                                            */
/*                                                                                      */
/* ReturnValue : 		CumulativeHourTime		                                        */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                                                                                      */
/****************************************************************************************/
CumulativeHourTime guiInterface_GetCumulativeHour(AllCumulativeHour id)
{
	CumulativeHourTime time;
	if(xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED)	//scheduler is not started yet
	{
		time = setting_GetCumulativeHour(id);
	}
	else
	{
		vTaskSuspendAll();
		time = setting_GetCumulativeHour(id);
		xTaskResumeAll();
	}
	return time;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-guiInterface_SetSettingValue		                                       	*/
/*                                                                                      */
/* Details  -This operation set the temp setting value in ram to an indicated setting id*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) settingID id				 											*/
/*             (I) uint8_t value		                                                */
/*                                                                                      */
/* ReturnValue : 		None			                                                */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                                                                                      */
/****************************************************************************************/
void guiInterface_SetSettingValue(settingID id, uint8_t value)
{
	if(xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED)	//scheduler is not started yet
	{
		setting_SetSettingValue(id, value);
	}
	else
	{
		vTaskSuspendAll();
		setting_SetSettingValue(id, value);
		xTaskResumeAll();
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-guiInterface_GetSettingValue		                                       	*/
/*                                                                                      */
/* Details  -This operation get the temp setting value in ram to an indicated setting id*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) settingID id				 											*/
/*                                                                                      */
/* ReturnValue : 		uint8_t			                                                */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                                                                                      */
/****************************************************************************************/
uint8_t guiInterface_GetSettingValue(settingID id)
{
	uint8_t value =0;
	if(xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED)	//scheduler is not started yet
	{
		value = setting_Get(id);
	}
	else
	{
		vTaskSuspendAll();
		value = setting_Get(id);
		xTaskResumeAll();
	}
	return value;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-guiInterface_CheckSettingRange						                        */
/*                                                                                      */
/* Details  -Check range all setting	 											    */
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE													            	*/
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : bool : if out of range is false, else is true	             			*/
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              											    */
/*                                                                                      */
/****************************************************************************************/
bool guiInterface_CheckSettingRange(void)
{
	bool result = false;

	if(xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED)	//scheduler is not started yet
	{
		if(setting_Checkrange() == false)
		{
			result = true;
		}
		else
		{
			result = false;
		}
	}
	else
	{
		vTaskSuspendAll();
		if(setting_Checkrange() == false)
		{
			result = true;
		}
		else
		{
			result = false;
		}
		xTaskResumeAll();
	}

	return result;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-guiInterface_GetChecksumOfSettings						                    */
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
uint8_t guiInterface_GetChecksumOfSettings(void)
{
	uint32_t cs = 0;

	if(xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED)	//scheduler is not started yet
	{
		EEPROM_Read(PAGE_OFFSET_SAVE_CHECKSUM, PAGE_ADDRESS_SAVE_CHECKSUM, (uint32_t*)&cs, SIZE_SAVE_CHECKSUM);
	}
	else
	{
		vTaskSuspendAll();
		EEPROM_Read(PAGE_OFFSET_SAVE_CHECKSUM, PAGE_ADDRESS_SAVE_CHECKSUM, (uint32_t*)&cs, SIZE_SAVE_CHECKSUM);
		xTaskResumeAll();
	}

	return (uint8_t)cs;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-guiInterface_HandlerPowerLost						                    	*/
/*                                                                                      */
/* Details  -Handler power lost															*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE					 					  							*/
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void							                                        */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (O)  NONE														*/
/*					   (I)  NONE														*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void guiInterface_HandlerPowerLost(void)
{
	modeMgr_HandlerPowerLost();

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-guiInterface_WatchDogReset 								    			*/
/*                                                                                      */
/* Details  -Watch dog reset															*/
/*			  														                    */
/*                                                                                      */
/* Arguments : (I) NONE																	*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void				                                                    */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void guiInterface_WatchDogReset(void)
{
	modeMgr_WatchDogReset();

	return;
}

/* end of files */
