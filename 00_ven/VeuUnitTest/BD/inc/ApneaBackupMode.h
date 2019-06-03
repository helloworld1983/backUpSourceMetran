/******************************************************************************/
//$COMMON.H$
//   File Name:  ApneaBackupMode.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class:
//      This file contains the class declaration for the ApneaBackupMode Mode
//      which is derived from the MixedMode base class.
/******************************************************************************/
#ifndef APNEABACKUPMODE_H
#define APNEABACKUPMODE_H

#include "MixedMode.h"

class ApneaBackupMode : public MixedMode
{
public:
    //Pointer to the object instantiated from this class
    static ApneaBackupMode* S_GetInstance(void);

    //Process doing something when another mode enters this mode and exits
    void Enter(E_VentilationMode fromModeId);
    void Exit(void);

    //Process action for each ID trigger
    virtual void ProcessTriggerID(E_TriggerId id);

    //Process PtInh Trigger
    virtual void ProcessPtInhTrigger(void);

    //Process Alarm Reset Trigger
    virtual void ProcessAlarmResetTrigger(void);

    //Process manual In Request trigger
    virtual void ProcessManualInhOpReqTrigger(void);

    //Process Mandatory Press Inhalation Time Trigger
    virtual void ProcessMandPressInhTimeTrigger(void);

    //Process Vcv Inhalation Time Trigger
    virtual void ProcessVcvInhTimeTrigger(void);

    //Process Plateau Time Trigger
    virtual void ProcessPlateauTimeTrigger(void);

    //Process Bp Time Trigger
    virtual void ProcessBpTimeTrigger(void);

    //Process Apneal Alarm Time Trigger
    virtual void ProcessApneaAlarmTimeTrigger(void);

    //COnstructor
    ApneaBackupMode(void);

    //Copy constructor
    ApneaBackupMode(const ApneaBackupMode&);

    //$COMMON.ATTRIBUTE$
    //  Name: S_Instance
    //  Description:  Pointer to the single instance of this class.
    //  Units: None
    //  Range: n/a
    static ApneaBackupMode* S_Instance;

    //$COMMON.ATTRIBUTE$
    //  Name: fromMode
    //  Description:  change from fromMode
    //  Units: None
    //  Range: n/a
    E_VentilationMode fromMode;

    //$COMMON.ATTRIBUTE$
    //  Name: IsPtInhTrigger
    //  Description:  Is Pt Inh Trigger or not.
    //  Units: None
    //  Range: n/a
    bool IsPtInhTrigger;
};

#endif // APNEABACKUPMODE_H

