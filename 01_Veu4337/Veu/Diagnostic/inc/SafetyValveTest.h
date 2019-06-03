//*****************************************************************************/
//$COMMON.H$
//   File Name:  SafetyValve.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: SafetyValve
//      Tests the safety valve to ensure it is capable of opening up
//      to atmosphere.
//*****************************************************************************/

#ifndef SAFETYVALVETEST_H
#define SAFETYVALVETEST_H

#include "ChildTest.h"

class SafetyValveTest : public ChildTest
{
public:
    SafetyValveTest(SelfTest*);
    ~SafetyValveTest() { }

    // events taken by this class
    void Start(void);
    void Cancel(void);

protected:
private:
    // prevent copying
    SafetyValveTest(const SafetyValveTest&);
    void operator=(SafetyValveTest&);

    // state machine states
    void ST_ChargeCircuit(void);
    void ST_SafetyValve(void);

    void DoStateAction(EventData* pData);
    
    // State enumeration order must match the order of state method pointers 
    // in stateMachine[] within DoStateAction().
    enum E_States 
    {
        eST_ChargeCircuit = ChildTest::eST_MaxStates,
        eST_SafetyValve,
        eST_MaxStates,
        eST_EventIgnored,
        eST_CannotHappen
    };
};
#endif //SAFETYVALVETEST_H

