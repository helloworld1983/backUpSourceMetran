#ifndef DEVICE_INC_DEVCTRL_H_
#define DEVICE_INC_DEVCTRL_H_

/********************************************************************/
/*                                                                  */
/* File Name    : DevCtrl.h             		                    */
/*                                                                  */
/* General       : This module contain function for init and handle */
/* all operation of Portable device									*/
/*                                                                  */
/* Product Name  : Portable                                         */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*           Dec 26, 2016      Thao Ha(MV)  	new file        	*/
/*                                                                  */
/********************************************************************/
#include <DualCoreCommon.h>
#include <DevTask.h>






/****************************************************************************************/
/*                                                                                      */
/* Function name:   devCtrl_Init                                                        */
/*                                                                                      */
/* Details: Init all specific device module                                             */
/*                                                                                      */
/* Arguments:   void                                                                    */
/*                                                                                      */
/* ReturnValue : None                                                                   */
/*                                                                                      */
/****************************************************************************************/
void devCtrl_Init(void);

/****************************************************************************************/
/*                                                                                      */
/* Function name:   devCtrl_Handle                                                      */
/*                                                                                      */
/* Details: Hanle device                                                                */
/*                                                                                      */
/* Arguments:   void                                                                    */
/*                                                                                      */
/* ReturnValue : None                                                                   */
/*                                                                                      */
/****************************************************************************************/
void devCtrl_Handle(void);

/****************************************************************************************/
/*                                                                                      */
/* Function name:   devCtrl_UpdateMonitorInfo                                           */
/*                                                                                      */
/* Details: Update monitor data                                                         */
/*                                                                                      */
/* Arguments:   void                                                                    */
/*                                                                                      */
/* ReturnValue : None                                                                   */
/*                                                                                      */
/****************************************************************************************/
void devCtrl_UpdateMonitorInfo(void);

/****************************************************************************************/
/*                                                                                      */
/* Function name:   devCtrl_UpdateAlarmMonitorStruct                                    */
/*                                                                                      */
/* Details: Update alarm monitor data                                                   */
/*                                                                                      */
/* Arguments:   void                                                                    */
/*                                                                                      */
/* ReturnValue : None                                                                   */
/*                                                                                      */
/****************************************************************************************/
void devCtrl_UpdateAlarmMonitorStruct(void);

/****************************************************************************************/
/*                                                                                      */
/* Function name:   devCtrl_SendMonitorInfoToGui                                        */
/*                                                                                      */
/* Details: Send monitor data to GUI                                                    */
/*                                                                                      */
/* Arguments:   void                                                                    */
/*                                                                                      */
/* ReturnValue : None                                                                   */
/*                                                                                      */
/****************************************************************************************/
void devCtrl_SendMonitorInfoToGui(void);

/****************************************************************************************/
/*                                                                                      */
/* Function name:   devCtrl_GetAlarmMonitorStruct                                       */
/*                                                                                      */
/* Details: Get Alarm Monitor Struct                                                    */
/*                                                                                      */
/* Arguments:   ALARM_MONITOR_t *storagePlace                                           */
/*                                                                                      */
/* ReturnValue : None                                                                   */
/*                                                                                      */
/****************************************************************************************/
void devCtrl_GetAlarmMonitorStruct(ALARM_MONITOR_t *storagePlace);

/****************************************************************************************/
/*                                                                                      */
/* Function name:   devCtrl_GetPCAppMonitorStruct                                       */
/*                                                                                      */
/* Details: Get PC Application Struct                                                    */
/*                                                                                      */
/* Arguments:   ALARM_MONITOR_t *storagePlace                                           */
/*                                                                                      */
/* ReturnValue : None                                                                   */
/*                                                                                      */
/****************************************************************************************/
void devCtrl_GetPCAppMonitorStruct(PCAPP_MONITOR_t *storagePlace);


/****************************************************************************************/
/*                                                                                      */
/* Function name: 	devCtrl_ProcessAlarmPacket							              	*/
/*                                                                                      */
/* Details: 	process USB packet receive from battery Task							*/
/*                                                                                      */
/* Arguments:	(I) DEV_EVENT_STRUCT alarmPacket										*/
/*                                                                                      */
/* ReturnValue : None			                                                        */
/*                                                                                      */
/****************************************************************************************/
void devCtrl_ProcessAlarmPacket(DEV_EVENT_STRUCT alarmPacket);


/****************************************************************************************/
/*                                                                                      */
/* Function name: 	devCtrl_ProcessUSBQueue							              		*/
/*                                                                                      */
/* Details: 	process USB packet receive from battery Task							*/
/*                                                                                      */
/* Arguments:	(I) DEV_EVENT_STRUCT usbPacket											*/
/*                                                                                      */
/* ReturnValue : None			                                                        */
/*                                                                                      */
/****************************************************************************************/
void devCtrl_ProcessUSBQueue(DEV_EVENT_STRUCT usbData);

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	devCtrl_ProcessLogPacket			         	     				*/
/*                                                                                      */
/* Details: Set current number of log													*/
/*                                                                                      */
/* Arguments:	(I)int16_t numberOfLog													*/
/*                                                                                      */
/* ReturnValue : None			                                                        */
/*                                                                                      */
/****************************************************************************************/
void devCtrl_ProcessLogPacket(DEV_EVENT_STRUCT logPacket);

/****************************************************************************************/
/*                                                                                      */
/* Function name:   devCtrl_SetCurrentNumberOfLog                                       */
/*                                                                                      */
/* Details: Set current number of log                                                   */
/*                                                                                      */
/* Arguments:   (I)int16_t numberOfLog                                                  */
/*                                                                                      */
/* ReturnValue : None                                                                   */
/*                                                                                      */
/****************************************************************************************/
void devCtrl_ProcessAudioPacket(DEV_EVENT_STRUCT audioPacket);

#endif /* DEVICE_INC_DEVCTRL_H_ */
