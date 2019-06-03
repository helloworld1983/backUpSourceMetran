//******************************************************************************//
//$COMMON.CPP$
//   File Name:  OilPumpOpRequest.cpp
//   Copyright
//
//   Purpose: This file contains the methods of the OilPumpOpRequest class.
//
//   Interfaces:
//      Derived Driver classes - interfaces to I/O hardware
//      Location data structure - HW mapping of output signal
//
//   Restrictions: None
//
//   Revision History:

//
//******************************************************************************

#include "OilPumpOpRequest.h"
#include "DebugTrace.h"

// Define statics
OilPumpOpRequest* OilPumpOpRequest::S_Instance = NULL;

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: S_GetInstance()
//
//    Processing:
//      Since only 1 instance of this object should be created, any access
//      to the object comes via this operation.  If any other object wants
//      access to the OilPumpOpRequest object, it invokes
//      OilPumpOpRequest::S_GetInstance(), which returns a pointer to the
//      OilPumpOpRequest object.
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
//      OilPumpOpRequest* - pointer to the object instantiated from this class
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
OilPumpOpRequest* OilPumpOpRequest::S_GetInstance(void)
{
    // If the object has not already been created, create it.
    if (NULL == S_Instance)
    {
        S_Instance = new OilPumpOpRequest();
        ASSERTION(S_Instance != NULL);
    }

    return(S_Instance);

}   // end S_GetInstance()

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: OilPumpOpRequest()
//
//    Processing: This constructor creates and initializes
//          OilPumpOpRequest objects
//
//    Input Parameters:

//
//    Output Parameters: None
//
//    Return Values: ptr to created object.
//
//    Pre-Conditions:
//          The Driver objects to which the outputs are assigned must exist
//          prior to the creation of the DiscreteOutput objects.
//
//    Miscellaneous: None
//    Requirements:
//
//******************************************************************************
OilPumpOpRequest::OilPumpOpRequest() : Timer((CHAR*)"OilPumpOpRequest", 10000, pdFALSE)
{

}

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
void OilPumpOpRequest::ProcessRequest(E_OpRequestCommand command)
{
    //Disable 10 second timer
//    Disable();

    //Turn on OilPump

//    OilPump->SetDesiredState(eOn);

    //Enable 10 second timer
//    Enable();

}   // end ProcessRequest()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ExpirationRoutine
//
//    Processing:
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
void OilPumpOpRequest::ExpirationRoutine(void)
{
    //Disable 10 second timer
//    Disable();

    //Turn off OilPump
//    OilPump->SetDesiredState(eOff);

}
