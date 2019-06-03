#include "ShutdownOpRequest.h"
#include "DebugTrace.h"

ShutdownOpRequest* ShutdownOpRequest::S_Instance = NULL;

ShutdownOpRequest::ShutdownOpRequest()
{
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: S_GetInstance()
//
//    Processing:
//      Since only 1 instance of this object should be created, any access
//      to the object comes via this operation.  If any other object wants
//      access to the OilPumpOpRequest object, it invokes
//      ShutdownOpRequest::S_GetInstance(), which returns a pointer to the
//      ShutdownOpRequest object.
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
//      ShutdownOpRequest* - pointer to the object instantiated from this class
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
ShutdownOpRequest* ShutdownOpRequest::S_GetInstance(void)
{
    // If the object has not already been created, create it.
    if (NULL == S_Instance)
    {
        S_Instance = new ShutdownOpRequest();
        ASSERTION(S_Instance != NULL);
    }

    return(S_Instance);

}   // end S_GetInstance()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessRequest()
//
//    Processing:
//      This method is invoked by MgrOpRequest::ReceiveRequest() when the
//      oil pump request is received. This operation invokes the Disable()
//      method of the base class Timer to stop the timer in case it is
//      already running. Then,it calls OilPump->SetStateDirect(eOn) to turn on the port.
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
//    Requirements:
//
/******************************************************************************/
void ShutdownOpRequest::ProcessRequest(E_OpRequestCommand command)
{
//    PhaseMgr::S_GetInstance()->ProcessNewPhase(eNonVentPhase);
//    TriggerMgr::S_GetInstance()->DisableAllTriggers();
//
//    HfoServo::S_GetInstance()->Disable();
//    HfoController->Disable();
//    CMVServoController->Disable();
//    VCVServoController->Disable();
}   // end ProcessRequest()
