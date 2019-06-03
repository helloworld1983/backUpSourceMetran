/******************************************************************************/
//$COMMON.CPP$
//   File Name:  HighTempTest.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: 
//      This file contains the definition of the Enclosure High Temperature
//      Test.  Enclosure Temperature is monitored periodically via the CBIT
//      task.  If temperature exceeds 65 degrees, Celsius for 10 seconds or
//      more, a high priority  alarm is triggered.  If temperature exceeds 
//      70 degrees C for more than 10 seconds, a Vent Inop condition is 
//      declared.
//
//   Interfaces:
//      Operations in this class interface with the enclosure temperature
//      sensor.
//
//   Restrictions:
//      There is only one instance of this class.
/******************************************************************************/
#include "HighTempTest.h"
#include "AlarmTask.h"

#include "CommonConstants.h"

//  Define Statics
HighTempTest* HighTempTest::S_Instance = NULL;

// If the enclosure temperature is greater than 65 degrees, Celsius for
// OOR_SAMPLE_LIMIT number of samples, a high priority alarm is declared.
const SHORT HI_TEMP_ALARM_LIMIT = 65 * SCALE;

// If the enclosure temperature is greater than 70 degrees, Celsius for 
// OOR_SAMPLE_LIMIT number of samples, a Vent Inop state is entered.
const SHORT HI_TEMP_VIO_LIMIT = 70 * SCALE;

// Number of consecutive samples that must exceed the temperature limits before
// an alarm condition is declared or the ventilator is put in a VIO state.
const SHORT OOR_SAMPLE_LIMIT = 10;

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: HighTempTest()
//
//    Processing: 
//      This operation is the constructor for the HighTempTest.
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
HighTempTest::HighTempTest(void) : 
    Test(eCbitHighTempTestCode, eCountBased, OOR_SAMPLE_LIMIT, VentInopHandler),
    NumHighAlarmReadings(0)
{
    
}   // end HighTempTest()




/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Run()
//
//    Processing: 
//      This operation is invoked from the CBIT Task to run the enclosure
//      temperature test.    It retrieves the current temperature reading
//      from the sensor and compares it to the limit.  If it exceeds the
//      limit, it checks if it has exceeded the limit for the minimum number
//      of consecutive cycles.  If so, a test fail status is returned.  
//      If the temperature does not exceed the limit, a check passed status
//      is returned.
//
//      In either case, the private operation CheckAlarmCondition() is invoked
//      to check if the temperature exceeds the alarm limits.  The alarm 
//      state is less serious than the CBIT test failure.  An alarm is triggered
//      at a lower temperature to give the operator a chance to correct the
//      situation before shutting down the ventilator.
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
E_TestStatus HighTempTest::Run(void) 
{
    SHORT tempReading;

    // Read the enclosure temperature.  The returned value is in engineering
    // units.    
//    tempReading = EnclosureTemp->GetCurrentReading();

    // First check if the temperature reading exceeds the limit for tripping
    // a VIO condition.   This is the part of the test that uses the base
    // class Test and its functionality for handling a failure.  
    if (tempReading > HI_TEMP_VIO_LIMIT)    
    {
        // Increment and Test the number of failures.  If the maximum 
        // allowed number of failures has been reached (as indicated by a 
        // TRUE return value), return eTestFailed which will cause Vent Inop.
        if (eTrue == IsFailedLimitMet())
        {
            TestStatus = eTestFailed;
        }    
        else
        {
            TestStatus = eCheckFailed;
        }
    }
    else
    {
        TestStatus = eCheckPassed;
    }
    

    // The critical temperature limit has not been exceeded.  However, another
    // check is made to determine if an alarm should be annunciated to the 
    // user.  This part of the test does not conform to the CBIT architecture.
    // Regardless of the status of this test, the status determined by the
    // check for HI_TEMP_VIO_LIMIT is returned to CBIT.     
    CheckAlarmCondition(tempReading);
        
    return(TestStatus);

}   // end Run()


/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CheckAlarmCondition()
//
//    Processing: 
//      This operation handles the detection and auto-reset of the high
//      enclosure temperature alarm.   If the enclosure temperature alarm
//      is not active and the temperature exceeds the HI_TEMP_ALARM_LIMIT
//      for OOR_SAMPLE_LIMIT samples, the alarm is declared.  If the alarm
//      is active and the temperature is less than or equal to the 
//      HI_TEMP_ALARM_LIMIT for OOR_SAMPL_LIMIT samples, the alarm is 
//      auto-reset.
//
//    Input Parameters:
//      tempReading - enclosure temperature reading
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
void HighTempTest::CheckAlarmCondition(SHORT tempReading)
{

    // Compare the temperature reading to the alarm limit.
    if (tempReading > HI_TEMP_ALARM_LIMIT)
    {
        // If the alarm is already active, just reset the NumHighAlarmReadings
        // attribute to the limit.  NumHighAlarmReadings may have been 
        // decremented if the temperature dropped below the limit; it must
        // be reset here since it is used as a counter for auto-reset triggering.
        if (eActive == AlarmTask::S_GetInstance()->GetIsActive(eHighEnclosureTemp))
        {
            NumHighAlarmReadings = OOR_SAMPLE_LIMIT;
        }
        
        else
        {
            // If the temperature has not been out of range for the required
            // number of samples, just increment the counter.        
            if (NumHighAlarmReadings < OOR_SAMPLE_LIMIT) 
            {
                NumHighAlarmReadings++;        
            }
            else
            {
                // Otherwise, notify alarms of the Enclosure High Temp condition
                AlarmTask::S_GetInstance()->SetIsActive(eHighEnclosureTemp, eActive);
            }
        
        }   // alarm not active
    
    }   // tempReading > HI_TEMP_ALARM_LIMIT
    
    
    else
    {
        // Temperature is below HI_TEMP_ALARM_LIMIT, check status of the alarm.
        if (eActive ==AlarmTask::S_GetInstance()->GetIsActive(eHighEnclosureTemp))
        {
        
            // Since the alarm condition is active, decrement the number of
            // high readings.  If this count reaches 0, reset the alarm.
            NumHighAlarmReadings--;
            if (NumHighAlarmReadings <= 0)
            {
                AlarmTask::S_GetInstance()->SetIsActive(eHighEnclosureTemp, eNotActive);
            }        

        }
        else
        {
            // The alarm condition was not annunciated.  Reset the counter.
            NumHighAlarmReadings = 0;
        }
    }
    
}   // CheckAlarmCondition()
