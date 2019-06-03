/********************************************************************/
/*                                                                  */
/* File Name    : stringtools.c                          			*/
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
/* #0001       2017/09/12       Viet Le				modify			*/
/*       Remove "static" word (bug 1825)                            */
/* #0002       2017/09/28       Viet Le				modify			*/
/*	-Remove #include "m4_img_ldr.h", add #include "updateFlash.h"	*/
/*  -Edit getBootloaderInfo() -> updateFlash_GetBootloaderInfo()	*/
/********************************************************************/
#include <stdlib.h>
#include <math.h>
#include <RealTimeClock.h>
#include <StringTools.h>
#include "stdio.h"
#include "GuiTask.h"
#include "GuiInterface.h"
#include "updateFlash.h"

static int8_t s_bufferFRString[SIZE_BUFFER_FR_STR] = {'\0'};
static int8_t s_bufferDateString[SIZE_BUFFER_DATE_STR] = {'\0'};
static int8_t s_cumulativeStr[SIZE_BUFFER_CUMULATIVE_STR] = {'\0'};
const int8_t s_versionFH310CBX[] = "V01-010592018/03/15\0";
const int8_t s_versionFH310[] = "V01-01 059 2018/03/15";
static int8_t s_versionBootFH310[SIZE_BUFFER_BOOT_VER_STR] = {'\0'};
const int8_t s_versionFH310NotDate[] = "V01-01 059";
static int8_t s_versionBootFH310NotDate[SIZE_BUFFER_BOOT_VER_NODATE_STR] = {'\0'};

/****************************************************************************************/
/*                                                                                      */
/* General 	- stringtools_ConvertFCFlowToString											*/
/*                                                                                      */
/* Details  - Convert flow output to string												*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) uint8_t value - flow output value					 				*/
/*						- range : 5-60													*/
/*						- unit : LPM													*/
/*																						*/
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : int8_t - string after convert									        */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_bufferFRString : flow rate buffer              	   		*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
int8_t* stringtools_ConvertFCFlowToString(uint8_t value)
{
	memset(s_bufferFRString,0,SIZE_BUFFER_FR_STR);
	double floatValue = (double)value/FLOWRATE_SCALE;
	sprintf((char*)&s_bufferFRString[0],"%0.2f",floatValue);

	return s_bufferFRString;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	- stringtools_GetDateString													*/
/*                                                                                      */
/* Details  - Get current date to string												*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE 															    */
/*																						*/
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : int8_t - string after convert									        */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_bufferDateString : date buffer                             */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
int8_t* stringtools_GetDateString(void)
{
	memset(s_bufferDateString, 0, SIZE_BUFFER_DATE_STR);

	int8_t yearBuffer[SIZE_BUFFER_YEAR] = {'\0'};
	int8_t monthBuffer[SIZE_BUFFER_MONTH] = {'\0'};
	int8_t dayBuffer[SIZE_BUFFER_DAY] = {'\0'};

	int8_t hourBuffer[SIZE_BUFFER_HOURS] = {'\0'};
	int8_t minuteBuffer[SIZE_BUFFER_MINUTE] = {'\0'};
	int8_t secondBuffer[SIZE_BUFFER_SECOND] = {'\0'};

	Real_Time_Clock clockTime = realtimeclock_GetLastTime();

	sprintf((char*)yearBuffer,"%.4d",clockTime.year);
	sprintf((char*)monthBuffer,"%.2d",clockTime.month);
	sprintf((char*)dayBuffer,"%.2d",clockTime.mday);

	sprintf((char*)hourBuffer,"%.2d",clockTime.hour);
	sprintf((char*)minuteBuffer,"%.2d",clockTime.min);
	sprintf((char*)secondBuffer,"%.2d",clockTime.sec);

	strncat((char*)s_bufferDateString,(char*)yearBuffer,SIZE_BUFFER_YEAR);
	strcat((char*)s_bufferDateString,"/");
	strncat((char*)s_bufferDateString,(char*)monthBuffer,SIZE_BUFFER_MONTH);
	strcat((char*)s_bufferDateString,"/");
	strncat((char*)s_bufferDateString,(char*)dayBuffer,SIZE_BUFFER_DAY);

	strcat((char*)s_bufferDateString," ");
	strcat((char*)s_bufferDateString," ");

	strncat((char*)s_bufferDateString,(char*)hourBuffer,SIZE_BUFFER_HOURS);
	strcat((char*)s_bufferDateString,":");
	strncat((char*)s_bufferDateString,(char*)minuteBuffer,SIZE_BUFFER_MINUTE);
	strcat((char*)s_bufferDateString,":");
	strncat((char*)s_bufferDateString,(char*)secondBuffer,SIZE_BUFFER_SECOND);

	return s_bufferDateString;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	- stringtools_GetVersion													*/
/*                                                                                      */
/* Details  - Get full version string													*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE 															    */
/*																						*/
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : const int8_t*: control SW version								        */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) s_versionFH310 : control SW version							*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              													*/
/*                                                                                      */
/****************************************************************************************/
const int8_t* stringtools_GetVersion(void)
{
	return s_versionFH310;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	- stringtools_GetBootVersion												*/
/*                                                                                      */
/* Details  - Get full boot version string												*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE 															    */
/*																						*/
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : const int8_t*: bootloader version								        */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_versionBootFH310 :  bootloader version    		   			*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
const int8_t* stringtools_GetBootVersion(void)
{
	int8_t version[SIZE_BUFFER_FULL_VERSION] = {'\0'};
	int8_t buidDate[SIZE_BUFFER_FULL_VERSION] = {'\0'};

	int8_t month[SIZE_BUFFER_MONTH + 1] = {'\0'};
	int8_t day[SIZE_BUFFER_DAY] = {'\0'};
	int8_t year[SIZE_BUFFER_YEAR] = {'\0'};

	memset(s_versionBootFH310,0,22);
	int8_t firstVersion[SIZE_BUFFER_FIRST_VERSION] = {'\0'};
	int8_t lastVersion[SIZE_BUFFER_LAST_VERSION] = {'\0'};

	updateFlash_GetBootloaderInfo(IFLASH_BANKA_ADDR,version,buidDate);//getBootloaderInfo(IFLASH_BANKA_ADDR,version,buidDate);
	memcpy(month,buidDate,SIZE_BUFFER_MONTH);
	memcpy(firstVersion,version,SIZE_BUFFER_YEAR);

	lastVersion[0] = version[6];
	lastVersion[1] = version[7];
	lastVersion[2] = version[8];

	day[0] = buidDate[4];
	day[1] = buidDate[5];

	year[0] = buidDate[7];
	year[1] = buidDate[8];
	year[2] = buidDate[9];
	year[3] = buidDate[10];

	if(strcmp((char*)month,"Jan") == 0)
	{
		memset(month,0,SIZE_BUFFER_MONTH);
		strcat((char*)month,"01");
	}
	else if(strcmp((char*)month,"Feb") == 0)
	{
		memset(month,0,SIZE_BUFFER_MONTH);
		strcat((char*)month,"02");
	}
	else if(strcmp((char*)month,"Mar") == 0)
	{
		memset(month,0,SIZE_BUFFER_MONTH);
		strcat((char*)month,"03");
	}
	else if(strcmp((char*)month,"Apr") == 0)
	{
		memset(month,0,SIZE_BUFFER_MONTH);
		strcat((char*)month,"04");
	}
	else if(strcmp((char*)month,"May") == 0)
	{
		memset(month,0,SIZE_BUFFER_MONTH);
		strcat((char*)month,"05");
	}
	else if(strcmp((char*)month,"Jun") == 0)
	{
		memset(month,0,SIZE_BUFFER_MONTH);
		strcat((char*)month,"06");
	}
	else if(strcmp((char*)month,"Jul") == 0)
	{
		memset(month,0,SIZE_BUFFER_MONTH);
		strcat((char*)month,"07");
	}
	else if(strcmp((char*)month,"Aug") == 0)
	{
		memset(month,0,SIZE_BUFFER_MONTH);
		strcat((char*)month,"08");
	}
	else if(strcmp((char*)month,"Sep") == 0)
	{
		memset(month,0,SIZE_BUFFER_MONTH);
		strcat((char*)month,"09");
	}
	else if(strcmp((char*)month,"Oct") == 0)
	{
		memset(month,0,SIZE_BUFFER_MONTH);
		strcat((char*)month,"10");
	}
	else if(strcmp((char*)month,"Nov") == 0)
	{
		memset(month,0,SIZE_BUFFER_MONTH);
		strcat((char*)month,"11");
	}
	else if(strcmp((char*)month,"Dec") == 0)
	{
		memset(month,0,SIZE_BUFFER_MONTH);
		strcat((char*)month,"12");
	}

	strcat((char*)s_versionBootFH310,"V");
	strncat((char*)s_versionBootFH310,(char*)firstVersion,SIZE_BUFFER_FIRST_VERSION);
	strcat((char*)s_versionBootFH310," ");
	strncat((char*)s_versionBootFH310,(char*)lastVersion,SIZE_BUFFER_LAST_VERSION);
	strcat((char*)s_versionBootFH310," ");
	strncat((char*)s_versionBootFH310,(char*)year,SIZE_BUFFER_YEAR);
	strcat((char*)s_versionBootFH310,"/");
	strncat((char*)s_versionBootFH310,(char*)month,SIZE_BUFFER_MONTH + 1);
	strcat((char*)s_versionBootFH310,"/");
	strncat((char*)s_versionBootFH310,(char*)day,SIZE_BUFFER_DAY);

	return s_versionBootFH310;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	- stringtools_GetVersionNotDate												*/
/*                                                                                      */
/* Details  - Get full version not date string											*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE 															    */
/*																						*/
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : const int8_t* : SW version not date									*/
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) s_versionFH310NotDate : SW version not date		            */
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
const int8_t* stringtools_GetVersionNotDate(void)
{
	return s_versionFH310NotDate;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	- stringtools_GetBootVersionNotDate											*/
/*                                                                                      */
/* Details  - Get full boot version not date string										*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE 															    */
/*																						*/
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : const int8_t*	: bootloader version not date							*/
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_versionBootFH310NotDate : bootloader version not date      */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
const int8_t* stringtools_GetBootVersionNotDate(void)
{
	int8_t version[SIZE_BUFFER_FULL_VERSION] = {'\0'};
	int8_t buidDate[SIZE_BUFFER_FULL_VERSION] = {'\0'};

	memset(s_versionBootFH310NotDate,0,SIZE_BUFFER_BOOT_VER_NODATE_STR);
	int8_t firstVersion[SIZE_BUFFER_FIRST_VERSION] = {'\0'};
	int8_t lastVersion[SIZE_BUFFER_LAST_VERSION] = {'\0'};

	updateFlash_GetBootloaderInfo(IFLASH_BANKA_ADDR,version,buidDate);//getBootloaderInfo(IFLASH_BANKA_ADDR,version,buidDate);
	memcpy(firstVersion,version,SIZE_BUFFER_YEAR);

	lastVersion[0] = version[6];
	lastVersion[1] = version[7];
	lastVersion[2] = version[8];

	strcat((char*)s_versionBootFH310NotDate,"V");
	strncat((char*)s_versionBootFH310NotDate,(char*)firstVersion,SIZE_BUFFER_FIRST_VERSION);
	strcat((char*)s_versionBootFH310NotDate," ");
	strncat((char*)s_versionBootFH310NotDate,(char*)lastVersion,SIZE_BUFFER_LAST_VERSION);

	return s_versionBootFH310NotDate;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	- stringtools_GetVersionCBX													*/
/*                                                                                      */
/* Details  - Get full version CBX string												*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE 															    */
/*																						*/
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : const int8_t* : SW version (CBX use)									*/
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_versionFH310CBX : SW version (CBX use)            		   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
const int8_t* stringtools_GetVersionCBX(void)
{
	return s_versionFH310CBX;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	- stringtools_GetCumlativeString										    */
/*                                                                                      */
/* Details  - Get cumulative hour string												*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE 															    */
/*																						*/
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : int8_t* : cumulative hour string									    */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_cumulativeStr : cumulative hour string             	   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
int8_t* stringtools_GetCumlativeString(AllCumulativeHour id)
{
	memset(s_cumulativeStr, 0, SIZE_BUFFER_CUMULATIVE_STR);
	CumulativeHourTime time;
	time = guiInterface_GetCumulativeHour(id);

	sprintf((char*)s_cumulativeStr,"%.2d:%.2d:%.2d",time.hour,time.min,time.sec);

	return s_cumulativeStr;
}

/* end of files */
