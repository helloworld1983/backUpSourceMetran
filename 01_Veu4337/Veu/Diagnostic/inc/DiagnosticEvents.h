//*****************************************************************************/
//$COMMON.H$
//   File Name:  DiagnosticEvents.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: DiagnosticEvents
//*****************************************************************************/

#ifndef DIAGNOSTICEVENTS_H
#define DIAGNOSTICEVENTS_H

// Diagnostic event enumeration
enum E_DiagnosticEvent 
{
    eNoEvent,

    // events from client (used to command self tests)
    eStartSSTEvent,
    eStartESTEvent,
    eCancelTestEvent,
    eSSTChildCompleteEvent,
    eESTChildCompleteEvent,
    eFailureEvent,
    eResponseEvent,
    eContextSwitchEvent,
    eHardwareTimerExpiredEvent,
    eSettingChangedEvent,
    eOutputChangeEvent
};

#endif //DIAGNOSTICEVENTS_H

