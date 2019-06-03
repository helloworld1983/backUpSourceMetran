#ifndef INC_SERIALNUMBERSETTINGSCREEN_H_
#define INC_SERIALNUMBERSETTINGSCREEN_H_

/********************************************************************/
/*                                                                  */
/* File Name    : SerialNumberSettingScreen.h                       */
/*                                                                  */
/* General      : The SerialNumberSettingScreen category provides   */
/* 				  the user an interface to display and              */
/*                change serial number setting in system            */
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

#include "WM.h"
#include "DualCoreCommon.h"

#define BORDER_SERIAL_COLOR 		0xECC9B1
#define MAX_LENGTH_SERIAL_NUMBER    8

extern WM_HWIN g_serialNumberSettingScreen;

//Init serial number Setting Screen
void serialNumberSettingScreen_Init(void);
//Show serial number Screen
void serialNumberSettingScreen_Show(void);
//Hide serial number Screen
void serialNumberSettingScreen_Hide(void);
//Display serial number
void serialNumberSettingScreen_DisplaySerialNumber(uint8_t length, const int8_t* serialStr);

#endif /* INC_SERIALNUMBERSETTINGSCREEN_H_ */
/* end of file */

