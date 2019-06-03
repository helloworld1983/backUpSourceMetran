/******************************************************************************/
//$COMMON.H$
//   File Name:  AzDirectTest.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: This file contains the class declaration for the Auto-zero 
//      Test class.  The AzDirectTest class is derived from the Test class.
//      AzDirectTest objects perform an auto-zero without using RTOS calls.
/******************************************************************************/
#ifndef AZDIRECTTEST_H
#define AZDIRECTTEST_H

#include "CommonConstants.h"
#include "AzTest.h"

#ifdef AZDIRECTTEST_CPP
#define EXTERN 
#else
#define EXTERN extern 
#endif

class AzDirectTest : public Test
{
    public:
        AzDirectTest
            (
                E_EventCode eventcode
//                DiscreteOutput* solPtr,
//                PressureSensor* sensorPtr
            );

        virtual E_TestStatus Run(void);
        static void S_Init(void);
        
    protected:


    private:
        AzDirectTest(void);
        AzDirectTest(const AzDirectTest&);

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
        //  Name: Index
        //  Description:  Index into the PressSampleArray
        //  Units: none
        //  Range: 0 - (NUM_READINGS - 1)
        SHORT Index;
};      

EXTERN AzDirectTest* PInhAzDirectTestPtr;
EXTERN AzDirectTest* PExhAzDirectTestPtr;

#undef EXTERN

#endif  // AZDIRECTTEST_H
