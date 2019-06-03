/********************************************************************/
/*                                                                  */
/* File Name    : MsHost.c			                		        */
/*                                                                  */
/* General      : Contain function for Interface with USB Masstorage*/
/*				  host 								     			*/
/*                 	 						                        */
/*                                                                  */
/* Product Name  : FH310                                            */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2016/11/25      Quyen Ngo		  	new file        */
/* #0001       2017/09/27      Quyen Ngo		  	modify			*/
/*		Feed WDT when waiting to plug USB	 						*/
/*																	*/
/********************************************************************/

/* include system files */

/* include user files */
#include "MsHost.h"
#include "fsusb_cfg.h"
#include "ff.h"
#include "GUI.h"
#include "TaskCommon.h"
#include "lpc_swim_font.h"

/* constants, macro definition */

/* definitions of structures */

/*global variable*/

/* external declaration */

extern SWIM_WINDOW_T upgradeScreen;

//Q_remove USB_ClassInfo_MS_Host_t and move Enum Function to USB Host Task File

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

static SCSI_Capacity_t DiskCapacity;

/******************************************************************************/
//COMMON.OPERATION
//    Function Name: FSUSB_DiskInsertWait
//
//
//    Detail:  Wait for disk to be inserted
//	  This function always return 1 because we process the MSHOst enumprocess by infinite
//					loop on USB host task
//
//    Arguments: (I) DISK_HANDLE_T *hDisk
//				 (O)
//
//    Return Values: int
//
//
//    Using Global Data : (I)
//						  (O)
//
//
//    Note: None
//
/******************************************************************************/
int FSUSB_DiskInsertWait(DISK_HANDLE_T *hDisk)
{
	return USB_PROCESS_SUCESSFULL;
}

//COMMON.OPERATION
//    Function Name: FSUSB_DiskInsertWait_nonRTOS
//					This function to process the USBHOst Masstorage enum process when RTOS does not run
//
//    Detail:  Wait for disk to be inserted
//
//    Arguments: (I) DISK_HANDLE_T *hDisk
//				 (O)
//
//    Return Values: int
//
//
//    Using Global Data : (I)
//						  (O)
//
//
//    Note: None
//
/******************************************************************************/
int FSUSB_DiskInsertWait_nonRTOS(DISK_HANDLE_T *hDisk)
{
	uint16_t s_Counter=0;
	usbMgr_TurnOffVBUS(USB_PORT);
	Delay_MS(500);
	usbMgr_TurnOnVBUS(USB_PORT);
	bool flag = false;
	while (USB_HostState[hDisk->Config.PortNumber] != HOST_STATE_Configured) {
		MS_Host_USBTask(hDisk);
		USB_USBTask(hDisk->Config.PortNumber, USB_MODE_Host);
		if(s_Counter == MAX_TIME_WAIT_FOR_INSERT_USB_CNT)
		{
			if(	USB_HostState[hDisk->Config.PortNumber] == HOST_STATE_Unattached && flag == false)
			{
				swim_put_text(&upgradeScreen,"Please insert the USB to start update Bootloader!");
				flag = true;
			}
		}
		s_Counter++;
		Chip_WWDT_Feed(LPC_WWDT);
	}
	s_Counter=0;
	return USB_PROCESS_SUCESSFULL;
}

//COMMON.OPERATION
//    Function Name: FSUSB_DiskAcquire
//
//    Detail:  Disk acquire function that waits for disk to be ready
//
//    Arguments: (I) DISK_HANDLE_T *hDisk
//				 (O)
//
//    Return Values: int
//
//
//    Using Global Data : (I)
//						  (O)
//
//
//    Note: None
//
/******************************************************************************/
int FSUSB_DiskAcquire(DISK_HANDLE_T *hDisk)
{
	DEBUGOUT("Waiting for ready...");
	for (;; ) {
		uint8_t ErrorCode = MS_Host_TestUnitReady(hDisk, 0);

		if (!(ErrorCode)) {
			break;
		}

		/* Check if an error other than a logical command error (device busy) received */
		if (ErrorCode != MS_ERROR_LOGICAL_CMD_FAILED) {
			DEBUGOUT("Failed\r\n");
			USB_Host_SetDeviceConfiguration(hDisk->Config.PortNumber, 0);
			return 0;
		}
	}
	DEBUGOUT("Done.\r\n");

	if (MS_Host_ReadDeviceCapacity(hDisk, 0, &DiskCapacity)) {
		DEBUGOUT("Error retrieving device capacity.\r\n");
		USB_Host_SetDeviceConfiguration(hDisk->Config.PortNumber, 0);
		return 0;
	}

	DEBUGOUT(("%lu blocks of %lu bytes.\r\n"), DiskCapacity.Blocks, DiskCapacity.BlockSize);
	return USB_PROCESS_SUCESSFULL;
}

//COMMON.OPERATION
//    Function Name: FSUSB_DiskGetSectorCnt
//
//    Detail:  Get sector count
//
//    Arguments: (I) DISK_HANDLE_T *hDisk
//				 (O)
//
//    Return Values: uint32_t
//
//
//    Using Global Data : (I)
//						  (O)
//
//
//    Note: None
//
/******************************************************************************/
uint32_t FSUSB_DiskGetSectorCnt(DISK_HANDLE_T *hDisk)
{
	return DiskCapacity.Blocks;
}

//COMMON.OPERATION
//    Function Name: FSUSB_DiskGetSectorSz
//
//    Detail:  Get Block size
//
//    Arguments: (I) DISK_HANDLE_T *hDisk
//				 (O)
//
//    Return Values: uint32_t
//
//
//    Using Global Data : (I)
//						  (O)
//
//
//    Note: None
//
/******************************************************************************/
uint32_t FSUSB_DiskGetSectorSz(DISK_HANDLE_T *hDisk)
{
	return DiskCapacity.BlockSize;
}

//COMMON.OPERATION
//    Function Name: FSUSB_DiskReadSectors
//
//    Detail:  Read sectors
//
//    Arguments: (I) DISK_HANDLE_T *hDisk
//				 (I)	void *buff
//				 (I)	uint32_t secStart
//				 (I)	uint32_t numSec
//				 (O)
//
//    Return Values: int
//
//
//    Using Global Data : (I)
//						  (O)
//
//
//    Note: None
//
/******************************************************************************/
int FSUSB_DiskReadSectors(DISK_HANDLE_T *hDisk, void *buff, uint32_t secStart, uint32_t numSec)
{
	if (MS_Host_ReadDeviceBlocks(hDisk, 0, secStart, numSec, DiskCapacity.BlockSize, buff)) {
		DEBUGOUT("Error reading device block.\r\n");
		USB_Host_SetDeviceConfiguration(hDisk->Config.PortNumber, 0);
		return 0;
	}
	return USB_PROCESS_SUCESSFULL;
}

//COMMON.OPERATION
//    Function Name: FSUSB_DiskWriteSectors
//
//    Detail:  Write Sectors
//
//    Arguments: (I) DISK_HANDLE_T *hDisk
//				 (I)	void *buff
//				 (I)	uint32_t secStart
//				 (I)	uint32_t numSec
//				 (O)
//
//    Return Values: int
//
//
//    Using Global Data : (I)
//						  (O)
//
//
//    Note: None
//
/******************************************************************************/
int FSUSB_DiskWriteSectors(DISK_HANDLE_T *hDisk, void *buff, uint32_t secStart, uint32_t numSec)
{
	if (MS_Host_WriteDeviceBlocks(hDisk, 0, secStart, numSec, DiskCapacity.BlockSize, buff)) {
		DEBUGOUT("Error writing device block.\r\n");
		return 0;
	}
	return USB_PROCESS_SUCESSFULL;
}

//COMMON.OPERATION
//    Function Name: FSUSB_DiskReadyWait
//
//    Detail:  Disk ready function
//
//    Arguments: (I) DISK_HANDLE_T *hDisk
//				 (I)	int tout
//				 (O)
//
//    Return Values: int
//
//
//    Using Global Data : (I)
//						  (O)
//
//
//    Note: None
//
/******************************************************************************/
int FSUSB_DiskReadyWait(DISK_HANDLE_T *hDisk, int tout)
{
	volatile int i = tout * 100;
	while (i--) {	/* Just delay */
	}
	return USB_PROCESS_SUCESSFULL;
}

/******************************************************************************/
//COMMON.OPERATION
//    Function Name: msHost_SetInterface
//
//    Detail:  Set Interface for Ms USB class
//
//    Arguments: (I) USB_ClassInfo_MS_Host_t msHostInterface
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
void msHost_SetInterface(USB_ClassInfo_MS_Host_t msHostInterface)
{
	FlashDisk_MS_Interface = msHostInterface;
	return;
}

/******************************************************************************/
//COMMON.OPERATION
//    Function Name: FSUSB_DiskInit
//
//    Detail:  Get the disk data structure
//
//    Arguments: (I)
//				 (O)
//
//    Return Values: DISK_HANDLE_T *
//
//
//    Using Global Data : (I)
//						  (O)
//
//
//    Note: None
//
/******************************************************************************/
DISK_HANDLE_T *FSUSB_DiskInit(void)
{
	return &FlashDisk_MS_Interface;
}

