/******************************************************************************/
//$COMMON.CPP$
//   File Name:  DiagnosticPhase.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose:
//      DiagnosticPhase is the breath phase entered when the ventilator goes into
//      a non-ventilation mode.  In this phase, the delivery valves are 
//      closed and the exhalation valve is open.
//
//   Interfaces:
//
//   Restrictions:
//      There is only one instance of this class.
/******************************************************************************/

#include "DiagnosticPhase.h"
#include "PhaseMgr.h"
#include "MgrBdSetting.h"
#include "MgrBreathData.h"
#include "CommonTypes.h"
#include "diagnostic.h"
#include "MgrDiscreteOutput.h"
#include "MgrFlowController.h"
#include "PressureSensor.h"

// Define statics
#define VERIABLE_SIZE   13

DiagnosticPhase* DiagnosticPhase::S_Instance = NULL;

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: S_GetInstance()
//
//    Processing:
//      Since only 1 instance of this object should be created, any access
//      to the object comes via this operation.  If any other object wants to
//      access the DiagnosticPhase object, it calls DiagnosticPhase::S_GetInstance(),
//      which returns a pointer to the DiagnosticPhase object.
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
//      DiagnosticPhase* - pointer to the object instantiated from this class
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
DiagnosticPhase* DiagnosticPhase::S_GetInstance(void)
{
    // If the object has not already been created, create it.
    if (NULL == S_Instance)
    {
        S_Instance = new DiagnosticPhase();
        ASSERTION(S_Instance != NULL);
    }
    return(S_Instance);

}   // end S_GetInstance()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: DiagnosticPhase()
//
//    Processing: 
//      This operation is the constructor for the DiagnosticPhase.  It invokes the
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
DiagnosticPhase::DiagnosticPhase(void) : Phase()
{
//    EVAPtr = NULL;
    receivedSize = 0;

//    DiagnosticQueuePtr = new Q("DIANOSTIC QUEUE", (20 * (DIAGNOSTIC_Q_MSG_SIZE+1)), VERIABLE_SIZE, DIAGNOSTIC_Q_MSG_SIZE);
//    ASSERTION(DiagnosticQueuePtr != NULL);

//    TheHardwareDiagnostic();

}   // end DiagnosticPhase()

DiagnosticPhase::~DiagnosticPhase()
{
//    delete DiagnosticQueuePtr;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Start()
//
//    Processing:
//      This operation is invoked at the start of every non-ventilation phase.
//
//      This operation interfaces with the air and oxygen flow controllers to
//      close the delivery valves and with the exhalation controller to open
//      the exhalation valve.
//
//      The Start() operation then calls SystemClock::Retrieve() to snapshot
//      the system time at the start of the non-ventilation phase.  It also 
//      calls MgrBreathData::NonVentStart() to update breath data at the start
//      of the non-ventilation phase.
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
void DiagnosticPhase::Start(void)
{
//    EVAPtr = ExhValveAlign::S_GetInstance();
//    ASSERTION( EVAPtr != NULL );

    MgrFlowController::AirFlowController->Disable();
    MgrFlowController::O2FlowController->Disable();
    PressureSensor::ExhController->Disable();

    // Update Patient Data
    (MgrBreathData::S_GetInstance())->NonVentStart();
}   // end Start()



/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: End()
//
//    Processing:
//      This operation is invoked at the end of the non-ventilation phase.
//      It performs no work but is needed since it is defined as pure
//      virtual in the base class Phase.
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
void DiagnosticPhase::End(void)
{
}   // end End()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessBdCycle()
//
//    Processing:
//      This operation is invoked at the end of the non-ventilation phase. 
//      It performs no work but is needed since it is defined as pure
//      virtual in the base class Phase.
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
void DiagnosticPhase::ProcessBdCycle(void)
{
//    STATUS status;

        // get event message
//    status = DiagnosticQueuePtr->Retrieve
//    (
//        &diagnosticMessage,
//        DIAGNOSTIC_Q_MSG_SIZE,
//        &receivedSize
//    );

//    switch (status)
//    {
//        case Q_SUCCESS:
//            // route the message to the Diagnostic class
//            TheHardwareDiagnostic().RouteMessage(diagnosticMessage.eventStruct);
//            break;
//        case QUEUE_EMPTY:
//            break;
//        default:
//            ASSERTION(0);
//    }
//    TheHardwareDiagnostic().Run();
}   // end ProcessBdCycle()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Send(CommandData &event)
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
STATUS DiagnosticPhase::Send(CommandData &event)
{
    // This value specifies the size of a Basic Event.
//    const UNSIGNED EVENT_SIZE =
//            (sizeof(CommandData) + sizeof(UNSIGNED) - 1)/sizeof(UNSIGNED);

    //    return (DiagnosticQueuePtr->Send(&event, EVENT_SIZE));
    return 0;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Send(CommandData &event)
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
STATUS DiagnosticPhase::Send(MotorEventData &event)
{
    // This value specifies the size of a Basic Event.
//    const UNSIGNED EVENT_SIZE =
//            (sizeof(MotorEventData) + sizeof(UNSIGNED) - 1)/sizeof(UNSIGNED);

    //    return (DiagnosticQueuePtr->Send(&event, EVENT_SIZE));
    return 0;
}
