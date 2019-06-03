//$COMMON.CPP$
//   File Name:  PDTASK.CPP
//   Copyright 1996 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose:
//              The Patient Data computation is scheduled through the use of 
//				event flags from the Breath Delivery subsystem.  The calculation 
//				of the patient data must keep up the with Bd subsystem's 
//				provision of flow and pressure readings otherwise system will go 
//				through system reset condition.  End Inhalation Pressure, and 
//				Peak Inhalation Pressure are computed during the completion of 
//				Inhalation Phase signaled by the Breath Delivery subsystem.  
//				Exhalation Tidal Volume, End Exhalation Pressure, Exhaled Minute 
//				Volume, Mean Airway Pressure, Spontaneous Respiratory Rate, 
//				Total Respiratory Rate, Inhalation and Exhalation Ratio 
//				(I:E ratio), and Rapid    Shallow Breathing Index will be 
//				computed during the end of the Exhalation Phase signaled by the 
//				Breath Delivery subsystem.  Computation of the Delivered %O2 is 
//				computed every second, signaled by O2Timer. 
//
//   Interfaces: Breath Delivery - event flags for signaling the inhalation
//              and exhalation phases, upon detection of a non-breathing mode
//              ie. SVO all patient data shall be blanked and history data shall
//              be cleared.
//
//   Restrictions: There is only one instance of this object
/******************************************************************************/

// Include standard project definitions
#include "PdTask.h"
#include "PtData.h"
#include "CommonTypes.h"

// FreeRTOS includes
#include "FreeRTOS.h"
#include "task.h"

//Utils includes
#include "TaskCommon.h"

//EventGroup class includes
#include "EventGroup.h"

static E_Bool nonbreathingmode = eTrue;

// instanciate Patient Data Task instance pointer
PdTask* PdTask::S_Instance = NULL;

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: S_GetInstance()
//
//    Processing:
//      Since only 1 instance of this object should be created, any access
//      to the object comes via this operation.  If any other object wants
//      access to the object, it invokes S_GetInstance(),
//      which returns a pointer to the object.
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
//      * - pointer to the object instantiated from this class
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
PdTask* PdTask::S_GetInstance (void)
{
    //if the pointer does not exits, create it
    if(S_Instance == NULL)
    {
        S_Instance = new PdTask ();
    }

    ASSERTION(S_Instance != NULL);

    return (S_Instance);
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: PDCreateTask
//
//    Processing: Create PD task
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
void PdTask::PDCreateTask (void)
{
    //Create PD task
    xTaskCreate (Entry, "PDTask", PD_TASK_SIZE,
                 NULL,TASK_PRIO_PD_TASK, (TaskHandle_t *) NULL);
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: PdTask   
//
//    Processing: constructor for class
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
PdTask::PdTask ()
{

}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: S_SetEventFlag()
//
//    Processing:
//      This operation invokes the Nucleus Plus RTOS kernel service Set()
//      to set an event flag in the PD Event Flag Group.  This operation is
//      not invoked in the Pd Task thread of control; it is called by other
//      tasks wanting to communicate with the Pd Task via its event flags.
//
//    Input Parameters:
//      eventFlags - event flags to be set
//      operation - operation to be performed.  NU_OR causes the event flags
//          passed as an input to be ORed with the current event flags in the
//          group.  NU_AND causes the event flags to be ANDed with the current
//          event flags in the group.
//
//    Output Parameters: None
//
//    Return Values:
//      STATUS - completion status for the Nucleus Plus kernel service Set
//
//    Pre-Conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
STATUS PdTask::S_SetEventFlag (UNSIGNED eventFlags)
{
    // Return the completion status from the RTOS kernel service call
    STATUS returnStatus;
    returnStatus = EventGroup::S_GetInstance ()->S_SetEventFlag (eventFlags);

    return returnStatus;

}   // end SetEventFlags()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: PdTask::Entry   
//
//    Processing:
//					Entry point into patient data task
//					Endless loop pull off O2, Inhalation & Exhalation,
//					and non breathing mode event flags.  When a non
//					breathing mode is entered all other events are
//					ignored and consumed.  O2 events are ignored until
//					an inhalation has occurred. This is done to prevent getting
//					O2 settings before breath delivery has them.
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
void PdTask::Entry (void* pvParameters)
{
    const int FOREVER = 1;
    const int HAVE_NO_EVENT = 0;

    STATUS status;
    Ptdata *pd = NULL;
    E_Bool ProcessO2 = eFalse;

    //	Monitor * monPtr = NULL;

    // Instantiate watch dog monitor
    //	monPtr = new Monitor(ePd);
    //	ASSERTION(monPtr != NULL);

    // application initialization
    pd = Ptdata::S_GetInstance ();
    ASSERTION(NULL != pd);

    // send blanked data to GUI
    pd->ClearHistory ();

    while (FOREVER)
    {
        DEBUG_PD("PD task\n");

        // wait for an event flag and consume it
        //after return , all bit is clear
        status = EventGroup::S_GetInstance ()->S_WaitEventFlag (ANY_PT_DATA_EVENT_FLAG);
        DEBUG_PD("\t\t%x\n", status);

        //        	    monPtr->MonitorBegin();

        if(HAVE_NO_EVENT != status)
        {
            // signal OK to start processing O2 events
            if(O2_SETTING_EVENT_FLAG == status)
            {
                ProcessO2 = eTrue;
            }

            // if non breathing mode event occurs clear history tables and
            // consume all other events too
            if(NONBREATHING_MODE_EVENT_FLAG == status)
            {
                nonbreathingmode = eTrue;
                pd->ClearHistory ();
            }
            else
            {
                //start of inhalation event, signals patient data
                // to perform exhalation calculations
                if(START_INH_EVENT_FLAG == status)
                {
                    nonbreathingmode = eFalse;
                    pd->DoExhalationData ();
                }

                // start of exhalation event, signals patient data
                // to perform inhalation calculations
                if(START_EXH_EVENT_FLAG == status)
                {
                    nonbreathingmode = eFalse;
                    pd->DoInhalationData ();
                }

                // Bd's signal to Pd that ready to process HFO data.
                if( HFO_DATA_READY_EVENT_FLAG == status)
                {
                    nonbreathingmode = eFalse;
                    pd->DoHFOData();
                }

                // Bd's signal to Pd that ready to update APRV and NCPAP
                if( UPDATE_APRV_BR_FLAG == status)
                {
                    nonbreathingmode = eFalse;
                    pd->DoAPRVBR();
                }

                // Bd's signal to Pd that ready to update APRV and NCPAP
                if(UPDATE_NCPAP_DATA_FLAG == status)
                {
                    nonbreathingmode = eFalse;
                    pd->DoNCPAP();
                }

                // APRV P_LOW
                if(APRV_PLOW_START == status)
                {
                    nonbreathingmode = eFalse;
                    pd->DoAPRV_PHigh();
                }

                // APRV P_HIGH
                if(APRV_PHIGH_START == status)
                {
                    nonbreathingmode = eFalse;
                    pd->DoAPRV_PLow();
                }

                // start of timed event, signals patient data to
                // perform O2 calculations
                if (START_TIMED_EVENT_FLAG == status)
                {
                    if (ProcessO2 == eTrue)
                        pd->DoO2();
                }

                // Bd's signal to Pd that ready to exit standby mode
                if(EXIT_STANDBYMODE_FLAG == status)
                {
                    nonbreathingmode = eFalse;
                    pd->GetCircuitComplianceFactor();
                }

                // Bd's signal to Pd that ready to clear compensate
                if(CLEAR_COMPENSATE_DATA == status)
                {
                    pd->ClearExhLeakArray();
                }

                // Bd's signal to Pd that ready to end exhalation of APRV
                if(END_EXH_PS_APRV == status)
                {
                    pd->DoEndExhPSInAPRV();
                }

                // Bd's signal to Pd that ready to end inhalation of APRV
                if(END_INH_PS_APRV == status)
                {
                    pd->DoEndInhPSInAPRV();
                }
            }

        }
        //        monPtr->MonitorEnd();

    } // while (FOREVER) ....

}

