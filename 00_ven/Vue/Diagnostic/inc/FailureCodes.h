//*****************************************************************************/
//$COMMON.H$
//   File Name:  FailureCodes.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: FailureCodes
//
//      A failure code is used to indicate a failure in the self test during 
//      diagnostic mode SST and EST.
//*****************************************************************************/

#ifndef FAILURECODES_H
#define FAILURECODES_H


// IMPORTANT: Existing failure code numberings should not change. These codes 
// are displayed to the user if a failure occurs and may be referenced in 
// the maintenance manual. Keeping a failure code for the duration of the 
// product prevents manual changes and customer confusion regarding the 
// failure code meanings. 
//
// When making a new diagnostic failure code follow these guidelines:
// 1) All failure codes start with eFC_.
// 2) The first part of the name should be the device.
// 3) The second half should be the problem.
// 4) Any new codes need to create a unique number. Do not let the compiler 
//    auto assign a value, explicitly create each new number.
// 5) If a code is no longer needed, comment it out. Commenting keeps it around 
//    as a a reminder not to reassign that number in the future.
//
// Naming example: 
//  eFC_InhValveStuck - 1) eFC_ is there, 2) the device in question is the 
//      inhalation valve, and 3) the problem is it is stuck.

enum E_FailureCodes
{                   
    eFC_None =                                      0,

    eFC_TestCanceledByUser =                        100, 

    eFC_InhStepperOutsideRange =                    101,
    eFC_O2StepperOutsideRange =                     102,
    eFC_InhFlowOutsideRange =                       103,
    eFC_O2FlowOutsideRange =                        104,
    eFC_ExhFlowOutsideRange =                       105,      
    eFC_PatientCircuitLeak =                        106,
    eFC_InhPressureTooLow =                         107,
    eFC_ExhPressureOutsideRange =                   108,
    eFC_CheckValve2Leaky =                          109,
    eFC_CheckValve3Leaky =                          110,
    eFC_O2NotConnected =                            111,
    eFC_O2NotDisconnected =                         112,
    eFC_O2Sensor100PercentSampleRange =             113,
    eFC_O2Sensor100PercentAvgRange =                114,
    eFC_PrimaryAudioNotSounding =                   115,
    eFC_BackupAudioNotSounding =                    116,
    eFC_CrossoverCircuitFault =                     117,
    eFC_BlowerNotOffSwitch =                        118,
    eFC_BlowerNotOffDAC =                           119,
	eFC_ReliefValveCrackFlowPressureTooHigh =       120,
    eFC_ReliefValveCrackFlowPressureNotStable =     121,
    eFC_ReliefValveCrackFlowPressureOutsideRange =  122,
    eFC_O2FlowCrackFlowOutsideRange =               123,
    eFC_O2FlowFullFlowOutsideRange =                124,
    eFC_DifferenceInhVsExh =                        125,
    eFC_DifferenceO2VsExh =                         126,
    eFC_HeatedFilterBackpressureRange =             127,
    eFC_CircuitComplianceRange =                    128,
    eFC_PressureLeakRange =                         129,
    eFC_SafeyValveCannotOpen =                      130,
    eFC_PatientWyeNotBlocked =                      131,
    eFC_KeyBroke =                                  132,
    eFC_RotaryKnobBroke =                           133,
    eFC_AirFlowSensorCannotCal =                    134,
    eFC_O2FlowSensorCannotCal =                     135,
    eFC_ExhFlowSensorCannotCal =                    136,
    eFC_AirFlowSensorVerifyCalFailure =             137,
    eFC_O2FlowSensorVerifyCalFailure =              138,
    eFC_ExhFlowSensorVerifyCalFailure =             139,
    eFC_FlowSensorsCannotEraseTable =               140,
    eFC_InhAutoZeroSolenoidCannotOpen =             141,
    eFC_ExhAutoZeroSolenoidCannotOpen =             142,
    eFC_InhStepperPositionOpenOutsideRange =        143,
    eFC_InhStepperPositionMidOutsideRange =         144,
    eFC_InhStepperPositionCloseOutsideRange =       145,
    eFC_O2StepperPositionOpenOutsideRange =         146,
    eFC_O2StepperPositionMidOutsideRange =          147,
    eFC_O2StepperPositionCloseOutsideRange =        148,
    eFC_ExhStepperPositionOpenOutsideRange =        149,
    eFC_ExhStepperPositionMidOutsideRange =         150,
    eFC_ExhStepperPositionCloseOutsideRange =       151,
    eFC_PatientWyeNotUnblocked =                    152,
    eFC_TouchscreenFailure =                        153,
    eFC_LEDIlluminationFailure =                    154,
    eFC_RemoteAlarmNotSounding =                    155
};

#endif //FAILURECODES_H

