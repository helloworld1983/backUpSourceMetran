/******************************************************************************/
//
//$COMMON.CPP$
//    File Name: AlarmStat.cpp
//    Copyright 1996 InVentive Technologies, Inc.  All Rights Reserved.
//
//    Class: AlarmStat
//
//  The AlarmStat class category interfaces with several major class 
//  categories including the BreathManagement class (see Breath Delivery 
//  SRS for more information), the PtData class (see Patient Data SRS),
//  the Continuous Built-in Test (CBIT) class, and the DataIO class to perform
//  low level operations for turning on/off LEDs and alarm horn.  It is the 
//  responsibility of each application to perform the detection and auto-reset 
//  of each of their respective alarms.
/******************************************************************************/
#include "AlarmStat.h"
#include "DebugTrace.h"

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: AlarmStat   
//
//    Processing: Constructor for Alarm Status Class
//
//					Builds an alarm timer if needed based on intial and final
//					priorities.
//
//    Input Parameters: 
//      E_AlarmLevel initialPriority - assigns inital priority alarm level
//      E_AlarmLevel finalPriority - assigns final priority alarm level 
//		UNSIGNED tim - time for alarm elevation
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
AlarmStat::AlarmStat (E_GUIEventID id, E_AlarmLevel initialPriority,
                      E_AlarmLevel finalPriority, UNSIGNED tim, UNSIGNED occurent)
{
    const int TIME_IS_INVALID = 0;

    Id = id;
    Level = eInactive;
    InitialPriority = initialPriority;
    FinalPriority = finalPriority;
    IsActive = eNotActive;
    OldIsActive = eNotActive;
    Processed = eTrue;

    if(TIME_IS_INVALID != tim)
    {
        //Create a timer to evaluate
        AlarmTimer = new ElevationTimer (tim);
        //Check available creation
        ASSERTION(NULL != AlarmTimer);
        //Set default for Elevation flag
        AlarmTimer->ElevationFlag = eFalse;
    }
    else
    {
        AlarmTimer = NULL;
    }

    PreLevel = eInactive;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ExpirationRoutine  
//
//    Processing: 
//				Sets elevation flag to true to indicate that an alarm
//				needs to be elevated.
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
void ElevationTimer::ExpirationRoutine (void)
{
    ElevationFlag = eTrue;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: SetIsActive   
//
//    Processing: Set the IsActive flag
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values:
//					
//					 eSuccess if the Alarms task can process call
//                   eFail if the Alarm's task is busy request ignored
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: calls to this function should be no faster than the
//					Alarms task runs which is 200 ms.     
//
/******************************************************************************/
E_ReturnStatus AlarmStat::SetIsActive (E_AlarmStatus stat)
{
    E_ReturnStatus retval;

    if(Processed == eTrue)
    {
        IsActive = stat;
        Processed = eFalse;
        retval = eSuccess;
    }
    else
    {
        retval = eError;
    }

    return (retval);

}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GetIsActive
//
//    Processing:
//              Return that alarm status is active or not
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values: IsActive
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
E_AlarmStatus AlarmStat::GetIsActive(void)
{
    return IsActive;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: AlarmStat
//
//    Processing: Constructor for AlarmStat Status Class
//
//    Input Parameters:
//      UNSIGNED tim - time to set
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
ElevationTimer::ElevationTimer (UNSIGNED tim)
                : Timer("ElevationTimer", tim * TICKS_PER_SEC, pdTRUE)
{
    ElevationFlag = eFalse;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GetLevel
//
//    Processing:
//              Return Level of alarm
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values: IsActive
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
E_AlarmLevel AlarmStat::GetLevel (void)
{
    return Level;
}
