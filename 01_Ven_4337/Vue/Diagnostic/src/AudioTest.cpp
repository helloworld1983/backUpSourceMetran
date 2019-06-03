//$COMMON.CPP$
//   File Name:  AudioTest.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: To test the primary and backup audio alarms.
//
//      The following systems are checked by this class:
//
//      Primary Audio
//          Test Approach - Sound the audio an have the operator confirm that
//              sound is being produced.
//
//      Backup Audio
//          Test Approach - Sound the audio an have the operator confirm that
//              sound is being produced.
//
//   Interfaces:
//
//   Restrictions:
/******************************************************************************/

#include "AudioTest.h"
#include "Criterion.h"
#include "FailureCodes.h"
#include "TestServices.h"
//#include "GUI.h"

//#ifndef NO_HARDWARE
//  #include "Audible.h"
//  #include "DOutput.h"
//#endif

//*****************************************************************************/
//    Operation AudioTest()
//
//    Processing: Class constructor.
//
//    Input Parameters: 
//      *client - a pointer to the test client.
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:    None
//    Miscellaneous:     None
//    Requirements:      None
//
//*****************************************************************************/
AudioTest::AudioTest(SelfTest* parent) : 
    ChildTest(parent, eAudioTest)
{
}

//*****************************************************************************/
//    Operation Name: Start()
//
//    Processing: Event to start this test. 
//
//    Input Parameters:  None
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:    None
//    Miscellaneous:     None
//    Requirements:      None
//
//*****************************************************************************/
void AudioTest::Start(void)
{
    static const UCHAR StateTransitions[eST_MaxStates] = {
        // -New State-                    -Current State-
		eST_SoundPrimaryAudio,          // ST_Idle
		eST_CannotHappen,               // ST_Complete
		eST_CannotHappen,               // ST_Cancel
        eST_CannotHappen,               // ST_SoundPrimaryAudio
        eST_CannotHappen,               // ST_VerifyPrimaryAudio
        eST_CannotHappen,               // ST_SoundBackupAudio
        eST_CannotHappen,               // ST_VerifyBackupAudio
        eST_CannotHappen,               // ST_SoundRemoteAlarm
        eST_CannotHappen                // ST_VerifyRemoteAlarm
    };
    
    ASSERTION(StateTransitions[CurrentState] != eST_CannotHappen);
    ASSERTION(CurrentState < eST_CannotHappen);
    if (StateTransitions[CurrentState] != eST_EventIgnored)
    {
        NextState(StateTransitions[CurrentState]);
        StateEngine();
    }
}

//*****************************************************************************/
//    Operation Name: Cancel()
//
//    Processing: Event to cancel this test. 
//
//    Input Parameters:  None
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:    None
//    Miscellaneous:     None
//    Requirements:      None
//
//*****************************************************************************/
void AudioTest::Cancel(void)
{
    static const UCHAR StateTransitions[eST_MaxStates] = {
        // -New State-                    -Current State-
		eST_EventIgnored,               // ST_Idle
        eST_CannotHappen,               // ST_Complete
		eST_CannotHappen,               // ST_Cancel
        eST_Cancel,                     // ST_SoundPrimaryAudio
        eST_Cancel,                     // ST_VerifyPrimaryAudio
        eST_Cancel,                     // ST_SoundBackupAudio
        eST_Cancel,                     // ST_VerifyBackupAudio
        eST_Cancel,                     // ST_SoundRemoteAlarm
        eST_Cancel                      // ST_VerifyRemoteAlarm
    };
    
    ASSERTION(StateTransitions[CurrentState] != eST_CannotHappen);
    ASSERTION(CurrentState < eST_CannotHappen);
    if (StateTransitions[CurrentState] != eST_EventIgnored)
    {
        NextState(StateTransitions[CurrentState]);
        StateEngine();
    }
}

//*****************************************************************************/
//    Operation Name: Response()
//
//    Processing: Event indicating a response from the client. 
//
//    Input Parameters:
//      pData - pointer to response data.
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:    None
//    Miscellaneous:     None
//    Requirements:      None
//
//*****************************************************************************/
void AudioTest::Response(ResponseData* pData) 
{
    static const UCHAR StateTransitions[eST_MaxStates] = {
        // -New State-                    -Current State-
		eST_CannotHappen,               // ST_Idle
		eST_CannotHappen,               // ST_Complete
		eST_CannotHappen,               // ST_Cancel
        eST_VerifyPrimaryAudio,         // ST_SoundPrimaryAudio
        eST_CannotHappen,               // ST_VerifyPrimaryAudio
        eST_VerifyBackupAudio,          // ST_SoundBackupAudio
        eST_CannotHappen,               // ST_VerifyBackupAudio
        eST_VerifyRemoteAlarm,          // ST_SoundRemoteAlarm
        eST_CannotHappen                // ST_VerifyRemoteAlarm
    };
    
    ASSERTION(StateTransitions[CurrentState] != eST_CannotHappen);
    ASSERTION(CurrentState < eST_CannotHappen);
    if (StateTransitions[CurrentState] != eST_EventIgnored)
    {
        NextState(StateTransitions[CurrentState], pData);
        StateEngine();
    }
}

//*****************************************************************************/
//    Operation Name: ST_SoundPrimaryAudio()
//
//    Processing: Sound the primary audio, then ask the operator if it is 
//      sounding.
//
//    Input Parameters:  None
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:    None
//    Miscellaneous:     None
//    Requirements:      None
//
//*****************************************************************************/
void AudioTest::ST_SoundPrimaryAudio(void)
{
    // set this test as active so all client events will route here
    S_SetActiveTest(this);
    
    // start the high sound audio
//    Audible::S_GetInstance()->AlarmSet(eHighSound);

    // ask operator to confirm the audio is sounding
//    TheGUI().GetResponse(ePrimaryAudioMsg);
}

//*****************************************************************************/
//    Operation Name: ST_VerifyPrimaryAudio()
//
//    Processing: The user was asked if the primary audio was sounding. Now 
//      process the response to that question.
//
//    Input Parameters:
//      pData - the response data obtained from the user.
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:    None
//    Miscellaneous:     None
//    Requirements:      None
//
//*****************************************************************************/
void AudioTest::ST_VerifyPrimaryAudio(ResponseData *pData)
{
    CRITERION(PrimaryAudio, int, 0, eEqual, eTrue, 0, eNoString,
        eFC_PrimaryAudioNotSounding)

#ifndef NO_HARDWARE
    // turn off all audio
//    Audible::S_GetInstance()->AlarmSet(eTerminateNow);
#endif

    // handle user response to the question of whether high audio is sounding
    switch (pData->response) 
    { 
        case eRejectResponse: 
//            VERIFY_RESULT(PrimaryAudio, eFalse)
            break; 
        case eAcceptResponse: 
            VERIFY_RESULT(PrimaryAudio, eTrue)
            break;
        default:
            break; 
    }

    NextState(eST_SoundBackupAudio);
}

//*****************************************************************************/
//    Operation Name: ST_SoundBackupAudio()
//
//    Processing: Sound the backup audio, then ask the operator if it is 
//      sounding.
//
//    Input Parameters:  None
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:    None
//    Miscellaneous:     None
//    Requirements:      None
//
//*****************************************************************************/
void AudioTest::ST_SoundBackupAudio(void)
{
    // turn on backup alarm
//    BackupAlarm->SetStateDirect(eOn);

    // ask operator to confirm the audio is sounding
//    TheGUI().GetResponse(eBackupAudioMsg);
}

//*****************************************************************************/
//    Operation Name: ST_VerifyBackupAudio()
//
//    Processing: The user was asked if the backup audio was sounding. Now
//      process the response to that question.
//
//    Input Parameters:
//      pData - the response data obtained from the user.
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:    None
//    Miscellaneous:     None
//    Requirements:      None
//
//*****************************************************************************/
void AudioTest::ST_VerifyBackupAudio(ResponseData *pData)
{
    CRITERION(BackupAudio, int, 0, eEqual, eTrue, 0, eNoString,
        eFC_BackupAudioNotSounding)

    // turn off backup alarm
//    BackupAlarm->SetStateDirect(eOff);

    // handle user response to the question of whether high audio is sounding
    switch (pData->response) 
    { 
        case eRejectResponse: 
            VERIFY_RESULT(BackupAudio, eFalse)
            break; 
        case eAcceptResponse: 
            VERIFY_RESULT(BackupAudio, eTrue)
            break; 
        default:
            break;
    }

    NextState(eST_SoundRemoteAlarm);
}

//*****************************************************************************/
//    Operation Name: ST_SoundRemoteAlarm()
//
//    Processing: Enable the remote alarm, then ask the operator if it is on.
//
//    Input Parameters:  None
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:    None
//    Miscellaneous:     None
//    Requirements:      None
//
//*****************************************************************************/
void AudioTest::ST_SoundRemoteAlarm(void)
{
    // Turn on the Remote Alarm
//    RemoteAlarm->SetState(eOn);

    // ask operator to confirm the alarm is on
//    TheGUI().GetResponse(eRemoteAlarmMsg);
}

//*****************************************************************************/
//    Operation Name: ST_VerifyRemoteAlarm()
//
//    Processing: The user was asked if the remote alarm was enabled. Now
//      process the response to that question.
//
//    Input Parameters:
//      pData - the response data obtained from the user.
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:    None
//    Miscellaneous:     None
//    Requirements:      None
//
//*****************************************************************************/
void AudioTest::ST_VerifyRemoteAlarm(ResponseData *pData)
{
    CRITERION(RemoteAlarmOn, int, 0, eEqual, eTrue, 0, eNoString,
        eFC_RemoteAlarmNotSounding)

    // turn off remote alarm
//    RemoteAlarm->SetStateDirect(eOff);

    // handle user response to remote alarm
    switch (pData->response) 
    { 
        case eRejectResponse: 
            VERIFY_RESULT(RemoteAlarmOn, eFalse)
            break; 
        case eAcceptResponse: 
            VERIFY_RESULT(RemoteAlarmOn, eTrue)
            break; 
        default:
            break;
    }

    NextState(eST_Complete);
}

//*****************************************************************************/
//    Operation Name: DoStateAction()
//
//    Processing: Perform the required state processing. Depending on the state
//      machine's current state, call the appropriate method.
//
//    Input Parameters:
//      pData - event data to send to the state.
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:    None
//    Miscellaneous:     None
//    Requirements:      None
//
//*****************************************************************************/
void AudioTest::DoStateAction(EventData* pData)
{
    // function prototype each state method must conform to
	typedef void (AudioTest::*pFunc)(EventData *);

    // state table with pointers to each state method
	static pFunc stateMachine[eST_MaxStates] = {
		(pFunc)&AudioTest::ST_Idle,
		(pFunc)&AudioTest::ST_Complete,
		(pFunc)&AudioTest::ST_Cancel,
        (pFunc)&AudioTest::ST_SoundPrimaryAudio,
        (pFunc)&AudioTest::ST_VerifyPrimaryAudio,
        (pFunc)&AudioTest::ST_SoundBackupAudio,
        (pFunc)&AudioTest::ST_VerifyBackupAudio,
        (pFunc)&AudioTest::ST_SoundRemoteAlarm,
        (pFunc)&AudioTest::ST_VerifyRemoteAlarm
	};

    // ensure current state is within bounds
    // execute the state method
    ASSERTION(CurrentState < eST_MaxStates);  
    (this->*stateMachine[CurrentState])(pData);
}




