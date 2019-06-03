#ifndef DRIVERS_USB_CLASS_FTDICLASS_H_
	#define DRIVERS_USB_CLASS_FTDICLASS_H_

/******************************************************************************/
//
//  File Name		: FTDIClass.h
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
	/* Macros: */
		#define __INCLUDE_FROM_USB_DRIVER
		#define __INCLUDE_FROM_FTDI_DRIVER

	/* Includes: */
		#include "../Core/USBMode.h"

		#if defined(USB_CAN_BE_DEVICE)
			#include "Device/FTDIClassDevice.h"
		#endif

		#if defined(USB_CAN_BE_HOST)
			#include "Host/FTDIClassHost.h"
		#endif

#endif /* end of file */
