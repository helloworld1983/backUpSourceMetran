
#ifndef DEVICE_INC_BREATHTRIGGER_H_
#define DEVICE_INC_BREATHTRIGGER_H_

/********************************************************************/
/*                                                                  */
/* File Name    : BreathTrigger.h                                   */
/*                                                                  */
/* General       : This module contain function to handle breath	*/
/* 				   trigger											*/
/*                                                                  */
/*                 	 						                        */
/*                                                                  */
/* Product Name  : Portable                                         */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             April 18, 2017  Thao Ha(MV)  		new file        */
/*                                                                  */
/********************************************************************/
typedef enum {
	eTriggerSensitivityLevel1=1,
	eTriggerSensitivityLevel2,
	eTriggerSensitivityLevel3,
}E_ID_TriggerSensitivity;



/****************************************************************************************/
/*                                                                                      */
/* Function name:   pressure_initDiff                                                   */
/*                                                                                      */
/* Details:    Initialize differential pressure by set resolution of Differential       */
/*          sensor and set global variable                                              */
/* Arguments:   None                                                                    */
/*                                                                                      */
/* ReturnValue : None                                                                   */
/*                                                                                      */
/****************************************************************************************/
void breathTrigger_InitTrigger(void);



/****************************************************************************************/
/*                                                                                      */
/* Function name:   pressure_Handle                                                     */
/*                                                                                      */
/* Details:   pressure handle function is call for checking whether user is inhaling to */
/*           make the flow controller open every 20ms                                   */
/*            calculate breath rate                                                     */
/* Arguments:   (I) float cannualaPress                                                 */
/* ReturnValue : void                                                                   */
/*                                                                                      */
/* Local variables : None                                                               */
/*                                                                                      */
/****************************************************************************************/
void breathTrigger_Handle(float cannualaPress);

/****************************************************************************************/
/*                                                                                      */
/* Function name:   pressure_IsInhale                                                   */
/*                                                                                      */
/* Details:   Check whether user is inhaling                                            */
/*                                                                                      */
/* Arguments:   None                                                                    */
/* ReturnValue : bool                                                                   */
/*                                                                                      */
/*                                                                                      */
/****************************************************************************************/
bool breathTrigger_IsInhale();

/****************************************************************************************/
/*                                                                                      */
/* Function name:   breathTrigger_UpdateTriggerSensitivityLevel                         */
/*                                                                                      */
/* Details:   Set trigger sensitivity level                                             */
/*                                                                                      */
/* Arguments:   (I) none						                                        */
/* ReturnValue : void                                                                   */
/*                                                                                      */
/*                                                                                      */
/****************************************************************************************/
void breathTrigger_UpdateTriggerSensitivityLevel();

/****************************************************************************************/
/*                                                                                      */
/* Function name:   pressure_GetTriggerPressure                                         */
/*                                                                                      */
/* Details:   get latest trigger pressure                                               */
/*                                                                                      */
/* Arguments:   (I) NONE                                                                */
/* ReturnValue : Float                                                                  */
/*                                                                                      */
/* Local variables : None                                                               */
/*                                                                                      */
/****************************************************************************************/
float breathTrigger_GetTriggerPressure();

/****************************************************************************************/
/*                                                                                      */
/* Function name:   pressure_Stop                                                       */
/*                                                                                      */
/* Details:   Stop detect breath                                                        */
/*                                                                                      */
/* Arguments:   (I) NONE                                                                */
/* ReturnValue : NONE                                                                   */
/*                                                                                      */
/* Local variables : None                                                               */
/*                                                                                      */
/****************************************************************************************/
void breathTrigger_Stop();

/****************************************************************************************/
/*                                                                                      */
/* Function name:   pressure_Stop                                                       */
/*                                                                                      */
/* Details:   Stop detect breath                                                        */
/*                                                                                      */
/* Arguments:   (I) NONE                                                                */
/* ReturnValue : NONE                                                                   */
/*                                                                                      */
/* Local variables : None                                                               */
/*                                                                                      */
/****************************************************************************************/
uint8_t breathTrigger_GetBreathRate();

/****************************************************************************************/
/*                                                                                      */
/* Function name:   pressure_SetTestSynchronizeMode                                     */
/*                                                                                      */
/* Details:   Set BPM of Synchronize Test Mote                                          */
/*                                                                                      */
/* Arguments:   (I) uint8_t breathPerMinute                                             */
/* ReturnValue : void                                                                   */
/*                                                                                      */
/****************************************************************************************/
void breathTrigger_SetTestSynchronizeMode(uint8_t breathPerMinute);

/****************************************************************************************/
/*                                                                                      */
/* Function name:   pressure_ResetDetectBreathMode                                      */
/*                                                                                      */
/* Details:   Reset all counter in Detect Breath Mode                                   */
/*                                                                                      */
/* Arguments:   (I) None                                                                */
/*              (O) None                                                                */
/* ReturnValue : void                                                                   */
/*                                                                                      */
/****************************************************************************************/
void breathTrigger_ResetDetectBreathMode();

/****************************************************************************************/
/*                                                                                      */
/* Function name:   pressure_ResetTestSynchronizeMode                                   */
/*                                                                                      */
/* Details:   Reset all counter in Test Synchronize Mode                                */
/*                                                                                      */
/* Arguments:   (I) None                                                                */
/*              (O) None                                                                */
/* ReturnValue : void                                                                   */
/*                                                                                      */
/****************************************************************************************/
void breathTrigger_ResetTestSynchronizeMode();

#endif /* DEVICE_INC_BREATHTRIGGER_H_ */
