#include "APRV_PtInhPressTrigger_InPLow.h"
#include "MgrBreathData.h"
#include "MgrBdSetting.h"
#include "APRV_PLowPhase.h"
#include "APRV_ExhPhase_InPLow.h"
#include "APRV_SupportedInhPhase_InPLow.h"
#include "ProxyFlowSensor.h"
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: APRV_PtInhPressTrigger_InPLow()
//
//    Processing:
//      This operation is the constructor for the Patient Inhalation Pressure
//      Trigger.  It invokes the base class Trigger constructor with the
//      Trigger ID.
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
APRV_PtInhPressTrigger_InPLow::APRV_PtInhPressTrigger_InPLow(void):Trigger(eAPRV_PtInhPressTrigger_InPLow)
{
    IsPressureFixed = eFalse;
    InUnrestrictedPhase = eFalse;
    PressureLimit = -20;
    PressSens = 0.5;
    PrevFilteredPress_PHigh = 0;
    PrevFilteredPress_PLow = 0;

}   // end APRV_PtInhPressTrigger_InPLow()


/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CheckCondition()
//
//    Processing:
//      This operation is invoked every BD task cycle while the Patient
//      Inhalation Pressure Trigger is enabled.  If the InUnrestrictedPhase
//      attribute is TRUE, indicating that it was already determined that the
//      ventilator is in the unrestricted phase of exhalation,
//      MgrBreathData::GetMonitoredPressure() is called to get the latest
//      pressure reading.  This operation then compares the value to the
//      PressureLimit attribute.  The PressureLimit attribute contains either
//      the operator set pressure sensitivity or a backup pressure sensitivity
//      used during flow triggering.  If the exhalation pressure is less than
//      or equal to the PressureLimit, the TriggerDidFire attribute is set
//      to TRUE and returned to the caller.
//
//      If the IsInUnrestrictedPhase attribute is FALSE,
//      ExhPhase::S_IsInUnrestrictedPhase() is called to determine if the
//      ventilator is in the unrestricted phase.  If so, IsInUnrestrictedPhase
//      is set TRUE so that this ExhPhase::S_IsInUnrestrictedPhase() does
//      not have to be called every BD cycle after it has already been
//      determined that the unrestricted part of exhalation is active. If
//      the unrestricted phase is active, MgrBreathData::GetMontoredPressure()
//      is called to get the latest pressure reading, which is
//      compared to the PressureLimit attribute.   If the exhalation pressure
//      is less than or equal to the PressureLimit, the TriggerDidFire
//      attribute is set to TRUE and returned to the caller.
//
//      If the unrestricted phase is not active or if the pressure condition
//      is not met, TriggerDidFire is not updated and its current value (FALSE)
//      is returned to the caller.
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      E_Bool - indicates if the trigger fired or not
//
//    Pre-Conditions:
//      This operation assumes that the TriggerMgr only invokes it while
//      the trigger is enabled.
//
//    Miscellaneous:
//      None
//
//    Requirements:
//
/******************************************************************************/
E_Bool APRV_PtInhPressTrigger_InPLow::CheckCondition(void)
{
    if(MgrBdSetting::S_GetInstance()->GetFlowTriggOn())
    {
        ProxyStatus status;
        status = ProxyFlowSensor::S_GetInstance()->getLastStatus();
        if((status == eCalibrated)||(status == eResetAndRecovered))
        {
            return eFalse;
        }

    }
    InUnrestrictedPhase = eTrue;
    E_Bool tmp = eFalse;
    LONG filteredPress;

    if (MgrBdSetting::S_GetInstance()->GetPressTriggOn())
    {

        filteredPress = (PrevFilteredPress_PLow >> 2) +
                (3*(MgrBreathData::S_GetInstance()->GetMonitoredPressure()) >> 2);

        PrevFilteredPress_PLow = filteredPress;

        if (eTrue == InUnrestrictedPhase)
        {
            // If measured pressure is less than or equal to the trigger
            // sensitivity, the trigger has fired.

            // Get pointer to the MgrBdSetting object

            MgrBdSetting* mgrBdSettingPtr = MgrBdSetting::S_GetInstance();

            PressSens = mgrBdSettingPtr->GetCurrentValue(ePressSens);

            // PressureLimit is the Pressure Sensitivity below Peep
            LONG BaselinePress = 0;

            BaselinePress = mgrBdSettingPtr->GetCurrentValue(eAPRVLowInhPress);


            PressureLimit = (SHORT) (BaselinePress - PressSens);


            if (filteredPress <= PressureLimit)
            {
                tmp = eTrue;
            }
        }

        else
        {
            // See if the ventilator is in the unrestricted phase of exhalation
            // By not passing any parameters, the default minimum exhalation
            // time is used.
            E_Bool IsInUnrestrictedPhase = eFalse;
            UNSIGNED timeInh = 0;
            E_PhaseId currentPhase = PhaseMgr::S_GetInstance()->GetCurrentPhaseId();


            //MgrBreathData::S_GetInstance()->GetBdBreathDataPtr()->GetTimeInInh();

            if(currentPhase == eAPRVLowPhase)
            {
                timeInh =  500;
                IsInUnrestrictedPhase = APRV_PLowPhase::S_IsInUnrestrictedPhase(timeInh);
            }
            else if(currentPhase == eAPRVExhPhase_InPLow)
            {
                timeInh = APRV_SupportedInhPhase_InPLow::GetTimeElapsed();
                IsInUnrestrictedPhase = APRV_ExhPhase_InPLow::S_IsInUnrestrictedPhase(timeInh);
            }

            if (IsInUnrestrictedPhase == eTrue)
            {
                // Set the InUnrestrictedPhase to TRUE so that it is not
                // necessary to call S_IsInUnrestrictedPhase() every cycle
                InUnrestrictedPhase = eTrue;

                // If measured pressure is less than or equal to the trigger
                // sensitivity, the trigger has fired.
                MgrBdSetting* mgrBdSettingPtr = MgrBdSetting::S_GetInstance();

                PressSens = mgrBdSettingPtr->GetCurrentValue(ePressSens);

                LONG BaselinePress = 0;

                BaselinePress = mgrBdSettingPtr->GetCurrentValue(eAPRVLowInhPress);

                PressureLimit = (SHORT) (BaselinePress - PressSens);



                if (filteredPress <= PressureLimit)
                {
                    tmp = eTrue;
                }
            }
        }

    }
    TriggerDidFire = tmp;

    return(TriggerDidFire);

}   // end CheckCondition()


/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: PreCheckCondition()
//
//    Processing:
//      This operation is invoked by the ModeChangeTrigger::CheckCondition()
//      operation when the mode change trigger is armed but is waiting
//      for the unrestricted phase of exhalation.   PreCheckCondition() looks
//      ahead to determine if the patient trigger is going to fire on the
//      current BD cycle.  The ModeChangeTrigger uses this information to
//      determine whether the mode change should be phased in early.
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      E_Bool - indicates if the trigger condition is met
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
E_Bool APRV_PtInhPressTrigger_InPLow::PreCheckCondition(void)
{
    E_Bool rtnValue = eFalse;       // return value; indicates if trigger fired

    if ((eTrue == IsEnabled)&&(MgrBdSetting::S_GetInstance()->GetPressTriggOn()))
    {

        // See if the ventilator is in the unrestricted phase of exhalation
        // By not passing any parameters, the default minimum exhalation
        // time is used.
        if (APRV_PLowPhase::S_IsInUnrestrictedPhase() == eTrue)
        {

            // If measured pressure is less than or equal to the trigger
            // sensitivity, the trigger is ready to fire.
            if ((MgrBreathData::S_GetInstance())->GetMonitoredPressure()
                    <= PressureLimit)
            {
                rtnValue = eTrue;
            }
        }
    }

    return(rtnValue);


}   // end PreCheckCondition()



/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: EnableTrigger()
//
//    Processing:
//      This operation is invoked to enable the APRV_PtInhPressTrigger_InPLow using the
//      operator set pressure sensitivity as the trigger sensitivity.  This
//      operation sets the private attributes IsInUnrestrictedPhase and
//      IsPressureFixed to FALSE.  It then invokes the GetPendingValue()
//      operation of MgrBdSetting to retrieve the operator settings for
//      pressure sensitivity and Baseline Pressure.  It updates the
//      PressureLimit attribute by subtracting pressure sensitivity from PEEP
//      and invokes the base class Trigger::EnableTrigger() method.
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
void APRV_PtInhPressTrigger_InPLow::EnableTrigger(void)
{
    // When the trigger is first enabled, the restricted phase of exhalation
    // is active.
    InUnrestrictedPhase = eFalse;

    // Initialize the previous filtered flow value
    PrevFilteredPress_PHigh = 0;
    PrevFilteredPress_PLow = 0;

    // Since the TriggerEnable() method is invoked with no pressure setting, the
    // operator set value for pressure sensitivity is being used and
    // any changes to the pressure sensitivity are phased in right away.
    // So, the pressure limit is not fixed.
    IsPressureFixed = eFalse;

    // Invoke Base Class Enable Method
    Trigger::EnableTrigger();

}   // end EnableTrigger()



/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: EnableTrigger()
//
//    Processing:
//      This operation is invoked to enable the APRV_PtInhPressTrigger_InPLow using a
//      backup pressure sensitivity as the trigger sensitivity.  This operation
//      is called to enable a backup triggering mechanism when Flow Triggering
//      is active.  This operation sets the private attribute
//      IsInUnrestrictedPhase to FALSE and IsPressureFixed to TRUE.  The
//      trigger sensitivity is fixed during flow triggering -- changes to
//      the pressure sensitvity do not affect the backup trigger sensitivity.
//
//      This operation then invokes MgrBdSetting::GetPendingValue()  to
//      retrieve the operator setting for the baseline pressure.  It updates
//      the PressureLimit attribute by subtracting the passed pressure argument
//      from the baseline pressure and invokes the base class
//      Trigger::EnableTrigger() method.
//
//
//    Input Parameters:
//      pressure - backup pressure limit in cmH2O * SF
//
//    Output Parameters:
//      None
//
//    Return Values:
//      None
//
//    Miscellaneous:
//
//    Requirements:
//
/******************************************************************************/
void APRV_PtInhPressTrigger_InPLow::EnableTrigger(UNSIGNED pressure)
{
    // Get pointer to the MgrBdSetting object

    // When the trigger is first enabled, the restricted phase of exhalation
    // is active.
    InUnrestrictedPhase = eFalse;

    // Since the Enable method is invoked with a specific pressure, then
    // any changes to the operator pressure setting are not phased in right
    // away
    IsPressureFixed = eTrue;

    // Determine the pressure limit based on the current Baseline pressure
    // reading and the passed pressure
    PressureLimit = (SHORT) pressure;

    // Invoke Base Class Enable Method
    Trigger::EnableTrigger();

}   // end EnableTrigger()



/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: PhaseInSettingChange()
//
//    Processing:
//      This operation is invoked by the Pressure Sensitvity setting object
//      whenever a pressure sensitivity change request is received.
//      If the trigger is enabled and the pressure limit is not fixed,
//      this operation updates the PressureLimit as baseline pressure - new
//      value.
//
//    Input Parameters:
//      value - new pressure sensitivity setting
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
void APRV_PtInhPressTrigger_InPLow::PhaseInSettingChange(LONG value)
{
    PressSens = value;

    // If the trigger is enabled and the pressure limit is not fixed,
    // update the PressureLimit as Baseline Pressure - new pressure sensitivity
    if ((eTrue == IsEnabled) && (eFalse == IsPressureFixed))
    {

        LONG BaselinePress = 0;

        BaselinePress = MgrBdSetting::S_GetInstance()->GetCurrentValue(eAPRVLowInhPress);

        PressureLimit = (SHORT) (BaselinePress - value);

    }

}   // end PhaseInSettingChange()



