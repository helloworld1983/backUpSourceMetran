#ifndef INC_MODE_MGR_H_
#define INC_MODE_MGR_H_

/********************************************************************/
/*                                                                  */
/* File Name    : ModeMgr.h                              			*/
/*                                                                  */
/* General       : This file contain functions for handle 			*/
/* 				   	    mode operation of FH310                     */
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
/* #0001       2017/09/08      Viet Le              modify			*/
/*      - Remove modeMgr_GoToBatteryAlarm()			                */
/*		- Add modeMgr_HandlerPowerLost()							*/
/********************************************************************/
#include "DualCoreCommon.h"

//Go to stand by mode
void modeMgr_GotoStandby(void);
//Go to Patient mode
void modeMgr_GotoPatient(void);
//Go to service mode
void modeMgr_GotoService(void);
//Go to alarm mode
void modeMgr_gotoAlarm(void);
//Go to extenal flow path mode
void modeMgr_GotoAlarmExternalFlowPath(void);
//Go to not O2 cylinder mode
void modeMgr_GotoAlarmNotO2Cylinder(void);
//Get current mode
E_ModeSetting  modeMgr_GetCurrentMode(void);
//Get previous mode
E_ModeSetting  modeMgr_GetPreviousMode(void);
//Recover Previous Mode
void  modeMgr_RecoverMode(E_ModeSetting eepromMode);
//Go to battery alarm mode
void modeMgr_GotoBatteryAlarmMode(void);
//Go to stop mode
void modeMgr_GotoStopMode(void);
//WatDog reset
void modeMgr_WatchDogReset(void);
//Handle pow lost
void modeMgr_HandlerPowerLost(void);

#endif /* INC_MODE_MGR_H_ */
/* end of file */
