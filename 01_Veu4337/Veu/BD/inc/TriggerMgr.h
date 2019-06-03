/******************************************************************************/
//$COMMON.H$
//   File Name:  TriggerMgr.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: 
//      The Trigger Manager defines a common interface to the triggers.
/******************************************************************************/
#ifndef TRIGGERMGR_H
#define TRIGGERMGR_H

#include <Trigger.h>

// INIT_UPDATE_STAT_TIME defines the initial time period used for the 
// Update Status Time Trigger.
static const UNSIGNED INIT_UPDATE_STAT_TIME = 350 / MS_PER_TICK; // 350ms in ticks

class TriggerMgr
{
public:
    //Pointer to the object instantiated from this class
    static TriggerMgr* S_GetInstance (void);

    //Enable Trigger ID
    inline void EnableTrigger (E_TriggerId id);

    //Enable Trigger ID with Value
    inline void EnableTrigger (E_TriggerId id, UNSIGNED value);

    //Disable Trigger ID
    inline void DisableTrigger (E_TriggerId id);

    //ReEnable Trigger ID
    inline void ReEnableTrigger (E_TriggerId id);

    //Set trigger fire
    inline void SetTriggerDidFire (E_TriggerId id, E_Bool didFire);

    //Check condition of trigger ID
    inline E_Bool CheckCondition (E_TriggerId id);

    //indicates if the trigger has fired or not
    inline E_Bool PreCheckCondition (E_TriggerId id);

    //return pointer to trigger
    inline Trigger* GetTriggerPtr (E_TriggerId id);

    //Get time period of trigger
    inline UNSIGNED GetTimePeriod (E_TriggerId id);

    // Goes through its list of triggers.
    // For each trigger that is enabled, this operation invokes the
    // CheckCondition() operation of the trigger.
    void ProcessBdCycle (void);
    void ProcessIMVBdCycle(void);
    void ProcessHFOBdCycle(void);

    //Disable all trigger
    void DisableAllTriggers (void);

    //Enable all HFO trigger
    void EnableHFOTrigger(void);

    //Enable IMVT trigger
    void EnableIMVTrigger(void);

    //Enable exhalation Trigger
    void EnableExhaleTrigger(void);

    //Enable Inhalation Trigger
    void EnableInhaleTrigger(void);

protected:

private:
    TriggerMgr (void);
    TriggerMgr (const TriggerMgr&);

    //$COMMON.ATTRIBUTE$
    //  Name: S_Instance
    //  Description:  Pointer to the single instance of this class.
    //  Units: None
    //  Range: n/a
    static TriggerMgr* S_Instance;

    //$COMMON.ATTRIBUTE$
    //  Name: TriggerList
    //  Description:   Array of Trigger Pointers.
    //  Units: None
    //  Range: n/a
    Trigger* TriggerList[eNumTriggers];

};

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: EnableTrigger()
//
//    Processing: 
//      This operation invokes the EnableTrigger() operation of the trigger
//      whose id is passed to this operation.  
//
//    Input Parameters:
//      id - Trigger Id of the trigger to be enabled
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
inline void TriggerMgr::EnableTrigger (E_TriggerId id)
{

    TriggerList[id]->EnableTrigger ();

}   // end EnableTrigger()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: EnableTrigger()
//
//    Processing: 
//      This operation invokes the EnableTrigger() operation of the trigger
//      whose id is passed to this operation.  
//
//    Input Parameters:
//      id - Trigger Id of the trigger to be enabled
//      value - value used to enable the trigger
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
inline void TriggerMgr::EnableTrigger (E_TriggerId id, UNSIGNED value)
{
    TriggerList[id]->EnableTrigger (value);

}   // end EnableTrigger()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: DisableTrigger()
//
//    Processing: 
//      This operation invokes the DisableTrigger() operation of the trigger
//      whose id is passed to this operation.  
//
//    Input Parameters:
//      id - Trigger Id of the trigger to be disabled
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
inline void TriggerMgr::DisableTrigger (E_TriggerId id)
{
    TriggerList[id]->DisableTrigger ();

}   // end DisableTrigger()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ReEnableTrigger()
//
//    Processing: 
//      This operation invokes the ReEnableTrigger() operation of the trigger
//      whose id is passed to this operation.  
//
//    Input Parameters:
//      id - Trigger Id of the trigger to be re-enabled
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
inline void TriggerMgr::ReEnableTrigger (E_TriggerId id)
{
    TriggerList[id]->ReEnableTrigger ();

}   // end ReEnableTrigger()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: SetTriggerDidFire()
//
//    Processing: 
//      This operation invokes the SetTriggerDidFire() operation of the trigger
//      whose id is passed to this operation.  
//
//    Input Parameters:
//      id - Trigger Id of the trigger 
//      didFire - value used to set the TriggerDidFire attribute
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
inline void TriggerMgr::SetTriggerDidFire (E_TriggerId id, E_Bool didFire)
{
    TriggerList[id]->SetTriggerDidFire (didFire);

}   // end SetTriggerDidFire()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CheckCondition()
//
//    Processing: 
//      This operation invokes the CheckCondition() operation of the trigger
//      whose id is passed to this operation.  
//
//    Input Parameters:
//      id - Trigger Id of the trigger whose condition is being checked
//
//    Output Parameters:
//      None
//
//    Return Values:
//      E_Bool - indicates if the trigger has fired or not
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
inline E_Bool TriggerMgr::CheckCondition (E_TriggerId id)
{
    return (TriggerList[id]->CheckCondition ());

}   // end CheckCondition()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: PreCheckCondition()
//
//    Processing: 
//      This operation invokes the PreCheckCondition() operation of the trigger
//      whose id is passed to this operation.  
//
//    Input Parameters:
//      id - Trigger Id of the trigger whose condition is being checked
//
//    Output Parameters:
//      None
//
//    Return Values:
//      E_Bool - indicates if the trigger has fired or not
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
inline E_Bool TriggerMgr::PreCheckCondition (E_TriggerId id)
{
    return (TriggerList[id]->PreCheckCondition ());

}   // end PreCheckCondition()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GetTriggerPtr()
//
//    Processing: 
//      This operation returns a pointer to the trigger whose id is passed 
//      to this operation.  
//
//    Input Parameters:
//      id - Trigger Id of the trigger
//
//    Output Parameters:
//      None
//
//    Return Values:
//      Trigger* - pointer to the trigger object
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
inline Trigger* TriggerMgr::GetTriggerPtr (E_TriggerId id)
{
    return (TriggerList[id]);

}   // end GetTriggerPtr()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GetTimePeriod()
//
//    Processing: 
//      This operation invokes the GetTimePeriod() operation of the trigger
//      whose id is passed to this operation.  
//
//    Input Parameters:
//      id - Trigger Id of the trigger whose value is being retrieved.  
//
//    Output Parameters:
//      None
//
//    Return Values:
//      E_Bool - indicates if the trigger has fired or not
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
inline UNSIGNED TriggerMgr::GetTimePeriod (E_TriggerId id)
{
    return (TriggerList[id]->GetTimePeriod ());

}   // end GetTimePeriod()

#endif  // TRIGGERMGR_H
