/******************************************************************************/
//$COMMON.CPP$
//   File Name:  BatteryTask.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: 
//      This class contains the operation to perform the Battery test
//      This test is performed every BATTERY_CYCLE_TIME that manages the
//		internal battery charger, status LEDS, and alarms.
//
//		Open Issues:
//
//			- The Current & Voltage Battery Thresholds are not defined for the
//			  5 minute & 15 minute time left of the ventilator.
//
//   Interfaces:
//
//		-- LEDs --
//
//		The off screen Internal Battery in Use indicator.  The Internal Battery 
//		in Use indicator shall illuminate flashing yellow when the ventilator is 
//		operating off the internal battery as indicated by the following:
//
//		1.	The discrete signal for Operating on AC/Battery is a logic 0 
//			indicating that the ventilator is running under battery power.
//		2.	The discrete signal for Internal/External Battery is a logic 0 
//			indicating that the ventilator is running off of an internal battery.
//
//		Internal Battery Charging
//		The Internal Battery Charging Indicator is maintained by the power 
//		supply.  When the ventilator is on, operating off of AC power, and not 
//		in the ventilator inoperative state, the Internal Battery Charging 
//		indicator illuminates solid yellow when the Battery Voltage is less than 
//		TBD VDC and the battery current is less than TBD amps.  When the 
//		ventilator is off and the ventilator is connected to an AC source, the 
//		Internal Battery Charging Indicator will be illuminated solid yellow by 
//		circuitry within the power supply which determines the charging status 
//		of the internal battery.
//
//		Internal Battery Low
//		The ventilator shall have an off screen Internal Battery Low indicator.  
//		The Internal Battery Low indicator will illuminate solid red when all 
//		of the following occur:
//
//			1.	The Operating on AC/Battery discrete is a logic 0 indicating 
//				operation off of a battery.
//			2.	The Internal/External Battery discrete is a logic 0 indicating 
//				operation off or an internal battery.
//			3.	The battery voltage is less than tbd VDC and the battery current 
//				is less than tbd amps indicating that battery life is 
//				approximately 15 minutes.
//
//		The Internal Battery Low indicator will illuminate flashing red when all 
//		of the following occur:
//
//			1.	The Operating on AC/Battery discrete is a logic 0 indicating 
//				operation off of a battery.
//			2.	The Internal/External Battery discrete is a logic 0 indicating 
//				operation off or an internal battery.
//			3.	The battery voltage is less than tbd VDC and the battery current 
//				is less than tbd amps indicating that battery life is 
//				approximately five (5) minutes.
//
//		External Battery
//		The ventilator has an off screen External Battery Indicator.  The 
//		External Battery indicator shall illuminate solid yellow when the 
//		following occur:
//
//			1.	The discrete signal for Operating on AC/Battery is a logic 0 
//				indicating that the ventilator is running under battery power.
//			2.	The discrete signal for Internal/External Battery is a logic 1 
//				indicating that the ventilator is running off of an external 
//				battery.
//
//		-- Alarms --
//
//		Low Internal Battery Alarms
//		Low External Battery Alarms
//
//		-- Audible --
//
//		Information sound for Internal Battery in use
//
//		-- Discretes --
//
//		Power Source Indicator Bit 0, Battery In Use 1 = Ext 0 = Int
//		Power Source Indicator Bit 1, Battery In Use 0 = DC 1 = AC
//		Run on AC, 0 = AC 1 = DC
//		Run on External Battery , 0 = External 1 = Internal
//
//		-- Analog Sensors --
//
//		Battery Current
//		Battery Voltage
//
//   Restrictions:
//      One instance of this class.  Testing of batteries will only be performed
//		on power up due to the problem of getting system RESETs and not getting
//		a Power Fail Signal when running without batteries.  This will minimize 
//		the amount of time the system will be subjected to this limitation.
/******************************************************************************/
#include "BatteryTask.h"

#include "CommonTypes.h"
#include "DebugTrace.h"

//  Define Statics
BatteryTask* BatteryTask::S_Instance = NULL;

// TBDs Threshold values for alarms scaled by VOLTAGE_SCALE 
static const SHORT LOW_INT_BAT_CURRENT  = 0;
static const SHORT LOW_INT_BAT_VOLTAGE  = 0;
static const SHORT HI_INT_BAT_CURRENT   = 0;
static const SHORT HI_INT_BAT_VOLTAGE   = 0;
static const SHORT LOW_EXT_BAT_CURRENT  = 0;
static const SHORT LOW_EXT_BAT_VOLTAGE  = 0;


static const E_State INT_BAT_CHARGING = eOff;	// 0 (eOff) = int bat present 
												// and being charged 
												// 1 (eOn) = charged

// TBD
// SOMETIME must be larger than dataio scheduled interval													
static const UNSIGNED SOMETIME = 10 * TICKS_PER_SEC;			// 10 sec

// pointers to gui and audible objects
//static	GUITask *guiPtr = NULL;
//static	Audible *sndPtr	= NULL;

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: S_GetInstance()
//
//    Processing:
//      Since only 1 instance of this object should be created, any access
//      to the object comes via this operation.  If any other object wants
//      access to the BatteryTask object, it invokes S_GetInstance(),
//      which returns a pointer to the BatteryTask object.
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
//      BatteryTask* - pointer to the object instantiated from this class
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
inline BatteryTask* BatteryTask::S_GetInstance(void)
{
    // If the object has not already been created, create it.
    if (NULL == S_Instance)
    {
        S_Instance = new BatteryTask();
        ASSERTION(S_Instance != NULL);
    }

    return(S_Instance);

}   // end S_GetInstance()


/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: BatteryTask   
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
BatteryTask::BatteryTask()
//    Task (tasks[eBatteryTask].Name, tasks[eBatteryTask].StackSize,
//    	  tasks[eBatteryTask].Pri, tasks[eBatteryTask].TimeSlice,
//    	  tasks[eBatteryTask].PreEmption)
{ 

	Init();

}



/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Init()
//
//    Processing: Initialize local pointers to audible sounds and gui task 
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
void BatteryTask::Init(void)
{

//	sndPtr = Audible::S_GetInstance();
//	ASSERTION(sndPtr != NULL);
//
//	guiPtr = GUITask::S_GetInstance();
//	ASSERTION (guiPtr != NULL);

}




/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ConfigureIntBat()
//
//    Processing: configure power supply some known state (AC/EXT BAT);
//				  configure it to run off internal battery.
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
void BatteryTask::ConfigureIntBat(void)
{

	// enable the LED charging light
	// power supply provides the 
//	InternalBatteryLED->SetState(eCharging);

	// put power supply in a known state AC on
//	RunACPower->SetStateDirect(eOn);

	// wait for power supply to switch
//	Task::Sleep(SOMETIME);

	// configure to run on External battery to 
	// disable battery charger
//	RunExternalBattery->SetStateDirect(eOn);

	// wait for power supply to switch
//	Task::Sleep(SOMETIME);

	// if the internal battery is present it is being charged

	// configure to run on Internal battery  
//	RunExternalBattery->SetStateDirect(eOff);

	// wait for power supply to switch
//	Task::Sleep(SOMETIME);

	// switch to run off batteries
//	RunACPower->SetStateDirect(eOff);

	// wait for power supply to switch
//	Task::Sleep(SOMETIME);

}




/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ConfigRunExternalBattery()
//
//    Processing: Purpose of this routine is to configure the external/internal
//			battery command.  If no batteries are attached to the ventilator and 
//			system is configured to run on external battery, no NMI will be 
//			generated.    
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
void BatteryTask::ConfigRunExternalBattery(void)
{
	// 19 volts scaled by 1000
//	static const SHORT BAT_LOW_THRESHOLD = 19 * VOLTAGE_SCALE;

//	SHORT voltage;

	// get battery voltage
//	voltage = BatteryVoltage->GetCurrentReading();

	// switch to run off AC
//	RunACPower->SetStateDirect(eOn);

	// wait for power supply to switch
//	Task::Sleep(SOMETIME);
  
//	if ( (BatChargingStatus->GetState() != INT_BAT_CHARGING) &&
//		 (voltage < BAT_LOW_THRESHOLD) )
		// no batteries are attached to ventilator
//		RunExternalBattery->SetStateDirect(eOff);
//	else
		// the internal battery has failed or is charging
		// internal battery is charged
		// configure to run on external batteries
//		RunExternalBattery->SetStateDirect(eOn);

	// wait for power supply to switch
//	Task::Sleep(SOMETIME);

}




/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: MonitorBatteryStatus()
//
//    Processing: Every second monitor and 
//		manage the charging LED
//		manage internal/external battery alarms based on current and voltage
//		manage battery inuse sound and GUI display messages	
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
void BatteryTask::MonitorBatteryStatus(void)
{
	static const E_State DC_POWER_SOURCE = eOff;	// 0 = Bat, 1 = AC
	static const E_State INT_BATTERY     = eOff;	// 0 = int bat, 1 = ext bat
	static const E_Bool FOREVER = eTrue;

	SHORT current, voltage;

	while (FOREVER)
	{
//		if (PowerSource1->GetState() == DC_POWER_SOURCE)
//		{
//			// get battery voltage & current
//			current = BatteryCurrent->GetCurrentReading();
//			voltage = BatteryVoltage->GetCurrentReading();
//
//			if (PowerSource0->GetState() == INT_BATTERY)
//			{
//				if (Alarm[ALARM_IDX(eLowInternalBattery)]->GetIsActive() == eActive)
//					CheckHiAlarm(voltage, current);
//				else
//					CheckLowAlarm(voltage, current);
//
//				sndPtr->SetFeedback(eInformationalSound);
//				BuildGUIMsg(eInternalBatteryOn, eLow);
//			}
//			else // External battery is in use
//			{
//				sndPtr->SetFeedback(eNoSound);
//				if (InternalBatteryLED->GetState() != eOff)
//				{
//					BuildGUIMsg(eInternalBatteryOn, eInactive);
//					InternalBatteryLED->SetState(eOff);
//				}
//
//				CheckExtBat(voltage, current);
//			}
//		}
//		else	// AC on
//		{
//			ClearBatAlarms();
//
//			// enable the LED charging light
//			// power supply provides the
//			InternalBatteryLED->SetState(eCharging);
//		}
//
//	// wait awhile
//
//	Task::Sleep(tasks[eBatteryTask].SchedulingInterval);
//
	} // while (FOREVER)...


}

void
BatteryTask::Entry (void)
{
}
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Entry()
//
//    Processing: 
//      This operation is invoked and run every BATTERY_CYCLE_TIME.  Checks
//		charging status or when the power transitions from battery to
//		AC or from AC to battery.  
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
//void BatteryTask::Entry(void)
//{
	// sleep some time to let dataio process discrete inputs
	// SOMETIME must be larger than dataio scheduled interval
//	Task::Sleep(SOMETIME);

	// configure system to run off internal battery
//	ConfigureIntBat();

	// configure RunExternalBattery Signal
//	ConfigRunExternalBattery();

	// continously monitor battery status; never returns
//	MonitorBatteryStatus();
    
//}   // Entry()


/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name:  BuildGUIMsg 
//
//    Processing:  Builds the GUI a basic gui event based on the id passed
//
//    Input Parameters: id
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
//void BatteryTask::BuildGUIMsg(E_GUIEventID id, E_AlarmLevel stat)
//{
//	AlarmStatusGUIEvent Msg;
//
//    Msg.ID = id;
//	Msg.Status = stat;
//	guiPtr->Send(Msg, NU_SUSPEND);

//}





/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CheckHiAlarm  
//
//    Processing:
//		Check threshold value for current and voltage
//		Manages the Low Int Battery LED and Alarm
//
//		TBD voltage and current will probably be used in a table to depict 
//
//    Input Parameters: SHORT current and voltage
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
//void BatteryTask::CheckHiAlarm(SHORT current, SHORT voltage)
//{
//
//	if ( (current < HI_INT_BAT_CURRENT) || (voltage < HI_INT_BAT_VOLTAGE) )
//	{
//		InternalBatteryLED->SetState(eVeryLowChg);
//		Alarm[ALARM_IDX(eLowInternalBattery)]->SetIsActive(eActive);
//	}
//	else
//	{
//		if ( !( (current < LOW_INT_BAT_CURRENT) ||
//			    (voltage < LOW_INT_BAT_VOLTAGE) ) )
//		{
//			InternalBatteryLED->SetState(eInUse);
//			Alarm[ALARM_IDX(eLowInternalBattery)]->SetIsActive(eNotActive);
//		}
//	}
//
//}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CheckLowAlarm  
//
//    Processing:
//		Check threshold value for current and voltage
//		Manages the Low Int Battery LED and Alarm
//
//		TBD voltage and current will probably be used in a table to depict 
//
//    Input Parameters: SHORT current and voltage
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
//void BatteryTask::CheckLowAlarm(SHORT current, SHORT voltage)
//{
//
//	if ( (current < LOW_INT_BAT_CURRENT) || (voltage < LOW_INT_BAT_VOLTAGE) )
//	{
//		InternalBatteryLED->SetState(eLowChg);
//		Alarm[ALARM_IDX(eLowInternalBattery)]->SetIsActive(eActive);
//	}
//	else
//	{
//		InternalBatteryLED->SetState(eInUse);
//		Alarm[ALARM_IDX(eLowInternalBattery)]->SetIsActive(eNotActive);
//	}
//
//}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CheckExtBat  
//
//    Processing:
//		Check threshold value for current and voltage
//		Manages the Low Ext Battery Alarm
//
//		TBD voltage and current will probably be used in a table to depict 
//
//    Input Parameters: SHORT current and voltage
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
//void BatteryTask::CheckExtBat(SHORT current, SHORT voltage)
//{
//
//	if ( (current < LOW_EXT_BAT_CURRENT) || (voltage < LOW_EXT_BAT_VOLTAGE) )
//		Alarm[ALARM_IDX(eLowExternalBattery)]->SetIsActive(eActive);
//	else
//		Alarm[ALARM_IDX(eLowExternalBattery)]->SetIsActive(eNotActive);
//
//}


/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ClearBatAlarms  
//
//    Processing:
//		Clears the Internal Battery LEDs and
//		internal and external battery alarms
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
//void BatteryTask::ClearBatAlarms(void)
//{
//	// clear informational sound
//
//	sndPtr->SetFeedback(eNoSound);
//
//	// clear Internal Battery LEDs and alarms
//
//	if (InternalBatteryLED->GetState() != eOff)
//	{
//		InternalBatteryLED->SetState(eOff);
//		BuildGUIMsg(eInternalBatteryOn, eInactive);
//	}
//
//	Alarm[ALARM_IDX(eLowInternalBattery)]->SetIsActive(eNotActive);
//	Alarm[ALARM_IDX(eLowExternalBattery)]->SetIsActive(eNotActive);
//
//}

