//*****************************************************************************/
//$COMMON.H$
//   File Name:  OxygenSensorTest.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: OxygenSensorTest
//      To calibrate the O2 sensor using 100% O2. 
//*****************************************************************************/

#ifndef OXYGENSENSORTEST_H
#define OXYGENSENSORTEST_H

#include "ChildTest.h"

class OxygenSensorTest : public ChildTest
{
public:
    OxygenSensorTest(SelfTest*);
    ~OxygenSensorTest() { }

    // events taken by this class
    void Start(void);
    void Cancel(void);
    void Response(ResponseData*);
    void RetestInit(void);

protected:
private:
    // prevent copying
    OxygenSensorTest(const OxygenSensorTest&);
    void operator=(OxygenSensorTest&);

    // state machine states
    void ST_O2SensorPresent(void);
    void ST_O2SensorCalibration(void);
    void ST_TestFailure(ResponseData* pData);

    void DoStateAction(EventData* pData);
    
    // State enumeration order must match the order of state method pointers 
    // in stateMachine[] within DoStateAction().
    enum E_States 
    {
        eST_O2SensorPresent = ChildTest::eST_MaxStates,
        eST_O2SensorCalibration,
        eST_TestFailure,
        eST_MaxStates,
        eST_EventIgnored,
        eST_CannotHappen
    };

    //$COMMON.ATTRIBUTE$
    //  Name: FailureInfo
    //  Description: Used to save the failure information for a test criterion
    //  Units: 
    //  Range: 
    Result::Info FailureInfo;
};
#endif //OXYGENSENSORTEST_H

