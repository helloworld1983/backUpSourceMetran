/*
 * UsbCommonDef.h
 *
 *  Created on: Jul 20, 2017
 *      Author: WINDOWS
 */

#ifndef USBCOMMONDEF_H_
#define USBCOMMONDEF_H_

#include "../../lib_lpcusblib/src/Drivers/USB/USB.h"
#define USB_PORT 1
#define ANYPAL_PORT 0


#define FTDI_VENDOR 0x0403
#define HDI_BARCODE_VENDOR1 0x24ea
#define HDI_BARCODE_VENDOR2 0x76d

#define BARCODE_MAXLENGTH 255

/**
 * @ingroup Mass_Storage_Host
 * @{
 */
typedef USB_ClassInfo_MS_Host_t DISK_HANDLE_T;

#endif /* USBCOMMONDEF_H_ */
