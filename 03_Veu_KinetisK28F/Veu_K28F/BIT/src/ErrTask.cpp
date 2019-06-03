/******************************************************************************/
//$COMMON.CPP$
//   File Name:  ErrTask.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: 
//      This file contains the ErrTask class, which defines the
//      ErrTask entry point.  This class is derived from the Nucleus 
//      Plus RTOS Task class.  This task runs at a high priority in
//      the system and is responsible for performing motor error checks and 
//		valve stuck error handling
//
//   Interfaces:
//      The ErrTask interfaces with all of the ErrTest objects and associated
//      timers.
//
//   Restrictions:
//      There is only 1 instance of this class.
/******************************************************************************/
#include "ErrTask.h"
#include "TestHandler.h"
#include "ErrorHan.h"
#include "AlarmTask.h"

#include "DebugTrace.h"

//  Define Statics
ErrTask* ErrTask::S_Instance = NULL;

E_Bool ErrTask::AirMotorError = eFalse;
E_Bool ErrTask::O2MotorError  = eFalse;
E_Bool ErrTask::ExhMotorError = eFalse;

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CheckMotorErrors()
//
//    Processing: 
//      Check motor error flags and send event code to indicate errors.   Once
//		the error is latched the motor error is not processed again.  
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
void ErrTask::CheckMotorErrors(void)

{
	STATUS status;

	static E_Bool AirMotorErrorLatch = eFalse;
	static E_Bool O2MotorErrorLatch  = eFalse;
	static E_Bool ExhMotorErrorLatch = eFalse;


	if (eFalse == AirMotorErrorLatch)
		if (eTrue == AirMotorError)
		{
			AirMotorErrorLatch = eTrue;
			status = Send(eCbitAirMotorError);

			// if posting to queue failed reset latch
			if (NU_SUCCESS != status)
				AirMotorErrorLatch = eFalse;
		}
	
	if (eFalse == O2MotorErrorLatch)
		if (eTrue == O2MotorError)
		{
			O2MotorErrorLatch = eTrue;
			status = Send(eCbitO2MotorError);

			// if posting to queue failed reset latch
			if (NU_SUCCESS != status)
				O2MotorErrorLatch = eFalse;
		}

	if (eFalse == ExhMotorErrorLatch)
		if (eTrue == ExhMotorError)
		{
			ExhMotorErrorLatch = eTrue;
			status = Send(eCbitExhMotorError);

			// if posting to queue failed reset latch
			if (NU_SUCCESS != status)
				ExhMotorErrorLatch = eFalse;
		}

}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: DoEventCode()
//
//    Processing: 
//      Perform operation base on event code.  
//
//    Input Parameters:
//      E_EventCode eventCode
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
void ErrTask::DoEventCode(E_EventCode eventCode)

{
	switch (eventCode)
	{
	case eCbitO2ValveClosedTestCode:     // 4011
		O2ValveClosedHandler(eventCode);
		break;
	case eCbitExhValveOpenTestCode:      // 4007
        // Notify Alarms of the Exh Valve Stuck condition.
        // continue ventilation as best as possible
          
        AlarmTask::S_GetInstance()->SetIsActive(eExhValveStuckOpen, eActive);
		break;
	case eCbitO2ValveOpenTestCode:       // 4009
	case eCbitExhValveClosedTestCode:    // 4012
	case eCbitO2MotorError:              // 4020
	case eCbitExhMotorError:             // 4021
		VentInopHandler(eventCode);
		break;
	case eCbitAirValveOpenTestCode:      // 4008
	case eCbitAirValveClosedTestCode:    // 4010
	case eCbitAirMotorError:             // 4019
		AirSourceFaultHandler(eventCode);
		break;
	default:
		// should never reach this statement!
		ASSERTION(eFalse);
		break;
	} // switch

}


/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Entry()
//
//    Processing: 
//      This operation serves as the Err Task entry point.  
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
void ErrTask::Entry(void)
{
	const int FOREVER = 1;

	STATUS status;
    E_EventCode eventCode;
    UNSIGNED receivedSize;
    
    while (FOREVER)
    {
//        status = QueuePtr->Receive(&eventCode, ERR_Q_MSG_SIZE,
//                        &receivedSize, NU_SUSPEND);
        
        if (NU_SUCCESS == status)
        {
			DoEventCode(eventCode);
        }
        else
        {
            // The only other expected status is that the queue is empty.
//            ASSERTION(NU_QUEUE_EMPTY == status);
        }

    } // while (FOREVER)


}   // end Entry()


/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Send()
//
//    Processing: 
//      This operation invokes the Nucleus Plus RTOS kernel servis Q::Send()
//      to place an Event on the Err Queue.  This operation is
//      not invoked in the Err Task thread of control; it is called by other
//      tasks wanting to communicate with the Err Task via its queue.
//
//    Input Parameters:
//      suspend - indicates if the calling task should suspend if the queue
//          is full
//
//    Output Parameters:
//      None
//
//    Return Values:
//      STATUS - completion status for the Nucleus Plus kernel service Send()
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
STATUS ErrTask::Send(E_EventCode code, UNSIGNED suspend)
{
//    return(ErrTask::QueuePtr->Send(&code, ERR_Q_MSG_SIZE, suspend));

}   // end Send()




/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ErrTask()
//
//    Processing: 
//      This operation is the Err  Task constructor.  It invokes the 
//      base Task class constructor.  The Err  Task is a preemptable 
//      task.  It runs at the lowest priority in the system using all 
//      remaining throughput.
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
ErrTask::ErrTask(void)
//    Task (tasks[eErrHand].Name, tasks[eErrHand].StackSize, tasks[eErrHand].Pri,
//          tasks[eErrHand].TimeSlice, tasks[eErrHand].PreEmption)
      
{

    // For now this queue handles fixed size messages of only the event code
    // size
//    QueuePtr = new Q("Err Q", (10 * (ERR_Q_MSG_SIZE+1)), NU_FIXED_SIZE,
//                        ERR_Q_MSG_SIZE, NU_FIFO);

}   // end ErrTask()








