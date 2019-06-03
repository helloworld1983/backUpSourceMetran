#ifndef INC_USBMGR_H_
#define INC_USBMGR_H_
/********************************************************************/
/*                                                                  */
/* File Name    : UsbMgr.h			                           		*/
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
/* #0001       2017/09/21      Viet Le		  		modify			*/
/*    Interface to get these data 									*/
/*			usbMgr_getFirmwareFilesBoot(void)						*/
/*			usbMgr_getFirmwareFilesControlSW(void) (bug 1847)		*/
/* #0002       2017/09/27      Viet Le		  		modify			*/
/*   - Remove usbMgr_getFirmwareFilesControlSW() function (bug 1847)*/
/*	 - Add two function : usbMgr_getFirmwareFilesM0(),				*/
/*			usbMgr_getFirmwareFilesM4()	(bug 1847)					*/
/********************************************************************/

#include "board.h"
#include "../../lib_lpcusblib/src/Drivers/USB/USB.h"
#include <ctype.h>
#include <stdio.h>
#include "UsbCommonDef.h"

#ifdef __cplusplus
extern "C" {
#endif
/** @defgroup Mass_Storage_Host_Definition Main definitions
 * @ingroup Mass_Storage_Host_18xx43xx Mass_Storage_Host_17xx40xx
 * @{
 */

/** LED mask for the library LED driver, to indicate that the USB interface is not ready. */
#define LEDMASK_USB_NOTREADY      LEDS_LED1

/** LED mask for the library LED driver, to indicate that the USB interface is enumerating. */
#define LEDMASK_USB_ENUMERATING  (LEDS_LED2 | LEDS_LED3)

/** LED mask for the library LED driver, to indicate that the USB interface is ready. */
#define LEDMASK_USB_READY        (LEDS_LED2 | LEDS_LED4)

/** LED mask for the library LED driver, to indicate that an error has occurred in the USB interface. */
#define LEDMASK_USB_ERROR        (LEDS_LED1 | LEDS_LED3)

/** LED mask for the library LED driver, to indicate that the USB interface is busy. */
#define LEDMASK_USB_BUSY          LEDS_LED2

#define RAMLOC32_ADDRESS 		0X10000000
#define RAMLOC32_SIZE 			0X8000

#define RAMLOC40_ADDRESS 		0X10080000
#define RAMLOC40_SIZE 			0Xa000

#define RAMAHB32_ADDRESS 		0X20000000
#define RAMAHB32_SIZE 			0X8000

#define RAMAHB16_ADDRESS 		0X20008000
#define RAMAHB16_SIZE 			0X4000

#define RAMAHB_ETB16_ADDRESS 	0X2000C000
#define RAMAHB_ETB16_SIZE 		0X4000

#define RAMAHB_EXT16_ADDRESS 	0X28000000
#define RAMAHB_EXT16_SIZE 		 0X1000000

#define RAMAHB_EXT16_2_ADDRESS 	0X28800000
#define RAMAHB_EXT16_2_SIZE		  0X800000

#define MFLASH_A_ADDRESS 		0X1A000000
#define MFLASH_A_SIZE 			0X80000

#define MFLASH_B_ADDRESS 		0X1B000000
#define MFLASH_B_SIZE 			0X80000

#define SPI_FLASH_ADDRESS 		0X14000000
#define SPI_FLASH_SIZE 			0X2000000

#define EEPROM_SIZE 		0X4000
#define EEPROM_LAST_UPLOAD_ADD 0x3f70

#define ONE_LINE_SIZE 		0X10

#define FLOW_RATE_SCALE 5

#define EXTENDED_LINEAR_SIZE 0x10000
#define ONE_LINE_SIZE 0X10

#define LAST_LINE 2

#define TIMER_START_STOP_WAIT 	10
#define QUEUE_WAIT 				500
#define YEAR_SCALE				100
#define PASSWORD_FILE_NAME "breath_technology.txt"
/**
 * @}
 */

#ifdef __cplusplus
}
#endif

typedef enum
{
	eErrorFWFile,
	eExistControlSW,
	eExistBootAndCTrlSW
}CheckFWFileResult;

typedef enum
{
	eSRAMId,
	eSDRAMId,
	eSPIFlashId,
	eFlashAId,
	eFlashBId
}MemoryTypeID;

typedef enum
{
	eEEPROMFileId,
	eCBXLogFileId,
	eAnypalLogFileId,
	eSpO2PulseLogId,
}FileTypeID;

void usb_hw_setup(void);

void usbMgr_Handle(void);

void usbMgr_CreateTimer(void);

void usbMgr_TurnOnVBUS(uint8_t portnum);

void usbMgr_TurnOffVBUS(uint8_t portnum);

void usbMgr_TurnOffVBUSAlarmOverCurrent(void);

bool usbMgr_GetOverCurrentStatus(uint8_t portnum);

bool usbMgr_isUSBAttachedGet(void);

int8_t* usbMgr_getFirmwareFilesBoot(void);
int8_t* usbMgr_getFirmwareFilesM0(void);
int8_t* usbMgr_getFirmwareFilesM4(void);

#endif /* INC_USBMGR_H_ */
