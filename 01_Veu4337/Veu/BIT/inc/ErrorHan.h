//******************************************************************************
//$COMMON.H$
//   File Name:  ErrorHan.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class:  ErrorHandler
//
//      This class is the interface to be used by the rest of the software
//      for dealing with various system anomalies.
//      The ErrorHandler logs the events in the event log and handles
//      the appropriate system shutdown procedure.
//******************************************************************************
#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

#include "standard.h"
#include "CommonConstants.h"
#include "logentry.h"

#ifdef  ERROR_HANDLER_CPP
#define EXTERN
#else
#define EXTERN extern
#endif

//
// Error Handler Class Definition
//
class ErrorHandler
{
    private:
        ErrorHandler();
	
    public:

        static ErrorHandler* S_GetInstance();

        void HandleVentInop(E_EventCode id, char * info = NULL);
        void HandleRecoverable(char * info = NULL);
        void LogEvent(E_EventCode id,
                      char * info = NULL, 
                      UNSIGNED suspend = NU_SUSPEND);

        static ErrorHandler* J_GetInstance();
        void J_HandleVentInop(E_EventCode id, char * info = NULL);
        void J_HandleRecoverable(char * info = NULL);

    protected:
        void TextModeVentInop(E_EventCode id);
        void DiagnosticVentInop(E_EventCode id);
                      
        static ErrorHandler* S_Instance;  //Single static instance of the CSI class
};

#undef EXTERN
#endif // ERROR_HANDLER_H

