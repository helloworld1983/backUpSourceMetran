#ifndef INC_MODE_HANDLE_H_
#define INC_MODE_HANDLE_H_

/********************************************************************/
/*                                                                  */
/* File Name     : ModeHandle.c		                               	*/
/*                                                                  */
/* General       : handle mode operation				            */
/*                 	 						                        */
/*                                                                  */
/* Product Name  : FH310                                            */
/* 						                                            */
/* Company		 : Metran                                           */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2016/08/05      Quyen Ngo  	new file       		    */
/*																	*/
/* #0001	   2017/09/08	   Quyen Ngo	modify					*/
/*		-remove goto stop mode										*/
/********************************************************************/
#include "DualCoreCommon.h"

//handle mode change
void mode_HandleModeChange(E_ModeSetting fromMode, E_ModeSetting toMode);

// go to stand by mode
void mode_GotoStandby(E_ModeSetting fromMode);

//go to Patient mode
void mode_GotoPatient(E_ModeSetting fromMode);

//go to service mode
void mode_GotoService(E_ModeSetting fromMode);

//go to test mode
void mode_GotoTest(E_ModeSetting fromMode);

//go to alarm mode
void mode_gotoAlarm(E_ModeSetting fromMode);

//go to battery alarm mode
void mode_GoToBatteryAlarm(E_ModeSetting fromMode);

void mode_gotoAlarmExtFlow(E_ModeSetting fromMode);

void mode_gotoAlarmNoOxyCylinder(E_ModeSetting fromMode);

// get current mode
E_ModeSetting  mode_GetCurrentMode(void);

#endif /* INC_MODE_HANDLE_H_ */
