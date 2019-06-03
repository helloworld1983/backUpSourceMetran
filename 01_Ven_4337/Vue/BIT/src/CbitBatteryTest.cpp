/******************************************************************************/
//$COMMON.CPP$
//   File Name:  CbitBatteryTest.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: 
//      This class contains the Run() operation to perform the Battery test 
//      during CBIT.  This test is run every second.
//
//		Open Issues:
//
//			- The Current and Battery Constants are not defined
//			- The LEDs for the Battery do not work properly
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
//		The off screen Internal Battery Charging Indicator.  When the ventilator 
//		is on, operating off of AC power, and not in the ventilator inoperative 
//		state, the Internal Battery Charging indicator illuminates solid yellow 
//		when the Battery Voltage is less than TBD VDC and the battery current is 
//		less than TBD amps.  When the ventilator is off and the ventilator is 
//		connected to an AC source, the Internal Battery Charging Indicator will 
//		be illuminated solid yellow by circuitry within the power supply which 
//		determines the charging status of the internal battery.
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
//		Power Souce Indicator Bit 0, Battery In Use 1 = Int 0 = Ext
//		Power Souce Indicator Bit 1, Battery In Use 0 = DC 1 = AC
//
//		-- Analog Sensors --
//
//		Battery Current
//		Battery Voltage
//
//   Restrictions:
//      One instance of this class
/******************************************************************************/
#include "CbitBatteryTest.h"
#include "AlarmTask.h"

// TBDs
static const SHORT LOW_INT_BAT_CURRENT  = 0;
static const SHORT LOW_INT_BAT_VOLTAGE  = 0;
static const SHORT HI_INT_BAT_CURRENT   = 0;
static const SHORT HI_INT_BAT_VOLTAGE   = 0;
static const SHORT LOW_EXT_BAT_CURRENT  = 0;
static const SHORT LOW_EXT_BAT_VOLTAGE  = 0;
static const SHORT CHARGING_BAT_CURRENT = 0;
static const SHORT CHARGING_BAT_VOLTAGE = 0;

static const E_State DC_POWER_SOURCE = eOff;
static const E_State INT_BATTERY     = eOff;

// TBD
static const UNSIGNED SOMETIME = 20 * MS_PER_TICK;

// TBD
static const UNSIGNED ONEDAY = 1440 * TICKS_PER_MIN;
static const UNSIGNED HALFHOUR = 30 * TICKS_PER_MIN;

//static	GUITask *guiPtr = NULL;
//static	Audible *sndPtr	= NULL;


/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ExpirationRoutine  
//
//    Processing: Check Internal Battery charging thresholds
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
void CbitBatteryTest::ExpirationRoutine(void)

{

    RecheckCharging = eTrue;

}


/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CbitBatteryTest()
//
//    Processing: 
//      This operation is the constructor for the CbitBatteryTest.  It 
//      invokes the base class Test constructor.
//
//    Input Parameters:
//      eventCode - Id code, HandlePtr - Fault handler
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
CbitBatteryTest::CbitBatteryTest
(
        E_EventCode eventCode, 
        HandlePtr handlePtr
)
//    Test(eventCode, eCountBased, 1, handlePtr),
//	Timer
//        (
//         "Battery",      		// Name of timer.
//          ONEDAY, 				// Initial expiration time. 1 day
//          ONEDAY, 				// Reschedule expiration time. 1 day
//          FALSE               	// Clock is initially disabled.
//        )

{
    STATUS stat;

    //	sndPtr = Audible::S_GetInstance();
    //	ASSERTION(sndPtr != NULL);
    //
    //	guiPtr = GUITask::S_GetInstance();
    //	ASSERTION (guiPtr != NULL);
    //
    //	RecheckCharging = eTrue;
    //
    //	// start 1 day timer to check internal battery charging status
    //
    //	stat = Enable();
    //	ASSERTION(stat == NU_SUCCESS);

}   // end CbitBatteryTest()




/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Run()
//
//    Processing: 
//      This operation is invoked every second from the CBIT task.  Checks
//		charging status once a day or when the power transitions from battery to
//		AC or from AC to battery.  
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      E_TestStatus - indicates if the test passed.
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
E_TestStatus CbitBatteryTest::Run(void)
{

    SHORT current, voltage;

    //	if (PowerSource1->GetState() == DC_POWER_SOURCE)
    //	{
    //		RecheckCharging = eTrue;
    //		current = BatteryCurrent->GetCurrentReading();
    //		voltage = BatteryVoltage->GetCurrentReading();
    //
    //		if (PowerSource0->GetState() == INT_BATTERY)
    //		{
    //
    //			if (Alarm[ALARM_IDX(eLowInternalBattery)]->GetIsActive() == eActive)
    //				CheckHiAlarm(voltage, current);
    //			else
    //				CheckLowAlarm(voltage, current);
    //
    //			sndPtr->SetFeedback(eInformationalSound);
    //			BuildGUIMsg(eInternalBatteryOn, eLow);
    //
    //		}
    //		else // external battery in use
    //		{
    //			sndPtr->SetFeedback(eNoSound);
    //			if (InternalBatteryLED->GetState() != eOff)
    //			{
    //				BuildGUIMsg(eInternalBatteryOn, eInactive);
    //				InternalBatteryLED->SetState(eOff);
    //			}
    //			CheckExtBat(voltage, current);
    //		}
    //
    //	}
    //	else
    //	{
    //		ClearBatAlarms();
    //		if (RecheckCharging)
    //		{
    //			RecheckCharging = eFalse;
    //			CheckChargingThresholds();
    //		}
    //	}

    // return a passed status

    return(eCheckPassed);

}   // end Run()


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
void CbitBatteryTest::BuildGUIMsg(E_GUIEventID id, E_AlarmLevel stat)

{
    AlarmStatusGUIEvent Msg;

    Msg.ID = id;
    Msg.Status = stat;
    //	guiPtr->Send(Msg, NU_SUSPEND);

}



/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CheckChargingThresholds  
//
//    Processing:  
//			The ventilator has an off screen Internal Battery Charging 
//			Indicator.  When the ventilator is on, operating off of AC power, 
//			and not in the ventilator inoperative state, the Internal Battery 
//			Charging indicator shall illuminate solid yellow when the Battery 
//			Voltage is less than TBD VDC and the battery current is less than 
//			TBD amps.  When the ventilator is off and the ventilator is 
//			connected to an AC source, the Internal Battery Charging Indicator 
//			shall be illuminated solid yellow by circuitry within the power 
//			supply which determines the charging status of the internal battery.
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-conditions: None
//
//    Miscellaneous: Must switch to internal battery to read internal voltages 
//					 and currents.
//
//    Requirements: None	 
//
/******************************************************************************/
void CbitBatteryTest::CheckChargingThresholds()

{
    //	SHORT current, voltage;
    //	STATUS stat;
    //
    //	// configure to run on internal battery
    //	RunExternalBattery->SetStateDirect(eOn);
    //
    //	// switch to internal battery to get battery voltage and current
    //	RunACPower->SetStateDirect(eOn);
    //
    //	// wait for power supply to switch
    //	Task::Sleep(SOMETIME);
    //
    //	if ( (PowerSource1->GetState() == DC_POWER_SOURCE) &&
    //		 (PowerSource0->GetState() == INT_BATTERY) )
    //	{
    //        // read battery current and voltage readings
    //		current = BatteryCurrent->GetCurrentReading();
    //		voltage = BatteryVoltage->GetCurrentReading();
    //
    //		if ( (current < CHARGING_BAT_CURRENT) ||
    //		     (voltage < CHARGING_BAT_VOLTAGE) )
    //		{
    //			if (InternalBatteryLED->GetState() != eCharging)
    //			{
    //				InternalBatteryLED->SetState(eCharging);
    //				BuildGUIMsg(eIntBatteryCharging, eLow);
    //
    //				// change checking interval to every 30 minutes
    //
    //				// disable timer
    //				stat = Disable();
    //				ASSERTION(stat == NU_SUCCESS);
    //
    //				// reset timer
    //				stat = Reset(HALFHOUR, HALFHOUR, TRUE);
    //				ASSERTION(stat == NU_SUCCESS);
    //			}
    //		}
    //		else
    //		{
    //			if (InternalBatteryLED->GetState() == eCharging)
    //			{
    //				InternalBatteryLED->SetState(eOff);
    //				BuildGUIMsg(eIntBatteryCharging, eInactive);
    //
    //				// change checking interval to every day
    //
    //				// disable timer
    //				stat = Disable();
    //				ASSERTION(stat == NU_SUCCESS);
    //
    //				// reset timer
    //				stat = Reset(ONEDAY, ONEDAY, TRUE);
    //				ASSERTION(stat == NU_SUCCESS);
    //			}
    //		}
    //	}
    //
    //	// restore power supply to orignal state
    //	// switch to AC power
    //	RunACPower->SetStateDirect(eOff);
    //
    //	// configure to run on external battery
    //	RunExternalBattery->SetStateDirect(eOff);
    //
    //	// wait for power supply to switch
    //	Task::Sleep(SOMETIME);

} // CheckChargingThresholds()


/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CheckHiAlarm  
//
//    Processing:
//		Check threshold value for current and voltage
//		Manages the Low Int Battery LED and Alarm
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
void CbitBatteryTest::CheckHiAlarm(SHORT current, SHORT voltage)
{

    if ( (current < HI_INT_BAT_CURRENT) || (voltage < HI_INT_BAT_VOLTAGE) )
    {
        //		InternalBatteryLED->SetState(eVeryLowChg);
        AlarmTask::S_GetInstance()->SetIsActive(eLowInternalBattery, eActive);
    }
    else
    {
        if ( !( (current < LOW_INT_BAT_CURRENT) ||
                (voltage < LOW_INT_BAT_VOLTAGE) ) )
        {
            //			InternalBatteryLED->SetState(eInUse);
            AlarmTask::S_GetInstance()->SetIsActive(eLowInternalBattery, eNotActive);
        }
    }

}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CheckLowAlarm  
//
//    Processing:
//		Check threshold value for current and voltage
//		Manages the Low Int Battery LED and Alarm
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
void CbitBatteryTest::CheckLowAlarm(SHORT current, SHORT voltage)
{

    if ( (current < LOW_INT_BAT_CURRENT) || (voltage < LOW_INT_BAT_VOLTAGE) )
    {
        //		InternalBatteryLED->SetState(eLowChg);
        AlarmTask::S_GetInstance()->SetIsActive(eLowInternalBattery, eActive);
    }
    else
    {
        //		InternalBatteryLED->SetState(eInUse);
        AlarmTask::S_GetInstance()->SetIsActive(eLowInternalBattery, eNotActive);
    }

}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CheckExtBat  
//
//    Processing:
//		Check threshold value for current and voltage
//		Manages the Low Ext Battery Alarm
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
void CbitBatteryTest::CheckExtBat(SHORT current, SHORT voltage)
{

    if ( (current < LOW_EXT_BAT_CURRENT) || (voltage < LOW_EXT_BAT_VOLTAGE) )
    {
        AlarmTask::S_GetInstance()->SetIsActive(eLowExternalBattery, eActive);
    }
    else
    {
        AlarmTask::S_GetInstance()->SetIsActive(eLowExternalBattery, eNotActive);
    }

}


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
void CbitBatteryTest::ClearBatAlarms(void)
{
    // clear informational sound

//    sndPtr->SetFeedback(eNoSound);

    // clear Internal Battery LEDs and alarms

//    if (InternalBatteryLED->GetState() != eOff)
//    {
//        InternalBatteryLED->SetState(eOff);
//        BuildGUIMsg(eInternalBatteryOn, eInactive);
//    }
    AlarmTask::S_GetInstance()->SetIsActive(eLowInternalBattery, eNotActive);
    AlarmTask::S_GetInstance()->SetIsActive(eLowExternalBattery, eNotActive);

}


