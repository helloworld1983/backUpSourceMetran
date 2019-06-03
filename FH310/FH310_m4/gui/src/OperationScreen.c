/********************************************************************/
/*                                                                  */
/* File Name    : OperationScreen.c                 		        */
/*                                                                  */
/* General       : OperationScreen display the value of flow oxygen,*/
/*                 display the status of machine      	 			*/
/*                                                                  */
/* Product Name  : FH310                                            */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2008/08/22      Viet Le  			new file       	*/
/* #0001       2017/08/25	   Viet Le				modify 			*/
/*       Change AnypalMgr to AnypalDisplayHandler                   */
/* #0002       2017/09/05	   Viet Le				modify 			*/
/*       remove #include "assert.h"									*/
/* #0003       2017/09/07	   Viet Le				modify 			*/
/*		 -Edit function operationScreen_ShowO2Cylinder()->      	*/
/*		 operationScreen_CheckAndShowO2Cylinder()					*/
/*       -Remove check flow path pressure (bug 1823)				*/
/* #0004       2017/09/12       Linh Nguyen	  	    modify          */
/*  Use new save log function                            			*/
/* #0005       2017/09/16	   Viet Le				modify 			*/
/*		 -Add check if operation screen show O2 cylinder icon,		*/
/* 			else hide O2 cylinder icon								*/
/* #0006       2017/09/27	   Viet Le				modify 			*/
/*		- Set flag enable irda is false and start timer enable irda */
/*		in operationScreen_EnterStartOperation() function (bug 1918)*/
/* #0007       2017/10/03	   Viet Le				modify 			*/
/*	    - Add flag s_changedFR. When change flow rate by 			*/
/*         encode or remote s_changedFR is true, after 5s in        */
/*         child lock mode s_changedFR is false.If user confirm by  */
/*         encoder or remote s_changedFR is false. Check flag in 	*/
/*		   operationScreen_ShowHideChildlock() to blinking flow rate*/
/*          when change flow rate in child lock mode(bug 1947)		*/
/* #0008       2017/10/19	   Viet Le				modify 			*/
/*      - Add passwordDialog_DisplayFlowRate() to dispaly flow rate */
/*			when change flow rate in operation screen(bug 1986)		*/
/* #0009       2017/10/27	   Viet Le				modify			*/
/*	-Edit function : TEXT_SetFont()->guiTaskTEXT_SetFont()(bug 1997)*/
/* #0010       2017/11/07	   Viet Le				modify			*/
/*  -When flow rate change in alarm screen(child lock ON), alarm  	*/
/*  inactive set flow rate on RAM = setting flow rate on EEPROM.	*/
/*	 Show flow rate in operation screen (bug 2017)					*/
/* #0011       2017/11/07	   Viet Le				modify			*/
/*	- Remove unused code											*/
/* #0012       2017/11/17       Viet Le				modify			*/
/*	Remove function operationScreen_SetExtFlowPath() (coverity)		*/
/* #0013       2017/11/17       Viet Le				modify			*/
/*	Remove s_extFlowPathPressure (coverity)							*/
/* #0014       2017/12/01       Viet Le				modify			*/
/*  Handle flag blinking flow rate when change flow rate with 		*/
/*   child lock mode(bug 2046)										*/
/* #0016	   2018/01/18      Viet Le 	    	        modify	    */
/*	 When the flow rate changes, wait 1 second to play the flow rate*/
/*		(bug 2057)													*/
/* #0017	   2018/02/01      Viet Le 	    	        modify	    */
/*   Not check current flow rate and pendding flow rate when play 	*/
/*		flow rate after 1 seconds(bug 2059)							*/
/* #0018	   2018/02/06      Viet Le 	    	        modify	    */
/*	 Add comment(bug 2064) 											*/
/********************************************************************/
#include "stdlib.h"
#include "string.h"
#include <EEPROM.h>
#include <StringTools.h>
#include "OperationScreen.h"
#include "board.h"
#include "TaskCommon.h"
#include "setting.h"
#include "BUTTON.h"
#include "IMAGE.h"
#include "StringTable.h"
#include "GuiTask.h"
#include "TEXT.h"
#include "PasswordDialog.h"
#include "AudioMgr.h"
#include "ModeMgr.h"
#include "GuiHandler.h"
#include "Font.h"
#include "AlarmScreenMgr.h"
#include "StandbyScreen.h"
#include "Bitmap.h"
#include "GUI.h"
#include "GuiInterface.h"
#include "LogInterface.h"
#include "AnyPalDisplayHandler.h"

//Define item id
enum{
	eAninationTimerId,
	eSendDeviceTimerId,
	eChildLockTimerId,
	eUpdateCumulativeHourId,
	eO2CylinderIconId,
	eChildlockIconId,
	eTimer1MinutesId,
	eTimer20SecondsId,
	eTimerChargingAnypalId,
	eImageOperationZeroId,
	eImageOperationOneId,
	eImageOperationTwoId,
	eImageOperationThreeId,
	eImageOperationFourId,
	eImageOperationFiveId,
	eImageOperationSixId,
	eImageOperationSevenId,
	eImageOperationEightId,
	eImageOperationNineId,
	eOperationFlowRateSettingValueStringID,
	eOperationFlowRateSettingUnitStringID,
	eOperationFlowRateSettingNameStringID,
	eOperationStatusStringID,
	eTimerUpdateFRId,
	eToplineOperationScrId,
	eBottomlineOperationScrId,
	eAnypalConnectTxtOperationScrId,
	eAnypalChargingTxtOperationScrId
};

WM_HWIN g_operationScreen;

static GUI_BITMAP s_bitmapanimationOne;
static GUI_BITMAP s_bitmapanimationTwo;
static GUI_BITMAP s_bitmapattachO2Cylinder;
static GUI_BITMAP s_bitmapnoattachO2Cylinder;
static GUI_BITMAP s_bitmapAnypalconnect;
static GUI_BITMAP s_bitmapAroundCharging;
static GUI_BITMAP s_bitmapCharging;
static GUI_BITMAP s_bitmapChildLock;
static WM_HTIMER s_timerUpdateFR;
static WM_HTIMER s_timerChildLock;
static IMAGE_Handle s_animationOne;
static IMAGE_Handle s_animationTwo;
static IMAGE_Handle s_animationThree;
static IMAGE_Handle s_animationFour;
static IMAGE_Handle s_animationFine;
static IMAGE_Handle s_animationSix;
static IMAGE_Handle s_animationSeven;
static IMAGE_Handle s_anypalConnect;
static IMAGE_Handle s_aroundCharging;
static IMAGE_Handle s_charging;
static IMAGE_Handle s_O2Cylinder;
static IMAGE_Handle s_childlockIcon;
static TEXT_Handle s_flowRateValue;
static TEXT_Handle s_flowRateName;
static TEXT_Handle s_flowRateUint;
static TEXT_Handle s_statusMachine;
static TEXT_Handle s_bottomLine;
static TEXT_Handle s_topLine;
static TEXT_Handle s_anypalConnectTxt;
static TEXT_Handle s_anypalChargingTxt;

static int s_patientHour = 0;
static int s_patientMin = 0;
static int s_patientSec = 0;

static int s_unitHour = 0;
static int s_unitMin = 0;
static int s_unitSec = 0;

static int s_unitHourThree = 0;
static int s_unitMinThree = 0;
static int s_unitSecThree = 0;

static int s_unitHourTwoFive = 0;
static int s_unitMinTwoFive = 0;
static int s_unitSecTwoFive = 0;

static int s_unitHourTwo = 0;
static int s_unitMinTwo = 0;
static int s_unitSecTwo = 0;

static int s_unitHourOneSevenFive = 0;
static int s_unitMinOneSevenFive = 0;
static int s_unitSecOneSevenFive = 0;

static int s_unitHourOneFive = 0;
static int s_unitMinOneFive = 0;
static int s_unitSecOneFive = 0;

static int s_unitHourOneTwoFive = 0;
static int s_unitMinOneTwoFive = 0;
static int s_unitSecOneTwoFive = 0;

static int s_unitHourOne = 0;
static int s_unitMinOne = 0;
static int s_unitSecOne = 0;

static int s_unitHourZeroSevenFive = 0;
static int s_unitMinZeroSevenFive = 0;
static int s_unitSecZeroSevenFive = 0;

static int s_unitHourZeroFive = 0;
static int s_unitMinZeroFive = 0;
static int s_unitSecZeroFive = 0;

static int s_unitHourZeroTwoFive = 0;
static int s_unitMinZeroTwoFive = 0;
static int s_unitSecZeroTwoFive = 0;

static int s_equipHour = 0;
static int s_equipMin = 0;
static int s_equipSec = 0;
static int s_equipHourStart = 0;

static int s_counter5s = 0;
static E_Touch s_touchPos = eTouchScrOne;
static bool s_changedFR = false;

/****************************************************************************************/
/*                                                                                      */
/* General 	-operationScreen_updateAnimation 								            */
/*                                                                                      */
/* Details  -update animation in operation screen 		                                */
/*			  														                    */
/*                                                                                      */
/* Arguments : (I) NONE                                      					 		*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_animationOne : animation image                             */
/*                     (O) s_animationTwo : animation image						        */
/*                     (O) s_animationThree : animation image                           */
/*                     (O) s_animationFour : animation image				            */
/*                     (O) s_animationFine : animation image                            */
/*                     (O) s_animationSix : animation image						        */
/*                     (O) s_animationSeven : animation image      		   				*/
/*【Note】                                                                              													*/
/*                                                                                      */
/****************************************************************************************/
void operationScreen_updateAnimation(void)
{
	static int counter = 0;
	switch(counter)
	{
	case 0:
		guiTask_IMAGESetBitmap(s_animationOne,&s_bitmapanimationTwo);
		guiTask_IMAGESetBitmap(s_animationTwo,&s_bitmapanimationOne);
		guiTask_IMAGESetBitmap(s_animationThree,&s_bitmapanimationOne);
		guiTask_IMAGESetBitmap(s_animationFour,&s_bitmapanimationOne);
		guiTask_IMAGESetBitmap(s_animationFine,&s_bitmapanimationOne);
		guiTask_IMAGESetBitmap(s_animationSix,&s_bitmapanimationOne);
		guiTask_IMAGESetBitmap(s_animationSeven,&s_bitmapanimationOne);
		break;
	case IMAGE_ANIM_ONE:
		guiTask_IMAGESetBitmap(s_animationOne,&s_bitmapanimationOne);
		guiTask_IMAGESetBitmap(s_animationTwo,&s_bitmapanimationTwo);
		guiTask_IMAGESetBitmap(s_animationThree,&s_bitmapanimationOne);
		guiTask_IMAGESetBitmap(s_animationFour,&s_bitmapanimationOne);
		guiTask_IMAGESetBitmap(s_animationFine,&s_bitmapanimationOne);
		guiTask_IMAGESetBitmap(s_animationSix,&s_bitmapanimationOne);
		guiTask_IMAGESetBitmap(s_animationSeven,&s_bitmapanimationOne);
		break;
	case IMAGE_ANIM_TWO:
		guiTask_IMAGESetBitmap(s_animationOne,&s_bitmapanimationOne);
		guiTask_IMAGESetBitmap(s_animationTwo,&s_bitmapanimationOne);
		guiTask_IMAGESetBitmap(s_animationThree,&s_bitmapanimationTwo);
		guiTask_IMAGESetBitmap(s_animationFour,&s_bitmapanimationOne);
		guiTask_IMAGESetBitmap(s_animationFine,&s_bitmapanimationOne);
		guiTask_IMAGESetBitmap(s_animationSix,&s_bitmapanimationOne);
		guiTask_IMAGESetBitmap(s_animationSeven,&s_bitmapanimationOne);
		break;
	case IMAGE_ANIM_THREE:
		guiTask_IMAGESetBitmap(s_animationOne,&s_bitmapanimationOne);
		guiTask_IMAGESetBitmap(s_animationTwo,&s_bitmapanimationOne);
		guiTask_IMAGESetBitmap(s_animationThree,&s_bitmapanimationOne);
		guiTask_IMAGESetBitmap(s_animationFour,&s_bitmapanimationTwo);
		guiTask_IMAGESetBitmap(s_animationFine,&s_bitmapanimationOne);
		guiTask_IMAGESetBitmap(s_animationSix,&s_bitmapanimationOne);
		guiTask_IMAGESetBitmap(s_animationSeven,&s_bitmapanimationOne);
		break;
	case IMAGE_ANIM_FOUR:
		guiTask_IMAGESetBitmap(s_animationOne,&s_bitmapanimationOne);
		guiTask_IMAGESetBitmap(s_animationTwo,&s_bitmapanimationOne);
		guiTask_IMAGESetBitmap(s_animationThree,&s_bitmapanimationOne);
		guiTask_IMAGESetBitmap(s_animationFour,&s_bitmapanimationOne);
		guiTask_IMAGESetBitmap(s_animationFine,&s_bitmapanimationTwo);
		guiTask_IMAGESetBitmap(s_animationSix,&s_bitmapanimationOne);
		guiTask_IMAGESetBitmap(s_animationSeven,&s_bitmapanimationOne);
		break;
	case IMAGE_ANIM_FIVE:
		guiTask_IMAGESetBitmap(s_animationOne,&s_bitmapanimationOne);
		guiTask_IMAGESetBitmap(s_animationTwo,&s_bitmapanimationOne);
		guiTask_IMAGESetBitmap(s_animationThree,&s_bitmapanimationOne);
		guiTask_IMAGESetBitmap(s_animationFour,&s_bitmapanimationOne);
		guiTask_IMAGESetBitmap(s_animationFine,&s_bitmapanimationOne);
		guiTask_IMAGESetBitmap(s_animationSix,&s_bitmapanimationTwo);
		guiTask_IMAGESetBitmap(s_animationSeven,&s_bitmapanimationOne);
		break;
	case IMAGE_ANIM_SIX:
		guiTask_IMAGESetBitmap(s_animationOne,&s_bitmapanimationOne);
		guiTask_IMAGESetBitmap(s_animationTwo,&s_bitmapanimationOne);
		guiTask_IMAGESetBitmap(s_animationThree,&s_bitmapanimationOne);
		guiTask_IMAGESetBitmap(s_animationFour,&s_bitmapanimationOne);
		guiTask_IMAGESetBitmap(s_animationFine,&s_bitmapanimationOne);
		guiTask_IMAGESetBitmap(s_animationSix,&s_bitmapanimationOne);
		guiTask_IMAGESetBitmap(s_animationSeven,&s_bitmapanimationTwo);
		break;
	default:
		guiTask_IMAGESetBitmap(s_animationOne,&s_bitmapanimationOne);
		guiTask_IMAGESetBitmap(s_animationTwo,&s_bitmapanimationOne);
		guiTask_IMAGESetBitmap(s_animationThree,&s_bitmapanimationOne);
		guiTask_IMAGESetBitmap(s_animationFour,&s_bitmapanimationOne);
		guiTask_IMAGESetBitmap(s_animationFine,&s_bitmapanimationOne);
		guiTask_IMAGESetBitmap(s_animationSix,&s_bitmapanimationOne);
		guiTask_IMAGESetBitmap(s_animationSeven,&s_bitmapanimationOne);
		break;
	}

	counter++;
	if(counter > MAX_IMAGE_ANIMATION)
	{
		counter = 0;
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-operationScreen_CallbackFlowRateName										*/
/*                                                                                      */
/* Details  -call back fuction for flow rate name, handle event from the window 		*/
/*			  														                    */
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
void operationScreen_CallbackFlowRateName(WM_MESSAGE *pMsg)
{
	switch(pMsg->MsgId)
	{
	case WM_PAINT:
		GUI_SetBkColor(PATIENT_BACKGROUND_COLOR);
		GUI_SetPenSize(3);
		GUI_SetColor(BORDER_FR_NAME_COLOR);
		GUI_AA_DrawRoundedRect(3,0,115,70,10);

		if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
		{
			guiTask_SetFont(&GUI_FontEng3L36B2PP);
			GUI_DispStringHCenterAt("FR", 63, 10);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L36B2PP);
			GUI_DispStringHCenterAt("流量", 60, 10);
		}
		break;
	default:
		TEXT_Callback(pMsg);
		break;
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-operationScreen_UpdatePatientCumulativeHour			 					*/
/*                                                                                      */
/* Details  -Update patient cumulative operation hour 									*/
/*			  														                    */
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
/*                     (O) s_patientSec : seconds of patient cumulative hour     		*/
/*                     (O) s_patientMin : minutes of patient cumulative hour            */
/*                     (O) s_patientHour : hour of patient cumulative hour              */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void operationScreen_UpdatePatientCumulativeHour(void)
{
	s_patientSec += INCREASE_CUMULATIVE_HOUR;
	if(s_patientSec > MAX_SEC_OF_CUMULATIVE_HOUR)
	{
		s_patientMin += INCREASE_CUMULATIVE_HOUR;
		s_patientSec = 0;
	}

	if(s_patientMin > MAX_MIN_OF_CUMULATIVE_HOUR)//60minutes
	{
		s_patientHour += INCREASE_CUMULATIVE_HOUR;
		s_patientMin = 0;
	}

	if(s_patientHour > MAX_HOUR_OF_CUMULATIVE_HOUR)//Max hour
	{
		s_patientHour = MAX_HOUR_OF_CUMULATIVE_HOUR;
		s_patientMin = MAX_MIN_OF_CUMULATIVE_HOUR;
		s_patientSec = MAX_SEC_OF_CUMULATIVE_HOUR;
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-operationScreen_UpdateEquipmentCumulativeHour			 					*/
/*                                                                                      */
/* Details  -Update equipment cumulative operation hour 								*/
/*			  														                    */
/*                                                                                      */
/* Arguments : (I) NONE															 		*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) s_equipHourStart : hour of equipment cumulative hour when start operation*/
/*                     (O) s_equipSec : seconds of equipment cumulative hour            */
/*                     (O) s_equipMin : minutes of equipment cumulative hour            */
/*                     (O) s_equipHour : hour of equipment cumulative hour              */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void operationScreen_UpdateEquipmentCumulativeHour(void)
{
	s_equipSec += INCREASE_CUMULATIVE_HOUR;
	if(s_equipSec > MAX_SEC_OF_CUMULATIVE_HOUR)
	{
		s_equipMin += INCREASE_CUMULATIVE_HOUR;
		s_equipSec = 0;
	}

	if(s_equipMin > MAX_MIN_OF_CUMULATIVE_HOUR)//60minutes
	{
		s_equipHour += INCREASE_CUMULATIVE_HOUR;
		s_equipMin = 0;
	}

	if(s_equipHour > MAX_HOUR_OF_CUMULATIVE_HOUR)//Max hour
	{
		s_equipHour = MAX_HOUR_OF_CUMULATIVE_HOUR;
		s_equipMin = MAX_MIN_OF_CUMULATIVE_HOUR;
		s_equipSec = MAX_SEC_OF_CUMULATIVE_HOUR;
	}

	int latchValveHour = s_equipHour - s_equipHourStart;
	if((latchValveHour == HOUR_IN_ONE_MONTH) &&
			(s_equipSec == 0) && (s_equipMin == 0))
	{
		s_equipHourStart = s_equipHour;
		guiTask_SendPendingEvent(eDeviceBackupSystemChangeId,eLatchValveOpenClose);
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-operationScreen_UpdateUnitCumulativeHour			 					    */
/*                                                                                      */
/* Details  -Update unit's cumulative operation hour 									*/
/*			  														                    */
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
/*                     (O) s_unitSec : seconds of unit cumulative hour     			   	*/
/*                     (O) s_unitMin : minutes of unit cumulative hour	            	*/
/*                     (O) s_unitHour : hour of unit cumulative hour     			   	*/
/*                     (O) s_unitSecThree : seconds of unit cumulative hour 3L		    */
/*                     (O) s_unitMinThree : minutes of unit cumulative hour 3L         	*/
/*                     (O) s_unitHourThree : hour of unit cumulative hour 3L        	*/
/*                     (O) s_unitSecTwoFive : seconds of unit cumulative hour 2.5L		*/
/*                     (O) s_unitMinTwoFive : minutes of unit cumulative hour 2.5L      */
/*                     (O) s_unitHourTwoFive : hour of unit cumulative hour 2.5L        */
/*                     (O) s_unitSecTwo : seconds of unit cumulative hour 2L			*/
/*                     (O) s_unitMinTwo : minutes of unit cumulative hour 2L      		*/
/*                     (O) s_unitHourTwo : hour of unit cumulative hour 2L        		*/
/*                     (O) s_unitSecOneSevenFive : seconds of unit cumulative hour 1.75L*/
/*                     (O) s_unitMinOneSevenFive : minutes of unit cumulative hour 1.75L*/
/*                     (O) s_unitHourOneSevenFive : hour of unit cumulative hour 1.75L  */
/*                     (O) s_unitSecOneFive : seconds of unit cumulative hour 1.5L		*/
/*                     (O) s_unitMinOneFive : minutes of unit cumulative hour 1.5L		*/
/*                     (O) s_unitHourOneFive : hour of unit cumulative hour 1.5L        */
/*                     (O) s_unitSecOneTwoFive : seconds of unit cumulative hour 1.25L	*/
/*                     (O) s_unitMinOneTwoFive : minutes of unit cumulative hour 1.25L	*/
/*                     (O) s_unitHourOneTwoFive : hour of unit cumulative hour 1.25L    */
/*                     (O) s_unitSecOne : seconds of unit cumulative hour 1L			*/
/*                     (O) s_unitMinOne : minutes of unit cumulative hour 1L			*/
/*                     (O) s_unitHourOne : hour of unit cumulative hour 1L    			*/
/*                     (O) s_unitSecZeroSevenFive : seconds of unit cumulative hour 0.75L*/
/*                     (O) s_unitMinZeroSevenFive : minutes of unit cumulative hour 0.75L*/
/*                     (O) s_unitHourZeroSevenFive : hour of unit cumulative hour 0.75L */
/*                     (O) s_unitSecZeroFive : seconds of unit cumulative hour 0.5L		*/
/*                     (O) s_unitMinZeroFive : minutes of unit cumulative hour 0.5L		*/
/*                     (O) s_unitHourZeroFive : hour of unit cumulative hour 0.5L  		*/
/*                     (O) s_unitSecZeroTwoFive : seconds of unit cumulative hour 0.25L */
/*                     (O) s_unitMinZeroTwoFive : minutes of unit cumulative hour 0.25L */
/*                     (O) s_unitHourZeroTwoFive : hour of unit cumulative hour 0.25L   */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void operationScreen_UpdateUnitCumulativeHour(void)
{
	//Total unit
	s_unitSec += INCREASE_CUMULATIVE_HOUR;
	if(s_unitSec > MAX_SEC_OF_CUMULATIVE_HOUR)
	{
		s_unitMin += INCREASE_CUMULATIVE_HOUR;
		s_unitSec = 0;
	}

	if(s_unitMin > MAX_MIN_OF_CUMULATIVE_HOUR)
	{
		s_unitHour += INCREASE_CUMULATIVE_HOUR;
		s_unitMin = 0;
	}

	if(s_unitHour > MAX_HOUR_OF_CUMULATIVE_HOUR)
	{
		s_unitHour = MAX_HOUR_OF_CUMULATIVE_HOUR;
		s_unitMin = MAX_MIN_OF_CUMULATIVE_HOUR;
		s_unitSec = MAX_SEC_OF_CUMULATIVE_HOUR;
	}

	uint32_t cumulativeHour = s_unitSec + s_unitMin * SECONDS_IN_ONE_MIN + s_unitHour * SECONDS_IN_ONE_HOUR;
	guiTask_SendPendingEvent(eUintCumulativeHourId,cumulativeHour);

	//Unit cumnulative by flow rate
	if(guiInterface_GetSettingValue(eFlowRateSettingId) == SETTING_FR_300L)
	{
		s_unitSecThree += INCREASE_CUMULATIVE_HOUR;
		if(s_unitSecThree > MAX_SEC_OF_CUMULATIVE_HOUR)
		{
			s_unitMinThree += INCREASE_CUMULATIVE_HOUR;
			s_unitSecThree = 0;
		}

		if(s_unitMinThree > MAX_MIN_OF_CUMULATIVE_HOUR)
		{
			s_unitHourThree += INCREASE_CUMULATIVE_HOUR;
			s_unitMinThree = 0;
		}

		if(s_unitHourThree > MAX_HOUR_OF_CUMULATIVE_HOUR)
		{
			s_unitHourThree = MAX_HOUR_OF_CUMULATIVE_HOUR;
			s_unitMinThree = MAX_MIN_OF_CUMULATIVE_HOUR;
			s_unitSecThree = MAX_SEC_OF_CUMULATIVE_HOUR;
		}
	}
	else if (guiInterface_GetSettingValue(eFlowRateSettingId) == SETTING_FR_250L)
	{
		s_unitSecTwoFive += INCREASE_CUMULATIVE_HOUR;
		if(s_unitSecTwoFive > MAX_SEC_OF_CUMULATIVE_HOUR)
		{
			s_unitMinTwoFive += INCREASE_CUMULATIVE_HOUR;
			s_unitSecTwoFive = 0;
		}

		if(s_unitMinTwoFive > MAX_MIN_OF_CUMULATIVE_HOUR)
		{
			s_unitHourTwoFive += INCREASE_CUMULATIVE_HOUR;
			s_unitMinTwoFive = 0;
		}

		if(s_unitHourTwoFive > MAX_HOUR_OF_CUMULATIVE_HOUR)
		{
			s_unitHourTwoFive = MAX_HOUR_OF_CUMULATIVE_HOUR;
			s_unitMinTwoFive = MAX_MIN_OF_CUMULATIVE_HOUR;
			s_unitSecTwoFive = MAX_SEC_OF_CUMULATIVE_HOUR;
		}
	}
	else if (guiInterface_GetSettingValue(eFlowRateSettingId) == SETTING_FR_200L)
	{
		s_unitSecTwo += INCREASE_CUMULATIVE_HOUR;
		if(s_unitSecTwo > MAX_SEC_OF_CUMULATIVE_HOUR)
		{
			s_unitMinTwo += INCREASE_CUMULATIVE_HOUR;
			s_unitSecTwo = 0;
		}

		if(s_unitMinTwo > MAX_MIN_OF_CUMULATIVE_HOUR)
		{
			s_unitHourTwo += INCREASE_CUMULATIVE_HOUR;
			s_unitMinTwo = 0;
		}

		if(s_unitHourTwo > MAX_HOUR_OF_CUMULATIVE_HOUR)
		{
			s_unitHourTwo = MAX_HOUR_OF_CUMULATIVE_HOUR;
			s_unitMinTwo = MAX_MIN_OF_CUMULATIVE_HOUR;
			s_unitSecTwo = MAX_SEC_OF_CUMULATIVE_HOUR;
		}
	}
	else if (guiInterface_GetSettingValue(eFlowRateSettingId) == SETTING_FR_175L)
	{
		s_unitSecOneSevenFive += INCREASE_CUMULATIVE_HOUR;
		if(s_unitSecOneSevenFive > MAX_SEC_OF_CUMULATIVE_HOUR)
		{
			s_unitMinOneSevenFive += INCREASE_CUMULATIVE_HOUR;
			s_unitSecOneSevenFive = 0;
		}

		if(s_unitMinOneSevenFive > MAX_MIN_OF_CUMULATIVE_HOUR)
		{
			s_unitHourOneSevenFive += INCREASE_CUMULATIVE_HOUR;
			s_unitMinOneSevenFive = 0;
		}

		if(s_unitHourOneSevenFive > MAX_HOUR_OF_CUMULATIVE_HOUR)
		{
			s_unitHourOneSevenFive = MAX_HOUR_OF_CUMULATIVE_HOUR;
			s_unitMinOneSevenFive = MAX_MIN_OF_CUMULATIVE_HOUR;
			s_unitSecOneSevenFive = MAX_SEC_OF_CUMULATIVE_HOUR;
		}
	}
	else if (guiInterface_GetSettingValue(eFlowRateSettingId) == SETTING_FR_150L)
	{
		s_unitSecOneFive += INCREASE_CUMULATIVE_HOUR;
		if(s_unitSecOneFive > MAX_SEC_OF_CUMULATIVE_HOUR)
		{
			s_unitMinOneFive += INCREASE_CUMULATIVE_HOUR;
			s_unitSecOneFive = 0;
		}

		if(s_unitMinOneFive > MAX_MIN_OF_CUMULATIVE_HOUR)
		{
			s_unitHourOneFive += INCREASE_CUMULATIVE_HOUR;
			s_unitMinOneFive = 0;
		}

		if(s_unitHourOneFive > MAX_HOUR_OF_CUMULATIVE_HOUR)
		{
			s_unitHourOneFive = MAX_HOUR_OF_CUMULATIVE_HOUR;
			s_unitMinOneFive = MAX_MIN_OF_CUMULATIVE_HOUR;
			s_unitSecOneFive = MAX_SEC_OF_CUMULATIVE_HOUR;
		}
	}
	else if (guiInterface_GetSettingValue(eFlowRateSettingId) == SETTING_FR_125L)
	{
		s_unitSecOneTwoFive += INCREASE_CUMULATIVE_HOUR;
		if(s_unitSecOneTwoFive > MAX_SEC_OF_CUMULATIVE_HOUR)
		{
			s_unitMinOneTwoFive += INCREASE_CUMULATIVE_HOUR;
			s_unitSecOneTwoFive = 0;
		}

		if(s_unitMinOneTwoFive > MAX_MIN_OF_CUMULATIVE_HOUR)
		{
			s_unitHourOneTwoFive += INCREASE_CUMULATIVE_HOUR;
			s_unitMinOneTwoFive = 0;
		}

		if(s_unitHourOneTwoFive > MAX_HOUR_OF_CUMULATIVE_HOUR)
		{
			s_unitHourOneTwoFive = MAX_HOUR_OF_CUMULATIVE_HOUR;
			s_unitMinOneTwoFive = MAX_MIN_OF_CUMULATIVE_HOUR;
			s_unitSecOneTwoFive = MAX_SEC_OF_CUMULATIVE_HOUR;
		}
	}
	else if (guiInterface_GetSettingValue(eFlowRateSettingId) == SETTING_FR_100L)
	{
		s_unitSecOne += INCREASE_CUMULATIVE_HOUR;
		if(s_unitSecOne > MAX_SEC_OF_CUMULATIVE_HOUR)
		{
			s_unitMinOne += INCREASE_CUMULATIVE_HOUR;
			s_unitSecOne = 0;
		}

		if(s_unitMinOne > MAX_MIN_OF_CUMULATIVE_HOUR)
		{
			s_unitHourOne += INCREASE_CUMULATIVE_HOUR;
			s_unitMinOne = 0;
		}

		if(s_unitHourOne > MAX_HOUR_OF_CUMULATIVE_HOUR)
		{
			s_unitHourOne = MAX_HOUR_OF_CUMULATIVE_HOUR;
			s_unitMinOne = MAX_MIN_OF_CUMULATIVE_HOUR;
			s_unitSecOne = MAX_SEC_OF_CUMULATIVE_HOUR;
		}
	}
	else if(guiInterface_GetSettingValue(eFlowRateSettingId) == SETTING_FR_075L)
	{
		s_unitSecZeroSevenFive += INCREASE_CUMULATIVE_HOUR;
		if(s_unitSecZeroSevenFive > MAX_SEC_OF_CUMULATIVE_HOUR)
		{
			s_unitMinZeroSevenFive += INCREASE_CUMULATIVE_HOUR;
			s_unitSecZeroSevenFive = 0;
		}

		if(s_unitMinZeroSevenFive > MAX_MIN_OF_CUMULATIVE_HOUR)
		{
			s_unitHourZeroSevenFive += INCREASE_CUMULATIVE_HOUR;
			s_unitMinZeroSevenFive = 0;
		}

		if(s_unitHourZeroSevenFive > MAX_HOUR_OF_CUMULATIVE_HOUR)
		{
			s_unitHourZeroSevenFive = MAX_HOUR_OF_CUMULATIVE_HOUR;
			s_unitMinZeroSevenFive = MAX_MIN_OF_CUMULATIVE_HOUR;
			s_unitSecZeroSevenFive = MAX_SEC_OF_CUMULATIVE_HOUR;
		}
	}
	else if (guiInterface_GetSettingValue(eFlowRateSettingId) == SETTING_FR_050L)
	{
		s_unitSecZeroFive += INCREASE_CUMULATIVE_HOUR;
		if(s_unitSecZeroFive > MAX_SEC_OF_CUMULATIVE_HOUR)
		{
			s_unitMinZeroFive += INCREASE_CUMULATIVE_HOUR;
			s_unitSecZeroFive = 0;
		}

		if(s_unitMinZeroFive > MAX_MIN_OF_CUMULATIVE_HOUR)
		{
			s_unitHourZeroFive += INCREASE_CUMULATIVE_HOUR;
			s_unitMinZeroFive = 0;
		}

		if(s_unitHourZeroFive > MAX_HOUR_OF_CUMULATIVE_HOUR)
		{
			s_unitHourZeroFive = MAX_HOUR_OF_CUMULATIVE_HOUR;
			s_unitMinZeroFive = MAX_MIN_OF_CUMULATIVE_HOUR;
			s_unitSecZeroFive = MAX_SEC_OF_CUMULATIVE_HOUR;
		}
	}
	else if (guiInterface_GetSettingValue(eFlowRateSettingId) == SETTING_FR_025L)
	{
		s_unitSecZeroTwoFive += INCREASE_CUMULATIVE_HOUR;

		if(s_unitSecZeroTwoFive > MAX_SEC_OF_CUMULATIVE_HOUR)
		{
			s_unitMinZeroTwoFive += INCREASE_CUMULATIVE_HOUR;
			s_unitSecZeroTwoFive = 0;
		}

		if(s_unitMinZeroTwoFive > MAX_MIN_OF_CUMULATIVE_HOUR)
		{
			s_unitHourZeroTwoFive += INCREASE_CUMULATIVE_HOUR;
			s_unitMinZeroTwoFive = 0;
		}

		if(s_unitHourZeroTwoFive > MAX_HOUR_OF_CUMULATIVE_HOUR)
		{
			s_unitHourZeroTwoFive = MAX_HOUR_OF_CUMULATIVE_HOUR;
			s_unitMinZeroTwoFive = MAX_MIN_OF_CUMULATIVE_HOUR;
			s_unitSecZeroTwoFive = MAX_SEC_OF_CUMULATIVE_HOUR;
		}
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-operationScreen_HandlerTouchPass 								    		*/
/*                                                                                      */
/* Details  -Handle position when user touch 											*/
/*			  														                    */
/*                                                                                      */
/* Arguments : (I) int posx :  - position x									 			*/
/*                     - range : 0-800                                                  */
/*                     - unit : NONE                                                    */
/*                                                                                      */
/*				   int posy : - position y												*/
/*                     - range : 0-480                                                  */
/*                     - unit : NONE                                                    */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) g_mainWindow : main window							        */
/*                     (O) s_touchPos : touch position                                  */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void operationScreen_HandlerTouchPass(int posx, int posy)
{
	switch(s_touchPos)
	{
	case eTouchScrOne:
		if((posx >= 400) &&
				(posy < 240))
		{
			s_touchPos = eTouchScrTwo;
		}
		break;
	case eTouchScrTwo:
		if((posx > 400) &&
				(posy >= 240))
		{
			s_touchPos = eTouchScrThree;
		}
		break;
	case eTouchScrThree:
		if((posx < 400) &&
				(posy >= 240))
		{
			s_touchPos = eTouchScrFour;
		}
		break;
	case eTouchScrFour:
		if((posx < 400) &&
				(posy< 240))
		{
			s_touchPos = eTouchScrOne;
			uint8_t brightness = guiInterface_GetSettingValue(eBrightnessSettingId);
			if(brightness >= MIN_BRIGHTNESS_SETTING && brightness <= MAX_BRIGHTNESS_SETTING)
			{
				guiTask_SendPendingEvent(eBacklightChangeId, (uint16_t)brightness);
			}
			WM_SendMessageNoPara(g_mainWindow,WM_SHOW_PASSWORD_DIALOG);
		}
		break;
	default:
		break;
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-operationScreen_Callback 								    				*/
/*                                                                                      */
/* Details  -call back fuction for operation screen, handle event from the window 		*/
/*			  														                    */
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
/*                     (O) s_flowRateValue : display flow rate     					   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void operationScreen_Callback(WM_MESSAGE *pMsg)
{
	GUI_RECT Rect;

	switch(pMsg->MsgId)
	{
	case WM_PAINT:
		GUI_SetBkColor(PATIENT_BACKGROUND_COLOR);
		GUI_GetClientRect(&Rect);
		GUI_ClearRectEx(&Rect);
		break;
	case WM_TIMER:
	{
		//Sub switch
		switch(WM_GetTimerId(pMsg->Data.v))
		{
		case eTimerUpdateFRId:
			WM_Paint(s_flowRateValue);
			break;
		case eChildLockTimerId:
			operationScreen_ShowHideChildlock();
			break;
		default:
			break;
		}
		//End Sub switch
		break;
	}
	case WM_TOUCH:
	case WM_TOUCH_CHILD:
	{
		static GUI_PID_STATE StatePID;
		GUI_PID_GetState(&StatePID);

		if(StatePID.Pressed == 1)
		{
			operationScreen_HandlerTouchPass(StatePID.x,StatePID.y);
		}
		break;
	}
	default:
		WM_DefaultProc(pMsg);
		break;
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-operationScreen_FlowRateCallback 		 								    */
/*                                                                                      */
/* Details  -Call back function for flow rate setting								    */
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
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void operationScreen_FlowRateCallback(WM_MESSAGE *pMsg)
{
	GUI_RECT Rect;
	int8_t buffer[SIZE_BUFFER_FLOW_RATE] = {'\0'};
	double floatValue = 0.0;

	floatValue = (double)guiTask_GetFlowrate()/FLOWRATE_SCALE;
	sprintf((char*)buffer,"%.2f",floatValue);

	switch(pMsg->MsgId)
	{
	case WM_PAINT:
		GUI_SetBkColor(PATIENT_BACKGROUND_COLOR);
		GUI_GetClientRect(&Rect);
		GUI_ClearRectEx(&Rect);

		GUI_SetColor(GUI_WHITE);
		guiTask_SetFont(&GUI_FontEng3L150B2PP);
		GUI_DispStringAt((char*)buffer,Rect.x0,Rect.y0);
		break;
	default:
		TEXT_Callback(pMsg);
		break;
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-operationScreen_CallbackBottomLine 								        */
/*                                                                                      */
/* Details  -Call back function for bottom line, handle event from 						*/
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
void operationScreen_CallbackBottomLine(WM_MESSAGE *pMsg)
{
	GUI_RECT Rect;
	switch(pMsg->MsgId)
	{
	case WM_PAINT:
		GUI_SetBkColor(GUI_WHITE);
		GUI_GetClientRect(&Rect);
		GUI_ClearRectEx(&Rect);

		GUI_FillRect(Rect.x0,Rect.y0,Rect.x1,Rect.y1);
		break;
	default:
		TEXT_Callback(pMsg);
		break;
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-operationScreen_CallbackTopLine 								            */
/*                                                                                      */
/* Details  -Call back function for top line, handle event from 						*/
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
void operationScreen_CallbackTopLine(WM_MESSAGE *pMsg)
{
	GUI_RECT Rect;
	switch(pMsg->MsgId)
	{
	case WM_PAINT:
		GUI_SetBkColor(GUI_WHITE);
		GUI_GetClientRect(&Rect);
		GUI_ClearRectEx(&Rect);

		GUI_FillRect(Rect.x0,Rect.y0,Rect.x1,Rect.y1);
		break;
	default:
		TEXT_Callback(pMsg);
		break;
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-operationScreen_Init 								       					*/
/*                                                                                      */
/* Details  -Initialize operation screen with 											*/
/*			  flow rate setting															*/
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
/*                     (O) g_operationScreen : operation screen                         */
/*                     (O) s_bitmapAnypalconnect : anypal connect icon	             	*/
/*                     (O) s_bitmapAroundCharging : around charging icon                */
/*                     (O) s_bitmapCharging : charging icon						        */
/*                     (O) s_anypalConnect : anypal connect image                      	*/
/*                     (O) s_aroundCharging : around charging image		               	*/
/*                     (O) s_charging : charging image                                  */
/*                     (O) s_bottomLine : bottom line						            */
/*                     (O) s_topLine : top line                                         */
/*                     (O) s_anypalConnectTxt : anypal connect text			            */
/*                     (O) s_anypalChargingTxt : anypal charging text     			   	*/
/*                     (O) s_bitmapanimationOne : animation icon	         			*/
/*                     (O) s_bitmapanimationTwo : animation icon                	   	*/
/*                     (O) s_animationOne : animation image						        */
/*                     (O) s_animationTwo : animation image						        */
/*                     (O) s_animationThree : animation image						    */
/*                     (O) s_animationFour : animation image						    */
/*                     (O) s_animationFive : animation image						    */
/*                     (O) s_animationSix : animation image						        */
/*                     (O) s_animationSeven : animation image						    */
/*                     (O) s_flowRateValue : display flow rate 						   	*/
/*                     (O) s_bitmapChildLock : childlock icon						    */
/*                     (O) s_childlockIcon : childlock image                            */
/*                     (O) s_flowRateName : display flow rate name						*/
/*                     (O) s_flowRateUint : display flow rate unit                      */
/*                     (O) s_statusMachine : display status machine			            */
/*                     (O) s_bitmapattachO2Cylinder : O2 cylinder attach icon      		*/
/*                     (O) s_bitmapnoattachO2Cylinder : O2 cylinder no attach icon		*/
/*                     (O) s_O2Cylinder : O2 cylinder image                             */
/*                     (O) s_timerUpdateFR : timer update flow rate						*/
/*                     (O) s_timerChildLock : timer show/hide flow rate in childlock	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void operationScreen_Init(void)
{
	g_operationScreen = WM_CreateWindowAsChild(0, 0, 800, 480,g_mainWindow,WM_CF_SHOW | WM_CF_MEMDEV | WM_CF_LATE_CLIP,operationScreen_Callback,0);

	s_bitmapAnypalconnect = bmAnypalconnect;
	s_bitmapAroundCharging = bmAroundCharging;
	s_bitmapCharging = bmCharging;

	s_anypalConnect = IMAGE_CreateEx(20,10,61,62,g_operationScreen,WM_CF_HIDE,IMAGE_CF_TILE,eImageOperationSevenId);
	IMAGE_SetBitmap(s_anypalConnect,&s_bitmapAnypalconnect);

	s_aroundCharging = IMAGE_CreateEx(610,30,21,22,g_operationScreen,WM_CF_HIDE,IMAGE_CF_TILE,eImageOperationEightId);
	IMAGE_SetBitmap(s_aroundCharging,&s_bitmapAroundCharging);

	s_charging = IMAGE_CreateEx(751,22,29,40,g_operationScreen,WM_CF_HIDE,IMAGE_CF_TILE,eImageOperationNineId);
	IMAGE_SetBitmap(s_charging,&s_bitmapCharging);

	s_bottomLine = TEXT_CreateEx(20,350,760,3,g_operationScreen,WM_CF_SHOW,TEXT_CF_LEFT,eBottomlineOperationScrId,"");
	WM_SetCallback(s_bottomLine,operationScreen_CallbackBottomLine);

	s_topLine = TEXT_CreateEx(20,81,760,3,g_operationScreen,WM_CF_HIDE,TEXT_CF_LEFT,eToplineOperationScrId,"");
	WM_SetCallback(s_topLine,operationScreen_CallbackTopLine);

	s_anypalConnectTxt = TEXT_CreateEx(85,10,250,50,g_operationScreen,WM_CF_HIDE,TEXT_CF_LEFT,eAnypalConnectTxtOperationScrId,"");

	s_anypalChargingTxt = TEXT_CreateEx(641,30,100,50,g_operationScreen,WM_CF_HIDE,TEXT_CF_LEFT,eAnypalChargingTxtOperationScrId,"");

	//Animation
	s_bitmapanimationOne = bmAnimation1;
	s_bitmapanimationTwo = bmAnimation2;

	s_animationOne = IMAGE_CreateEx(20,393,13,72,g_operationScreen,WM_CF_HIDE,IMAGE_CF_TILE,eImageOperationZeroId);
	IMAGE_SetBitmap(s_animationOne,&s_bitmapanimationOne);

	s_animationTwo = IMAGE_CreateEx(40,393,13,72,g_operationScreen,WM_CF_HIDE,IMAGE_CF_TILE,eImageOperationOneId);
	IMAGE_SetBitmap(s_animationTwo,&s_bitmapanimationOne);

	s_animationThree = IMAGE_CreateEx(60,393,13,72,g_operationScreen,WM_CF_HIDE,IMAGE_CF_TILE,eImageOperationTwoId);
	IMAGE_SetBitmap(s_animationThree,&s_bitmapanimationOne);

	s_animationFour = IMAGE_CreateEx(80,393,13,72,g_operationScreen,WM_CF_HIDE,IMAGE_CF_TILE,eImageOperationThreeId);
	IMAGE_SetBitmap(s_animationFour,&s_bitmapanimationOne);

	s_animationFine = IMAGE_CreateEx(100,393,13,72,g_operationScreen,WM_CF_HIDE,IMAGE_CF_TILE,eImageOperationFourId);
	IMAGE_SetBitmap(s_animationFine,&s_bitmapanimationOne);

	s_animationSix = IMAGE_CreateEx(120,393,13,72,g_operationScreen,WM_CF_HIDE,IMAGE_CF_TILE,eImageOperationFiveId);
	IMAGE_SetBitmap(s_animationSix,&s_bitmapanimationOne);

	s_animationSeven = IMAGE_CreateEx(140,393,13,72,g_operationScreen,WM_CF_HIDE,IMAGE_CF_TILE,eImageOperationSixId);
	IMAGE_SetBitmap(s_animationSeven,&s_bitmapanimationOne);
	//End Animation

	s_flowRateValue = TEXT_CreateEx(150, 100, 480, 200,g_operationScreen,WM_CF_HIDE | WM_CF_MEMDEV,TEXT_CF_HCENTER,eOperationFlowRateSettingValueStringID,"");
	WM_SetCallback(s_flowRateValue,operationScreen_FlowRateCallback);

	s_bitmapChildLock = bmchildlock;
	s_childlockIcon = IMAGE_CreateEx(692,85,71,32,g_operationScreen,WM_CF_HIDE,IMAGE_CF_TILE,eChildlockIconId);
	IMAGE_SetBitmap(s_childlockIcon,&s_bitmapChildLock);

	s_flowRateName = TEXT_CreateEx(660, 130, 120, 200,g_operationScreen,WM_CF_HIDE,TEXT_CF_HCENTER,eOperationFlowRateSettingNameStringID,"");
	WM_SetCallback(s_flowRateName, operationScreen_CallbackFlowRateName);
	TEXT_SetText(s_flowRateName, "FR");

	s_flowRateUint = TEXT_CreateEx(660,230, 120, 60,g_operationScreen,WM_CF_HIDE,TEXT_CF_HCENTER,eOperationFlowRateSettingUnitStringID,"");
	guiTaskTEXT_SetFont(s_flowRateUint,&GUI_FontEng3L36B2PP);
	TEXT_SetTextColor(s_flowRateUint,BORDER_FR_NAME_COLOR);
	TEXT_SetText(s_flowRateUint, "LPM");

	s_statusMachine = TEXT_CreateEx(150,400,500,60,g_operationScreen,WM_CF_SHOW,TEXT_CF_HCENTER,eOperationStatusStringID,"");
	guiTaskTEXT_SetFont(s_statusMachine,&GUI_FontEng3L28B2PP);
	TEXT_SetTextColor(s_statusMachine,GUI_WHITE);
	TEXT_SetTextAlign(s_statusMachine,GUI_TA_CENTER | GUI_TA_VCENTER);

	s_bitmapattachO2Cylinder = bmattachO2cylinder;
	s_bitmapnoattachO2Cylinder = bmnotattachO2cylinder;

	s_O2Cylinder = IMAGE_CreateEx(600,360,174,101,g_operationScreen,WM_CF_HIDE,IMAGE_CF_TILE,eO2CylinderIconId);

	s_timerUpdateFR = WM_CreateTimer(g_operationScreen,eTimerUpdateFRId,TIMER_50_MS,0);
	s_timerChildLock = WM_CreateTimer(g_operationScreen, eChildLockTimerId, TIMER_500_MS, 0);

	WM_HideWindow(g_operationScreen);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-operationScreen_Show 								       			        */
/*                                                                                      */
/* Details  -Show operation screen with 											    */
/*			  flow rate setting															*/
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
/*                     (O) s_touchPos : touch position                                  */
/*                     (O) s_counter5s : counter 5 seconds use in childlock mode		*/
/*                     (O) g_operationScreen : operation screen						    */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void operationScreen_Show(void)
{
	s_touchPos = eTouchScrOne;
	s_counter5s = 0;
	standbyScreen_Hide();
	WM_ShowWindow(g_operationScreen);
	WM_SetFocus(g_operationScreen);

	operationScreen_ShowAnypal();
	operationScreen_ShowAnypalCharging();

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-operationScreen_Hide								       			        */
/*                                                                                      */
/* Details  -Hide operation screen with 											    */
/*			  flow rate setting															*/
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
/*                     (O) s_counter5s : counter 5 seconds use in childlock mode        */
/*                     (O) g_operationScreen : operation screen					      	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void operationScreen_Hide(void)
{
	s_counter5s = 0;
	WM_HideWindow(g_operationScreen);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-operationScreen_EnterStartOperation 								       	*/
/*                                                                                      */
/* Details  -Enter start Operation screen												*/
/*			  														                    */
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
/*                     (O) s_childlockIcon : childlock image                            */
/*                     (O) s_O2Cylinder : O2 cylinder image						        */
/*                     (O) s_animationOne : animation image                             */
/*                     (O) s_animationTwo : animation image						        */
/*                     (O) s_animationThree : animation image                           */
/*                     (O) s_animationFour : animation image						    */
/*                     (O) s_animationFive : animation image                            */
/*                     (O) s_animationSix : animation image						        */
/*                     (O) s_animationSeven : animation image                           */
/*                     (O) s_flowRateValue : display flow rate						   	*/
/*                     (O) s_flowRateUint : display flow rate name     				   	*/
/*                     (O) s_flowRateName : display flow rate unit			         	*/
/*                     (O) s_statusMachine : display status machine			         	*/
/*【Note】                                                                              													*/
/*                                                                                      */
/****************************************************************************************/
void operationScreen_EnterStartOperation(void)
{
	guiTask_SetFlagEnableChangeFR(false);
	guiTask_SetFlagOnOff(false);
	guiTask_EnableIrda(false);

	WM_HideWindow(s_childlockIcon);
	WM_HideWindow(s_O2Cylinder);
	WM_HideWindow(s_animationOne);
	WM_HideWindow(s_animationTwo);
	WM_HideWindow(s_animationThree);
	WM_HideWindow(s_animationFour);
	WM_HideWindow(s_animationFine);
	WM_HideWindow(s_animationSix);
	WM_HideWindow(s_animationSeven);

	passwordDialog_SetBackgroundColor(PATIENT_BACKGROUND_COLOR);
	standbyScreen_SetStateMachine(eStartSequence);

	uint8_t brightness = guiInterface_GetSettingValue(eBrightnessSettingId);
	if(brightness >= MIN_BRIGHTNESS_SETTING && brightness <= MAX_BRIGHTNESS_SETTING)
	{
		guiTask_SendPendingEvent(eBacklightChangeId, (uint16_t)brightness);
	}

	WM_ShowWindow(s_flowRateValue);
	WM_ShowWindow(s_flowRateUint);
	WM_ShowWindow(s_flowRateName);

	WM_MoveChildTo(s_statusMachine,150,400);
	WM_SetSize(s_statusMachine, 500,60);

	if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
	{
		guiTaskTEXT_SetFont(s_statusMachine, &GUI_FontEng3L28B2PP);
		TEXT_SetText(s_statusMachine, "Operation is started");

		guiTaskTEXT_SetFont(s_flowRateUint,&GUI_FontEng3L36B2PP);
		TEXT_SetText(s_flowRateUint, "LPM");
	}
	else
	{
		guiTaskTEXT_SetFont(s_statusMachine,&GUI_FontJapan3L38R2PP);
		TEXT_SetText(s_statusMachine, "運 転 を 開 始  し  ま  す");

		guiTaskTEXT_SetFont(s_flowRateUint,&GUI_FontJapan3L36B2PP);
		TEXT_SetText(s_flowRateUint, "L/分");

	}

	operationScreen_Show();
	guihandler_StartTimerAnimation();
	guihandler_StartTimerEnableIrda();

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-operationScreen_EnterOperationMode 								       	*/
/*                                                                                      */
/* Details  -Enter Operation screen 													*/
/*			  														                    */
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
/*                     (O) s_flowRateValue : display flow rate                          */
/*                     (O) s_animationOne : animation image						        */
/*                     (O) s_animationTwo : animation image						        */
/*                     (O) s_animationThree : animation image						    */
/*                     (O) s_animationFour : animation image						    */
/*                     (O) s_animationFive : animation image						    */
/*                     (O) s_animationSix : animation image						        */
/*                     (O) s_animationSeven : animation image						    */
/*                     (O) s_childlockIcon : childlock image						    */
/*                     (O) s_statusMachine : display status machine						*/
/*                     (O) s_flowRateUint : display flow rate unit						*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void operationScreen_EnterOperationMode(void)
{
	guiTask_SetFlagEnableChangeFR(true);
	guiTask_SetFlagOnOff(true);

	standbyScreen_SetStateMachine(eOperating);
	passwordDialog_SetBackgroundColor(PATIENT_BACKGROUND_COLOR);

	//When flow rate change in alarm screen(child lock ON), alarm inactive
	//set flow rate on RAM = setting flow rate on EEPROM
	if(guiInterface_GetSettingValue(eChildLockSettingId) == eOn)
	{
		guiTask_SetFowRate(guiInterface_GetSettingValue(eFlowRateSettingId));
	}

	WM_ShowWindow(s_flowRateValue);
	WM_ShowWindow(s_animationOne);
	WM_ShowWindow(s_animationTwo);
	WM_ShowWindow(s_animationThree);
	WM_ShowWindow(s_animationFour);
	WM_ShowWindow(s_animationFine);
	WM_ShowWindow(s_animationSix);
	WM_ShowWindow(s_animationSeven);

	if(guiInterface_GetSettingValue(eChildLockSettingId) == eOn)
	{
		WM_ShowWindow(s_childlockIcon);
	}
	else
	{
		WM_HideWindow(s_childlockIcon);
	}

	operationScreen_CheckAndShowO2Cylinder();

	WM_MoveChildTo(s_statusMachine,200,400);
	WM_SetSize(s_statusMachine, 400,60);

	if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
	{
		guiTaskTEXT_SetFont(s_statusMachine, &GUI_FontEng3L28B2PP);
		TEXT_SetText(s_statusMachine, "In operation...");

		guiTaskTEXT_SetFont(s_flowRateUint,&GUI_FontEng3L36B2PP);
		TEXT_SetText(s_flowRateUint, "LPM");
	}
	else
	{
		guiTaskTEXT_SetFont(s_statusMachine,&GUI_FontJapan3L38R2PP);
		TEXT_SetText(s_statusMachine, "運 転 中 で す. . .");

		guiTaskTEXT_SetFont(s_flowRateUint,&GUI_FontJapan3L36B2PP);
		TEXT_SetText(s_flowRateUint, "L/分");

	}

	operationScreen_Show();

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-operationScreen_ChangeFlowRate 								       		*/
/*                                                                                      */
/* Details  -Change flow rate by encoder							   					*/
/*			  														                    */
/*                                                                                      */
/* Arguments : (I) uint8_t FRPSAValue :  : - flow rate change					 		*/
/*						- range : 5-60													*/
/*                      - unit : LPM                                                    */
/*																						*/
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_flowRateValue : display flow rate                          */
/*                     (O) s_counter5s : counter 5 seconds use in childlock mode		*/
/*                     (O) s_timerChildLock : timer show/hide flow rate					*/
/*                     (O) s_timerUpdateFR : timer update flow rate			         	*/
/*                     (O) s_changedFR : flag change flow rate				         	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void operationScreen_ChangeFlowRate(uint8_t FRPSAValue)
{
	WM_ShowWindow(s_flowRateValue);
	WM_Paint(s_flowRateValue);

	s_changedFR = true;

	//Child lock
	if(guiInterface_GetSettingValue(eChildLockSettingId) == eOn)
	{
		s_counter5s = 0;
		WM_RestartTimer(s_timerChildLock,TIMER_500_MS);
		guiTask_SetBlinkingFR(true);
	}
	else
	{
		/** >>#0018
			Remove play flow rate audio immediately when change flow rate. Play flow rate audio after 1 seconds.
		#0018 >>**/
		guihandler_StartTimerSendFRToDevice();
	}

	WM_RestartTimer(s_timerUpdateFR,TIMER_50_MS);

	//Display flow rate in password dialog
	passwordDialog_DisplayFlowRate(FRPSAValue);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-operationScreen_ConfirmChildLock 								       		*/
/*                                                                                      */
/* Details  -Confirm flow rate when child lock on							   			*/
/*			  														                    */
/*                                                                                      */
/* Arguments : (I) uint8_t FRPSAValue :  - flow rate change					 		    */
/*						- range : 5-60													*/
/*                      - unit : LPM                                                    */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) g_operationScreen : operation screen							*/
/*                     (O) s_flowRateValue : display flow rate         				   	*/
/*                     (O) s_timerUpdateFR : timer update flow rate				     	*/
/*                     (O) s_changedFR : flag change flow rate				         	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void operationScreen_ConfirmChildLock(uint8_t FRPSAValue)
{
	s_changedFR = false;
	guiTask_SetBlinkingFR(false);

	if(guiInterface_GetSettingValue(eChildLockSettingId) == eOn)
	{
		if(WM_IsVisible(g_operationScreen) == true)
		{
			WM_ShowWindow(s_flowRateValue);

			uint8_t flowRate = guiTask_GetFlowrate();
			uint8_t penddingFlowRate = guiTask_GetPenddingFlowRate();
			uint8_t flowRateSetting = 0;
			if(flowRate != penddingFlowRate)
			{
				s_counter5s = 0;
				WM_RestartTimer(s_timerUpdateFR,TIMER_50_MS);
				guiInterface_SetSettingValue(eFlowRateSettingId,guiTask_GetFlowrate());
				setting_SaveAllSettingValue();

				flowRateSetting = guiInterface_GetSettingValue(eFlowRateSettingId);

				if(flowRateSetting >= SETTING_FR_025L && flowRateSetting <= SETTING_FR_300L)
				{
					guiTask_SendPSAParaEvent(ePacketPSAParaChangeId,flowRateSetting,
							guiTask_GetConditionO2(),guiTask_GetConditionTemp());
				}
				logInterface_WriteMacineLog(guiInterface_GetSettingValue(eFlowRateSettingId), eChangingFlowRateLogId);
			}
		}
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-operationScreen_restartUpdateCumulativeHour	 							*/
/*                                                                                      */
/* Details  -Restart timer update cumulative operation hour							   	*/
/*			  														                    */
/*                                                                                      */
/* Arguments : (I) uint8_t FRPSAValue :  - flow rate change					 		    */
/*						- range : 5-60													*/
/*                      - unit : LPM                                                    */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_patientSec : seconds of patient cumulative hour     		*/
/*                     (O) s_patientMin : minutes of patient cumulative hour            */
/*                     (O) s_patientHour : hour of patient cumulative hour              */
/*                     (O) s_equipSec : seconds of equipment cumulative hour            */
/*                     (O) s_equipMin : minutes of equipment cumulative hour            */
/*                     (O) s_equipHour : hour of equipment cumulative hour              */
/*                     (O) s_equipHourStart : hour of equipment cumulative hour when start operation*/
/*                     (O) s_unitSec : seconds of unit cumulative hour     			   	*/
/*                     (O) s_unitMin : minutes of unit cumulative hour	            	*/
/*                     (O) s_unitHour : hour of unit cumulative hour     			   	*/
/*                     (O) s_unitSecThree : seconds of unit cumulative hour 3L		    */
/*                     (O) s_unitMinThree : minutes of unit cumulative hour 3L         	*/
/*                     (O) s_unitHourThree : hour of unit cumulative hour 3L        	*/
/*                     (O) s_unitSecTwoFive : seconds of unit cumulative hour 2.5L		*/
/*                     (O) s_unitMinTwoFive : minutes of unit cumulative hour 2.5L      */
/*                     (O) s_unitHourTwoFive : hour of unit cumulative hour 2.5L        */
/*                     (O) s_unitSecTwo : seconds of unit cumulative hour 2L			*/
/*                     (O) s_unitMinTwo : minutes of unit cumulative hour 2L      		*/
/*                     (O) s_unitHourTwo : hour of unit cumulative hour 2L        		*/
/*                     (O) s_unitSecOneSevenFive : seconds of unit cumulative hour 1.75L*/
/*                     (O) s_unitMinOneSevenFive : minutes of unit cumulative hour 1.75L*/
/*                     (O) s_unitHourOneSevenFive : hour of unit cumulative hour 1.75L  */
/*                     (O) s_unitSecOneFive : seconds of unit cumulative hour 1.5L		*/
/*                     (O) s_unitMinOneFive : minutes of unit cumulative hour 1.5L		*/
/*                     (O) s_unitHourOneFive : hour of unit cumulative hour 1.5L        */
/*                     (O) s_unitSecOneTwoFive : seconds of unit cumulative hour 1.25L	*/
/*                     (O) s_unitMinOneTwoFive : minutes of unit cumulative hour 1.25L	*/
/*                     (O) s_unitHourOneTwoFive : hour of unit cumulative hour 1.25L    */
/*                     (O) s_unitSecOne : seconds of unit cumulative hour 1L			*/
/*                     (O) s_unitMinOne : minutes of unit cumulative hour 1L			*/
/*                     (O) s_unitHourOne : hour of unit cumulative hour 1L    			*/
/*                     (O) s_unitSecZeroSevenFive : seconds of unit cumulative hour 0.75L*/
/*                     (O) s_unitMinZeroSevenFive : minutes of unit cumulative hour 0.75L*/
/*                     (O) s_unitHourZeroSevenFive : hour of unit cumulative hour 0.75L */
/*                     (O) s_unitSecZeroFive : seconds of unit cumulative hour 0.5L		*/
/*                     (O) s_unitMinZeroFive : minutes of unit cumulative hour 0.5L		*/
/*                     (O) s_unitHourZeroFive : hour of unit cumulative hour 0.5L  		*/
/*                     (O) s_unitSecZeroTwoFive : seconds of unit cumulative hour 0.25L */
/*                     (O) s_unitMinZeroTwoFive : minutes of unit cumulative hour 0.25L */
/*                     (O) s_unitHourZeroTwoFive : hour of unit cumulative hour 0.25L   */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void operationScreen_restartUpdateCumulativeHour(void)
{
	CumulativeHourTime patientTime;
	CumulativeHourTime unitTime;
	CumulativeHourTime unitTime_300;
	CumulativeHourTime unitTime_250;
	CumulativeHourTime unitTime_200;
	CumulativeHourTime unitTime_175;
	CumulativeHourTime unitTime_150;
	CumulativeHourTime unitTime_125;
	CumulativeHourTime unitTime_100;
	CumulativeHourTime unitTime_075;
	CumulativeHourTime unitTime_050;
	CumulativeHourTime unitTime_025;
	CumulativeHourTime equipTime;

	patientTime = guiInterface_GetCumulativeHour(patients);
	s_patientHour = patientTime.hour;
	s_patientMin = patientTime.min;
	s_patientSec = patientTime.sec;

	unitTime = guiInterface_GetCumulativeHour(uints);
	s_unitHour = unitTime.hour;
	s_unitMin = unitTime.min;
	s_unitSec = unitTime.sec;

	unitTime_300 = guiInterface_GetCumulativeHour(uints_300);
	s_unitHourThree = unitTime_300.hour;
	s_unitMinThree = unitTime_300.min;
	s_unitSecThree = unitTime_300.sec;

	unitTime_250 = guiInterface_GetCumulativeHour(units_250);
	s_unitHourTwoFive = unitTime_250.hour;
	s_unitMinTwoFive = unitTime_250.min;
	s_unitSecTwoFive = unitTime_250.sec;

	unitTime_200 = guiInterface_GetCumulativeHour(uints_200);
	s_unitHourTwo = unitTime_200.hour;
	s_unitMinTwo = unitTime_200.min;
	s_unitSecTwo = unitTime_200.sec;

	unitTime_175 = guiInterface_GetCumulativeHour(uints_175);
	s_unitHourOneSevenFive = unitTime_175.hour;
	s_unitMinOneSevenFive = unitTime_175.min;
	s_unitSecOneSevenFive = unitTime_175.sec;

	unitTime_150 = guiInterface_GetCumulativeHour(uints_150);
	s_unitHourOneFive = unitTime_150.hour;
	s_unitMinOneFive = unitTime_150.min;
	s_unitSecOneFive = unitTime_150.sec;

	unitTime_125 = guiInterface_GetCumulativeHour(uints_125);
	s_unitHourOneTwoFive = unitTime_125.hour;
	s_unitMinOneTwoFive = unitTime_125.min;
	s_unitSecOneTwoFive = unitTime_125.sec;

	unitTime_100 = guiInterface_GetCumulativeHour(uints_100);
	s_unitHourOne = unitTime_100.hour;
	s_unitMinOne = unitTime_100.min;
	s_unitSecOne = unitTime_100.sec;

	unitTime_075 = guiInterface_GetCumulativeHour(uints_075);
	s_unitHourZeroSevenFive = unitTime_075.hour;
	s_unitMinZeroSevenFive = unitTime_075.min;
	s_unitSecZeroSevenFive = unitTime_075.sec;

	unitTime_050 = guiInterface_GetCumulativeHour(uints_050);
	s_unitHourZeroFive = unitTime_050.hour;
	s_unitMinZeroFive = unitTime_050.min;
	s_unitSecZeroFive = unitTime_050.sec;

	unitTime_025 = guiInterface_GetCumulativeHour(uints_025);
	s_unitHourZeroTwoFive = unitTime_025.hour;
	s_unitMinZeroTwoFive = unitTime_025.min;
	s_unitSecZeroTwoFive = unitTime_025.sec;

	equipTime = guiInterface_GetCumulativeHour(equipments);
	s_equipHour = equipTime.hour;
	s_equipMin = equipTime.min;
	s_equipSec = equipTime.sec;
	s_equipHourStart = equipTime.hour;

	guihandler_StartTimerUpdateCumulative();

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-operationScreen_ShowAnypal	 								       			*/
/*                                                                                      */
/* Details  -Show anypal when connect anypal										   	*/
/*			  														                    */
/*                                                                                      */
/* Arguments : (I) NONE								 		    						*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_anypalConnectTxt : anypal connect text                     */
/*                     (O) s_topLine : top line						                    */
/*                     (O) s_anypalConnect : anypal connect image						*/
/*                     (O) s_anypalChargingTxt : anypal charging text					*/
/*                     (O) s_charging : charging image						            */
/*                     (O) s_aroundCharging : around charging image						*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void operationScreen_ShowAnypal(void)
{
	if(anypalDisplayHandler_GetAnypalConnectStatus() == true)
	{
		uint8_t brightness = guiInterface_GetSettingValue(eBrightnessSettingId);
		if(brightness >= MIN_BRIGHTNESS_SETTING && brightness <= MAX_BRIGHTNESS_SETTING)
		{
			guiTask_SendPendingEvent(eBacklightChangeId, (uint16_t)brightness);
		}

		TEXT_SetTextColor(s_anypalConnectTxt,GUI_WHITE);
		if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
		{
			WM_MoveChildTo(s_anypalConnectTxt,85,30);
			guiTaskTEXT_SetFont(s_anypalConnectTxt,&GUI_FontEng3L16B2PP);
			TEXT_SetText(s_anypalConnectTxt, "Anypal respond OK");
		}
		else
		{
			WM_MoveChildTo(s_anypalConnectTxt,90,25);
			guiTaskTEXT_SetFont(s_anypalConnectTxt,&GUI_FontJapan3L25R2PP);
			TEXT_SetText(s_anypalConnectTxt,"エニィパル接続");
		}

		WM_ShowWindow(s_anypalConnectTxt);
		WM_ShowWindow(s_topLine);
		WM_ShowWindow(s_anypalConnect);
		WM_ShowWindow(s_anypalConnectTxt);
	}
	else
	{
		WM_HideWindow(s_topLine);
		WM_HideWindow(s_anypalConnectTxt);
		WM_HideWindow(s_anypalChargingTxt);
		WM_HideWindow(s_anypalConnect);
		WM_HideWindow(s_charging);
		WM_HideWindow(s_aroundCharging);
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-operationScreen_ShowAnypalCharging	 								       	*/
/*                                                                                      */
/* Details  -Show anypal charging when connect anypal									*/
/*			  														                    */
/*                                                                                      */
/* Arguments : (I) NONE														 		    */
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_anypalChargingTxt : anypal charging text           		*/
/*                     (O) s_aroundCharging : around charging image				       	*/
/*                     (O) s_charging : charging image                                  */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void operationScreen_ShowAnypalCharging(void)
{
	if(anypalDisplayHandler_GetChargingStatus() == eAnyPalCharging)
	{
		TEXT_SetTextColor(s_anypalChargingTxt,GUI_WHITE);

		if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
		{
			WM_MoveChildTo(s_aroundCharging,610,30);

			WM_MoveChildTo(s_anypalChargingTxt,638,30);
			WM_SetSize(s_anypalChargingTxt,120,50);
			guiTaskTEXT_SetFont(s_anypalChargingTxt,&GUI_FontEng3L16B2PP);
			TEXT_SetText(s_anypalChargingTxt, "Charging");
		}
		else
		{
			WM_MoveChildTo(s_aroundCharging,624,32);

			WM_MoveChildTo(s_anypalChargingTxt,655,30);
			WM_SetSize(s_anypalChargingTxt,100,50);
			guiTaskTEXT_SetFont(s_anypalChargingTxt,&GUI_FontJapan3L25R2PP);
			TEXT_SetText(s_anypalChargingTxt, "充電中");
		}
		WM_ShowWindow(s_charging);
		WM_ShowWindow(s_aroundCharging);
		WM_ShowWindow(s_anypalChargingTxt);
	}
	else if (anypalDisplayHandler_GetChargingStatus() == eAnyPalChargedFull)
	{
		WM_HideWindow(s_charging);
		WM_HideWindow(s_aroundCharging);
		WM_HideWindow(s_anypalChargingTxt);
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-operationScreen_CheckAndShowO2Cylinder		 								*/
/*                                                                                      */
/* Details  -Show O2 cylinder when o2 cylinder is attach or not attach					*/
/*			  														                    */
/*                                                                                      */
/* Arguments : (I) NONE																	*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_O2Cylinder : O2 cylinder image                             */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void operationScreen_CheckAndShowO2Cylinder(void)
{
	if(guiTask_GetO2CylinStatus() == O2_CYLIN_ATTACH)
	{
		if(standbyScreen_GetStateMachine() == eOperating)
		{
			guiTask_IMAGESetBitmap(s_O2Cylinder,&s_bitmapattachO2Cylinder);
			WM_ShowWindow(s_O2Cylinder);
		}
	}
	else if (guiTask_GetO2CylinStatus() == O2_CYLIN_NOTATTACH ||
			guiTask_GetO2CylinStatus() == O2_CYLIN_REMOVE)
	{
		if(guiInterface_GetSettingValue(eO2CylinderSettingId) == eOn)
		{
			if(standbyScreen_GetStateMachine() == eOperating)
			{
				guiTask_IMAGESetBitmap(s_O2Cylinder,&s_bitmapnoattachO2Cylinder);
				WM_ShowWindow(s_O2Cylinder);
			}
		}
		else
		{
			WM_HideWindow(s_O2Cylinder);
		}
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-operationScreen_ClearPatientCumulative 		 							*/
/*                                                                                      */
/* Details  -Clear patient cumualtive operation hour									*/
/*			  														                    */
/*                                                                                      */
/* Arguments : (I) NONE																	*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_patientHour : hour of patient cumulative hour         		*/
/*                     (O) s_patientMin	: minutes of patient cumulative hour			*/
/*                     (O) s_patientSec	: seconds of patient cumulative hour			*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void operationScreen_ClearPatientCumulative(void)
{
	guiInterface_SetCumulativeHour(patients,0,0,0);

	s_patientHour = 0;
	s_patientMin = 0;
	s_patientSec = 0;

	setting_SaveCumulativeHours();

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-operationScreen_ClearUnitCumulativeThree 		 							*/
/*                                                                                      */
/* Details  -Clear unit cumualtive operation hour with flow rate 3L					    */
/*			  														                    */
/*                                                                                      */
/* Arguments : (I) int hours : - hour change											*/
/*					 - range : 0-99999													*/
/*                   - unit : NONE                                                      */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_unitHourThree : hour of unit cumulative hour 3L        	*/
/*                     (O) s_unitMinThree	: minutes of unit cumulative hour 3L		*/
/*                     (O) s_unitSecThree	: seconds of unit cumulative hour 3L		*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void operationScreen_ClearUnitCumulativeThree(int hours)
{
	guiInterface_SetCumulativeHour(uints_300,hours,0,0);

	s_unitHourThree = hours;
	s_unitMinThree = 0;
	s_unitSecThree = 0;

	setting_SaveCumulativeHours();

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-operationScreen_ClearUnitCumulativeTwoFive 		 						*/
/*                                                                                      */
/* Details  -Clear unit cumualtive operation hour with flow rate 2.5L					*/
/*			  														                    */
/*                                                                                      */
/* Arguments : (I) int hours : - hour change											*/
/*					 - range : 0-99999													*/
/*                   - unit : NONE                                                      */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_unitHourTwoFive : hour of unit cumulative hour 2.5L        */
/*                     (O) s_unitMinTwoFive	: minutes of unit cumulative hour 2.5L		*/
/*                     (O) s_unitSecTwoFive	: seconds of unit cumulative hour 2.5L		*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void operationScreen_ClearUnitCumulativeTwoFive(int hours)
{
	guiInterface_SetCumulativeHour(units_250,hours,0,0);

	s_unitHourTwoFive = hours;
	s_unitMinTwoFive = 0;
	s_unitSecTwoFive = 0;

	setting_SaveCumulativeHours();

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-operationScreen_ClearUnitCumulativeTwo 		 							*/
/*                                                                                      */
/* Details  -Clear unit cumualtive operation hour with flow rate 2L						*/
/*			  														                    */
/*                                                                                      */
/* Arguments : (I) int hours : - hour change											*/
/*					 - range : 0-99999													*/
/*                   - unit : NONE                                                      */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_unitHourTwo : hour of unit cumulative hour 2L 		       	*/
/*                     (O) s_unitMinTwo	: minutes of unit cumulative hour 2L			*/
/*                     (O) s_unitSecTwo	: seconds of unit cumulative hour 2L			*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void operationScreen_ClearUnitCumulativeTwo(int hours)
{
	guiInterface_SetCumulativeHour(uints_200,hours,0,0);

	s_unitHourTwo = hours;
	s_unitMinTwo = 0;
	s_unitSecTwo = 0;

	setting_SaveCumulativeHours();

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-operationScreen_ClearUnitCumulativeOneSevenFive 		 					*/
/*                                                                                      */
/* Details  -Clear unit cumualtive operation hour with flow rate 1.75L					*/
/*			  														                    */
/*                                                                                      */
/* Arguments : (I) int hours : - hour change											*/
/*					 - range : 0-99999													*/
/*                   - unit : NONE                                                      */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_unitHourOneSevenFive : hour of unit cumulative hour 1.75L 	*/
/*                     (O) s_unitMinOneSevenFive : minutes of unit cumulative hour 1.75L*/
/*                     (O) s_unitSecOneSevenFive : seconds of unit cumulative hour 1.75L*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void operationScreen_ClearUnitCumulativeOneSevenFive(int hours)
{
	guiInterface_SetCumulativeHour(uints_175,hours,0,0);

	s_unitHourOneSevenFive = hours;
	s_unitMinOneSevenFive = 0;
	s_unitSecOneSevenFive = 0;

	setting_SaveCumulativeHours();

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-operationScreen_ClearUnitCumulativeOneFive 		 						*/
/*                                                                                      */
/* Details  -Clear unit cumualtive operation hour with flow rate 1.5L					*/
/*			  														                    */
/*                                                                                      */
/* Arguments : (I) int hours : - hour change											*/
/*					 - range : 0-99999													*/
/*                   - unit : NONE                                                      */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_unitHourOneFive : hour of unit cumulative hour 1.5L 		*/
/*                     (O) s_unitMinOneFive : minutes of unit cumulative hour 1.5L 		*/
/*                     (O) s_unitSecOneFive : seconds of unit cumulative hour 1.5L 		*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void operationScreen_ClearUnitCumulativeOneFive(int hours)
{
	guiInterface_SetCumulativeHour(uints_150,hours,0,0);

	s_unitHourOneFive = hours;
	s_unitMinOneFive = 0;
	s_unitSecOneFive = 0;

	setting_SaveCumulativeHours();

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-operationScreen_ClearUnitCumulativeOneTwoFive 		 						*/
/*                                                                                      */
/* Details  -Clear unit cumualtive operation hour with flow rate 1.25L					*/
/*			  														                    */
/*                                                                                      */
/* Arguments : (I) int hours : - hour change											*/
/*					 - range : 0-99999													*/
/*                   - unit : NONE                                                      */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_unitHourOneTwoFive : hour of unit cumulative hour 1.25L 	*/
/*                     (O) s_unitMinOneTwoFive : minutes of unit cumulative hour 1.25L 	*/
/*                     (O) s_unitSecOneTwoFive : seconds of unit cumulative hour 1.25L 	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void operationScreen_ClearUnitCumulativeOneTwoFive(int hours)
{
	guiInterface_SetCumulativeHour(uints_125,hours,0,0);

	s_unitHourOneTwoFive = hours;
	s_unitMinOneTwoFive = 0;
	s_unitSecOneTwoFive = 0;

	setting_SaveCumulativeHours();

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-operationScreen_ClearUnitCumulativeOne 		 							*/
/*                                                                                      */
/* Details  -Clear unit cumualtive operation hour with flow rate 1L						*/
/*			  														                    */
/*                                                                                      */
/* Arguments : (I) int hours : - hour change											*/
/*					 - range : 0-99999													*/
/*                   - unit : NONE                                                      */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_unitHourOne : hour of unit cumulative hour 1L 				*/
/*                     (O) s_unitMinOne : minutes of unit cumulative hour 1L 			*/
/*                     (O) s_unitSecOne : seconds of unit cumulative hour 1L 			*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void operationScreen_ClearUnitCumulativeOne(int hours)
{
	guiInterface_SetCumulativeHour(uints_100,hours,0,0);

	s_unitHourOne = hours;
	s_unitMinOne = 0;
	s_unitSecOne = 0;

	setting_SaveCumulativeHours();

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-operationScreen_ClearUnitCumulativeZeroSevenFive 		 					*/
/*                                                                                      */
/* Details  -Clear unit cumualtive operation hour with flow rate 0.75L					*/
/*			  														                    */
/*                                                                                      */
/* Arguments : (I) int hours : - hour change											*/
/*					 - range : 0-99999													*/
/*                   - unit : NONE                                                      */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_unitHourZeroSevenFive : hour of unit cumulative hour 0.75L */
/*                     (O) s_unitMinZeroSevenFive : minutes of unit cumulative hour 0.75L*/
/*                     (O) s_unitSecZeroSevenFive : seconds of unit cumulative hour 0.75L*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void operationScreen_ClearUnitCumulativeZeroSevenFive(int hours)
{
	guiInterface_SetCumulativeHour(uints_075,hours,0,0);

	s_unitHourZeroSevenFive = hours;
	s_unitMinZeroSevenFive = 0;
	s_unitSecZeroSevenFive = 0;

	setting_SaveCumulativeHours();

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-operationScreen_ClearUnitCumulativeZeroFive 		 						*/
/*                                                                                      */
/* Details  -Clear unit cumualtive operation hour with flow rate 0.5L					*/
/*			  														                    */
/*                                                                                      */
/* Arguments : (I) int hours : - hour change											*/
/*					 - range : 0-99999													*/
/*                   - unit : NONE                                                      */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_unitHourZeroFive : hour of unit cumulative hour 0.5L 		*/
/*                     (O) s_unitMinZeroFive : minutes of unit cumulative hour 0.5L		*/
/*                     (O) s_unitSecZeroFive : seconds of unit cumulative hour 0.5L		*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void operationScreen_ClearUnitCumulativeZeroFive(int hours)
{
	guiInterface_SetCumulativeHour(uints_050,hours,0,0);

	s_unitHourZeroFive = hours;
	s_unitMinZeroFive = 0;
	s_unitSecZeroFive = 0;

	setting_SaveCumulativeHours();

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-operationScreen_ClearUnitCumulativeZeroTwoFive 		 					*/
/*                                                                                      */
/* Details  -Clear unit cumualtive operation hour with flow rate 0.25L					*/
/*			  														                    */
/*                                                                                      */
/* Arguments : (I) int hours : - hour change											*/
/*					 - range : 0-99999													*/
/*                   - unit : NONE                                                      */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_unitHourZeroTwoFive : hour of unit cumulative hour 0.25L 	*/
/*                     (O) s_unitMinZeroTwoFive : minutes of unit cumulative hour 0.25L	*/
/*                     (O) s_unitSecZeroTwoFive : seconds of unit cumulative hour 0.25L	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void operationScreen_ClearUnitCumulativeZeroTwoFive(int hours)
{
	guiInterface_SetCumulativeHour(uints_025,hours,0,0);

	s_unitHourZeroTwoFive = hours;
	s_unitMinZeroTwoFive = 0;
	s_unitSecZeroTwoFive = 0;

	setting_SaveCumulativeHours();

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-operationScreen_ClearUnitCumulative 		 								*/
/*                                                                                      */
/* Details  -Clear unit cumualtive operation hour										*/
/*			  														                    */
/*                                                                                      */
/* Arguments : (I) int hours : - hour change											*/
/*					 - range : 0-99999													*/
/*                   - unit : NONE                                                      */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_unitHour : hour of unit cumulative hour  					*/
/*                     (O) s_unitMin : minutes of unit cumulative hour 					*/
/*                     (O) s_unitSec : seconds of unit cumulative hour 					*/
/*                     (O) s_equipHour : hour of equipment cumulative hour  			*/
/*                     (O) s_equipMin : minutes of equipment cumulative hour 			*/
/*                     (O) s_equipSec : seconds of equipment cumulative hour 			*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void operationScreen_ClearUnitCumulative(int hours)
{
	guiInterface_SetCumulativeHour(uints,hours,0,0);

	s_unitHour = hours;
	s_unitMin = 0;
	s_unitSec = 0;

	//Equipment cumaulative
	guiInterface_SetCumulativeHour(equipments,hours,0,0);

	s_equipHour = hours;
	s_equipMin = 0;
	s_equipSec = 0;

	setting_SaveCumulativeHours();

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-operationScreen_HandlerCumulativeHour	 		 							*/
/*                                                                                      */
/* Details  -Handler operation cumulative hour											*/
/*			  														                    */
/*                                                                                      */
/* Arguments : (I) NONE																	*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                        	*/
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void operationScreen_HandlerCumulativeHour(void)
{
	if(modeMgr_GetCurrentMode() == ePatientMode || modeMgr_GetCurrentMode() == eServiceMode)
	{
		/**************Patient cumulative**************************/
		operationScreen_UpdatePatientCumulativeHour();
		/**************Patient cumulative**************************/

		/**************Unit cumulative*****************************/
		operationScreen_UpdateUnitCumulativeHour();
		/**************Unit cumulative*****************************/
		operationScreen_UpdateEquipmentCumulativeHour();
		operationScreen_SetCumulativeHour();

		//Timer one hour
		static int counterHour = 0;
		counterHour++;
		if(counterHour >= SECONDS_IN_ONE_HOUR)
		{
			counterHour = 0;
			setting_SaveCumulativeHours();
		}
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-operationScreen_SendFRToDevice	 		 									*/
/*                                                                                      */
/* Details  -Send flow rate setting to device after change flow rate setting			*/
/*			  														                    */
/*                                                                                      */
/* Arguments : (I) NONE																	*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : NONE			                                                        */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) g_operationScreen : operration screen		              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void operationScreen_SendFRToDevice(void)
{
	if(WM_IsVisible(g_operationScreen))
	{
		if(modeMgr_GetCurrentMode() == ePatientMode)
		{
			uint8_t flowRate = guiTask_GetFlowrate();
			uint8_t penddingFlowRate = guiTask_GetPenddingFlowRate();
			uint8_t flowrateSetting = 0;
			if(flowRate != penddingFlowRate)
			{
				guiTask_SetPenddingFowRate(flowRate);
				if(guiInterface_GetSettingValue(eChildLockSettingId) == eOff)
				{
					guiInterface_SetSettingValue(eFlowRateSettingId,guiTask_GetFlowrate());
					setting_SaveAllSettingValue();
					logInterface_WriteMacineLog(guiInterface_GetSettingValue(eFlowRateSettingId), eChangingFlowRateLogId);
				}

				flowrateSetting = guiInterface_GetSettingValue(eFlowRateSettingId);

				if(flowrateSetting >= SETTING_FR_025L && flowrateSetting <= SETTING_FR_300L)
				{
					guiTask_SendPSAParaEvent(ePacketPSAParaChangeId,flowrateSetting,
							guiTask_GetConditionO2(),guiTask_GetConditionTemp());
				}
			}

			/** >>#0017
				Not check current flow rate and pendding flow rate when play flow rate
			#0017 >>**/
			audioMgr_PlayFlowRate(flowRate);
		}
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-operationScreen_SetCumulativeHour							                */
/*                                                                                      */
/* Details  -Set cumulative operation hour to cumulative hour array						*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE										 							*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_patientSec : seconds of patient cumulative hour     		*/
/*                     (O) s_patientMin : minutes of patient cumulative hour            */
/*                     (O) s_patientHour : hour of patient cumulative hour              */
/*                     (O) s_equipSec : seconds of equipment cumulative hour            */
/*                     (O) s_equipMin : minutes of equipment cumulative hour            */
/*                     (O) s_equipHour : hour of equipment cumulative hour              */
/*                     (O) s_unitSec : seconds of unit cumulative hour     			   	*/
/*                     (O) s_unitMin : minutes of unit cumulative hour	            	*/
/*                     (O) s_unitHour : hour of unit cumulative hour     			   	*/
/*                     (O) s_unitSecThree : seconds of unit cumulative hour 3L		    */
/*                     (O) s_unitMinThree : minutes of unit cumulative hour 3L         	*/
/*                     (O) s_unitHourThree : hour of unit cumulative hour 3L        	*/
/*                     (O) s_unitSecTwoFive : seconds of unit cumulative hour 2.5L		*/
/*                     (O) s_unitMinTwoFive : minutes of unit cumulative hour 2.5L      */
/*                     (O) s_unitHourTwoFive : hour of unit cumulative hour 2.5L        */
/*                     (O) s_unitSecTwo : seconds of unit cumulative hour 2L			*/
/*                     (O) s_unitMinTwo : minutes of unit cumulative hour 2L      		*/
/*                     (O) s_unitHourTwo : hour of unit cumulative hour 2L        		*/
/*                     (O) s_unitSecOneSevenFive : seconds of unit cumulative hour 1.75L*/
/*                     (O) s_unitMinOneSevenFive : minutes of unit cumulative hour 1.75L*/
/*                     (O) s_unitHourOneSevenFive : hour of unit cumulative hour 1.75L  */
/*                     (O) s_unitSecOneFive : seconds of unit cumulative hour 1.5L		*/
/*                     (O) s_unitMinOneFive : minutes of unit cumulative hour 1.5L		*/
/*                     (O) s_unitHourOneFive : hour of unit cumulative hour 1.5L        */
/*                     (O) s_unitSecOneTwoFive : seconds of unit cumulative hour 1.25L	*/
/*                     (O) s_unitMinOneTwoFive : minutes of unit cumulative hour 1.25L	*/
/*                     (O) s_unitHourOneTwoFive : hour of unit cumulative hour 1.25L    */
/*                     (O) s_unitSecOne : seconds of unit cumulative hour 1L			*/
/*                     (O) s_unitMinOne : minutes of unit cumulative hour 1L			*/
/*                     (O) s_unitHourOne : hour of unit cumulative hour 1L    			*/
/*                     (O) s_unitSecZeroSevenFive : seconds of unit cumulative hour 0.75L*/
/*                     (O) s_unitMinZeroSevenFive : minutes of unit cumulative hour 0.75L*/
/*                     (O) s_unitHourZeroSevenFive : hour of unit cumulative hour 0.75L */
/*                     (O) s_unitSecZeroFive : seconds of unit cumulative hour 0.5L		*/
/*                     (O) s_unitMinZeroFive : minutes of unit cumulative hour 0.5L		*/
/*                     (O) s_unitHourZeroFive : hour of unit cumulative hour 0.5L  		*/
/*                     (O) s_unitSecZeroTwoFive : seconds of unit cumulative hour 0.25L */
/*                     (O) s_unitMinZeroTwoFive : minutes of unit cumulative hour 0.25L */
/*                     (O) s_unitHourZeroTwoFive : hour of unit cumulative hour 0.25L   */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void operationScreen_SetCumulativeHour(void)
{
	guiInterface_SetCumulativeHour(patients,s_patientHour,s_patientMin,s_patientSec);
	guiInterface_SetCumulativeHour(uints,s_unitHour,s_unitMin,s_unitSec);
	guiInterface_SetCumulativeHour(uints_300,s_unitHourThree,s_unitMinThree,s_unitSecThree);
	guiInterface_SetCumulativeHour(units_250,s_unitHourTwoFive,s_unitMinTwoFive,s_unitSecTwoFive);
	guiInterface_SetCumulativeHour(uints_200,s_unitHourTwo,s_unitMinTwo,s_unitSecTwo);
	guiInterface_SetCumulativeHour(uints_175,s_unitHourOneSevenFive,s_unitMinOneSevenFive,s_unitSecOneSevenFive);
	guiInterface_SetCumulativeHour(uints_150,s_unitHourOneFive,s_unitMinOneFive,s_unitSecOneFive);
	guiInterface_SetCumulativeHour(uints_125,s_unitHourOneTwoFive,s_unitMinOneTwoFive,s_unitSecOneTwoFive);
	guiInterface_SetCumulativeHour(uints_100,s_unitHourOne,s_unitMinOne,s_unitSecOne);
	guiInterface_SetCumulativeHour(uints_075,s_unitHourZeroSevenFive,s_unitMinZeroSevenFive,s_unitSecZeroSevenFive);
	guiInterface_SetCumulativeHour(uints_050,s_unitHourZeroFive,s_unitMinZeroFive,s_unitSecZeroFive);
	guiInterface_SetCumulativeHour(uints_025,s_unitHourZeroTwoFive,s_unitMinZeroTwoFive,s_unitSecZeroTwoFive);
	guiInterface_SetCumulativeHour(equipments,s_equipHour,s_equipMin,s_equipSec);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-operationScreen_ShowHideChildlock						                    */
/*                                                                                      */
/* Details  -Show hide flow rate chilld lock										    */
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE										 							*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) g_operationScreen : operation screen			 		    	*/
/*                     (O) s_flowRateValue : display flow rate   					   	*/
/*                     (O) s_timerChildLock : timer show/hide flow rate in childlock    */
/*                     (O) s_changedFR : flag change flow rate				         	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void operationScreen_ShowHideChildlock(void)
{
	if(WM_IsVisible(g_operationScreen))
	{
		if(modeMgr_GetCurrentMode() == ePatientMode)
		{
			if(guiInterface_GetSettingValue(eChildLockSettingId) == eOn)
			{
				if(s_changedFR == true)
				{
					s_counter5s++;

					if(s_counter5s >= COUNTER_HIDE_SHOW_CHILD_LOCK)
					{
						guihandler_StartTimerPlayChildLock();

						guiTask_SetFowRate(guiTask_GetPenddingFlowRate());
						WM_ShowWindow(s_flowRateValue);
						WM_Paint(s_flowRateValue);
						s_changedFR = false;
						s_counter5s = 0;
						guiTask_SetBlinkingFR(false);

						//Display flow rate in password dialog
						passwordDialog_DisplayFlowRate(guiTask_GetPenddingFlowRate());
					}
					else
					{
						if(WM_IsVisible(s_flowRateValue))
						{
							WM_HideWindow(s_flowRateValue);
						}
						else
						{
							WM_ShowWindow(s_flowRateValue);
						}

						WM_RestartTimer(s_timerChildLock,TIMER_500_MS);
					}
				}
			}
		}
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-operationScreen_HideO2Cylinder 								       	    */
/*                                                                                      */
/* Details  -Hide O2 cylinder image														*/
/*			  														                    */
/*                                                                                      */
/* Arguments : (I) NONE								 		   							*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_O2Cylinder : O2 cylinder image                             */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void operationScreen_HideO2Cylinder(void)
{
	WM_HideWindow(s_O2Cylinder);

	return;
}

/* end of files */

