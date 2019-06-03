//$COMMON.CPP$
//   File Name:  PdAlarms.cpp
//   Copyright 1996 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: Handle all of Patient Data alarms
//              Low Inhalation Pressure (Lip)
//              High Oxygen concentration (HiO2)
//				High Internal Oxygen concentration (HighInternalO2)
//              Low Oxygen concentration (LowO2)
//              Low Exhaled Minute Volume Volume (LowExhMinVol)
//              Low Exhaled Spontaneous Tidal Volume (LowExhSpontTv)
//              Low Exhaled Mandatory Tidal Volume (LowExhManTv)
//              High Respiratory Rate (HiRespRate) 
//
//   Interfaces: 
//               ALARMS
//
//   Restrictions: One instance of this class exists
/******************************************************************************/

/* Include necessary Nucleus PLUS files.  */
#include "PdAlarm.h"
#include "AlarmTask.h"

//Utils includes
#include "DebugTrace.h"

static const ULONG O2TIMELIMIT = 1;//5;          // 30 seconds
static const ULONG O2RESETTIMELIMIT = 1;//2;    // 2 samples at 1 seconds intervals
static const ULONG ONE_SECOND = 1000;
static const ULONG TWO_SECOND = ONE_SECOND * 2;

PdAlarms* PdAlarms::S_Instance = NULL;

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: S_GetInstance()
//
//    Processing:
//      Since only 1 instance of this object should be created, any access
//      to the object comes via this operation.  If any other object wants
//      access to the object, it invokes S_GetInstance(),
//      which returns a pointer to the object.
//
//      If the object has not already been created, this method instantiates
//      it and returns a pointer to the object.  If the object has already been
//      instantiated, a pointer to the object is returned.
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      * - pointer to the object instantiated from this class
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
PdAlarms* PdAlarms::S_GetInstance (void)
{
    //Create object if it is null
    if(S_Instance == NULL)
    {
        S_Instance = new PdAlarms ();
    }
    //check it is null or not
    ASSERTION(S_Instance != NULL);


    return (S_Instance);
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: PdAlarms  
//
//    Processing: Constructor for patient data alarms
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None	 
//
/******************************************************************************/
PdAlarms::PdAlarms (void)
{
    mHighO2AlarmDelayTimeAfterFlushO2 = 0;
    mLowO2AlarmDelayTimeAfterFlushO2 = 0;

}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CheckHiCondition  
//
//    Processing: Checks value to be greater than high limit  
//
//    Input Parameters: 
//		LONG hilimit - limit value is check against
//		LONG value -   value to be checked
//		alarmID - Alarm ID object
//      SensitiveValue - Base on Sensitive value to set alarm status
//
//    Output Parameters: None 
//
//    Return Values: None 
//
//    Pre-conditions: None
// 
//    Miscellaneous: None
//
//    Requirements: None      
//
/******************************************************************************/
void PdAlarms::CheckHiCondition (LONG hilimit, LONG value, int alarmID, int SensitiveValue)
{
    static int senValCounter = 0;

    //when value is higher than limit value, check alarm status
    if(value > hilimit)
    {
        //if alarm status is not active
        if(eNotActive == AlarmTask::S_GetInstance ()->GetIsActive(alarmID))
        {
            //increase counter
            ++senValCounter;
            //if counter higher or equal SensitiveValue
            if(senValCounter >= SensitiveValue)
            {
                //Reset senVal
                senValCounter = 0;
                //set alarm status acitve
                AlarmTask::S_GetInstance ()->SetIsActive (alarmID, eActive);
            }
        }
        //Do nothing when alarm status is active currently.

    }
    //when value is lower than limit value
    else
    {
        //if alarm status is still active, deactivate alarm status
        if(eActive == AlarmTask::S_GetInstance ()->GetIsActive(alarmID))
        {
            //Reset senVal
            senValCounter = 0;
            //deactivate alarm status
            AlarmTask::S_GetInstance ()->SetIsActive (alarmID, eNotActive);
        }
        //Do nothing when alarm status is currently deactivate
    }
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CheckLowCondition  
//
//    Processing: Checks value to be less than low limit  
//
//    Input Parameters: 
//		LONG lowlimit - limit value is check against
//		LONG value -   value to be checked
//		alarmID - Alarm ID object
//      SensitiveValue - Base on Sensitive value to set alarm status
//
//    Output Parameters: None 
//
//    Return Values: None 
//
//    Pre-conditions: None
// 
//    Miscellaneous: None
//
//    Requirements: None      
//
/******************************************************************************/
void PdAlarms::CheckLowCondition (LONG lowlimit, LONG value, int alarmID, int SensitiveValue)
{
    static int senValCounter = 0;

    //when value is lower than limit value, check alarm status
    if(value < lowlimit)
    {
        //if alarm status is not active
        if(eNotActive == AlarmTask::S_GetInstance ()->GetIsActive(alarmID))
        {
            //increase counter
            ++senValCounter;
            //if counter higher or equal SensitiveValue
            if(senValCounter >= SensitiveValue)
            {
                //Reset senVal
                senValCounter = 0;
                //set alarm status acitve
                AlarmTask::S_GetInstance ()->SetIsActive (alarmID, eActive);
            }
        }
        //Do nothing when alarm status is active currently.

    }
    //when value is higher than limit value
    else
    {
        //if alarm status is still active, deactivate alarm status
        if(eActive == AlarmTask::S_GetInstance ()->GetIsActive(alarmID))
        {
            //Reset senVal
            senValCounter = 0;
            //deactivate alarm status
            AlarmTask::S_GetInstance ()->SetIsActive (alarmID, eNotActive);
        }
        //Do nothing when alarm status is currently deactivate
    }
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CheckHiO2  
//
//    Processing: 	If the O2 is higher than the limit for 30 seconds
//					an alarm is activated. Hi O2 alarm is auto-reset if the 
//					alarm is alarm is not present for O2RESETTIMELIMIT seconds.
//
//    Input Parameters: 
//			LONG hilimit 
//			LONG value 
//			AlarmStat* alarmtype
//
//    Output Parameters: None 
//
//    Return Values: None 
//
//    Pre-conditions: None
// 
//    Miscellaneous: None
//
//    Requirements:  None     
//
/******************************************************************************/
void PdAlarms::CheckHiO2 (LONG hilimit, LONG value, int alarmID)
{
    static ULONG activetime = 0;
    static ULONG notactivetime = 0;
    const LONG HIGH_O2_ALARM_DELAY_TIME_LIMIT = 0;
    const ULONG RESET = 0;

    //Do nothing when time delay after flushing O2 is equal LIMIT value
    if(mHighO2AlarmDelayTimeAfterFlushO2 >= HIGH_O2_ALARM_DELAY_TIME_LIMIT)
    {
        --mHighO2AlarmDelayTimeAfterFlushO2;
        return;
    }

    //when value is higher than limit and alarm status is not active
    if((value >= hilimit)
            && (eNotActive == AlarmTask::S_GetInstance ()->GetIsActive (alarmID)))
    {
        //increase counter activetime
        ++activetime;
    }
    else
    {
        //Reset counter
        activetime = RESET;
    }

    //if activetime counter is passed, set alarm status active
    if(activetime >= O2TIMELIMIT)
    {
        AlarmTask::S_GetInstance ()->SetIsActive (alarmID, eActive);
    }

    //when value is lower than limit and alarm status is active
    if((value < hilimit)
            && (eActive == AlarmTask::S_GetInstance ()->GetIsActive (alarmID)))
    {
        //increase notactivetime counter
        ++notactivetime;
    }
    else
    {
        //Reset notactivetime counter
        notactivetime = RESET;
    }

    //when notactivetime counter is passed, set alarm status deactivate
    if(notactivetime >= O2RESETTIMELIMIT)
    {
        AlarmTask::S_GetInstance ()->SetIsActive (alarmID, eNotActive);
    }

}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CheckLowO2  
//
//    Processing: 	If the O2 is lower than the limit for 30 seconds
//					an alarm is activated. Low O2 alarm is auto-reset if the 
//					alarm is alarm is not present for O2RESETTIMELIMIT seconds.   
//
//    Input Parameters:
//			LONG lowlimit
//			LONG value 
//			AlarmStat* alarmtype
//
//    Output Parameters: None 
//
//    Return Values: None 
//
//    Pre-conditions: None
// 
//    Miscellaneous:  None
//
//    Requirements:  None     
//
/******************************************************************************/
void PdAlarms::CheckLowO2 (LONG lowlimit, LONG value, int alarmID)
{
    static ULONG activetime = 0;
    static ULONG notactivetime = 0;
    const LONG LOW_O2_ALARM_DELAY_TIME_LIMIT = 0;
    const ULONG RESET = 0;

    if(mLowO2AlarmDelayTimeAfterFlushO2 >= LOW_O2_ALARM_DELAY_TIME_LIMIT)
    {
        --mLowO2AlarmDelayTimeAfterFlushO2;
        return;
    }

    if((value <= lowlimit)
            && (eNotActive == AlarmTask::S_GetInstance ()->GetIsActive (alarmID)))
    {
        ++activetime;
    }
    else
    {
        activetime = RESET;
    }

    if(activetime >= O2TIMELIMIT)
    {
        AlarmTask::S_GetInstance ()->SetIsActive (alarmID, eActive);
    }

    if((value > lowlimit)
            && (eActive == AlarmTask::S_GetInstance ()->GetIsActive (alarmID)))
    {
        ++notactivetime;
    }
    else
    {
        notactivetime = RESET;
    }

    if(notactivetime >= O2RESETTIMELIMIT)
    {
        AlarmTask::S_GetInstance ()->SetIsActive (alarmID, eNotActive);
    }

}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CheckHiInternalO2  
//
//    Processing: 	If the Internal O2 is higher than the limit for 30 seconds
//					an alarm is activated.  The raw counts are linear.
//					
//					  0% O2 = 0 counts
//					 21% O2 = 477 counts
//					 25% O2 = 568 counts
//					100% O2 = 2272 counts
//
//					This alarm is not resettable or silenceable. 
//
//    Input Parameters: LONG value in raw counts 
//
//    Output Parameters: None 
//
//    Return Values: None 
//
//    Pre-conditions: None 
//
//    Miscellaneous: None
//
//    Requirements: None      
//
/******************************************************************************/
void PdAlarms::CheckHiInternalO2 (LONG value)
{
    // 25 percent O2 concentration
    const LONG HILIMIT = 568;
    static ULONG activetime = 0;
    const ULONG RESET = 0;

    if(value >= HILIMIT)
    {
        activetime++;
    }
    else
    {
        activetime = RESET;
    }

#ifndef AARC
    if(activetime >= O2TIMELIMIT)
    {
        AlarmTask::S_GetInstance ()->SetIsActive (eHighInternalO2, eActive);
    }
#endif

}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ResetO2Time
//
//    Processing:   Reset O2's Timer and alarm status
//
//    Input Parameters:
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-conditions: None
//
//    Miscellaneous:  None
//
//    Requirements:  None
//
/******************************************************************************/
void PdAlarms::ResetO2Time()
{
    //30 sec
    mHighO2AlarmDelayTimeAfterFlushO2 = 30;
    //30 sec
    mLowO2AlarmDelayTimeAfterFlushO2 = 30;

    //Reset alarm status of eHighO2 and eLowO2
    AlarmTask::S_GetInstance()->SetIsActive(eHighO2, eNotActive);
    AlarmTask::S_GetInstance()->SetIsActive(eLowO2, eNotActive);
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CheckLowAmlpitude
//
//    Processing:     If the Amplitude is lower than the limit for 3 seconds
//                    an alarm is activated.
//
//    Input Parameters:
//            LONG lowlimit
//            LONG value
//            AlarmStat* alarmtype
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-conditions: None
//
//    Miscellaneous:  None
//
//    Requirements:  None
//
/******************************************************************************/
void PdAlarms::CheckLowAmlpitude (LONG lowlimit, LONG value, int alarmID)
{
    static ULONG activetime = 0;
    const ULONG RESET = 0;
    const ULONG ACTIVE_TIME_LIMIT = 6;

    if (value < lowlimit)
    {
        activetime++;
        if(activetime > ACTIVE_TIME_LIMIT)
        {
            activetime = RESET;
            if(eNotActive == AlarmTask::S_GetInstance()->GetIsActive(alarmID))
            {
                AlarmTask::S_GetInstance()->SetIsActive(alarmID, eActive);
            }
        }
    }
    else
    {
        activetime = RESET;
        if(eActive == AlarmTask::S_GetInstance()->GetIsActive(alarmID))
        {
            AlarmTask::S_GetInstance()->SetIsActive(alarmID, eNotActive);
        }
    }
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CheckHighAmlpitude
//
//    Processing:     If the Amplitude is lower than the limit for 3 seconds
//                    an alarm is activated.
//
//    Input Parameters:
//            LONG lowlimit
//            LONG value
//            AlarmStat* alarmtype
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-conditions: None
//
//    Miscellaneous:  None
//
//    Requirements:  None
//
/******************************************************************************/
void PdAlarms::CheckHighAmlpitude (LONG hilimit, LONG value, int alarmID)
{
    static ULONG activetime = 0;
    const ULONG RESET = 0;
    const ULONG ACTIVE_TIME_LIMIT = 6;

    if (value > hilimit)
    {
        activetime++;
        if(activetime > ACTIVE_TIME_LIMIT)
        {

            activetime = RESET;
            if(eNotActive == AlarmTask::S_GetInstance()->GetIsActive(alarmID))
            {
                AlarmTask::S_GetInstance()->SetIsActive(alarmID, eActive);
            }
        }
    }
    else
    {
        activetime = RESET;
        if(eActive == AlarmTask::S_GetInstance()->GetIsActive(alarmID))
        {
            AlarmTask::S_GetInstance()->SetIsActive(alarmID, eNotActive);
        }
    }
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CheckLowBaseLine
//
//    Processing:     If the Base Line is lower than the limit for 5 times
//                    an alarm is activated.
//
//    Input Parameters:
//            LONG lowlimit
//            LONG value
//            AlarmStat* alarmtype
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-conditions: None
//
//    Miscellaneous:  None
//
//    Requirements:  None
//
/******************************************************************************/
void PdAlarms::CheckLowBaseLine (LONG lowlimit, LONG value, int alarmID)
{
    const ULONG LOW_PEEP_ACTIVE_TIME_LIMIT = 5;
    const ULONG RESET = 0;

    if (value < lowlimit)
    {
        lowPEEPActiveTime++;

        if(lowPEEPActiveTime >= LOW_PEEP_ACTIVE_TIME_LIMIT)
        {
            lowPEEPActiveTime = RESET;

            if(eActive != AlarmTask::S_GetInstance()->GetIsActive(alarmID))
            {
                AlarmTask::S_GetInstance()->SetIsActive(alarmID, eActive);
            }
        }
    }
    else
    {
        lowPEEPActiveTime = RESET;

        if(eNotActive != AlarmTask::S_GetInstance()->GetIsActive(alarmID))
        {
            AlarmTask::S_GetInstance()->SetIsActive(alarmID, eNotActive);
        }
    }
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CheckToClearBaseLine
//
//    Processing:
//
//    Input Parameters:
//            LONG lowlimit
//            LONG value
//            AlarmStat* alarmtype
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-conditions: None
//
//    Miscellaneous:  None
//
//    Requirements:  None
//
/******************************************************************************/
void PdAlarms::CheckLowBaseLine2Secs (LONG lowlimit, LONG value, int alarmID)
{
    const ULONG RESET = 0;
    const ULONG NUMBER_OF_2SEC = 2;
    if (value > lowlimit)
    {
        lowPEEP2SecActiveTime = RESET;
        lowPEEP2SecClearTime += NUMBER_OF_2SEC;

        if(lowPEEP2SecClearTime >= TWO_SECOND)
        {
            lowPEEP2SecClearTime = RESET;
            if(eNotActive != AlarmTask::S_GetInstance()->GetIsActive(alarmID))
            {
                AlarmTask::S_GetInstance()->SetIsActive(alarmID, eNotActive);
            }
        }
    }
    else if(value < lowlimit)
    {
        lowPEEP2SecClearTime = RESET;
        lowPEEP2SecActiveTime += NUMBER_OF_2SEC;

        if(lowPEEP2SecActiveTime >= TWO_SECOND)
        {
            lowPEEP2SecActiveTime = RESET;
            if(eActive != AlarmTask::S_GetInstance()->GetIsActive(alarmID))
            {
                AlarmTask::S_GetInstance()->SetIsActive(alarmID, eActive);
            }
        }
    }
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CheckHiBaseLine
//
//    Processing:     If the Base Line is higher than the limit for 5 times
//                    an alarm is activated.
//
//    Input Parameters:
//            LONG lowlimit
//            LONG value
//            AlarmStat* alarmtype
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-conditions: None
//
//    Miscellaneous:  None
//
//    Requirements:  None
//
/******************************************************************************/
void PdAlarms::CheckHiBaseLine (LONG hilimit, LONG value, int alarmID)
{
    const ULONG HIGH_PEEP_ACTIVE_TIME_LIMIT = 5;
    const ULONG RESET = 0;

    if (value > hilimit)
    {
        highPEEPActiveTime++;
        if(highPEEPActiveTime >= HIGH_PEEP_ACTIVE_TIME_LIMIT)
        {
            highPEEPActiveTime = RESET;
            if(eActive != AlarmTask::S_GetInstance()->GetIsActive(alarmID))
            {
                AlarmTask::S_GetInstance()->SetIsActive(alarmID, eActive);
            }
        }
    }
    else
    {
        highPEEPActiveTime = RESET;
        if(eNotActive != AlarmTask::S_GetInstance()->GetIsActive(alarmID))
        {
            AlarmTask::S_GetInstance()->SetIsActive(alarmID, eNotActive);
        }
    }
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CheckHiBaseLine
//
//    Processing:     If the Base Line is higher than the limit for 5 times
//                    an alarm is activated.
//
//    Input Parameters:
//            LONG lowlimit
//            LONG value
//            AlarmStat* alarmtype
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-conditions: None
//
//    Miscellaneous:  None
//
//    Requirements:  None
//
/******************************************************************************/
void PdAlarms::CheckHiBaseLine2Secs (LONG lowlimit, LONG value, int alarmID)
{
    const ULONG RESET = 0;
    const ULONG NUMBER_OF_2SEC = 2;

    if (value < lowlimit)
    {
        highPEEP2SecActiveTime = RESET;
        highPEEP2SecClearTime += NUMBER_OF_2SEC;
        if(highPEEP2SecClearTime >= TWO_SECOND)
        {

            highPEEP2SecClearTime = RESET;
            if(eNotActive != AlarmTask::S_GetInstance()->GetIsActive(alarmID))
            {
                AlarmTask::S_GetInstance()->SetIsActive(alarmID, eNotActive);
            }
        }
    }
    else
    {
        highPEEP2SecClearTime = RESET;
        highPEEP2SecActiveTime += NUMBER_OF_2SEC;
        if(highPEEP2SecActiveTime >= TWO_SECOND)
        {

            highPEEP2SecActiveTime = RESET;
            if(eActive != AlarmTask::S_GetInstance()->GetIsActive(alarmID))
            {
                AlarmTask::S_GetInstance()->SetIsActive(alarmID, eActive);
            }
        }
    }
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CheckHiPip
//
//    Processing:     If the PIP is higher than the limit for 5 times
//                    an alarm is activated.
//
//    Input Parameters:
//            LONG lowlimit
//            LONG value
//            AlarmStat* alarmtype
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-conditions: None
//
//    Miscellaneous:  None
//
//    Requirements:  None
//
/******************************************************************************/
void PdAlarms::CheckHiPip (LONG hilimit, LONG value, int alarmID)
{
    if (value < hilimit)
    {
        AlarmTask::S_GetInstance()->SetIsActive(alarmID, eNotActive);
    }
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CheckHiPip
//
//    Processing:     If the PIP is higher than the limit for 5 times
//                    an alarm is activated.
//
//    Input Parameters:
//            LONG lowlimit
//            LONG value
//            AlarmStat* alarmtype
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-conditions: None
//
//    Miscellaneous:  None
//
//    Requirements:  None
//
/******************************************************************************/
void PdAlarms::CheckLowPip (LONG lowlimit, LONG value, int alarmID)
{
    static ULONG activetime = 0;

    if (value < lowlimit)
    {
        activetime++;
        if(activetime > 1)
        {
            if(eActive != AlarmTask::S_GetInstance()->GetIsActive(alarmID))
            {
                AlarmTask::S_GetInstance()->SetIsActive(alarmID, eActive);
            }
        }
    }
    else
    {
        activetime = 0;
        if(eNotActive != AlarmTask::S_GetInstance()->GetIsActive(alarmID))
        {
            AlarmTask::S_GetInstance()->SetIsActive(alarmID, eNotActive);
        }
    }
}
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CheckLowAmlpitude
//
//    Processing:     If the Amplitude is lower than the limit for 3 seconds
//                    an alarm is activated.
//
//    Input Parameters:
//            LONG lowlimit
//            LONG value
//            AlarmStat* alarmtype
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-conditions: None
//
//    Miscellaneous:  None
//
//    Requirements:  None
//
/******************************************************************************/
void PdAlarms::CheckLowMap (LONG lowlimit, LONG value, int alarmID)
{
    static ULONG activetime = 0;
    const ULONG ACTIVE_TIME_LIMIT = 6;
    const ULONG RESET = 0;

    if (value < lowlimit)
    {
        activetime++;
        if(activetime > ACTIVE_TIME_LIMIT)
        {
            activetime = RESET;
            if(eNotActive == AlarmTask::S_GetInstance()->GetIsActive(alarmID))
            {
                AlarmTask::S_GetInstance()->SetIsActive(alarmID, eActive);
            }
        }
    }
    else
    {
        activetime = RESET;
        if(eActive == AlarmTask::S_GetInstance()->GetIsActive(alarmID))
        {
            AlarmTask::S_GetInstance()->SetIsActive(alarmID, eNotActive);
        }
    }
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CheckHiPip
//
//    Processing:     If the PIP is higher than the limit for 5 times
//                    an alarm is activated.
//
//    Input Parameters:
//            LONG lowlimit
//            LONG value
//            AlarmStat* alarmtype
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-conditions: None
//
//    Miscellaneous:  None
//
//    Requirements:  None
//
/******************************************************************************/
void PdAlarms::CheckHiMap (LONG hilimit, LONG value, int alarmID)
{
    static ULONG activetime = 0;
    const ULONG RESET = 0;
    const ULONG ACTIVE_TIME_LIMIT = 5;

    if (value > hilimit)
    {
        activetime++;
        if(activetime > ACTIVE_TIME_LIMIT)
        {
            if(eActive != AlarmTask::S_GetInstance()->GetIsActive(alarmID))
            {
                AlarmTask::S_GetInstance()->SetIsActive(alarmID, eActive);
            }
        }
    }
    else
    {
        activetime = RESET;
        if(eNotActive != AlarmTask::S_GetInstance()->GetIsActive(alarmID))
        {
            AlarmTask::S_GetInstance()->SetIsActive(alarmID, eNotActive);
        }
    }
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CheckLowAmlpitude
//
//    Processing:     If the Amplitude is lower than the limit for 3 seconds
//                    an alarm is activated.
//
//    Input Parameters:
//            LONG lowlimit
//            LONG value
//            AlarmStat* alarmtype
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-conditions: None
//
//    Miscellaneous:  None
//
//    Requirements:  None
//
/******************************************************************************/
void PdAlarms::CheckLowMinuteVol (LONG lowlimit, LONG value, int alarmID)
{
    static ULONG activetime = 0;
    const ULONG RESET = 0;
    const ULONG ACTIVE_TIME_LIMIT = 5;

    if (value < lowlimit)
    {
        activetime++;
        if(activetime > ACTIVE_TIME_LIMIT)
        {
            activetime = RESET;
            if(eNotActive == AlarmTask::S_GetInstance()->GetIsActive(alarmID))
            {
                AlarmTask::S_GetInstance()->SetIsActive(alarmID, eActive);
            }
        }
    }
    else
    {
        activetime = RESET;
        if(eActive == AlarmTask::S_GetInstance()->GetIsActive(alarmID))
        {
            AlarmTask::S_GetInstance()->SetIsActive(alarmID, eNotActive);
        }
    }
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CheckHiMinuteVol
//
//    Processing:     If the MV is higher than the limit for 5 times
//                    an alarm is activated.
//
//    Input Parameters:
//            LONG lowlimit
//            LONG value
//            AlarmStat* alarmtype
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-conditions: None
//
//    Miscellaneous:  None
//
//    Requirements:  None
//
/******************************************************************************/
void PdAlarms::CheckHiMinuteVol (LONG hilimit, LONG value, int alarmID)
{
    static ULONG activetime = 0;
    const ULONG RESET = 0;
    const ULONG ACTIVE_TIME_LIMIT = 5;

    if (value > hilimit)
    {
        activetime++;
        if(activetime > ACTIVE_TIME_LIMIT)
        {
            if(eNotActive != AlarmTask::S_GetInstance()->GetIsActive(alarmID))
            {
                AlarmTask::S_GetInstance()->SetIsActive(alarmID, eNotActive);
            }
        }
    }
    else
    {
        activetime = RESET;
        if(eNotActive != AlarmTask::S_GetInstance()->GetIsActive(alarmID))
        {
            AlarmTask::S_GetInstance()->SetIsActive(alarmID, eNotActive);
        }
    }
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CheckHiPip
//
//    Processing:     If the PIP is higher than the limit for 5 times
//                    an alarm is activated.
//
//    Input Parameters:
//            LONG lowlimit
//            LONG value
//            AlarmStat* alarmtype
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-conditions: None
//
//    Miscellaneous:  None
//
//    Requirements:  None
//
/******************************************************************************/
void PdAlarms::CheckHiBreathRate (LONG hilimit, LONG value, int alarmID)
{
    static ULONG activetime = 0;
    const ULONG RESET = 0;
    const ULONG ACTIVE_TIME_LIMIT = 5;

    if (value > hilimit)
    {
        activetime++;
        if(activetime > ACTIVE_TIME_LIMIT)
        {
            if(eNotActive != AlarmTask::S_GetInstance()->GetIsActive(alarmID))
            {
                AlarmTask::S_GetInstance()->SetIsActive(alarmID, eNotActive);
            }
        }
    }
    else
    {
        activetime = RESET;
        if(eNotActive != AlarmTask::S_GetInstance()->GetIsActive(alarmID))
        {
            AlarmTask::S_GetInstance()->SetIsActive(alarmID, eNotActive);
        }
    }
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ResetAlarmActiveTime
//
//    Processing:   Reset alarm Timer and alarm status
//
//    Input Parameters:
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-conditions: None
//
//    Miscellaneous:  None
//
//    Requirements:  None
//
/******************************************************************************/
void PdAlarms::ResetAlarmActiveTime ()
{
    highPEEPActiveTime = 0;
    lowPEEPActiveTime = 0;

    highPEEP2SecActiveTime = 0;
    lowPEEP2SecActiveTime = 0;

    lowPEEP2SecClearTime = 0;
    highPEEP2SecClearTime = 0;

    ResetPLowAlarmActiveTime();
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ResetPLowAlarmActiveTime
//
//    Processing:   Reset low alarm Timer and alarm status
//
//    Input Parameters:
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-conditions: None
//
//    Miscellaneous:  None
//
//    Requirements:  None
//
/******************************************************************************/
void PdAlarms::ResetPLowAlarmActiveTime ()
{
    highPLow1SecActiveTime = 0;
    lowPLow1SecActiveTime = 0;

    highPLow1SecClearTime = 0;
    lowPLow1SecClearTime = 0;

    highPHigh2SecActiveTime = 0;
    lowPHigh2SecActiveTime = 0;

    lowPHigh2SecClearTime = 0;
    highPHigh2SecClearTime = 0;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ResetPLowAlarmActiveTime
//
//    Processing:   Reset low alarm Timer and alarm status
//
//    Input Parameters:
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-conditions: None
//
//    Miscellaneous:  None
//
//    Requirements:  None
//
/******************************************************************************/
void PdAlarms::ResetPEEPAlarmActiveTime ()
{
    highPEEP2SecActiveTime = 0;
    lowPEEP2SecActiveTime = 0;

    lowPEEP2SecClearTime = 0;
    highPEEP2SecClearTime = 0;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CheckLow_PLow
//
//    Processing:
//
//    Input Parameters:
//            LONG lowlimit
//            LONG value
//            AlarmStat* alarmtype
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-conditions: None
//
//    Miscellaneous:  None
//
//    Requirements:  None
//
/******************************************************************************/
void PdAlarms::CheckLow_PLow (LONG lowlimit, LONG value, int alarmID)
{
    const ULONG RESET = 0;
    const ULONG NUMBER_OF_LOWPLOW_ACTIVE_ONE_TIMES = 2;

    if (value < lowlimit)
    {
        lowPLow1SecClearTime = RESET;
        lowPLow1SecActiveTime += NUMBER_OF_LOWPLOW_ACTIVE_ONE_TIMES ;
        if(lowPLow1SecActiveTime >= ONE_SECOND)
        {
            if(eActive != AlarmTask::S_GetInstance()->GetIsActive(alarmID))
            {
                AlarmTask::S_GetInstance()->SetIsActive(alarmID, eActive);
            }
        }
    }
    else{
        lowPLow1SecActiveTime = RESET;
        lowPLow1SecClearTime += NUMBER_OF_LOWPLOW_ACTIVE_ONE_TIMES ;
        if(lowPLow1SecClearTime >= ONE_SECOND)
        {
            if(eNotActive != AlarmTask::S_GetInstance()->GetIsActive(alarmID))
            {
                AlarmTask::S_GetInstance()->SetIsActive(alarmID, eNotActive);
            }
        }
    }
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CheckHigh_PLow
//
//    Processing:     If the PIP is higher than the limit for 5 times
//                    an alarm is activated.
//
//    Input Parameters:
//            LONG lowlimit
//            LONG value
//            AlarmStat* alarmtype
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-conditions: None
//
//    Miscellaneous:  None
//
//    Requirements:  None
//
/******************************************************************************/
void PdAlarms::CheckHigh_PLow (LONG hilimit, LONG value, int alarmID)
{
    const ULONG RESET = 0;
    const ULONG NUMBER_OF_HIGHPLOW_ONE_TIMES = 2;

    if (value > hilimit)
    {
        highPLow1SecClearTime = RESET;
        highPLow1SecActiveTime += NUMBER_OF_HIGHPLOW_ONE_TIMES;

        if(highPLow1SecActiveTime >= ONE_SECOND)
        {
            highPLow1SecActiveTime = 0;
            if(eActive != AlarmTask::S_GetInstance()->GetIsActive(alarmID))
            {
                AlarmTask::S_GetInstance()->SetIsActive(alarmID, eActive);
            }
        }
    }
    else
    {
        highPLow1SecActiveTime = RESET ;
        highPLow1SecClearTime += NUMBER_OF_HIGHPLOW_ONE_TIMES;
        if(highPLow1SecClearTime >= ONE_SECOND)
        {
            if(eNotActive != AlarmTask::S_GetInstance()->GetIsActive(alarmID))
            {
                AlarmTask::S_GetInstance()->SetIsActive(alarmID, eNotActive);
            }
        }
    }
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CheckLowPLow5Time
//
//    Processing:     If the Pressure is lower than the limit for 5 times
//                    an alarm is activated.
//
//    Input Parameters:
//            LONG lowlimit
//            LONG value
//            AlarmStat* alarmtype
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-conditions: None
//
//    Miscellaneous:  None
//
//    Requirements:  None
//
/******************************************************************************/
void PdAlarms::CheckLowPLow5Time (LONG lowlimit, LONG value, int alarmID)
{
    const ULONG RESET = 0;
    const ULONG ACTIVE_TIME_LIMIT = 5;
    if (value < lowlimit)
    {
        lowPLowActive5Times++;
        if(lowPLowActive5Times >= ACTIVE_TIME_LIMIT)
        {
            lowPLowActive5Times = RESET;
            if(eActive != AlarmTask::S_GetInstance()->GetIsActive(alarmID))
            {
                AlarmTask::S_GetInstance()->SetIsActive(alarmID, eActive);
            }
        }
    }
    else
    {
        lowPLowActive5Times = RESET;
        if(eNotActive != AlarmTask::S_GetInstance()->GetIsActive(alarmID))
        {
            AlarmTask::S_GetInstance()->SetIsActive(alarmID, eNotActive);
        }
    }
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CheckHiPLow5Time
//
//    Processing:     If the Pressure is higher than the limit for 5 times
//                    an alarm is activated.
//
//    Input Parameters:
//            LONG lowlimit
//            LONG value
//            AlarmStat* alarmtype
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-conditions: None
//
//    Miscellaneous:  None
//
//    Requirements:  None
//
/******************************************************************************/
void PdAlarms::CheckHighPLow5Time (LONG hilimit, LONG value, int alarmID)
{
    const ULONG RESET = 0;
    const ULONG ACTIVE_TIME_LIMIT = 5;

    if (value > hilimit)
    {
        highPLowActive5Times++;
        if(highPLowActive5Times >= ACTIVE_TIME_LIMIT)
        {
            highPLowActive5Times = RESET;
            if(eActive != AlarmTask::S_GetInstance()->GetIsActive(alarmID))
            {
                AlarmTask::S_GetInstance()->SetIsActive(alarmID, eActive);
            }
        }
    }
    else
    {
        highPLowActive5Times = RESET;
        if(eNotActive != AlarmTask::S_GetInstance()->GetIsActive(alarmID))
        {
            AlarmTask::S_GetInstance()->SetIsActive(alarmID, eNotActive);
        }
    }

}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CheckLow_PHigh
//
//    Processing:     If the PIP is higher than the limit for 5 times
//                    an alarm is activated.
//
//    Input Parameters:
//            LONG lowlimit
//            LONG value
//            AlarmStat* alarmtype
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-conditions: None
//
//    Miscellaneous:  None
//
//    Requirements:  None
//
/******************************************************************************/
void PdAlarms::CheckLow_PHigh (LONG lowlimit, LONG value, int alarmID)
{
    const ULONG RESET = 0;
    const ULONG NUMBER_OF_LOEHPHIGH_ONE_TIMES = 2;

    if (value < lowlimit)
    {
        lowPHigh2SecClearTime = RESET;
        lowPHigh2SecActiveTime += NUMBER_OF_LOEHPHIGH_ONE_TIMES ;
        if(lowPHigh2SecActiveTime >= TWO_SECOND)
        {
            if(eActive != AlarmTask::S_GetInstance()->GetIsActive(alarmID))
            {
                AlarmTask::S_GetInstance()->SetIsActive(alarmID, eActive);
            }
        }
    }
    else{
        lowPHigh2SecActiveTime = RESET;
        lowPHigh2SecClearTime += NUMBER_OF_LOEHPHIGH_ONE_TIMES;
        if(lowPHigh2SecClearTime >= TWO_SECOND)
        {
            if(eNotActive != AlarmTask::S_GetInstance()->GetIsActive(alarmID))
            {
                AlarmTask::S_GetInstance()->SetIsActive(alarmID, eNotActive);
            }
        }
    }
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CheckHigh_PHigh
//
//    Processing:     If the PIP is higher than the limit for 2 secs
//                    an alarm is activated.
//
//    Input Parameters:
//            LONG lowlimit
//            LONG value
//            AlarmStat* alarmtype
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-conditions: None
//
//    Miscellaneous:  None
//
//    Requirements:  None
//
/******************************************************************************/
void PdAlarms::CheckHigh_PHigh (LONG hilimit, LONG value, int alarmID)
{
    const ULONG RESET = 0;
    const ULONG NUMBER_OF_LOEHPHIGH_ONE_TIMES = 2;

    if (value > hilimit)
    {
        highPHigh2SecClearTime = RESET;
        highPHigh2SecActiveTime += NUMBER_OF_LOEHPHIGH_ONE_TIMES ;
        if(highPHigh2SecActiveTime >= TWO_SECOND)
        {
            if(eActive != AlarmTask::S_GetInstance()->GetIsActive(alarmID))
            {
                AlarmTask::S_GetInstance()->SetIsActive(alarmID, eActive);
            }
        }
    }else{
        highPHigh2SecActiveTime = RESET;
        highPHigh2SecClearTime += NUMBER_OF_LOEHPHIGH_ONE_TIMES;
        if(highPHigh2SecClearTime >= TWO_SECOND)
        {
            if(eNotActive != AlarmTask::S_GetInstance()->GetIsActive(alarmID))
            {
                AlarmTask::S_GetInstance()->SetIsActive(alarmID, eNotActive);
            }
        }
    }
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CheckHigh_PHigh
//
//    Processing:     If the PIP is higher than the limit for 5 times
//                    an alarm is activated.
//
//    Input Parameters:
//            LONG lowlimit
//            LONG value
//            AlarmStat* alarmtype
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-conditions: None
//
//    Miscellaneous:  None
//
//    Requirements:  None
//
/******************************************************************************/
void PdAlarms::CheckLowPHigh5Time (LONG lowlimit, LONG value, int alarmID)
{
    const ULONG RESET = 0;
    const ULONG ACTIVE_TIME_LIMIT = 5;

    if (value < lowlimit)
    {
        lowPHighActive5Times++;
        if(lowPHighActive5Times >= ACTIVE_TIME_LIMIT)
        {
            lowPHighActive5Times = RESET;
            if(eActive != AlarmTask::S_GetInstance()->GetIsActive(alarmID))
            {
                AlarmTask::S_GetInstance()->SetIsActive(alarmID, eActive);
            }
        }
    }
    else
    {
        lowPHighActive5Times = RESET;
        if(eNotActive != AlarmTask::S_GetInstance()->GetIsActive(alarmID))
        {
            AlarmTask::S_GetInstance()->SetIsActive(alarmID, eNotActive);
        }
    }
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CheckHigh_PHigh
//
//    Processing:     If the PIP is higher than the limit for 5 times
//                    an alarm is activated.
//
//    Input Parameters:
//            LONG lowlimit
//            LONG value
//            AlarmStat* alarmtype
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-conditions: None
//
//    Miscellaneous:  None
//
//    Requirements:  None
//
/******************************************************************************/
void PdAlarms::CheckHighPHigh5Time (LONG lowlimit, LONG value, int alarmID)
{
    const ULONG RESET = 0;

    if (value > lowlimit)
    {

    }
    else
    {
        highPHighActive5Times = RESET;
        if(eNotActive != AlarmTask::S_GetInstance()->GetIsActive(alarmID))
        {
            AlarmTask::S_GetInstance()->SetIsActive(alarmID, eNotActive);
        }
    }
}
