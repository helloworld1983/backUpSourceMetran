//******************************************************************************
//$COMMON.H$
//    File Name: LEDTest.h
//    Copyright 1997 InVentived Technologies, Inc. All Rights Reserved.
//
//    Class:  LEDTest
//      To verify the front panel LEDs function correctly. 
//******************************************************************************
#ifndef    LEDTEST_H
#define    LEDTEST_H

#include "ChildTest.h"

//#ifndef NO_HARDWARE
//    #include "LED.h"
//#endif

//
//  Led Test Class Definition
//
class LEDTest : public ChildTest
{
public:
    LEDTest(SelfTest* parent);
    ~LEDTest() { }

    // events taken by this class
    void Start(void);
    void Cancel(void);
    void Response(ResponseData*);

protected:
private:
    // prevent copying
    LEDTest(const LEDTest&);
    void operator=(LEDTest&);

    // state machine states
    void ST_Idle(void);
    void ST_TurnOnNormalLED(void);
    void ST_TurnOnAlarmHighLED(ResponseData*);
    void ST_TurnOnAlarmMedLED(ResponseData*);
    void ST_TurnOnVentInopLED(ResponseData*);
    void ST_TurnOnSafetyValveLED(ResponseData*);
    void ST_TurnOnAlarmSilenceLED(ResponseData*);
    void ST_TurnOnAllO2LED(ResponseData*);
    void ST_TurnOnScreenLockLED(ResponseData*);
    void ST_TurnOnIntBattInUseLED(ResponseData*);
    void ST_TurnOnIntBattChargingLED(ResponseData*);
    void ST_TurnOnIntBattLowLED(ResponseData*);
    void ST_VerifyIntBattLowLEDOn(ResponseData*);

    void DoStateAction(EventData* pData);

    // State enumeration order must match the order of state method pointers 
    // in stateMachine[] within DoStateAction().
    enum E_States 
    {
        eST_TurnOnNormalLED = ChildTest::eST_MaxStates,
        eST_TurnOnAlarmHighLED,
        eST_TurnOnAlarmMedLED,
        eST_TurnOnVentInopLED,
        eST_TurnOnSafetyValveLED,
        eST_TurnOnAlarmSilenceLED,
        eST_TurnOnAllO2LED,
        eST_TurnOnScreenLockLED,
        eST_TurnOnIntBattInUseLED,
        eST_TurnOnIntBattChargingLED,
        eST_TurnOnIntBattLowLED,
        eST_VerifyIntBattLowLEDOn,
        eST_MaxStates,
        eST_EventIgnored,
        eST_CannotHappen
    };
};

//#undef EXTERN
#endif  // LEDTEST_H
