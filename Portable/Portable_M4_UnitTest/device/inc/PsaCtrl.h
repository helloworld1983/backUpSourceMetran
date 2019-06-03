/********************************************************************/
/*                                                                  */
/* File Name    : PsaCtrl.h		        	                        */
/*                                                                  */
/* General       : This module contain function for init and handle */
/* 				   psa device based on psa parameter				*/
/*                                                                  */
/* Product Name  : Portable                                         */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*            Jun 30, 2016      Thao Ha(MV)	  		new file        */
/*                                                                  */
/********************************************************************/

#ifndef DEVICE_INC_PSACTRL_H_
#define DEVICE_INC_PSACTRL_H_

#include <DualCoreCommon.h>
#include <TaskCommon.h>

#define TIME_15S_COUNT 15000/DEVICE_TASK_DELAY_TIME
#define TIME_300MS_COUNT 300/DEVICE_TASK_DELAY_TIME
#define TIME_800MS_COUNT 800/DEVICE_TASK_DELAY_TIME

/****************************************************************************************/
/*                                                                                      */
/* Function name:   psa_Handle                                                          */
/*                                                                                      */
/* Details:     This function is called every 20ms to handle psa operation              */
/*                                                                                      */
/* Arguments:   (I) float productTankPress                                              */
/*              (I) uint16_t compBoardTemp                                              */
/* ReturnValue : void                                                                   */
/*                                                                                      */
/****************************************************************************************/
void psa_Handle(float,uint16_t);

/****************************************************************************************/
/*                                                                                      */
/* Function name:   psa_Start                                                           */
/*                                                                                      */
/* Details:     This function is called when psa function run first time                */
/*                                                                                      */
/* Arguments:   (I) None                                                                */
/*              (O) None                                                                */
/* ReturnValue : void                                                                   */
/*                                                                                      */
/****************************************************************************************/
void psa_Start(void);

/****************************************************************************************/
/*                                                                                      */
/* Function name:   psa_CreateTimer                                                     */
/*                                                                                      */
/* Details:     Create all timer for Key_panel module                                   */
/*                                                                                      */
/* Arguments:   (I) None                                                                */
/*              (O) None                                                                */
/* ReturnValue : void                                                                   */
/*                                                                                      */
/****************************************************************************************/
void psa_CreateTimer(void);

/****************************************************************************************/
/*                                                                                      */
/* Function name:   psa_setMaxPressure                                                  */
/*                                                                                      */
/* Details:     Set max pressure for valve switching                                    */
/*                                                                                      */
/* Arguments:   (I) uint16_t pressure                                                   */
/*              (O) None                                                                */
/* ReturnValue : void                                                                   */
/*                                                                                      */
/****************************************************************************************/
void psa_setMaxPressure(uint16_t);

/****************************************************************************************/
/*                                                                                      */
/* Function name:   psa_setPVSATime                                                     */
/*                                                                                      */
/* Details:     Set max PVSA time for valve switching                                   */
/*                                                                                      */
/* Arguments:   (I) double time                                                         */
/*              (O) None                                                                */
/* ReturnValue : void                                                                   */
/*                                                                                      */
/****************************************************************************************/
void psa_setPVSATime(double);

/****************************************************************************************/
/*                                                                                      */
/* Function name:   psa_setSol2OpenATime                                                */
/*                                                                                      */
/* Details:     Set valve 2 Open Time A                                                 */
/*                                                                                      */
/* Arguments:   (I) double time                                                         */
/*              (O) None                                                                */
/* ReturnValue : void                                                                   */
/*                                                                                      */
/****************************************************************************************/
void psa_setSol2OpenATime(double);

/****************************************************************************************/
/*                                                                                      */
/* Function name:   psa_setSol2OpenBTime                                                */
/*                                                                                      */
/* Details:     Set valve 2 Open Time B                                                 */
/*                                                                                      */
/* Arguments:   (I) double time                                                         */
/*              (O) None                                                                */
/* ReturnValue : void                                                                   */
/*                                                                                      */
/****************************************************************************************/
void psa_setSol2OpenBTime(double);

void psa_setSol1DelayOpenTime(double time);

void psa_setSol3DelayOpenTime(double time);
/****************************************************************************************/
/*                                                                                      */
/* Function name:   psa_setSol1OnOffsetTime                                             */
/*                                                                                      */
/* Details:     Set valve 1 On Offset Time                                              */
/*                                                                                      */
/* Arguments:   (I) double time                                                         */
/*              (O) None                                                                */
/* ReturnValue : void                                                                   */
/*                                                                                      */
/****************************************************************************************/
void psa_setSol1OnOffsetTime(double);

/****************************************************************************************/
/*                                                                                      */
/* Function name:   psa_setSol3OnOffsetTime                                             */
/*                                                                                      */
/* Details:     Set valve 3 On Offset Time                                              */
/*                                                                                      */
/* Arguments:   (I) double time                                                         */
/*              (O) None                                                                */
/* ReturnValue : void                                                                   */
/*                                                                                      */
/****************************************************************************************/
void psa_setSol3OnOffsetTime(double);

/****************************************************************************************/
/*                                                                                      */
/* Function name:   psa_select                                                          */
/*                                                                                      */
/* Details:     Select PVSA table based on request flow, current mode and current       */
/*              compressor board temperature                                            */
/* Arguments:   (I) uint8_t flowRequest                                                 */
/*              (I) E_ModeSetting mode                                                  */
/* ReturnValue : void                                                                   */
/*                                                                                      */
/****************************************************************************************/
void psa_select(uint8_t flowRequest,E_ModeSetting mode);

/****************************************************************************************/
/*                                                                                      */
/* Function name:   psa_isThreeSecondFinish                                             */
/*                                                                                      */
/* Details:     Check whether PSA has functioned for 3 second                           */
/* Arguments:   (I) None                                                                */
/*              (O) None                                                                */
/* ReturnValue : bool                                                                   */
/*                                                                                      */
/****************************************************************************************/
bool psa_isThreeSecondFinish(void);

/****************************************************************************************/
/*                                                                                      */
/* Function name:   psa_isWarmUpFinish                                                  */
/*                                                                                      */
/* Details:     Check whether Warm up process is finished                               */
/* Arguments:   (I) None                                                                */
/*              (O) None                                                                */
/* ReturnValue : bool                                                                   */
/*                                                                                      */
/****************************************************************************************/
bool psa_isWarmUpFinish(void);

/****************************************************************************************/
/*                                                                                      */
/* Function name:   psa_Stop                                                            */
/*                                                                                      */
/* Details:     Stop PSA function                                                       */
/* Arguments:   (I) None                                                                */
/*              (O) None                                                                */
/* ReturnValue : void                                                                   */
/*                                                                                      */
/****************************************************************************************/
void psa_Stop(void);

/****************************************************************************************/
/*                                                                                      */
/* Function name:   psa_GetCurrentPSATemperature                                        */
/*                                                                                      */
/* Details:     Get current temperture state of PSA function                            */
/* Arguments:   (I) None                                                                */
/*              (O) None                                                                */
/* ReturnValue : E_PSATemperature                                                       */
/*                                                                                      */
/****************************************************************************************/
E_PSATemperature psa_GetCurrentPSATemperature();

#endif /* DEVICE_INC_PSACTRL_H_ */
