//*****************************************************************************/
//$COMMON.H$
//   File Name:  BlockPatientWyeTest.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: BlockPatientWyeTest
//      To verify the patient wye is blocked and the patient is disconnected 
//      from the instrument.
//*****************************************************************************/

#ifndef BLOCKPATIENTWYETEST_H
#define BLOCKPATIENTWYETEST_H

#include "ChildTest.h"

class BlockPatientWyeTest : public ChildTest
{
public:
    BlockPatientWyeTest(SelfTest*);
    ~BlockPatientWyeTest() { }

    // events taken by this class
    void Start(void);
    void Cancel(void);
    void Response(ResponseData*);

protected:
private:
    // prevent copying
    BlockPatientWyeTest(const BlockPatientWyeTest&);
    void operator=(BlockPatientWyeTest&);

    // state machine states
    void ST_Idle(void);
    void ST_FlowSensorsCalibration(void);
    void ST_UnblockPatientWye(void);
    void ST_VerifyPatientWyeUnblocked(ResponseData*);
    void ST_BlockPatientWye(void);
    void ST_VerifyPatientWyeBlocked(ResponseData*);
    void ST_TestAutoZeroSolenoids(void);
    void ST_PatientIsConnected(ResponseData*);

    void DoStateAction(EventData* pData);

    // State enumeration order must match the order of state method pointers 
    // in stateMachine[] within DoStateAction().
    enum E_States 
    {
        eST_FlowSensorsCalibration = ChildTest::eST_MaxStates,
        eST_UnblockPatientWye,
        eST_VerifyPatientWyeUnblocked,
        eST_BlockPatientWye,
        eST_VerifyPatientWyeBlocked,
        eST_TestAutoZeroSolenoids,
        eST_PatientIsConnected,
        eST_MaxStates,
        eST_EventIgnored,
        eST_CannotHappen
    };
    
    //$COMMON.ATTRIBUTE$
    //  Name: blockWyeAttempts
    //  Description: Tracks how many times the operator has tried to block the 
    //      the patient wye but the software determined there was a leak. 
    //  Units: 
    //  Range: 
    UCHAR blockWyeAttempts;

    //$COMMON.ATTRIBUTE$
    //  Name: FailureInfo
    //  Description: Used to save the failure information for a test criterion
    //  Units: 
    //  Range: 
    Result::Info FailureInfo;
};
#endif //BLOCKPATIENTWYETEST_H

