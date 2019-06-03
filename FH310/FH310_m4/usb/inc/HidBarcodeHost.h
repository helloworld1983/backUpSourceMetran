#ifndef INC_HIDBARCODEHOST_H_
	#define INC_HIDBARCODEHOST_H_

/********************************************************************/
/*                                                                  */
/* File Name    : HidBarcodeHost.h			                        */
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

#include <lpc_types.h>
#include "USB.h"

void hidBarcode_KeyboardHostTask(void);

void hidBarcode_Handle(void);

void hidBarcode_SetInterface(USB_ClassInfo_HID_Host_t hidBarcodeInterface);


#endif /* end of file */
