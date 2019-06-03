#ifndef APRV_PTINHPRESSTRIGGER_INPLOW_H
#define APRV_PTINHPRESSTRIGGER_INPLOW_H

#include "Trigger.h"
#include "PhaseMgr.h"
#include "APRV_PLowPhase.h"

class APRV_PtInhPressTrigger_InPLow : public Trigger
{
public:
    APRV_PtInhPressTrigger_InPLow(void);
    virtual E_Bool CheckCondition(void);
    virtual void EnableTrigger(void);
    virtual void EnableTrigger(UNSIGNED pressure);
    virtual void PhaseInSettingChange(LONG newValue);
    virtual inline void ProcessTrigger(Mode* mode);
    virtual E_Bool PreCheckCondition(void);

protected:


private:
    APRV_PtInhPressTrigger_InPLow(const APRV_PtInhPressTrigger_InPLow&);

    E_Bool APRV_PHigh_CheckCondition(void);
    E_Bool APRV_PLow_CheckCondition(void);

    //$COMMON.ATTRIBUTE$
    //  Name: IsPressureFixed
    //  Description:  Indicates if the PressureLimit is fixed for the
    //      currently active exhalation phase.
    //  Units: None
    //  Range: eTrue, eFalse
    E_Bool IsPressureFixed;

    //$COMMON.ATTRIBUTE$
    //  Name: InUnrestrictedPhase
    //  Description:  Indicates if the unrestricted phase of exhalation
    //      is active.
    //  Units: None
    //  Range: eTrue, eFalse
    E_Bool InUnrestrictedPhase;

    //$COMMON.ATTRIBUTE$
    //  Name: PressureLimit
    //  Description:  Baseline Pressure - Pressure Sensitivity
    //  Units: cmH2O * SCALE
    //  Range: -20 to 34.9 cmH2O
    SHORT PressureLimit;

    //$COMMON.ATTRIBUTE$
    //  Name: PressureLimit
    //  Description:  Baseline Pressure - Pressure Sensitivity
    //  Units: cmH2O * SCALE
    //  Range: 0.5 to 10.5 cmH2O
    SHORT PressSens;

    //$COMMON.ATTRIBUTE$
    //  Name: PrevFilteredPress
    //  Description:  Filtered pressure for the previous BD cycle;
    //      used to filter the monitor press for the current cycle.
    //  Units: cmH2O * SCALE
    //  Range:  0 - 12000
    LONG PrevFilteredPress_PHigh;
    LONG PrevFilteredPress_PLow;

};

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessTrigger()
//
//    Processing:
//      This operation invokes the ProcessPtInhTrigger() operation of the active
//      mode.
//
//    Input Parameters:
//      mode - pointer to the currently active Mode object
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
inline void APRV_PtInhPressTrigger_InPLow::ProcessTrigger(Mode* mode)
{
    // Invoke the operation to process the patient pressure trigger in the
    // currently active mode.
    mode->ProcessTriggerID(eAPRV_PtInhPressTrigger_InPLow);

}   // end APRV_PtInhPressTrigger_InPLow()


#endif // APRV_PTINHPRESSTRIGGER_INPLOW_H
