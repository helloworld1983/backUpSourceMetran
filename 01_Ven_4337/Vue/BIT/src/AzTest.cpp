/******************************************************************************/
//$COMMON.CPP$
//   File Name:  AzTest.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: 
//      This class contains operations to perform a pressure transducer 
//      auto-zero.
//
//   Interfaces:
//      Operations in this class interface with the pressure transducer 
//      auto-zero solenoids (discrete outputs).  They also interface with
//      the pressure sensors to get the current pressure reading.  And, they
//      interface with the Error Handler to log auto-zero failures.
//
//   Restrictions:
//      None
/******************************************************************************/
#define AZTEST_CPP
#include "AzTest.h"
#undef AZTEST_CPP

#include "Util.h"
#include "CommonConstants.h"
#include "CbitTask.h"

/******************************************************************************/
//      CONSTANT DEFINITIONS
/******************************************************************************/

// Amount of time in TICKS that is allowed for settling when the auto-zero
// solenoid is opened/closed.
static const UNSIGNED SETTLING_TIME = 50/MS_PER_TICK;   

// Amount of time between samples in TICKS.
static const UNSIGNED SAMPLING_TIME = 20/MS_PER_TICK;   

// Maximum allowed time for the zero pressure to stabilize.
// This is the minimum exhalation time (200ms) minus the switching time 
// required for the solenoid to both open and close.
static const UNSIGNED MAX_ALLOWED_TIME = (200/MS_PER_TICK - (2*SETTLING_TIME));

/******************************************************************************/
//      PUBLIC OPERATIONS
/******************************************************************************/

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: AzTest()
//
//    Processing: 
//      This operation is the constructor for objects of the AzTest class.
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
AzTest::AzTest
    (
        E_EventCode eventCode,
//        DiscreteOutput* solPtr,
//        PressureSensor* sensorPtr,
        AzScheduler* schedulerPtr
    ): Timer("AZ_TEST", 1, pdTRUE),  //add to build
//    : Timer("AZ_TEST", 0, 0, FALSE),
      Test(eventCode, eCountBased, 3, VentInopHandler)
//      SchedulerPtr(schedulerPtr),
//      AzTestState(eAzCompleted),
//      SolenoidPtr(solPtr),
//      SensorPtr(sensorPtr),
//      CallBack(NULL)
{
    // Start the scheduler if there is one
    if (schedulerPtr != NULL)
    {
        schedulerPtr->RestartTiming();
    }

}   // end AzTest()




/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ExpirationRoutine()
//
//    Processing: 
//      This operation is the definition of the ExpirationRoutine() declared
//      as a pure virtual method in the RTOS Timer class.  This operation
//      is invoked by the RTOS whenever the associated timer expires.
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
//      Since this operation is invoked via the OS, it cannot make any 
//      function calls that could potentially cause suspension.
//
//    Requirements:  
//
/******************************************************************************/
void AzTest::ExpirationRoutine(void)
{
    STATUS status;      // RTOS return status
    
    // Processing performed when the timer expires depends upon the current
    // state of the Auto-Zero test.
    switch (AzTestState)
    {
        case eAzCompleted:
            // Timer should not be running when the test has completed.
            ASSERTION(AzTestState != eAzCompleted);
            break;
            
        case eSettlingDelayOnStart:
            // Settling Delay has elapsed, enter Sampling state.
            AzTestState = eAzSampling;
            
            // Update number of samples
            NumSamples = 0;
            
            // Start the timer for the Sampling Time Delay
//            status = Reset(SAMPLING_TIME, SAMPLING_TIME, TRUE);
        
            // It is an error if the return status is not successful.    
//            ASSERTION(status == NU_SUCCESS);

            // Take a sample; it should always return FALSE since this is the
            // first sample
//            ASSERTION(eFalse == TakeSample());

            break;
            
        case eAzSampling:
            // Take a sample; return value indicates if sampling is complete
            if (eTrue == TakeSample())
            {
                // Disable the timer.
//                Disable();
                
                // Restart Timer for the settling time
//                status = Reset(SETTLING_TIME, 0, TRUE);
            
                // It is an error if the return status is not successful.    
//                ASSERTION(status == NU_SUCCESS);

                AzTestState = eSettlingDelayOnStop;
            }

            break;    

        case eSettlingDelayOnStop:
            // Auto-zero is complete.  Disable the timer and update the state.
//            Disable();
            AzTestState = eAzCompleted;
            
            if (CallBack != NULL)
            {
                CallBack(TestStatus);
            }

            break;            
        
        default:
            // Invalid state.
            ASSERTION(eFalse);
            break;
    }    
    
}   // end ExpirationRoutine()

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
//      AzTestState is Completed.
//
//    Miscellaneous:
//      None
//
//    Requirements:  
//
/******************************************************************************/
E_TestStatus AzTest::Run(void)
{
    
    // Start the test.
    Start();
    
    // Now wait until the auto-zero is complete
    while (eAzCompleted != AzTestState)
    {
//        Task::Sleep(20);
    }
    
    return(TestStatus);
    
}   // end Run()


/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Start()
//
//    Processing: 
//      This operation is invoked during normal ventilator operation to kick
//      start an auto-zero.  The auto-zero takes some time to complete and
//      the caller does not block if this Run() operation is invoked.
//
//      This operation updates the auto-zero state, resets NumSamples 
//      since a new auto-zero is about to begin, activates the auto-zero 
//      solenoid, and starts the timer for the Settling Time Delay.
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
//      AzTestState is Completed.
//
//    Miscellaneous:
//      This operation does not wait until the completion of the auto-zero.
//      If the caller wishes to block, Run() should be invoked.
//
//    Requirements:  
//
/******************************************************************************/
void AzTest::Start(void)
{
    STATUS status;
    
    // If the scheduler has determined that it is time to perform an auto-zero,
    // make sure that the previous auto-zero was completed.  It's an error if
    // not.  
//    PRE_CONDITION(AzTestState == eAzCompleted);

    // Update state
    AzTestState = eSettlingDelayOnStart;

    // Reset number of samples since a new auto-zero is about to begin.
    NumSamples = 0;
    
    // Reset the Index
    Index = 0;
    
    // Activate the auto-zero solenoid
//    SolenoidPtr->SetStateDirect(eOpen);
    
    // Start the timer for the Settling Time Delay
//    status = Reset(SETTLING_TIME, 0, TRUE);
    
    // It is an error if the return status is not successful.    
//    ASSERTION(status == NU_SUCCESS);

}   // end Start()

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
void AzTest::S_Init(E_Bool useSchedulers)
{
    if (eTrue == useSchedulers)
    {
        // Instantiate the schedulers.
        AzScheduler::S_Init();

        // Instantiate auto-zero test objects
//        PInhAzTestPtr =
//            new AzTest(eCbitInhPAutoZeroCode, InhPressureZero, InhPressSensor, InhAzSchedulerPtr);

//        PExhAzTestPtr =
//            new AzTest(eCbitExhPAutoZeroCode, ExhPressureZero, ExhPressSensor, ExhAzSchedulerPtr);

    }
    
    // Don't use the schedulers
    else
    {
        // Instantiate auto-zero test objects
//        PInhAzTestPtr =
//            new AzTest(eCbitInhPAutoZeroCode, InhPressureZero, InhPressSensor);

//        PExhAzTestPtr =
//            new AzTest(eCbitExhPAutoZeroCode, ExhPressureZero, ExhPressSensor);

    }
}   // end S_Init()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessFailedCheck()
//
//    Processing: 
//      This operation is invoked from the registered call back routine
//      if the autozero operation fails.
//
//      If the maximum allowed auto-zero failures has been detected, this
//      operation interfaces with the Error Handler to put the ventilator
//      into an inoperative state.  If the maximum limit has not been 
//      reached, this operation calls AzScheduler::Init() to restart the 
//      auto-zero timing; this will cause the auto-zero to be tried again in
//      1 minute.
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
void AzTest::ProcessFailedCheck(void)
{
    STATUS status;      // RTOS return status   

    // If the maximum number of failed auto-zeroes have occurred, handle it.
    if (eTrue == IsFailedLimitMet())
    {
        Handler(EventCode);
        TestStatus = eTestFailed;
    }
    else
    {
        // Tell CBIT to log the event to NVRAM.  
        status = (CbitTask::S_GetInstance())->Send(EventCode);
//        ASSERTION(status == NU_SUCCESS);
        
        // Restart the auto-zero timing.
        if (SchedulerPtr != NULL)
        {
            SchedulerPtr->RestartTiming();
        }

    }    
}   // ProcessFailedAzTest()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: TakeSample()
//
//    Processing: 
//      This operation is invoked from the ExpirationRoutine() method of this
//      class each time the pressure transducer should be sampled.
//
//      This operation contains all of the logic to sample the pressure, 
//      determine if the last NUM_READINGS are within the allowed 
//      READING_VARIANCE of each other and if the time limit for obtaining 
//      a valid auto-zero has elapsed.  If the time limit has elapsed or 
//      the auto-zero value is out of range, eFalse is returned to the caller.
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
// TBD: this is invoked in the Timer thread of control...  Is this okay, or not?
// May need to move it into thread of control of caller!  

E_Bool AzTest::TakeSample(void)
{
    E_Bool rtnValue = eFalse;   // indicates if the test is complete
    static UNSIGNED startTime;  // time the sampling started
    SHORT ii, jj;               // loop control
    E_Bool isSettled;           // indicates if the pressure has settled
    LONG zeroValue;             // auto-zero value
        

    // Increment NumSamples up to NUM_READINGS
    if (NumSamples < NUM_READINGS)    
    {
        if (NumSamples == 0)
        {   
            // If this is the first sample, snapshot the system clock
//            startTime = SystemClock::Retrieve();
        }
        
        NumSamples++;
    }
    
    // Sample and store the pressure reading
//    PressSampleArray[Index] = SensorPtr->GetRawCounts();
    Index = (Index + 1) % NUM_READINGS;
    
    if (NumSamples == NUM_READINGS)
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
//                if (abs(PressSampleArray[ii]-PressSampleArray[jj]) < READING_VARIANCE)
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

            // De-activate the auto-zero solenoid
//            SolenoidPtr->SetStateDirect(eClose);
            
            // TBD average
            zeroValue = 0;
            for (int ii= 0; ii < NUM_READINGS; ii++)
            {
                zeroValue+= PressSampleArray[ii];
            }
            zeroValue/= NUM_READINGS; 

            // Update the attribute.            
            ZeroValue = (SHORT) zeroValue;
            
            // Range check zero value
            if ((ZeroValue > MIN_ZERO_VALUE)  && (ZeroValue < MAX_ZERO_VALUE))
            {
                // Update the zero value of the pressure transducer
//                SensorPtr->SetZeroOffset(zeroValue);
                
                TestStatus = eCheckPassed;
            }
            else
            {
                TestStatus = eCheckFailed;
            }
        }
      
        else
        {
            // Determine if the allowed time has elapsed
            if (CalcElapsedTime(startTime) > MAX_ALLOWED_TIME)
            {
                // The pressure reading did not settle in the allowed time
                TestStatus = eCheckFailed;
                rtnValue = eTrue;

                // De-activate the auto-zero solenoid
                // TBD
//                SolenoidPtr->SetStateDirect(eClose);
            }
        }
    }
    
    return(rtnValue);       // can rtnValue be combined with Test Status


}   // TakeSample()
