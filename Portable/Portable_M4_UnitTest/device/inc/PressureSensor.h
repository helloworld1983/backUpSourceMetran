#ifndef INC_PRESSURE_SENSOR_H_
#define INC_PRESSURE_SENSOR_H_
/********************************************************************/
/*                                                                  */
/* File Name    : PressureSensor.h                                  */
/*                                                                  */
/* General       : This module contain function for read pressure	*/
/* 				   sensor											*/
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


/****************************************************************************************/
/*                                                                                      */
/* Function name:   pressure_ReadProvidePressure                                        */
/*                                                                                      */
/* Details:     read provide tank pressure value                                        */
/*                                                                                      */
/* Arguments:   None                                                                    */
/*                                                                                      */
/* ReturnValue : float                                                                  */
/*                                                                                      */
/*                                                                                      */
/****************************************************************************************/
float pressure_ReadProvidePressure(void);

/****************************************************************************************/
/*                                                                                      */
/* Function name:   pressure_ReadTankPressure                                           */
/*                                                                                      */
/* Details:    read tank pressure value                                                 */
/*                                                                                      */
/* Arguments:   None                                                                    */
/*                                                                                      */
/* ReturnValue : float                                                                  */
/*                                                                                      */
/*                                                                                      */
/****************************************************************************************/
float pressure_ReadTankPressure(void);

/****************************************************************************************/
/*                                                                                      */
/* Function name:   pressure_ReadVacuumPressure                                         */
/*                                                                                      */
/* Details:    read vacuum pressure value                                               */
/*                                                                                      */
/* Arguments:   None                                                                    */
/*                                                                                      */
/* ReturnValue : float                                                                  */
/*                                                                                      */
/*                                                                                      */
/****************************************************************************************/
float pressure_ReadVacuumPressure(void);

/****************************************************************************************/
/*                                                                                      */
/* Function name:   pressure_ResetDiff                                                  */
/*                                                                                      */
/* Details:    Reset Differential Sensor                                                */
/* Arguments:   None                                                                    */
/*                                                                                      */
/* ReturnValue : None                                                                   */
/*                                                                                      */
/****************************************************************************************/
void pressure_ResetDiff(void);

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
//void pressure_InitDiff(void);

/****************************************************************************************/
/*                                                                                      */
/* Function name:   pressure_ReadDiff                                                   */
/*                                                                                      */
/* Details:   Get differential pressure value                                           */
/*                                                                                      */
/* Arguments:   (I)float* value                                                         */
/*              (O)float* value                                                         */
/* ReturnValue : bool                                                                   */
/*                                                                                      */
/* Local variables : None                                                               */
/*                                                                                      */
/****************************************************************************************/
bool pressure_ReadDiff(float*);

/****************************************************************************************/
/*                                                                                      */
/* Function name:   pressure_ChangeResolutionDiff                                       */
/*                                                                                      */
/* Details:   Change resolution of Differential Sensor to 9 bit                         */
/*                                                                                      */
/* Arguments:   NONE                                                                    */
/* ReturnValue : void                                                                   */
/*                                                                                      */
/****************************************************************************************/
void pressure_ChangeResolutionDiff();

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
//void pressure_Handle(float cannualaPress);

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
//bool pressure_IsInhale();

/****************************************************************************************/
/*                                                                                      */
/* Function name:   pressure_SetTriggerSensitivityLevel                                 */
/*                                                                                      */
/* Details:   Set trigger sensitivity level                                             */
/*                                                                                      */
/* Arguments:   (I) E_ID_TriggerSensitivity level                                       */
/* ReturnValue : void                                                                   */
/*                                                                                      */
/*                                                                                      */
/****************************************************************************************/
//void pressure_SetTriggerSensitivityLevel(E_ID_TriggerSensitivity);

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
//float pressure_GetTriggerPressure();

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
//void pressure_Stop();

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
//uint8_t pressure_GetBreathRate();

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
//void pressure_SetTestSynchronizeMode(uint8_t breathPerMinute);

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
//void pressure_ResetDetectBreathMode();

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
//void pressure_ResetTestSynchronizeMode();

#endif /* INC_PRESSURE_SENSOR_H_ */
