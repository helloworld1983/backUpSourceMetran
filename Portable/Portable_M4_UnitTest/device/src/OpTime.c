/********************************************************************/
/*                                                                  */
/* File Name    : OpTime.c		                                    */
/*                                                                  */
/* General       : This module contain function for manage Operation*/
/*                 Time						                        */
/*                                                                  */
/* Product Name  : Portable                                         */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*          April 18, 2017      Thao Ha(MV)  	new file        	*/
/*                                                                  */
/********************************************************************/
#include <DualCoreCommon.h>
#include <OpTime.h>
#include <PanelLed.h>
#include <RTC.h>
#include <TaskCommon.h>
#include <UIMgr.h>
#include "Setting.h"
#include "USBControl.h"
#include "USBTask.h"

#define UPDATE_OPERATION_HOUR_LOOP_CNT 600000/DEVICE_TASK_DELAY_TIME
#define AFTER_0MS_LOOP_CNT 0/DEVICE_TASK_DELAY_TIME
#define AFTER_200MS_LOOP_CNT 200/DEVICE_TASK_DELAY_TIME
#define AFTER_1200MS_LOOP_CNT 1200/DEVICE_TASK_DELAY_TIME
#define AFTER_1400MS_LOOP_CNT 1400/DEVICE_TASK_DELAY_TIME
#define AFTER_2400MS_LOOP_CNT 2400/DEVICE_TASK_DELAY_TIME
#define AFTER_2600MS_LOOP_CNT 2600/DEVICE_TASK_DELAY_TIME
#define hourToTick(x) x*6
#define tickToHour(x) ((float)x*10.0)/60.0
//#define DEBUG_OPERTIME_EN
#ifdef DEBUG_OPERTIME_EN
#define DEBUG_OPERTIME(...) printf(__VA_ARGS__)
#else
#define DEBUG_OPERTIME(...)
#endif

static uint32_t gs_currOperationTimeContinuousTick=0;
static uint32_t gs_currOperationTimeSynchronizeTick=0;
static uint32_t gs_currOperationTimeAfterMaintenanceContinuousTick=0;
static uint32_t gs_currOperationTimeAfterMaintenanceSynchronizeTick=0;
static bool gs_isDisplayOperationTime = false;

uint16_t gs_currentTotalOperationTimeSynchronizeMode=0;
uint16_t gs_currentTotalOperationTimeContinuousMode=0;
uint16_t gs_currentOperationTimeFromPrevMaintenanceSynchronizeMode=0;
uint16_t gs_currentOperationTimeFromPrevMaintenanceContinuousMode=0;
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	OperationTime_Init					              				    */
/*                                                                                      */
/* Details:        Initialize OperationTime function by reading the previous tick time  */
/* 					from EEPROM															*/
/*                                                                                      */
/* Arguments:	None																	*/
/*                                                                                      */
/* ReturnValue : void			                                                        */
/*                                                                                      */
/*                                                                                      */
/****************************************************************************************/
void OperationTime_Init()
{
	gs_currOperationTimeContinuousTick=setting_GetTotalOperationTimeContinuous();
	gs_currOperationTimeSynchronizeTick=setting_GetTotalOperationTimeSynchronize();
	gs_currOperationTimeAfterMaintenanceContinuousTick=setting_GetOperationTimeAfterMaintenanceContinuous();
	gs_currOperationTimeAfterMaintenanceSynchronizeTick=setting_GetOperationTimeAfterMaintenanceSynchronize();

	gs_currentTotalOperationTimeSynchronizeMode=((gs_currOperationTimeSynchronizeTick)*10)/60;
	gs_currentTotalOperationTimeContinuousMode=((gs_currOperationTimeContinuousTick)*10)/60;
	gs_currentOperationTimeFromPrevMaintenanceSynchronizeMode=((gs_currOperationTimeAfterMaintenanceSynchronizeTick)*10)/60;
	gs_currentOperationTimeFromPrevMaintenanceContinuousMode=((gs_currOperationTimeAfterMaintenanceContinuousTick)*10)/60;

	DEBUG_OPERTIME("gs_currOperationTimeContinuousTick %d\n",gs_currOperationTimeContinuousTick);
	DEBUG_OPERTIME("gs_currOperationTimeSynchronizeTick %d \n",gs_currOperationTimeSynchronizeTick);
	DEBUG_OPERTIME("gs_currOperationTimeAfterMaintenanceContinuousTick %d \n",gs_currOperationTimeAfterMaintenanceContinuousTick);
	DEBUG_OPERTIME("gs_currOperationTimeAfterMaintenanceSynchronizeTick %d \n",gs_currOperationTimeAfterMaintenanceSynchronizeTick);

}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	OperationTime_HandleDisplay			              				    */
/*                                                                                      */
/* Details:        Display current total operation time on LED panel					*/
/*                                                                                      */
/* Arguments:	None																	*/
/*                                                                                      */
/* ReturnValue : void			                                                        */
/*                                                                                      */
/*                                                                                      */
/****************************************************************************************/
static void OperationTime_HandleDisplay()
{
	static uint16_t s_cnt = 0;
	uint16_t currentHour=0;
	static uint16_t hour[2]={0};

	switch(s_cnt)
	{
	case AFTER_0MS_LOOP_CNT:
		currentHour = ((gs_currOperationTimeSynchronizeTick)*10)/60;
		currentHour += ((gs_currOperationTimeContinuousTick)*10)/60;
		hour[0]=currentHour/1000.0;
		hour[1]=currentHour - (hour[0]*1000);
		DEBUG_OPERTIME("number %d %d \n",hour[0],hour[1]);

		panelLed_LedDisplay(eOffSevenSeg);

		s_cnt++;
		break;
	case AFTER_200MS_LOOP_CNT:
		panelLed_SevenSegDisplayOperationTimeValue(hour[0]);
		s_cnt++;
		break;
	case AFTER_1200MS_LOOP_CNT:

		panelLed_LedDisplay(eOffSevenSeg);

		s_cnt++;
		break;
	case AFTER_1400MS_LOOP_CNT:
		panelLed_SevenSegDisplayOperationTimeValue(hour[1]);
		s_cnt++;
		break;
	case AFTER_2400MS_LOOP_CNT:

		panelLed_LedDisplay(eOffSevenSeg);

		s_cnt++;
		break;
	case AFTER_2600MS_LOOP_CNT:
		UIMgr_UpdateMode();
		s_cnt=0;
		gs_isDisplayOperationTime=false;
		break;
	default:
		s_cnt++;
		break;
	}
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	OperationTime_Handle				              				    */
/*                                                                                      */
/* Details:     OperationTime handle function is call for update current Operation		*/
/* 				tick time every 1 hour													*/
/*                                                                                      */
/* Arguments:	None																	*/
/*                                                                                      */
/* ReturnValue : void			                                                        */
/*                                                                                      */
/*                                                                                      */
/****************************************************************************************/
void OperationTime_Handle()
{
	static uint32_t s_cnt = 0;
	s_cnt++;
	if(s_cnt>=UPDATE_OPERATION_HOUR_LOOP_CNT)
	{
		s_cnt = 0;
		if(setting_Get(eModeSettingID)==eSynchronizedMode)
		{
			gs_currOperationTimeSynchronizeTick++;
			gs_currOperationTimeAfterMaintenanceSynchronizeTick++;
		}
		else
		{
			gs_currOperationTimeContinuousTick++;
			gs_currOperationTimeAfterMaintenanceContinuousTick++;
		}
		OperationTime_Save();
		uint16_t temp_currentTotalOperationTimeSynchronizeMode=((gs_currOperationTimeSynchronizeTick)*10)/60;
		uint16_t temp_currentTotalOperationTimeContinuousMode=((gs_currOperationTimeContinuousTick)*10)/60;
		uint16_t temp_currentOperationTimeFromPrevMaintenanceSynchronizeMode=((gs_currOperationTimeAfterMaintenanceSynchronizeTick)*10)/60;
		uint16_t temp_currentOperationTimeFromPrevMaintenanceContinuousMode=((gs_currOperationTimeAfterMaintenanceContinuousTick)*10)/60;
		if(temp_currentTotalOperationTimeSynchronizeMode!=gs_currentTotalOperationTimeSynchronizeMode
				|| temp_currentTotalOperationTimeContinuousMode!=gs_currentTotalOperationTimeContinuousMode
				|| temp_currentOperationTimeFromPrevMaintenanceSynchronizeMode!=gs_currentOperationTimeFromPrevMaintenanceSynchronizeMode
				|| temp_currentOperationTimeFromPrevMaintenanceContinuousMode!=gs_currentOperationTimeFromPrevMaintenanceContinuousMode
		)
		{
		    E_USBQUEUE usbQueue;
		    usbQueue.ID = eOperationTimeUpdate;
		    usbQueue.data = 0;

			if(!USBTask_SendToUSBQueue(usbQueue, 10)){
				DEBUG_TASK("Failed to send Item to USBQueue from alarmCtrl_SendErrorCodeToUI");
			}
		}
	}
	if(gs_isDisplayOperationTime == true)
	{
		OperationTime_HandleDisplay();
	}
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	OperationTime_Display				              				    */
/*                                                                                      */
/* Details:     Display current Operation time (continuous and synchronize)				*/
/*                                                                                      */
/* Arguments:	None																	*/
/*                                                                                      */
/* ReturnValue : void			                                                        */
/*                                                                                      */
/*                                                                                      */
/****************************************************************************************/
void OperationTime_Display()
{
	gs_isDisplayOperationTime = true;
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	OperationTime_SetTotalOperationTimeContinuous     				    */
/*                                                                                      */
/* Details:     Set total continuous operation time										*/
/*                                                                                      */
/* Arguments:	(I) uint32_t hour														*/
/*                                                                                      */
/* ReturnValue : void			                                                        */
/*                                                                                      */
/*                                                                                      */
/****************************************************************************************/
void OperationTime_SetTotalOperationTimeContinuous(uint32_t hour)
{
	gs_currOperationTimeContinuousTick = hourToTick(hour);
	OperationTime_Save();
	E_USBQUEUE usbQueue;
	usbQueue.ID = eOperationTimeUpdate;
	usbQueue.data = 0;

	if(!USBTask_SendToUSBQueue(usbQueue, 10)){
		DEBUG_TASK("Failed to send Item to USBQueue from alarmCtrl_SendErrorCodeToUI");
	}
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	OperationTime_SetTotalOperationTimeSynchronize     				    */
/*                                                                                      */
/* Details:     Set total Synchronize operation time									*/
/*                                                                                      */
/* Arguments:	(I) uint32_t hour														*/
/*                                                                                      */
/* ReturnValue : void			                                                        */
/*                                                                                      */
/*                                                                                      */
/****************************************************************************************/
void OperationTime_SetTotalOperationTimeSynchronize(uint32_t hour)
{
	gs_currOperationTimeSynchronizeTick = hourToTick(hour);
	OperationTime_Save();
	E_USBQUEUE usbQueue;
	usbQueue.ID = eOperationTimeUpdate;
	usbQueue.data = 0;

	if(!USBTask_SendToUSBQueue(usbQueue, 10)){
		DEBUG_TASK("Failed to send Item to USBQueue from alarmCtrl_SendErrorCodeToUI");
	}
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	OperationTime_SetOperationTimeAfterMaintenanceContinuous     	    */
/*                                                                                      */
/* Details:     Set total continuous operation time after maintenance					*/
/*                                                                                      */
/* Arguments:	(I) uint32_t hour														*/
/*                                                                                      */
/* ReturnValue : void			                                                        */
/*                                                                                      */
/*                                                                                      */
/****************************************************************************************/
void OperationTime_SetOperationTimeAfterMaintenanceContinuous(uint32_t hour)
{
	gs_currOperationTimeAfterMaintenanceContinuousTick = hourToTick(hour);
	OperationTime_Save();
	E_USBQUEUE usbQueue;
	usbQueue.ID = eOperationTimeUpdate;
	usbQueue.data = 0;

	if(!USBTask_SendToUSBQueue(usbQueue, 10)){
		DEBUG_TASK("Failed to send Item to USBQueue from alarmCtrl_SendErrorCodeToUI");
	}
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	OperationTime_SetOperationTimeAfterMaintenanceSynchronize     	    */
/*                                                                                      */
/* Details:     Set total Synchronize operation time after maintenance					*/
/*                                                                                      */
/* Arguments:	(I) uint32_t hour														*/
/*                                                                                      */
/* ReturnValue : void			                                                        */
/*                                                                                      */
/*                                                                                      */
/****************************************************************************************/
void OperationTime_SetOperationTimeAfterMaintenanceSynchronize(uint32_t hour)
{
	gs_currOperationTimeAfterMaintenanceSynchronizeTick = hourToTick(hour);
	OperationTime_Save();
	E_USBQUEUE usbQueue;
	usbQueue.ID = eOperationTimeUpdate;
	usbQueue.data = 0;

	if(!USBTask_SendToUSBQueue(usbQueue, 10)){
		DEBUG_TASK("Failed to send Item to USBQueue from alarmCtrl_SendErrorCodeToUI");
	}
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	OperationTime_Save										     	    */
/*                                                                                      */
/* Details:     Save total operation tick time to EEPROM								*/
/*                                                                                      */
/* Arguments:	None																	*/
/*                                                                                      */
/* ReturnValue : void			                                                        */
/*                                                                                      */
/*                                                                                      */
/****************************************************************************************/
void OperationTime_Save()
{
	setting_SetTotalOperationTimeContinuous(gs_currOperationTimeContinuousTick);
	setting_SetTotalOperationTimeSynchronize(gs_currOperationTimeSynchronizeTick);
	setting_SetOperationTimeAfterMaintenanceContinuous(gs_currOperationTimeAfterMaintenanceContinuousTick);
	setting_SetOperationTimeAfterMaintenanceSynchronize(gs_currOperationTimeAfterMaintenanceSynchronizeTick);
	setting_SystemInformationSave();
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	OperationTime_GetTotalOperationTimeSynchronize     	    			*/
/*                                                                                      */
/* Details:     get total Synchronize operation time 									*/
/*                                                                                      */
/* Arguments:	None																	*/
/*                                                                                      */
/* ReturnValue : uint32_t			                                                    */
/*                                                                                      */
/*                                                                                      */
/****************************************************************************************/
uint32_t OperationTime_GetTotalOperationTimeSynchronize(void)
{
	float currentHour = tickToHour(gs_currOperationTimeSynchronizeTick);
	return (uint32_t)currentHour;
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	OperationTime_GetTotalOperationTimeContinuous     	    			*/
/*                                                                                      */
/* Details:     get total continuous operation time 									*/
/*                                                                                      */
/* Arguments:	None																	*/
/*                                                                                      */
/* ReturnValue : uint32_t			                                                    */
/*                                                                                      */
/*                                                                                      */
/****************************************************************************************/
uint32_t OperationTime_GetTotalOperationTimeContinuous(void)
{
	float currentHour = tickToHour(gs_currOperationTimeContinuousTick);
	return (uint32_t)currentHour;
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	OperationTime_GetOperationTimeAfterMaintenanceSynchronize  			*/
/*                                                                                      */
/* Details:     get total synchronize operation time after maintenance					*/
/*                                                                                      */
/* Arguments:	None																	*/
/*                                                                                      */
/* ReturnValue : uint32_t			                                                    */
/*                                                                                      */
/*                                                                                      */
/****************************************************************************************/
uint32_t OperationTime_GetOperationTimeAfterMaintenanceSynchronize(void)
{
	float currentHour = tickToHour(gs_currOperationTimeAfterMaintenanceSynchronizeTick);
	return (uint32_t)currentHour;
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	OperationTime_GetOperationTimeAfterMaintenanceContinuous  			*/
/*                                                                                      */
/* Details:     get total continuous operation time after maintenance					*/
/*                                                                                      */
/* Arguments:	None																	*/
/*                                                                                      */
/* ReturnValue : uint32_t			                                                    */
/*                                                                                      */
/*                                                                                      */
/****************************************************************************************/
uint32_t OperationTime_GetOperationTimeAfterMaintenanceContinuous(void)
{
	float currentHour = tickToHour(gs_currOperationTimeAfterMaintenanceContinuousTick);
	return (uint32_t)currentHour;
}

