/******************************************************************************/
//$COMMON.CPP$
//   File Name:  OpRequest.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: 
//      This file contains the OpRequest class definition.  The OpRequest
//      class is defined as an abstract base class.  It is only defined so
//      that operator requests (e.g., manual inhalation, 100% O2, alarm reset)
//      can be handled similarly by the MgrOpRequest object.  An empty default 
//      constructor is defined (instead of relying on the one provided by
//      the compiler) simply to resolve a compiler error that results because
//      the copy constructor is declared.
//
//      The operations AcceptRequest(), CompleteRequest() and RejectRequest()
//      interface with objects in the DataIO subsystem to turn on/off LED's
//      and to sound good/bad audible beeps to provide feedback to the operator
//      regarding the status of the operator request.
//
//   Interfaces:
//      None
//
//   Restrictions:
//      OpRequest is an abstract base class; there are no instances of this
//      class.
/******************************************************************************/
#include "OpRequest.h"
//#include "Audible.h"

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: OpRequest()
//
//    Processing: 
//      This operation is the constructor for objects of the OpRequest class.
//      It performs no work.
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
//OpRequest::OpRequest(Led* ledPtr)
//    : LedPtr(ledPtr)
//{
//
//}   // end OpRequest()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: AcceptRequest()
//
//    Processing: 
//      If the LedPtr associated with this request is not NULL, this operation
//      calls the SetState() operation of the LED to turn on the visual 
//      indicator.  It also calls Audible::SetFeedback() to provide
//      audible indication that the request was accepted.
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
void OpRequest::AcceptRequest(E_State state)
{

    // If there is an LED associated with this operator request, update its
    // state.
//    if (NULL != LedPtr)
//    {
//        LedPtr->SetState(state);
//    }

    // Provide audible feedback that the request was accepted.
//    (Audible::S_GetInstance())->SetFeedback(eGoodBeep);

}   // AcceptRequest()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CompleteRequest()
//
//    Processing: 
//      If the LedPtr associated with this request is not NULL, this operation
//      calls the SetState() operation of the LED to turn off the visual 
//      indicator since the requested feature is no longer active.  
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
void OpRequest::CompleteRequest (void)
{
    // If there is an LED associated with this request, extinguish it.
//    if (NULL != LedPtr)
//    {
//        LedPtr->SetState(eOff);
//    }

}   // end CompleteRequest()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: RejectRequest()
//
//    Processing: 
//      This operation calls Audible::SetFeedback() to provide
//      audible indication that the request was rejected.
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
void OpRequest::RejectRequest (void)
{

//    (Audible::S_GetInstance())->SetFeedback(eBadBeep);

}   // RejectRequest()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: OpRequest()
//
//    Processing:   Constructor for OpRequest class
//
//    Input Parameters:
//      E_VentilationMode fromModeId
//
//    Output Parameters:
//      None
//
//    Return Values:
//      None
//
//    Pre-Conditions:
//
//
//    Requirements:
//
/******************************************************************************/
OpRequest::OpRequest (void)
{
}
