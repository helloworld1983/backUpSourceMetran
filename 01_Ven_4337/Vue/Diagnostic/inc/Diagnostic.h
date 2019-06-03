//*****************************************************************************/
//$COMMON.H$
//   File Name:  Diagnostic.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: Diagnostic
//      Encapsulates the entire diagnostics domain.
//*****************************************************************************/

#ifndef DIAGNOSTIC_H
#define DIAGNOSTIC_H

#include "Standard.h"
//#ifndef USE_NUCLEUS
//#include "DiagnosticGUI.h"
//#endif

// forward declaration eliminates include files thus speeding compile times
class SST;
class EST;
class HardwareDiagnostic;
class DiagnosticTask;
struct DiagnosticEventStruct;
struct SensorData;

class Diagnostic 
{
public:
    Diagnostic();
 
    E_Bool IsTesting(void);
    void GetSensorData(SensorData*);
    friend class DiagnosticTask;

#ifndef USE_NUCLEUS
    friend class DiagnosticGUI;
#endif

protected:
private:
    // prevent copying
    Diagnostic(const Diagnostic&);
    void operator=(Diagnostic&);
    
    void RouteMessage(DiagnosticEventStruct&);

    //$COMMON.ATTRIBUTE$
    //  Name: ShortSelfTest
    //  Description: Class to handle the SST.
    //  Units: 
    //  Range: 
    SST* const ShortSelfTest;

    //$COMMON.ATTRIBUTE$
    //  Name: ExtendedSelfTest
    //  Description: Class to handle the EST.
    //  Units: 
    //  Range: 
    EST* const ExtendedSelfTest;
    
    //$COMMON.ATTRIBUTE$
    //  Name: HardwareDiagnostic
    //  Description: Class to handle the hardware screen functionality.
    //  Units: 
    //  Range: 
    HardwareDiagnostic* const HW_Diagnostic;
};

Diagnostic& TheDiagnostic(void);

struct SensorData
{
    CHAR date[10];
    CHAR time[10];
    LONG airFlow;
    LONG oxygenFlow;
    LONG exhalationFlow;
    USHORT airPosition;
    USHORT oxygenPosition;
    USHORT exhalationPosition;
    SHORT oxygenSensor;
    E_Bool oxygenSupply;
    SHORT inhalationPressure;
    SHORT exhalationPressure;
    SHORT batteryVoltage;
    SHORT batteryCurrent;
    SHORT voltageReference;
    SHORT enclosureTemp;
    SHORT internalOxygen;
    SHORT ADC_WrapAI;
    E_Bool inhalationSol;
    E_Bool exhalationSol;
    E_Bool crossoverSol;
    E_Bool safetyValveSol;
    LONG altitudeCompensation;
    LONG circuitCompliance;
    LONG complianceOnOff;
};
#endif //DIAGNOSTIC_H



