/********************************************************************/
/*                                                                  */
/* File Name    : ScreenTest.c                       			    */
/*                                                                  */
/* General       : ScreenTest allow user to test screen				*/
/*                 	 						                        */
/*                                                                  */
/* Product Name  : FH310                                            */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2016/08/22      Viet Le			  	new file        */
/* #0001       														*/
/*                                                                  */
/********************************************************************/

#include "board.h"
#include "ScreenTest.h"
#include "MainScreen.h"
#include "MaintenanceScreen.h"
#include "FunctionTestPageFourScreen.h"
#include "AlarmInfor.h"

static WM_HWIN s_screenTest;

/****************************************************************************************/
/*                                                                                      */
/* General 	-screenTest_Callback			 							                */
/*                                                                                      */
/* Details  -Call back function for screen test, handle event from the window 		   	*/
/* 																						*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) WM_MESSAGE *pMsg : message from the window							*/
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
void screenTest_Callback(WM_MESSAGE *pMsg)
{
	GUI_RECT Rect;
	static E_TestScreen testScreen = eTestBlackScreen;
	switch(pMsg->MsgId)
	{
	case WM_PAINT:
	{
		//Start sub switch
		switch(testScreen)
		{
		case eTestBlackScreen:
			GUI_GetClientRect(&Rect);
			GUI_SetBkColor(GUI_BLACK);
			GUI_ClearRectEx(&Rect);
			break;
		case eTestWhiteScreen:
			GUI_GetClientRect(&Rect);
			GUI_SetBkColor(GUI_WHITE);
			GUI_ClearRectEx(&Rect);
			break;
		case eTestBlackWhiteScreen:
			GUI_GetClientRect(&Rect);
			GUI_SetBkColor(GUI_WHITE);
			GUI_SetPenSize(1);
			for(int i = 0 ;i < Rect.x1; i+=(2*PIXEL_INCREASE))
			{
				for(int j = 0;j < Rect.y1; j+=(2*PIXEL_INCREASE))
				{
					GUI_SetColor(GUI_BLACK);
					GUI_FillRect(i,j,i+PIXEL_INCREASE,j+PIXEL_INCREASE);

					GUI_SetColor(GUI_BLACK);
					GUI_FillRect(i+PIXEL_INCREASE,j+PIXEL_INCREASE,i+(2*PIXEL_INCREASE),j+(2*PIXEL_INCREASE));
				}
			}
			break;
		case eEndTestScreen:
			screenTest_Hide();
			functionTestPageFourScreen_Show();
			break;
		default:
			break;
		}
		//End sub switch
		break;
	}
	case WM_SET_FOCUS:
		testScreen = eTestBlackScreen;
		break;
	case WM_PID_STATE_CHANGED:
		if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 0)
		{
			switch(testScreen)
			{
			case eTestBlackScreen:
				testScreen = eTestWhiteScreen;
				WM_Paint(pMsg->hWin);
				break;
			case eTestWhiteScreen:
				testScreen = eTestBlackWhiteScreen;
				WM_Paint(pMsg->hWin);
				break;
			case eTestBlackWhiteScreen:
				screenTest_Hide();
				functionTestPageFourScreen_Show();
				testScreen = eTestBlackScreen;
				break;
			case eEndTestScreen:
				break;
			default:
				break;
			}
		}
		break;
	default:
		WM_DefaultProc(pMsg);
		break;
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-screenTest_Init			 							                	*/
/*                                                                                      */
/* Details  -Initialize screen test 			            	                		*/
/* 																						*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE															 		*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_screenTest : screen test                                   */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void screenTest_Init(void)
{
	s_screenTest = WM_CreateWindowAsChild(0,0,800,480,g_maintenanceScreen,WM_CF_HIDE| WM_CF_MEMDEV | WM_CF_LATE_CLIP,screenTest_Callback,0);
	WM_HideWindow(s_screenTest);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-screenTest_Show			 							               		*/
/*                                                                                      */
/* Details  -Show screen test 			            	                				*/
/* 																						*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE															 		*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_screenTest : screen test                                   */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void screenTest_Show(void)
{
	alarmInfor_HideAlarmInfor();
	WM_ShowWindow(s_screenTest);
	WM_SetFocus(s_screenTest);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-screenTest_Hide			 							               		*/
/*                                                                                      */
/* Details  -Hide screen test 			            	                				*/
/* 																						*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE															 		*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_screenTest : screen test                                   */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void screenTest_Hide(void)
{
	WM_HideWindow(s_screenTest);

	return;
}

/* end of files */

