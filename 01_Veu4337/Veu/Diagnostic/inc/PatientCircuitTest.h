//*****************************************************************************/
//$COMMON.H$
//   File Name:  PatientCircuitTest.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: PatientCircuitTest
//      Verifies the patient circuit integrity following the change
//      of a patient circuit. This includes testing for leaks and a measurement 
//      of patient circuit compliance. 
//*****************************************************************************/

#ifndef PATIENTCIRCUITTEST_H
#define PATIENTCIRCUITTEST_H

#include "ChildTest.h"

class PatientCircuitTest : public ChildTest
{
public:
    PatientCircuitTest(SelfTest*);
    ~PatientCircuitTest() { }

    // events taken by this class
    void Start(void);
    void Cancel(void);

protected:
private:
    // prevent copying
    PatientCircuitTest(const PatientCircuitTest&);
    void operator=(PatientCircuitTest&);

    // state machine states
    void ST_Idle(void);
    void ST_ChargePatientCircuit(void);
    void ST_TestCircuitCompliance(void);
    void ST_CheckValve3(void);
    void ST_Complete(void);

    void DoStateAction(EventData* pData);
    
    // State enumeration order must match the order of state method pointers 
    // in stateMachine[] within DoStateAction().
    enum E_States 
    {
        eST_ChargePatientCircuit = ChildTest::eST_MaxStates,
        eST_TestCircuitCompliance,
        eST_CheckValve3,
        eST_MaxStates, 
        eST_EventIgnored,
        eST_CannotHappen
    };
    
    //$COMMON.ATTRIBUTE$
    //  Name: Pressure50_01
    //  Description: Pressure measured at 50 cmH20 0.1 seconds after being
    //      pressurized. 
    //  Units: cmH2O * SCALE
    //  Range: 
    SHORT Pressure50_01;

    //$COMMON.ATTRIBUTE$
    //  Name: Pressure50_11
    //  Description: Pressure measured at 50 cmH20 1.1 seconds after being
    //      pressurized. 
    //  Units: cmH2O * SCALE
    //  Range: 
//    SHORT Pressure50_11;
    
    //$COMMON.ATTRIBUTE$
    //  Name: Volume_Compl
    //  Description: The compliance of the patient circuit. Or in other words, 
    //      the volume of air it took to pressurize the patient circuit.
    //  Units: ml * SCALE
    //  Range: 
    LONG Volume_Compl;
    
    //$COMMON.ATTRIBUTE$
    //  Name: Pressure_Leak
    //  Description: The leak pressure being the difference between the 0.1S 
    //      and 1.1S pressures.
    //  Units: cmH2O * SCALE
    //  Range: 
    LONG Pressure_Leak;
    
    //$COMMON.ATTRIBUTE$
    //  Name: StartTime
    //  Description: The starting time stamp.
    //  Units: ticks
    //  Range: 
    UNSIGNED StartTime;
};

#endif //PATIENTCIRCUITTEST_H

