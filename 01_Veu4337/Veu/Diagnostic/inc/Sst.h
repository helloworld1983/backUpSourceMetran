//*****************************************************************************/
//$COMMON.H$
//   File Name:  SST.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: SST
//      Class to execute the Short Self Tests (SST) for Diagnostic mode. 
//*****************************************************************************/

#ifndef SHORTSELFTEST_H                         
#define SHORTSELFTEST_H

#include "ParentTest.h"
#include "PatientCircuitTest.h"
#include "BlockPatientWyeTest.h"
#include "SafetyValveTest.h"

class SST : public ParentTest
{
//    ALLOCATOR_H_INTERFACE(SST, 1)
public:
    SST();
    ~SST() { }

    // events taken by this class
    void Start(void);
    void Cancel(void);
    void Response(ResponseData*);
    void Complete(CompleteData*);
    void Failure(FailureData*);

protected:
private:
    // prevent copying
    SST(const SST&);
    void operator=(SST&);

    // state machine states
    void ST_BlockPatientWyeTest(CompleteData*);
    void ST_SafetyValveTest(CompleteData*);
    void ST_PatientCircuitTest(CompleteData*);

    void DoStateAction(EventData*);
    
    int NextStateInSequence(int);
    
    // State enumeration order must match the order of state method pointers 
    // in stateMachine[] within DoStateAction().
    enum E_States 
    {
        eST_BlockPatientWyeTest = ParentTest::eST_MaxStates,
        eST_SafetyValveTest,
        eST_PatientCircuitTest,
        eST_MaxStates,
        eST_EventIgnored,
        eST_CannotHappen
    };

    //$COMMON.ATTRIBUTE$
    //  Name: patientCircuitTest
    //  Description: A child test performed by SST.
    //  Units: 
    //  Range: 
    PatientCircuitTest patientCircuitTest;

    //$COMMON.ATTRIBUTE$
    //  Name: blockPatientWyeTest
    //  Description: A child test performed by SST.
    //  Units: 
    //  Range: 
    BlockPatientWyeTest blockPatientWyeTest;

    //$COMMON.ATTRIBUTE$
    //  Name: safetyValveTest
    //  Description: A child test performed by SST.
    //  Units: 
    //  Range: 
    SafetyValveTest safetyValveTest;
};

SST& TheSST(void);

#endif //SHORTSELFTEST_H

