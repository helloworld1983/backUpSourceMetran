//*****************************************************************************/
//$COMMON.H$
//   File Name:  ExhalationValveTest.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: ExhalationValveTest
//      Tests for correct operation of the exhalation system by commanding
//      the system to control backpressure at various levels.
//*****************************************************************************/

#ifndef EXHALATIONVALVETEST_H
#define EXHALATIONVALVETEST_H

#include "ChildTest.h"

class ExhalationValveTest : public ChildTest
{
public:
    ExhalationValveTest(SelfTest*);
    ~ExhalationValveTest() { }

    // events taken by this class
    void Start(void);
    void Cancel(void);

protected:
private:
    // prevent copying
    ExhalationValveTest(const ExhalationValveTest&);
    void operator=(ExhalationValveTest&);
        
    // state machine states
    void ST_StepperRangeOfMotion(void);
    void ST_ControlBackpressure(void);

    void DoStateAction(EventData* pData);
    
    // State enumeration order must match the order of state method pointers 
    // in stateMachine[] within DoStateAction().
    enum E_States 
    {
        eST_StepperRangeOfMotion = ChildTest::eST_MaxStates,
        eST_ControlBackpressure,
        eST_MaxStates,
        eST_EventIgnored,
        eST_CannotHappen
    };
};
#endif //EXHALATIONVALVETEST_H

