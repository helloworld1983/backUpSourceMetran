/******************************************************************************/
//$COMMON.CPP$
//   File Name:  O2Setting.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: 
//      This file contains the O2Setting class.  The O2Setting is
//      derived from the BdSetting base class.  In addition to providing
//      the services of a BdSetting, the O2Setting keeps track of the
//      status of the 100% O2 feature.  The CalculateAirO2Flows method
//      also calculates the split of flow between the air and oxygen flow
//      sensors based upon the status of the 100% O2 feature, the operator
//      setting for %O2 and the status of the air and oxygen supply alarms.
//
//      This class also contains a private operation that updates the %O2
//      Alarm Limits based on the %O2 setting and the 100% O2 request.
//
//   Interfaces:
//      Whenever the status of the 100% O2 feature changes, ChangeAllO2Status() 
//      invokes MgrBdSetting::AddSettingToChangeList().     
//
//      If an air AND O2 source fault is registered, the TriggerMgr object is
//      notified of a total gas loss.
//
//   Restrictions:
//      There is only 1 instance of the O2Setting class.
/******************************************************************************/
#include "MgrBdSetting.h"
#include "TriggerMgr.h"
#include "O2Setting.h"
#include "NoGasMode.h"
#include "Btps.h"
#include "AlarmTask.h"
#include "DebugTrace.h"

// The O2 Alarm Limits are based on the %O2 setting +/- 6%.  This value defines
// 6%, scaled.
static const SHORT O2_ALARM_LIMIT_OFFSET = 6 * (SHORT) SCALE;

// The minimum value that the Low O2 Alarm Limit can be set to is 18%.  This
// value defines 18%, scaled.
static const SHORT MIN_O2_ALARM_LIMIT = 18 * (SHORT) SCALE;

// The following constant defines 100%, scaled.
static const SHORT O2_PCT_100 = 100 * (SHORT) SCALE;

// The following LONG defines 21%, scaled.
static const LONG SCALED_21_PCT = 21 * SCALE;

static const ULONG SCALED_100_PCT = 100 * SCALE;
static const ULONG SCALED_79_PCT = 79 * SCALE;


// Define statics
O2Setting* O2Setting::S_Instance = NULL;

LONG O2Setting::DesiredAirRatio = 0;
LONG O2Setting::DesiredO2Ratio = 0;

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: S_GetInstance()
//
//    Processing:
//      Since only 1 instance of this object should be created, any access
//      to the object comes via this operation.  If any other object wants
//      access to the O2Setting object, it invokes O2Setting::S_GetInstance(),
//      which returns a pointer to the O2Setting object.
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
//      O2Setting* - pointer to the object instantiated from this class
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
O2Setting* O2Setting::S_GetInstance(void)
{
    // If the object has not already been created, create it.
    if (NULL == S_Instance)
    {
        S_Instance = new O2Setting(ePctO2);
        ASSERTION(S_Instance != NULL);
    }

    return(S_Instance);

}   // end S_GetInstance()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: PhaseInSetting()
//
//    Processing: 
//      This operation is invoked by the MgrBdSetting whenever setting changes
//      are phased in.  This method updates the CurrentValue attribute with
//      the PendingValue attribute.  This operation also updates the
//      AllO2Active current value with the pending value and the 
//      O2PressSwitchFault current value with the pending value.
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
void O2Setting::PhaseInSetting(void)
{
    if(CurrentValue.value != PendingValue.value)
    {
        //        PdAlarms::S_GetInstance()->ResetO2Time();
    }

    CurrentValue.value = PendingValue.value;
    CurrentValue.backup = PendingValue.backup;

    // Phase in 100% O2 Request if necessary
    AllO2Active.currentValue = AllO2Active.pendingValue;

    // Phase in change in O2 Pressure Supply Status change.
    O2PressSwitchFault.currentValue = O2PressSwitchFault.pendingValue;

    // Phase in change in Air Pressure Supply Status change.
    AirPressSwitchFault.currentValue = AirPressSwitchFault.pendingValue;

    // Update the exhaled flow BTPS correction factor based on %O2 setting, the
    // the status of the 100% O2, and the status of the air and oxygen supplies.
    if (eTrue == O2SourceFault())
    {
        O2Setting::DesiredAirRatio = 1 << 7;
        O2Setting::DesiredO2Ratio = 0;
    }

    else if (eTrue == AirSourceFault())
    {

        O2Setting::DesiredAirRatio = 0;
        O2Setting::DesiredO2Ratio = 1 << 7;
    }
    else if(eTrue == AllO2Active.currentValue)
    {
        LONG O2FlushValue = MgrBdSetting::S_GetInstance()->GetCurrentValue(eFlushPctO2);
        O2Setting::DesiredAirRatio =
                ((SCALED_100_PCT - O2FlushValue) * (1 << 7)) / SCALED_79_PCT;


        O2Setting::DesiredO2Ratio =
                (1 << 7) - O2Setting::DesiredAirRatio;
    }
    else
    {
        O2Setting::DesiredAirRatio =
                ((SCALED_100_PCT - CurrentValue.value) * (1 << 7)) / SCALED_79_PCT;


        O2Setting::DesiredO2Ratio =
                (1 << 7) - O2Setting::DesiredAirRatio;
    }

}   // end PhaseInSetting()




/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GetHighO2AlarmLimit()
//
//    Processing: 
//      This operation is invoked from the Patient Data Task thread of 
//      control to determine the High O2 Alarm Limit used in the alarm check.
//      If the 100% O2 feature is not active, the alarm limit is set to the
//      O2 setting + O2_ALARM_LIMIT_OFFSET.  If the 100% O2 feature is active,
//      the High O2 Alarm Limit is set to 100% + O2_ALARM_LIMIT_OFFSET.  
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      SHORT - high O2 alarm limit
//
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//      If AllO2Active contains an unexpected value, error handling is 
//      performed.
//
//    Requirements:  
//
/******************************************************************************/
SHORT O2Setting::GetHighO2AlarmLimit(void)
{
    SHORT AlarmLimit;       // calculated alarm limit

    // If the 100% O2 feature is not active, the alarm limit is
    // determined based on the current %O2 setting which is contained in
    // the CurrentValue attribute.  
    if (eFalse == AllO2Active.currentValue)
    {
        AlarmLimit = (SHORT) GetCurrentValue() + O2_ALARM_LIMIT_OFFSET;
    }

    // If the 100% O2 feature is active, the high limit is based on a
    // %O2 setting of 100%.
    else if (eTrue == AllO2Active.currentValue)
    {
        LONG O2FlushValue = MgrBdSetting::S_GetInstance()->GetCurrentValue(eFlushPctO2);

        AlarmLimit = O2FlushValue + O2_ALARM_LIMIT_OFFSET;
    }
    else
    {
        ASSERTION(eFalse);
    }

    return(AlarmLimit);

}   // end GetHighO2AlarmLimit()



/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GetLowO2AlarmLimit()
//
//    Processing: 
//      This operation is invoked from the Patient Data Task thread of 
//      control to determine the Low O2 Alarm Limit used in the alarm check.
//      If the 100% O2 feature is not active, the alarm limit is set to the
//      O2 setting - O2_ALARM_LIMIT_OFFSET.  If the 100% O2 feature is active,
//      the Low O2 Alarm Limit is set to 100% - O2_ALARM_LIMIT_OFFSET.   The
//      Low O2 Alarm Limit is limit checked to make sure that it is never less
//      than the MIN_O2_ALARM_LIMIT.
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      SHORT - low O2 alarm limit
//
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//      If AllO2Active contains an unexpected value, error handling is 
//      performed.
//
//    Requirements:  
//
/******************************************************************************/
SHORT O2Setting::GetLowO2AlarmLimit(void)
{
    SHORT AlarmLimit;       // calculated alarm limit

    // If the 100% O2 feature is not active, the alarm limit is
    // determined based on the current %O2 setting which is contained in
    // the CurrentValue attribute.  
    if (eFalse == AllO2Active.currentValue)
    {
        AlarmLimit = (SHORT) GetCurrentValue() - O2_ALARM_LIMIT_OFFSET;

        // The lower alarm limit is limited to the MIN_O2_ALARM_LIMIT.
        if (AlarmLimit < MIN_O2_ALARM_LIMIT)
        {
            AlarmLimit = MIN_O2_ALARM_LIMIT;
        }
    }

    // If the 100% O2 feature is active, update the alarm limit based on a
    // an %O2 setting of 100%.
    else if (eTrue == AllO2Active.currentValue)
    {
        LONG O2FlushValue = MgrBdSetting::S_GetInstance()->GetCurrentValue(eFlushPctO2);
        AlarmLimit = O2FlushValue - O2_ALARM_LIMIT_OFFSET;
    }

    else
    {
        // Illegal value for AllO2Active
        ASSERTION(eFalse);
    }

    return(AlarmLimit);

}   // end GetLowO2AlarmLimit()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ClearAirValveFault()
//
//    Processing:
//      This operation is invoked by user push on reset button.
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
void O2Setting::ClearAirValveFault(void)
{
    AirValveFault = eFalse;

    // Call SetAirPressureSwitchFault will do the trick.
    SetAirPressSwitchFault(eFalse);

}   // end ClearAirValveFault()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ClearO2ValveFault()
//
//    Processing: 
//      This operation is invoked by user push on reset button.
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
void O2Setting::ClearO2ValveFault(void)
{
    O2ValveFault = eFalse;

    // Call SetAirPressureSwitchFault will do the trick.
    SetO2PressSwitchFault(eFalse);

}   // end ClearO2ValveFault()


/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: SetAirPressSwitchFault()
//
//    Processing:
//      This operation updates the SetAirPressSwitchFault attribute based on the
//      passed value.  It also checks if an O2 Valve fault has occurred.  If
//      an O2 Valvev fault has occurred and this operation is being invoked
//      because the Air is source is gone, the TriggerMgr is notified since
//      a Total Gas Loss condition is now present.  If the O2PressSwitchFault
//      indicates that O2 has come back, O2ValveFault is checked to see if
//      another O2 fault exists.  If not, the Total Gas Loss condition is reset.
//
//      This operation is invoked through the DataIo thread of control.
//
//    Input Parameters:
//      isO2Gone = indicates if the O2 Pressure Switch senses a loss of O2 press
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
void O2Setting::SetAirPressSwitchFault(E_Bool isAirGone)
{

    // Always update the pending value.  If the change is phased in right away,
    // the current value will also be updated.
    AirPressSwitchFault.pendingValue = isAirGone;

    // Update the AirPressSwitchFault values.  This change is phased in
    // right away since the supply cannot be used if it's gone.
    AirPressSwitchFault.currentValue = isAirGone;

    if (eTrue == isAirGone)
    {
        if(eNotActive == AlarmTask::S_GetInstance()->GetIsActive(eAirSourceFault))
        {
            AlarmTask::S_GetInstance()->SetIsActive(eAirSourceFault, eActive);
        }

        // If the O2 source is also gone, it may be necessary to trigger a
        // total gas loss or a reset from a total gas loss.

        if (eFalse == O2SourceFault())
        {
            // Since the vent is now using all air, update the exhaled flow BTPS
            // correction factor based on 21%.
            Btps::S_ComputeBtpsExhFactor(SCALED_100_PCT);

            O2Setting::DesiredO2Ratio = 1 << 7;
            O2Setting::DesiredAirRatio = 0;

            // Set transition flag so flow controllers will be reset.
            SourceFaultTransition = eTrue;
        }
    }

    // Otherwise, the Air Pressure has come back.
    else if (eFalse == isAirGone)
    {
        if(eActive == AlarmTask::S_GetInstance()->GetIsActive(eAirSourceFault))
        {
            AlarmTask::S_GetInstance()->SetIsActive(eAirSourceFault, eNotActive);
        }

        // If the O2 source is gone, reset the total gas loss condition
        // since the Air supply just came back.
        if (eTrue == O2SourceFault())
        {
            // Since the vent is now using all Air, update the exhaled flow BTPS
            // correction factor based on 21%.
            Btps::S_ComputeBtpsExhFactor(SCALED_21_PCT);
            // Notify the TriggerMgr that a reset from the Total Gas Loss
            // condition can occur.
            (TriggerMgr::S_GetInstance())->
                    SetTriggerDidFire(eResetGasLossTrigger, eTrue);

            O2Setting::DesiredAirRatio = 1 << 7;
            O2Setting::DesiredO2Ratio = 0;

            // Set transition flag so flow controllers will be reset.
            SourceFaultTransition = eTrue;
        }
        else if (eFalse == O2SourceFault())
        {
            // Since the Air source is coming back, just add the O2Setting to the
            // setting change list so that the AirPressSwitchFault current
            // value is updated at the start of the next phase.  The return
            // of the Air supply is not phased in until the next breath phase.
            // This is done to avoid the situation where the O2 source is lost
            // then comes back within a single inhalation; it is not desirable
            // to switch to all air and then back again within the inhalation.
            // When the setting change is phased in, exhaled flow BTPS will be
            // updated.
            (MgrBdSetting::S_GetInstance())->AddSettingToChangeList(this);
        }
        else
        {
            // Illegal value for AirSourceFault
            ASSERTION(eFalse);
        }
    }
    else
    {
        // Illegal value for isO2Gone
        ASSERTION(eFalse);
    }

}   // end SetAirPressSwitchFault()



/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: SetO2PressSwitchFault()
//
//    Processing: 
//      This operation updates the O2PressSwitchFault attribute based on the
//      passed value.  It also checks if an air source fault has occurred.  If 
//      an air source fault has occurred and this operation is being invoked
//      because the O2 source is gone, the TriggerMgr is notified since 
//      a Total Gas Loss condition is now present.  If the O2PressSwitchFault
//      indicates that O2 has come back, O2ValveFault is checked to see if
//      another O2 fault exists.  If not, the Total Gas Loss condition is reset.
//
//      This operation is invoked through the DataIo thread of control.
//
//    Input Parameters:
//      isO2Gone = indicates if the O2 Pressure Switch senses a loss of O2 press
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
void O2Setting::SetO2PressSwitchFault(E_Bool isO2Gone)
{
    // Always update the pending value.  If the change is phased in right away,
    // the current value will also be updated.
    O2PressSwitchFault.pendingValue = isO2Gone;
    // Update the O2PressSwitchFault values.  This change is phased in
    // right away since the supply cannot be used if it's gone.
    O2PressSwitchFault.currentValue = isO2Gone;

    if (eTrue == isO2Gone)
    {
        {
            // Notify Alarms of the loss of the oxygen supply
            if(eNotActive == AlarmTask::S_GetInstance()->GetIsActive(eLowO2Supply))
            {
                AlarmTask::S_GetInstance()->SetIsActive(eLowO2Supply, eActive);
            }

        }
        if (eFalse == AirSourceFault())
        {
            // Since the vent is now using all air, update the exhaled flow BTPS
            // correction factor based on 21%.
            Btps::S_ComputeBtpsExhFactor(SCALED_21_PCT);

            O2Setting::DesiredAirRatio = 1 << 7;
            O2Setting::DesiredO2Ratio = 0;

            // Set transition flag so flow controllers will be reset.
            SourceFaultTransition = eTrue;
        }
    }

    // Otherwise, the O2 Pressure has come back.  
    else if (eFalse == isO2Gone)
    {

        // Notify Alarms of the return of the oxygen supply
        if(eActive == AlarmTask::S_GetInstance()->GetIsActive(eLowO2Supply))
        {
            AlarmTask::S_GetInstance()->SetIsActive(eLowO2Supply, eNotActive);
        }

        // If the air source is gone, reset the total gas loss condition 
        // since the oxygen supply just came back.
        if (eTrue == AirSourceFault())
        {


            // Since the vent is now using all O2, update the exhaled flow BTPS
            // correction factor based on 100%.
            Btps::S_ComputeBtpsExhFactor(O2_PCT_100);

            // Notify the TriggerMgr that a reset from the Total Gas Loss 
            // condition can occur.
            (TriggerMgr::S_GetInstance())->
                    SetTriggerDidFire(eResetGasLossTrigger, eTrue);


            O2Setting::DesiredAirRatio = 0;
            O2Setting::DesiredO2Ratio = 1 << 7;

            // Set transition flag so flow controllers will be reset.
            SourceFaultTransition = eTrue;
        }
        else if (eFalse == AirSourceFault())
        {
            // Since the O2 source is coming back, just add the O2Setting to the
            // setting change list so that the O2PressSwitchFault current
            // value is updated at the start of the next phase.  The return
            // of the O2 supply is not phased in until the next breath phase.
            // This is done to avoid the situation where the O2 source is lost
            // then comes back within a single inhalation; it is not desirable
            // to switch to all air and then back again within the inhalation.
            // When the setting change is phased in, exhaled flow BTPS will be
            // updated.
            (MgrBdSetting::S_GetInstance())->AddSettingToChangeList(this);
        }
        else
        {
            // Illegal value for AirSourceFault
            ASSERTION(eFalse);
        }
    }
    else
    {
        // Illegal value for isO2Gone
        ASSERTION(eFalse);
    }


}   // end SetO2PressSwitchFault()



/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: UpdateAirO2Alarms()
//
//    Processing: 
//      This operation is called from MgrBdSetting::ProcessNewPhase() to
//      update the status of air and O2 alarms based upon the %O2 setting,
//      and fault status.
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
void O2Setting::UpdateAirO2Alarms(void)
{

    // Notify Alarms of the status of the Total Gas Loss - SVO alarm based upon
    // the AirSourceFault and O2PressSwitchFault statuses.
    if ((eTrue == AirSourceFault()) && (eTrue == O2SourceFault()))
    {
        // If the air source is gone and the O2 Pressure Switch has detected
        // a loss of O2, notify the TriggerMgr of the Total Gas Loss 
        // Needed in case do not process the total gas loss when it occurred -
        // e.g. during Occlusion.
        (TriggerMgr::S_GetInstance())->
                SetTriggerDidFire(eTotalGasLossTrigger, eTrue);

    }

    // Update the O2 Supply pressure alarm based upon the status of the
    // O2PressSwitchFault attribute, the current operator setting for %O2
    // and the current status of the 100% O2 request
    if ((eTrue == O2PressSwitchFault.currentValue))
    {
        AlarmTask::S_GetInstance()->SetIsActive(eLowO2Supply, eActive);
    }
    else
    {
        AlarmTask::S_GetInstance()->SetIsActive(eLowO2Supply, eNotActive);
    }


    // Update the air pressure alarm based upon the status of the
    // AirPressSwitchFault attribute
    if (
            (eTrue == AirPressSwitchFault.currentValue))
    {
        AlarmTask::S_GetInstance()->SetIsActive(eAirSourceFault, eActive);
    }
    else
    {
        AlarmTask::S_GetInstance()->SetIsActive(eAirSourceFault, eNotActive);
    }
}   // end UpdateAirO2Alarms()



/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: O2Setting()
//
//    Processing: 
//      This method is the constructor for the O2Setting.  It initializes
//      private attributes and invokes the base class constructor.
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
O2Setting::O2Setting(E_BdSettingId anID) : BdSetting(anID)
{
    // Assume no supply faults at start-up.  
    O2ValveFault = eFalse;
    AirValveFault = eFalse;
    SourceFaultTransition = eFalse;

    O2PressSwitchFault.currentValue = eFalse;
    O2PressSwitchFault.pendingValue = eFalse;

    AirPressSwitchFault.currentValue = eFalse;
    AirPressSwitchFault.pendingValue = eFalse;

    // At initialization, 100% O2 request is not active.
    AllO2Active.currentValue = eFalse;
    AllO2Active.pendingValue = eFalse;

}   // end O2Setting()

void O2Setting::Set100PercentO2(E_Bool isActive)
{
//    PdAlarms::S_GetInstance()->ResetO2Time();
    if(isActive == eTrue)
    {
        AllO2Active.currentValue = eTrue;
        AllO2Active.pendingValue = eTrue;

    }
    else
    {
        AllO2Active.currentValue = eFalse;
        AllO2Active.pendingValue = eFalse;
    }
    MgrBdSetting::S_GetInstance()->AddSettingToChangeList(this);
}


/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CalculateAirO2Flows()
//
//    Processing:
//      Based on the setting for %O2, the status of the 100% O2 feature and
//      the status of the gas supply alarms, this operation determines the
//      desired air and O2 flows based on a desired total flow.
//
//    Input Parameters:
//      totalFlow - total flow command in ml/sec * SCALE
//
//    Output Parameters:
//      desiredAirFlow - pointer to the BTPS compensated flow command for the
//                          Air Flow Valve
//      desiredO2Flow - pointer to the BTPS compensated flow command for the
//                          O2 Flow Valve
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
void O2Setting::CalculateAirO2Flows
(
        LONG totalFlow,
        LONG* desiredAirFlow,
        LONG* desiredO2Flow
)
{
    ULONG tempAirFlow;           // temporary storage
    LONG btpsAirFactor;
    LONG btpsO2Factor;

    if (eTrue == AirSourceFault())
    {
        // If there is an air source fault, there is no air flow
        *desiredAirFlow = 0;

        // If there is no O2, there is no O2 flow; if there is O2, use 100% O2.
        if (eTrue == O2SourceFault())
        {
            *desiredO2Flow = 0;
        }
        else
        {
            *desiredO2Flow = totalFlow;
        }

    }

    // If there is an O2 source fault but no air fault, use all air.
    else if (eTrue == O2SourceFault())
    {
        *desiredAirFlow = totalFlow;
        *desiredO2Flow = 0;
    }

    // If 100% O2 is not active, calculate the desired air and O2 flow based
    // upon the operator setting for %O2 which is stored in the CurrentValue
    // attribute and is scaled.
    else if (eFalse == AllO2Active.currentValue)
    {
        // tempAirFlow is a ULONG to prevent over flow.  %O2, and total flow
        // are always positive numbers.
        tempAirFlow = ((SCALED_100_PCT - (ULONG) GetCurrentValue()) * (ULONG) totalFlow) /
                SCALED_79_PCT;
        *desiredAirFlow = (LONG) tempAirFlow;

        // Desired O2 makes up the balance
        *desiredO2Flow = totalFlow - *desiredAirFlow;
    }

    // If the 100% O2 request is active, use all oxygen
    else if (eTrue == AllO2Active.currentValue)
    {
        *desiredAirFlow = 0;
        *desiredO2Flow = totalFlow;
    }

    else
    {
        // Illegal possibility
        ASSERTION(eFalse);
    }

    // Retrieve and check the BTPS factors
    btpsAirFactor = Btps::S_GetBtpsAirFactor();
    btpsO2Factor = Btps::S_GetBtpsO2Factor();
    ASSERTION((btpsAirFactor != 0) && (btpsO2Factor != 0));

    // BTPS compensate the flows; the multiply by the scale is to remove the
    // scaling in the BTPS factors.
    *desiredAirFlow = (*desiredAirFlow * SCALE) / btpsAirFactor;
    *desiredO2Flow = (*desiredO2Flow * SCALE) / btpsO2Factor;

}   // end CalculateAirO2Flows()
