#ifndef ALARMCTRL_HEADER_H
#define ALARMCTRL_HEADER_H
/********************************************************************/
/*                                                                  */
/* File Name    : AlarmControl.h                                   	*/
/*                                                                  */
/* General      : AlarmControl module receive queue from Alarm Task	*/
/*                to handle and send to UI					     	*/
/*                 	 						                        */
/*                                                                  */
/* Product Name  : Portable                                         */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2017/02/09     Truong Nguyen(MV)  	 new file       */
/*                                                                  */
/********************************************************************/
#include <DualCoreCommon.h>



/****************************************************************************************/
/*                                                                                      */
/* Function name: 	alarmCtrl_Init 														*/
/*                                                                                      */
/* Details:         Initial Alarm Control												*/
/*                                                                                      */
/* Arguments : (I)  None      															*/
/*                                                                                      */
/****************************************************************************************/
void alarmMsgHandler_Init();


/****************************************************************************************/
/*                                                                                      */
/* Function name：  	alarmCtrl_StopDisplayErrorCode										*/
/*                                                                                      */
/* Details：  	Stop display Error Code to display Flow Rate Setting when The value		*/
/*              of Flow Rate Setting is changed by press volume button                  */
/*                                                                                      */
/*【Note】                                                                            										    */
/*                                                                                      */
/****************************************************************************************/
void alarmMsgHandler_StopDisplayErrorCode();


/****************************************************************************************/
/*                                                                                      */
/* Function name：  	alarmCtrl_GetStatusAlarm											*/
/*                                                                                      */
/* Details：  	Return Status of Alarm 													*/
/*                                                                                      */
/****************************************************************************************/
bool alarmMsgHandler_GetStatusAlarm();


/****************************************************************************************/
/*                                                                                      */
/* Function name：  	alarmCtrl_GetStatusAlarm											*/
/*                                                                                      */
/* Details：  	Return Status of Volume Button Once Press 								*/
/*              ( return True if it has not any Alarm)                                  */
/*                                                                                      */
/****************************************************************************************/
bool alarmMsgHandler_GetStatusVolumeButtonOncePress();


/****************************************************************************************/
/*                                                                                      */
/* Function name:   alarmCtrl_HandleKeyWhenAlarmOccur                                   */
/*                                                                                      */
/* Details: Handle alarm when buttons is pressed                                        */
/*                                                                                      */
/* Arguments:   None                                                                    */
/*                                                                                      */
/* ReturnValue : None                                                                   */
/*                                                                                      */
/****************************************************************************************/
void alarmMsgHandler_HandleKeyWhenAlarmOccur();

/****************************************************************************************/
/*                                                                                      */
/* Function name：   alarmMsgHandler_BatteryInvalidHandle                                 */
/*                                                                                      */
/* Details：     Handle when battery invalid                                               */
/*                                                                                      */
/* Arguments :  None                                                                    */
/*                                                                                      */
/* ReturnValue : void                                                                   */
/*                                                                                      */
/* Using Global Data :  gs_isPowerLostOccur                                             */
/*                                                                                      */
/* local variables   :                                                                  */
/*                                                                                      */
/*【Note】                                                                                                                                                                */
/*                                                                                      */
/****************************************************************************************/
void alarmMsgHandler_BatteryInvalidHandle();
/****************************************************************************************/
/*                                                                                      */
/* Function name：  	alarmMsgHandler_HandleAlarm										*/
/*                                                                                      */
/* Details：  	Handling Alarm Queue to Update Alarm        						    */
/*                                                                                      */
/* Arguments :  ALARM_STAT_T alarm_Receive		 					  					*/
/*                                                                                      */
/*                                                                                      */
/****************************************************************************************/
void alarmMsgHandler_HandleAlarm(ALARM_STAT_T alarmReceive);

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	alarmCtrl_DisableAllAlarm							              	*/
/*                                                                                      */
/* Details: clear all alarm and stop all alarm sound									*/
/*                                                                                      */
/* Arguments:	None																	*/
/*                                                                                      */
/* ReturnValue : None			                                                        */
/*                                                                                      */
/****************************************************************************************/
void alarmMsgHandler_DisableAllAlarm();


bool alarmMsgHandler_IsGoingToPlayPowerLostAlarm();



#endif /* ALARMCTRL_HEADER_H */
