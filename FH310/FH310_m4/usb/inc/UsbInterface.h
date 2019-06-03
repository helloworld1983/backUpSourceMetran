/********************************************************************/
/*                                                                  */
/* File Name    : UsbInterface.h                           			*/
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
/*  		   2017/10/28      Linh Nguyen(MV)  	    modify	    */
/* #0002 Add the dump SPI flash memory functionality (bug #1992)	*/
/********************************************************************/

#ifndef INC_USBINTERFACE_H_
#define INC_USBINTERFACE_H_

#include <lpc_types.h>
#include "UsbCommonDef.h"
#include "TaskCommon.h"
//Q_add
typedef enum
{
	eRequestUpdate,
	eRequestSaveLog,
	eRequestPassword,
	eRequestEEPROMDump,
	eRequestCBXLogDump,
	eRequestAnypalLogDump,
	eRequestSpO2PulseLogDump,
	eRequestSRAMDump,
	eRequestSDRAMDump,
	eRequestFlashADump,
	eRequestFlashBDump,
	eRequestFlashSPIDump,
	eRequestEEPRomUpload,
	eRequestCBXLogUpload,
	eRequestAnypalUpload,
	eRequestSpO2PulseUpload,
	eRequestTurnOnAnypalVbus,
	eRequestTurnOffAnypalVbus,
	eRequestTurnOnUSBVbus,
	eRequestTurnOffUSBVbus,
	eRequestTurnOffAnypalVbusWhenOVCR,
	eRequestExportDebugLog,
	HID_BARCODE_NONE_ENVENT_ID,
	HID_BARCODE_ENUM_COMPLETE_ID,
	MS_HOST_ENUM_COMPLETE_ID
}USB_EVENT;

//Define Anypal message data
typedef union
{
	USB_ClassInfo_MS_Host_t msHostDiskInterface;
	USB_ClassInfo_HID_Host_t hDiskInterface;
}USB_MSG_DATA;

//Define packet send to GUI
typedef struct{
	USB_EVENT id;//Id packet
	USB_MSG_DATA data;//Data send
}USB_MSG_STRUCT_t;
//end Q_add

bool usb_SendMsg(USB_MSG_STRUCT_t msg);

void usb_SetMsHosInterface(USB_ClassInfo_MS_Host_t msHostInterface);

bool usb_SendEvent(USB_EVENT event);

bool usb_getFirmwareFilesBoot(int8_t* fileName);
bool usb_getFirmwareFilesM0(int8_t* fileName);
bool usb_getFirmwareFilesM4(int8_t* fileName);

#endif /* INC_USBINTERFACE_H_ */
