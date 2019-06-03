/******************************************************************************/
//
//  File Name		: FTDIClassHost.c
//  
//	General       	: XXXX 
//
//  Product Name	: lpcusblib_MassStorageHost
//
//
//  Revision History:
//          Rev:      Date:       		Editor:         
//          01        Nov 21, 2016	        Quyen Ngo(MV)         
//          Description:  InitialRevision
//
/******************************************************************************/

/* include system files */

/* include user files */

/* constants, macro definition */

/* definitions of structures */

/*global variable*/

/* external declaration */

#define  __INCLUDE_FROM_USB_DRIVER
#include "../../Core/USBMode.h"

#if defined(USB_CAN_BE_HOST)

#define  __INCLUDE_FROM_FTDI_DRIVER
#define  __INCLUDE_FROM_FTDI_HOST_C
#include "FTDIClassHost.h"
#include "board.h"

/*
 * 2017/1/12 Quyen Ngo Delete Pipe_SetInterruptPeriod Bug 2047
 */
uint8_t FTDI_Host_ConfigurePipes(USB_ClassInfo_FTDI_Host_t* const CDCInterfaceInfo,
		uint16_t ConfigDescriptorSize,
		void* ConfigDescriptorData)
{
	USB_Descriptor_Endpoint_t*  DataINEndpoint       = NULL;
	USB_Descriptor_Endpoint_t*  DataOUTEndpoint      = NULL;
	USB_Descriptor_Interface_t* FTDIControlInterface  = NULL;
	uint8_t portnum = CDCInterfaceInfo->Config.PortNumber;

	memset(&CDCInterfaceInfo->State, 0x00, sizeof(CDCInterfaceInfo->State));

	if (DESCRIPTOR_TYPE(ConfigDescriptorData) != DTYPE_Configuration)
		return FTDI_ENUMERROR_InvalidConfigDescriptor;

	while (!(DataINEndpoint) || !(DataOUTEndpoint) )
	{
		if (!(FTDIControlInterface) ||
				USB_GetNextDescriptorComp(&ConfigDescriptorSize, &ConfigDescriptorData,
						DCOMP_FTDI_Host_NextCDCInterfaceEndpoint) != DESCRIPTOR_SEARCH_COMP_Found)
		{

			if (USB_GetNextDescriptorComp(&ConfigDescriptorSize, &ConfigDescriptorData,
					DCOMP_FTDI_Host_NextCDCControlInterface) != DESCRIPTOR_SEARCH_COMP_Found)
			{
				return FTDI_ENUMERROR_NoCompatibleInterfaceFound;
			}

			FTDIControlInterface = DESCRIPTOR_PCAST(ConfigDescriptorData, USB_Descriptor_Interface_t);

			DataINEndpoint  = NULL;
			DataOUTEndpoint = NULL;

			continue;
		}

		USB_Descriptor_Endpoint_t* EndpointData = DESCRIPTOR_PCAST(ConfigDescriptorData, USB_Descriptor_Endpoint_t);

		if ((EndpointData->EndpointAddress & ENDPOINT_DIR_MASK) == ENDPOINT_DIR_IN)
		{
			DataINEndpoint = EndpointData;
		}
		else
		{
			DataOUTEndpoint = EndpointData;
			DEBUGOUT("Addr:%x \n",DataOUTEndpoint->EndpointAddress);
			DEBUGOUT("Size:%x \n",DataOUTEndpoint->EndpointSize);
		}
	}

	for (uint8_t PipeNum = 1; PipeNum < PIPE_TOTAL_PIPES; PipeNum++)
	{
		uint16_t Size;
		uint8_t  Type;
		uint8_t  Token;
		uint8_t  EndpointAddress;
		uint8_t  InterruptPeriod;
		bool     DoubleBanked;

		if (PipeNum == CDCInterfaceInfo->Config.DataINPipeNumber)
		{
			Size            = le16_to_cpu(DataINEndpoint->EndpointSize);
			EndpointAddress = DataINEndpoint->EndpointAddress;
			Token           = PIPE_TOKEN_IN;
			Type            = EP_TYPE_BULK;
			DoubleBanked    = CDCInterfaceInfo->Config.DataINPipeDoubleBank;
			InterruptPeriod = 0;

			CDCInterfaceInfo->State.DataINPipeSize = DataINEndpoint->EndpointSize;
		}
		else if (PipeNum == CDCInterfaceInfo->Config.DataOUTPipeNumber)
		{
			Size            = le16_to_cpu(DataOUTEndpoint->EndpointSize);
			EndpointAddress = DataOUTEndpoint->EndpointAddress;
			Token           = PIPE_TOKEN_OUT;
			Type            = EP_TYPE_BULK;
			DoubleBanked    = CDCInterfaceInfo->Config.DataOUTPipeDoubleBank;
			InterruptPeriod = 0;

			CDCInterfaceInfo->State.DataOUTPipeSize = DataOUTEndpoint->EndpointSize;
		}
		else
		{
			continue;
		}

		if (!(Pipe_ConfigurePipe(portnum,PipeNum, Type, Token, EndpointAddress, Size,
				DoubleBanked ? PIPE_BANK_DOUBLE : PIPE_BANK_SINGLE)))
		{
			return FTDI_ENUMERROR_PipeConfigurationFailed;
		}
	}

	CDCInterfaceInfo->State.ControlInterfaceNumber = FTDIControlInterface->InterfaceNumber;
	CDCInterfaceInfo->State.ControlLineStates.HostToDevice = (FTDI_CONTROL_LINE_OUT_RTS | FTDI_CONTROL_LINE_OUT_DTR);
	CDCInterfaceInfo->State.ControlLineStates.DeviceToHost = (FTDI_CONTROL_LINE_IN_DCD  | FTDI_CONTROL_LINE_IN_DSR);
	CDCInterfaceInfo->State.IsActive = true;

	return FTDI_ENUMERROR_NoError;
}

static uint8_t DCOMP_FTDI_Host_NextCDCControlInterface(void* const CurrentDescriptor)
{
	USB_Descriptor_Header_t* Header = DESCRIPTOR_PCAST(CurrentDescriptor, USB_Descriptor_Header_t);

	if (Header->Type == DTYPE_Interface)
	{
		USB_Descriptor_Interface_t* Interface = DESCRIPTOR_PCAST(CurrentDescriptor, USB_Descriptor_Interface_t);

		if ((Interface->Class    == FTDI_CSCP_VendorSpecificProtocol)    &&
				(Interface->SubClass == FTDI_CSCP_VendorSpecificProtocol) &&
				(Interface->Protocol == FTDI_CSCP_VendorSpecificProtocol))
		{
			return DESCRIPTOR_SEARCH_Found;
		}
	}

	return DESCRIPTOR_SEARCH_NotFound;
}

//static uint8_t DCOMP_FTDI_Host_NextCDCDataInterface(void* const CurrentDescriptor)
//{
//	USB_Descriptor_Header_t* Header = DESCRIPTOR_PCAST(CurrentDescriptor, USB_Descriptor_Header_t);
//
//	if (Header->Type == DTYPE_Interface)
//	{
//		USB_Descriptor_Interface_t* Interface = DESCRIPTOR_PCAST(CurrentDescriptor, USB_Descriptor_Interface_t);
//
//		if ((Interface->Class    == FTDI_CSCP_CDCDataClass)   &&
//				(Interface->SubClass == FTDI_CSCP_NoDataSubclass) &&
//				(Interface->Protocol == FTDI_CSCP_NoDataProtocol))
//		{
//			return DESCRIPTOR_SEARCH_Found;
//		}
//	}
//
//	return DESCRIPTOR_SEARCH_NotFound;
//}

static uint8_t DCOMP_FTDI_Host_NextCDCInterfaceEndpoint(void* const CurrentDescriptor)
{
	USB_Descriptor_Header_t* Header = DESCRIPTOR_PCAST(CurrentDescriptor, USB_Descriptor_Header_t);

	if (Header->Type == DTYPE_Endpoint)
	{
		USB_Descriptor_Endpoint_t* Endpoint = DESCRIPTOR_PCAST(CurrentDescriptor, USB_Descriptor_Endpoint_t);

		uint8_t EndpointType = (Endpoint->Attributes & EP_TYPE_MASK);

		if (((EndpointType == EP_TYPE_BULK) || (EndpointType == EP_TYPE_INTERRUPT)) &&
				!(Pipe_IsEndpointBound(Endpoint->EndpointAddress)))
		{
			return DESCRIPTOR_SEARCH_Found;
		}
	}
	else if (Header->Type == DTYPE_Interface)
	{
		return DESCRIPTOR_SEARCH_Fail;
	}

	return DESCRIPTOR_SEARCH_NotFound;
}

void FTDI_Host_USBTask(USB_ClassInfo_FTDI_Host_t* const CDCInterfaceInfo)
{
	uint8_t portnum = CDCInterfaceInfo->Config.PortNumber;

	if ((USB_HostState[portnum] != HOST_STATE_Configured) || !(CDCInterfaceInfo->State.IsActive))
		return;

	Pipe_Unfreeze();

	if (Pipe_IsINReceived(portnum))
	{
		USB_Request_Header_t Notification;
		Pipe_Read_Stream_LE(portnum,&Notification, sizeof(USB_Request_Header_t), NULL);

		if ((Notification.bRequest      == FTDI_NOTIF_SerialState) &&
				(Notification.bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE)))
		{
			Pipe_Read_Stream_LE(portnum,
					&CDCInterfaceInfo->State.ControlLineStates.DeviceToHost,
					sizeof(CDCInterfaceInfo->State.ControlLineStates.DeviceToHost),
					NULL);

			Pipe_ClearIN(portnum);

			EVENT_FTDI_Host_ControLineStateChanged(CDCInterfaceInfo);
		}
		else
		{
			Pipe_ClearIN(portnum);
		}
	}

	Pipe_Freeze();

#if !defined(NO_CLASS_DRIVER_AUTOFLUSH)
	FTDI_Host_Flush(CDCInterfaceInfo);
#endif
}

uint8_t FTDI_Host_Reset(USB_ClassInfo_FTDI_Host_t* const CDCInterfaceInfo)
{
	USB_ControlRequest = (USB_Request_Header_t)
					{
		.bmRequestType = (bmREQ_FTDI_OUT),
				.bRequest      = FTDI_REQ_Reset,
				.wValue        = 0x0000,
				.wIndex        = 0x0000,
				.wLength       = 0x0100,
					};
	uint8_t portnum = CDCInterfaceInfo->Config.PortNumber;

	Pipe_SelectPipe(portnum,PIPE_CONTROLPIPE);

	return USB_Host_SendControlRequest(portnum,NULL);
}

uint8_t FTDI_Host_SetBaudrate(USB_ClassInfo_FTDI_Host_t* const CDCInterfaceInfo)
{
	    unsigned long baud_value;
	    unsigned long baud_index = 0;
	    unsigned long divisor3;

	    //
	    // divisor shifted 3 bits to the left
	    //
	    divisor3 = 48000000 / 2 / 38400;

	    static const unsigned char divfrac [8] = { 0, 3, 2, 0, 1, 1, 2, 3 };
	    static const unsigned char divindex[8] = { 0, 0, 0, 1, 0, 1, 1, 1 };

	    baud_value	= divisor3 >> 3;
	    baud_value |= divfrac [divisor3 & 0x7] << 14;
	    baud_index	= divindex[divisor3 & 0x7];

	    //
	    // Deal with special cases for highest baud rates.
	    //
	    if (baud_value == 1)
	        baud_value = 0; else	// 1.0

	    if (baud_value == 0x4001)
	        baud_value = 1;		// 1.5

	USB_ControlRequest = (USB_Request_Header_t)
					{
		.bmRequestType = (bmREQ_FTDI_OUT),
				.bRequest      = FTDI_REQ_SetBaudRate,
				.wValue        = baud_value,
				.wIndex        = baud_index,
				.wLength       = 0x0000,
					};
	uint8_t portnum = CDCInterfaceInfo->Config.PortNumber;

	Pipe_SelectPipe(portnum,PIPE_CONTROLPIPE);

	return USB_Host_SendControlRequest(portnum,NULL);
}

uint8_t FTDI_Host_SetData(USB_ClassInfo_FTDI_Host_t* const CDCInterfaceInfo)
{
	USB_ControlRequest = (USB_Request_Header_t)
					{
		.bmRequestType = (bmREQ_FTDI_OUT),
				.bRequest      = FTDI_REQ_SetData,
				.wValue        = 0x0800, //8 bit data, parity none, stop bit 1
				.wIndex        = 0x0000,
				.wLength       = 0x0000,
					};
	uint8_t portnum = CDCInterfaceInfo->Config.PortNumber;

	Pipe_SelectPipe(portnum,PIPE_CONTROLPIPE);

	return USB_Host_SendControlRequest(portnum,NULL);
}

uint8_t FTDI_Host_DisableModemControl(USB_ClassInfo_FTDI_Host_t* const CDCInterfaceInfo)
{
	USB_ControlRequest = (USB_Request_Header_t)
					{
		.bmRequestType = (bmREQ_FTDI_OUT),
				.bRequest      = FTDI_REQ_SetModemControl,
				.wValue        = 0x0000,
				.wIndex        = 0x0000,
				.wLength       = 0x0000,
					};
	uint8_t portnum = CDCInterfaceInfo->Config.PortNumber;

	Pipe_SelectPipe(portnum,PIPE_CONTROLPIPE);

	return USB_Host_SendControlRequest(portnum,NULL);
}


uint8_t FTDI_Host_DisableFlowControl(USB_ClassInfo_FTDI_Host_t* const CDCInterfaceInfo)
{
	USB_ControlRequest = (USB_Request_Header_t)
					{
		.bmRequestType = (bmREQ_FTDI_OUT),
				.bRequest      = FTDI_REQ_SetFlowControl,
				.wValue        = 0x0000,
				.wIndex        = 0x0000,
				.wLength       = 0x0000,
					};
	uint8_t portnum = CDCInterfaceInfo->Config.PortNumber;

	Pipe_SelectPipe(portnum,PIPE_CONTROLPIPE);

	return USB_Host_SendControlRequest(portnum,NULL);
}





//uint8_t FTDI_Host_SetLineEncoding(USB_ClassInfo_FTDI_Host_t* const CDCInterfaceInfo)
//{
//	USB_ControlRequest = (USB_Request_Header_t)
//					{
//		.bmRequestType = (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE),
//				.bRequest      = FTDI_REQ_SetLineEncoding,
//				.wValue        = 0,
//				.wIndex        = CDCInterfaceInfo->State.ControlInterfaceNumber,
//				.wLength       = sizeof(CDCInterfaceInfo->State.LineEncoding),
//					};
//	uint8_t portnum = CDCInterfaceInfo->Config.PortNumber;
//
//	Pipe_SelectPipe(portnum,PIPE_CONTROLPIPE);
//
//	return USB_Host_SendControlRequest(portnum,&CDCInterfaceInfo->State.LineEncoding);
//}
//
//uint8_t FTDI_Host_SendControlLineStateChange(USB_ClassInfo_FTDI_Host_t* const CDCInterfaceInfo)
//{
//	USB_ControlRequest = (USB_Request_Header_t)
//					{
//		.bmRequestType = (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE),
//				.bRequest      = FTDI_REQ_SetControlLineState,
//				.wValue        = CDCInterfaceInfo->State.ControlLineStates.HostToDevice,
//				.wIndex        = CDCInterfaceInfo->State.ControlInterfaceNumber,
//				.wLength       = 0,
//					};
//	uint8_t portnum = CDCInterfaceInfo->Config.PortNumber;
//
//	Pipe_SelectPipe(portnum,PIPE_CONTROLPIPE);
//
//	return USB_Host_SendControlRequest(portnum,NULL);
//}
//
//uint8_t FTDI_Host_SendBreak(USB_ClassInfo_FTDI_Host_t* const CDCInterfaceInfo,
//		const uint8_t Duration)
//{
//	USB_ControlRequest = (USB_Request_Header_t)
//					{
//		.bmRequestType = (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE),
//				.bRequest      = FTDI_REQ_SendBreak,
//				.wValue        = Duration,
//				.wIndex        = CDCInterfaceInfo->State.ControlInterfaceNumber,
//				.wLength       = 0,
//					};
//	uint8_t portnum = CDCInterfaceInfo->Config.PortNumber;
//
//	Pipe_SelectPipe(portnum,PIPE_CONTROLPIPE);
//
//	return USB_Host_SendControlRequest(portnum,NULL);
//}

uint8_t FTDI_Host_SendData(USB_ClassInfo_FTDI_Host_t* const CDCInterfaceInfo,
		const uint8_t* const Buffer,
		const uint16_t Length)
{
	uint8_t portnum = CDCInterfaceInfo->Config.PortNumber;
	uint8_t ErrorCode;

	if ((USB_HostState[portnum] != HOST_STATE_Configured) || !(CDCInterfaceInfo->State.IsActive))
		return PIPE_READYWAIT_DeviceDisconnected;

	Pipe_SelectPipe(portnum,CDCInterfaceInfo->Config.DataOUTPipeNumber);

	Pipe_Unfreeze();
	ErrorCode = Pipe_Write_Stream_LE(portnum,Buffer, Length, NULL);
	Pipe_Freeze();

	return ErrorCode;
}

uint8_t FTDI_Host_SendString(USB_ClassInfo_FTDI_Host_t* const CDCInterfaceInfo,
		const char* const String)
{
	uint8_t portnum = CDCInterfaceInfo->Config.PortNumber;
	uint8_t ErrorCode;

	if ((USB_HostState[portnum] != HOST_STATE_Configured) || !(CDCInterfaceInfo->State.IsActive))
		return PIPE_READYWAIT_DeviceDisconnected;

	Pipe_SelectPipe(portnum,CDCInterfaceInfo->Config.DataOUTPipeNumber);

	Pipe_Unfreeze();
	ErrorCode = Pipe_Write_Stream_LE(portnum,String, strlen(String), NULL);
	Pipe_Freeze();

	return ErrorCode;
}

uint8_t FTDI_Host_SendByte(USB_ClassInfo_FTDI_Host_t* const CDCInterfaceInfo,
		const uint8_t Data)
{
	uint8_t portnum = CDCInterfaceInfo->Config.PortNumber;
	uint8_t ErrorCode;

	if ((USB_HostState[portnum] != HOST_STATE_Configured) || !(CDCInterfaceInfo->State.IsActive))
		return PIPE_READYWAIT_DeviceDisconnected;

	Pipe_SelectPipe(portnum,CDCInterfaceInfo->Config.DataOUTPipeNumber);
	Pipe_Unfreeze();

	if (!(Pipe_IsReadWriteAllowed(portnum)))
	{
		Pipe_ClearOUT(portnum);

		if ((ErrorCode = Pipe_WaitUntilReady(portnum)) != PIPE_READYWAIT_NoError)
			return ErrorCode;
	}

	Pipe_Write_8(portnum,Data);
	Pipe_Freeze();

	return PIPE_READYWAIT_NoError;
}

uint16_t FTDI_Host_BytesReceived(USB_ClassInfo_FTDI_Host_t* const CDCInterfaceInfo)
{
	uint8_t portnum = CDCInterfaceInfo->Config.PortNumber;

	if ((USB_HostState[portnum] != HOST_STATE_Configured) || !(CDCInterfaceInfo->State.IsActive))
		return 0;

	Pipe_SelectPipe(portnum,CDCInterfaceInfo->Config.DataINPipeNumber);
	Pipe_Unfreeze();

	if (Pipe_IsINReceived(portnum))
	{
		if (!(Pipe_BytesInPipe(portnum)))
		{
			Pipe_ClearIN(portnum);
			Pipe_Freeze();
			return 0;
		}
		else
		{
			Pipe_Freeze();
			return Pipe_BytesInPipe(portnum);
		}
	}
	else
	{
		Pipe_Freeze();

		return 0;
	}
}

int16_t FTDI_Host_ReceiveByte(USB_ClassInfo_FTDI_Host_t* const CDCInterfaceInfo)
{
	uint8_t portnum = CDCInterfaceInfo->Config.PortNumber;
	int16_t ReceivedByte = -1;

	if ((USB_HostState[portnum] != HOST_STATE_Configured) || !(CDCInterfaceInfo->State.IsActive))
		return -1;

	Pipe_SelectPipe(portnum,CDCInterfaceInfo->Config.DataINPipeNumber);
	Pipe_Unfreeze();

	if (Pipe_IsINReceived(portnum))
	{
		if (Pipe_BytesInPipe(portnum))
			ReceivedByte = Pipe_Read_8(portnum);

		if (!(Pipe_BytesInPipe(portnum)))
			Pipe_ClearIN(portnum);
	}

	Pipe_Freeze();

	return ReceivedByte;
}

uint8_t FTDI_Host_Flush(USB_ClassInfo_FTDI_Host_t* const CDCInterfaceInfo)
{
	uint8_t portnum = CDCInterfaceInfo->Config.PortNumber;
	uint8_t ErrorCode;

	if ((USB_HostState[portnum] != HOST_STATE_Configured) || !(CDCInterfaceInfo->State.IsActive))
		return PIPE_READYWAIT_DeviceDisconnected;

	Pipe_SelectPipe(portnum,CDCInterfaceInfo->Config.DataOUTPipeNumber);
	Pipe_Unfreeze();

	if (!(Pipe_BytesInPipe(portnum)))
		return PIPE_READYWAIT_NoError;

	bool BankFull = !(Pipe_IsReadWriteAllowed(portnum));

	Pipe_ClearOUT(portnum);

	if (BankFull)
	{
		if ((ErrorCode = Pipe_WaitUntilReady(portnum)) != PIPE_READYWAIT_NoError)
			return ErrorCode;

		Pipe_ClearOUT(portnum);
	}

	Pipe_Freeze();

	return PIPE_READYWAIT_NoError;
}

#if defined(FDEV_SETUP_STREAM)
void FTDI_Host_CreateStream(USB_ClassInfo_FTDI_Host_t* const CDCInterfaceInfo,
		FILE* const Stream)
{
	*Stream = (FILE)FDEV_SETUP_STREAM(FTDI_Host_putchar, FTDI_Host_getchar, _FDEV_SETUP_RW);
	fdev_set_udata(Stream, CDCInterfaceInfo);
}

void FTDI_Host_CreateBlockingStream(USB_ClassInfo_FTDI_Host_t* const CDCInterfaceInfo,
		FILE* const Stream)
{
	*Stream = (FILE)FDEV_SETUP_STREAM(FTDI_Host_putchar, FTDI_Host_getchar_Blocking, _FDEV_SETUP_RW);
	fdev_set_udata(Stream, CDCInterfaceInfo);
}

static int FTDI_Host_putchar(char c,
		FILE* Stream)
{
	return FTDI_Host_SendByte((USB_ClassInfo_FTDI_Host_t*)fdev_get_udata(Stream), c) ? _FDEV_ERR : 0;
}

static int FTDI_Host_getchar(FILE* Stream)
{
	int16_t ReceivedByte = FTDI_Host_ReceiveByte((USB_ClassInfo_FTDI_Host_t*)fdev_get_udata(Stream));

	if (ReceivedByte < 0)
		return _FDEV_EOF;

	return ReceivedByte;
}

static int FTDI_Host_getchar_Blocking(FILE* Stream)
{
	int16_t ReceivedByte;

	while ((ReceivedByte = FTDI_Host_ReceiveByte((USB_ClassInfo_FTDI_Host_t*)fdev_get_udata(Stream))) < 0)
	{
		if (USB_HostState == HOST_STATE_Unattached)
			return _FDEV_EOF;

		FTDI_Host_USBTask((USB_ClassInfo_FTDI_Host_t*)fdev_get_udata(Stream));
		USB_USBTask();
	}

	return ReceivedByte;
}
#endif

void FTDI_Host_Event_Stub(void)
{

}

#endif
