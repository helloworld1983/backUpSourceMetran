#ifndef SETTINGONOFFBUTTON_H_
#define SETTINGONOFFBUTTON_H_

/********************************************************************/
/*                                                                  */
/* File Name    : SettingOnOffButton.h                            	*/
/*                                                                  */
/* General      : The SettingOnOffButton category provides          */
/* 				  the user an interface to display and              */
/*                change setting on/off in system             		*/
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

#include "stdint.h"
#include "WM.h"
#include "EDIT.h"
#include "stdbool.h"

#define TIMER_CLOSE_VALVE					1800000//1800000ms = 30 minutes
#define TIMER_CLOSE_COMPRESSOR				180000//180000ms = 3 minutes
#define SIZE_BUFFER_ONOFF					5
#define TIMER_10_MS							10
#define DELAY_SEND_VIA_IPC					5

//Define id for setting on/off button
typedef enum
{
	eValueValve1Id = 0,
	eValueValve2Id,
	eValueValve3Id,
	eValueValve4Id,
	eValueValve5Id,
	eValueMotorCoolingId,
	eValueMotorFilterId,
	eValueMotorCompressorId,
	eValueBuzzerId,
	eValueRedLedId,
	eValueYellowLedId,
	eValuePowerLedId,
	eValueVoiceId,
	eValueChildLockId,
	eValueBluetoothId,
	eValueBluetoothTransferId,
	eValueRTCId,
	eValueMaxOnOffId
}E_ValueOnOffId;

//Init setting on/off button
void settingOnOffLargeButton_Init(EDIT_Handle obj, int onBtnId,int offBtnId,E_ValueOnOffId posArr,
		int valueId, GUI_RECT Rect);
//Custom setting on/off button
void settingOnOffButton_Callback(WM_MESSAGE *pMsg);
//Get value setting on/off button
uint16_t settingOnOffButton_GetValue(E_ValueOnOffId posArr);
//Set value for setting
void settingOnOffButton_SetValue(E_ValueOnOffId valueEdit,const int8_t * value);
//Set off device
void settingOnOffButton_SetOffAllDevice(void);

#endif /* SETTINGONOFFBUTTON_H_ */
/* end of file */

