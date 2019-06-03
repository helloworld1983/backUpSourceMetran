#ifndef INC_EQUIPMENTSETTINGSCREEN_H_
#define INC_EQUIPMENTSETTINGSCREEN_H_

/********************************************************************/
/*                                                                  */
/* File Name    : EquipmentSettingScreen.h                          */
/*                                                                  */
/* General       : Changing the equipment settings 		            */
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

#include "WM.h"

#define SIZE_BUFFER_HOUR_EQUIP				6
#define SIZE_BUFFER_SETTING_EQUIP			10

extern WM_HWIN g_equipmentSettingScreen;

//Init equipment Setting screen
void equipmentSettingScreen_Init(void);
//Set default setting
void equipmentSettingScreen_SetDefaut(void);
//Show equipment Setting screen
void equipmentSettingScreen_Show(void);
//Hide equipment Setting screen
void equipmentSettingScreen_Hide(void);
//Hide all child
void equipmentSettingScreen_HideAll(void);
//Show all child
void equipmentSettingScreen_ShowAll(void);
//Update setting
void equipmentSettingScreen_UpdateSetting(void);
//Show patient cumulative operation hour
void equipmentSettingScreen_ShowPatientHour(void);

#endif /* INC_EQUIPMENTSETTINGSCREEN_H_ */
/* end of file */
