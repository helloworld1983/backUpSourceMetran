/******************************************************************************/
//$COMMON.CPP$
//   File Name:  AzDirectTest.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: 
//      This class contains operations to perform a pressure transducer 
//      auto-zero.  The auto-zero is performed without using RTOS calls but
//      the caller is blocked while pressure settles and is sampled.  
//      Operations in AzTest perform the same type of test but with the use
//      of the RTOS calls.  The operations in this class also do not wait for
//      the pressure sensor to settle when the auto-zero solenoid is deactivated.
//      These operations are invoked from POST which is not using the pressure
//      sensors anyway.  Auto-zeroes performed during normal operation (and
//      during Self Test - TBD) should not use the operations in this class.
//      Instead they should be using the operations in AzTest. 
//
//   Interfaces:
//      Operations in this class interface with the pressure transducer 
//      auto-zero solenoids (discrete outputs).  They also interface with
//      the pressure sensors to get the current pressure reading.  
//
//   Restrictions:
//      None
/******************************************************************************/
#define AZDIRECTTEST_CPP
#include "AzDirectTest.h"
#undef AZDIRECTTEST_CPP

#include "Util.h"
#include "CommonConstants.h"

// Amount of time in counts to wait for the pressure to settle after 
// energizing the auto-zero solenoid
static const ULONG SETTLING_TIME = 40;

// Amount of time between samples in counts.
static const UNSIGNED SAMPLING_TIME = 25;

// Maximum allowed samples for a stable pressure.
static const USHORT MAX_ALLOWED_SAMPLES = 10;

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: AzDirectTest()
//
//    Processing: 
//      This operation is the constructor for objects of the AzDirectTest class.
//      This operation invokes the constructors for the Timer class from which
//      this class is derived.  
//
//    Input Parameters:
//      timerName - name of the timer
//
//    Output Parameters:
//      None
//
//    Return Values:
//      None
//
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//      None
//
//    Requirements:  
//
/******************************************************************************/
AzDirectTest::AzDirectTest
    (
        E_EventCode eventCode
//        DiscreteOutput* solPtr,
//        PressureSensor* sensorPtr
    )
    : Test(eventCode, eCountBased, 1, VentInopHandler)
//      SolenoidPtr(solPtr),
//      SensorPtr(sensorPtr)
{

}   // end AzDirectTest()







/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Run()
//
//    Processing: 
//      This operation is invoked by callers who want to
//      wait for the completion of the auto-zero test.  This operation invokes
//      the Run() method with no parameters to initiate the auto-zero test.
//      It then delays, waiting for the test to complete.
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      zeroValue - computed zero value.
//
//    Return Values:
//      E_TestStatus - indicates if the test passed.
//
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//      None
//
//    Requirements:  
//
/******************************************************************************/
E_TestStatus AzDirectTest::Run(void)
{
    UNSIGNED clock, elapsed;        // for debug.
    
    // Reset number of samples since a new auto-zero is about to begin.
    NumSamples = 0;
    
    // Reset the Index
    Index = 0;
    
    // Activate the auto-zero solenoid
//    SolenoidPtr->SetStateDirect(eOpen);


    // DEBUG
//    clock = SystemClock::Retrieve();
    
    // Wait for the pressure reading to settle
//    Wait(SETTLING_TIME); replace by fuction of system
    
    // DEBUG
    elapsed = CalcElapsedTime(clock);

    while (eFalse == TakeSample())
    {
//        clock = SystemClock::Retrieve();

        // Delay between samples
//        Wait(SAMPLING_TIME); replace by function of system
        
        // DEBUG
        elapsed = CalcElapsedTime(clock);
    }

    return(TestStatus);
    
}   // end Run()


/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: S_Init()
//
//    Processing: 
//      This operation is invoked at start-up to instantiate the Inhalation
//      and Exhalation Pressure Transducer Auto-Zero Test objects.
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      None
//
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//      None
//
//    Requirements:  
//
/******************************************************************************/
void AzDirectTest::S_Init(void)
{
    // Instantiate auto-zero test objects
//    PInhAzDirectTestPtr =
//        new AzDirectTest(eCbitInhPAutoZeroCode, InhPressureZero, InhPressSensor);

//    PExhAzDirectTestPtr =
//        new AzDirectTest(eCbitExhPAutoZeroCode, ExhPressureZero, ExhPressSensor);

}   // end S_Init()



/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: TakeSample()
//
//    Processing: 
//      This operation is invoked each time the pressure transducer should 
//      be sampled.
//
//      This operation contains all of the logic to sample the pressure, 
//      determine if the last NUM_READINGS are within the allowed 
//      READING_VARIANCE of each other and if the time limit for obtaining 
//      a valid auto-zero has elapsed.  If the time limit has elapsed or 
//      the auto-zero value is out of range, ProcessFailedCheck() is called.
//
//      If a valid auto-zero is determined, the SetZeroOffset() operation of
//      the sensor being zeroed is called to update the zero value.
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      E_Bool - indicates if the test is complete.  The test is complete if
//          either the pressure stabilized and a valid or invalid auto-zero
//          was computed or if the time limit to allow for reading stabilization
//          has expired.
//
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//      None
//
//    Requirements:  
//
/******************************************************************************/
E_Bool AzDirectTest::TakeSample(void)
{
    E_Bool rtnValue = eFalse;   // indicates if the test is complete
    SHORT ii, jj;               // loop control
    E_Bool isSettled;           // indicates if the pressure has settled
    LONG zeroValue;             // auto-zero value
        
    // Increment number of samples
    NumSamples++;
    
    // Sample the pressure sensor.    
//    PressSampleArray[Index] = SensorPtr->GetRawCounts();
    Index = (Index + 1) % NUM_READINGS;
    
    if (NumSamples >= NUM_READINGS)
    {

        // Now check to see if the last NUM_READINGS are within accepted 
        // tolerance of each other.
        ii = 0;
        isSettled = eTrue;
        
        while ((ii < (NUM_READINGS - 1)) && (isSettled == eTrue))
        {
            jj = ii + 1;
            while ((jj < NUM_READINGS) && (isSettled == eTrue))
            {
//                if (abs(PressSampleArray[ii] - PressSampleArray[jj])
//                    < READING_VARIANCE)
//                {
//                    jj++;
//                }
//                else
//                {
//                    isSettled = eFalse;
//                }
            }
            ii++;
        }

        // Check the test status.
        if (eTrue == isSettled)        
        {
            // The test is complete
            rtnValue = eTrue;

            // De-activate the auto-zero solenoid.  No delay for settling; 
            // POST doesn't need it.
//            SolenoidPtr->SetStateDirect(eClose);
            
            // TBD average
            zeroValue = 0;
            for (int ii = 0; ii < NUM_READINGS; ii++)
            {
                zeroValue += PressSampleArray[ii];
            }
            zeroValue/= NUM_READINGS; 

            // Range check zero value
            if ((zeroValue > MIN_ZERO_VALUE)  && (zeroValue < MAX_ZERO_VALUE))
            {
                // Update the zero value of the pressure transducer
//                SensorPtr->SetZeroOffset(zeroValue);
                
                TestStatus = eCheckPassed;
            }
            else
            {
                TestStatus = eTestFailed;
            }
        }
      
        else
        {
            // Determine if the maximum samples have been taken
            if (NumSamples > MAX_ALLOWED_SAMPLES)
            {
                // The pressure reading did not settle in the allowed time
                TestStatus = eTestFailed;
                rtnValue = eTrue;

                // De-activate the auto-zero solenoid.  Note that there is no
                // delay to allow for settling.  POST doesn't need it.
//                SolenoidPtr->SetStateDirect(eClose);
            }
        }
    }
    
    return(rtnValue);       // can rtnValue be combined with Test Status

}   // TakeSample()







