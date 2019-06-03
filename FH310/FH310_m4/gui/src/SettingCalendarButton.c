/********************************************************************/
/*                                                                  */
/* File Name    : SettingCalendarButton.c                           */
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
/* #0001       2017/09/05       Viet Le				modify			*/
/*     remove #include "assert.h"                                   */
/* #0002       2017/10/27	   Viet Le				modify			*/
/*	-Edit function : EDIT_SetFont()->guiTaskEDIT_SetFont()(bug 1997)*/
/********************************************************************/

#include "string.h"
#include "stdlib.h"
#include "SettingCalendarButton.h"
#include "BUTTON.h"
#include "GuiTask.h"
#include "setting.h"
#include "Font.h"
#include "MainScreen.h"
#include "GuiInterface.h"

static BUTTON_Handle s_upBtn;
static BUTTON_Handle s_downBtn;
static EDIT_Handle s_valueSettingList[eValueCalendarSettingMAXId];
static int8_t s_bufferSettingValue[10] = {0};

/*******************************************************************
 * *
The points of the arrow
 */
static const GUI_POINT s_aPointUpArrowLarge[] = {
		{49/2,5},
		{5,44},
		{49-5,49-5}
};

static const GUI_POINT s_aPointDownArrowLarge[] = {
		{5,5},
		{44,5},
		{49/2,44}
};

/****************************************************************************************/
/*                                                                                      */
/* General 	-settingCalendarButton_CallbackUpBtn					 					*/
/*                                                                                      */
/* Details  -Call back function for up button, handle event from the window				*/
/* 																						*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) WM_MESSAGE * pMsg : message from window								*/
/* 				   							    										*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_valueSettingList : setting list                            */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void settingCalendarButton_CallbackUpBtn(WM_MESSAGE * pMsg)
{
	GUI_RECT Rect;
	int8_t intBuffer[SIZE_BUFFER_DATE] = {'\0'};

	switch(pMsg->MsgId)
	{
	case WM_PAINT:
		GUI_GetClientRect(&Rect);
		GUI_SetColor(GUI_WHITE);
		GUI_FillRoundedRect(Rect.x0,Rect.y0,Rect.x1,Rect.y1,5);
		GUI_SetBkColor(GUI_WHITE);
		GUI_SetColor(GUI_BLACK);
		GUI_DrawRoundedRect(Rect.x0,Rect.y0,Rect.x1,Rect.y1,5);
		GUI_SetColor(GUI_BLACK);
		GUI_FillPolygon(&s_aPointUpArrowLarge[0],3,Rect.x0,Rect.y0);
		break;
	case WM_PID_STATE_CHANGED:
		if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 0)
		{
			//Sub switch
			switch(WM_GetId(pMsg->hWin))
			{
			case eUpYearBtnId:
			{
				uint16_t yearValue = settingCalendarButton_GetValue(eValueYearSettingId);

				yearValue += STEP_DATE;

				if(yearValue > MAX_YEAR)
				{
					yearValue = MAX_YEAR;
				}

				sprintf((char*)intBuffer,"%.4d",yearValue);
				EDIT_SetText(s_valueSettingList[eValueYearSettingId],(char*)intBuffer);
				break;
			}
			case eUpMonthBtnId:
			{
				uint8_t monthValue = settingCalendarButton_GetValue(eValueMonthSettingId);

				monthValue += STEP_DATE;

				if(monthValue > MAX_MONTH)
				{
					monthValue = MAX_MONTH;
				}

				sprintf((char*)intBuffer,"%.2d",monthValue);
				EDIT_SetText(s_valueSettingList[eValueMonthSettingId],(char*)intBuffer);
				break;
			}
			case eUpDayBtnId:
			{
				uint8_t dayValue = settingCalendarButton_GetValue(eValueDaySettingId);

				dayValue += STEP_DATE;

				if(dayValue > MAX_DAY)
				{
					dayValue = MAX_DAY;
				}

				sprintf((char*)intBuffer,"%.2d",dayValue);
				EDIT_SetText(s_valueSettingList[eValueDaySettingId],(char*)intBuffer);
				break;
			}
			case eUpHourBtnId:
			{
				uint8_t hourValue = settingCalendarButton_GetValue(eValueHourSettingId);

				hourValue += STEP_DATE;

				if(hourValue > MAX_HOUR)
				{
					hourValue = MAX_HOUR;
				}

				sprintf((char*)intBuffer,"%.2d",hourValue);
				EDIT_SetText(s_valueSettingList[eValueHourSettingId],(char*)intBuffer);

				break;
			}
			case eUpMinuteBtnId:
			{
				uint8_t minuteValue = settingCalendarButton_GetValue(eValueMinuteSettingId);

				minuteValue += STEP_DATE;

				if(minuteValue > MAX_MINUTE)
				{
					minuteValue = MAX_MINUTE;
				}

				sprintf((char*)intBuffer,"%.2d",minuteValue);
				EDIT_SetText(s_valueSettingList[eValueMinuteSettingId],(char*)intBuffer);

				break;
			}
			case eUpSecondBtnId:
			{
				uint8_t secondValue = settingCalendarButton_GetValue(eValueSecondSettingId);

				secondValue += STEP_DATE;

				if(secondValue > MAX_SECOND)
				{
					secondValue = MAX_SECOND;
				}

				sprintf((char*)intBuffer,"%.2d",secondValue);
				EDIT_SetText(s_valueSettingList[eValueSecondSettingId],(char*)intBuffer);

				break;
			}
			default:
				break;
			}
			//End sub switch
		}
		break;
	default:
		BUTTON_Callback(pMsg);
		break;
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-settingCalendarButton_CallbackDownBtn					 					*/
/*                                                                                      */
/* Details  -Call back function for down button, handle event from the window			*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) WM_MESSAGE * pMsg : message from window								*/
/* 				   							    										*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_valueSettingList : setting list                            */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void settingCalendarButton_CallbackDownBtn(WM_MESSAGE * pMsg)
{
	GUI_RECT Rect;
	int8_t intBuffer[SIZE_BUFFER_DATE] = {'\0'};

	switch(pMsg->MsgId)
	{
	case WM_PAINT:
		GUI_GetClientRect(&Rect);
		GUI_SetColor(GUI_WHITE);
		GUI_FillRoundedRect(Rect.x0,Rect.y0,Rect.x1,Rect.y1,5);
		GUI_SetBkColor(GUI_WHITE);
		GUI_SetColor(GUI_BLACK);
		GUI_DrawRoundedRect(Rect.x0,Rect.y0,Rect.x1,Rect.y1,5);

		GUI_SetColor(GUI_BLACK);
		GUI_FillPolygon(&s_aPointDownArrowLarge[0],3,Rect.x0,Rect.y0);
		break;
	case WM_PID_STATE_CHANGED:
		if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 0)
		{
			//Sub switch
			switch(WM_GetId(pMsg->hWin))
			{
			case eDownYearBtnId:
			{
				uint16_t yearValue = settingCalendarButton_GetValue(eValueYearSettingId);

				yearValue -= STEP_DATE;

				if(yearValue < MIN_YEAR)
				{
					yearValue = MIN_YEAR;
				}

				sprintf((char*)intBuffer,"%.4d",yearValue);
				EDIT_SetText(s_valueSettingList[eValueYearSettingId],(char*)intBuffer);
				break;
			}
			case eDownMonthBtnId:
			{
				uint8_t monthValue = settingCalendarButton_GetValue(eValueMonthSettingId);

				monthValue -= STEP_DATE;

				if(monthValue < MIN_MONTH)
				{
					monthValue = MIN_MONTH;
				}

				sprintf((char*)intBuffer,"%.2d",monthValue);
				EDIT_SetText(s_valueSettingList[eValueMonthSettingId],(char*)intBuffer);
				break;
			}
			case eDownDayBtnId:
			{
				uint8_t dayValue = settingCalendarButton_GetValue(eValueDaySettingId);

				dayValue -= STEP_DATE;

				if(dayValue < MIN_DAY)
				{
					dayValue = MIN_DAY;
				}

				sprintf((char*)intBuffer,"%.2d",dayValue);
				EDIT_SetText(s_valueSettingList[eValueDaySettingId],(char*)intBuffer);
				break;
			}
			case eDownHourBtnId:
			{
				uint8_t hourValue = settingCalendarButton_GetValue(eValueHourSettingId);
				if(hourValue == 0)
				{
					break;
				}

				hourValue -= STEP_DATE;

				if(hourValue <= MIN_HOUR)
				{
					hourValue = MIN_HOUR;
				}

				sprintf((char*)intBuffer,"%.2d",hourValue);
				EDIT_SetText(s_valueSettingList[eValueHourSettingId],(char*)intBuffer);
				break;
			}
			case eDownMinuteBtnId:
			{
				uint8_t minuteValue = settingCalendarButton_GetValue(eValueMinuteSettingId);
				if(minuteValue == 0)
				{
					break;
				}

				minuteValue -= STEP_DATE;

				if(minuteValue <= MIN_MINUTE)
				{
					minuteValue = MIN_MINUTE;
				}

				sprintf((char*)intBuffer,"%.2d",minuteValue);
				EDIT_SetText(s_valueSettingList[eValueMinuteSettingId],(char*)intBuffer);
				break;
			}
			case eDownSecondBtnId:
			{
				uint8_t secondValue = settingCalendarButton_GetValue(eValueSecondSettingId);
				if(secondValue == 0)
				{
					break;
				}

				secondValue -= STEP_DATE;

				if(secondValue <= MIN_SECOND)
				{
					secondValue = MIN_SECOND;
				}

				sprintf((char*)intBuffer,"%.2d",secondValue);
				EDIT_SetText(s_valueSettingList[eValueSecondSettingId],(char*)intBuffer);
				break;
			}
			default:
				break;
			}
			//End sub switch
		}
		break;
	default:
		BUTTON_Callback(pMsg);
		break;
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-settingCalendarButton_Init					 								*/
/*                                                                                      */
/* Details  -Initialize setting calendar button not save to EPPROM                      */
/*																						*/
/*                                                                                      */
/* Arguments : (I) EDIT_Handle obj : object setting 									*/
/* 				   int upBtnId : - id of up button 										*/
/*        		 		- range : 0-336	                                                */
/*        		 		- unit : NONE	                                                */
/*        		 			                                                            */
/* 				   int downBtnId : id of down button 									*/
/*        		 		- range : 0-336	                                                */
/*        		 		- unit : NONE	                                                */
/*        		 			                                                            */
/* 				   E_ValueSettingCalendarBtnId posArr : id of edit value 				*/
/*        		 		- range : 0-7	                                                */
/*        		 		- unit : NONE	                                                */
/*        		 			                                                            */
/* 				   int valueId : id of setting button 									*/
/*        		 		- range : 0-336	                                                */
/*        		 		- unit : NONE	                                                */
/*        		 			                                                            */
/* 				   int settingId : id of setting button									*/
/*        		 		- range : 0-336	                                                */
/*        		 		- unit : NONE	                                                */
/*        		 			                                                            */
/* 				   GUI_RECT Rect : rectangle of object setting					    	*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_upBtn : up button                                          */
/*                     (O) s_downBtn : down button						                */
/*                     (O) s_valueSettingList : setting list					    	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void settingCalendarButton_Init(EDIT_Handle obj, int upBtnId,int downBtnId,
		E_ValueSettingCalendarBtnId posArr,int valueId,GUI_RECT Rect)
{
	s_upBtn = BUTTON_CreateEx(Rect.x0,Rect.y0 + 20,50,50,obj,WM_CF_SHOW | WM_CF_MEMDEV | WM_CF_LATE_CLIP,0,upBtnId);
	WM_SetCallback(s_upBtn,settingCalendarButton_CallbackUpBtn);

	s_downBtn = BUTTON_CreateEx(Rect.x0 + 50,Rect.y0 + 20,50,50,obj,WM_CF_SHOW | WM_CF_MEMDEV | WM_CF_LATE_CLIP,0,downBtnId);
	WM_SetCallback(s_downBtn,settingCalendarButton_CallbackDownBtn);

	s_valueSettingList[posArr] = EDIT_CreateEx(Rect.x0 + 100,Rect.y0 + 20,90,50,obj,WM_CF_SHOW | WM_CF_MEMDEV | WM_CF_LATE_CLIP,0,valueId,10);
	EDIT_SetFocussable(s_valueSettingList[posArr],EDIT_CI_DISABLED);
	guiTaskEDIT_SetFont(s_valueSettingList[posArr],&GUI_FontEng3L14B2PP);
	EDIT_SetTextAlign(s_valueSettingList[posArr],GUI_TA_VCENTER | GUI_TA_HCENTER);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-settingCalendarButton_SetNameSetting					 					*/
/*                                                                                      */
/* Details  -Set name setting calendar button                                           */
/*																						*/
/*                                                                                      */
/* Arguments : (I) EDIT_Handle obj : object setting										*/
/* 				   GUI_RECT Rect : rectangle of object setting					    	*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void settingCalendarButton_SetNameSetting(EDIT_Handle obj, GUI_RECT Rect)
{
	const int8_t* nameStr = (int8_t*)"";

	switch(WM_GetId(obj))
	{
	case eYearId:
		nameStr = (int8_t*)"Year";
		break;
	case eMonthId:
		nameStr = (int8_t*)"Month";
		break;
	case eDayId:
		nameStr = (int8_t*)"Dayr";
		break;
	case eHourId:
		nameStr = (int8_t*)"Hour";
		break;
	case eMinuteId:
		nameStr = (int8_t*)"Minute";
		break;
	case eSecondId:
		nameStr = (int8_t*)"Second";
		break;
	default:
		break;
	}
	guiTask_SetFont(&GUI_FontEng3L10B2PP);
	GUI_DispStringAt((char*)nameStr,Rect.x0,Rect.y0 + 2);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-settingCalendarButton_SetUintSetting					 					*/
/*                                                                                      */
/* Details  -Set uint setting                                                 			*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) EDIT_Handle obj : object setting										*/
/* 				   GUI_RECT Rect : rectangle of object setting					    	*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void settingCalendarButton_SetUintSetting(EDIT_Handle obj, GUI_RECT Rect)
{
	//get Unit
	const int8_t* unitStr = (int8_t*)"";

	GUI_SetColor(GUI_BLACK);
	GUI_SetTextAlign(GUI_TA_LEFT);

	switch(WM_GetId(obj))
	{
	case eYearId:
		if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
		{
			unitStr = (int8_t*)"Year";
		}
		else
		{
			unitStr = (int8_t*)"年";
		}
		break;
	case eMonthId:
		if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
		{
			unitStr = (int8_t*)"Month";
		}
		else
		{
			unitStr = (int8_t*)"月";
		}
		break;
	case eDayId:
		if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
		{
			unitStr = (int8_t*)"Day";
		}
		else
		{
			unitStr = (int8_t*)"日";
		}
		break;
	case eHourId:
		if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
		{
			unitStr = (int8_t*)"Hour";
		}
		else
		{
			unitStr = (int8_t*)"時";
		}
		break;
	case eMinuteId:
		if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
		{
			unitStr = (int8_t*)"Minute";
		}
		else
		{
			unitStr = (int8_t*)"分";
		}
		break;
	case eSecondId:
		if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
		{
			unitStr = (int8_t*)"Second";
		}
		else
		{
			unitStr = (int8_t*)"秒";
		}
		break;
	default:
		break;
	}

	if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
	{
		guiTask_SetFont(&GUI_FontEng3L8B2PP);
		GUI_DispStringAt((char*)unitStr,180+20-10,Rect.y1 - 29);
	}
	else
	{
		guiTask_SetFont(&GUI_FontJapan3L12B2PP);
		GUI_DispStringAt((char*)unitStr,183+20-10,Rect.y1 - 29);
	}

	return;
}


/****************************************************************************************/
/*                                                                                      */
/* General 	-settingCalendarButton_Callback 								            */
/*                                                                                      */
/* Details  -Call back function for settingCalendarButton, handle event from     	    */
/* 				the window																*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) WM_MESSAGE *pMsg : messenger from the window					 		*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void settingCalendarButton_Callback(WM_MESSAGE * pMsg)
{
	GUI_RECT Rect;
	switch(pMsg->MsgId)
	{
	case WM_PAINT:
		GUI_GetClientRect(&Rect);
		GUI_SetBkColor(SERVICE_BACKGROUND_COLOR);
		GUI_ClearRectEx(&Rect);
		GUI_SetColor(GUI_BLACK);

		settingCalendarButton_SetUintSetting(pMsg->hWin,Rect);
		break;
	default:
		EDIT_Callback(pMsg);
		break;
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-settingCalendarButton_SetValue 								            */
/*                                                                                      */
/* Details  -Set value for setting button												*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) E_ValueSettingCalendarBtnId valueEdit : value id					 	*/
/* 				   const int8_t * value : value											*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_valueSettingList : setting list                            */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void settingCalendarButton_SetValue(E_ValueSettingCalendarBtnId valueEdit,const int8_t * value)
{
	EDIT_SetText(s_valueSettingList[valueEdit],(char*)value);
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-settingCalendarButton_GetValue 								            */
/*                                                                                      */
/* Details  -Get value for setting button												*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) E_ValueSettingCalendarBtnId valueEdit :  - value id					*/
/*						- range : 0-7													*/
/*                      - unit : NONE                                                   */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) s_valueSettingList : setting list		            		*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
int settingCalendarButton_GetValue(E_ValueSettingCalendarBtnId valueEdit)
{
	for(int i = 0; i < SIZE_BUFFER_DATE; i++)
	{
		s_bufferSettingValue[i] = '\0';
	}
	int value = 0;
	EDIT_GetText(s_valueSettingList[valueEdit],(char*)s_bufferSettingValue,10);

	value = atoi((char*)s_bufferSettingValue);

	return value;
}

/* end of files */

