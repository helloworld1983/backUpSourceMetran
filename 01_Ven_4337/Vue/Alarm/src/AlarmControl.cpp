/******************************************************************************/
//
//$COMMON.CPP$
//    File Name: AlarmControl.cpp
//    Copyright 1996 InVentive Technologies, Inc.  All Rights Reserved.
//
//    Class: AlarmControl
//
//  The AlarmControl class category interfaces with the DataIO class to perform
//  low level operations for silencing alarm horn and resetting alarms in the
//	auto-reset priority back to normal. Also controls the feed back for alarm
//  silence key.
/******************************************************************************/

// alarm includes
#include "AlarmStat.h"
#include "MgrAlarmStat.h"
#include "AlarmControl.h"

// dataIo  Key includes

// Led includes

// Audible includes for feedback sounds

const LONG SILENCETIME = (60 * 2);      // 2 mins
#define TIMETOGETSEM (( TickType_t ) 10)

AlarmControl * AlarmControl::S_Instance = NULL;

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: AlarmControl   
//
//    Processing: Constructor for Alarm Control Class
//
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
AlarmControl::AlarmControl (void)
                : Timer("AlarmControlTimer", SILENCETIME*TICKS_PER_SEC, pdTRUE)
{
    ResetFlag = eFalse;
    SilenceFlag = eFalse;
    SilenceOn = eFalse;

    //initialize for audio
    //	sndPtr = Audible::S_GetInstance();

}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ExpirationRoutine  
//
//    Processing: 
//				Sets SilenceOn flag to eFalse to indicate that the silence
//				duration is over
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
void AlarmControl::ExpirationRoutine (void)
{
    MgrAlarmStat * ptr = NULL;

    CancelSilence ();

    ptr = MgrAlarmStat::S_GetInstance ();

    ptr->Enunciate (NONEWALARMS);

}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name:  CancelSilence 
//
//    Processing: Perform processing associated with the canceling the silence
//				  operation
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
void AlarmControl::CancelSilence (void)
{

//	STATUS stat;
//
//	stat = Disable();
//	ASSERT(stat == NU_SUCCESS);
//
//	AlarmSilenceLED->SetState(eOff);

    SilenceOn = eFalse;
}


//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: Send()
//
//    Processing: This is for receiving the Alarm Silence and Reset keys
//
//    Input Parameters: E_DeviceId id - identification of the keys
//
//    Output Parameters: None
//
//    Return Values: E_ReturnStatus - sucess if key was registered for
//						              failure if key was not registered for
//
//
//    Pre-Conditions: None
//
//    Miscellaneous: None
//
//    Requirements: N/A  
//
//******************************************************************************
E_ReturnStatus AlarmControl::Send (E_DeviceId id)
{

    switch (id)
    {
        case eAlarmResetKey:
            // no audible sound made here
            // provided by Bd	
            SetResetFlag (eTrue);
            break;

        case eAlarmSilenceKey:
//			sndPtr->SetFeedback(eGoodBeep);
            SetSilenceFlag (eTrue);
            break;

        default:
            // should never get any other keys!
            ASSERTION(eFalse);
            break;
    };

    return (eSuccess);

}   // end Send() 
