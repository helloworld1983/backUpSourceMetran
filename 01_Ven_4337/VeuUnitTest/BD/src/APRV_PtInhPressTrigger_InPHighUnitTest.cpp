#include "EnableTestCase.h"
#ifdef BD_TRIGGER_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"
#include "APRV_PtInhPressTrigger_InPHigh.h"
#include "DebugTrace.h"

using namespace ::testing;

#include "BreathMgrMockHeader.h"
#include "MgrBdSettingMockHeader.h"
#include "StepMotorMockHeader.h"
#include "PhaseMgrMockHeader.h"

APRV_PtInhPressTrigger_InPHigh::APRV_PtInhPressTrigger_InPHigh(void):Trigger(eAPRV_PtInhPressTrigger_InPHigh)
{
    IsPressureFixed = eFalse;
    InUnrestrictedPhase = eFalse;
    PressureLimit = -20;
    PressSens = 0.5;

    PrevFilteredPress = 0;

}   // end APRV_PtInhPressTrigger_InPHigh()
E_Bool APRV_PtInhPressTrigger_InPHigh::CheckCondition(void)
{
    if(MgrBdSettingGetFlowTriggOn())
    {
        ProxyStatus Status = ProxyFlowSensorgetLastStatus();

        if((Status==eCalibrated)||(Status==eResetAndRecovered))
        {
            return eFalse;
        }
    }

    E_Bool tmp = eFalse;
    LONG filteredPress;

    if (MgrBdSettingGetPressTriggOn())
    {

        filteredPress = (PrevFilteredPress >> 2) +
                (3*(MgrBreathDataGetMonitoredPressure()) >> 2);


        PrevFilteredPress = filteredPress;

        if (eTrue == InUnrestrictedPhase)
        {

            // If measured pressure is less than or equal to the trigger
            // sensitivity, the trigger has fired.

            // Get pointer to the MgrBdSetting object
            //            MgrBdSetting* mgrBdSettingPtr = MgrBdSetting::S_GetInstance();

            PressSens = mgrBdSettingPtrGetCurrentValue(ePressSens);

            // PressureLimit is the Pressure Sensitivity below Peep
            LONG BaselinePress = 0;

            BaselinePress = mgrBdSettingPtrGetCurrentValue(eAPRVHighInhPress);


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
            E_PhaseId currentPhase = BDPhaseMgrGetCurrentPhaseId();

            UNSIGNED timeInh = 0;
            E_Bool IsInUnrestrictedPhase = eFalse;
            if(currentPhase == eAPRVHighPhase)
            {
                LONG PHigh = BDMgrBdSettingGetCurrentValue(eAPRVHighInhPress);
                LONG pressMeasured = 0;
                BDRequestDevice(eActivePressureSensorGetLastReading);
                LONG tmp = abs(PHigh - pressMeasured);
                if(tmp < 50)
                {
                    IsInUnrestrictedPhase = eTrue;
                }
            }
            else if(currentPhase == eAPRVExhPhase_InPHigh)
            {
                //                timeInh = APRV_SupportedInhPhase_InPHigh::GetTimeElapsed();
                timeInh = 0;
                IsInUnrestrictedPhase = APRV_ExhPhase_InPHighS_IsInUnrestrictedPhase(timeInh);
            }

            if (IsInUnrestrictedPhase)
            {

                // Set the InUnrestrictedPhase to TRUE so that it is not
                // necessary to call S_IsInUnrestrictedPhase() every cycle
                InUnrestrictedPhase = eTrue;

                // If measured pressure is less than or equal to the trigger
                // sensitivity, the trigger has fired.
                //                MgrBdSetting* mgrBdSettingPtr = MgrBdSetting::S_GetInstance();

                PressSens = mgrBdSettingPtrGetCurrentValue(ePressSens);

                LONG BaselinePress = 0;

                BaselinePress = mgrBdSettingPtrGetCurrentValue(eAPRVHighInhPress);

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
E_Bool APRV_PtInhPressTrigger_InPHigh::PreCheckCondition(void)
{
    E_Bool rtnValue = eFalse;       // return value; indicates if trigger fired

    if ((eTrue == IsEnabled)&&(MgrBdSettingGetPressTriggOn()))
    {

        // See if the ventilator is in the unrestricted phase of exhalation
        // By not passing any parameters, the default minimum exhalation
        // time is used.
        if (APRV_PHighPhaseS_IsInUnrestrictedPhase() == eTrue)
        {

            // If measured pressure is less than or equal to the trigger
            // sensitivity, the trigger is ready to fire.
            if (MgrBreathDataGetMonitoredPressure() <= PressureLimit)
            {
                rtnValue = eTrue;
            }
        }
    }

    return(rtnValue);

}   // end PreCheckCondition()
void APRV_PtInhPressTrigger_InPHigh::EnableTrigger(void)
{
    // When the trigger is first enabled, the restricted phase of exhalation
    // is active.
    InUnrestrictedPhase = eFalse;

    // Initialize the previous filtered flow value
    PrevFilteredPress = 0;

    // Since the TriggerEnable() method is invoked with no pressure setting, the
    // operator set value for pressure sensitivity is being used and
    // any changes to the pressure sensitivity are phased in right away.
    // So, the pressure limit is not fixed.
    IsPressureFixed = eFalse;
    // Invoke Base Class Enable Method
    //    Trigger::EnableTrigger();

}   // end EnableTrigger()
void APRV_PtInhPressTrigger_InPHigh::EnableTrigger(UNSIGNED pressure)
{
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

void APRV_PtInhPressTrigger_InPHigh::PhaseInSettingChange(LONG value)
{
    PressSens = value;

    // If the trigger is enabled and the pressure limit is not fixed,
    // update the PressureLimit as Baseline Pressure - new pressure sensitivity
    if ((eTrue == IsEnabled) && (eFalse == IsPressureFixed))
    {

        LONG BaselinePress = 0;

        BaselinePress = BDMgrBdSettingGetCurrentValue(eAPRVHighInhPress);

        PressureLimit = (SHORT) (BaselinePress - value);

    }

}   // end PhaseInSettingChange()

class APRV_PtInhPressTrigger_InPHighUnitMock : public BDTestFixture
{
public:
    APRV_PtInhPressTrigger_InPHighUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~APRV_PtInhPressTrigger_InPHighUnitMock(void){}
    virtual void SetUp(void)
    {
        MyAPRV_PtInhPressTrigger_InPHigh = new APRV_PtInhPressTrigger_InPHigh();
    }
    virtual void TearDown(void)
    {
        delete MyAPRV_PtInhPressTrigger_InPHigh;
    }
    APRV_PtInhPressTrigger_InPHigh *MyAPRV_PtInhPressTrigger_InPHigh;
};

//test for APRV_PtInhPressTrigger_InPHigh::APRV_PtInhPressTrigger_InPHigh(void):Trigger(eAPRV_PtInhPressTrigger_InPHigh)
TEST_F(APRV_PtInhPressTrigger_InPHighUnitMock, APRV_PtInhPressTrigger_InPHigh)
{
    EXPECT_EQ(MyAPRV_PtInhPressTrigger_InPHigh->IsPressureFixed, eFalse);
    EXPECT_EQ(MyAPRV_PtInhPressTrigger_InPHigh->InUnrestrictedPhase, eFalse);
    EXPECT_EQ(MyAPRV_PtInhPressTrigger_InPHigh->PressureLimit, -20);
    EXPECT_EQ(MyAPRV_PtInhPressTrigger_InPHigh->PressSens, 0);
    EXPECT_EQ(MyAPRV_PtInhPressTrigger_InPHigh->PrevFilteredPress, 0);
}

//test for E_Bool APRV_PtInhPressTrigger_InPHigh::CheckCondition(void)
TEST_F(APRV_PtInhPressTrigger_InPHighUnitMock, CheckCondition_0)
{
    EXPECT_CALL(*_MgrBdSetting, MgrBdSettingGetFlowTriggOn()).Times(1).WillOnce(Return(true));

    EXPECT_CALL(*_StepMotor, ProxyFlowSensorgetLastStatus()).Times(1).WillOnce(Return(eCalibrated));

    EXPECT_EQ(MyAPRV_PtInhPressTrigger_InPHigh->CheckCondition(), eFalse);
}
TEST_F(APRV_PtInhPressTrigger_InPHighUnitMock, CheckCondition_1)
{
    EXPECT_CALL(*_MgrBdSetting, MgrBdSettingGetFlowTriggOn()).Times(1).WillOnce(Return(true));

    EXPECT_CALL(*_StepMotor, ProxyFlowSensorgetLastStatus()).Times(1).WillOnce(Return(eResetAndRecovered));

    EXPECT_EQ(MyAPRV_PtInhPressTrigger_InPHigh->CheckCondition(), eFalse);
}
TEST_F(APRV_PtInhPressTrigger_InPHighUnitMock, CheckCondition_2)
{
    EXPECT_CALL(*_MgrBdSetting, MgrBdSettingGetPressTriggOn()).Times(1).WillOnce(Return(eTrue));
    EXPECT_CALL(*_BreathMgr, MgrBreathDataGetMonitoredPressure()).Times(1).WillOnce(Return(4));
    MyAPRV_PtInhPressTrigger_InPHigh->PrevFilteredPress = 4;

    MyAPRV_PtInhPressTrigger_InPHigh->InUnrestrictedPhase = eTrue;

    EXPECT_CALL(*_MgrBdSetting, mgrBdSettingPtrGetCurrentValue(ePressSens)).Times(1).WillOnce(Return(1));
    EXPECT_CALL(*_MgrBdSetting, mgrBdSettingPtrGetCurrentValue(eAPRVHighInhPress)).Times(1).WillOnce(Return(100));


    EXPECT_EQ(MyAPRV_PtInhPressTrigger_InPHigh->CheckCondition(), eTrue);
}
TEST_F(APRV_PtInhPressTrigger_InPHighUnitMock, CheckCondition_3)
{
    EXPECT_CALL(*_MgrBdSetting, MgrBdSettingGetPressTriggOn()).Times(1).WillOnce(Return(eTrue));
    EXPECT_CALL(*_BreathMgr, MgrBreathDataGetMonitoredPressure()).Times(1).WillOnce(Return(4));
    MyAPRV_PtInhPressTrigger_InPHigh->PrevFilteredPress = 4;

    MyAPRV_PtInhPressTrigger_InPHigh->InUnrestrictedPhase = eFalse;


    EXPECT_EQ(MyAPRV_PtInhPressTrigger_InPHigh->CheckCondition(), eFalse);
}

//test for E_Bool APRV_PtInhPressTrigger_InPHigh::PreCheckCondition(void)
TEST_F(APRV_PtInhPressTrigger_InPHighUnitMock, PreCheckCondition)
{
    MyAPRV_PtInhPressTrigger_InPHigh->IsEnabled = eTrue;
    EXPECT_CALL(*_MgrBdSetting, MgrBdSettingGetPressTriggOn()).Times(1).WillOnce(Return(eTrue));

    EXPECT_CALL(*_PhaseMgrPtr, APRV_PHighPhaseS_IsInUnrestrictedPhase()).Times(1).WillOnce(Return(eTrue));
    MyAPRV_PtInhPressTrigger_InPHigh->PressureLimit = 100;
    EXPECT_CALL(*_BreathMgr, MgrBreathDataGetMonitoredPressure()).Times(1).WillOnce(Return(20));

    EXPECT_EQ(MyAPRV_PtInhPressTrigger_InPHigh->PreCheckCondition(), eTrue);
}

//test for void APRV_PtInhPressTrigger_InPHigh::EnableTrigger(void)
TEST_F(APRV_PtInhPressTrigger_InPHighUnitMock, EnableTrigger)
{
    MyAPRV_PtInhPressTrigger_InPHigh->EnableTrigger();

    EXPECT_EQ(MyAPRV_PtInhPressTrigger_InPHigh->InUnrestrictedPhase, eFalse);
    EXPECT_EQ(MyAPRV_PtInhPressTrigger_InPHigh->PrevFilteredPress, 0);
    EXPECT_EQ(MyAPRV_PtInhPressTrigger_InPHigh->IsPressureFixed, eFalse);
}

//test for void APRV_PtInhPressTrigger_InPHigh::EnableTrigger(UNSIGNED pressure)
TEST_F(APRV_PtInhPressTrigger_InPHighUnitMock, EnableTrigger_with)
{
    MyAPRV_PtInhPressTrigger_InPHigh->EnableTrigger(1);

    EXPECT_EQ(MyAPRV_PtInhPressTrigger_InPHigh->InUnrestrictedPhase, eFalse);
    EXPECT_EQ(MyAPRV_PtInhPressTrigger_InPHigh->IsPressureFixed, eTrue);
    EXPECT_EQ(MyAPRV_PtInhPressTrigger_InPHigh->PressureLimit, 1);
}

//test for void APRV_PtInhPressTrigger_InPHigh::PhaseInSettingChange(LONG value)
TEST_F(APRV_PtInhPressTrigger_InPHighUnitMock, PhaseInSettingChange)
{
    MyAPRV_PtInhPressTrigger_InPHigh->IsEnabled = eTrue;
    MyAPRV_PtInhPressTrigger_InPHigh->IsPressureFixed = eFalse;

    EXPECT_CALL(*_MgrBdSetting, BDMgrBdSettingGetCurrentValue(eAPRVHighInhPress)).Times(1).WillOnce(Return(20));
    MyAPRV_PtInhPressTrigger_InPHigh->PhaseInSettingChange(10);

    EXPECT_EQ(MyAPRV_PtInhPressTrigger_InPHigh->PressureLimit, 20 - 10);
}

#endif //end define BD_TRIGGER_UNIT_TEST
