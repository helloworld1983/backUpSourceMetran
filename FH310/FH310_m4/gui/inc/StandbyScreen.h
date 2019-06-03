#ifndef INC_STANDBYSCREEN_H_
#define INC_STANDBYSCREEN_H_

/********************************************************************/
/*                                                                  */
/* File Name    : StandbyScreen.h                                 	*/
/*                                                                  */
/* General      : Display standby screen			                */
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
#include "MainScreen.h"
#include "DualCoreCommon.h"

extern WM_HWIN g_standbyScreen;

//Init standby screen
void standbyScreen_Init(void);
//Show standby screen
void standbyScreen_Show(void);
//Hide standby screen
void standbyScreen_Hide(void);
//Set state machine for system
void standbyScreen_SetStateMachine(E_StateMachine state);
//Get state machine for system
E_StateMachine standbyScreen_GetStateMachine(void);
//Show anypal in stanby screen
void standbyScreen_ShowAnypal(void);
//Show icon charging anypal in standby screen
void standbyScreen_ShowAnypalCharging(void);
//Set psa sequence
void standbyScreen_SetPsaSequence(E_PsaSequence sequence);
//Restart timer black screen
void standbyScreen_RestartTimerBlackScreen(void);

#endif /* INC_STANDBYSCREEN_H_ */
/* end of file */

