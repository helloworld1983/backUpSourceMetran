#ifndef INC_USBHOSTINTERFACE_H_
#define INC_USBHOSTINTERFACE_H_
/********************************************************************/
/*                                                                  */
/* File Name    : UsbHostInterface.c                       			*/
/*                                                                  */
/* General   : Contains functions for interface with USBhost module	*/
/*                 	 						                        */
/*                                                                  */
/* Product Name  : FH310                                            */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*        	   2017/08/15       Quyen Ngo(MV)  	    new file        */
/*  #0001	   2017/09/15      Linh Nguyen		  	modify		    */
/*  Remove un-use function											*/
/*  #0002	   2017/12/13	   Quyen Ngo(MV)		modify			*/
/*		Add void to non parameter function to follow coding rule	*/
/********************************************************************/
#include <lpc_types.h>

void usbHostInterface_SetBarcodeConnectState(bool isConnect);
bool usbHostInterface_GetBarcodeConnectState(void);
#endif /* INC_USBHOSTINTERFACE_H_ */
