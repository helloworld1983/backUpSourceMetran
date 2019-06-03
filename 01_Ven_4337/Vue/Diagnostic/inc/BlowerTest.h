//*****************************************************************************/
//$COMMON.H$
//   File Name:  BlowerTest.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: BlowerTest
//      Tests whether the blower is capable of being shut off by software.
//*****************************************************************************/

#ifndef BLOWERTEST_H
#define BLOWERTEST_H

#include "ChildTest.h"

class BlowerTest : public ChildTest
{
public:
    BlowerTest(SelfTest*);
    ~BlowerTest() { }

    // events taken by this class
    void Start(void);
    void Cancel(void);

protected:
private:
    // prevent copying
    BlowerTest(const BlowerTest&);
    void operator=(BlowerTest&);

    // state machine states
    void ST_BlowerShutoffSwitch(void);
    void ST_BlowerShutoffDAC(void);

    void DoStateAction(EventData* pData);
    
    // State enumeration order must match the order of state method pointers 
    // in stateMachine[] within DoStateAction().
    enum E_States 
    {
        eST_BlowerShutoffSwitch = ChildTest::eST_MaxStates,
        eST_BlowerShutoffDAC,
        eST_MaxStates,
        eST_EventIgnored,
        eST_CannotHappen
    };

    static const LONG BLOWER_SPINDOWN_FLOW;
};
#endif //BLOWERTEST_H

