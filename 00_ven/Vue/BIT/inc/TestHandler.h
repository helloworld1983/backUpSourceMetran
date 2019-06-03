//******************************************************************************/
//$COMMON.H$
//   File Name:  TestHandler.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: None.  This file contains the external declarations for the handler
//      operations for the CBIT Tests.
//
//   Revision History:
//      Rev:  Date:     Engineer:           Project:
//      01    09/30/97  K.A. McGuigan       Morph
//      Description: Initial Revision
//
/******************************************************************************/
#ifndef TESTHANDLER_H
#define TESTHANDLER_H

#include <ErrorHan.h>

//This operation invokes the ErrorHandler HandleVentInop() operation
extern void VentInopHandler(E_EventCode eventCode);

//This operation invokes the ErrorHandler HandleRecoverable() operation
extern void RecoverableHandler(E_EventCode eventCode);

//This operation places the event on the CBIT queue so that
extern void LogOnlyHandler(E_EventCode eventCode);

//This operation is invoked when a problem is detected with the air
//source.  It turns the blower off, notifies the O2Setting object to
//switch to all O2 and logs the event.
extern void AirSourceFaultHandler(E_EventCode eventCode);

//This operation is invoked when an O2 Valve Stuck closed condition is
//detected.  If the O2 Valve stuck open condition is detected, it is
//handled as a VIO condition.
//
//This operation notifies the O2Setting that the oxygen source is no
//longer available for use and the event code is placed on the CBIT Task
//queue to be logged to NVRAM.
extern void O2ValveClosedHandler(E_EventCode eventCode);

//This operation was created to conform to the CBIT Test structure when
//nothing needs to be done in response to a test failure.  The constructor
//for a Test expects a pointer to a function that takes as input
//E_EventCode and returns nothing.
extern void NullHandler(E_EventCode);

#endif // TESTHANDLER_H
