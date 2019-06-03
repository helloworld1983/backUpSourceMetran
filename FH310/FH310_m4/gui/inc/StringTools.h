#ifndef STRINGTOOLS_H
#define STRINGTOOLS_H

/********************************************************************/
/*                                                                  */
/* File Name    : stringtools.h                          			*/
/*                                                                  */
/* General      : stringtools provides all string tool 				*/
/*                 	 						                        */
/*                                                                  */
/* Product Name  : FH310                                            */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2016/08/14       Viet Le		  	    new file        */
/* #0001       														*/
/*                                                                  */
/********************************************************************/

#include <string.h>
#include "stdint.h"
#include "setting.h"

#define SIZE_BUFFER_YEAR					5
#define SIZE_BUFFER_MONTH					3
#define SIZE_BUFFER_DAY						3
#define SIZE_BUFFER_HOURS					3
#define SIZE_BUFFER_MINUTE					3
#define SIZE_BUFFER_SECOND					3

#define SIZE_BUFFER_FR_STR 					10
#define SIZE_BUFFER_DATE_STR 				30
#define SIZE_BUFFER_CUMULATIVE_STR			12
#define SIZE_BUFFER_BOOT_VER_STR			22
#define SIZE_BUFFER_BOOT_VER_NODATE_STR		11
#define SIZE_BUFFER_CONVERT					10
#define DIV_10								10
#define SIZE_BUFFER_FULL_VERSION			32
#define SIZE_BUFFER_FIRST_VERSION			6
#define SIZE_BUFFER_LAST_VERSION			4

//Convert flow rate to string
int8_t* stringtools_ConvertFCFlowToString(uint8_t value);
//Convert date to string
int8_t* stringtools_GetDateString(void);
//Convert cumulative operation hour to string
int8_t* stringtools_GetCumlativeString(AllCumulativeHour id);
//Get version system for CBX
const int8_t* stringtools_GetVersionCBX(void);
//Get full version system
const int8_t* stringtools_GetVersion(void);
//Get full boot version
const int8_t* stringtools_GetBootVersion(void);
//Get version not date
const int8_t* stringtools_GetVersionNotDate(void);
//Get boot version not date
const int8_t* stringtools_GetBootVersionNotDate(void);

#endif /* STRINGTOOLS_H */
/* end of file */

