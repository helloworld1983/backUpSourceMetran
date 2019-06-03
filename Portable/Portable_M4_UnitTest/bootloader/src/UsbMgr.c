/********************************************************************/
/*                                                                  */
/* File Name    : usb_controller.c                            		*/
/*                                                                  */
/* General       : Init and handle USB control                      */
/*                 	 						                        */
/*                                                                  */
/* Product Name  : Portable                                         */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2016/08/22      Quyen Ngo		  	new file        */
/* #0001       														*/
/*                                                                  */
/********************************************************************/

#include <AudioMgr.h>
#include <UsbMgr.h>
#include "board.h"
#include "fsusb_cfg.h"
#include "USB.h"
#include "USBController.h"
#include "Led.h"

#include "Setting.h"
#define MS_ENUMERROR_NoError 0
#define MS_ERROR_LOGICAL_CMD_FAILED 0x80

typedef struct
{
        uint32_t Blocks; /**< Number of blocks in the addressed LUN of the device. */
        uint32_t BlockSize; /**< Number of bytes in each block in the addressed LUN. */
} SCSI_Capacity_t;

typedef struct
{
    uint8_t  ResponseCode;

    uint8_t  SegmentNumber;

    unsigned SenseKey            : 4;
    unsigned Reserved            : 1;
    unsigned ILI                 : 1;
    unsigned EOM                 : 1;
    unsigned FileMark            : 1;

    uint8_t  Information[4];
    uint8_t  AdditionalLength;
    uint8_t  CmdSpecificInformation[4];
    uint8_t  AdditionalSenseCode;
    uint8_t  AdditionalSenseQualifier;
    uint8_t  FieldReplaceableUnitCode;
    uint8_t  SenseKeySpecific[3];
} SCSI_Request_Sense_Response_t;

typedef ATTR_IAR_PACKED struct
{
    unsigned DeviceType          : 5;
    unsigned PeripheralQualifier : 3;

    unsigned Reserved            : 7;
    unsigned Removable           : 1;

    uint8_t  Version;

    unsigned ResponseDataFormat  : 4;
    unsigned Reserved2           : 1;
    unsigned NormACA             : 1;
    unsigned TrmTsk              : 1;
    unsigned AERC                : 1;

    uint8_t  AdditionalLength;
    uint8_t  Reserved3[2];

    unsigned SoftReset           : 1;
    unsigned CmdQue              : 1;
    unsigned Reserved4           : 1;
    unsigned Linked              : 1;
    unsigned Sync                : 1;
    unsigned WideBus16Bit        : 1;
    unsigned WideBus32Bit        : 1;
    unsigned RelAddr             : 1;

    uint8_t  VendorID[8];
    uint8_t  ProductID[16];
    uint8_t  RevisionID[4];
} ATTR_PACKED SCSI_Inquiry_Response_t;


static bool gs_isUSBattached =false;
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
                .PortNumber = 0,       	//0->1
        },
};

static SCSI_Capacity_t DiskCapacity;
//static uint8_t buffer[8 * 1024];

//STATIC FATFS fatFS;	/* File system object */
//STATIC FIL fileObj;	/* File object */

#define USB_DETECT_5V_PORT_NUM			2
#define USB_DETECT_5V_BIT_NUM			11
#define USB_PPWR_PORT_NUM			2
#define USB_PPWR_BIT_NUM			6

/****************************************************************************************/
/*                                                                                      */
/* General USB hardware setup			                                                */
/*                                                                                      */
/* Details  -Init USB1 CHIP                 											*/
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
    //	Chip_SCU_PinMuxSet(5, 2, SCU_MODE_FUNC0); //USB_DETECT_5V
    //	Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT, USB_DETECT_5V_PORT_NUM, USB_DETECT_5V_BIT_NUM);

    Chip_SCU_PinMuxSet(4, 6, (SCU_MODE_FUNC0)); //USB_PPWR
    Chip_GPIO_Init(LPC_GPIO_PORT);
    Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT,USB_PPWR_PORT_NUM, USB_PPWR_BIT_NUM);

    //	while(Chip_GPIO_ReadPortBit(LPC_GPIO_PORT,USB_DETECT_5V_PORT_NUM,USB_DETECT_5V_BIT_NUM)==false);
    Chip_GPIO_SetPinOutLow(LPC_GPIO_PORT,USB_PPWR_PORT_NUM, USB_PPWR_BIT_NUM);

    if (FlashDisk_MS_Interface.Config.PortNumber == 0) {
        Chip_USB0_Init();
    } else {
        Chip_USB1_Init();
    }
    USB_Init(FlashDisk_MS_Interface.Config.PortNumber, USB_MODE_Host);
}

/** Event handler for the USB_DeviceAttached event. This indicates that a device has been attached to the host, and
 *  starts the library USB task to begin the enumeration and USB management process.
 */
void EVENT_USB_Host_DeviceAttached(const uint8_t corenum)
{
    DEBUGOUT(("Device Attached on port %d\r\n"), corenum);
    gs_isUSBattached=true;
}

/** Event handler for the USB_DeviceUnattached event. This indicates that a device has been removed from the host, and
 *  stops the library USB task management process.
 */
void EVENT_USB_Host_DeviceUnattached(const uint8_t corenum)
{
    DEBUGOUT(("\r\nDevice Unattached on port %d\r\n"), corenum);
    gs_isUSBattached=false;
    Chip_GPIO_SetPinOutHigh(LPC_GPIO_PORT,USB_PPWR_PORT_NUM, USB_PPWR_BIT_NUM);
}

/** Event handler for the USB_DeviceEnumerationComplete event. This indicates that a device has been successfully
 *  enumerated by the host and is now ready to be used by the application.
 */
void EVENT_USB_Host_DeviceEnumerationComplete(const uint8_t corenum)
{
    uint16_t ConfigDescriptorSize;
    uint8_t  ConfigDescriptorData[512];

    if (USB_Host_GetDeviceConfigDescriptor(corenum, 1, &ConfigDescriptorSize, ConfigDescriptorData,
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

    if (USB_Host_SetDeviceConfiguration(FlashDisk_MS_Interface.Config.PortNumber, 1) != HOST_SENDCONTROL_Successful) {
        DEBUGOUT("Error Setting Device Configuration.\r\n");
        return;
    }

    uint8_t MaxLUNIndex;
    if (MS_Host_GetMaxLUN(&FlashDisk_MS_Interface, &MaxLUNIndex)) {
        DEBUGOUT("Error retrieving max LUN index.\r\n");
        USB_Host_SetDeviceConfiguration(FlashDisk_MS_Interface.Config.PortNumber, 0);
        return;
    }

    DEBUGOUT(("Total LUNs: %d - Using first LUN in device.\r\n"), (MaxLUNIndex + 1));

    if (MS_Host_ResetMSInterface(&FlashDisk_MS_Interface)) {
        DEBUGOUT("Error resetting Mass Storage interface.\r\n");
        USB_Host_SetDeviceConfiguration(FlashDisk_MS_Interface.Config.PortNumber, 0);
        return;
    }

    SCSI_Request_Sense_Response_t SenseData;
    if (MS_Host_RequestSense(&FlashDisk_MS_Interface, 0, &SenseData) != 0) {
        DEBUGOUT("Error retrieving device sense.\r\n");
        USB_Host_SetDeviceConfiguration(FlashDisk_MS_Interface.Config.PortNumber, 0);
        return;
    }

    // 	if (MS_Host_PreventAllowMediumRemoval(&FlashDisk_MS_Interface, 0, true)) {
    // 		DEBUGOUT("Error setting Prevent Device Removal bit.\r\n");
    // 		USB_Host_SetDeviceConfiguration(FlashDisk_MS_Interface.Config.PortNumber, 0);
    // 		return;
    // 	}

    SCSI_Inquiry_Response_t InquiryData;
    if (MS_Host_GetInquiryData(&FlashDisk_MS_Interface, 0, &InquiryData)) {
        DEBUGOUT("Error retrieving device Inquiry data.\r\n");
        USB_Host_SetDeviceConfiguration(FlashDisk_MS_Interface.Config.PortNumber, 0);
        return;
    }

    /* DEBUGOUT("Vendor \"%.8s\", Product \"%.16s\"\r\n", InquiryData.VendorID, InquiryData.ProductID); */

    DEBUGOUT("Mass Storage Device Enumerated.\r\n");
}

/** Event handler for the USB_HostError event. This indicates that a hardware error occurred while in host mode. */
void EVENT_USB_Host_HostError(const uint8_t corenum, const uint8_t ErrorCode)
{
    USB_Disable(corenum, USB_MODE_Host);

    DEBUGOUT(("Host Mode Error\r\n"
            " -- Error port %d\r\n"
            " -- Error Code %d\r\n" ), corenum, ErrorCode);

    while(1);
}

/** Event handler for the USB_DeviceEnumerationFailed event. This indicates that a problem occurred while
 *  enumerating an attached USB device.
 */
void EVENT_USB_Host_DeviceEnumerationFailed(const uint8_t corenum,
                                            const uint8_t ErrorCode,
                                            const uint8_t SubErrorCode)
{
    DEBUGOUT(("Dev Enum Error\r\n"
            " -- Error port %d\r\n"
            " -- Error Code %d\r\n"
            " -- Sub Error Code %d\r\n"
            " -- In State %d\r\n" ),
             corenum, ErrorCode, SubErrorCode, USB_HostState[corenum]);

}

/* Get the disk data structure */
DISK_HANDLE_T *FSUSB_DiskInit(void)
{
    return &FlashDisk_MS_Interface;
}

/* Wait for disk to be inserted */
int FSUSB_DiskInsertWait(DISK_HANDLE_T *hDisk)
{
    uint16_t s_counter = 0;
    Chip_GPIO_SetPinOutLow(LPC_GPIO_PORT,2, 6);
    Delay_MS(5000);//5000
    Chip_GPIO_SetPinOutHigh(LPC_GPIO_PORT,2, 6);
    while (USB_HostState[hDisk->Config.PortNumber] != HOST_STATE_Configured) {
//        MS_Host_USBTask(hDisk);
        USB_USBTask(hDisk->Config.PortNumber, USB_MODE_Host);
        if(s_counter == 30000)//30000
        {
            NVIC_EnableIRQ(TIMER3_IRQn);
            if(gs_isUSBattached == false)
            {
                led_DisplayWord(eUSB);
            }
        }
        s_counter++;
    }
    return 1;
}

/* Disk acquire function that waits for disk to be ready */
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
    return 1;
}

/* Get sector count */
uint32_t FSUSB_DiskGetSectorCnt(DISK_HANDLE_T *hDisk)
{
    return DiskCapacity.Blocks;
}

/* Get Block size */
uint32_t FSUSB_DiskGetSectorSz(DISK_HANDLE_T *hDisk)
{
    return DiskCapacity.BlockSize;
}

/* Read sectors */
int FSUSB_DiskReadSectors(DISK_HANDLE_T *hDisk, void *buff, uint32_t secStart, uint32_t numSec)
{
    if (MS_Host_ReadDeviceBlocks(hDisk, 0, secStart, numSec, DiskCapacity.BlockSize, buff)) {
        DEBUGOUT("Error reading device block.\r\n");
        USB_Host_SetDeviceConfiguration(FlashDisk_MS_Interface.Config.PortNumber, 0);
        return 0;
    }
    return 1;
}

/* Write Sectors */
int FSUSB_DiskWriteSectors(DISK_HANDLE_T *hDisk, void *buff, uint32_t secStart, uint32_t numSec)
{
    if (MS_Host_WriteDeviceBlocks(hDisk, 0, secStart, numSec, DiskCapacity.BlockSize, buff)) {
        DEBUGOUT("Error writing device block.\r\n");
        return 0;
    }
    return 1;
}

/* Disk ready function */
int FSUSB_DiskReadyWait(DISK_HANDLE_T *hDisk, int tout)
{
    volatile int i = tout * 100;
    while (i--) {	/* Just delay */
    }
    return 1;
}
