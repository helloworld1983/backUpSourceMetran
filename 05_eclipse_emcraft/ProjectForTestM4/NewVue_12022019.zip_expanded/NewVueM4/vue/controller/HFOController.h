/*
 * HFOController.h
 *
 *  Created on: 3 thg 1, 2019
 *      Author: haxua
 */

#ifndef CONTROLLER_HFOCONTROLLER_H_
#define CONTROLLER_HFOCONTROLLER_H_
#include <CommonTypes.h>
#include <IpcCommonTypes.h>
#include "stdbool.h"
#include "stdint.h"

void HFOController_SetSIRequest(bool SIRequest);
int32_t HFOController_GetFilterPressIntegrator();
int32_t HFOController_GetExhMap();
bool HFOController_GetHIMP();
bool HFOController_GetSIRequet();
int32_t HFOController_GetCurrentFreq();
void HFOController_SetSIType(E_SIType SI);
void HFOController_ResetHFOStaticVariables();
void HFOController_ResetSomeStaticVariables();
void HFOController_SetHFOAlarmInactiveTimer(uint16_t value);
uint16_t HFOController_GetHFOAlarmInactiveTimer();
void HFOController_SetConstFlow(bool constFlowFlag);
desiredMotorPositionStruct HFOController_Run(RealTimeM4ToA53*);
void HFOController_Enable();
void HFOController_Diable();
bool HFOController_IsEnable();
void HFOController_HFOPressControllerByExhValve();
void HFOController_GetHFOOper200Hours();
void HFOController_SetExhValveLimit(int32_t limitStep);
void HFOController_NotchFilter(void);
int32_t HFOController_MAPAirPDFControl( int32_t flowCmd, int32_t flowSensorVal);
int32_t HFOController_MAPO2PDFControl ( int32_t flowCmd, int32_t flowSensorVal) ;
int32_t HFOController_AmplitudeMinimizer(int32_t notchFilterInput, int32_t freq);
int32_t HFOController_Exh_AmplitudeMinimizer(int32_t notchFilterInput, int32_t freq);
void HFOController_PreFilterBlock();
void HFOController_PreProcessPresssureBlock();
int32_t HFOController_LeadCircuitBlock();
void HFOController_FlowCompensatorBlock();
void HFOController_PressFilter();
void HFOController_RampFreq(int32_t freq);
int16_t HFOController_AirFilter(int16_t u);
int16_t HFOController_O2Filter(int16_t u);
void  HFOController_CalculateHFOOperationTime();


#endif /* CONTROLLER_HFOCONTROLLER_H_ */
