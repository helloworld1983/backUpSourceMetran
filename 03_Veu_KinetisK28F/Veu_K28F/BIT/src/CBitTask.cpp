/******************************************************************************/
//$COMMON.CPP$
//   File Name:  CbitTask.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: 
//      This file contains the CbitTask class, which defines the
//      CbitTask entry point.  This class is derived from the Nucleus 
//      Plus RTOS Task class.  This task runs as the lowest priority in
//      the system and is responsible for performing low level background
//      checks.  
//
//   Interfaces:
//      The CbitTask interfaces with all of the CbitTest objects and associated
//      timers.
//
//   Restrictions:
//      There is only 1 instance of this class.
/******************************************************************************/
//#include "Tasks.h"
//#include "Monitor.h"
#include "CbitTask.h"
#include "AzTest.h"
#include "HighTempTest.h"
#include "AdcDacTest.h"
#include "CbitTimer.h"
#include "ErrorHan.h"
#include "Util.h"
#include "TaskCommon.h"

#include "FreeRTOS.h"
#include "task.h"

#include "DebugTrace.h"

//test send event to pd task
#include "PdTask.h"

//  Define Statics
CbitTask* CbitTask::S_Instance = NULL;

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: S_GetInstance()
//
//    Processing:
//      Since only 1 instance of this object should be created, any access
//      to the object comes via this operation.  If any other object wants
//      access to the CbitTask object, it invokes
//      CbitTask::S_GetInstance(), which returns a pointer to the
//      CbitTask object.
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
//      CbitTask* - pointer to the object instantiated from this class
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
CbitTask* CbitTask::S_GetInstance(void)
{
    // If the object has not already been created, create it.
    if (NULL == S_Instance)
    {
        S_Instance = new CbitTask();
        ASSERTION(S_Instance != NULL);
    }

    return(S_Instance);

}   // end S_GetInstance()



/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Send()
//
//    Processing:
//      This operation invokes the Nucleus Plus RTOS kernel servis Q::Send()
//      to place a Batch Setting Event on the CBIT Queue.  This operation is
//      not invoked in the CBIT Task thread of control; it is called by other
//      tasks wanting to communicate with the CBIT Task via its queue.
//
//    Input Parameters:
//      batchEvent - reference to a Batch Settings Change Event message
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
STATUS CbitTask::Send(E_EventCode code, UNSIGNED suspend)
{
//    return(CbitTask::QueuePtr->Send(&code, CBIT_Q_MSG_SIZE, suspend));

}   // end Send()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CBitCreateTask()
//
//    Processing: Create task
//
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
void CbitTask::CBitCreateTask (void)
{
    xTaskCreate (Entry, "BDTask", CBIT_TASK_SIZE,
                 NULL, TASK_PRIO_CBIT_TASK, (TaskHandle_t *) NULL);
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Entry()
//
//    Processing: 
//      This operation serves as the CBIT  Task entry point.  It loops through
//      the list of CBIT Tests and runs the ones that are due to be run.  It
//      processes failed tests by invoking the appropriate error handler.
//
//      After running the tests, the CBIT Task checks its queue to see if there
//      are any events to be logged.  If so, it logs them.
//
//      Finally, if the CLEAR_STRIKES_TIME has elapsed since power up, the 
//      CBIT Task resets the error strikes count in NVRAM.
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
void CbitTask::Entry(void* pvParameters)
{
//    STATUS status;
//    E_EventCode eventCode;
//    UNSIGNED receivedSize;
//	Monitor * monPtr = NULL;
//    UNSIGNED startTime;
    const int FOREVER = 1;
    volatile UBaseType_t uxHighWaterMark;
    uxHighWaterMark = uxTaskGetStackHighWaterMark( NULL );
    DEBUG_CBIT("StackSize ENTER CBIT: %d \n", uxHighWaterMark);

//    E_Bool logResetComplete = eFalse;

	// Instantiate watch dog monitor
//	monPtr = new Monitor(eCbit);
//	ASSERTION(monPtr != NULL);
    
    // Snapshot the system clock.  This startTime is used to determine when
    // to erase the error log
//    startTime = SystemClock::Retrieve();

    while (FOREVER)
    {
    	DEBUG_CBIT("CBit task\n");

    	DEBUG_CBIT("cbit send event 04'\n");
    	PdTask::S_GetInstance()->S_SetEventFlag(START_TIMED_EVENT_FLAG);

//		monPtr->MonitorBegin();

        // Loop through list of tests and run them if they are due.
//        for (SHORT ii=0; ii < eNumCbitTests; ii++)
//        {
//            if (eTrue == TestList[ii].isTimeToRun)
//            {
//                // Run the test.
//                if (eTestFailed == TestList[ii].testPtr->Run())
//                {
//                    TestList[ii].testPtr->Handle();
//                    TestList[ii].isTimeToRun = eFalse;
//                }
//
//                // If this is a time based test, reset the isTimeToRun element;
//                // it will be set to TRUE again by the timer expiration
//                // routine the next time it is time to run.
//                if (NULL != TestList[ii].timerPtr)
//                {
//                    TestList[ii].isTimeToRun = eFalse;
//                }
//            }
//        }
        
        
        // loop here and pull all messages off queue!!!!
        // is this what we want to do?
        
//        status = NU_SUCCESS;
//        while (status == NU_SUCCESS)
//        {
//            status = QueuePtr->Receive(&eventCode, CBIT_Q_MSG_SIZE,
//                            &receivedSize, NU_NO_SUSPEND);
            
//            if (status == NU_SUCCESS)
//            {
                // For now, there is no extra information logged - just eventCode
//                (ErrorHandler::S_GetInstance())->LogEvent(eventCode, NULL, NU_SUSPEND);
//            }
//            else
//            {
                // The only other expected status is that the queue is empty.
//                ASSERTION(status == NU_QUEUE_EMPTY);
//            }

//        }
        
        // Check if enough time has elapsed since system restart to clear
        // the error strikes.   If 3 strikes occur within CLEAR_STRIKES_TIME,
        // the system is put into a Vent Inop state.
//        if ((eFalse == logResetComplete) &&
//            (CalcElapsedTime(startTime) >= CLEAR_STRIKES_TIME))
//        {
//            systemSaveNvram->SetValue(eRestartCount, (LONG) 0, NU_NO_SUSPEND);
//            logResetComplete = eTrue;
//        }
        
//		monPtr->MonitorEnd();
        vTaskDelay(CBIT_TASK_DELAY_TIME);
        uxHighWaterMark = uxTaskGetStackHighWaterMark( NULL );

        DEBUG_CBIT("StackSize REMAIN CBIT: (%d)  USE (%d)\n", uxHighWaterMark, (CBIT_TASK_SIZE - uxHighWaterMark)*4);

    } //end while(FOREVER)
}   // end Entry()



/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CbitTask()
//
//    Processing: 
//      This operation is the CBIT  Task constructor.  It invokes the 
//      base Task class constructor.  The CBIT  Task is a preemptable 
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
CbitTask::CbitTask(void)
//    Task (tasks[eCbit].Name, tasks[eCbit].StackSize, tasks[eCbit].Pri,
//          tasks[eCbit].TimeSlice, tasks[eCbit].PreEmption)
      
{
    SHORT ii;   // loop control
    SHORT numFlowTableEntries;
    ULONG* flowTableCrcPtr;

//#pragma weak __ghsbegin_flowtables
//    extern void* __ghsbegin_flowtables;

    // For now this queue handles fixed size messages of only the event code
    // size
//    QueuePtr = new Q("CBIT Q", (10 * (CBIT_Q_MSG_SIZE+1)), NU_FIXED_SIZE,
//                        CBIT_Q_MSG_SIZE, NU_FIFO);

    // Initialize to a known state    
    for (ii = 0; ii < eNumCbitTests; ii++)
    {
        TestList[ii].testPtr = NULL;
        TestList[ii].timerPtr = NULL;
        TestList[ii].isTimeToRun = eFalse;
    }    
    
    // Now set up the tests individually
    
    // The CRC Tests are not time driven and should be invoked every
    // cycle of the CBIT task.
//    TestList[eCbitOnChipCrcTestId].testPtr =
//        new CbitCrcTest(eCbitOnChipCrcTestCode, VentInopHandler,
//                        (ULONG) OTP_START, OTP_BYTES, pOTPId->ImageCRCValue);
    TestList[eCbitOnChipCrcTestId].isTimeToRun = eTrue;

//    TestList[eCbitProgramCrcTestId].testPtr =
//        new CbitCrcTest(eCbitProgramCrcTestCode, VentInopHandler,
//                        (ULONG) FLASH_PROGRAM_START, pFlashId->TotalBytes,
//                        pFlashId->ImageCRCValue);
    TestList[eCbitProgramCrcTestId].isTimeToRun = eTrue;

    // There are 3 flow sensors, each with FLOW_TABLE_SIZE entries 
//    numFlowTableEntries = FLOW_TABLE_SIZE * 3;
//    flowTableCrcPtr = (ULONG*) (&__ghsbegin_flowtables) + numFlowTableEntries;

//    TestList[eCbitFlowTableCrcTestId].testPtr =
//        new CbitCrcTest(eCbitFlowTableCrcTestCode, VentInopHandler,
//                        (ULONG) &__ghsbegin_flowtables,
//                        (ULONG) (numFlowTableEntries * sizeof(LONG)),
//                        *flowTableCrcPtr);
        
    TestList[eCbitFlowTableCrcTestId].isTimeToRun = eTrue;

    // The RAM Test is not time driven and should be invoked every
    // cycle of the CBIT task.
//    TestList[eCbitRamTestId].testPtr =
//        new CbitRamTest(eCbitRamTestCode, RecoverableHandler, RAM_BASE, RAM_SIZE);
//    TestList[eCbitRamTestId].isTimeToRun = eTrue;
    
    // The High Temperature Test is run on a scheduled time interval.
    TestList[eCbitHighTempTestId].testPtr = HighTempTest::S_GetInstance();
    TestList[eCbitHighTempTestId].timerPtr = 
            new CbitTimer((CHAR*)"HITEMP", HIGH_TEMP_TIME, eCbitHighTempTestId);
    TestList[eCbitHighTempTestId].isTimeToRun = eFalse;

    // The ADC/DAC Wraparound Test is run on a scheduled time interval.
    TestList[eCbitAdcDacTestId].testPtr = AdcDacTest::S_GetInstance();
    TestList[eCbitAdcDacTestId].timerPtr = 
                new CbitTimer((CHAR*)"ADCDAC", ADC_DAC_TIME, eCbitAdcDacTestId);
    TestList[eCbitAdcDacTestId].isTimeToRun = eFalse;

    // Make sure the entire TestList got initialized
    for (ii = 0; ii < eNumCbitTests; ii++)
    {
//        ASSERTION(TestList[ii].testPtr != NULL);
    }
    
    // Instantiate the auto-zero tests.  These are not 
    // run as part of the CBIT Task thread of control, but they are created here.
    AzTest::S_Init(eTrue);

}   // end CbitTask()








