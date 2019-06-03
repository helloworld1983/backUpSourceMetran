/******************************************************************************/
//$COMMON.CPP$
//   File Name:  TestHandler.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: 
//      This file contains the free subprograms which handle failed CBIT
//      conditions.
//
//   Interfaces:
//      Operations in this file interface with the CBIT Task and
//      the Error Handler.
//
//   Restrictions:
//      None
/******************************************************************************/
#include "TestHandler.h"
#include "CbitTask.h"

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: VentInopHandler()
//
//    Processing: 
//      This operation invokes the ErrorHandler HandleVentInop() operation.
//
//    Input Parameters:
//      eventCode - code associated with the fault condition
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
void VentInopHandler(E_EventCode eventCode)
{
#ifdef PRODUCTION_BUILD
    // Let the error handler deal with the VIO.
    (ErrorHandler::S_GetInstance())->HandleVentInop(eventCode);
#endif
}   // end VentInopHandler



/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: RecoverableHandler()
//
//    Processing: 
//      This operation invokes the ErrorHandler HandleRecoverable() operation.
//
//    Input Parameters:
//      eventCode - code associated with the fault condition
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
void RecoverableHandler(E_EventCode eventCode)
{
#ifdef PRODUCTION_BUILD
    // Let the error handler deal with the recoverable error.  
    (ErrorHandler::S_GetInstance())->HandleRecoverable();
#endif
}   // end RecoverableHandler()




/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: LogOnlyHandler()
//
//    Processing: 
//      This operation places the event on the CBIT queue so that 
//
//    Input Parameters:
//      eventCode - code associated with the fault condition
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
void LogOnlyHandler(E_EventCode eventCode)
{
    STATUS status;      // RTOS Return Status
    
    // Tell CBIT to log the event to NVRAM.  It is undesirable to have the
    // caller do it due to the response time of writing to NVRAM.
//    status = (CbitTask::S_GetInstance())->Send(eventCode);
//    ASSERTION(status == NU_SUCCESS);

}   // end LogOnlyHandler()



/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: AirSourceFaultHandler()
//
//    Processing: 
//      This operation is invoked when a problem is detected with the air
//      source.  It turns the blower off, notifies the O2Setting object to
//      switch to all O2 and logs the event.
//
//    Input Parameters:
//      eventCode - code associated with the fault condition
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
void AirSourceFaultHandler(E_EventCode eventCode)
{
    // Turn off the blower.
//    TheBlower->SwitchOff();
//    TheBlower->JumpToSpeed(0);
    
    // This call will cause the vent to switch to All O2 if it can.
    // It will also update the alarm status.
//    (O2Setting::S_GetInstance())->SetAirSourceFault(eventCode);

    // Log the event
//    ErrorHandler::S_GetInstance()->LogEvent(eventCode);

}   // end AirSourceFaultHandler()



/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: O2ValveClosedHandler()
//
//    Processing: 
//      This operation is invoked when an O2 Valve Stuck closed condition is
//      detected.  If the O2 Valve stuck open condition is detected, it is
//      handled as a VIO condition. 
//
//      This operation notifies the O2Setting that the oxygen source is no
//      longer available for use and the event code is placed on the CBIT Task
//      queue to be logged to NVRAM.
//
//    Input Parameters:
//      eventCode - code associated with the fault condition
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
void O2ValveClosedHandler(E_EventCode eventCode)
{
    // This call will cause the vent to switch to All O2 if it can.
    // It will also update the alarm status.
//    (O2Setting::S_GetInstance())->SetO2ValveFault();

    // Log the event
    ErrorHandler::S_GetInstance()->LogEvent(eventCode);

}   // end O2ValveFaultHandler()



/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: NullHandler()
//
//    Processing: 
//      This operation was created to conform to the CBIT Test structure when
//      nothing needs to be done in response to a test failure.  The constructor
//      for a Test expects a pointer to a function that takes as input 
//      E_EventCode and returns nothing. 
//
//    Input Parameters:
//      Not used
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
void NullHandler(E_EventCode)
{

}   // end NullHandler()
