/******************************************************************************/
//$COMMON.CPP$
//   File Name:  SelfTestPhase.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
/******************************************************************************/
#include "SelfTestPhase.h"
#include "SelfTestMode.h"
#include "MgrBreathData.h"
#include "MgrFlowController.h"

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: SelfTestPhase()
//
//    Processing: 
//      This operation is the constructor for the SelfTestPhase. It invokes the
//      base class Phase constructor.
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
//      None
//
//    Miscellaneous:
//      None
//
//    Requirements:  
//
/******************************************************************************/
SelfTestPhase::SelfTestPhase(void) : Phase() 
{
    
}   // end SelfTestPhase()


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
//      None
//
//    Miscellaneous:
//      None
//
//    Requirements:  
//
/******************************************************************************/
void SelfTestPhase::Start(void)
{

    // Update Patient Data
    (MgrBreathData::S_GetInstance())->NonVentStart();

    // Enable the air and O2 flow controllers
    MgrFlowController::O2FlowController->Reset();
    MgrFlowController::AirFlowController->Reset();
    MgrFlowController::AirFlowController->SetDesired(0);
    MgrFlowController::O2FlowController->SetDesired(0);
    MgrFlowController::AirFlowController->Enable();
    MgrFlowController::O2FlowController->Enable();

}   // end Start()



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
//      None
//
//    Miscellaneous:
//      None
//
//    Requirements:  
//
/******************************************************************************/
void SelfTestPhase::End(void)
{
}   // end End()


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
//    Miscellaneous:
//      None
//
//    Requirements:
//
/******************************************************************************/
void SelfTestPhase::ProcessBdCycle(void)
{

}   // end ProcessBdCycle()
