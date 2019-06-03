#ifndef INC_USBTASK_H_
	#define INC_USBTASK_H_
/********************************************************************/
/*                                                                  */
/* File Name    : UsbTask.h			               		            */
/*                                                                  */
/* General       : Handle comunication with USB devices on port1	*/
/*				   (include Masstorage and Barcode device)			*/
/*                 	 						                        */
/*                                                                  */
/* Product Name  : FH310                                            */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2016/10/06      Quyen Ngo		  	new file        */
/********************************************************************/


void usbTask_Create(void);

void usbTask_Init(void);

#endif /* INC_USBTASK_H_ */
/* end of file */
