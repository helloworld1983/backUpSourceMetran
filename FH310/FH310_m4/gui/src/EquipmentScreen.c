/********************************************************************/
/*                                                                  */
/* File Name    : EquipmentScreen.c		                            */
/*                                                                  */
/* General      : Equipment screen include :						*/
/* 					-Equipment information screen				    */
/* 					-Equipment configure screen 					*/
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

#include "EquipmentScreen.h"
#include "ServiceScreen.h"
#include "MainScreen.h"
#include "EquipmentInforScreen.h"
#include "EquipmentSettingScreen.h"
#include "EquipmentCumulativeHourScreen.h"
#include "ConnectedEquipmentInfoScreen.h"

WM_HWIN g_equipmentScreen;

/****************************************************************************************/
/*                                                                                      */
/* General 	-equipmentScreen_Callback	                                                */
/*                                                                                      */
/* Details  -call back function for equipment screen, handle event from window	    	*/
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
void equipmentScreen_Callback(WM_MESSAGE *pMsg)
{
	GUI_RECT Rect;

	switch(pMsg->MsgId)
	{
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
/* General 	-equipmentScreen_Init								       					*/
/*                                                                                      */
/* Details  -Initialize equipment screen        										*/
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
/*                     (O) g_equipmentScreen : equipment screen                         */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void equipmentScreen_Init(void)
{
	g_equipmentScreen = WM_CreateWindowAsChild(0, 0, 800, 480,g_serviceScreen,WM_CF_HIDE | WM_CF_MEMDEV | WM_CF_LATE_CLIP, equipmentScreen_Callback, 0);

	equipmentInforScreen_Init();
	equipmentSettingScreen_Init();
	equipmentCumulativeHourScreen_Init();
	connectedEquipmentInforScreen_Init();

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-equipmentScreen_Show								       					*/
/*                                                                                      */
/* Details  -Show equipment screen        												*/
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
/*                     (O) g_equipmentScreen : equipment screen                         */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void equipmentScreen_Show(void)
{
	if(!WM_IsVisible(g_equipmentScreen))
	{
		WM_ShowWindow(g_equipmentScreen);
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-equipmentScreen_Hide								       					*/
/*                                                                                      */
/* Details  -Hide equipment screen        												*/
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
/*                     (O) g_equipmentScreen : equipment screen                         */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void equipmentScreen_Hide(void)
{
	WM_HideWindow(g_equipmentScreen);

	return;
}

/* end of files */
