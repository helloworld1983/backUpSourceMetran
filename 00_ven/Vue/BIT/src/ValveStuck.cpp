/******************************************************************************/
//$COMMON.CPP$
//   File Name:  ValveStuck.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: 
//      This file contains the ValveStuck class, which defines the
//		valve stuck error checks
//
//   Interfaces:
//      The ValveStuck class interfaces with all of the stepper motor objects,
//		FlowSensor objects, MgrBdSetting TriggerType and ErrTask object.
//
//   Restrictions:
//      There is only 1 instance of this class.
/******************************************************************************/
#include "Btps.h"
#include "ValveStuck.h"
#include "MgrBdSetting.h"
#include "ErrTask.h"
#include "tasks.h"

#include "DebugTrace.h"


ValveStuck* ValveStuck::S_Instance = NULL;

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CheckExhValveStuckOpen()
//
//    Processing:
//			During the inhalation phase of a breath the exhalation valve is 
//			commanded closed.  During this phase, both the exhalation flow 
//			sensor and the exhalation valve motor position are monitored to 
//			determine if the exhalation valve is stuck open.  The process for 
//			determining an exhalation valve stuck open condition shall apply 
//			only to the inhalation phase, and is as follows.
//
//	1.	Wait TBD(100 ms) milliseconds into the inhalation phase to allow for the 
//		exhalation valve to close.
//	2.	Integrate all exhalation flow sensor readings (BTPS compensated) greater 
//		than 3 lpm TBD .
//	3.	The exhalation valve is stuck open if the integrated flow above 3 lpm TBD 
//		ml/sec is greater than or equal to TBD ml.
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
void ValveStuck::CheckExhValveStuckOpen(void)

{														// 50 ml/sec = 
	static const LONG EXHFLOW_LIMIT = 50 * SCALE;		// 3 lpm 
														// base on min peak flow TBD
	static const LONG VOLUME_LIMIT  = 30 * SCALE;		// 30 ml based on 50 ml
														// min vol TBD
	static const UNSIGNED SOMETIME  = 100 / MS_PER_TICK;// 100 ms for exh close
	static UNSIGNED timeCounter = 0;					// ms
	static LONG volume = 0;								// ml SCALED
	LONG flowReading;									// ml/sec SCALED

	//  exhalation valve should be closed during these phases

    if ( (PhaseId != eMandInhPhase) && (PhaseId != eSpontInhPhase) &&
		 (PhaseId != ePlateauPhase) && (PhaseId != eExhHoldPhase) )
	{
		timeCounter = 0;
		volume = 0;
		return;
	}
	else
		timeCounter += BD_CYCLE_TIME_IN_MS;

	// allow sometime to let the exhalation valve close

	if (timeCounter >= SOMETIME)
	{
//		flowReading	= (ExhFlowSensor->GetLastReading() *
//		               Btps::S_GetBtpsExhFactor() ) / SCALE;

		// integrate flow to get volume

		if (flowReading > EXHFLOW_LIMIT)
			volume += ((flowReading * BD_CYCLE_TIME_IN_MS) / 1000);

		if (volume > VOLUME_LIMIT)
		{
			timeCounter = 0;
			volume = 0;
			ErrTask::S_GetInstance()->Send(eCbitExhValveOpenTestCode);
		}

	} 

} // CheckExhValveStuckOpen

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CheckExhValveStuckClosed()
//
//    Processing:
//		During the exhalation phase of a breath the exhalation valve is 
//		commanded so as to achieve and maintain the PEEP/CPAP or EPAP setting.  
//		During this phase, there may or may not be exhaled flow (e.g., no flow 
//		if the circuit is disconnected or the patient is not exhaling).  
//		Therefore the exhalation flow sensor cannot be used to determine the 
//		function of the exhalation valve.
//		If the exhalation valve is determined to be stuck closed, the ventilator 
//		shall enter the Exhalation Valve Stuck Closed Ventilator Inoperative 
//		State.
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
void ValveStuck::CheckExhValveStuckClosed(void)

{

// this functionality is performed by the motor error routines

} // CheckExhValveStuckClosed


/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CheckAirValveStuckOpen()
//
//    Processing:
//		During the exhalation phase the air valve is commanded to be either 
//		closed (when pressure triggering is active) or to base flow (when flow 
//		triggering is active).  Therefore, the expected total flow during 
//		exhalation is between zero (0) and 23 lpm.  If the air valve were to 
//		fail in the full open position, flows in excess of 50 lpm would be 
//		measured.
//
//		The test for an air valve stuck open shall apply only to the exhalation 
//		phase of a non-flow triggered breath.  The air valve shall be considered 
//		to be stuck in the open position if either of the following occur.
//
//		1.	If for a consecutive 60 ms, for measured air flow (BTPS compensated)
//			is greater 833 ml/sec
//
//			When an air valve stuck open condition is indicated, the ventilator 
//			activates the Air Source Inoperative Alarm.
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
void ValveStuck::CheckAirValveStuckOpen(void)

{
	static const LONG AIRFLOW_LIMIT = 833 * SCALE;		// 833 ml/sec SCALED
	static const UNSIGNED SOMETIME  = 60 / MS_PER_TICK;	// 60 ms
	static UNSIGNED timeCounter     = 0;
    E_TriggerType triggerType;
	LONG flowReading;									// ml/sec SCALED

	// if vent is not in exhalation phase reset counter and exit

    if ( (PhaseId != eExhPhase) && (PhaseId != eExhHoldPhase) )
	{
		timeCounter = 0;
		return;
	}

    MgrBdSetting* bdSettingPtr = MgrBdSetting::S_GetInstance();  

	//	check only non-flow triggered breaths
	    
    triggerType = (E_TriggerType) bdSettingPtr->GetCurrentValue(eTriggerType);    
    if (eFlowTrigger != triggerType)
    {
//		flowReading = (AirFlowSensor->GetLastReading() *
//		               Btps::S_GetBtpsAirFactor()) / SCALE;
		if (flowReading > AIRFLOW_LIMIT)
			timeCounter += BD_CYCLE_TIME_IN_MS;
		else
			timeCounter	= 0;

		if (timeCounter >= SOMETIME)
		{
			timeCounter = 0;
			ErrTask::S_GetInstance()->Send(eCbitAirValveOpenTestCode);
		}

    }
	else
		timeCounter	= 0;

} // CheckAirValveStuckOpen


/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CheckAirValveStuckClosed()
//
//    Processing:
//		The ventilator shall declare an air valve stuck closed error condition 
//		if either of the following conditions exist:
//
//		1.	For a consecutive 100 consecutive ms.
//	   
//   	current air flow valve step position command greater than or equal to
//   	nominal step position for 16.67 ml/sec (one (1) lpm) with the minimum 
//		blower valve inlet pressure and the measured air flow BTPS compensated
//		in ml/sec
//
//		When an air valve stuck closed condition is indicated, the ventilator 
//		activates the Air Source Inoperative Alarm.
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
//      NOMINAL_POSITION was derived at looking at the diagnostic screen for 
//		what the valve step position read at 1 LPM.  The value 333 looks fishy
//		I know but it just coincidental.
//
//    Requirements:  
//
/******************************************************************************/
void ValveStuck::CheckAirValveStuckClosed(void)

{
	static const USHORT NOMINAL_POSITION = 333;			// TBD 1 LPM
	static const LONG NOMINAL_AIRFLOW    = 333;			// 3.33 ml/s
	static const UNSIGNED SOMETIME = 100 / MS_PER_TICK;	// 100 ms
	static UNSIGNED timeCounter          = 0;			// ms 
	LONG flowReading;									// ml/sec SCALED
	USHORT desiredStepPosition;

	// get flow reading and Air Valve desired step position
	 
//	flowReading = (AirFlowSensor->GetLastReading() *
//	               Btps::S_GetBtpsAirFactor() ) / SCALE;
//	desiredStepPosition = AirValve->GetDesiredStepPosition();

	if ( (desiredStepPosition >= NOMINAL_POSITION) &&
	     (flowReading < NOMINAL_AIRFLOW) )
		timeCounter += BD_CYCLE_TIME_IN_MS;
	else
		timeCounter	= 0;

	if (timeCounter >= SOMETIME)
	{
		timeCounter = 0;
		ErrTask::S_GetInstance()->Send(eCbitAirValveClosedTestCode);
	}
	 
} // CheckAirValveStuckClosed()


/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CheckO2ValveStuckOpen()
//
//    Processing:
//		During the exhalation phase the oxygen valve is commanded to be either 
//		closed (when pressure triggering is active) or to base flow (when flow 
//		triggering is active).  Therefore, the expected total flow during 
//		exhalation is between zero (0) and 23 lpm.  If the oxygen valve were to 
//		fail in the full open position, flows in excess of 50 lpm would be 
//		measured.
//		The test for an oxygen valve stuck open shall apply only to the 
//		exhalation phase of a non-flow triggered breath.  The oxygen valve shall 
//		be considered to be stuck in the open position if either of the 
//		following occur.
//
//		1.	If for a consecutive 60 ms, measured oxygen flow BTPS compensated
//			> 833 ml/sec
//
//		If the oxygen flow valve is detected to be stuck open, the ventilator 
//		shall enter the oxygen flow valve stuck open ventilator inoperative 
//		state.
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
void ValveStuck::CheckO2ValveStuckOpen(void)

{
	static const LONG O2FLOW_LIMIT = 833 * SCALE;		// 833 ml/sec
	static const UNSIGNED SOMETIME = 60 / MS_PER_TICK;	// 60 ms
	static UNSIGNED timeCounter    = 0;
    E_TriggerType triggerType;
	LONG flowReading;									// ml/sec SCALED

	// if vent is not in exhalation phase reset counter and exit

    if ( (PhaseId != eExhPhase) && (PhaseId != eExhHoldPhase) )
	{
		timeCounter = 0;
		return;
	}

    MgrBdSetting* bdSettingPtr = MgrBdSetting::S_GetInstance();  

	//	check only non-flow triggered breaths
	    
    triggerType = (E_TriggerType) bdSettingPtr->GetCurrentValue(eTriggerType);    
    if (eFlowTrigger != triggerType)
    {
//		flowReading = (O2FlowSensor->GetLastReading() *
//					   Btps::S_GetBtpsO2Factor() ) / SCALE;
		if (flowReading > O2FLOW_LIMIT)
			timeCounter += BD_CYCLE_TIME_IN_MS;
		else
			timeCounter	= 0;

		if (timeCounter >= SOMETIME)
		{
			timeCounter = 0;
			ErrTask::S_GetInstance()->Send(eCbitO2ValveOpenTestCode);
		}

    }
	else
		timeCounter	= 0;

} // CheckO2ValveStuckOpen


/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CheckO2ValveStuckClosed()
//
//    Processing:
//		The ventilator shall declare an oxygen valve stuck closed error 
//		condition if either of the following conditions exist:
//		1.	For a consecutive 100 consecutive ms, the current oxygen flow valve 
//			step position command greater than or equal to nominal step position 
//			for 16.67 ml/sec (one (1) lpm)of oxygen	and the measured oxygen flow 
//			BTPS compensated, in ml/sec is greater than 3.33 LPM
//
//			When an oxygen valve stuck closed condition is indicated, the 
//			ventilator activates the Oxygen Valve Stuck Closed Alarm.
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
//      NOMINAL_POSITION was derived at looking at the diagnostic screen for 
//		what the valve step position read at 1 LPM.  The value 333 looks fishy
//		I know but it just coincidental.
//
//    Requirements:  
//
/******************************************************************************/
void ValveStuck::CheckO2ValveStuckClosed(void)

{
	static const USHORT NOMINAL_POSITION = 333;			// TBD 1 LPM
	static const LONG NOMINAL_O2FLOW     = 333;			// 3.33  ml/s
	static const UNSIGNED SOMETIME = 100 / MS_PER_TICK;	// 100 ms
	static UNSIGNED timeCounter          = 0;
	LONG flowReading;									// ml/sec SCALED
	USHORT step;

	// get flow reading and O2 valve step position

//	flowReading =( O2FlowSensor->GetLastReading() *
//				   Btps::S_GetBtpsO2Factor() ) / SCALE;
//	step = O2Valve->GetDesiredStepPosition();

	if ( (step >= NOMINAL_POSITION) && (flowReading < NOMINAL_O2FLOW) )
		timeCounter += BD_CYCLE_TIME_IN_MS;
	else
		timeCounter	= 0;

	if (timeCounter >= SOMETIME)
	{
		timeCounter = 0;
		ErrTask::S_GetInstance()->Send(eCbitO2ValveClosedTestCode);
	}

} // CheckO2ValveStuckClosed


/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CheckAllValves()
//
//    Processing: Checks all pneumatic valves for valves to be stuck.  This 
//			function is called every Breath Delivery application control cycle.
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
void ValveStuck::CheckAllValves(E_PhaseId phaseId)

{
	// save the breath phase
	PhaseId = phaseId;

	CheckO2ValveStuckOpen();
	CheckO2ValveStuckClosed();
	CheckAirValveStuckOpen();
	CheckAirValveStuckClosed();
	CheckExhValveStuckOpen();

} // CheckAllValves

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: S_GetInstance()
//
//    Processing: 
//      Since only 1 instance of this object should be created, any access
//      to the object comes via this operation.  If any other object wants 
//      access to the ValveStuck object, it invokes 
//      ValveStuck::S_GetInstance(), which returns a pointer to the 
//      ValveStuck object.
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
//      ValveStuck* - pointer to the object instantiated from this class
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
ValveStuck* ValveStuck::S_GetInstance(void)
{
    // If the object has not already been created, create it.

    if (NULL == S_Instance)    
    {
        S_Instance = new ValveStuck();
        ASSERTION(S_Instance != NULL);
    }
   
    return(S_Instance);

}   // end S_GetInstance()


