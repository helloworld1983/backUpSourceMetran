/********************************************************************/
/*                                                                  */
/* File Name    : UsbMgr.c			                           		*/
/*                                                                  */
/* General       : Init and handle USB control                      */
/*                 	 						                        */
/*                                                                  */
/* Product Name  : FH310                                            */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2016/08/22      Quyen Ngo		  	new file        */
/* #0001       2017/08/29      Linh Nguyen		  	remove unuse code*/
/* #0002	   2017/09/01		Quyen Ngo			modify			*/
/*		-remove assert												*/
/* #0003	   2017/09/01		Viet Le			modify				*/
/*      -remove usbMgr_ReadDebugLog() function                      */
/*      -remove usbMgr_ReadNewLog() function                        */
/*      -edit debugLog_WriteLogToFile() function                    */
/*      -add usbMgr_DumpAllLog() function                           */
/*      -edit usbMgr_ExportDebugLog() function                      */
/*  #0004	   2017/09/07      Linh Nguyen(MV)  	    modify	    */
/*    use  the new get serial number function design				*/
/*  #0005	   2017/09/12      Linh Nguyen(MV)  	    modify	    */
/*    Fix timer text name											*/
/*  Use new save log function                            			*/
/*                                                                  */
/*	#0006	   2017/09/11	   Quyen Ngo(MV)			modify		*/
/*	   -change arument of f_mount function for adapt with new update*/
/*		of fatfs R0.11a												*/
/*  #0007	   2017/09/20      Linh Nguyen(MV)  	    modify	    */
/*  Use the new get log function									*/
/* #0008	   2017/09/21		Viet Le			modify				*/
/*	   -make static variable to save these data inside of USB function*/
/*			static int8_t s_m4_filename[];							*/
/*			static int8_t s_m0_filename[];							*/
/*			static int8_t s_boot_filename[];(bug 1847)				*/
/*     -Interface to get these data 								*/
/*			usbMgr_getFirmwareFilesBoot(void)						*/
/*			usbMgr_getFirmwareFilesControlSW(void) (bug 1847)		*/
/*  #0009	   2017/09/24      Linh Nguyen(MV)  	    modify	    */
/*  Remove unuse code												*/
/*  Change function name for coding rule					     	*/
/* #0010       2017/09/27      Viet Le		  		modify			*/
/*   - Remove usbMgr_getFirmwareFilesControlSW() function (bug 1847)*/
/*	 - Add two function : usbMgr_getFirmwareFilesM0(),				*/
/*			usbMgr_getFirmwareFilesM4()	(bug 1847)					*/
/* #0011       2017/10/03      Viet Le		  		modify			*/
/*   - initialize variables before start checking files in 			*/
/*		usbMgr_CheckFWFile() function (bug 1847)				    */
/* 		       2017/10/07      Linh Nguyen	  		modify			*/
/* #0012 Change the flowrate setting in upload memory version for	*/
/*				bug #1960											*/
/* #0013       2017/10/18      Viet Le		  		modify			*/
/*	 - Edit Prefix debug log : DEBUG_LOW to DEBUG_LOG (bug 1978)	*/
/*  		   2017/10/28      Linh Nguyen(MV)  	    modify	    */
/* #0014 Add the dump SPI flash memory functionality (bug #1992)	*/
/* #0015       2017/11/20      Viet Le		  		modify			*/
/*  Check return value in usbMgr_PrepareCSVFile() function (Coverity)*/
/********************************************************************/

#include <UsbMgr.h>
#include "board.h"
#include "fsusb_cfg.h"
#include "ff.h"
#include "MsHost.h"
#include "HidBarcodeHost.h"
#include "TaskCommon.h"
#include "timers.h"
#include "RealTimeClock.h"
#include "stdlib.h"
#include "stringtools.h"
#include "updateFlash.h"
#include "LogDataScreen.h"
#include "GuiTask.h"
#include "util.h"
#include "LogMgr.h"
#include "setting.h"
#include "DebugLog.h"
#include "ExternalEeprom.h"
#include "AnypalInterface.h"
#include "UsbInterface.h"
#include "AnypalHandler.h"
#include "FtdiAnypalHost.h"
#include "EEPROM.h"
#include "GuiInterface.h"
#include "LogInterface.h"
#include "UsbHostInterface.h"


#ifdef DEBUG
#define DEBUG_USB_EN
#endif

#ifdef DEBUG_USB_EN
#define DEBUG_USB(...) printf(__VA_ARGS__)
#else
#define DEBUG_USB(...)
#endif

#define POWER_USB1_EN_PORT_NUM			5
#define POWER_USB1_EN_BIT_NUM			18
#define POWER_USB0_EN_PORT_NUM			5
#define POWER_USB0_EN_BIT_NUM			3
#define USB0_PWR_FAULT_PORT				4
#define USB0_PWR_FAULT_BIT				0
#define USB1_PWR_FAULT_PORT				4
#define USB1_PWR_FAULT_BIT				11
#define NO_WAIT 0

#define MAX_FILENAME_LENG 50
#define MAX_DATA_LENG 18

bool isMsHostEnumurated = false;

extern FATFS fatFS;	/* File system object */
STATIC FIL fileObj;	/* File object */

static int8_t s_m4_filename[FIRMWARE_NAME_ARRAY_SIZE];
static int8_t s_m0_filename[FIRMWARE_NAME_ARRAY_SIZE];
static int8_t s_boot_filename[FIRMWARE_NAME_ARRAY_SIZE];

#define VBUS0_TIMER_ID 		1
#define VBUS1_TIMER_ID 		2
#define TURN_ON_ANYPAL_TIMER_ID 3

#ifndef pdMS_TO_TICKS
#define pdMS_TO_TICKS( xTimeInMs ) ( ( TickType_t ) ( ( ( TickType_t ) ( xTimeInMs ) * ( TickType_t ) configTICK_RATE_HZ ) / ( TickType_t ) 1000 ) )
#endif

#define VBUS_TURN_OFF_TIME 		   2500

extern QueueHandle_t USBHandleQueue;

static TimerHandle_t s_vBus0OffTimer;
static TimerHandle_t s_vBus1OffTimer;
TimerHandle_t g_turnOnAnypalTimer;

static bool s_usb0OverCurrent = false;
static bool s_usb1OverCurrent = false;

static const float gs_flowRateST[] ={0, 0.25, 0.5, 0.75, 1.00, 1.25, 1.50, 1.75, 2.00, 2.5, 3.00};
/* Define a callback function that will be used by multiple timer
 instances. */
static void usbMgr_TimerCallback( TimerHandle_t xTimer )
{
	uint32_t timerId =  ( uint32_t ) pvTimerGetTimerID( xTimer );
	switch (timerId) {
	case VBUS0_TIMER_ID:
	{
		usbMgr_TurnOnVBUS(ANYPAL_PORT);
		s_usb0OverCurrent = false;
		anypalInterface_SendEvent(eAnypalOverCurrentClear);
		break;
	}
	case VBUS1_TIMER_ID:
		usbMgr_TurnOnVBUS(USB_PORT);
		s_usb1OverCurrent = false;
		break;
	case TURN_ON_ANYPAL_TIMER_ID:
		usbMgr_TurnOnVBUS(USB_PORT);
		break;
	default:
		DEBUGOUT("Invalid timer id\n");
		break;
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name usbMgr_TurnOffVBUSAlarmOverCurrent                   	                */
/*                                                                                      */
/* Details  Turn Off VBUS Alarm Over Current	    								    */
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE					 					  							*/
/*																						*/
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
void usbMgr_TurnOffVBUSAlarmOverCurrent(void)
{
	usbMgr_TurnOffVBUS(ANYPAL_PORT);
	//Start timer 2.5S for turn on
	if (xTimerStart(s_vBus0OffTimer,TIMER_START_STOP_WAIT) != pdPASS) {
		DEBUG_USB("ERR: Failed to start s_vBus0OffTimer \n");
	}
	return;
}


/******************************************************************************/
//COMMON.OPERATION
//    Function Name: usbMgr_DetectOverCurrent
//
//    Detail: Detect over current, PWR_FAULT Normal high, over current low
//
//    Arguments: (I)
//				 (O)
//
//    Return Values: NONE
//
//
//    Using Global Data : (I)
//						  (O)
//
//
//    Note: None
//
/******************************************************************************/
static void usbMgr_DetectOverCurrent(void)
{
	if(s_usb0OverCurrent == false)
	{
		if(Chip_GPIO_GetPinState(LPC_GPIO_PORT, USB0_PWR_FAULT_PORT, USB0_PWR_FAULT_BIT) == false)
		{
			anypalInterface_SendEvent(eAnypalOverCurrentEvent);
			DEBUG_USB("detect over current\n");
			//TODO
			s_usb0OverCurrent = true;
		}
	}

	if(s_usb1OverCurrent == false)
	{
		if(Chip_GPIO_GetPinState(LPC_GPIO_PORT, USB1_PWR_FAULT_PORT, USB1_PWR_FAULT_BIT) == false)
		{
			DEBUG_USB("detect over current port 1\n");
			//TODO
			s_usb1OverCurrent = true;
			usbMgr_TurnOffVBUS(USB_PORT);
			//Start timer 2.5S for turn on
			if (xTimerStart(s_vBus1OffTimer,TIMER_START_STOP_WAIT) != pdPASS) {
				DEBUG_USB("ERR: Failed to start s_vBus1OffTimer \n");
			}
			logInterface_WriteMacineLog(guiInterface_GetSettingValue(eFlowRateSettingId), eOverCurrentIsDetectedLogId);
		}
	}
	return;
}

/******************************************************************************/
//COMMON.OPERATION
//    Function Name: usbMgr_DetectOverCurrent
//
//    Detail: Detect over current, PWR_FAULT Normal high, over current low
//
//    Arguments: (I)
//				 (O)
//
//    Return Values: NONE
//
//
//    Using Global Data : (I)
//						  (O)
//
//
//    Note: None
//
/******************************************************************************/
bool usbMgr_GetOverCurrentStatus(uint8_t portnum)
{
	bool anypalOverCurrent;
	if(portnum == ANYPAL_PORT)
	{
		anypalOverCurrent = s_usb0OverCurrent;
	}
	else
	{
		anypalOverCurrent = s_usb1OverCurrent;
	}
	return anypalOverCurrent;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	gpio_ConfigUsb					             					    */
/*                                                                                      */
/* Details:         Config power port for USB1						    				*/
/*                                                                                      */
/*                                                                                      */
/* ReturnValue :    void                                     		                    */
/*                                                                                      */
/* Using Global Data : None				                          						*/
/*                                                                                      */
/****************************************************************************************/
static void usbMgr_ConfigPort(void)
{
	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT,POWER_USB1_EN_PORT_NUM, POWER_USB1_EN_BIT_NUM);		/* output */
	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT,POWER_USB0_EN_PORT_NUM, POWER_USB0_EN_BIT_NUM);     /*USB 0*/

	usbMgr_TurnOffVBUS(ANYPAL_PORT);
	usbMgr_TurnOffVBUS(USB_PORT);
	usbMgr_TurnOnVBUS(ANYPAL_PORT);
	usbMgr_TurnOnVBUS(USB_PORT);
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name usbMgr_TurnOnVBUS                                   	                */
/*                                                                                      */
/* Details  Turn ON USB Vbus signal, when power signal on/off VBUS is ON/OFF            */
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE					 					  							*/
/*																						*/
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
void usbMgr_TurnOnVBUS(uint8_t portnum)
{
	if(portnum == USB_PORT)
	{
		Chip_GPIO_SetPinOutHigh(LPC_GPIO_PORT,POWER_USB1_EN_PORT_NUM, POWER_USB1_EN_BIT_NUM);
	}
	else if(portnum == ANYPAL_PORT)
	{
		Chip_GPIO_SetPinOutHigh(LPC_GPIO_PORT,POWER_USB0_EN_PORT_NUM, POWER_USB0_EN_BIT_NUM);
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name usbMgr_TurnOffVBUS                                   	                */
/*                                                                                      */
/* Details  Turn OFF USB Vbus signal, when power signal on/off VBUS is ON/OFF           */
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE					 					  							*/
/*																						*/
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
void usbMgr_TurnOffVBUS(uint8_t portnum)
{
	if(portnum == USB_PORT)
	{
		Chip_GPIO_SetPinOutLow(LPC_GPIO_PORT,POWER_USB1_EN_PORT_NUM, POWER_USB1_EN_BIT_NUM);
	}
	else if(portnum == ANYPAL_PORT)
	{
		Chip_GPIO_SetPinOutLow(LPC_GPIO_PORT,POWER_USB0_EN_PORT_NUM, POWER_USB0_EN_BIT_NUM);
	}
	return;
}

/******************************************************************************/
//COMMON.OPERATION
//    Function Name: usbMgr_CreateTimer
//
//    Detail:Timer for turnoff VBUS 2.5S
//
//    Arguments: (I)
//				 (O)
//
//    Return Values: NONE
//
//
//    Using Global Data : (I)
//						  (O)
//
//
//    Note: None
//
/******************************************************************************/
void usbMgr_CreateTimer(void)
{
	s_vBus0OffTimer = xTimerCreate
			( /* Just a text name, not used by the RTOS
			                      kernel. */
					"VBUS0OfTm",
					/* The timer period in ticks, must be
			                      greater than 0. */
					pdMS_TO_TICKS(VBUS_TURN_OFF_TIME),
					/* The timers will auto-reload themselves
			                      when they expire. */
					pdFALSE,
					/* The ID is used to store a count of the
			                      number of times the timer has expired, which
			                      is initialised to 0. */
					( void * ) VBUS0_TIMER_ID,
					/* Each timer calls the same callback when
			                      it expires. */
					usbMgr_TimerCallback
			);

	s_vBus1OffTimer = xTimerCreate
			( /* Just a text name, not used by the RTOS
			                      kernel. */
					"VBUS1OfTm",
					/* The timer period in ticks, must be
			                      greater than 0. */
					pdMS_TO_TICKS(VBUS_TURN_OFF_TIME),
					/* The timers will auto-reload themselves
			                      when they expire. */
					pdFALSE,
					/* The ID is used to store a count of the
			                      number of times the timer has expired, which
			                      is initialised to 0. */
					( void * ) VBUS1_TIMER_ID,
					/* Each timer calls the same callback when
			                      it expires. */
					usbMgr_TimerCallback
			);
	g_turnOnAnypalTimer =  xTimerCreate
			( /* Just a text name, not used by the RTOS
			                      kernel. */
					"TurnOnATP",
					/* The timer period in ticks, must be
			                      greater than 0. */
					pdMS_TO_TICKS(TURN_ON_VBUS_TIMER),
					/* The timers will auto-reload themselves
			                      when they expire. */
					pdFALSE,
					/* The ID is used to store a count of the
			                      number of times the timer has expired, which
			                      is initialised to 0. */
					( void * ) TURN_ON_ANYPAL_TIMER_ID,
					/* Each timer calls the same callback when
			                      it expires. */
					usbMgr_TimerCallback
			);
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General USB hardware setup			                                                        */
/*                                                                                      */
/* Details  -Init USB1 CHIP                 									*/
/*			-Set up USB1 power enable signal											*/
/*                                                                                      */
/* Arguments : (I) NONE					 					  							*/
/*																						*/
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
void usb_hw_setup(void)
{
	usbMgr_CreateTimer();
	usbMgr_ConfigPort();
	Chip_USB0_Init();
	Chip_USB1_Init();
	USB_Init(USB_MODE_Host); //init USB 0 and 1
	NVIC_SetPriority(USB1_IRQn, USB1_IRQ_PRIO);//Q_add
	NVIC_SetPriority(USB0_IRQn, USB0_IRQ_PRIO);//Q_add
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-Save log to USB														    */
/*                                                                                      */
/* Details  -save fh310 log file to usb													*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE					 					  							*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : bool		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/* local valiables   : NONE			                                                  	*/
/*【Note】                                                                              													*/
/*                                                                                      */
/****************************************************************************************/
static bool usbMgr_SaveLogToUSB(void)
{
	bool result = true;
	FRESULT rc;		/* Result code */
	UINT bw;
	int8_t *stringWrite;

	if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
	{
		stringWrite = (int8_t*)"No.,Date and Time,Log name,Flow Rate,\r\n";//{"No.,Date and Time,Log name,Flow Rate,\r\n"};
	}
	else
	{
		stringWrite = (int8_t*)"No., 日時 , ログ名称 , 設定流量 ,\r\n";
	}
	f_mount(&fatFS,"",0);		/* Register volume work area (never fails) */

	int8_t fileBuffer[MAX_FILENAME_LENG] = {'\0'};
	int8_t dateBuffer[MAX_DATA_LENG]= {'\0'};

	Real_Time_Clock clock;
	guiInterface_RTCGettime(&clock);

	strcat((char*)fileBuffer, "FH-310_");
	int8_t serialNumber[SERIAL_NUMBER_BUFFER_SIZE];
	guiInterface_GetSerialNumber(serialNumber);
	strcat((char*)fileBuffer,(char*)serialNumber);
	strcat((char*)fileBuffer, "_");
	sprintf((char*)dateBuffer,"%.2d%.2d%.2d_%.2d%.2d%.2d.csv",(clock.year%100),
			clock.month,clock.mday,clock.hour,clock.min,clock.sec);
	strcat((char*)fileBuffer, (char*)dateBuffer);

	rc = f_open(&fileObj,(const TCHAR*)&fileBuffer[0],FA_WRITE | FA_CREATE_ALWAYS);
	if (rc)
	{
		DEBUG_USB("*******DIE %d*******\r\n", rc);

		result = false;
	}
	else
	{
		int8_t BOM[3]={0xEF, 0xBB, 0xBF};  // Utf-8 BOM
		rc = f_write(&fileObj,BOM,3,&bw); // Write the BOM first

		rc = f_write(&fileObj, (char*)stringWrite, strlen((char*)stringWrite), &bw);
		int32_t numLog = (int32_t)logInterface_GetNumberOfLog(eMachineLog);
		FH310_LOG_T log[MAX_LOG_USB_RQ];
		FH310_LOG_T logGet[MAX_LOG_USB_RQ];
		uint8_t NoLogGet;
		LOG_REQUEST_t request;
		request.id = eUSBGetMachineLogId;
		uint16_t cout =0;
		while(numLog>0)
		{
			guiInterface_WWDTFeed(LPC_WWDT);
			if(numLog > MAX_LOG_USB_RQ)
			{
				request.beginLog =  numLog-MAX_LOG_USB_RQ+1;
				request.numberOfLog = MAX_LOG_USB_RQ;
			}
			else
			{
				request.beginLog =  FIRST_LOG_INDEX;
				request.numberOfLog = numLog;
			}
			if (logInterface_ReceiveLogForUSB(request, logGet, &NoLogGet) == eSuccess)
			{
				for (int i =0; i<NoLogGet; i++)
				{
					log[i] = logGet[NoLogGet -i-1];
					int8_t array_1[5];
					itoa(cout+1,(char*)array_1,10);
					int8_t array_2[18];

					sprintf((char*)array_2,"%.2d/%.2d/%.2d %.2d:%.2d:%.2d",(log[i].YYYY%YEAR_SCALE),log[i].MM,
							log[i].DD,log[i].hh,log[i].min,log[i].ss);

					int8_t *array_3;
					array_3 = guiInterface_GetStringLogName(log[i].ID);

					int8_t array_4[5] = {'\0'};
					float flowRate = log[i].flowRateSetting /20.0;
					sprintf((char*)array_4,"%.2f",flowRate);
					rc = f_write(&fileObj, (char*)array_1, strlen((char*)array_1), &bw);
					rc = f_write(&fileObj, ",", 1, &bw);
					rc = f_write(&fileObj, (char*)array_2, strlen((char*)array_2), &bw);
					rc = f_write(&fileObj, ",", 1, &bw);
					rc = f_write(&fileObj, (char*)array_3, strlen((char*)array_3), &bw);
					rc = f_write(&fileObj, ",", 1, &bw);
					rc = f_write(&fileObj, (char*)array_4, strlen((char*)array_4), &bw);
					rc = f_write(&fileObj, ",", 1, &bw);
					rc = f_write(&fileObj, "\r\n", 2, &bw);
					cout++;
				}
			}
			else
			{
				break;
			}
			numLog -= NoLogGet;
		}
		DEBUG_USB("\r\n\r\nClose Machine log the file.\r\n");
		//		f_sync(&fileObj);
		rc = f_close(&fileObj);
	}
	if (rc)
	{
		DEBUG_USB("*******DIE %d*******\r\n", rc);
		result = false;
	}

	return result;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-usbMgr_CheckFWFile														    */
/*                                                                                      */
/* Details  -Check FirmWare File														*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE					 					  							*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : bool		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE													        */
/*                     (O) s_m0_filename : M0 file name                                 */
/*                     (O) s_m4_filename : M4 file name 						        */
/*                     (O) s_boot_filename : bootloader file name 						*/
/* local valiables   : NONE			                                                  	*/
/*【Note】                                                                              													*/
/*                                                                                      */
/****************************************************************************************/
static CheckFWFileResult usbMgr_CheckFWFile(void)
{
	CheckFWFileResult result = eErrorFWFile;

	memset(s_boot_filename,0,FIRMWARE_NAME_ARRAY_SIZE);
	memset(s_m0_filename,0,FIRMWARE_NAME_ARRAY_SIZE);
	memset(s_m4_filename,0,FIRMWARE_NAME_ARRAY_SIZE);

	if(updateFlash_CheckFirwareFiles(s_m0_filename,s_m4_filename) == false)
	{
		if(updateFlash_CheckBootloaderFileRTOS(s_boot_filename) == false)
		{
			result = eExistBootAndCTrlSW;
		}
		else
		{
			result = eExistControlSW;
		}
	}
	return result;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-Check password file													    */
/*                                                        	                            */
/* Details  -check existence of breathtest_technology file								*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE					 					  							*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : bool		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/* local valiables   : NONE			                                                  	*/
/*【Note】                                                                              													*/
/*                                                                                      */
/****************************************************************************************/
static bool usbMgr_CheckPWFile(void)
{
	bool result = false;
	DIR dir={'\0'};
	FILINFO fno;
	static TCHAR lfname[_MAX_LFN];

	f_mount(&fatFS,"",0);

	FRESULT rc = f_opendir(&dir, "");//"breath_technology.txt");
	if (rc)
	{
		result = false;
	}
	else
	{
		while(1)
		{
			fno.lfname = lfname;
			fno.lfsize = _MAX_LFN - 1;
			rc = f_readdir(&dir, &fno);
			if (rc || !fno.fname[0])
			{
				break;
			}
			if (fno.fattrib & AM_DIR)
			{
				//DEBUG_USB("   <dir>  %s\r\n", fno.fname);
			}
			else
			{
				if(strcmp((char*)fno.lfname,(char*)PASSWORD_FILE_NAME) == 0)
				{
					result = true;
					break;
				}
			}
		}
	}
	return result;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-Dump EEPROM Memory														    */
/*                                                        	                            */
/* Details  -EEPROM memory dump file													*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE					 					  							*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : bool		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/* local valiables   : NONE			                                                  	*/
/*【Note】                                                                              													*/
/*                                                                                      */
/****************************************************************************************/
static bool usbMgr_DumpEEPROMMemory(void)
{
	bool result = true;
	FRESULT rc;		/* Result code */
	UINT bw;
	int8_t *stringWrite = (int8_t*)"Adderss,F,E,D,C,B,A,9,8,7,6,5,4,3,2,1,0\r\n";

	f_mount(&fatFS,"",0);		/* Register volume work area (never fails) */

	//DEBUG_USB("\r\nCreate a new file (hello.txt).\r\n");
	int8_t fileBuffer[MAX_FILENAME_LENG] = {'\0'};
	int8_t dateBuffer[MAX_DATA_LENG]= {'\0'};

	Real_Time_Clock clock;
	guiInterface_RTCGettime(&clock);

	strcat((char*)fileBuffer, "FH310_EEPROM_");
	int8_t serialNumber[SERIAL_NUMBER_BUFFER_SIZE];
	guiInterface_GetSerialNumber(serialNumber);
	strcat((char*)fileBuffer,(char*)serialNumber);
	strcat((char*)fileBuffer, "_");
	sprintf((char*)dateBuffer,"%.2d%.2d%.2d_%.2d%.2d%.2d.csv",(clock.year%YEAR_SCALE),clock.month,clock.mday,clock.hour,clock.min,clock.sec);
	strcat((char*)fileBuffer, (char*)dateBuffer);

	rc = f_open(&fileObj,(const TCHAR*)fileBuffer, FA_WRITE | FA_CREATE_ALWAYS);
	if (rc) {
		DEBUG_USB("*******DIE %d*******\r\n", rc);

		result = false;
	}
	else
	{
		rc = f_write(&fileObj, (char*)stringWrite, strlen((char*)stringWrite), &bw);
		for(int i = 0; i< EEPROM_SIZE;i+=ONE_LINE_SIZE)
		{
			guiInterface_WWDTFeed(LPC_WWDT);
			uint8_t buffRead[ONE_LINE_SIZE]={0};
			guiInterface_ReadEEPROMForDump(i, (uint32_t*) buffRead);
			int8_t buffWrite[6]={0};
			sprintf((char*)buffWrite,"%.4x\0",i);
			rc = f_write(&fileObj, (char*)buffWrite, strlen((char*)buffWrite), &bw);
			for(int j=ONE_LINE_SIZE-1; j >= 0; j--)
			{
				sprintf((char*)buffWrite,",%.2x\0",buffRead[j]);
				rc = f_write(&fileObj, (char*)buffWrite, strlen((char*)buffWrite), &bw);
			}
			rc = f_write(&fileObj, "\r\n", 2, &bw);
		}
		DEBUG_USB("\r\n\r\nClose the EEPROM memory dump file.\r\n");
		rc = f_close(&fileObj);
	}
	if (rc)
	{
		DEBUG_USB("*******DIE %d*******\r\n", rc);
		result = false;
	}
	return result;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-Dump EEPROM Memory														    */
/*                                                        	                            */
/* Details  -EEPROM memory dump file													*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE					 					  							*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : bool		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/* local valiables   : NONE			                                                  	*/
/*【Note】                                                                              													*/
/*                                                                                      */
/****************************************************************************************/
static bool usbMgr_DumpCBXUsageLog(void)
{
	bool result = true;
	FRESULT rc;		/* Result code */
	UINT bw;
	int8_t *stringWrite = (int8_t*)"Log Number,Date and Time,Code,Flow rate\r\n";

	f_mount(&fatFS,"",0);	/* Register volume work area (never fails) */

	//DEBUG_USB("\r\nCreate a new file (hello.txt).\r\n");
	int8_t fileBuffer[MAX_FILENAME_LENG] = {'\0'};
	int8_t dateBuffer[MAX_DATA_LENG]= {'\0'};

	Real_Time_Clock clock;
	guiInterface_RTCGettime(&clock);

	strcat((char*)fileBuffer, "FH310_USAGE_CBX_");
	int8_t serialNumber[SERIAL_NUMBER_BUFFER_SIZE];
	guiInterface_GetSerialNumber(serialNumber);
	strcat((char*)fileBuffer,(char*)serialNumber);
	strcat((char*)fileBuffer, "_");
	sprintf((char*)dateBuffer,"%.2d%.2d%.2d_%.2d%.2d%.2d.csv",(clock.year%YEAR_SCALE),clock.month,clock.mday,clock.hour,clock.min,clock.sec);
	strcat((char*)fileBuffer, (char*)dateBuffer);

	rc = f_open(&fileObj,(const TCHAR*)fileBuffer, FA_WRITE | FA_CREATE_ALWAYS);
	if (rc) {
		DEBUG_USB("*******DIE %d*******\r\n", rc);

		result = false;
	}
	else
	{
		rc = f_write(&fileObj, (char*)stringWrite, strlen((char*)stringWrite), &bw);
		int32_t numLog = (int32_t)logInterface_GetNumberOfLog(eServerLog);
		FH310_LOG_T log[MAX_LOG_USB_RQ];
		FH310_LOG_T logGet[MAX_LOG_USB_RQ];
		uint8_t NoLogGet;
		LOG_REQUEST_t request;
		request.id = eUSBGetServerLogId;
		uint16_t cout =0;
		while(numLog>0)
		{
			guiInterface_WWDTFeed(LPC_WWDT);
			if(numLog > MAX_LOG_USB_RQ)
			{
				request.beginLog =  numLog-MAX_LOG_USB_RQ+1;
				request.numberOfLog = MAX_LOG_USB_RQ;
			}
			else
			{
				request.beginLog =  FIRST_LOG_INDEX;
				request.numberOfLog = numLog;
			}
			if (logInterface_ReceiveLogForUSB(request, logGet, &NoLogGet) == eSuccess)
			{
				for (int i =0; i<NoLogGet; i++)
				{
					log[i] = logGet[NoLogGet -i-1];
					int8_t array_1[5];
					itoa(cout + 1,(char*)array_1,10);
					int8_t array_2[20];

					sprintf((char*)array_2,"%.2d/%.2d/%.2d %.2d:%.2d:%.2d\0",(log[i].YYYY%100),log[i].MM,
							log[i].DD,log[i].hh,log[i].min,log[i].ss);

					int8_t array_3[5];
					sprintf((char*)array_3,"%.4x\0",log[i].ID);
					int8_t array_4[5] = {'\0'};
					//				float flowRate = log[i].flowRateSetting*5;
					sprintf((char*)array_4,"%.4x\0",log[i].flowRateSetting*FLOW_RATE_SCALE);
					rc = f_write(&fileObj, (char*)array_1, strlen((char*)array_1), &bw);
					rc = f_write(&fileObj, ",", 1, &bw);
					rc = f_write(&fileObj, (char*)array_2, strlen((char*)array_2), &bw);
					rc = f_write(&fileObj, ",", 1, &bw);
					rc = f_write(&fileObj, (char*)array_3, strlen((char*)array_3), &bw);
					rc = f_write(&fileObj, ",", 1, &bw);
					rc = f_write(&fileObj, (char*)array_4, strlen((char*)array_4), &bw);
					rc = f_write(&fileObj, "\r\n", 2, &bw);
					cout++;
				}
			}
			else
			{
				break;
			}
			numLog -= NoLogGet;
		}
		DEBUG_USB("\r\n\r\nClose the Usage log (CBX) dump file.\r\n");
		//		f_sync(&fileObj);
		rc = f_close(&fileObj);
	}
	if (rc)
	{
		DEBUG_USB("*******DIE %d*******\r\n", rc);
		result = false;
	}

	return result;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-Dump Anypal Usage Log													    */
/*                                                        	                            */
/* Details  -all data of Usage Anypal is recorded into USB memory						*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE					 					  							*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : bool		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/* local valiables   : NONE			                                                  	*/
/*【Note】                                                                              													*/
/*                                                                                      */
/****************************************************************************************/
static bool usbMgr_DumpAnypalUsageLog(void)
{
	bool result = true;
	FRESULT rc;		/* Result code */
	UINT bw;
	int8_t *stringWrite = (int8_t*)"Log Number,Year,Month,Day,Hour,Minuter,Second,Event number,Flow rate\r\n";
	f_mount(&fatFS,"",0);	/* Register volume work area (never fails) */

	//DEBUG_USB("\r\nCreate a new file (hello.txt).\r\n");
	int8_t fileBuffer[MAX_FILENAME_LENG] = {'\0'};
	int8_t dateBuffer[MAX_DATA_LENG]= {'\0'};

	Real_Time_Clock clock;
	guiInterface_RTCGettime(&clock);

	strcat((char*)fileBuffer, "FH310_USAGE_ATP_");
	int8_t serialNumber[SERIAL_NUMBER_BUFFER_SIZE];
	guiInterface_GetSerialNumber(serialNumber);
	strcat((char*)fileBuffer,(char*)serialNumber);
	strcat((char*)fileBuffer, "_");
	sprintf((char*)dateBuffer,"%.2d%.2d%.2d_%.2d%.2d%.2d.csv",(clock.year%YEAR_SCALE)
			,clock.month,clock.mday,clock.hour,clock.min,clock.sec);
	strcat((char*)fileBuffer, (char*)dateBuffer);

	rc = f_open(&fileObj,(const TCHAR*)fileBuffer, FA_WRITE | FA_CREATE_ALWAYS);
	if (rc) {
		DEBUG_USB("*******DIE %d*******\r\n", rc);

		result = false;
	}
	else
	{
		rc = f_write(&fileObj, (char*)stringWrite, strlen((char*)stringWrite), &bw);
		int32_t numLog = (int32_t)logInterface_GetNumberOfLog(eUsageLog);
		FH310_LOG_T log[MAX_LOG_USB_RQ];
		FH310_LOG_T logGet[MAX_LOG_USB_RQ];
		uint8_t NoLogGet;
		LOG_REQUEST_t request;
		request.id = eUSBGetUsageLogId;
		uint16_t cout =0;
		while(numLog>0)
		{
			guiInterface_WWDTFeed(LPC_WWDT);
			if(numLog > MAX_LOG_USB_RQ)
			{
				request.beginLog =  numLog-MAX_LOG_USB_RQ+1;
				request.numberOfLog = MAX_LOG_USB_RQ;
			}
			else
			{
				request.beginLog =  FIRST_LOG_INDEX;
				request.numberOfLog = numLog;
			}
			if (logInterface_ReceiveLogForUSB(request, logGet, &NoLogGet) == eSuccess)
			{
				for (int i =0; i<NoLogGet; i++)
				{
					log[i] = logGet[NoLogGet -i-1];
					int8_t array[7]={0};
					itoa(cout+1,(char*)array,10);
					rc = f_write(&fileObj, (char*)array, strlen((char*)array), &bw);
					rc = f_write(&fileObj, ",", 1, &bw);

					sprintf((char*)array,"%.2x,\0", log[i].YYYY%100);
					rc = f_write(&fileObj, (char*)array, strlen((char*)array), &bw);

					sprintf((char*)array,"%.2x,\0", log[i].MM);
					rc = f_write(&fileObj, (char*)array, strlen((char*)array), &bw);

					sprintf((char*)array,"%.2x,\0", log[i].DD);
					rc = f_write(&fileObj, (char*)array, strlen((char*)array), &bw);

					sprintf((char*)array,"%.2x,\0", log[i].hh);
					rc = f_write(&fileObj, (char*)array, strlen((char*)array), &bw);

					sprintf((char*)array,"%.2x,\0", log[i].min);
					rc = f_write(&fileObj, (char*)array, strlen((char*)array), &bw);

					sprintf((char*)array,"%.2x,\0", log[i].ss);
					rc = f_write(&fileObj, (char*)array, strlen((char*)array), &bw);

					uint8_t eventNumber;
					if(log[i].ID == eOperationStartLogId)//Operation start
					{
						eventNumber = (uint8_t)0x01;

					}
					else if(log[i].ID == eChangingFlowRateLogId)//Changing flow rate
					{
						eventNumber = (uint8_t)0x02;
					}
					else //Operation stop
					{
						eventNumber = (uint8_t)0x03;
					}

					sprintf((char*)array,"%.1x,\0", eventNumber);
					rc = f_write(&fileObj, (char*)array, strlen((char*)array), &bw);

					float flowRate  = (float)log[i].flowRateSetting / 20.0;
					uint8_t flowRateSave=0;
					for (flowRateSave=0; flowRateSave < 11; flowRateSave++)
					{
						if (gs_flowRateST[flowRateSave] == flowRate)
						{
							break;
						}
					}
					sprintf((char*)array,"%.2d\r\n\0", flowRateSave);
					rc = f_write(&fileObj, (char*)array, strlen((char*)array), &bw);

					cout++;
				}
			}
			else
			{
				break;
			}
			numLog -= NoLogGet;
		}
		DEBUG_USB("\r\n\r\nClose the Usage log (ATP) dump file.\r\n");
		//		f_sync(&fileObj);
		rc = f_close(&fileObj);
	}
	if (rc)
	{
		DEBUG_USB("*******DIE %d*******\r\n", rc);
		result = false;
	}
	return result;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-Dump SpO2 Pulse Log													    */
/*                                                        	                            */
/* Details  -all data of SpO2 Pulse is recorded into USB memory							*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE					 					  							*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : bool		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/* local valiables   : NONE			                                                  	*/
/*【Note】                                                                              													*/
/*                                                                                      */
/****************************************************************************************/
static bool usbMgr_DumpSpO2PulseLog(void)
{
	bool result = true;
	FRESULT rc;		/* Result code */
	UINT bw;
	int8_t *stringWrite = (int8_t*)"Log Number,Date and Time,SpO2/Pulse\r\n";
	f_mount(&fatFS,"",0);		/* Register volume work area (never fails) */

	//DEBUG_USB("\r\nCreate a new file (hello.txt).\r\n");
	int8_t fileBuffer[MAX_FILENAME_LENG] = {'\0'};
	int8_t dateBuffer[MAX_DATA_LENG]= {'\0'};

	Real_Time_Clock clock;
	guiInterface_RTCGettime(&clock);

	strcat((char*)fileBuffer, "FH310_SPO2_PULSE_");
	int8_t serialNumber[SERIAL_NUMBER_BUFFER_SIZE];
	guiInterface_GetSerialNumber(serialNumber);
	strcat((char*)fileBuffer,(char*)serialNumber);
	strcat((char*)fileBuffer, "_");
	sprintf((char*)dateBuffer,"%.2d%.2d%.2d_%.2d%.2d%.2d.csv",(clock.year%100),clock.month,clock.mday,clock.hour,clock.min,clock.sec);
	strcat((char*)fileBuffer, (char*)dateBuffer);

	rc = f_open(&fileObj,(const TCHAR*)fileBuffer, FA_WRITE | FA_CREATE_ALWAYS);
	if (rc) {
		DEBUG_USB("*******DIE %d*******\r\n", rc);

		result = false;
	}
	else
	{
		rc = f_write(&fileObj, (char*)stringWrite, strlen((char*)stringWrite), &bw);
		int32_t numLog = (int32_t)logInterface_GetNumberOfLog(eAnypalLog);
		FH310_LOG_T log[MAX_LOG_USB_RQ];
		FH310_LOG_T logGet[MAX_LOG_USB_RQ];
		uint8_t NoLogGet;
		LOG_REQUEST_t request;
		request.id = eUSBGetAnypalLogId;
		uint16_t cout =0;
		while(numLog>0)
		{
			guiInterface_WWDTFeed(LPC_WWDT);
			if(numLog > MAX_LOG_USB_RQ)
			{
				request.beginLog =  numLog-MAX_LOG_USB_RQ+1;
				request.numberOfLog = MAX_LOG_USB_RQ;
			}
			else
			{
				request.beginLog =  FIRST_LOG_INDEX;
				request.numberOfLog = numLog;
			}
			if (logInterface_ReceiveLogForUSB(request, logGet, &NoLogGet) == eSuccess)
			{
				for (int i =0; i<NoLogGet; i++)
				{
					log[i] = logGet[NoLogGet -i-1];
					int8_t array[20]={0};
					itoa(cout+1,(char*)array,10);
					rc = f_write(&fileObj, (char*)array, strlen((char*)array), &bw);
					rc = f_write(&fileObj, ",", 1, &bw);

					sprintf((char*)array,"%.2d/%.2d/%.2d %.2d:%.2d:%.2d,\0",(log[i].YYYY%YEAR_SCALE),log[i].MM,
							log[i].DD,log[i].hh,log[i].min,log[i].ss);
					rc = f_write(&fileObj, (char*)array, strlen((char*)array), &bw);

					sprintf((char*)array,"%.2x%.2x\r\n\0",log[i].SpO2,log[i].pulse);
					rc = f_write(&fileObj, (char*)array, strlen((char*)array), &bw);
					cout++;
				}
			}
			else
			{
				break;
			}
			numLog -= NoLogGet;
		}
		DEBUG_USB("\r\n\r\nClose the SpO2/Pulse dump file.\r\n");
		//		f_sync(&fileObj);
		rc = f_close(&fileObj);
	}
	if (rc)
	{
		DEBUG_USB("*******DIE %d*******\r\n", rc);
		result = false;
	}
	return result;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-Intel Hex File CheckSum												    */
/*                                                        	                            */
/* Details  -calculate Intel file check sum												*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE					 					  							*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : bool		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/* local valiables   : NONE			                                                  	*/
/*【Note】                                                                              													*/
/*                                                                                      */
/****************************************************************************************/
static uint8_t usbMgr_IntelHexFileCheckSum(int8_t *ccArray)
{
	uint8_t checkSum =0;
	for(int i=1; i< strlen((char*)ccArray); i+=2)
	{
		checkSum += (((HexCharToInt(ccArray[i]))<<sizeof(uint32_t)) + HexCharToInt(ccArray[i+1]));
	}
	return (0x01 + ~(checkSum));
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-Dump memory															    */
/*                                                        	                            */
/* Details  -all data in memory record into file										*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE					 					  							*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : bool		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/* local valiables   : NONE			                                                  	*/
/*【Note】                                                                              													*/
/*                                                                                      */
/****************************************************************************************/
static bool usbMgr_DumpMemory(uint32_t beginAddr,FIL *fp, uint32_t size)
{
	FRESULT rc;
	UINT bw;
	int8_t array[MAX_FILENAME_LENG];
	uint8_t ccInt =0;
	int8_t ccStr[5] = {0};
	for (int i= 0; i< size; i+= EXTENDED_LINEAR_SIZE)
	{
		//Extended Linear Address Records
		uint32_t extLinerAdd = ((beginAddr + i)>>16);
		memset(array, 0, 50);
		sprintf((char*)array, ":02000004%.4X\0",extLinerAdd);
		ccInt = usbMgr_IntelHexFileCheckSum(array);
		sprintf((char*)ccStr, "%.2X\r\n\0", ccInt);
		strcat((char*)array, (char*)ccStr);
		rc = f_write(fp, (char*)array, strlen((char*)array), &bw);
		if (rc) {
			DEBUG_USB("*******DIE %d*******\r\n", rc);
			return false;
		}
		//Data Records
		for(int j =0; j<EXTENDED_LINEAR_SIZE; j+=ONE_LINE_SIZE)
		{
			guiInterface_WWDTFeed(LPC_WWDT);
			if((i + j) >= size) break;

			memset(array, 0, MAX_FILENAME_LENG);
			sprintf((char*)array, ":10%.4X00\0",i + j);

			uint8_t dataInt = 0;
			int8_t dataStr[3] = {0};

			uint8_t *memoryPtr = (uint8_t*)(beginAddr + i + j);

			for(int k=ONE_LINE_SIZE-1; k>=0; k--)
			{
				dataInt = memoryPtr[k];
				sprintf((char*)dataStr, "%.2X\0", dataInt);
				strcat((char*)array, (char*)dataStr);
			}
			ccInt = usbMgr_IntelHexFileCheckSum(array);
			sprintf((char*)ccStr, "%.2X\r\n\0", ccInt);
			strcat((char*)array, (char*)ccStr);
			rc = f_write(fp, (char*)array, strlen((char*)array), &bw);
			if (rc) {
				DEBUG_USB("*******DIE %d*******\r\n", rc);

				return false;
			}
		}
	}
	return true;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-Dump memory to USB														    */
/*                                                        	                            */
/* Details  -all data in memory record into usb											*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE					 					  							*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : bool		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/* local valiables   : NONE			                                                  	*/
/*【Note】                                                                              													*/
/*                                                                                      */
/****************************************************************************************/
static bool usbMgr_DumpMemoryToFile(MemoryTypeID memoryType)
{
	bool result = true;
	FRESULT rc;		/* Result code */
	UINT bw;

	guiInterface_WWDTFeed(LPC_WWDT);

	f_mount(&fatFS,"",0);		/* Register volume work area (never fails) */

	//DEBUG_USB("\r\nCreate a new file (hello.txt).\r\n");
	int8_t fileBuffer[MAX_FILENAME_LENG] = {'\0'};
	int8_t dateBuffer[MAX_DATA_LENG]= {'\0'};

	Real_Time_Clock clock;
	guiInterface_RTCGettime(&clock);
	switch (memoryType)
	{
	case eSRAMId:
		strcat((char*)fileBuffer, "FH310_SRAM_");
		break;
	case eSDRAMId:
		strcat((char*)fileBuffer, "FH310_SDRAM_");
		break;
	case eSPIFlashId:
		strcat((char*)fileBuffer, "FH310_SPI_FLASH_");
		break;
	case eFlashAId:
		strcat((char*)fileBuffer, "FH310_BANK_A_");
		break;
	case eFlashBId:
		strcat((char*)fileBuffer, "FH310_BANK_B_");
		break;
	default:
		break;
	}

	int8_t serialNumber[SERIAL_NUMBER_BUFFER_SIZE];
	guiInterface_GetSerialNumber(serialNumber);
	strcat((char*)fileBuffer,(char*)serialNumber);
	strcat((char*)fileBuffer, "_");
	sprintf((char*) dateBuffer,"%.2d%.2d%.2d_%.2d%.2d%.2d.hex",(clock.year%100),clock.month,clock.mday,clock.hour,clock.min,clock.sec);
	strcat((char*)fileBuffer, (char*)dateBuffer);

	guiInterface_WWDTFeed(LPC_WWDT);

	rc = f_open(&fileObj,(const TCHAR*)fileBuffer, FA_WRITE | FA_CREATE_ALWAYS);
	if (rc) {
		DEBUG_USB("*******DIE %d*******\r\n", rc);

		return false;
	}
	else
	{
		switch (memoryType)
		{
		case eSRAMId:
		{
			result = usbMgr_DumpMemory(RAMLOC32_ADDRESS,&fileObj, RAMLOC32_SIZE);
			result = usbMgr_DumpMemory(RAMLOC40_ADDRESS,&fileObj, RAMLOC40_SIZE);
			result = usbMgr_DumpMemory(RAMAHB32_ADDRESS,&fileObj, RAMAHB32_SIZE);
			result = usbMgr_DumpMemory(RAMAHB16_ADDRESS,&fileObj, RAMAHB16_SIZE);
			result = usbMgr_DumpMemory(RAMAHB_ETB16_ADDRESS,&fileObj, RAMAHB_ETB16_SIZE);

			//EOF
			rc = f_write(&fileObj,":00000001FF", 11, &bw);
			if (rc) {
				DEBUG_USB("*******DIE %d*******\r\n", rc);

				return false;
			}

			DEBUG_USB("\r\n\r\nClose the SRAM dump file.\r\n");
			break;
		}
		case eSDRAMId:
		{
			result = usbMgr_DumpMemory(RAMAHB_EXT16_ADDRESS,&fileObj, RAMAHB_EXT16_SIZE);

			//EOF

			rc = f_write(&fileObj,":00000001FF", 11, &bw);
			if (rc) {
				DEBUG_USB("*******DIE %d*******\r\n", rc);

				return false;
			}

			DEBUG_USB("\r\n\r\nClose the SDRAM dump file.\r\n");
			break;
		}
		case eSPIFlashId:
		{
			result = usbMgr_DumpMemory(SPI_FLASH_ADDRESS,&fileObj, SPI_FLASH_SIZE);
			rc = f_write(&fileObj,":00000001FF", 11, &bw);
			if (rc) {
				DEBUG_USB("*******DIE %d*******\r\n", rc);

				return false;
			}
			DEBUG_USB("\r\n\r\nClose the SPI FLASH dump file.\r\n");
			break;
		}
		case eFlashAId:
		{
			result = usbMgr_DumpMemory(MFLASH_A_ADDRESS,&fileObj, MFLASH_A_SIZE);

			//EOF
			rc = f_write(&fileObj,":00000001FF", 11, &bw);
			if (rc) {
				DEBUG_USB("*******DIE %d*******\r\n", rc);

				return false;
			}

			DEBUG_USB("\r\n\r\nClose the FLASH_A dump file.\r\n");
			break;
		}
		case eFlashBId:
		{
			result = usbMgr_DumpMemory(MFLASH_B_ADDRESS,&fileObj, MFLASH_B_SIZE);

			//EOF
			rc = f_write(&fileObj,":00000001FF", 11, &bw);
			if (rc) {
				DEBUG_USB("*******DIE %d*******\r\n", rc);

				return false;
			}

			DEBUG_USB("\r\n\r\nClose the FLASH_B dump file.\r\n");
			break;
		}
		default:
			break;
		}
	}

	rc = f_close(&fileObj);
	if (rc)
	{
		DEBUG_USB("*******DIE %d*******\r\n", rc);
		result = false;
	}

	return result;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-Upload CBX Usage Log															    */
/*                                                        	                            */
/* Details  -store Usage log (CBX)log into FH-310 from files in USB memory				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE					 					  							*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : bool		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/* local valiables   : NONE			                                                  	*/
/*【Note】                                                                              													*/
/*                                                                                      */
/****************************************************************************************/
static bool usbMgr_UploadMemory(FileTypeID fileType)
{
	FILINFO fno;
	DIR dir={'\0'};
	int8_t buff=0;
	UINT r=1;
	FIL file;

	int8_t fileName[MAX_FILENAME_LENG]={'\0'};

	f_mount(&fatFS,"",0);

	FRESULT rc;

	if (f_opendir(&dir, ""))
	{
		return false;
	}

	int8_t fileNameForm[MAX_FILENAME_LENG]={0};
	switch (fileType)
	{
	case eCBXLogFileId:
		strcpy((char*)fileNameForm,"FH310_USAGE_CBX_");
		break;
	case eAnypalLogFileId:
		strcpy((char*)fileNameForm,"FH310_USAGE_ATP_");
		break;
	case eSpO2PulseLogId:
		strcpy((char*)fileNameForm,"FH310_SPO2_PULSE_");
		break;
	case eEEPROMFileId:
		strcpy((char*)fileNameForm,"FH310_EEPROM_");
		break;
	default:
		break;
	}

	int file_exist=0;
	static TCHAR lfname[_MAX_LFN];
	fno.lfsize = _MAX_LFN - 1;
	fno.lfname = lfname;
	for (;; )
	{
		guiInterface_WWDTFeed(LPC_WWDT);

		rc = f_readdir(&dir, &fno);

		if (rc || !fno.fname[0])
		{
			break;
		}
		if (fno.fattrib & AM_DIR)
		{
			//DEBUG_USB("   <dir>  %s\r\n", fno.fname);
		}
		else
		{
			if(strstr((char*)fno.lfname,(char*)fileNameForm)!=NULL)
			{
				strncpy((char*)fileName,fno.lfname,MAX_FILENAME_LENG);
				DEBUG_USB("file name: %s\n", fileName);
				file_exist++;
				if (file_exist >1) return false;
			}

		}
	}

	uint32_t byteCount =0;
	uint32_t lineCount =0;
	if(f_open(&file, (char*)fileName, FA_READ))
	{
		return false;
	}

	if (eEEPROMFileId != fileType)
	{
		guiInterface_WWDTFeed(LPC_WWDT);
		while(r!=0)
		{
			if (f_read(&file,&buff,1,&r)) return false;
			if('\n' ==buff)
			{
				lineCount++;
			}
			byteCount++;
		}
		DEBUG_USB("lineNumber: %d",lineCount);
		DEBUG_USB("byteNumber: %d\n",byteCount);
	}
	else
	{
		if (f_read(&file,&buff,1,&r)) return false;
		while('\n' !=buff)
		{
			if (f_read(&file,&buff,1,&r)) return false;
		}
	}

	FH310_LOG_T log;


	switch (fileType)
	{
	case eCBXLogFileId:
	{
		logInterface_DeleteLog(eDeleteServerLogId);
		vTaskDelay(50/portTICK_PERIOD_MS);
		while(lineCount > LAST_LINE)
		{
			guiInterface_WWDTFeed(LPC_WWDT);
			log.flowRateSetting = GetValue(&file, (int8_t)',', &byteCount, 4, true)/5;
			log.ID = GetValue(&file, (int8_t)',', &byteCount, 4, true);
			log.ss = GetValue(&file, (int8_t)':', &byteCount, 2, false);
			log.min = GetValue(&file, (int8_t)':', &byteCount, 2, false);
			log.hh = GetValue(&file, (int8_t)' ', &byteCount, 2, false);
			log.DD = GetValue(&file, (int8_t)'/', &byteCount, 2, false);
			log.MM = GetValue(&file, (int8_t)'/', &byteCount, 2, false);
			log.YYYY = 2000 + GetValue(&file, (int8_t)',', &byteCount, 2, false);
			logInterface_WriteLogToSpiFlash(log, eServerLog);
			lineCount--;
		}
		DEBUG_USB("Upload CBX Usage Log done\n");
		break;
	}
	case eAnypalLogFileId:
	{
		logInterface_DeleteLog(eDeleteUsageLogId);
		vTaskDelay(50/portTICK_PERIOD_MS);
		while(lineCount > LAST_LINE)
		{
			guiInterface_WWDTFeed(LPC_WWDT);
			uint8_t flowRateSettingIndex = GetValue(&file, (int8_t)',', &byteCount, 2, false);
			log.flowRateSetting = (uint8_t)(gs_flowRateST[flowRateSettingIndex]*20);
			log.ID = GetValue(&file, (int8_t)',', &byteCount, 1, true);
			if(log.ID == (LogId)0x01)//Operation start
			{
				log.ID = eOperationStartLogId;

			}
			else if(log.ID == (LogId)0x02)//Changing flow rate
			{
				log.ID = eChangingFlowRateLogId;
			}
			else //Operation stop
			{
				log.ID = eOperationStopLogId;
			}
			log.ss = GetValue(&file, (int8_t)',', &byteCount, 2, true);
			log.min = GetValue(&file, (int8_t)',', &byteCount, 2, true);
			log.hh = GetValue(&file, (int8_t)',', &byteCount, 2, true);
			log.DD = GetValue(&file, (int8_t)',', &byteCount, 2, true);
			log.MM = GetValue(&file, (int8_t)',', &byteCount, 2, true);
			log.YYYY = 2000+ GetValue(&file, (int8_t)',', &byteCount, 2, true);
			logInterface_WriteLogToSpiFlash(log, eUsageLog);
			lineCount--;
		}
		DEBUG_USB("Upload ATP Usage Log done\n");
		break;
	}
	case eSpO2PulseLogId:
	{
		logInterface_DeleteLog(eDeleteAnypalLogId);
		vTaskDelay(50/portTICK_PERIOD_MS);
		uint16_t SpO2Pulse =0;
		while(lineCount >LAST_LINE)
		{
			guiInterface_WWDTFeed(LPC_WWDT);
			SpO2Pulse = GetValue(&file, ',', &byteCount, 4, true);
			log.ID = eAnypalLogId;
			log.pulse = (uint8_t)(SpO2Pulse&0x00FF);
			log.SpO2 = (uint8_t)(SpO2Pulse>>8);
			log.ss = GetValue(&file, (int8_t)':', &byteCount, 2, false);
			log.min = GetValue(&file, (int8_t)':', &byteCount, 2, false);
			log.hh = GetValue(&file, (int8_t)' ', &byteCount, 2, false);
			log.DD = GetValue(&file, (int8_t)'/', &byteCount, 2, false);
			log.MM = GetValue(&file, (int8_t)'/', &byteCount, 2, false);
			log.YYYY = 2000 + GetValue(&file, (int8_t)',', &byteCount, 2, false);
			logInterface_WriteLogToSpiFlash(log, eAnypalLog);
			lineCount--;
		}
		DEBUG_USB("Upload SPO2 PULSE Log done\n");
		break;
	}
	case eEEPROMFileId:
	{
		int8_t buffEEP[3] = {0};
		uint8_t EEPInt[MAX_DATA_LENG]= {0};
		for (int j =0; j< EEPROM_LAST_UPLOAD_ADD ; j+=ONE_LINE_SIZE)
		{
			guiInterface_WWDTFeed(LPC_WWDT);
			for (int i=0; i<ONE_LINE_SIZE; i++)
			{
				while(',' !=buff)
				{
					if (f_read(&file,&buff,1,&r)) return false;
				}
				buff=0;
				FRESULT rc = f_read(&file,buffEEP,2,&r);
				if (rc) return false;
				EEPInt[15-i] = (uint8_t)HexStrToInt(buffEEP,2);
			}
			guiInterface_EEPROMWriteForUpload(j, EEPInt);
		}
		DEBUG_USB("Upload EEPROM done\n");
		break;
	}
	default:
		break;
	}

	return true;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-usbMgr_PrepareCSVFile														*/
/*                                                        	                            */
/* Details  -Prepare CSV File			 												*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE					 												*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : bool : result Prepare CSV File	                                        */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) fileObj : file object                                        */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              	*/
/*                                                                                      */
/****************************************************************************************/
bool usbMgr_PrepareCSVFile(void)
{
	FRESULT rc;		/* Result code */
	UINT bw;
	int8_t *stringWrite;

	stringWrite = (int8_t*)"Log Number,Year,Month,Day,Hour,Minutes,Second,Pt tank pressure,Unit internal temperature,Compressor outlet temperature,Measured flow rate,O2 concentration,Others,\r\n";
	f_mount(&fatFS,"",0);		/* Register volume work area (never fails) */

	int8_t fileBuffer[MAX_FILENAME_LENG] = {'\0'};
	int8_t dateBuffer[MAX_DATA_LENG]= {'\0'};

	Real_Time_Clock clock;
	guiInterface_RTCGettime(&clock);

	strcat((char*)fileBuffer, "FH310_DEBUG_LOG_");
	int8_t serialNumber[SERIAL_NUMBER_BUFFER_SIZE];
	guiInterface_GetSerialNumber(serialNumber);
	strcat((char*)fileBuffer,(char*)serialNumber);
	strcat((char*)fileBuffer, "_");
	sprintf((char*)dateBuffer,"%.2d%.2d%.2d_%.2d%.2d%.2d.csv",(clock.year%100),
			clock.month,clock.mday,clock.hour,clock.min,clock.sec);
	strcat((char*)fileBuffer, (char*)dateBuffer);

	rc = f_open(&fileObj,(const TCHAR*)&fileBuffer[0],FA_WRITE | FA_CREATE_ALWAYS);

	if (rc)
	{
		DEBUG_USB("*******DIE %d*******\r\n", rc);
		return false;
	}
	else
	{
		//write header to CSV file
		rc = f_write(&fileObj, (char*)stringWrite, strlen((char*)stringWrite), &bw);

		//Check return value
		if(rc)
		{
			return false;
		}

		return true;
	}
}
/****************************************************************************************/
/*                                                                                      */
/* General 	-debugLog_WriteLogToFile													*/
/*                                                        	                            */
/* Details  -Write log to file			 												*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) uint16_t logNumber : - log data					 					*/
/*                     - range : 1-4088                                                 */
/*                     - unit : none                                                    */
/*                                                                                      */
/*				   DEBUG_LOG_STRUCT* logPtr : debug log data							*/
/*                                                                                      */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) fileObj : file object                                        */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              													*/
/*                                                                                      */
/****************************************************************************************/
void usbMgr_WriteLogToFile(uint16_t logNumber, DEBUG_LOG_STRUCT* logPtr)
{
	UINT bw;

	//buffer write to CSV file
	int8_t logNumberStr[SIZE_BUFFER_NUMBER_DEBUG_LOG] = {'\0'};

	sprintf((char*)logNumberStr,"%d",logNumber);

	f_write(&fileObj, (char*)logNumberStr, strlen((char*)logNumberStr), &bw);
	f_write(&fileObj, ",", 1, &bw);

	uint8_t year[SIZE_BUFFER_DATE_DEBUG_LOG] = {'\0'};
	uint8_t month[SIZE_BUFFER_DATE_DEBUG_LOG] = {'\0'};
	uint8_t day[SIZE_BUFFER_DATE_DEBUG_LOG] = {'\0'};
	uint8_t hour[SIZE_BUFFER_DATE_DEBUG_LOG] = {'\0'};
	uint8_t min[SIZE_BUFFER_DATE_DEBUG_LOG] = {'\0'};
	uint8_t sec[SIZE_BUFFER_DATE_DEBUG_LOG] = {'\0'};

	int8_t productTank[SIZE_BUFFER_DATE_DEBUG_LOG + 2] = {'\0'};
	int8_t unitTemp[SIZE_BUFFER_DATE_DEBUG_LOG + 2] = {'\0'};
	int8_t compOutletTemp[SIZE_BUFFER_DATE_DEBUG_LOG + 2] = {'\0'};
	int8_t measurementFR[SIZE_BUFFER_DATE_DEBUG_LOG + 2] = {'\0'};
	int8_t o2Concentration[SIZE_BUFFER_DATE_DEBUG_LOG + 2] = {'\0'};

	sprintf((char*)year,"%.2d",logPtr->year);
	sprintf((char*)month,"%.2d",logPtr->month);
	sprintf((char*)day,"%.2d",logPtr->day);
	sprintf((char*)hour,"%.2d",logPtr->hour);
	sprintf((char*)min,"%.2d",logPtr->min);
	sprintf((char*)sec,"%.2d",logPtr->sec);

	float productTankValue = logPtr->productTank /SCALE_DEBUG_LOG;
	float unitTempValue = logPtr->unitTemp /SCALE_DEBUG_LOG;
	float compOutletTempValue = logPtr->compOutletTemp /SCALE_DEBUG_LOG;
	float measurementFRValue = logPtr->measurementFR /SCALE_DEBUG_LOG;
	float o2ConcentrationValue = logPtr->o2Concentration /SCALE_DEBUG_LOG;

	sprintf((char*)productTank,"%.2f",productTankValue);
	sprintf((char*)unitTemp,"%.2f",unitTempValue);
	sprintf((char*)compOutletTemp,"%.2f",compOutletTempValue);
	sprintf((char*)measurementFR,"%.2f",measurementFRValue);
	sprintf((char*)o2Concentration,"%.0f",o2ConcentrationValue);

	f_write(&fileObj, (char*)year, strlen((char*)year), &bw);
	f_write(&fileObj, ",", 1, &bw);

	f_write(&fileObj, (char*)month, strlen((char*)month), &bw);
	f_write(&fileObj, ",", 1, &bw);

	f_write(&fileObj, (char*)day, strlen((char*)day), &bw);
	f_write(&fileObj, ",", 1, &bw);

	f_write(&fileObj, (char*)hour, strlen((char*)hour), &bw);
	f_write(&fileObj, ",", 1, &bw);

	f_write(&fileObj, (char*)min, strlen((char*)min), &bw);
	f_write(&fileObj, ",", 1, &bw);

	f_write(&fileObj, (char*)sec, strlen((char*)sec), &bw);
	f_write(&fileObj, ",", 1, &bw);

	f_write(&fileObj, (char*)productTank, strlen((char*)productTank), &bw);
	f_write(&fileObj, ",", 1, &bw);

	f_write(&fileObj, (char*)unitTemp, strlen((char*)unitTemp), &bw);
	f_write(&fileObj, ",", 1, &bw);

	f_write(&fileObj, (char*)compOutletTemp, strlen((char*)compOutletTemp), &bw);
	f_write(&fileObj, ",", 1, &bw);

	f_write(&fileObj, (char*)measurementFR, strlen((char*)measurementFR), &bw);
	f_write(&fileObj, ",", 1, &bw);

	f_write(&fileObj, (char*)o2Concentration, strlen((char*)o2Concentration), &bw);
	f_write(&fileObj, ",", 1, &bw);

	if(logPtr->other == FAN_AND_FILTER_ERROR_DEBUG_LOG)
	{
		f_write(&fileObj, "03", 2, &bw);
	}
	else if (logPtr->other == FILTER_ERROR_DEBUG_LOG)
	{
		f_write(&fileObj, "02", 2, &bw);
	}
	else
	{
		f_write(&fileObj, "01", 2, &bw);
	}


	f_write(&fileObj, ",", 1, &bw);

	f_write(&fileObj, "\r\n", 2, &bw);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-usbMgr_DumpAllLog													    	*/
/*                                                        	                            */
/* Details  -Dum all log 																*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE					 					  							*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : bool : result dump all log					   					        */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                 				                        */
/*                     (O) NONE											                */
/*【Note】                                                                              													*/
/*                                                                                      */
/****************************************************************************************/
bool usbMgr_DumpAllLog(void)
{
	//reset debug log read pointer
	debugLog_ResetLogIndex();
	//get total number of log in EEPROM
	int numOfLog = debugLog_GetNumberOfLog();
	if(numOfLog <= 0)
	{
		if (f_close(&fileObj))
		{
			DEBUG_USB("*******DIE %d*******\r\n");
		}
		return false;
	}

	//prepare a log object to hold log data
	DEBUG_LOG_STRUCT log;
	//loop to dump Log to File one by one
	for(int i = 0; i < numOfLog; i++)
	{
		//get a Log
		debugLog_GetNextLog(&log);
		//dump a log has been got to file
		usbMgr_WriteLogToFile((i+1), &log);
	}
	//write to file done, just close file
	if (f_close(&fileObj)) {
		DEBUG_USB("*******DIE %d*******\r\n");
		return false;
	}

	return true;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-usbMgr_ExportDebugLog													    */
/*                                                        	                            */
/* Details  -store debug log into FH-310 from files in USB memory						*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE					 					  							*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : bool : result export debug log to csv		   					        */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                 				                        */
/*                     (O) NONE											                */
/*【Note】                                                                              													*/
/*                                                                                      */
/****************************************************************************************/
static bool usbMgr_ExportDebugLog(void)
{
	if(usbMgr_PrepareCSVFile() == false)
		return false;		//return with error

	//prepare file OK, go to Dump data to file
	usbMgr_DumpAllLog();
	return true;
}

/******************************************************************************/
//COMMON.OPERATION
//    Function Name: usbMgr_Handle
//
//    Detail: usb handle
//
//    Arguments: (I)
//				 (O)
//
//    Return Values: NONE
//
//
//    Using Global Data : (I)
//						  (O)
//
//
//    Note: None
//
/******************************************************************************/
void usbMgr_Handle(void)
{
	USB_MSG_STRUCT_t msg;

	if(xQueueReceive(USBHandleQueue, &msg, NO_WAIT)== pdTRUE)
	{
		GUI_EVENT_STRUCT event;

		switch (msg.id)
		{
		case eRequestPassword:
		{
			event.id = eCheckPWFileResultEventId;
			event.data.charData = usbMgr_CheckPWFile();
			guiInterface_SendGuiEvent(event);
			break;
		}
		case eRequestUpdate:
		{
			event.id = eCheckFWFileResultEventId;
			event.data.charData = usbMgr_CheckFWFile();
			guiInterface_SendGuiEvent(event);
			break;
		}
		case eRequestSaveLog:
		{
			event.id = eResultSaveLogEventId;
			event.data.charData = usbMgr_SaveLogToUSB();
			guiInterface_SendGuiEvent(event);
			break;
		}
		case eRequestEEPROMDump:
		{
			event.id = eDumpFileEEPROMResultEventId;
			event.data.charData = usbMgr_DumpEEPROMMemory();
			guiInterface_SendGuiEvent(event);
			break;
		}
		case eRequestCBXLogDump:
		{
			event.id = eDumpFileCBXLogResultEventId;
			event.data.charData = usbMgr_DumpCBXUsageLog();
			guiInterface_SendGuiEvent(event);
			break;
		}
		case eRequestAnypalLogDump:
		{
			event.id = eDumpFileAnypalLogResultEventId;
			event.data.charData = usbMgr_DumpAnypalUsageLog();
			guiInterface_SendGuiEvent(event);
			break;
		}
		case eRequestSpO2PulseLogDump:
		{
			event.id = eDumpFileSpO2PulseLogResultEventId;
			event.data.charData = usbMgr_DumpSpO2PulseLog();
			guiInterface_SendGuiEvent(event);
			break;
		}
		case eRequestSRAMDump:
		{
			event.id = eDumpFileSRAMLogResultEventId;
			event.data.charData = usbMgr_DumpMemoryToFile(eSRAMId);
			guiInterface_SendGuiEvent(event);
			break;
		}
		case eRequestSDRAMDump:
		{
			event.id = eDumpFileSDRAMLogResultEventId;
			event.data.charData = usbMgr_DumpMemoryToFile(eSDRAMId);
			guiInterface_SendGuiEvent(event);
			break;
		}
		case eRequestFlashADump:
		{
			event.id = eDumpFileFlashALogResultEventId;
			event.data.charData = usbMgr_DumpMemoryToFile(eFlashAId);
			guiInterface_SendGuiEvent(event);
			break;
		}
		case eRequestFlashBDump:
		{
			event.id = eDumpFileFlashBLogResultEventId;
			event.data.charData = usbMgr_DumpMemoryToFile(eFlashBId);
			guiInterface_SendGuiEvent(event);
			break;
		}
		case eRequestFlashSPIDump:
		{
			event.id = eDumpFileFlashSPILogResultEventId;
			event.data.charData = usbMgr_DumpMemoryToFile(eSPIFlashId);
			guiInterface_SendGuiEvent(event);
			break;
		}
		case eRequestEEPRomUpload:
		{
			event.id = eUploadEEPROMResultEventId;;
			event.data.charData = usbMgr_UploadMemory(eEEPROMFileId);
			guiInterface_SendGuiEvent(event);
			break;
		}
		case eRequestCBXLogUpload:
		{
			event.id = eUploadCBXResultEventId;;
			event.data.charData = usbMgr_UploadMemory(eCBXLogFileId);
			guiInterface_SendGuiEvent(event);
			break;
		}
		case eRequestAnypalUpload:
		{
			event.id = eUploadAnypalResultEventId;;
			event.data.charData = usbMgr_UploadMemory(eAnypalLogFileId);
			guiInterface_SendGuiEvent(event);
			break;
		}
		case eRequestSpO2PulseUpload:
		{
			event.id = eUploadSPO2PulseResultEventId;
			event.data.charData = usbMgr_UploadMemory(eSpO2PulseLogId);
			guiInterface_SendGuiEvent(event);
			break;
		}
		case eRequestTurnOnAnypalVbus:
		{
			if(usbMgr_GetOverCurrentStatus(ANYPAL_PORT) == false)
			{
				usbMgr_TurnOnVBUS(ANYPAL_PORT);
			}
			break;
		}
		case eRequestTurnOffAnypalVbus:
		{
			usbMgr_TurnOffVBUS(ANYPAL_PORT);
			break;
		}
		case eRequestTurnOnUSBVbus:
		{
			if(usbMgr_GetOverCurrentStatus(USB_PORT) == false)
			{
				usbMgr_TurnOnVBUS(USB_PORT);
			}
			break;
		}
		case eRequestTurnOffUSBVbus:
		{
			usbMgr_TurnOffVBUS(USB_PORT);
			break;
		}
		case eRequestTurnOffAnypalVbusWhenOVCR:
		{
			usbMgr_TurnOffVBUSAlarmOverCurrent();
			break;
		}
		case eRequestExportDebugLog:
		{
			event.id = eExportFileDebugLogEventId;
			event.data.charData = usbMgr_ExportDebugLog();
			guiInterface_SendGuiEvent(event);
			break;
		}
		case HID_BARCODE_ENUM_COMPLETE_ID:
			hidBarcode_SetInterface(msg.data.hDiskInterface);
			break;
		case MS_HOST_ENUM_COMPLETE_ID:
			msHost_SetInterface(msg.data.msHostDiskInterface);
			break;
		default:
			break;
		}
	}

	if(usbHostInterface_GetBarcodeConnectState() == true)
	{
		hidBarcode_Handle();
	}
	usbMgr_DetectOverCurrent();
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-usbMgr_getFirmwareFilesBoot	                    		    			*/
/*                                                                                      */
/* Details  -Get string name of bootloader file	 							 			*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE																	*/
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : int8_t* : string name		                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) s_boot_filename : buffer name of bootloader file			    */
/*                     (O) NONE                                           			   	*/
/*【Note】                                                                              													*/
/*                                                                                      */
/****************************************************************************************/
int8_t* usbMgr_getFirmwareFilesBoot(void)
{
	return s_boot_filename;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-usbMgr_getFirmwareFilesM0	                    		    				*/
/*                                                                                      */
/* Details  -Get string name of M0 file	 							 					*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE																	*/
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : int8_t* : string name		                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) s_m0_filename : buffer name of M0 file			   			*/
/*                     (O) NONE                                           			   	*/
/*【Note】                                                                              													*/
/*                                                                                      */
/****************************************************************************************/
int8_t* usbMgr_getFirmwareFilesM0(void)
{
	return s_m0_filename;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-usbMgr_getFirmwareFilesM4	                    		    				*/
/*                                                                                      */
/* Details  -Get string name of M4 file	 							 					*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE																	*/
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : int8_t* : string name		                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) s_m4_filename : buffer name of M4 file			   			*/
/*                     (O) NONE                                           			   	*/
/*【Note】                                                                              													*/
/*                                                                                      */
/****************************************************************************************/
int8_t* usbMgr_getFirmwareFilesM4(void)
{
	return s_m4_filename;
}

/* end of files */
