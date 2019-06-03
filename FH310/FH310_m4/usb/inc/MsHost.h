#ifndef INC_MSHOST_H_
	#define INC_MSHOST_H_

/******************************************************************************/
//
//  File Name		: MsHost.h
//  
//	General       	: XXXX 
//
//  Product Name	: FH310_m4
//
//
//  Revision History:
//          Rev:      Date:       		Editor:         
//          01        Nov 25, 2016	        Quyen Ngo(MV)         
//          Description:  InitialRevision
//
/******************************************************************************/

#include <lpc_types.h>
#include "USB.h"

#define USB_PROCESS_SUCESSFULL					1
#define MAX_TIME_WAIT_FOR_INSERT_USB_CNT 		55000

void msHost_SetInterface(USB_ClassInfo_MS_Host_t msHostInterface);
#endif /* end of file */
