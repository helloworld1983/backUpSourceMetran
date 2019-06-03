/******************************************************************************/
//$COMMON.H$
//   File Name:  CommonTypes.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: 
//      This file contains no class declaration.  It is a repository for 
//      common ventilator specific types used throughout the system. 
//
//   Revision History:
//      Rev:  Date:     Engineer:           Project:
//      05    11/30/98  K.A. McGuigan       Morph
//      Description: SCR-402.  Added E_DualUnitsType enum.
//
//      Rev:  Date:     Engineer:           Project:
//      04    08/05/97  K.A. McGuigan       Morph
//      Description: Added E_PatientType enum.
//
//      Rev:  Date:     Engineer:           Project:
//      03    06/17/97  K.A. McGuigan       Morph
//      Description: Added eNoVentType to the E_VentilationType enum.
//
//      Rev:  Date:     Engineer:           Project:
//      02    06/14/97  K.A. McGuigan       Morph
//      Description: Changed the name of NonVentMode to NoGasMode and added
//          eNoVentilation to the E_BreathPhaseIndicator enum.
//
//      Rev:  Date:     Engineer:           Project:
//      01    01/28/97  K.A. McGuigan       Morph
//      Description: Initial Revision
//
/******************************************************************************/
#ifndef COMMONTYPES_H
#define COMMONTYPES_H
#include "Standard.h"

// Operator set Ventilation Type can be Volume, Pressure or Non-Invasive.
enum E_VentilationType	
{
	eVolumeVentilation, 
	ePressureVentilation, 
	eNonInvasiveVentilation,
	eNoVentType    
};


// Ventilation Mode includes a list of both operator set modes and alarm
// modes and a start-up mode.  
enum E_VentilationMode 
{
    eStartUpMode,           

    eFirstAlarmMode,
    eApneaMode = eFirstAlarmMode,  
    eOcclusionMode,

    // Add any new alarm modes here
	eHighPressMode,
	eDisconnectionMode,
    eNoGasMode,
#ifdef HFO_SYSTEM
	eFirstHFOMode,
	eHFOHighMAP_2Mode = eFirstHFOMode,
	eHFOExtremeLowMAPMode,
	eLastAlarmMode = eHFOExtremeLowMAPMode,
#else
	eLastAlarmMode = eNoGasMode,
#endif 

    // following are operator settable modes
#ifdef HFO_SYSTEM
	eHFOMode,
	eLastHFOMode = eHFOMode,
#endif
	eMixedMode, 
	eAssistControlMode,
	eSpontaneousMode,   

	eNumberOfModes		// must be last entry
};

// Patient Type setting
enum E_PatientType
{
    eAdult,
    ePediatric
};

// Trigger Type setting
enum E_TriggerType 
{
    eFlowTrigger, 
    ePressureTrigger,
    eNoTriggerType
};

// Valve Units setting 
enum E_DualUnitsType
{
    eSecondaryUnits,
    ePrimaryUnits,
    eNoDualUnits
};


// Flow pattern setting
enum E_FlowPattern 
{
    eSquare, 
    eRamp
};


// The Breath Phase Indicator is used to indicate breath phase and type.
enum E_BreathPhaseIndicator
{
    eSpontaneous,
    eAssisted,
    eSupported,
    eMandatory,
    ePlateau,
    eExhalation,
    eExhHold,
    eNoVentilation,
    eNoPhase
};


// Alarm level defines the exact level at which the alarm is being annunciated.
enum E_AlarmLevel
{
   	eHigh,
	eMedium,
	eLow,
	eAutoReset,
    eInactive,
    eLastAlarmLevel = eInactive
};


// Alarm status indicates if an alarm is active or not.
enum E_AlarmStatus
{
   	eActive = 0xBEEF,
    eNotActive = 0xDEAD
};


enum E_SettingType
	{
		eIMVSetting,
		eHFOSetting
	};


#endif // COMMONTYPES_H
