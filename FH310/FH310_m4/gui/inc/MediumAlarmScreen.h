#ifndef INC_MEDIUMALARMSCREEN_H_
#define INC_MEDIUMALARMSCREEN_H_

/********************************************************************/
/*                                                                  */
/* File Name    : MediumAlarmScreen.h                               */
/*                                                                  */
/* General      : Display medium alarm screen			            */
/* 								                                    */
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

#include "WM.h"
#include "DualCoreCommon.h"

#define BACKGROUND_MEDIUM_ALARM 			0x0947E9
#define BORDER_MEDIUM_ALARM_COLOR 			0x966B00

extern WM_HWIN g_mediumAlarmScreen;

//Init medium alarm screen
void mediumAlarmScreen_Init(void);
//Show medium alarm screen
void mediumAlarmScreen_Show(E_AlarmId alarmId);
//Hide medium alarm screen
void mediumAlarmScreen_Hide(void);
//Update animation in medium alarm
void mediumAlarmScreen_UpdateMediumAnimation(void);

#endif /* INC_MEDIUMALARMSCREEN_H_ */
/* end of file */
