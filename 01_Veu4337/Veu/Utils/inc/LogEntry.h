//******************************************************************************
//$COMMON.H$
//   File Name:  LogEntry.H
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: LogEntry - this structure defines the format of an Event Log entry. 
//
//   Restriction:
//      *** When adding new log entries, be sure to add to the end of a section.
//          DO NOT RE-ASSIGN ANY LOG ENTRY ID NUMBERS.
//******************************************************************************
#ifndef LOG_ENTRY_H
#define LOG_ENTRY_H

#include "commontypes.h"

// EventCode Errors.
//      *** When adding new log entries, be sure to add to the end of a section.
//          DO NOT RE-ASSIGN ANY LOG ENTRY ID NUMBERS.
enum E_EventCode
{
    eSystemBase = 0, eEmptyRecord = 0, eVentRestart, // Normal system restart event
    eVentShutdown,          // Power Fail NMI event
    eDiagnosticStartup,     // System run in Diagnostic mode
    eUnknownRestart,        // Misc. anomaly, system reset event
    ePostBase = 1000,       // Vent Inop Conditions during Power On Self Test
    eBusActivityMonitor,            // 1001
    eWatchDogTimer,                 // 1002
    eProcessorTest,                 // 1003
    eOtpCrc,                        // 1004
    eFlashCrc,                      // 1005
    eInternalRam,                   // 1006
    eExternalRam,                   // 1007
    eMaxSystemResets,               // 1008
    ePostInhAzTest,                 // 1009
    ePostExhAzTest,                 // 1010
    eCompatibility,                 // 1011
    ePOSTPowerSupply,               // 1012
    eSST_Base = 2000,
    eSST_PassedCode = eSST_Base,    // 2000
    eEST_Base = 3000,
    eEST_PassedCode = eEST_Base,    // 3000
    eCBITBase = 4000,
    eCbitSupplyPressTestCode,       // 4001
    eCbitRamTestCode,               // 4002
    eCbitAdcDacTestCode,            // 4003
    eCbitInhPAutoZeroCode,          // 4004
    eCbitExhPAutoZeroCode,          // 4005
    eCbitHighTempTestCode,          // 4006
    eCbitExhValveOpenTestCode,      // 4007
    eCbitAirValveOpenTestCode,      // 4008
    eCbitO2ValveOpenTestCode,       // 4009
    eCbitAirValveClosedTestCode,    // 4010
    eCbitO2ValveClosedTestCode,     // 4011
    eCbitExhValveClosedTestCode,    // 4012
    eCbitOnChipCrcTestCode,         // 4013
    eCbitProgramCrcTestCode,        // 4014
    eCbitAlarmWrapTestCode,         // 4015
    eCbitO2SupplyPressTestCode,     // 4016
    eCbitBatteryTestCode,           // 4017
    eCbitFlowTableCrcTestCode,      // 4018
    eCbitAirMotorError,             // 4019
    eCbitO2MotorError,              // 4020
    eCbitExhMotorError,             // 4021

    ePowerFailError = 7000,

    eSoftware = 8000,
    eRecoverableAnomaly,            // 8001
    eAlarmCorruptStatus,            // 8002
    eVentInop = 9000,
    eFlashProgrammingError,         // 9001
    eFlowSensorMismatch,            // 9002   flow tables and sensor don't match
    eFlowSensorFailure,             // 9003
    ePressureSensorFailure,         // 9004
    eSensorFailure,                 // 9005
    ePowerSupplyError,              // 9006
    eWrongSensorModelNumInO2,       // 9007   unexpected sensor in O2 spot
    eWrongSensorModelNumInAirOrExh, // 9008   unexpected sensor in air or exh spot
    eFlowSensCalDataOutOfRange      // 9009   Flow Sens EEPROM data out of range
};

class TimeRecord
{
    public:
        UCHAR Seconds;
        UCHAR Minutes;
        UCHAR Hours;
        UCHAR Day;
        UCHAR Month;
        UCHAR Year;
        UCHAR Status;         // stores the AM/PM and military time bits
};

//
// LogEntry class definition
//
class LogEntry
{
    public:
        UCHAR EntryNumber;   // Keeps order of entries
        UCHAR Consecutive;
        TimeRecord TimeStamp;     // records time of entry  
        E_EventCode Id;            // uniquely identifies error
        char * AdditionalInfo; // free format supplemental info
};

//
// Event Log related constants
//
static const USHORT ADDL_INFO_SIZE = 120;   // 120 bytes of ASCII data
static const USHORT LOG_ENTRY_SIZE = (sizeof(LogEntry) - sizeof(char*))
        + ADDL_INFO_SIZE;

#endif  // LOG_ENTRY_H

