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
#include "UsbHostInterface.h"

static bool s_isBarcodeConnect = false;

/****************************************************************************************/
/*                                                                                      */
/* General 	-usbHostInterface_SetBarcodeConnectState   								    */
/*                                                                                      */
/* Details  -Set Barcode Connect state 													*/
/*					 																	*/
/* 																		      			*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) boot isConnect					 					  				*/
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_isBarcodeConnect                            			   	*/
/*【Note】                                                                              													*/
/*                                                                                      */
/****************************************************************************************/
void usbHostInterface_SetBarcodeConnectState(bool isConnect)
{
	s_isBarcodeConnect = isConnect;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-usbHostInterface_SetBarcodeConnectState   								    */
/*                                                                                      */
/* Details  -Set Barcode Connect state 													*/
/*					 																	*/
/* 																		      			*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) boot isConnect					 					  				*/
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : bool s_isBarcodeConnect		                                        */
/*				 range true/false: connect/disconnect									*/
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) s_isBarcodeConnect			                              	*/
/*                     (O)     					                        			   	*/
/*【Note】                                                                              													*/
/*                                                                                      */
/****************************************************************************************/
bool usbHostInterface_GetBarcodeConnectState(void)
{
	return s_isBarcodeConnect;
}
