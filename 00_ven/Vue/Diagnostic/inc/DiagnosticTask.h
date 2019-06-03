//*****************************************************************************/
//$COMMON.H$
//   File Name:  DiagnosticTask.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: DiagnosticTask
//      A task to control the execution of diagnostic related code. 
//*****************************************************************************/

#ifndef DIAGNOSTICTASK_H
#define DIAGNOSTICTASK_H

#include "StateMachine.h"
#include "DiagnosticEvents.h"
#include "CommonConstants.h"

// structure to carry an event plus event data
struct DiagnosticEventStruct
{
    E_DiagnosticEvent event;
    EventData* pData;

    DiagnosticEventStruct() 
    { 
        event = eNoEvent; 
        pData = NULL;
    }
};

#ifdef USE_NUCLEUS
#include "nucpp.h"

// Delcare a FIFO queue of DiagnosticEventStruct
DefineQ(DiagnosticEventStruct, Fifo);

class DiagnosticTask : public Task
{
public:
    DiagnosticTask();
    ~DiagnosticTask() { }

    STATUS Send(E_DiagnosticEvent, EventData* = NULL, UNSIGNED = NU_NO_SUSPEND);
    STATUS SendToFront(E_DiagnosticEvent, EventData* = NULL, 
        UNSIGNED = NU_NO_SUSPEND);
    STATUS SetCancelEvent(void) { return CancelEvent.Set(); }
    STATUS RetrieveCancelEvent(void) 
    {
        return CancelEvent.Retrieve(TRUE, NU_NO_SUSPEND);
    }
protected:
    // Entry Point into Task
    virtual void Entry(void);

private:
    // prevent copying
    DiagnosticTask(const DiagnosticTask&);
    void operator=(DiagnosticTask&);
    
    //$COMMON.ATTRIBUTE$
    //  Name: Queue
    //  Description: Intertask message queue which accepts only 
    //      DiagnosticEventStruct messages.
    //  Units: 
    //  Range: 
    DiagnosticEventStructFifoQ Queue;
    
    //$COMMON.ATTRIBUTE$
    //  Name: CancelEvent
    //  Description: Event is set when the SST/EST test is canceled. 
    //  Units: 
    //  Range: 
    Event CancelEvent;
};

DiagnosticTask& TheDiagnosticTask(void);
#else
// dummy class to be used only in test mode
typedef int STATUS;


class DiagnosticTask
{
public:
    static DiagnosticTask* S_GetInstance(void) { return NULL; }
    DiagnosticTask ();
    STATUS Send                
        (
            E_DiagnosticEvent,
            EventData* = NULL,
            UNSIGNED = NU_SUSPEND
        );
    STATUS SendToFront
        (
            E_DiagnosticEvent,
            EventData* = NULL,
            UNSIGNED = NU_SUSPEND
        );
    STATUS SetCancelEvent(void) { return NU_SUCCESS; }
};
DiagnosticTask& TheDiagnosticTask(void);
#endif //USE_NUCLEUS
#endif //DIAGNOSTICTASK_H
