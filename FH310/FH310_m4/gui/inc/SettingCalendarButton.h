#ifndef INC_SETTINGCALENDARBUTTON_H_
#define INC_SETTINGCALENDARBUTTON_H_

/********************************************************************/
/*                                                                  */
/* File Name    : SettingCalendarButton.h                           */
/*                                                                  */
/* General      : The SettingCalendarButton category provides       */
/* 				  the user an interface to display and              */
/*                change calendar setting in system                 */
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

#include "EDIT.h"
#include "stdint.h"

#define STEP_DATE				1
#define SIZE_BUFFER_DATE  		10
#define MIN_YEAR 				2016
#define MAX_YEAR 				2050
#define MIN_MONTH 				1
#define MAX_MONTH 				12
#define MIN_DAY 				1
#define MAX_DAY 				31

#define MIN_HOUR 				0
#define MAX_HOUR 				23
#define MIN_MINUTE 				0
#define MAX_MINUTE 				59
#define MIN_SECOND 				0
#define MAX_SECOND 				59

//Define id value for setting button
typedef enum
{
	eFirstValueCalendarSettingId = 0,
	eValueYearSettingId,
	eValueMonthSettingId,
	eValueDaySettingId,
	eValueHourSettingId,
	eValueMinuteSettingId,
	eValueSecondSettingId,
	eValueCalendarSettingMAXId
}E_ValueSettingCalendarBtnId;

//Define id for setting button
enum
{
	eCurrentClockTimeId,
	eBackBtnCalendarId,
	eHomeBtnCalendarId,
	eYearId,
	eYearFieldId,
	eMonthId,
	eMonthFieldId,
	eDayId,
	eDayFieldId,
	eHourId,
	eHourFieldId,
	eMinuteId,
	eMinuteFieldId,
	eSecondId,
	eSecondFieldId,
	eUpYearBtnId,
	eDownYearBtnId,
	eUpMonthBtnId,
	eDownMonthBtnId,
	eUpDayBtnId,
	eDownDayBtnId,
	eUpHourBtnId,
	eDownHourBtnId,
	eUpMinuteBtnId,
	eDownMinuteBtnId,
	eUpSecondBtnId,
	eDownSecondBtnId,
	eChangeClockTimeBtnId
};

//Init setting button not save to EPPROM
void settingCalendarButton_Init(EDIT_Handle obj, int upBtnId,int downBtnId, E_ValueSettingCalendarBtnId posArr,int valueId,GUI_RECT Rect);
//Custom setting button
void settingCalendarButton_Callback(WM_MESSAGE * pMsg);
//Set value for setting button
void settingCalendarButton_SetValue(E_ValueSettingCalendarBtnId valueEdit,const int8_t * value);
//Get value for setting button
int settingCalendarButton_GetValue(E_ValueSettingCalendarBtnId valueEdit);

#endif /* INC_SETTINGCALENDARBUTTON_H_ */
/* end of file */

