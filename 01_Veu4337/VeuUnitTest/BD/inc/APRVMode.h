/******************************************************************************/
//$COMMON.H$
//   File Name:  APRVMode.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class:
//      This file contains the class declaration for the APRVMode Mode
//      which is derived from the SpontMode,Timer base class.
/******************************************************************************/
#ifndef APRVMODE_H
#define APRVMODE_H

#include "Timer.h"
#include "SpontMode.h"

enum E_TimeWindowType
{
    eNot_ReleaseWindow,
    eReleaseWindow
};
enum E_PressureWindowType
{
    ePLowWindow,
    ePHighWindow
};

#include "Timer.h"

class APRVMode: public SpontMode,Timer
{
public:
    //Pointer to the object instantiated from this class
    static APRVMode* S_GetInstance(void);

    //Process doing something when another mode enters this mode
    void Enter(E_VentilationMode fromModeId);

    //Process action for each ID trigger
    virtual void ProcessTriggerID(E_TriggerId id);

    //Process Inh Trigger
    virtual void ProcessPtInhTrigger(void);

    //Process Exh Trigger
    virtual void ProcessPtExhTrigger(void);

    //Process APRV Low Time Trigger
    virtual void ProcessAPRVPLowTimeTrigger(void);

    //Process APRV High Time Trigger
    virtual void ProcessAPRVPHighTimeTrigger(void);

    //Process ItLong Alarm Time Trigger
    virtual void ProcessItLongAlarmTimeTrigger(void);

    //Process Hip Alarm Trigger
    virtual void ProcessHipAlarmTrigger(void);

    //Process Apneal Alarm Time Trigger
    virtual void ProcessApneaAlarmTimeTrigger(void);

    //Process APRV Update Trigger
    virtual void ProcessAPRVUpdateBRTrigger(void);

    //Process HPHigh Alarm Trigger
    virtual void ProcessHPHighAlarmTrigger(void);

    //Process doing something when it exits mode
    void Exit();

    //Copy constructor
    APRVMode(const APRVMode&);

    //constructor
    APRVMode(void);

    //This operation handles the Timer Expiration.
    virtual void ExpirationRoutine(void);

    //This operation ends the Spont Inhalation Phase then Processes a new phase
    void EndSpontInhalation(void);

    //This operation ends the Spont Inhalation Phase then Processes a new phase in release windows
    void EndSpontInhalation_InReleaseWindow(void);

    //computes the time before opening Release Window form starting APRV PHigh Phase
    LONG CalculateReleaseWindowTime(void);

    //$COMMON.ATTRIBUTE$
    //  Name: S_Instance
    //  Description:  Pointer to the single instance of this class.
    //  Units: None
    //  Range: n/a
    static APRVMode* S_Instance;

    //$COMMON.ATTRIBUTE$
    //  Name: mTimeWindow
    //  Description:
    //  Units: None
    //  Range: n/a
    E_TimeWindowType mTimeWindow;

    //$COMMON.ATTRIBUTE$
    //  Name: IsInReleaseWindow
    //  Description:
    //  Units: None
    //  Range: n/a
    bool IsInReleaseWindow;

    //$COMMON.ATTRIBUTE$
    //  Name: IsPtInhTrigger
    //  Description:
    //  Units: None
    //  Range: n/a
    bool IsPtInhTrigger;

    //$COMMON.ATTRIBUTE$
    //  Name: IsPtInhTrigger
    //  Description:
    //  Units: None
    //  Range: n/a
    E_PressureWindowType mPressureWindow;
};

#endif // APRVMODE_H
