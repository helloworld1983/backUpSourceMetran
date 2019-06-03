#include "EnableTestCase.h"
#ifdef BD_TRIGGER_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"
#include "APRV_PtInhPressTrigger_InPLow.h"
#include "DebugTrace.h"

using namespace ::testing;

#include "BreathMgrMockHeader.h"
#include "MgrBdSettingMockHeader.h"
#include "StepMotorMockHeader.h"
#include "PhaseMgrMockHeader.h"

APRV_PtInhPressTrigger_InPLow::APRV_PtInhPressTrigger_InPLow(void):Trigger(eAPRV_PtInhPressTrigger_InPLow)
{
    IsPressureFixed = eFalse;
    InUnrestrictedPhase = eFalse;
    PressureLimit = -20;
    PressSens = 0.5;
    PrevFilteredPress_PHigh = 0;
    PrevFilteredPress_PLow = 0;

}   // end APRV_PtInhPressTrigger_InPLow()

E_Bool APRV_PtInhPressTrigger_InPLow::CheckCondition(void)
{
    if(MgrBdSettingGetFlowTriggOn())
    {
        ProxyStatus status;
        status = ProxyFlowSensorgetLastStatus();
        if((status == eCalibrated)||(status == eResetAndRecovered))
        {
            return eFalse;
        }

    }
    InUnrestrictedPhase = eTrue;
    E_Bool tmp = eFalse;
    LONG filteredPress;

    if (MgrBdSettingGetPressTriggOn())
    {

        filteredPress = (PrevFilteredPress_PLow >> 2) +
                (3*(MgrBreathDataGetMonitoredPressure()) >> 2);

        PrevFilteredPress_PLow = filteredPress;

        if (eTrue == InUnrestrictedPhase)
        {
            // If measured pressure is less than or equal to the trigger
            // sensitivity, the trigger has fired.

            // Get pointer to the MgrBdSetting object

//            MgrBdSetting* mgrBdSettingPtr = MgrBdSetting::S_GetInstance();

            PressSens = mgrBdSettingPtrGetCurrentValue(ePressSens);

            // PressureLimit is the Pressure Sensitivity below Peep
            LONG BaselinePress = 0;

            BaselinePress = mgrBdSettingPtrGetCurrentValue(eAPRVLowInhPress);


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
            E_PhaseId currentPhase = phaseMgrPtrGetCurrentPhaseId();


            //MgrBreathData::S_GetInstance()->GetBdBreathDataPtr()->GetTimeInInh();

            if(currentPhase == eAPRVLowPhase)
            {
                timeInh =  500;
                IsInUnrestrictedPhase = APRV_PLowPhaseS_IsInUnrestrictedPhase(timeInh);
            }
            else if(currentPhase == eAPRVExhPhase_InPLow)
            {
                timeInh = APRV_SupportedInhPhase_InPLowGetTimeElapsed();
                IsInUnrestrictedPhase = APRV_ExhPhase_InPLowS_IsInUnrestrictedPhase(timeInh);
            }

            if (IsInUnrestrictedPhase == eTrue)
            {
                // Set the InUnrestrictedPhase to TRUE so that it is not
                // necessary to call S_IsInUnrestrictedPhase() every cycle
                InUnrestrictedPhase = eTrue;

                // If measured pressure is less than or equal to the trigger
                // sensitivity, the trigger has fired.
//                MgrBdSetting* mgrBdSettingPtr = MgrBdSetting::S_GetInstance();

                PressSens = mgrBdSettingPtrGetCurrentValue(ePressSens);

                LONG BaselinePress = 0;

                BaselinePress = mgrBdSettingPtrGetCurrentValue(eAPRVLowInhPress);

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
E_Bool APRV_PtInhPressTrigger_InPLow::PreCheckCondition(void)
{
    E_Bool rtnValue = eFalse;       // return value; indicates if trigger fired

    if ((eTrue == IsEnabled)&&(MgrBdSettingGetPressTriggOn()))
    {

        // See if the ventilator is in the unrestricted phase of exhalation
        // By not passing any parameters, the default minimum exhalation
        // time is used.
        if (APRV_PLowPhaseS_IsInUnrestrictedPhase() == eTrue)
        {

            // If measured pressure is less than or equal to the trigger
            // sensitivity, the trigger is ready to fire.
            if (MgrBreathDataGetMonitoredPressure()
                    <= PressureLimit)
            {
                rtnValue = eTrue;
            }
        }
    }

    return(rtnValue);


}   // end PreCheckCondition()
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
//    Trigger::EnableTrigger();

}   // end EnableTrigger()

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
//    Trigger::EnableTrigger();

}   // end EnableTrigger()
void APRV_PtInhPressTrigger_InPLow::PhaseInSettingChange(LONG value)
{
    PressSens = value;

    // If the trigger is enabled and the pressure limit is not fixed,
    // update the PressureLimit as Baseline Pressure - new pressure sensitivity
    if ((eTrue == IsEnabled) && (eFalse == IsPressureFixed))
    {

        LONG BaselinePress = 0;

        BaselinePress = BDMgrBdSettingGetCurrentValue(eAPRVLowInhPress);

        PressureLimit = (SHORT) (BaselinePress - value);

    }

}   // end PhaseInSettingChange()

class APRV_PtInhPressTrigger_InPLowUnitMock : public BDTestFixture
{
public:
    APRV_PtInhPressTrigger_InPLowUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~APRV_PtInhPressTrigger_InPLowUnitMock(void){}
    virtual void SetUp(void)
    {
        MyAPRV_PtInhPressTrigger_InPLow = new APRV_PtInhPressTrigger_InPLow();
    }
    virtual void TearDown(void)
    {
        delete MyAPRV_PtInhPressTrigger_InPLow;
    }
    APRV_PtInhPressTrigger_InPLow *MyAPRV_PtInhPressTrigger_InPLow;
};

//test for APRV_PtInhPressTrigger_InPHigh::APRV_PtInhPressTrigger_InPHigh(void):Trigger(eAPRV_PtInhPressTrigger_InPHigh)
TEST_F(APRV_PtInhPressTrigger_InPLowUnitMock, APRV_PtInhPressTrigger_InPHigh)
{
    EXPECT_EQ(MyAPRV_PtInhPressTrigger_InPLow->IsPressureFixed, eFalse);
    EXPECT_EQ(MyAPRV_PtInhPressTrigger_InPLow->InUnrestrictedPhase, eFalse);
    EXPECT_EQ(MyAPRV_PtInhPressTrigger_InPLow->PressureLimit, -20);
    EXPECT_EQ(MyAPRV_PtInhPressTrigger_InPLow->PressSens, 0);
    EXPECT_EQ(MyAPRV_PtInhPressTrigger_InPLow->PrevFilteredPress_PHigh, 0);
    EXPECT_EQ(MyAPRV_PtInhPressTrigger_InPLow->PrevFilteredPress_PLow, 0);
}

//test for E_Bool APRV_PtInhPressTrigger_InPLow::CheckCondition(void)
TEST_F(APRV_PtInhPressTrigger_InPLowUnitMock, CheckCondition_0)
{
    EXPECT_CALL(*_MgrBdSetting, MgrBdSettingGetFlowTriggOn()).Times(1).WillOnce(Return(true));

    EXPECT_CALL(*_StepMotor, ProxyFlowSensorgetLastStatus()).Times(1).WillOnce(Return(eCalibrated));

    EXPECT_EQ(MyAPRV_PtInhPressTrigger_InPLow->CheckCondition(), eFalse);
}
TEST_F(APRV_PtInhPressTrigger_InPLowUnitMock, CheckCondition_1)
{
    EXPECT_CALL(*_MgrBdSetting, MgrBdSettingGetFlowTriggOn()).Times(1).WillOnce(Return(true));

    EXPECT_CALL(*_StepMotor, ProxyFlowSensorgetLastStatus()).Times(1).WillOnce(Return(eResetAndRecovered));

    EXPECT_EQ(MyAPRV_PtInhPressTrigger_InPLow->CheckCondition(), eFalse);
}

TEST_F(APRV_PtInhPressTrigger_InPLowUnitMock, CheckCondition_2)
{
    EXPECT_CALL(*_MgrBdSetting, MgrBdSettingGetFlowTriggOn()).Times(1).WillOnce(Return(false));
    EXPECT_CALL(*_MgrBdSetting, MgrBdSettingGetPressTriggOn()).Times(1).WillOnce(Return(eTrue));
    MyAPRV_PtInhPressTrigger_InPLow->PrevFilteredPress_PLow = 4;
    EXPECT_CALL(*_BreathMgr, MgrBreathDataGetMonitoredPressure()).Times(1).WillOnce(Return(4));

    MyAPRV_PtInhPressTrigger_InPLow->InUnrestrictedPhase = eTrue;

    EXPECT_CALL(*_MgrBdSetting, mgrBdSettingPtrGetCurrentValue(ePressSens)).Times(1).WillOnce(Return(1));
    EXPECT_CALL(*_MgrBdSetting, mgrBdSettingPtrGetCurrentValue(eAPRVLowInhPress)).Times(1).WillOnce(Return(100));


    EXPECT_EQ(MyAPRV_PtInhPressTrigger_InPLow->CheckCondition(), eTrue);
}

TEST_F(APRV_PtInhPressTrigger_InPLowUnitMock, CheckCondition_3)
{
    EXPECT_CALL(*_MgrBdSetting, MgrBdSettingGetFlowTriggOn()).Times(1).WillOnce(Return(false));
    EXPECT_CALL(*_MgrBdSetting, MgrBdSettingGetPressTriggOn()).Times(1).WillOnce(Return(eTrue));
    MyAPRV_PtInhPressTrigger_InPLow->PrevFilteredPress_PLow = 4;
    EXPECT_CALL(*_BreathMgr, MgrBreathDataGetMonitoredPressure()).Times(1).WillOnce(Return(4));

    MyAPRV_PtInhPressTrigger_InPLow->InUnrestrictedPhase = eFalse;


    EXPECT_EQ(MyAPRV_PtInhPressTrigger_InPLow->CheckCondition(), eFalse);
}

//test for E_Bool APRV_PtInhPressTrigger_InPLow::PreCheckCondition(void)
TEST_F(APRV_PtInhPressTrigger_InPLowUnitMock, PreCheckCondition)
{
    MyAPRV_PtInhPressTrigger_InPLow->IsEnabled = eTrue;
    EXPECT_CALL(*_MgrBdSetting, MgrBdSettingGetPressTriggOn()).Times(1).WillOnce(Return(eTrue));

    EXPECT_CALL(*_PhaseMgrPtr, APRV_PLowPhaseS_IsInUnrestrictedPhase()).Times(1).WillOnce(Return(eTrue));
    MyAPRV_PtInhPressTrigger_InPLow->PressureLimit = 100;
    EXPECT_CALL(*_BreathMgr, MgrBreathDataGetMonitoredPressure()).Times(1).WillOnce(Return(20));

    EXPECT_EQ(MyAPRV_PtInhPressTrigger_InPLow->PreCheckCondition(), eTrue);
}

//test for void APRV_PtInhPressTrigger_InPLow::EnableTrigger(void)
TEST_F(APRV_PtInhPressTrigger_InPLowUnitMock, EnableTrigger)
{
    MyAPRV_PtInhPressTrigger_InPLow->EnableTrigger();

    EXPECT_EQ(MyAPRV_PtInhPressTrigger_InPLow->InUnrestrictedPhase, eFalse);
    EXPECT_EQ(MyAPRV_PtInhPressTrigger_InPLow->PrevFilteredPress_PHigh, 0);
    EXPECT_EQ(MyAPRV_PtInhPressTrigger_InPLow->PrevFilteredPress_PLow, 0);
    EXPECT_EQ(MyAPRV_PtInhPressTrigger_InPLow->IsPressureFixed, eFalse);
}

//test for void APRV_PtInhPressTrigger_InPLow::EnableTrigger(UNSIGNED pressure)
TEST_F(APRV_PtInhPressTrigger_InPLowUnitMock, EnableTrigger_with)
{
    MyAPRV_PtInhPressTrigger_InPLow->EnableTrigger(1);

    EXPECT_EQ(MyAPRV_PtInhPressTrigger_InPLow->InUnrestrictedPhase, eFalse);
    EXPECT_EQ(MyAPRV_PtInhPressTrigger_InPLow->IsPressureFixed, eTrue);
    EXPECT_EQ(MyAPRV_PtInhPressTrigger_InPLow->PressureLimit, 1);
}

//test for void APRV_PtInhPressTrigger_InPLow::PhaseInSettingChange(LONG value)
TEST_F(APRV_PtInhPressTrigger_InPLowUnitMock, PhaseInSettingChange)
{
    MyAPRV_PtInhPressTrigger_InPLow->IsEnabled = eTrue;
    MyAPRV_PtInhPressTrigger_InPLow->IsPressureFixed = eFalse;

    EXPECT_CALL(*_MgrBdSetting, BDMgrBdSettingGetCurrentValue(eAPRVLowInhPress)).Times(1).WillOnce(Return(20));
    MyAPRV_PtInhPressTrigger_InPLow->PhaseInSettingChange(10);

    EXPECT_EQ(MyAPRV_PtInhPressTrigger_InPLow->PressureLimit, 20 - 10);
}

#endif //end define BD_TRIGGER_UNIT_TEST
