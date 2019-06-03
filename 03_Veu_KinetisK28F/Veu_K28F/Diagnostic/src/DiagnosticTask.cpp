//$COMMON.CPP$
//   File Name:  DiagnosticTask.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: A task to control the execution of diagnostic related code. 
//
//   Interfaces:
//
//   Restrictions: The DiagnosticTask needs to have a lower priority than the 
//      GUITask so user events can be processed in a timely fashion. Some of the
//      diagnostic tests can take a significant amount of time (seconds) during
//      which all lower priority tasks will be held off. So any tasks
//      that need servicing on a regular basis need a priority level higher 
//      than the DiagnosticTask. For the GUITask this is especially important
//      since if the user presses a key, for instance, we don't want a large 
//      response lag time.
/******************************************************************************/

#include "DiagnosticTask.h"
#include "CommonTypes.h"
#include "Diagnostic.h"
#include "DiagnosticGUI.h"

#ifdef USE_NUCLEUS
#include "Tasks.h"
#include "AzTest.h"
#include "nucpp.h"

//*****************************************************************************/
//    Operation Name: TheDiagnosticTask()
//
//    Processing: Return a reference to a local static instance of this class. 
//      All access to this class should be made through this function. 
//
//    Input Parameters:
//
//    Output Parameters:
//
//    Return Values:
//      DiagnosticTask& - a ref to the only instance of this class. 
//
//    Pre-Conditions:
//
//    Miscellaneous:
//
//    Requirements:  
//
//*****************************************************************************/
DiagnosticTask& TheDiagnosticTask(void)
{
    static DiagnosticTask diagnosticTask;
    return diagnosticTask;
}
    
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: DiagnosticTask()
//
//    Processing: Class constructor. 
//
//    Input Parameters: 
//
//    Output Parameters:
//
//    Return Values: 
//
//    Pre-conditions:
//
//    Miscellaneous: 
//
//    Requirements: 
//
/******************************************************************************/
DiagnosticTask::DiagnosticTask () : 
    Task
    (
        tasks[eDiagnostics].Name, 
        tasks[eDiagnostics].StackSize, 
        tasks[eDiagnostics].Pri, 
        tasks[eDiagnostics].TimeSlice, 
        tasks[eDiagnostics].PreEmption
    ),
    Queue("Diagnostic Event Q", 10) 
{
    // create the diagnostic class category
    new Diagnostic;

    AzTest::S_Init(eFalse);
}

//*****************************************************************************/
//    Operation Name: Send()
//
//    Processing: Post a message to the DiagnosticTask message queue.
//
//    Input Parameters:
//      eventStruct - a valid DiagnosticEventStruct.
//      suspend - the suspend type requested if request cannot be fulfilled. 
//
//    Output Parameters:
//      STATUS - the return status of the OS Send operation.
//
//    Return Values:
//
//    Pre-Conditions:
//
//    Miscellaneous:
//
//    Requirements:  
//
//*****************************************************************************/
STATUS DiagnosticTask::Send                
    (
        E_DiagnosticEvent event, 
        EventData* pData,
        UNSIGNED suspend
    )
{
    // create event struct and load with the event and event data
    DiagnosticEventStruct eventStruct;
    eventStruct.event = event;
    eventStruct.pData = pData;

    // post message to the task
    STATUS status = Queue.Send(eventStruct, suspend);
    DEBUG_ASSERTION(status == NU_SUCCESS || status == NU_QUEUE_FULL);

    // if message could not be posted, delete the data
    if (status == NU_QUEUE_FULL && pData)
        pData->Delete();

    return status;
}

//*****************************************************************************/
//    Operation Name: SendToFront()
//
//    Processing: Post a message to the DiagnosticTask message queue at the 
//      front of the list.
//
//    Input Parameters:
//      eventStruct - a valid DiagnosticEventStruct.
//      suspend - the suspend type requested if request cannot be fulfilled. 
//
//    Output Parameters:
//      STATUS - the return status of the OS Send operation.
//
//    Return Values:
//
//    Pre-Conditions:
//
//    Miscellaneous:
//
//    Requirements:  
//
//*****************************************************************************/
STATUS DiagnosticTask::SendToFront
    (
        E_DiagnosticEvent event, 
        EventData* pData,
        UNSIGNED suspend
    )
{
    // create event struct and load with the event and event data
    DiagnosticEventStruct eventStruct;
    eventStruct.event = event;
    eventStruct.pData = pData;

    // post message to the task
    STATUS status = Queue.SendToFront(eventStruct, suspend);
    DEBUG_ASSERTION(status == NU_SUCCESS || status == NU_QUEUE_FULL);

    // if message could not be posted, delete the data
    if (status == NU_QUEUE_FULL && pData)
        pData->Delete();

    return status;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Entry()
//
//    Processing: Entry point into Self Test task.
//
//    Input Parameters: 
//
//    Output Parameters:
//
//    Return Values: 
//
//    Pre-conditions: 
//
//    Miscellaneous: 
//
//    Requirements: 
//
/******************************************************************************/
void DiagnosticTask::Entry(void)
{ 
    DiagnosticEventStruct eventStruct;
    STATUS status;

    // create the interface to the from diagnostic GUI to diagnostic 
    new DiagnosticGUI;

    // infinite task loop
    while(eTrue)
    {
        // get event message. Note task suspends here until a message arrives.
        status = Queue.Receive(eventStruct);
        switch (status) 
        {
            case NU_SUCCESS:
                // route the message to the Diagnostic class
//                TheDiagnostic().RouteMessage(eventStruct);
                break;
            case NU_QUEUE_EMPTY:
                break;
            default:
                ASSERTION(0);
        }
        
        // clear the event every spin (needed for the delay break-out to work)
        CancelEvent.Clear();
    } 
} 

#else //USE_NUCLEUS
DiagnosticTask& TheDiagnosticTask(void)
{
    static DiagnosticTask diagnosticTask;
    return diagnosticTask;
}
STATUS DiagnosticTask::Send                
    (
        E_DiagnosticEvent event, 
        EventData* pData,
        UNSIGNED suspend
    )
{
    DiagnosticEventStruct eventStruct;
    eventStruct.event = event;
    eventStruct.pData = pData;

//    TheDiagnostic().RouteMessage(eventStruct);

    return NU_SUCCESS;
}
STATUS DiagnosticTask::SendToFront 
    (
        E_DiagnosticEvent event, 
        EventData* pData,
        UNSIGNED suspend
    )
{
    DiagnosticEventStruct eventStruct;
    eventStruct.event = event;
    eventStruct.pData = pData;

//    TheDiagnostic().RouteMessage(eventStruct);

    return NU_SUCCESS;
}
DiagnosticTask::DiagnosticTask() 
{
        // create the interface to the from diagnostic GUI to diagnostic 
//        new DiagnosticGUI;
}

#endif //USE_NUCLEUS
