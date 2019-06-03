//******************************************************************************//
//$COMMON.CPP$
//   File Name: Errorhan.cpp  
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: This file contains the source code for the member functions
//      of the ErrorHandler class.
//
//   Interfaces:
//      EventLog
//      Many system Discrete Ouputs
//      System Clock Interfaces
//
//   Restrictions:
//      One one ErrorHandler object exists in the system.
//******************************************************************************
#define ERROR_HANDLER_CPP

#include "errorhan.h"
#include "util.h"

#include "DebugTrace.h"

//
// Single ErrorHandler object
//
ErrorHandler*  ErrorHandler::S_Instance;

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: ErrorHandler()
//
//    Processing:   This is the private constructor.
//          This routine is only called from the S_GetInstance() method.
//
//    Input Parameters:     None
//    Output Parameters:    None
//    Return Values:        None
//    Pre-Conditions:       None
//    Miscellaneous:        None
//    Requirements:  
//
//******************************************************************************
ErrorHandler::ErrorHandler()
{
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: S_GetInstance()
//
//    Processing: Handles static object.
//          If not yet defined, calls private constructor
//          Else returns pointer to existing object
//
//    Input Parameters:  None
//    Output Parameters: None
//    Return Values:     Ptr to static ErrorHandler object.
//    Pre-Conditions:    None
//    Miscellaneous:     None
//    Requirements:  
//
//******************************************************************************
ErrorHandler* ErrorHandler::S_GetInstance()
{
    if (S_Instance == NULL)    
    {
        S_Instance = new ErrorHandler();
        ASSERTION( S_Instance != NULL );
    }
    return(S_Instance);
}


//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: HandleVentInop()
//
//    Processing: 
//    Places the system in a safe state then disables the hardware,
//    configures the LEDs/Alarms and displays the Vent Inop message on the
//    LCD Display.
//
//    Input Parameters:
//        id   - Vent Inop number to be logged and displayed on LCD
//        info - Additional information to be recorded in NVRAM.
//
//    Output Parameters: None.
//
//    Return Values: None.
//
//    Pre-Conditions:None
//
//    Miscellaneous:
//    This routine writes directly to the system hardware rather than using
//    the standard objects.  This ensures that the hardware can be properly
//    configured even if the objects have not yet been created.
//
//    Requirements:  
//
//******************************************************************************
void ErrorHandler::HandleVentInop(E_EventCode id, char * info)
{
    // If Diagnostic Mode, log and continue unless the flow sensors are either
    // in the wrong spot (e.g. air where O2 sensor should be) or the flow tables
    // could not be programmed to match the sensor 
    if (InDiagnosticMode() && (id != eWrongSensorModelNumInO2) &&
        (id != eWrongSensorModelNumInAirOrExh) && (id != eFlowSensorMismatch) &&
        (id != eFlashProgrammingError) && (id != eFlowSensCalDataOutOfRange))
    {
        DiagnosticVentInop( id );
        return;
    }

    // Block Out all other tasks by disabling the timer 
    // Also clear any pending interrupts and Diasable the CSI interrupt.
//    *TIMER4CONTROL = STOP_TIMER4;
//    *CMIC4 = TIMER_4_DISABLE;

    // Now we need to start Timer 1 so that we will continue to strobe
    // the watchdog while we put the ventilator in a safe state and log
    // the error.  See SCR-148
//    TimeTick::S_GetInstance()->InitTimer();
//    TimeTick::S_GetInstance()->StartTimer();
    
    // Place Pneumatic hardware in safe state
//    CreateSafeState();

    // Configure the AlarmHandler object to handle lighting and flashing the
    // correct LEDs for a vent inop condition
//    AlarmHandler::S_GetInstance()->S_SetAlarmStatus(eOTPVentInop);

//    *(PIU_MAIN + 0x04) = 0; // Turn Off Blower Discrete

    // Log Event
//    LogEvent(id, info, NU_NO_SUSPEND);

    // If Early in the Boot Process, go to Text Mode Vent Inop    
//    if (Multitasking == eFalse)
//    {
//        TextModeVentInop(id);
//    }
//    else    // Else use Graphics Mode vent Inop
//    {
        // Display VentInop message on Display Screen
//        VentInopDialog * message = new VentInopDialog(id);
//        message->Display();
//    }

    // signal NMI PowerFail routine that 24 Volts is purposefully
    // being shut down
//    Discrete24Volt = eOn;

    // Now that we are in a safe state, lets shut down the hardware 
//    *PORT_1 = 0x00;      // Turn Off 24 Volts

    // Wait here forever.  Timer 1 is still active and will strobe the Watchdog
    while (1)
    {
        // While we are waiting, the AlarmHandler class flashes the 
        // High Alarm LED
        ;
    }
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: HandleRecoverable()
//
//    Processing: 
//    Places the system in a safe state and logs information about the failure
//    that has occured.  The operation then disables interrupts and waits for
//    the watchdog timer to reset the system.
//
//    Input Parameters:
//        info - Text string to be recorded in NVRAM.
//
//    Output Parameters: None.
//
//    Return Values: None.
//
//    Pre-Conditions: None
//
//    Miscellaneous:
//    For development work where the watchdog is disabled by jumper JP-2,
//    a 'jmp r0' is used to emulate a system reset.
//
//    Requirements:  
//
//******************************************************************************
void ErrorHandler::HandleRecoverable(char * info)
{
    // If Diagnostic Mode, ignore the error and continue. See SCR-154.
    if (InDiagnosticMode())
    {
        return;
    }

    // Block Out all other tasks by stopping the Timer Interrupt
//    *TIMER4CONTROL = STOP_TIMER4;
//    *CMIC4 = TIMER_4_DISABLE;

    // Now we need to start Timer 1 so that we will continue to strobe
    // the watchdog while we put the ventilator in a safe state and log
    // the error.  See SCR-148
//    TimeTick::S_GetInstance()->InitTimer();
//    TimeTick::S_GetInstance()->StartTimer();
    
    // Place Pneumatic hardware in safe state
//    CreateSafeState();

    // Log Event
//    LogEvent(eRecoverableAnomaly, info, NU_NO_SUSPEND);

    // Clear screen
//    OtpVGA::S_GetInstance()->DisableVideo();
      
    // Disable Interrupts  - This will stop Timer 1 from strobing the watchdog
//    asm( "di" );

#ifdef PRODUCTION_BUILD
    // If we are in a production environment, we will use the watchdog
    // timer to generate the reset.
    while (1)
    {
        ;  // spin here until reset comes
    }
#else
    // In the development environment, the watchdog is disabled, so we 
    // will cause the same effect by jumping to the reset vector.
//    asm("jmp r0");
#endif

}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: LogEvent()
//
//    Processing: 
//    Logs information in the NVRAM regarding the failure that has occured.
//
//    Input Parameters:
//        id  - The error code to be recorded.
//        info - Additional information to be recorded in NVRAM.
//
//    Output Parameters: None.
//
//    Return Values: None.
//
//    Pre-Conditions: None.
//
//    Miscellaneous:
//    Checks to make sure the System Event Log object has been created
//    before attempting to write to NVRAM.
//
//    Requirements:  
//
//******************************************************************************
void ErrorHandler::LogEvent(E_EventCode id, char * info, UNSIGNED suspend)
{
    // Log event in the SystemEventLog
//    if (SystemEventLog != NULL)
//        SystemEventLog->PutLogEntry(id, info, suspend);
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: TextModeVentInop()
//
//    Processing: 
//    Displays a vent inop error code on the LCD panel in text mode.
//
//    Input Parameters:
//        id - the error number to be displayed.
//
//    Output Parameters: None.
//
//    Return Values: None.
//
//    Pre-Conditions: None.
//
//    Miscellaneous: None.
//
//    Requirements:  
//
//******************************************************************************
void ErrorHandler::TextModeVentInop(E_EventCode id)
{
//    OtpVGA::S_GetInstance()->InitTextMode();
//    OtpVGA::S_GetInstance()->PutString("VENT INOP ");
//    OtpVGA::S_GetInstance()->PutNumber(id);
}


//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: DiagnosticVentInop()
//
//    Processing: This routine simply returns control to the calling routine.
//      A Vent Inop during diagnostics does not bring down the system
//
//    Input Parameters: Vent Inop ID code
//    Output Parameters:None
//    Return Values:    None
//    Pre-Conditions:   None
//    Miscellaneous:    None
//    Requirements:  
//
//******************************************************************************
void ErrorHandler::DiagnosticVentInop(E_EventCode id)
{
    // In diagnostics mode don't do anything.
    return;
}

