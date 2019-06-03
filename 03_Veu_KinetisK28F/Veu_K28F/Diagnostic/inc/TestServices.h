//*****************************************************************************/
//$COMMON.H$
//   File Name:  TestServices.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: TestServices
//      Provides common services required by tests.
//*****************************************************************************/

#ifndef TESTSERVICES_H
#define TESTSERVICES_H

#include "CommonTypes.h"
#include "CommonConstants.h"
#include "FailureCodes.h"
#include "Result.h"

#ifdef USE_NUCLEUS
    #include "nucpp.h"
#endif

#ifdef NO_HARDWARE
    #include "HardwareStubs.h"
#else
    // forward declaration eliminates include files thus speeding compile times
    class FlowController;
    class PressureSensor;
    class FlowSensor;
#endif

// all LPM for diagnostics represented using implied decimal (e.g 102 is 10.2)
static const SHORT LPM_SCALE = 10;

// used to convert LPM * LPM_SCALE to/from ml/sec * SCALE
// 		multiply to convert (LPM * LPM_SCALE) to (ml/sec * SCALE)
// 		divide to convert (ml/sec * SCALE) to (LPM * LPM_SCALE)
static const SHORT LPM_FACTOR = ((1000 * SCALE) / 60) / LPM_SCALE;

static const UNSIGNED TWENTYFIVE_MS = 25 * MS_PER_TICK;
static const UNSIGNED FIFTY_MS = 50 * MS_PER_TICK;
static const UNSIGNED ONE_HUNDRED_MS = 100 * MS_PER_TICK;
static const UNSIGNED TWO_HUNDRED_MS = 200 * MS_PER_TICK;
static const UNSIGNED FIVE_HUNDRED_MS = 500 * MS_PER_TICK;
static const UNSIGNED ONE_SECOND = 1000 * MS_PER_TICK;
static const UNSIGNED TWO_SECONDS = 2000 * MS_PER_TICK;

class TestServices 
{
public:
    TestServices() { }
    ~TestServices() { }

    void Initiate(void);
    void Conclude(void);
    void KnownState(void);
    void GetFirstFailure(Result::Info&);
    E_Bool DoTestsPass(void);
    LONG GetFlow(FlowSensor*, USHORT = 5, SHORT* = NULL, SHORT* = NULL);
    SHORT GetPressure(PressureSensor*, USHORT = 5, SHORT* = NULL, SHORT* =NULL);
    USHORT GetO2V(USHORT = 5, USHORT* = NULL, USHORT* = NULL);
    void ChargeCircuit(FlowController*, LONG, UNSIGNED, SHORT pressure, SHORT*,
        LONG* = NULL);
    E_Bool BlowerSpinDown(LONG, LONG*);
    UNSIGNED ElapsedTime(UNSIGNED startTime);
    void DelayNoSleep(UNSIGNED delay, UNSIGNED startTime = 0);
    E_Bool DelayWithCancel(UNSIGNED delay);
    void SendResultsOutRS232(void);
private:
    // prevent copying   
    TestServices(const TestServices&);
    void operator=(TestServices&);
    
    static const UNSIGNED MAXUNSIGNED;
};

TestServices& TheTestServices(void);

#endif //TESTSERVICES_H

