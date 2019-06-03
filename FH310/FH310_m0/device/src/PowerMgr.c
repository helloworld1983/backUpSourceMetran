/********************************************************************/
/*                                                                  */
/* File Name     : PowerMgr.c			                            */
/*                                                                  */
/* General       : This file contain function for handle the power	*/
/*  			   signal of FH310									*/
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
/*             2016/09/28      Quyen Ngo  			new file  	    */
/*	#0002.	   2017/09/08	   Quyen Ngo			modify			*/
/*		When detect power lost signal, just send the msg to M4, does*/
/*      not judge the current mode                                  */
/*	#0003	   2017/09/08	   Viet Le			modify				*/
/*      Edit eGotoBatteryAlarmMode->ePowerLostDetected in			*/
/*			power_SendPowerLostMsgToM4()							*/
/*	#0004	   2017/10/30	   Quyen Ngo			modify			*/
/*      Remove s_powerBinarySem not use								*/
/*	#0005	   2017/11/02	   Quyen Ngo			modify			*/
/*      Handle the case IPC queue full when send the power lost Msg */
/*	#0006	   2017/11/17	   Quyen Ngo			modify			*/
/*      Send the power lost msg every cycle of dev task and switch  */
/*		to send with interval 1s after send to IPC QUEUE sucesscully*/
/*		(Bug 2019)													*/
/********************************************************************/

#include <string.h>

#include "PowerMgr.h"
#include "gpio.h"
#include "ModeHandle.h"
#include "board.h"
#include "DeviceController.h"
#include "DualCoreCommon.h"
#include "AlarmLed.h"
#include "LatchValve.h"
#include "BatteryHandling.h"
#include "../../ipclib/IpcMsg.h"
#include "../../ipc/IpcTask.h"
#include "TaskCommon.h"

#ifdef DEBUG
#define DEBUG_POWER_EN
#endif

#ifdef DEBUG_POWER_EN
#define DEBUG_POWER(...) printf(__VA_ARGS__)
#else
#define DEBUG_POWER(...)
#endif

#define GPIO_GROUP_INT_NUM						1

#define POWER_RESEND_TIME						1000/DEVICE_TASK_DELAY_TIME //1s

static bool s_isLostPower = false;

/******************************************************************************/
//$COMMON.OPERATION$
//    Function Name: GINT0_IRQHandler
//
//    Detail: Handle power lost detect interrupt
//
//    Arguments: (I)
//				 (O)
//
//    Return Values: NONE
//
//
//    Using Global Data : (I)
//						  (O)
//
//    Local variables: None
//
//    Note: None
//
/******************************************************************************/
void GINT1_IRQHandler(void)
{
	Chip_GPIOGP_ClearIntStatus(LPC_GPIOGROUP, GPIO_GROUP_INT_NUM);
	s_isLostPower = true;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Function Name: power_Init
//
//    Detail: power port Init
//
//    Arguments: (I)
//				 (O)
//
//    Return Values: NONE
//
//
//    Using Global Data : (I)
//						  (O)
//
//    Local variables: None
//
//    Note: None
//
/******************************************************************************/
void power_Init(void)
{
	Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT,POWER24_DETECT_PORT_NUM, POWER24_DETECT_BIT_NUM);		/* Power 24 detect */
	Chip_GPIOGP_SelectHighLevel(LPC_GPIOGROUP, GPIO_GROUP_INT_NUM, POWER24_DETECT_PORT_NUM, 1 << POWER24_DETECT_BIT_NUM);
	Chip_GPIOGP_EnableGroupPins(LPC_GPIOGROUP, GPIO_GROUP_INT_NUM, POWER24_DETECT_PORT_NUM, 1 << POWER24_DETECT_BIT_NUM);
	Chip_GPIOGP_SelectAndMode(LPC_GPIOGROUP, GPIO_GROUP_INT_NUM);
	Chip_GPIOGP_SelectEdgeMode(LPC_GPIOGROUP, GPIO_GROUP_INT_NUM);

	/* Enable Group GPIO interrupt 0 */
	NVIC_EnableIRQ(GINT1_IRQn);
	NVIC_SetPriority(GINT1_IRQn,POWER_DETECT_INT_PRIO);
	return;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Function Name: power_Handle
//
//    Detail: Handle power
//
//    Arguments: (I)
//				 (O)
//
//    Return Values: NONE
//
//
//    Using Global Data : (I)
//						  (O)
//
//    Local variables: None
//
//    Note: None
//
/******************************************************************************/
void power_Handle(void)
{
	if(s_isLostPower == true)
	{
		static uint16_t s_ResendMsgCnt = POWER_RESEND_TIME;
		if (s_ResendMsgCnt >= POWER_RESEND_TIME)
		{
			if(!power_SendPowerLostMsgToM4())
			{
				s_ResendMsgCnt = POWER_RESEND_TIME;
			}
			else
			{
				s_ResendMsgCnt = 1;
			}
		}
		else
		{
			s_ResendMsgCnt++;
		}
	}
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Function Name: power_CheckPowerWhenHandShake
//
//    Detail: Check power status when process IPC hand shake with M4
//
//    Arguments: (I)
//				 (O)
//
//    Return Values: bool
//					 true: power is normal
//					 false: lost power
//
//
//    Using Global Data : (I)
//						  (O)
//
//    Local variables: None
//
//    Note: None
//
/******************************************************************************/
bool power_CheckPowerWhenHandShake(void)
{
	bool s_curACLostState = false;
	s_curACLostState = Chip_GPIO_GetPinState(LPC_GPIO_PORT, POWER24_DETECT_PORT_NUM, POWER24_DETECT_BIT_NUM);
	return !s_curACLostState;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name:  power_SendPowerLostMsgToM4											*/
/*                                                                                      */
/* Details:  send power lost message to M4												*/
/*                                                                                      */
/* Arguments :														 					*/
/*                                                                                      */
/* Macros:	   																			*/
/*																						*/
/* ReturnValue : bool 		 															*/
/*				 true/false: Send successful/not										*/
/*                                                                                      */
/* Using Global Data :																	*/
/*                     								                                 	*/
/*【Note】                                                                            											    	*/
/*                                                                                      */
/****************************************************************************************/
bool power_SendPowerLostMsgToM4(void)
{
	IPC_MSG_DATA_t data;
	data.data_id = ePowerLostDetected;
	memset(&data.stPsaMonitorPacket, 0, sizeof(data.stPsaMonitorPacket));
	memset(&data.stAlarmStatPacket, 0, sizeof(data.stAlarmStatPacket));
	memset(&data.stPsaParamPacket, 0, sizeof(data.stPsaParamPacket));
	memset(&data.version, 0, sizeof(data.version));
	data.data_value = (uint16_t)ePatientMode;
	if (ipc_PushMsg(IPCEX_ID_GUI, data) != QUEUE_INSERT) {
		DEBUGOUT("M0 Push data Err: %d\n",data.data_id);
		return false;
	}
	else
	{
		return true;
	}
}

