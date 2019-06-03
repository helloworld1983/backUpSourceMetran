/********************************************************************/
/*                                                                  */
/* File Name    : common.h                                    		*/
/*                                                                  */
/* General      : Initialization Hardware							*/
/* 								                                    */
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

#ifndef INC_COMMON_H_
#define INC_COMMON_H_
#define SPIF_BASE_ADDRESS 	0x14000000
#define USER_PRO_ADDR_M4	0x1A00E000

#define PORTABLE_BOOTLOADER_VERSION "00-01-004"

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	SetupHardware							              				*/
/*                                                                                      */
/* Details:    Init all devices which are used in bootloader							*/
/*                                                                                      */
/* Arguments:	None																	*/
/*                                                                                      */
/* ReturnValue : None			                                                        */
/*                                                                                      */
/****************************************************************************************/
void SetupHardware(void);

#endif /* INC_COMMON_H_ */
