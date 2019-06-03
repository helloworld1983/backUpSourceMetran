//*****************************************************************************/
//$COMMON.H$
//   File Name:  AirDeliveryTest.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: AirDeliveryTest
//      To ensure the air delivery system is operating correctly. 
//*****************************************************************************/

#ifndef AIRDELIVERYTEST_H
#define AIRDELIVERYTEST_H

#include "ChildTest.h"

class AirDeliveryTest : public ChildTest
{
public:
    AirDeliveryTest(SelfTest*);
    ~AirDeliveryTest() { }

    // events taken by this class
    void Start(void);
    void Cancel(void);

protected:
private:
    // prevent copying
    AirDeliveryTest(const AirDeliveryTest&);
    void operator=(AirDeliveryTest&);

    // state machine states
    void ST_StepperRangeOfMotion(void);
    void ST_AirFlowSymmetry(void);

    void DoStateAction(EventData* pData);
    
    // State enumeration order must match the order of state method pointers 
    // in stateMachine[] within DoStateAction().
    enum E_States 
    {
        eST_StepperRangeOfMotion = ChildTest::eST_MaxStates,
        eST_AirFlowSymmetry,
        eST_MaxStates,
        eST_EventIgnored,
        eST_CannotHappen
    };
};
#endif //AIRDELIVERYTEST_H

