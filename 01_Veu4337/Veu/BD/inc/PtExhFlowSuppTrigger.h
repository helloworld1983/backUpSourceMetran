/******************************************************************************/
//$COMMON.H$
//   File Name:  PtExhFlowSuppTrigger.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: 
//      This file contains the class declaration for the Patient Exhalation
//      Flow Trigger, which is derived from the Trigger class.
/******************************************************************************/
#ifndef PTEXHFLOWSUPPTRIGGER_H
#define PTEXHFLOWSUPPTRIGGER_H
#include "Trigger.h"

class PtExhFlowSuppTrigger : public Trigger
{
public:
    //Constructor
    PtExhFlowSuppTrigger(void);

    //Check condition and process action
    virtual E_Bool CheckCondition(void);

    //Enable trigger
    virtual void EnableTrigger(void);

    //Process action base on trigger ID
    virtual inline void ProcessTrigger(Mode* mode);

    //updates the FlowLimit and bias flow
    virtual void PhaseInSettingChange(LONG newValue);

protected:


private:
    PtExhFlowSuppTrigger(const PtExhFlowSuppTrigger&);

    //$COMMON.ATTRIBUTE$
    //  Name: ExhSens
    //  Description:  Exhalation sensitivity; percentage of peak flow
    //      that the instantaneous flow must drop to cause the trigger
    //      to fire.
    //  Units: %
    //  Range: 10 - 45%
    LONG BiasFlow;

    //$COMMON.ATTRIBUTE$
    //  Name: IsConditionMet
    //  Description:  Boolean that indicates if the trigger criteria were
    //      met the previous time it was checked
    //  Units: None
    //  Range: eTrue, eFalse
    E_Bool IsConditionMet;

    //$COMMON.ATTRIBUTE$
    //  Name: TimeConditionFirstMet
    //  Description:  System Time that the condition was met; used to
    //      insure the criteria are met for a minimum amount of time in
    //      the initial part of inhalation.
    //  Units: TICKS
    //  Range: 0 - maximum UNSIGNED value
    UNSIGNED TimeConditionFirstMet;


    //$COMMON.ATTRIBUTE$
    //  Name: PrevFilteredDelFlow
    //  Description:  Filtered delivered flow for the previous BD cycle;
    //      used to filter the delivered flow for the current cycle.
    //  Units: TICKS
    //  Range:  0 - 3333.33 ml/sec (200 LPM), scaled
    LONG PrevFilteredDelFlow;

    //$COMMON.ATTRIBUTE$
    //  Name: PrevFilteredProxFlow
    //  Description:  Filtered delivered flow for the previous BD cycle;
    //      used to filter the delivered flow for the current cycle.
    //  Units: TICKS
    //  Range:  0 - 3333.33 ml/sec (200 LPM), scaled
    LONG PrevFilteredProxFlow;

};      


/******************************************************************************/
//      INLINE OPERATIONS
/******************************************************************************/

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessTrigger()
//
//    Processing: 
//      This operation invokes the ProcessPtExhTrigger() operation of the active
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
inline void PtExhFlowSuppTrigger::ProcessTrigger(Mode* mode)
{
    mode->ProcessTriggerID(ePtExhFlowSuppTrigger);

}   // end PtExhFlowSuppTrigger()


#endif // PTEXHFLOWSUPPTRIGGER_H
