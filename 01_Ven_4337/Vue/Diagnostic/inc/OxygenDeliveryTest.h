//*****************************************************************************/
//$COMMON.H$
//   File Name:  OxygenDeliveryTest.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: OxygenDeliveryTest
//      To ensure the oxygen delivery system is operating correctly. 
//*****************************************************************************/

#ifndef OXYGENDELIVERYTEST_H
#define OXYGENDELIVERYTEST_H

#include "ChildTest.h"

class OxygenDeliveryTest : public ChildTest
{
public:
    OxygenDeliveryTest(SelfTest*);
    ~OxygenDeliveryTest() { }

    // events taken by this class
    void Start(void);
    void Cancel(void);

protected:
private:
    // prevent copying
    OxygenDeliveryTest(const OxygenDeliveryTest&);
    void operator=(OxygenDeliveryTest&);

    // state machine states
    void ST_Idle(void);
    void ST_StepperRangeOfMotion(void);
    void ST_OxygenFlowSymmetry(void);
    void ST_Complete(void);

    void DoStateAction(EventData* pData);
    
    // State enumeration order must match the order of state method pointers 
    // in stateMachine[] within DoStateAction().
    enum E_States 
    {
        eST_StepperRangeOfMotion = ChildTest::eST_MaxStates,
        eST_OxygenFlowSymmetry,
        eST_MaxStates,
        eST_EventIgnored,
        eST_CannotHappen
    };

    //$COMMON.ATTRIBUTE$
    //  Name: Passed
    //  Description: A flag that is set true if all tests within this class
	//		pass, false otherwise.
    //  Units: 
    //  Range: 
	E_Bool Passed;
};
#endif //OXYGENDELIVERYTEST_H

