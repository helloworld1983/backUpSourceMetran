#ifndef INC_CALENDARSETTINGSCREEN_H_
#define INC_CALENDARSETTINGSCREEN_H_

/********************************************************************/
/*                                                                  */
/* File Name    : CalendarSettingScreen.h                           */
/*                                                                  */
/* General      :The CalendarSettingScreen allows user              */
/*                to change calendar								*/
/*                 	 						                        */
/*                                                                  */
/* Product Name  : FH310                                            */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2008/08/22      Viet Le  			new file        */
/* #0001       														*/
/*                                                                  */
/********************************************************************/

#include "WM.h"
#include "DualCoreCommon.h"

#define MIN_YEAR_CALENDAR 			2016
#define YEAR_DIV_4					4
#define MIN_YEAR_STR				((int8_t*)"2016")

extern WM_HWIN g_calendarSettingScreen;

//Init calendar Setting Screen
void calendarSettingScreen_Init(void);
//Show calendar Setting Screen
void calendarSettingScreen_Show(void);
//Hide calendar Setting Screen
void calendarSettingScreen_Hide(void);
//Check valid date
bool calendarSettingScreen_CheckValidDate(void);

#endif /* INC_CALENDARSETTINGSCREEN_H_ */
/* end of file */
