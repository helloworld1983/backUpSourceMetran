#ifndef INC_GUIINTERFACE_H_
#define INC_GUIINTERFACE_H_
/********************************************************************/
/*                                                                  */
/* File Name    : GuiInterface.h                          			*/
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
/*  #0003	   2017/09/28      Linh Nguyen(MV)  	    modify	    */
/*    Change guiInterface_RTCSettime function 						*/
/*  #0004	   2017/09/28      Viet Le			  	    modify	    */
/*    - Add guiInterface_Init() function (bug 1923)					*/
/*  #0005	   2017/10/03      Viet Le			  	    modify	    */
/*    - Add guiInterface_CheckSettingRange() function to 			*/
/*			check setting range	(bug 1924)							*/
/*  #0006	   2017/10/03      Viet Le			  	    modify	    */
/*    - Add guiInterface_GetChecksumOfSettings() function to 		*/
/*			read check sum	(bug 1925)								*/
/*  #0007	   2017/10/06      Viet Le			  	    modify	    */
/*	  - Add guiInterface_HandlerPowerLost() handle power lost (bug 1957)*/
/*  #0008	   2017/10/06      Viet Le			  	    modify	    */
/*	  - Add guiInterface_WatchDogReset() watch dog reset (bug 1957)	*/
/*  		   2017/10/07      Linh Nguyen(MV)  	    modify	    */
/*  #0009 add function guiInterface_GetStringLogName for bug #1948	*/
/*  #0010 change function guiInterface_RTCSettime for bug #1938		*/
/*  		   2017/10/28      Linh Nguyen(MV)  	    modify	    */
/* 	#0011 Change RTC design  (bug #2002)							*/
/********************************************************************/
#include "setting.h"
#include "RealTimeClock.h"
#include "GuiTask.h"
#include "board.h"
#define QUEUE_WAITING 							500/portTICK_PERIOD_MS
#define SERIAL_NUMBER_BUFFER_SIZE 9

void guiInterface_setCurrentTime(Real_Time_Clock currentTime);
void guiInterface_Init(void);
void guiInterface_SetSerialNumber(int8_t *bufferSerial);
void guiInterface_GetSerialNumber(int8_t* SerialNumber);
void guiInterface_SetNumberOfLogStored(uint32_t numberOfLogStored);
uint32_t guiInterface_GetNumberOfLogStored(void);
void guiInterface_SetAnypalInfo(int8_t* equipmentName, int8_t* commodityCode, int8_t* version, int8_t* numberID);
AnypalInfor guiInterface_GetAnypalInfo(void);
void guiInterface_SetCumulativeHour(AllCumulativeHour id, uint32_t hour, uint8_t min, uint8_t sec);
CumulativeHourTime guiInterface_GetCumulativeHour(AllCumulativeHour id);
void guiInterface_SetSettingValue(settingID id, uint8_t value);
uint8_t guiInterface_GetSettingValue(settingID id);
void guiInterface_ReadEEPROMForDump(uint32_t AddOffset, uint32_t* ptr);
void guiInterface_EEPROMWriteForUpload(uint32_t AddOffset, uint8_t* EEPInt);
void guiInterface_ReadEEPROM(uint32_t pageOffset, uint32_t pageAddr, uint32_t* ptr, uint32_t size);
void guiInterface_EraseEEPROM(uint32_t pageAddr);
void guiInterface_EEPROMWrite(uint32_t pageOffset, uint32_t pageAddr, uint32_t* ptr, uint32_t size);
uint8_t guiInterface_EEPROMChecksum(uint32_t startPage, uint32_t endPage);
bool guiInterface_SendGuiEvent(GUI_EVENT_STRUCT event);

void guiInterface_RTCGettime (Real_Time_Clock *rtc);
bool guiInterface_RTCSettime(Real_Time_Clock *rtc, E_RTC_EVENT_ID id);

void guiInterface_WWDTFeed(LPC_WWDT_T *pWWDT);
bool guiInterface_CheckSettingRange(void);
uint8_t guiInterface_GetChecksumOfSettings(void);
void guiInterface_HandlerPowerLost(void);
void guiInterface_WatchDogReset(void);
int8_t* guiInterface_GetStringLogName(LogId ID);

#endif /* INC_GUIINTERFACE_H_ */
