//*****************************************************************************/
//$COMMON.H$
//   File Name:  EST.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: EST
//      Class to execute the Extended Self Tests (EST) for Diagnostic mode.
//*****************************************************************************/

#ifndef EXTENDEDSELFTEST_H
#define EXTENDEDSELFTEST_H

#include "ParentTest.h"
#include "BlockPatientWyeTest.h"
#include "OxygenDeliveryTest.h"
#include "OxygenSensorTest.h"
#include "OxygenSupplyTest.h"
#include "AirDeliveryTest.h"
#include "CrossoverCircuitTest.h"
#include "BlowerTest.h"
#include "HeatedFilterTest.h"
#include "SafetyValveTest.h"
#include "PressureReliefValveTest.h"
#include "ExhalationValveTest.h"
#include "AudioTest.h"
#include "KeyboardTest.h"
#include "PatientCircuitTest.h"
//#include "Allocator.h"
#include "LEDTest.h"

class EST : public ParentTest
{
//    ALLOCATOR_H_INTERFACE(EST, 1)
public:
    EST();
    ~EST() { }

    void Start(void);
    void Cancel(void);
    void Response(ResponseData*);
    void Complete(CompleteData*);
    void Failure(FailureData*);

protected:
private:
    // prevent copying
    EST(const EST&);
    void operator=(EST&);

    // state machine states
    void ST_BlockPatientWyeTest(CompleteData*);
    void ST_SafetyValveTest(CompleteData*);
    void ST_OxygenSupplyTest(CompleteData*);
    void ST_OxygenDeliveryTest(CompleteData*);
    void ST_OxygenSensorTest(CompleteData*);
    void ST_AirDeliveryTest(CompleteData*);
    void ST_CrossoverCircuitTest(CompleteData*);
    void ST_BlowerTest(CompleteData*);
    void ST_PressureReliefValveTest(CompleteData*);
    void ST_ExhalationValveTest(CompleteData*);
    void ST_PatientCircuitTest(CompleteData*);
    void ST_HeatedFilterTest(CompleteData*);
    void ST_AudioTest(CompleteData*);
    void ST_KeyboardTest(CompleteData*);
    void ST_LEDTest(CompleteData*);

    void DoStateAction(EventData* pData);
                                          
    int NextStateInSequence(int);
    
    // State enumeration order must match the order of state method pointers 
    // in stateMachine[] within DoStateAction().
    enum E_States 
    {
        eST_BlockPatientWyeTest = ParentTest::eST_MaxStates,
        eST_SafetyValveTest,
        eST_BlowerTest,
        eST_OxygenSupplyTest,
        eST_CrossoverCircuitTest,
        eST_OxygenDeliveryTest,
        eST_OxygenSensorTest,
        eST_AirDeliveryTest,
        eST_PressureReliefValveTest,
        eST_ExhalationValveTest,
        eST_PatientCircuitTest,
        eST_HeatedFilterTest,
        eST_AudioTest,
        eST_LEDTest,
        eST_KeyboardTest,
        eST_MaxStates,
        eST_EventIgnored,
        eST_CannotHappen
    };

    //$COMMON.ATTRIBUTE$
    //  Name: patientCircuitTest
    //  Description: A child test performed by EST.
    //  Units: 
    //  Range: 
    BlockPatientWyeTest blockPatientWyeTest;

    //$COMMON.ATTRIBUTE$
    //  Name: safetyValveTest
    //  Description: A child test performed by EST.
    //  Units: 
    //  Range: 
    SafetyValveTest safetyValveTest;

    //$COMMON.ATTRIBUTE$
    //  Name: oxygenSupplyTest
    //  Description: A child test performed by EST.
    //  Units: 
    //  Range: 
    OxygenSupplyTest oxygenSupplyTest;

    //$COMMON.ATTRIBUTE$
    //  Name: oxygenDeliveryTest
    //  Description: A child test performed by EST.
    //  Units: 
    //  Range: 
    OxygenDeliveryTest oxygenDeliveryTest;

    //$COMMON.ATTRIBUTE$
    //  Name: oxygenSensorTest
    //  Description: A child test performed by EST.
    //  Units: 
    //  Range: 
    OxygenSensorTest oxygenSensorTest;

    //$COMMON.ATTRIBUTE$
    //  Name: airDeliveryTest
    //  Description: A child test performed by EST.
    //  Units: 
    //  Range: 
    AirDeliveryTest airDeliveryTest;

    //$COMMON.ATTRIBUTE$
    //  Name: crossoverCircuitTest
    //  Description: A child test performed by EST.
    //  Units: 
    //  Range: 
    CrossoverCircuitTest crossoverCircuitTest;

    //$COMMON.ATTRIBUTE$
    //  Name: blowerTest
    //  Description: A child test performed by EST.
    //  Units: 
    //  Range: 
    BlowerTest blowerTest;

    //$COMMON.ATTRIBUTE$
    //  Name: pressureReliefValveTest
    //  Description: A child test performed by EST.
    //  Units: 
    //  Range: 
    PressureReliefValveTest pressureReliefValveTest;

    //$COMMON.ATTRIBUTE$
    //  Name: exhalationValveTest
    //  Description: A child test performed by EST.
    //  Units: 
    //  Range: 
    ExhalationValveTest exhalationValveTest;

    //$COMMON.ATTRIBUTE$
    //  Name: patientCircuitTest
    //  Description: A child test performed by EST.
    //  Units: 
    //  Range: 
    PatientCircuitTest patientCircuitTest;
    
    //$COMMON.ATTRIBUTE$
    //  Name: heatedFilterTest
    //  Description: A child test performed by EST.
    //  Units: 
    //  Range: 
    HeatedFilterTest heatedFilterTest;
    
    //$COMMON.ATTRIBUTE$
    //  Name: audioTest
    //  Description: A child test performed by EST.
    //  Units: 
    //  Range: 
    AudioTest audioTest;

    //$COMMON.ATTRIBUTE$
    //  Name: keyboardTest
    //  Description: A child test performed by EST.
    //  Units: 
    //  Range: 
    KeyboardTest keyboardTest;

    //$COMMON.ATTRIBUTE$
    //  Name: ledTest
    //  Description: A child test performed by EST.
    //  Units: 
    //  Range: 
    LEDTest ledTest;
};

EST& TheEST(void);

#endif //EXTENDEDSELFTEST_H

