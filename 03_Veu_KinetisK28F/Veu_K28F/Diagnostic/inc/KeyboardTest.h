//*****************************************************************************/
//$COMMON.H$
//   File Name:  KeyboardTest.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: KeyboardTest
//      To verify the front panel keyboard and rotary knob functions
//      correctly. 
//*****************************************************************************/

#ifndef KEYBOARDTEST_H
#define KEYBOARDTEST_H

#include "ChildTest.h"

class KeyboardTest : public ChildTest
{
public:
    KeyboardTest(SelfTest* parent);
    ~KeyboardTest() { }

    // events taken by this class
    void Start(void);
    void Cancel(void);
    void Response(ResponseData*);

protected:
private:
    // prevent copying
    KeyboardTest(const KeyboardTest&);
    void operator=(KeyboardTest&);

    // state machine states
    void ST_Idle(void);
    void ST_AskForFailedBeams(void);
    void ST_VerifyNoFailedBeams(ResponseData*);
    void ST_PressAlarmSilence(void);
    void ST_PressAlarmReset(ResponseData*);
    void ST_PressAllO2(ResponseData*);
    void ST_PressManInsp(ResponseData*);
    void ST_PressScreenLock(ResponseData*);
    void ST_PressExpHold(ResponseData*);
    void ST_PressOptionLeft(ResponseData*);
    void ST_PressOptionRight(ResponseData*);
    void ST_PressRotaryKnobAccept(ResponseData*);
    void ST_TurnRotaryKnob(ResponseData*);
    void ST_VerifyRotaryKnobTurned(ResponseData*);

    void DoStateAction(EventData* pData);

    // State enumeration order must match the order of state method pointers 
    // in stateMachine[] within DoStateAction().
    enum E_States 
    {
        eST_AskForFailedBeams = ChildTest::eST_MaxStates,
        eST_VerifyNoFailedBeams,
        eST_PressAlarmSilence,
        eST_PressAlarmReset,
        eST_PressAllO2,
        eST_PressManInsp,
        eST_PressScreenLock,
        eST_PressExpHold,
        eST_PressOptionLeft,
        eST_PressOptionRight,
        eST_PressRotaryKnobAccept,
        eST_TurnRotaryKnob,
        eST_VerifyRotaryKnobTurned,
        eST_MaxStates,
        eST_EventIgnored,
        eST_CannotHappen
    };
};
#endif //KEYBOARDTEST_H

