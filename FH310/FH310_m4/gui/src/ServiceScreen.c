/********************************************************************/
/*                                                                  */
/* File Name    : ServiceScreen.c                       			*/
/*                                                                  */
/* General       : Service screen									*/
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
#include "ServiceScreen.h"
#include "MainScreen.h"
#include "OptionScreen.h"
#include "EquipmentScreen.h"
#include "MaintenanceScreen.h"

WM_HWIN g_serviceScreen;

/****************************************************************************************/
/*                                                                                      */
/* General 	-serviceScreen_Callback	                                            		*/
/*                                                                                      */
/* Details  -call back function for service screen, handle event from window		    */
/*        																				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) WM_MESSAGE *pMsg : messenger	from window 					  		*/
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
void serviceScreen_Callback(WM_MESSAGE *pMsg)
{
	GUI_RECT Rect;
	switch (pMsg->MsgId) {
	case WM_PAINT:
		GUI_GetClientRect(&Rect);
		GUI_SetBkColor(SERVICE_BACKGROUND_COLOR);
		GUI_ClearRectEx(&Rect);
		break;
	default:
		WM_DefaultProc(pMsg);
		break;
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-serviceScreen_Init			 							                	*/
/*                                                                                      */
/* Details  -Initialize service screen 			            	                		*/
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
/*                     (O) g_serviceScreen : service screen                             */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void serviceScreen_Init(void)
{
	g_serviceScreen = WM_CreateWindowAsChild(0, 0, 800, 480, g_mainWindow,WM_CF_HIDE | WM_CF_MEMDEV | WM_CF_LATE_CLIP, serviceScreen_Callback, 0);

	optionScreen_Init();
	equipmentScreen_Init();
	maintenanceScreen_Init();

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-serviceScreen_Show			 							                	*/
/*                                                                                      */
/* Details  -Show service screen 			            	                			*/
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
/*                     (O) g_serviceScreen : service screen                             */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void serviceScreen_Show(void)
{
	equipmentScreen_Hide();
	maintenanceScreen_Hide();
	optionScreen_SetTab(eServiceTabId);
	optionScreen_Show();
	WM_ShowWindow(g_serviceScreen);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-serviceScreen_Hide			 							                	*/
/*                                                                                      */
/* Details  -Hide service screen 			            	                			*/
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
/*                     (O) g_serviceScreen : service screen                             */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void serviceScreen_Hide(void)
{
	WM_HideWindow(g_serviceScreen);

	return;
}

/* end of files */

