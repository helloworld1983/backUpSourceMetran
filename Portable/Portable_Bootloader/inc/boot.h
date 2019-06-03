/********************************************************************/
/*                                                                  */
/* File Name    : boot.h		                                 	*/
/*                                                                  */
/* General      : This module contains function to run bootloader  	*/
/*				and get current control software from Flash bank	*/
/*                 	 						                        */
/*                                                                  */
/* Product Name  : Portable                                         */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*            April 18,2017     Thao Ha(MV)		  	    new file    */
/*                                                                  */
/********************************************************************/

#ifndef INC_BOOT_H_
#define INC_BOOT_H_
#include "board.h"

struct image_sig {
	uint32_t signature;		/* Signature M0/M4 image */
	char build_date[32];	/* Build Date string */
	char build_time[32];	/* Build Time string */
	char version[32];
};
#define SIGNATURE_MAGIC     0xF00D4BAD
void boot_startBootloader();
int boot_getControlSWVersion(uint32_t image_addr,uint8_t* versionH,uint8_t* versionL);
bool boot_isACConnected();
bool boot_isBatteryConnected();
#endif /* INC_BOOT_H_ */
