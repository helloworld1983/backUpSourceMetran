/********************************************************************/
/*                                                                  */
/* File Name    : UsbInterface.c                           			*/
/*                                                                  */
/* General      : USB interface support functions to access         */
/*                	to USB device from other places				    */
/*                 	 						                        */
/*                                                                  */
/* Product Name  : FH310                                            */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2017/08/22      Linh Nguyen(MV)  	new file		*/
/*  #0001	   2017/09/28      Viet Le				modify  		*/
/*	- Remove usb_getFirmwareFilesControlSW(void) function			*/
/*	- Add two function : usb_getFirmwareFilesM0(int8_t* fileName)	*/ 
/*		and usb_getFirmwareFilesM4(int8_t* fileName)				*/
/*  - Edit usb_getFirmwareFilesBoot(void)							*/
/*      -> usb_getFirmwareFilesBoot(int8_t* fileName)				*/
/********************************************************************/

#include "UsbInterface.h"
#include "MsHost.h"
#include "UsbMgr.h"
#include "updateFlash.h"

#define QUEUE_WAITING 500/portTICK_PERIOD_MS

extern QueueHandle_t USBHandleQueue;

/****************************************************************************************/
/*                                                                                      */
/* General 	-hidBarcode_SendMsg													    	*/
/*                                                                                      */
/* Details  -Send Event																	*/
/*                                                                                      */
/* Arguments : (I) HID_BARCODE_MSG_STRUCT_t msg	 					  					*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : bool		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) USBHandleQueue				                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/* local valiables   : NONE			                                                  	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
bool usb_SendMsg(USB_MSG_STRUCT_t msg)
{
	if(xQueueSend(USBHandleQueue, &msg,10) == pdFALSE)
	{
		DEBUGOUT("\n Failure send hid barcode msg");
		return false;
	}
	return true;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-usb_SetMsHosInterface												    	*/
/*                                                                                      */
/* Details  -Set Interface, only call when USB host task does not run					*/
/*                                                                                      */
/* Arguments : (I) USB_ClassInfo_MS_Host_t msHostInterface	 		  					*/
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
void usb_SetMsHosInterface(USB_ClassInfo_MS_Host_t msHostInterface)
{
	msHost_SetInterface(msHostInterface);
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-hidBarcode_SendMsg													    	*/
/*                                                                                      */
/* Details  -Send Event																	*/
/*                                                                                      */
/* Arguments : (I) HID_BARCODE_MSG_STRUCT_t msg	 					  					*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : bool		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) USBHandleQueue				                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/* local valiables   : NONE			                                                  	*/
/****************************************************************************************/
bool usb_SendEvent(USB_EVENT event)
{
	USB_MSG_STRUCT_t msg;
	msg.id = event;
	if(xQueueSend(USBHandleQueue, &msg, QUEUE_WAITING)== pdFALSE)
	{
		DEBUGOUT("Failed to send event to USB within 500m\n");
		return false;
	}
	return true;
}
//end Q_add


/****************************************************************************************/
/*                                                                                      */
/* General 	-usb_getFirmwareFilesBoot	                    		    				*/
/*                                                                                      */
/* Details  -Get string name of bootloader file	 							 			*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) int8_t* : string name												*/
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : bool : false:no update file											*/
/*						true:there are update file						                */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE														    */
/*                     (O) NONE                                           			   	*/
/*【Note】                                                                              													*/
/*                                                                                      */
/****************************************************************************************/
bool usb_getFirmwareFilesBoot(int8_t* fileName)
{
	int8_t* buffPtr = usbMgr_getFirmwareFilesBoot();
	int8_t strLen = strlen((char*)buffPtr);

	if(strLen <= FIRMWARE_NAME_ARRAY_SIZE)
	{
		memcpy(fileName,buffPtr,strLen);

		if(strncmp((char*)buffPtr,FIRMWARE_NAME_BOOTLOADER_STRING,BOOTLOADER_PREFIX_NAME_SIZE) == 0)
		{
			return true;
		}
	}

	return false;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-usb_getFirmwareFilesM0	                    		    					*/
/*                                                                                      */
/* Details  -Get string name of M0 file	 							 					*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) int8_t* : string name												*/
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : bool : false:no update file											*/
/*						true:there are update file						                */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE														    */
/*                     (O) NONE                                           			   	*/
/*【Note】                                                                              													*/
/*                                                                                      */
/****************************************************************************************/
bool usb_getFirmwareFilesM0(int8_t* fileName)
{
	int8_t* buffPtr = usbMgr_getFirmwareFilesM0();
	int8_t strLen = strlen((char*)buffPtr);

	if(strLen <= FIRMWARE_NAME_ARRAY_SIZE)
	{
		memcpy(fileName,buffPtr,strLen);

		if(strncmp((char*)buffPtr,FIRMWARE_NAME_M0_STRING,FIRMWARE_PREFIX_NAME_SIZE) == 0)
		{
			return true;
		}
	}

	return false;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-usb_getFirmwareFilesM4	                    		    					*/
/*                                                                                      */
/* Details  -Get string name of M4 file	 							 					*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) int8_t* : string name												*/
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : bool : false:no update file											*/
/*						true:there are update file						                */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE														    */
/*                     (O) NONE                                           			   	*/
/*【Note】                                                                              													*/
/*                                                                                      */
/****************************************************************************************/
bool usb_getFirmwareFilesM4(int8_t* fileName)
{
	int8_t* buffPtr = usbMgr_getFirmwareFilesM4();
	int8_t strLen = strlen((char*)buffPtr);

	if(strLen <= FIRMWARE_NAME_ARRAY_SIZE)
	{
		memcpy(fileName,buffPtr,strLen);

		if(strncmp((char*)buffPtr,FIRMWARE_NAME_M4_STRING,FIRMWARE_PREFIX_NAME_SIZE) == 0)
		{
			return true;
		}
	}

	return false;
}

/* end of files */
