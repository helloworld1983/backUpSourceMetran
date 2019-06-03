//*****************************************************************************/
//$COMMON.H$
//   File Name:  AudioTest.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: AudioTest
//      To test the primary and backup audio alarms.
//*****************************************************************************/

#ifndef AUDIOTEST_H
#define AUDIOTEST_H

#include "ChildTest.h"

class AudioTest : public ChildTest
{
public:
    AudioTest(SelfTest* parent);
    ~AudioTest() { }

    // events taken by this class
    void Start(void);
    void Cancel(void);
    void Response(ResponseData*);

protected:
private:
    // prevent copying
    AudioTest(const AudioTest&);
    void operator=(AudioTest&);

    // state machine states
    void ST_SoundPrimaryAudio(void);
    void ST_VerifyPrimaryAudio(ResponseData*);
    void ST_SoundBackupAudio(void);
    void ST_VerifyBackupAudio(ResponseData*);
    void ST_SoundRemoteAlarm(void);
    void ST_VerifyRemoteAlarm(ResponseData*);

    void DoStateAction(EventData* pData);

    // State enumeration order must match the order of state method pointers 
    // in stateMachine[] within DoStateAction().
    enum E_States 
    {
        eST_SoundPrimaryAudio = ChildTest::eST_MaxStates,
        eST_VerifyPrimaryAudio,
        eST_SoundBackupAudio,
        eST_VerifyBackupAudio,
        eST_SoundRemoteAlarm,
        eST_VerifyRemoteAlarm,
        eST_MaxStates,
        eST_EventIgnored,
        eST_CannotHappen
    };
};
#endif //AUDIOTEST_H

