/********************************************************************/
/*                                                                  */
/* File Name    : MaintenanceScreen.c		                        */
/*                                                                  */
/* General      : Maintenance screen include:						*/
/* 					monitor screen                                  */
/* 					log data screen                                 */
/* 					calendar setting screen                         */
/* 					software upgrade screen                         */
/* 					function test screen                            */
/* 					serial number setting screen 	                */
/* 					unit cumulative operation hour screen           */
/* 					other setting screen                            */
/* 					psa parameter change screen                     */
/* 					development setting screen						*/
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
#include "MaintenanceScreen.h"
#include "ServiceScreen.h"
#include "MonitorScreen.h"
#include "MainScreen.h"
#include "LogDataScreen.h"
#include "MonitorGraphScreen.h"
#include "PSAParameterChangePageOneScreen.h"
#include "PSAParameterChangePageTwoScreen.h"
#include "FunctionTestPageOneScreen.h"
#include "FunctionTestPageTwoScreen.h"
#include "FunctionTestPageThreeScreen.h"
#include "FunctionTestPageFourScreen.h"
#include "SoftwareUpgradeScreen.h"
#include "ScreenTest.h"
#include "TouchPanelScreen.h"
#include "OtherSettingScreen.h"
#include "CalendarSettingScreen.h"
#include "SerialNumberSettingScreen.h"
#include "UnitCumulativeHourScreen.h"
#include "DevelopmentSettingScreen.h"
#include "DevelopmentSettingScreenPageTwo.h"
#include "DevelopmentSettingScreenPageThree.h"
#include "DevelopmentSettingScreenPageFour.h"
#include "DevelopmentSettingScreenPageFive.h"

WM_HWIN g_maintenanceScreen;

/****************************************************************************************/
/*                                                                                      */
/* General 	-maintenanceScreen_Callback	                                            	*/
/*                                                                                      */
/* Details  -call back function for maintenance screen, handle event from window	    */
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
void maintenanceScreen_Callback(WM_MESSAGE *pMsg)
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
/* General 	-maintenanceScreen_Init								       					*/
/*                                                                                      */
/* Details  -Initialize maintenance screen        										*/
/*			  																			*/
/*                                                                                      */
/* Arguments : (I) NONE					 												*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) g_maintenanceScreen : maintenance screen          			*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void maintenanceScreen_Init(void)
{
	g_maintenanceScreen = WM_CreateWindowAsChild(0, 0, 800, 480,g_serviceScreen,WM_CF_HIDE | WM_CF_MEMDEV | WM_CF_LATE_CLIP, maintenanceScreen_Callback, 0);

	monitorScreen_Init();
	monitorGraphScreen_Init();
	logDataScreen_Init();
	PSAParameterChangePageOneScreen_Init();
	PSAParameterChangePageTwoScreen_Init();
	functionTestPageOneScreen_Init();
	functionTestPageTwoScreen_Init();
	functionTestPageThreeScreen_Init();
	functionTestPageFourScreen_Init();
	softwareUpgradeScreen_Init();
	screenTest_Init();
	touchPanelScreen_Init();
	otherSettingScreen_Init();
	calendarSettingScreen_Init();
	serialNumberSettingScreen_Init();
	unitCumulativeHourScreen_Init();
	developmentSettingScreen_Init();
	developmentSettingScreenPageTwo_Init();
	developmentSettingScreenPageFive_Init();
	developmentSettingScreenPageThree_Init();
	developmentSettingScreenPageFour_Init();

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-maintenanceScreen_Show								       					*/
/*                                                                                      */
/* Details  -Show maintenance screen        											*/
/*			  																			*/
/*                                                                                      */
/* Arguments : (I) NONE					 												*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) g_maintenanceScreen : maintenance screen     			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void maintenanceScreen_Show(void)
{
	WM_ShowWindow(g_maintenanceScreen);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-maintenanceScreen_Hide								       					*/
/*                                                                                      */
/* Details  -Hide maintenance screen        											*/
/*			  																			*/
/*                                                                                      */
/* Arguments : (I) NONE					 												*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) g_maintenanceScreen : maintenance screen    			   	    */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void maintenanceScreen_Hide(void)
{
	WM_HideWindow(g_maintenanceScreen);

	return;
}

/* end of files */
