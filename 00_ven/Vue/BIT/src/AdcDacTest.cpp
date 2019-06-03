/******************************************************************************/
//$COMMON.CPP$
//   File Name:  AdcDacTest.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: 
//      This class contains the operations to perform the ADC/DAC wrap around
//      test during CBIT.  
//
//   Interfaces:
//      The Run() operation interfaces with the Voltage Wrap Analog Output and
//      the ADC Wrap Analog input.  It also interfaces with the Error Handler
//      if the test fails.
//
//   Restrictions:
//      There is only 1 instance of this class.
/******************************************************************************/
#include "AdcDacTest.h"
#include "CommonConstants.h"

// Amount of time to delay to make sure that value output is really output 
// and is coming back in on other ADC channel.
const UNSIGNED ADC_DAC_DELAY_IN_TICKS = 20/MS_PER_TICK;

//  Define Statics
AdcDacTest* AdcDacTest::S_Instance = NULL;

SHORT AdcDacTest::AdcDacData[NUM_ADC_DAC_TEST_POINTS][eNumAdcDacTableIx] =
{
    // DAC Out          ADC In Min          ADC In Max
    {   100,               50,                   200    },
    {   1000,             900,                  1100    },
    {   2000,            1800,                  2200    },
    {   3000,            2700,                  3300    },
    {   4000,            3600,                  4400    },
    {   4500,            4050,                  5000    }
};

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: AdcDacTest()
//
//    Processing: 
//      This operation is the constructor for the AdcDacTest.  It 
//      invokes the base class Test constructor.
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
AdcDacTest::AdcDacTest(void) :
    Test(eCbitAdcDacTestCode, eCountBased, 3, VentInopHandler),
    Index(0)
{

}   // end AdcDacTest()




/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Run()
//
//    Processing: 
//      This operation is invoked every ADC_DAC_TIME from CbitTask::Entry().
//      This operation outputs the next value from the AdcDacData array on
//      the Voltage Wraparound channel.  It then goes to sleep for 
//      ADC_DAC_DELAY_IN_TICKS.  When it wakes up, it reads the wraparound
//      input channel and compares this value to the output value.  
//
//      If the input value is within allowed tolerances of the output value
//      (as specified in the AdcDacData array), the NumFailures attribute is
//      set to FALSE and the Index into the AdcDacData array is incremented,
//      accounting for wrap around.
//
//      If the input value is not within an allowed tolerance, the NumFailures
//      is incremented and the Index is left unchanged.  The same test is then
//      repeated. If the test fails 3 times in a row, the ventilator is put 
//      into an inoperative state.
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      E_TestStatus - returns the results of the test
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
E_TestStatus AdcDacTest::Run(void)
{
    SHORT value;            // Value read from ADC 
    E_Bool testDone = eFalse;

    // testDone is used to cause CBIT to repeat the ADC/DAC test until it
    // passes or fails.  If a failure condition is detected, it is not desirable
    // to wait for the interval for the test to be rescheduled.  Instead, the
    // test will be repeated by using testDone as loop control.
    while (eFalse == testDone)
    {

        // Output the next value on the DAC    
//        VoltageWrapAO->SetState(AdcDacData[Index][eDacOutput]);

        // Give the output time to get back around 
//        Task::Sleep(ADC_DAC_DELAY_IN_TICKS);
        
        // Read the value from the ADC
//        value = ADCWrapAI->GetCurrentReading();

        if ((value >= AdcDacData[Index][eMinAdcInput]) &&
            (value <= AdcDacData[Index][eMaxAdcInput]))
        {
            // Increment the index into the AdcDacData table; wrap around if 
            // necessary.  This is to get ready for the next test.
            Index++;
            if (Index >= NUM_ADC_DAC_TEST_POINTS)
            {
                Index = 0;
            }
            
            // Reset the number of failures
            TestStatus = eCheckPassed;
            
            // Since it passed, the test is done.  
            testDone = eTrue;
        }
        else
        {
            // Increment and Test the number of failures.  If the maximum 
            // allowed number of failures has been reached (as indicated by a 
            // TRUE return value), go Vent Inop.
            if (eTrue == IsFailedLimitMet())
            {

                TestStatus = eTestFailed;
                
                // For debugging, set the testDone variable so that processing
                // moves on from this test
                testDone = eTrue;
            }
            else
            {
                // Only a strike
                TestStatus = eCheckFailed;
            }
        }
    }    

    return(TestStatus);
    
}   // end Run()


