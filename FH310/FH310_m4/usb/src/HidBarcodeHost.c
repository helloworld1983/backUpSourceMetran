/********************************************************************/
/*                                                                  */
/* File Name    : HidBarcodeHost.c			                        */
/*                                                                  */
/* General       : Comunicate with hid barcode device      			*/
/*                 	 						                        */
/*                                                                  */
/* Product Name  : FH310                                            */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2016/08/25      Quyen Ngo		  	new file        */
/* #0001       2017/09/23      Quyen Ngo		  	modify			*/
/*		-remove function hidBarcode_Enum							*/
/*		-remove meaningless fucntion								*/
/********************************************************************/

/* include system files */
#include <stdlib.h>

/* include user files */
#include "HidBarcodeHost.h"
#include "USB.h"
#include "board.h"
#include "TaskCommon.h"
#include "GuiTask.h"
#include "UsbInterface.h"
#include "GuiInterface.h"

/* constants, macro definition */
#define WAIT_MAX_CNT      20

/* definitions of structures */

/*global variable*/

//Q_remove Keyboard_HID_Interface
static USB_ClassInfo_HID_Host_t s_hDiskInterface; //replace FlashDisk_FTDI_Interface by s_hDiskInterface


/* external declaration */

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/****************************************************************************************/
/*                                                                                      */
/* Function name 	hidBarcode_KeyboardHostTask											*/
/*                                                                                      */
/* Details  Task for read data from barcode reader										*/
/*                                                                                      */
/* Arguments : (I)  NONE							 									*/
/*																						*/
/*                                                                                      */
/*             (O) 	NONE																*/
/*                                                                                      */
/* ReturnValue : NONE		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                                                                                      */
/****************************************************************************************/
void hidBarcode_KeyboardHostTask(void)
{
	uint8_t KeyCode;
	static uint8_t s_bCodeLength = 0;
	static unsigned char s_barcodeString[BARCODE_MAXLENGTH];

	static int16_t waitNextCharCnt = 0;

	if (USB_HostState[s_hDiskInterface.Config.PortNumber] != HOST_STATE_Configured) {
		return;
	}

	if (HID_Host_IsReportReceived(&s_hDiskInterface)) {
		USB_KeyboardReport_Data_t KeyboardReport;
		memset(&KeyboardReport, 0, sizeof(KeyboardReport));
		HID_Host_ReceiveReport(&s_hDiskInterface, &KeyboardReport);

		KeyCode = KeyboardReport.KeyCode[0];

		if (KeyCode) {
			char PressedKey = 0;

			/* Retrieve pressed key character if alphanumeric */
			if ((KeyCode >= HID_KEYBOARD_SC_A) && (KeyCode <= HID_KEYBOARD_SC_Z)) {
				PressedKey = (KeyCode - HID_KEYBOARD_SC_A) + 'A';
			}
			else if ((KeyCode >= HID_KEYBOARD_SC_1_AND_EXCLAMATION) &
					(KeyCode  < HID_KEYBOARD_SC_0_AND_CLOSING_PARENTHESIS)) {
				PressedKey = (KeyCode - HID_KEYBOARD_SC_1_AND_EXCLAMATION) + '1';
			}
			else if (KeyCode == HID_KEYBOARD_SC_0_AND_CLOSING_PARENTHESIS) {
				PressedKey = '0';
			}
			else if (KeyCode == HID_KEYBOARD_SC_SPACE) {
				PressedKey = ' ';
			}
			else if (KeyCode == HID_KEYBOARD_SC_ENTER) {
				PressedKey = '\n';
			}

			if (PressedKey) {
				s_barcodeString[s_bCodeLength] = PressedKey;
				s_bCodeLength++;
				waitNextCharCnt = 0; //notice that received char and start count for wait to next char
			}
		}
	}
	else
	{
		if(s_bCodeLength > 0) //allow cnt
		{
			waitNextCharCnt++;
			if(waitNextCharCnt == WAIT_MAX_CNT)
			{
				DEBUGOUT("code: %s length %d \n",s_barcodeString,s_bCodeLength);
				//Send serial to GUI
				BARCODE_SERIAL_PACKET_T stSerialPacket;
				stSerialPacket.id = 0x00;
				stSerialPacket.length = s_bCodeLength;
				memcpy(stSerialPacket.barcodeString, s_barcodeString,s_bCodeLength);

				GUI_EVENT_STRUCT event;
				event.id = eBarcodeSerialEventId;
				event.data.barcode = stSerialPacket;
				guiInterface_SendGuiEvent(event);

				waitNextCharCnt = 0;
				s_bCodeLength = 0;
				memset(s_barcodeString, 0, BARCODE_MAXLENGTH);
			}
		}
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name 	hidBarcode_Handle													*/
/*                                                                                      */
/* Details  handle read data from barcode												*/
/*                                                                                      */
/* Arguments : (I)  NONE							 									*/
/*																						*/
/*                                                                                      */
/*             (O) 	NONE																*/
/*                                                                                      */
/* ReturnValue : NONE		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                                                                                      */
/****************************************************************************************/
void hidBarcode_Handle(void)
{
		hidBarcode_KeyboardHostTask();
		HID_Host_USBTask(&s_hDiskInterface);
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name 	hidBarcode_SetInterface												*/
/*                                                                                      */
/* Details Set Interface for HID class													*/
/*                                                                                      */
/* Arguments : (I)  USB_ClassInfo_HID_Host_t hidBarcodeInterface						*/
/*																						*/
/*                                                                                      */
/*             (O) 	NONE																*/
/*                                                                                      */
/* ReturnValue : NONE		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                                                                                      */
/****************************************************************************************/
void hidBarcode_SetInterface(USB_ClassInfo_HID_Host_t hidBarcodeInterface)
{
	s_hDiskInterface = hidBarcodeInterface;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name 	CALLBACK_HIDParser_FilterHIDReportItem								*/
/*                                                                                      */
/* Details Dummy callback function for HID Parser										*/
/*                                                                                      */
/* Arguments : (I)  HID_ReportItem_t *const CurrentItem									*/
/*																						*/
/*                                                                                      */
/*             (O) 	NONE																*/
/*                                                                                      */
/* ReturnValue : NONE		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                                                                                      */
/****************************************************************************************/
bool CALLBACK_HIDParser_FilterHIDReportItem(HID_ReportItem_t *const CurrentItem)
{
	return true;
}
