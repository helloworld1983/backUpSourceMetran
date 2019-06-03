/********************************************************************/
/*                                                                  */
/* File Name    : TouchPanelScreen.c                       			*/
/*                                                                  */
/* General       : ScreenTest allow user to test touch panel		*/
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
#include "TouchPanelScreen.h"
#include "WM.h"
#include "MainScreen.h"
#include "MaintenanceScreen.h"
#include "FunctionTestPageFourScreen.h"
#include "AlarmInfor.h"

static WM_HWIN s_touchPanelScreen;
static int s_pixelsArray[X_LIST_SIZE][Y_LIST_SIZE];

/****************************************************************************************/
/*                                                                                      */
/* General 	-touchPanelScreen_Callback				 							        */
/*                                                                                      */
/* Details  -Call back function for touch panel test, handle event from the window 		*/
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
/*                     (O) s_pixelsArray : pixels array                                 */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void touchPanelScreen_Callback(WM_MESSAGE *pMsg)
{
	GUI_RECT Rect;

	int xIndex;
	int yIndex;

	switch(pMsg->MsgId)
	{
	case WM_PAINT:
		GUI_GetClientRect(&Rect);
		GUI_SetBkColor(SERVICE_BACKGROUND_COLOR);
		GUI_ClearRectEx(&Rect);

		GUI_SetPenSize(1);

		for(int i = 0; i < X_LIST_SIZE; i++)
		{
			for(int j = 0; j < Y_LIST_SIZE; j++)
			{
				if(s_pixelsArray[i][j] == 0)//background white, rect small black
				{
					GUI_SetColor(GUI_BLACK);
					GUI_DrawRect(i*40,j*40,i*40+40,j*40+40);

					GUI_FillRect(i*40 + 17,j*40 + 17,i*40 + 17+6,j*40 + 17+6);

				}
				else// backgroud black, rect small white
				{
					GUI_SetColor(GUI_BLACK);
					GUI_FillRect(i*40,j*40,i*40+40,j*40+40);

					GUI_SetColor(GUI_WHITE);
					GUI_FillRect(i*40 + 17,j*40 + 17,i*40 + 17+6,j*40 + 17+6);

				}
			}
		}
		break;
	case WM_PID_STATE_CHANGED:
		if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 0)
		{
			xIndex = (int)(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->x / DIV_40);
			yIndex = (int)(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->y / DIV_40);

			if((xIndex >= X_LIST_SIZE) || (yIndex >= Y_LIST_SIZE))
			{
				break;
			}
			int pressPixel = s_pixelsArray[xIndex][yIndex];

			if(pressPixel == 0)
			{
				s_pixelsArray[xIndex][yIndex] = 1;
			}
			else
			{
				s_pixelsArray[xIndex][yIndex] = 0;

			}

			if((xIndex == X_LIST_SIZE - 1) && (yIndex == Y_LIST_SIZE - 1))
			{
				touchPanelScreen_Hide();
				functionTestPageFourScreen_Show();
				break;
			}
			else
			{
				WM_Paint(pMsg->hWin);
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
/* General 	-touchPanelScreen_Init			 							        		*/
/*                                                                                      */
/* Details  -Initialize touch panel test 			            	                    */
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
/*                     (O) s_touchPanelScreen : touch panel screen               	   	*/
/*                     (O) s_pixelsArray : pixels array						            */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void touchPanelScreen_Init(void)
{
	s_touchPanelScreen = WM_CreateWindowAsChild(0,0,800,480,g_maintenanceScreen,WM_CF_HIDE | WM_CF_MEMDEV | WM_CF_LATE_CLIP,touchPanelScreen_Callback,0);
	for(int i = 0; i < X_LIST_SIZE; i++)
	{
		for(int j = 0; j < Y_LIST_SIZE; j++)
		{
			s_pixelsArray[i][j] = 0;
		}
	}

	WM_HideWindow(s_touchPanelScreen);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-touchPanelScreen_Show			 							        		*/
/*                                                                                      */
/* Details  -Show touch panel test 			            	                    		*/
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
/*                     (O) s_pixelsArray : pixels array                                 */
/*                     (O) s_touchPanelScreen : touch panel screen		            	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void touchPanelScreen_Show(void)
{
	for(int i = 0; i < X_LIST_SIZE; i++)
	{
		for(int j = 0; j < Y_LIST_SIZE; j++)
		{
			s_pixelsArray[i][j] = 0;
		}
	}
	alarmInfor_HideAlarmInfor();
	WM_ShowWindow(s_touchPanelScreen);
	WM_SetFocus(s_touchPanelScreen);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-touchPanelScreen_Hide			 							        		*/
/*                                                                                      */
/* Details  -Hide touch panel test 			            	                    		*/
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
/*                     (O) s_touchPanelScreen : touch panel screen              	   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void touchPanelScreen_Hide(void)
{
	WM_HideWindow(s_touchPanelScreen);

	return;
}

/* end of files */
