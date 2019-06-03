/******************************************************************************/
//$COMMON.H$
//   File Name:  NCPAP_Phase.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
/******************************************************************************/
#include "NCPAP_Phase.h"
#include "MgrBdSetting.h"
#include "MgrBreathData.h"
#include "TriggerMgr.h"
#include "NCPAPController.h"

#define ONE_SECOND 1000
NCPAP_Phase* NCPAP_Phase::S_Instance = NULL;

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: S_GetInstance()
//
//    Processing:
//      Since only 1 instance of this object should be created, any access
//      to the object comes via this operation.  If any other object wants
//      access to the object, it invokes S_GetInstance(),
//      which returns a pointer to the object.
//
//      If the object has not already been created, this method instantiates
//      it and returns a pointer to the object.  If the object has already been
//      instantiated, a pointer to the object is returned.
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      * - pointer to the object instantiated from this class
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
NCPAP_Phase* NCPAP_Phase::S_GetInstance(void)
{
    // If the object has not already been created, create it.
    if (NULL == S_Instance)
    {
        S_Instance = new NCPAP_Phase();
        ASSERTION(S_Instance != NULL);
    }
    return(S_Instance);

}   // end S_GetInstance()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: NCPAP_Phase()
//
//    Processing:
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      None
//
//    Pre-Conditions:
//      The input command must be Start.
//
//    Requirements:
//
/******************************************************************************/
NCPAP_Phase::NCPAP_Phase(): Phase()
{
    timer_1s = 0;
}
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Start()
//
//    Processing:
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      None
//
//    Pre-Conditions:
//      The input command must be Start.
//
//    Requirements:
//
/******************************************************************************/
void NCPAP_Phase::Start()
{
    MgrBdSetting::S_GetInstance()->PhaseInNCPAPSettings();
    LONG FlowSetting = MgrBdSetting::S_GetInstance()->GetCurrentValue(eNasalCPAPFlow);
    NCPAPController::S_GetInstance()->SetTargetFlow(FlowSetting);

    TriggerMgr* triggerMgrPtr = TriggerMgr::S_GetInstance();

    triggerMgrPtr->EnableTrigger(eExtremeHighPEEPTrigger);
    timer_1s = 0;
}
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: End()
//
//    Processing:
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      None
//
//    Pre-Conditions:
//      The input command must be Start.
//
//    Requirements:
//
/******************************************************************************/
void NCPAP_Phase::End()
{
    timer_1s = 0;

    TriggerMgr* triggerMgrPtr = TriggerMgr::S_GetInstance();
    triggerMgrPtr->DisableTrigger(eExtremeHighPEEPTrigger);
}
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessBdCycle()
//
//    Processing:
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      None
//
//    Pre-Conditions:
//      The input command must be Start.
//
//    Requirements:
//
/******************************************************************************/
void NCPAP_Phase::ProcessBdCycle()
{
    MgrBdSetting::S_GetInstance()->PhaseInNCPAPSettings();
    LONG FlowSetting = MgrBdSetting::S_GetInstance()->GetCurrentValue(eNasalCPAPFlow);
    NCPAPController::S_GetInstance()->SetTargetFlow(FlowSetting);

    if(0 == timer_1s)
    {
        MgrBreathData::S_GetInstance()->ProcessNCPAP();
    }

    timer_1s = timer_1s + 2;
    //2ms
    if(timer_1s >= ONE_SECOND)
    {
        timer_1s = 0;
    }
}
