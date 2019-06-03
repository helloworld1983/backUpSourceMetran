
#ifndef INC_USBMGR_H_
#define INC_USBMGR_H_
/********************************************************************/
/*                                                                  */
/* File Name    : m4_img_ldr.h                                		*/
/*                                                                  */
/* General      : Provide function to read and write to USB         */
/*                 	 						                        */
/*                                                                  */
/* Product Name  : Portable                                         */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2016/10/25       Thao Ha		  	    new file        */
/* #0001       														*/
/*                                                                  */
/********************************************************************/


#include <ctype.h>
#include <stdio.h>
#include "board.h"
#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup Mass_Storage_Host_18xx43xx Mass Storage Host
 * @ingroup EXAMPLES_USB_18XX43XX
 * <b>Example description</b><br>
 * This example implements a mass storage class host mode device that enumerates
 * a mass storage class device (USB flash drive). It reads the first sector
 * of the device and displays a hexdump style listing of that data on a terminal.
 *
 * When the example is first run the terminal window will display:
 * Mass Storage Host Demo running.
 *
 * When a flash drive is plugged in the terminal window will display
 * something similar to this:
 *
 * Device Attached.
 *
 * Total LUNs: 1 - Using first LUN in device.
 * Vendor "USB     ", Product "Flash Disk      "
 * Mass Storage Device Enumerated.
 *
 * Waiting until ready...
 *
 * Retrieving Capacity...
 *
 * 1957887 blocks of 512 bytes.
 *
 * Contents of first block:
 *
 * 33 C0 8E D0 BC 00 7C FB 50 07 50 1F FC BE 1B 7C     3.....|.P.P....|
 * BF 1B 06 50 57 B9 E5 01 F3 A4 CB BD BE 07 B1 04     ...PW...........
 * 38 6E 00 7C 09 75 13 83 C5 10 E2 F4 CD 18 8B F5     8n.|.u..........
 * 83 C6 10 49 74 19 38 2C 74 F6 A0 B5 07 B4 07 8B     ...It.8,t.......
 * F0 AC 3C 00 74 FC BB 07 00 B4 0E CD 10 EB F2 88     ..<.t...........
 * 4E 10 E8 46 00 73 2A FE 46 10 80 7E 04 0B 74 0B     N..F.s*.F..~..t.
 * 80 7E 04 0C 74 05 A0 B6 07 75 D2 80 46 02 06 83     .~..t....u..F...
 * 46 08 06 83 56 0A 00 E8 21 00 73 05 A0 B6 07 EB     F...V...!.s.....
 * BC 81 3E FE 7D 55 AA 74 0B 80 7E 10 00 74 C8 A0     ..>.}U.t..~..t..
 * B7 07 EB A9 8B FC 1E 57 8B F5 CB BF 05 00 8A 56     .......W.......V
 * 00 B4 08 CD 13 72 23 8A C1 24 3F 98 8A DE 8A FC     .....r#..$?.....
 * 43 F7 E3 8B D1 86 D6 B1 06 D2 EE 42 F7 E2 39 56     C..........B..9V
 * 0A 77 23 72 05 39 46 08 73 1C B8 01 02 BB 00 7C     .w#r.9F.s......|
 * 8B 4E 02 8B 56 00 CD 13 73 51 4F 74 4E 32 E4 8A     .N..V...sQOtN2..
 * 56 00 CD 13 EB E4 8A 56 00 60 BB AA 55 B4 41 CD     V......V.`..U.A.
 * 13 72 36 81 FB 55 AA 75 30 F6 C1 01 74 2B 61 60     .r6..U.u0...t+a`
 * 6A 00 6A 00 FF 76 0A FF 76 08 6A 00 68 00 7C 6A     j.j..v..v.j.h.|j
 * 01 6A 10 B4 42 8B F4 CD 13 61 61 73 0E 4F 74 0B     .j..B....aas.Ot.
 * 32 E4 8A 56 00 CD 13 EB D6 61 F9 C3 49 6E 76 61     2..V.....a..Inva
 * 6C 69 64 20 70 61 72 74 69 74 69 6F 6E 20 74 61     lid partition ta
 * 62 6C 65 00 45 72 72 6F 72 20 6C 6F 61 64 69 6E     ble.Error loadin
 * 67 20 6F 70 65 72 61 74 69 6E 67 20 73 79 73 74     g operating syst
 * 65 6D 00 4D 69 73 73 69 6E 67 20 6F 70 65 72 61     em.Missing opera
 * 74 69 6E 67 20 73 79 73 74 65 6D 00 00 00 00 00     ting system.....
 * 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00     ................
 * 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00     ................
 * 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00     ................
 * 00 00 00 00 00 2C 44 63 18 2E 07 C3 00 00 80 01     .....,Dc........
 * 01 00 06 20 00 77 00 02 00 00 00 DE 1D 00 00 00     ... .w..........
 * 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00     ................
 * 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00     ................
 * 00 00 00 00 00 00 00 00 00 00 00 00 00 00 55 AA     ..............U.
 *
 * When the flash drive is unplugged the ternimal window will display:
 * Device Unattached.<br>
 *
 * <b>Special connection requirements</b><br>
 * For Keil 1858 and 4357 boards, the host examples requires an external power supply
 * via the PWR power connector.<br>
 *
 * <b>Build procedures:</b><br>
 * @ref LPCOPEN_18XX43XX_BUILDPROCS_KEIL<br>
 * @ref LPCOPEN_18XX43XX_BUILDPROCS_IAR<br>
 * @ref LPCOPEN_18XX43XX_BUILDPROCS_LPCXPRESSO<br>
 *
 * <b>Supported boards and board setup:</b><br>
 * @ref LPCOPEN_18XX_BOARD_HITEX1850<br>
 * @ref LPCOPEN_43XX_BOARD_HITEX4350<br>
 * @ref LPCOPEN_18XX_BOARD_KEIL1857<br>
 * @ref LPCOPEN_43XX_BOARD_KEIL4357<br>
 * @ref LPCOPEN_18XX_BOARD_NGX1830<br>
 * @ref LPCOPEN_43XX_BOARD_NGX4330<br>
 *
 * <b>Submitting LPCOpen issues:</b><br>
 * @ref LPCOPEN_COMMUNITY
 */

/**
 * @}
 */

/** @defgroup Mass_Storage_Host_17xx40xx Mass Storage Host
 * @ingroup EXAMPLES_USB_17XX40XX
 * <b>Example description</b><br>
 * This example implements a mass storage class host mode device that enumerates
 * a mass storage class device (USB flash drive). It reads the first sector
 * of the device and displays a hexdump style listing of that data on a terminal.
 *
 * When the example is first run the terminal window will display:
 * Mass Storage Host Demo running.
 *
 * When a flash drive is plugged in the terminal window will display
 * something similar to this:
 *
 * Device Attached.
 *
 * Total LUNs: 1 - Using first LUN in device.
 * Vendor "USB     ", Product "Flash Disk      "
 * Mass Storage Device Enumerated.
 *
 * Waiting until ready...
 *
 * Retrieving Capacity...
 *
 * 1957887 blocks of 512 bytes.
 *
 * Contents of first block:
 *
 * 33 C0 8E D0 BC 00 7C FB 50 07 50 1F FC BE 1B 7C     3.....|.P.P....|
 * BF 1B 06 50 57 B9 E5 01 F3 A4 CB BD BE 07 B1 04     ...PW...........
 * 38 6E 00 7C 09 75 13 83 C5 10 E2 F4 CD 18 8B F5     8n.|.u..........
 * 83 C6 10 49 74 19 38 2C 74 F6 A0 B5 07 B4 07 8B     ...It.8,t.......
 * F0 AC 3C 00 74 FC BB 07 00 B4 0E CD 10 EB F2 88     ..<.t...........
 * 4E 10 E8 46 00 73 2A FE 46 10 80 7E 04 0B 74 0B     N..F.s*.F..~..t.
 * 80 7E 04 0C 74 05 A0 B6 07 75 D2 80 46 02 06 83     .~..t....u..F...
 * 46 08 06 83 56 0A 00 E8 21 00 73 05 A0 B6 07 EB     F...V...!.s.....
 * BC 81 3E FE 7D 55 AA 74 0B 80 7E 10 00 74 C8 A0     ..>.}U.t..~..t..
 * B7 07 EB A9 8B FC 1E 57 8B F5 CB BF 05 00 8A 56     .......W.......V
 * 00 B4 08 CD 13 72 23 8A C1 24 3F 98 8A DE 8A FC     .....r#..$?.....
 * 43 F7 E3 8B D1 86 D6 B1 06 D2 EE 42 F7 E2 39 56     C..........B..9V
 * 0A 77 23 72 05 39 46 08 73 1C B8 01 02 BB 00 7C     .w#r.9F.s......|
 * 8B 4E 02 8B 56 00 CD 13 73 51 4F 74 4E 32 E4 8A     .N..V...sQOtN2..
 * 56 00 CD 13 EB E4 8A 56 00 60 BB AA 55 B4 41 CD     V......V.`..U.A.
 * 13 72 36 81 FB 55 AA 75 30 F6 C1 01 74 2B 61 60     .r6..U.u0...t+a`
 * 6A 00 6A 00 FF 76 0A FF 76 08 6A 00 68 00 7C 6A     j.j..v..v.j.h.|j
 * 01 6A 10 B4 42 8B F4 CD 13 61 61 73 0E 4F 74 0B     .j..B....aas.Ot.
 * 32 E4 8A 56 00 CD 13 EB D6 61 F9 C3 49 6E 76 61     2..V.....a..Inva
 * 6C 69 64 20 70 61 72 74 69 74 69 6F 6E 20 74 61     lid partition ta
 * 62 6C 65 00 45 72 72 6F 72 20 6C 6F 61 64 69 6E     ble.Error loadin
 * 67 20 6F 70 65 72 61 74 69 6E 67 20 73 79 73 74     g operating syst
 * 65 6D 00 4D 69 73 73 69 6E 67 20 6F 70 65 72 61     em.Missing opera
 * 74 69 6E 67 20 73 79 73 74 65 6D 00 00 00 00 00     ting system.....
 * 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00     ................
 * 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00     ................
 * 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00     ................
 * 00 00 00 00 00 2C 44 63 18 2E 07 C3 00 00 80 01     .....,Dc........
 * 01 00 06 20 00 77 00 02 00 00 00 DE 1D 00 00 00     ... .w..........
 * 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00     ................
 * 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00     ................
 * 00 00 00 00 00 00 00 00 00 00 00 00 00 00 55 AA     ..............U.
 *
 * When the flash drive is unplugged the ternimal window will display:
 * Device Unattached.<br>
 *
 * <b>Special connection requirements</b><br>
 *  - EA1788 and EA4088 Developer's Kits<br>
 *      - Open jumper JP15 near 20 pin JTAG connector<br>
 *
 * <b>Build procedures:</b><br>
 * @ref LPCOPEN_17XX40XX_BUILDPROCS_KEIL<br>
 * @ref LPCOPEN_17XX40XX_BUILDPROCS_IAR<br>
 * @ref LPCOPEN_17XX40XX_BUILDPROCS_LPCXPRESSO<br>
 *
 * <b>Supported boards and board setup:</b><br>
 * @ref LPCOPEN_17XX40XX_BOARD_EA1788<br>
 * @ref LPCOPEN_17XX40XX_BOARD_EA4088<br>
 *
 * <b>Submitting LPCOpen issues:</b><br>
 * @ref LPCOPEN_COMMUNITY
 * @{
 */

/**
 * @}
 */

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

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

void usb_hw_setup(void);
bool usb_isAttached(void);
//void USB_ReadWriteFile(void);

#endif /* INC_USBMGR_H_ */