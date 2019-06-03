/********************************************************************/
/*                                                                  */
/* File Name    : USBHostTask.c                               		*/
/*                                                                  */
/* General      : This file contains the function for create and 	*/
/*                 run USB host freeRTOS task, which is responsible */
/*                for process USB host event and feed Watchdog      */
/*                                                                  */
/* Product Name  : FH310                                            */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2017/02/14       Quyen Ngo(MV)  	    new file        */
/*  #0001           2017/08/24      Linh Nguyen		  	modify		  */
/*  update flag for handle bootloader update failure				*/
/*  #0002         2017/09/01      Linh Nguyen		  	modify		    */
/*  Fix re-enum failure problem										*/
/*  #0003      2017/09/15      Linh Nguyen		  	modify		    */
/*  Send push out anypal event										*/
/*	#0004	   2017/09/23		Quyen Ngo(MV)		modify			*/
/*	-Change the name function follow coding rule, change enum		*/
/*	function to static												*/
/*  #0005      2017/09/28		    Linh Nguyen		  	modify		*/
/*  Change function name for coding rule, remove unuse code			*/
/*  #0006	   2017/12/13	   Quyen Ngo(MV)		modify			*/
/*		Add void to non parameter function to follow coding rule	*/
/********************************************************************/

/* include system files */

/* include user files */
#include "USBHostTask.h"

#include "TaskCommon.h"
#include "UsbCommonDef.h"
#include "AnypalInterface.h"
#include "UsbInterface.h"
#include "GuiInterface.h"
#include "timers.h"
#include "UsbHostInterface.h"

/* constants, macro definition */
#ifdef DEBUG
#define DEBUG_USB_EN
#endif

#ifdef DEBUG_USB_EN
#define DEBUG_USB(...) printf(__VA_ARGS__)
#else
#define DEBUG_USB(...)
#endif

#define USB_HOST_TASK_STACK_SIZE			1024

#define CONFIG_DESCRIPTOR_DATA_MAX_SIZE 		512
#define HOST_STATE_CONFIG_ID					1
#define HOST_STATE_ADDR_ID						0

extern TimerHandle_t g_turnOnAnypalTimer;
extern void set_STA_NOINIT(void);

static bool s_anypalPushOutEvent = false;

/* definitions of structures */

/*global variable*/
TaskHandle_t xUsbTaskHandle = NULL;
bool s_isHostTaskRunning = false;
/* external declaration */

/** LPCUSBlib Mass Storage Class driver interface configuration and state information. This structure is
 *  passed to all Mass Storage Class driver functions, so that multiple instances of the same class
 *  within a device can be differentiated from one another.
 */
static USB_ClassInfo_MS_Host_t FlashDisk_MS_Interface = {
		.Config = {
				.DataINPipeNumber       = 1,
				.DataINPipeDoubleBank   = false,

				.DataOUTPipeNumber      = 2,
				.DataOUTPipeDoubleBank  = false,
				.PortNumber = 1,
		},
};

/** LPCUSBlib Mass Storage Class driver interface configuration and state information. This structure is
 *  passed to all Mass Storage Class driver functions, so that multiple instances of the same class
 *  within a device can be differentiated from one another.
 */

static USB_ClassInfo_FTDI_Host_t FlashDisk_FTDI_Interface =
{
		.Config =
		{
				.DataINPipeNumber           = 1,
				.DataINPipeDoubleBank       = false,

				.DataOUTPipeNumber          = 2,
				.DataOUTPipeDoubleBank      = false,

				.PortNumber = 0,
		},
};

/** LPCUSBlib HID Class driver interface configuration and state information. This structure is
 *  passed to all HID Class driver functions, so that multiple instances of the same class
 *  within a device can be differentiated from one another.
 */
static USB_ClassInfo_HID_Host_t Keyboard_HID_Interface = {
		.Config = {
				.DataINPipeNumber       = 1,
				.DataINPipeDoubleBank   = false,

				.DataOUTPipeNumber      = 2,
				.DataOUTPipeDoubleBank  = false,

				.HIDInterfaceProtocol   = HID_CSCP_KeyboardBootProtocol,
				.PortNumber = 1,
		},
};

/****************************************************************************************/
/*                                                                                      */
/* General 	-usbTask_Run															    */
/*                                                                                      */
/* Details  -Run function of Usb task													*/
/* 			+Call USB USB_HostTask to process USB host event on USB port 0 and 1		*/
/* 				when host stage change during enumuration process						*/
/* 			+Feed watchdog																*/
/*					 																	*/
/* 																		      			*/
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
/* local valiables   : NONE			                                                  	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
static void usbHostTask_Run(void)
{
	while(1)
	{
		if(s_anypalPushOutEvent == true)
		{
			s_anypalPushOutEvent = false;
			anypalInterface_SendEvent(eAnypalPushOut);
		}
		if(s_isHostTaskRunning == false)
		{
			s_isHostTaskRunning = true;
		}
		guiInterface_WWDTFeed(LPC_WWDT);
		USB_USBTask(ANYPAL_PORT, USB_MODE_Host);
		USB_USBTask(USB_PORT, USB_MODE_Host);
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name 	-usbHost_EnumFTDIDevice											    */
/*                                                                                      */
/* Details  -Process FTDI device enumeration											*/
/*					 																	*/
/* 																		      			*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) uint8_t corenum					 					  				*/
/*				range: {0,1}															*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) USB_ClassInfo_FTDI_Host_t FlashDisk_FTDI_Interface          	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
static void usbHost_EnumFTDIDevice(const uint8_t corenum)
{
	uint16_t ConfigDescriptorSize;
	uint8_t  ConfigDescriptorData[512];

	if (USB_Host_GetDeviceConfigDescriptor(corenum, 1, &ConfigDescriptorSize, ConfigDescriptorData,
			sizeof(ConfigDescriptorData)) != HOST_GETCONFIG_Successful) {
		DEBUGOUT("Error Retrieving Configuration Descriptor.\r\n");
		//		anypal_ActiveAlarmCommunication();Q_remove
		return;
	}

	FlashDisk_FTDI_Interface.Config.PortNumber = corenum;

	if (FTDI_Host_ConfigurePipes(&FlashDisk_FTDI_Interface,
			ConfigDescriptorSize, ConfigDescriptorData) != CDC_ENUMERROR_NoError)
	{
		DEBUGOUT("Attached Device Not a Valid FTDI Class Device.\r\n");
		//LEDs_SetAllLEDs(LEDMASK_USB_ERROR);
		//		anypal_ActiveAlarmCommunication();Q_remove
		return;
	}

	if (USB_Host_SetDeviceConfiguration(FlashDisk_FTDI_Interface.Config.PortNumber, 1) != HOST_SENDCONTROL_Successful) {
		DEBUGOUT("Error Setting Device Configuration.\r\n");
		//		anypal_ActiveAlarmCommunication();Q_remove
		return;
	}

	if (FTDI_Host_Reset(&FlashDisk_FTDI_Interface))
	{
		DEBUGOUT("Error reset communication.\r\n");
		//		anypal_ActiveAlarmCommunication();Q_remove
		return;
	}

	FlashDisk_FTDI_Interface.State.LineEncoding.BaudRateBPS = 38400;
	FlashDisk_FTDI_Interface.State.LineEncoding.CharFormat  = CDC_LINEENCODING_OneStopBit;
	FlashDisk_FTDI_Interface.State.LineEncoding.ParityType  = CDC_PARITY_None;
	FlashDisk_FTDI_Interface.State.LineEncoding.DataBits    = 8;

	if (FTDI_Host_SetBaudrate(&FlashDisk_FTDI_Interface))
	{
		DEBUGOUT("Error set baud rate.\r\n");
		//		anypal_ActiveAlarmCommunication();Q_remove
		return;
	}

	if (FTDI_Host_SetData(&FlashDisk_FTDI_Interface))
	{
		DEBUGOUT("Error set baud rate.\r\n");
		//		anypal_ActiveAlarmCommunication();Q_remove
		return;
	}

	if (FTDI_Host_DisableFlowControl(&FlashDisk_FTDI_Interface))
	{
		DEBUGOUT("Error disable flow control.\r\n");
		//		anypal_ActiveAlarmCommunication();Q_remove
		return;
	}

	if (FTDI_Host_DisableModemControl(&FlashDisk_FTDI_Interface))
	{
		DEBUGOUT("Error disable modem control.\r\n");
		//		anypal_ActiveAlarmCommunication();Q_remove
		return;
	}
	if(s_isHostTaskRunning == true)
	{

		//Q_add
		ANYPAL_MSG_STRUCT_t msg;
		msg.id = eAnypalEnumComplete;
		msg.data.hDiskInterface = FlashDisk_FTDI_Interface;
		anypalInterface_SendMsg(msg);
		//end
	}
	DEBUGOUT("\nFTDI Enumerated.\r\n");
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name 	usbHost_EnumHidBarcode											    */
/*                                                                                      */
/* Details  -Process HID barcode device enumeration										*/
/*					 																	*/
/* 																		      			*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) uint8_t corenum					 					  				*/
/*				range: {0,1}															*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) USB_ClassInfo_HID_Host_t Keyboard_HID_Interface          	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
static void usbHost_EnumHidBarcode(const uint8_t corenum)
{
	uint16_t ConfigDescriptorSize;
	uint8_t  ConfigDescriptorData[512];
	if (USB_Host_GetDeviceConfigDescriptor(corenum, 1, &ConfigDescriptorSize, ConfigDescriptorData,
			sizeof(ConfigDescriptorData)) != HOST_GETCONFIG_Successful) {
		DEBUGOUT("Error Retrieving Configuration Descriptor.\r\n");

		return;
	}

	Keyboard_HID_Interface.Config.PortNumber = corenum;
	if (HID_Host_ConfigurePipes(&Keyboard_HID_Interface,
			ConfigDescriptorSize, ConfigDescriptorData) != HID_ENUMERROR_NoError) {
		DEBUGOUT("Attached Device Not a Valid Keyboard.\r\n");

		return;
	}

	if (USB_Host_SetDeviceConfiguration(Keyboard_HID_Interface.Config.PortNumber, 1) != HOST_SENDCONTROL_Successful) {
		DEBUGOUT("Error Setting Device Configuration.\r\n");

		return;
	}

	if (HID_Host_SetBootProtocol(&Keyboard_HID_Interface) != 0) {
		DEBUGOUT("Could not Set Boot Protocol Mode.\r\n");

		USB_Host_SetDeviceConfiguration(Keyboard_HID_Interface.Config.PortNumber, 0);
		return;
	}

	if(s_isHostTaskRunning == true)
	{
		//Q_add
		USB_MSG_STRUCT_t msg;
		msg.id = HID_BARCODE_ENUM_COMPLETE_ID;
		msg.data.hDiskInterface = Keyboard_HID_Interface;
		usb_SendMsg(msg);
		//end
	}
	usbHostInterface_SetBarcodeConnectState(true);

	DEBUGOUT("Keyboard Enumerated.\r\n");
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name 	usbHost_EnumMscProcess											    */
/*                                                                                      */
/* Details  -Process Mass storage device enumeration									*/
/*					 																	*/
/* 																		      			*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) uint8_t corenum					 					  				*/
/*				range: {0,1}															*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) USB_ClassInfo_MS_Host_t FlashDisk_MS_Interface     	     	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
static void usbHost_EnumMscProcess(const uint8_t corenum)
{
	uint16_t ConfigDescriptorSize;
	uint8_t  ConfigDescriptorData[CONFIG_DESCRIPTOR_DATA_MAX_SIZE];

	if (USB_Host_GetDeviceConfigDescriptor(corenum, HOST_STATE_CONFIG_ID, &ConfigDescriptorSize, ConfigDescriptorData,
			sizeof(ConfigDescriptorData)) != HOST_GETCONFIG_Successful) {
		DEBUGOUT("Error Retrieving Configuration Descriptor.\r\n");
		return;
	}

	FlashDisk_MS_Interface.Config.PortNumber = corenum;
	if (MS_Host_ConfigurePipes(&FlashDisk_MS_Interface,
			ConfigDescriptorSize, ConfigDescriptorData) != MS_ENUMERROR_NoError) {
		DEBUGOUT("Attached Device Not a Valid Mass Storage Device.\r\n");
		return;
	}

	if (USB_Host_SetDeviceConfiguration(FlashDisk_MS_Interface.Config.PortNumber, HOST_STATE_CONFIG_ID) != HOST_SENDCONTROL_Successful) {
		DEBUGOUT("Error Setting Device Configuration.\r\n");
		return;
	}

	uint8_t MaxLUNIndex;
	if (MS_Host_GetMaxLUN(&FlashDisk_MS_Interface, &MaxLUNIndex)) {
		DEBUGOUT("Error retrieving max LUN index.\r\n");
		USB_Host_SetDeviceConfiguration(FlashDisk_MS_Interface.Config.PortNumber, HOST_STATE_ADDR_ID);
		return;
	}

	DEBUGOUT(("Total LUNs: %d - Using first LUN in device.\r\n"), (MaxLUNIndex + 1));

	if (MS_Host_ResetMSInterface(&FlashDisk_MS_Interface)) {
		DEBUGOUT("Error resetting Mass Storage interface.\r\n");
		USB_Host_SetDeviceConfiguration(FlashDisk_MS_Interface.Config.PortNumber, HOST_STATE_ADDR_ID);
		return;
	}

	SCSI_Request_Sense_Response_t SenseData;
	if (MS_Host_RequestSense(&FlashDisk_MS_Interface, 0, &SenseData) != 0) {
		DEBUGOUT("Error retrieving device sense.\r\n");
		USB_Host_SetDeviceConfiguration(FlashDisk_MS_Interface.Config.PortNumber, HOST_STATE_ADDR_ID);
		return;
	}

	SCSI_Inquiry_Response_t InquiryData;
	if (MS_Host_GetInquiryData(&FlashDisk_MS_Interface, 0, &InquiryData)) {
		DEBUGOUT("Error retrieving device Inquiry data.\r\n");
		USB_Host_SetDeviceConfiguration(FlashDisk_MS_Interface.Config.PortNumber, HOST_STATE_ADDR_ID);
		return;
	}

	if(s_isHostTaskRunning == true)
	{
		//Q_add
		USB_MSG_STRUCT_t msg;
		msg.id = MS_HOST_ENUM_COMPLETE_ID;
		msg.data.msHostDiskInterface = FlashDisk_MS_Interface;
		usb_SendMsg(msg);
		//end
	}
	else
	{
		usb_SetMsHosInterface(FlashDisk_MS_Interface);
	}

	/* DEBUGOUT("Vendor \"%.8s\", Product \"%.16s\"\r\n", InquiryData.VendorID, InquiryData.ProductID); */
	DEBUGOUT("Mass Storage Device Enumerated.\r\n");
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name 	usbHostTask_Create												    */
/*                                                                                      */
/* Details  Create USB host task														*/
/*					 																	*/
/* 																		      			*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE								 					  				*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) TaskHandle_t xUsbTaskHandle					     	     	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void usbHostTask_Create(void)
{
	/* Creat  USB host Task */
	xTaskCreate( (TaskFunction_t)usbHostTask_Run, "USB Host",
			USB_HOST_TASK_STACK_SIZE, (void *)1, TASK_PRIO_USB_HOST_TASK,
			&xUsbTaskHandle );
	return;
}

/** Event handler for the USB_DeviceAttached event. This indicates that a device has been attached to the host, and
 *  starts the library USB task to begin the enumeration and USB management process.
 */
void EVENT_USB_Host_DeviceAttached(const uint8_t corenum)
{
	DEBUGOUT(("Device Attached on port %d\r\n"), corenum);
	if(!GetUpdateResult() && corenum == USB_PORT)
	{
		SetUpdateResult(true);
	}
	return;
}

/** Event handler for the USB_DeviceUnattached event. This indicates that a device has been removed from the host, and
 *  stops the library USB task management process.
 */

void EVENT_USB_Host_DeviceUnattached(const uint8_t corenum)
{
	if(s_isHostTaskRunning == true)
	{
		if(corenum == USB_PORT)
		{
			usbHostInterface_SetBarcodeConnectState(false);
			set_STA_NOINIT();
		}
		else if(corenum == ANYPAL_PORT)
		{
			s_anypalPushOutEvent = true;
		}
	}
	DEBUGOUT(("\r\nDevice Unattached on port %d\r\n"), corenum);

	return;
}


/** Event handler for the USB_DeviceEnumerationComplete event. This indicates that a device has been successfully
 *  enumerated by the host and is now ready to be used by the application.
 */
void EVENT_USB_Host_DeviceEnumerationComplete(const uint8_t corenum)
{
	uint8_t ConfigHeader[sizeof(USB_Descriptor_Device_t)] = {0};
	USB_Host_GetDeviceDescriptor(corenum,ConfigHeader);

	static bool s_USB1Enumerative = false;


	uint16_t idVendor = DESCRIPTOR_IDVENDOR(ConfigHeader);
	DEBUG_USB("id vendor: %x \n",idVendor);

	if (corenum == ANYPAL_PORT)
	{
		if (idVendor == FTDI_VENDOR)
		{
			//FTDI Device

			usbHost_EnumFTDIDevice(corenum);
			if (s_USB1Enumerative == false)
			{
				usb_SendEvent(eRequestTurnOffUSBVbus);
				if (xTimerStart(g_turnOnAnypalTimer,10) != pdPASS)
				{
					DEBUGOUT("ERR: Failed to start gs_turnOnVBUSTimer \n");
				}
			}
		}
	}
	else if (corenum == USB_PORT)
	{
		if ((idVendor == HDI_BARCODE_VENDOR1)||(idVendor == HDI_BARCODE_VENDOR2))
		{
			//HID Device
			usbHost_EnumHidBarcode(corenum);
		}
		else
		{
			//Mass storage device class
			usbHost_EnumMscProcess(corenum);
		}
	}

	s_USB1Enumerative = true;

	return;
}

/** Event handler for the USB_DeviceEnumerationFailed event. This indicates that a problem occurred while
 *  enumerating an attached USB device.
 */
void EVENT_USB_Host_DeviceEnumerationFailed(const uint8_t corenum,
		const uint8_t ErrorCode,
		const uint8_t SubErrorCode)
{
	//TODO Q_add send msg to Anypal task
	//	if(corenum == ANYPAL_PORT)
	//	{
	//		anypal_ActiveAlarmCommunication();
	//	}
	DEBUGOUT(("Dev Enum Error\r\n"
			" -- Error port %d\r\n"
			" -- Error Code %d\r\n"
			" -- Sub Error Code %d\r\n"
			" -- In State %d\r\n" ),
			corenum, ErrorCode, SubErrorCode, USB_HostState[corenum]);
	return;
}
