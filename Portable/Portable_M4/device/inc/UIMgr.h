#ifndef DEVICE_INC_UIMGR_H_
#define DEVICE_INC_UIMGR_H_
/********************************************************************/
/*                                                                  */
/* File Name    : UIMgr.h                        				    */
/*                                                                  */
/* General      : contain function to update UI                     */
/*                 	 						                        */
/*                                                                  */
/* Product Name  : Portable                                         */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*       April 18, 2017     Thao Ha(MV)  		new file        	*/
/*                                                                  */
/********************************************************************/
#include "board.h"


/****************************************************************************************/
/*                                                                                      */
/* Function name:   UIMgr_Handle                                                        */
/*                                                                                      */
/* Details:        UI handle function is call for checking and updating UI every 20ms   */
/*                                                                                      */
/* Arguments:   None                                                                    */
/*                                                                                      */
/* ReturnValue : None                                                                   */
/*                                                                                      */
/****************************************************************************************/
void UIMgr_Handle(uint16_t remainingPercentage,bool isBatteryCharging);

/****************************************************************************************/
/*                                                                                      */
/* Function name:   UIMgr_UpdateMode                                                    */
/*                                                                                      */
/* Details:       Update UI base on current mode                                        */
/*                                                                                      */
/* Arguments:   None                                                                    */
/*                                                                                      */
/* ReturnValue : void                                                                   */
/*                                                                                      */
/****************************************************************************************/
void UIMgr_UpdateMode();

/****************************************************************************************/
/*                                                                                      */
/* Function name:   UIMgr_UpdateBattery                                                 */
/*                                                                                      */
/* Details:       Update UI base on current battery status                              */
/*                                                                                      */
/* Arguments:   None                                                                    */
/*                                                                                      */
/* ReturnValue : void                                                                   */
/*                                                                                      */
/****************************************************************************************/
void UIMgr_UpdateBattery();

/****************************************************************************************/
/*                                                                                      */
/* Function name:   UIMgr_SetPowerOffFlag                                               */
/*                                                                                      */
/* Details:       Set Power off flag                                                    */
/*                                                                                      */
/* Arguments:   (I) bool state                                                          */
/*                                                                                      */
/* ReturnValue : void                                                                   */
/*                                                                                      */
/****************************************************************************************/
void UIMgr_SetPowerOffFlag(bool state);

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	UIMgr_DisableDisplayBatteryPercentWhenShutdown						*/
/*                                                                                      */
/* Details:       Disable display battery Percent when shutdown	Portable				*/
/*                                                                                      */
/* Arguments:	(I) void																*/
/*                                                                                      */
/* ReturnValue : void			                                                        */
/*                                                                                      */
/****************************************************************************************/
void UIMgr_DisableDisplayBatteryPercentWhenShutdown();
/****************************************************************************************/
/*                                                                                      */
/* Function name:   UIMgr_GetCurrentMode                                                */
/*                                                                                      */
/* Details:        Get displaying mode on UI                                            */
/*                                                                                      */
/* Arguments:   None                                                                    */
/*                                                                                      */
/* ReturnValue : int                                                                    */
/*                                                                                      */
/****************************************************************************************/
int UIMgr_GetCurrentMode(void);

/****************************************************************************************/
/*                                                                                      */
/* Function name:   UIMgr_GetCurrentFlowrate                                            */
/*                                                                                      */
/* Details:        Get displaying flowrate on UI                                        */
/*                                                                                      */
/* Arguments:   None                                                                    */
/*                                                                                      */
/* ReturnValue : int                                                                    */
/*                                                                                      */
/****************************************************************************************/
int UIMgr_GetCurrentFlowrate(void);

/****************************************************************************************/
/*                                                                                      */
/* Function name:   UIMgr_isACExist                                                     */
/*                                                                                      */
/* Details:        check whether AC exist                                               */
/*                                                                                      */
/* Arguments:   None                                                                    */
/*                                                                                      */
/* ReturnValue : bool                                                                   */
/*                                                                                      */
/****************************************************************************************/
bool UIMgr_isACExist(void);

#endif /* DEVICE_INC_UIMGR_H_ */
