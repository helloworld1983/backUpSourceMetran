/******************************************************************************/
//$COMMON.H$
//   File Name:  MgrBdSetting.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: 
//      The BD Settings Manager class manages the phasing in of all
//      Breath Settings.  It also maintains all setting values.
/******************************************************************************/
#ifndef MGRBDSETTING_H
#define MGRBDSETTING_H

#include "BdInterface.h"
#include "BdSetting.h"

#include "DebugTrace.h"

class MgrBdSetting
{
public:
    //Get current setting
    LONG GetCurrentValue (E_BdSettingId id);

    //Get pending setting
    inline LONG GetPendingValue (E_BdSettingId id);

    //Pointer to the object instantiated from this class
    static MgrBdSetting* S_GetInstance (void);

    //Receive request setting
    void ReceiveRequest (const BdEvent& bdEvent);

    //Add setting to change list
    void AddSettingToChangeList (BdSetting* bdSettingPtr);

    //Process new Phase for each setting
    void ProcessNewPhase (void);

    //Phase in the setting change and reset the pointer in the change list
    void PhaseInAllSettings(void);

    //Process phase in NCPAP setting
    void PhaseInNCPAPSettings(void);

    //Process phase in PIP setting
    void PhaseInPIPSetting(void);

    //Do not start automatically set Amp
    void DoNotStartAutoSetAmp(void);

#ifdef HFO_SYSTEM
    void PhaseInHFOSettings(void);
    void ProcessHFOSettings(void);
#endif

    //get status of trigger
    E_Bool GetPressTriggOn(void);
    E_Bool GetFlowTriggOn(void);

protected:
    //pointer to the object instantiated from this class
    static MgrBdSetting* S_Instance;

private:
    //Constructor
    MgrBdSetting (void);
    //Copy constructor
    MgrBdSetting (const MgrBdSetting&);

    //handle volume setting
    void HandleVolumeSettings (const VolumeSpecificSettings& volSettings);

    //Handle pressure setting
    void HandlePressureSettings (
            const PressureSpecificSettings& pressSettings);
    //Handle nippv setting
    void HandleNippvSettings (const NippvSpecificSettings& nippvSettings);

    //handle commond settings
    void HandleCommonSettings (const BdEvent& bdEvent);

    //process phaseInImmediately
    void HandlephaseInImmediatelyFlag(E_Bool phaseInImmediately,
                 BdSetting* bdSettingPtr, Trigger* TriggerPtr, LONG value);

    //$COMMON.ATTRIBUTE$
    //  Name: startAutoSetAmp
    //  Description:  Control start or not to set amp
    //  Units: true or false
    //  Range: n/a
    E_Bool startAutoSetAmp;

    //$COMMON.ATTRIBUTE$
    //  Name: ChangeListOverflow
    //  Description:  Indicates if the number of changed settings exceeds
    //      the number that can be held in the ChangedList
    //  Units: None
    //  Range: n/a
    E_Bool ChangeListOverflow;

    //$COMMON.ATTRIBUTE$
    //  Name: SettingList
    //  Description:  List of pointers to the BdSetting objects.
    //  Units: None
    //  Range: n/a
    BdSetting* SettingList[eNumBdSettings];

    //$COMMON.ATTRIBUTE$
    //  Name: ChangeList
    //  Description:  List of pointers to the BdSetting objects that have
    //      changed since the last breath phase.
    //  Units: None
    //  Range: n/a
    BdSetting* ChangedList[eNumBdSettings];

    //$COMMON.ATTRIBUTE$
    //  Name: ChangeListIx
    //  Description:  Index into the ChangeList.
    //  Units: None
    //  Range: n/a
    SHORT ChangeListIx;

    //$COMMON.ATTRIBUTE$
    //  Name: PressTriggOn
    //  Description:  Check pressure trigger is called or not.
    //  Units: None
    //  Range: n/a
    E_Bool PressTriggOn;

    //$COMMON.ATTRIBUTE$
    //  Name: FlowTrigOn
    //  Description:  Check flow trigger is called or not.
    //  Units: None
    //  Range: n/a
    E_Bool FlowTrigOn;

//    E_Bool PSVOn;

};

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GetPendingValue()
//
//    Processing: 
//      This operation invokes the GetPendingValue() operation of the setting
//      whose id is passed to this operation.  
//
//    Input Parameters:
//      id - Setting Id of the pending setting value to be retrieved
//
//    Output Parameters:
//      None
//
//    Return Values:
//      LONG - setting value
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
inline LONG MgrBdSetting::GetPendingValue (E_BdSettingId id)
{
    ASSERTION(id < eNumBdSettings);
    DEBUG_MGR_BDSETTING("GetPendingValue %d \n", id);
    return (SettingList[id]->GetPendingValue ());

}   // end GetPendingValue()

#endif  // MGRBDSETING_H
