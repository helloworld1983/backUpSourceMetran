/******************************************************************************/
//$COMMON.H$
//   File Name:  AzTest.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: This file contains the class declaration for the Auto-zero CBIT
//      Test class.  The AzTest class is derived from the Test class and the
//      RTOS Timer base class.
/******************************************************************************/
#ifndef AZTEST_H
#define AZTEST_H

#include "CommonConstants.h"
#include "Test.h"
#include "AzScheduler.h"
#include "Timer.h"

#ifdef AZTEST_CPP
#define EXTERN 
#else
#define EXTERN extern 
#endif

typedef void (*AzCallBackPtr) (E_TestStatus testStatus);

// E_AZTestState is used to define the current state of an auto-zero test
enum E_AzTestState
{
    eAzCompleted,
    eSettlingDelayOnStart,
    eAzSampling,
    eSettlingDelayOnStop
};

// Number of pressure readings that must be in range for an auto-zero
const SHORT NUM_READINGS = 3; 

// Allowed variance in raw counts between all of the pressure readings used 
// in the auto-zero calculation.  
// This value is computed as 0.5% of the theoretical zero value (in counts).
static const USHORT READING_VARIANCE = 8;  

// Minimum and Maximum auto-zero readings in raw counts.
// The theoretical zero point should be 1594 counts.
// If we allow the zero point to drift +/- 4%, then we get:
static const SHORT MIN_ZERO_VALUE = 1530;
static const SHORT MAX_ZERO_VALUE = 1658;

class AzTest : public Timer, public Test
{
    public:
        AzTest
            (
                E_EventCode eventcode,
//                DiscreteOutput* solPtr,
//                PressureSensor* sensorPtr,
                AzScheduler* schedulerPtr = NULL
            );

        virtual void ExpirationRoutine(void);
        virtual E_TestStatus Run(void);
        void Start(void);
        static void S_Init(E_Bool useSchedulers = eFalse);
        void ProcessFailedCheck(void);
        
        inline void RegisterCallBack(AzCallBackPtr callBack) {CallBack = callBack;}
        inline E_AzTestState GetAzTestState(void) {return AzTestState;}

    protected:


    private:
        AzTest(void);
        AzTest(const AzTest&);

        E_Bool TakeSample(void);

        //$COMMON.ATTRIBUTE$
        //  Name: PressSampleArray[]
        //  Description:  Array of pressure samples.
        //  Units: None
        //  Range: n/a
        USHORT PressSampleArray[NUM_READINGS];

        //$COMMON.ATTRIBUTE$
        //  Name: NumSamples
        //  Description:  Number of samples taken for the auto-zero.
        //  Units: None
        //  Range: n/a
        SHORT NumSamples;

        //$COMMON.ATTRIBUTE$
        //  Name: SolenoidPtr
        //  Description:  Pointer to the auto-zero solenoid object.
        //  Units: None
        //  Range: n/a
//        DiscreteOutput* SolenoidPtr;

        //$COMMON.ATTRIBUTE$
        //  Name: SensorPtr
        //  Description:  Pointer to the pressure sensor object.
        //  Units: None
        //  Range: n/a
//        PressureSensor* SensorPtr;

        //$COMMON.ATTRIBUTE$
        //  Name: AzTestState
        //  Description:  Current state of the auto-zero test.
        //  Units: None
        //  Range: n/a
        E_AzTestState AzTestState;

        //$COMMON.ATTRIBUTE$
        //  Name: ZeroValue
        //  Description:  Last auto-zero value.
        //  Units: ADC counts
        //  Range: n/a
        SHORT ZeroValue;

        //$COMMON.ATTRIBUTE$
        //  Name: Index
        //  Description:  Index into the PressSampleArray
        //  Units: none
        //  Range: 0 - (NUM_READINGS - 1)
        SHORT Index;

        //$COMMON.ATTRIBUTE$
        //  Name: SchedulerPtr
        //  Description:  Pointer to the AzScheduler associated with this 
        //      Auto-Zero object
        //  Units: none
        //  Range: n/a
        AzScheduler* SchedulerPtr;

        //$COMMON.ATTRIBUTE$
        //  Name: CallBack
        //  Description:  Pointer to the function to invoke when the test 
        //      completes.
        //  Units: none
        //  Range: n/a
        AzCallBackPtr CallBack;
};      



/******************************************************************************/
//      GLOBALS
/******************************************************************************/

EXTERN AzTest* PInhAzTestPtr;
EXTERN AzTest* PExhAzTestPtr;

#undef EXTERN

#endif  // AZTEST_H
