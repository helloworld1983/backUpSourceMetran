#include "EnableTestCase.h"
#ifdef BD_TRIGGER_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"
#include "APRV_PtInhPressTrigger.h"
#include "DebugTrace.h"

using namespace ::testing;

#include "BreathMgrMockHeader.h"
#include "MgrBdSettingMockHeader.h"
#include "PhaseMgrMockHeader.h"

APRV_PtInhPressTrigger::APRV_PtInhPressTrigger():Trigger(eAPRV_PtInhPressTrigger)
{
    IsPressureFixed = eFalse;
    InUnrestrictedPhase = eFalse;
    PressureLimit = -20;
    PressSens = 0.5;
    PrevFilteredPress_PHigh = 0;
    PrevFilteredPress_PLow = 0;

}   // end APRV_PtInhPressTrigger()
E_Bool APRV_PtInhPressTrigger::CheckCondition(void)
{
//    if(eP_Low == mPressureLevel)
//    {
//        TriggerDidFire = APRV_PLow_CheckCondition();
//    }
//    else
//    {
//        TriggerDidFire = APRV_PHigh_CheckCondition();
//    }

    return(TriggerDidFire);

}   // end CheckCondition()
E_Bool APRV_PtInhPressTrigger::PreCheckCondition(void)
{
    E_Bool rtnValue = eFalse;       // return value; indicates if trigger fired

    if ((eTrue == IsEnabled)&&(MgrBdSettingGetPressTriggOn()))
    {

        // See if the ventilator is in the unrestricted phase of exhalation
        // By not passing any parameters, the default minimum exhalation
        // time is used.
        E_PhaseId currentPhase =  BDPhaseMgrGetCurrentPhaseId();
        if(currentPhase == eAPRV_PLow_Phase)
        {
            if (APRV_PLowPhaseS_IsInUnrestrictedPhase() == eTrue)
            {

                // If measured pressure is less than or equal to the trigger
                // sensitivity, the trigger is ready to fire.
                if (MgrBreathDataGetMonitoredPressure() <= PressureLimit)
                {
                    rtnValue = eTrue;
                }
            }
        }
        else
        {
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
    }

    return(rtnValue);

}   // end PreCheckCondition()

void APRV_PtInhPressTrigger::EnableTrigger(void)
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
void APRV_PtInhPressTrigger::EnableTrigger(UNSIGNED pressure)
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
void APRV_PtInhPressTrigger::PhaseInSettingChange(LONG value)
{
//    MgrBdSetting* mgrBdSettingPtr;   // pointer to the MgrBdSetting object

    PressSens = value;

    // If the trigger is enabled and the pressure limit is not fixed,
    // update the PressureLimit as Baseline Pressure - new pressure sensitivity
    if ((eTrue == IsEnabled) && (eFalse == IsPressureFixed))
    {
//        mgrBdSettingPtr = MgrBdSetting::S_GetInstance();


        LONG BaselinePress = 0;

        E_PhaseId currentPhase =  BDPhaseMgrGetCurrentPhaseId();
        if(currentPhase == eAPRV_PLow_Phase)
        {
            BaselinePress = mgrBdSettingPtrGetCurrentValue(eAPRVLowInhPress);
        }
        else
        {
            BaselinePress = mgrBdSettingPtrGetCurrentValue(eAPRVHighInhPress);
        }

        PressureLimit = (SHORT) (BaselinePress - value);


    }

}   // end PhaseInSettingChange()
E_Bool APRV_PtInhPressTrigger::APRV_PHigh_CheckCondition(void)
{
    InUnrestrictedPhase = eTrue;
    E_Bool tmp = eFalse;
    LONG filteredPress;

    if (MgrBdSettingS_GetInstanceGetPressTriggOn())
    {

        filteredPress = (PrevFilteredPress_PHigh >> 2) +
                (3*(MgrBreathDataGetMonitoredPressure()) >> 2);


        PrevFilteredPress_PHigh = filteredPress;

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
            UNSIGNED timeInh =  MgrBreathDataGetBdBreathDataPtrGetTimeInInh();

            if (APRV_PLowPhaseS_IsInUnrestrictedPhase(timeInh) == eTrue)
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
    return(tmp);
}
E_Bool APRV_PtInhPressTrigger::APRV_PLow_CheckCondition()
{
    InUnrestrictedPhase = eTrue;
    E_Bool tmp = eFalse;
    LONG filteredPress;

    if (MgrBdSettingS_GetInstanceGetPressTriggOn())
    {

        filteredPress = (PrevFilteredPress_PLow >> 2) +
                (3*(MgrBreathDataS_GetInstanceGetMonitoredPressure()) >> 2);


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
            UNSIGNED timeInh =  MgrBreathDataGetBdBreathDataPtrGetTimeInInh();

            if (APRV_PLowPhaseS_IsInUnrestrictedPhase(timeInh) == eTrue)
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
    return(tmp);
}

class APRV_PtInhPressTriggerUnitMock : public BDTestFixture
{
public:
    APRV_PtInhPressTriggerUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~APRV_PtInhPressTriggerUnitMock(void){}
    virtual void SetUp(void)
    {
        MyAPRV_PtInhPressTrigger = new APRV_PtInhPressTrigger();
    }
    virtual void TearDown(void)
    {
        delete MyAPRV_PtInhPressTrigger;
    }
    APRV_PtInhPressTrigger *MyAPRV_PtInhPressTrigger;
};

//test for APRV_PtInhPressTrigger::APRV_PtInhPressTrigger():Trigger(eAPRV_PtInhPressTrigger)
TEST_F(APRV_PtInhPressTriggerUnitMock, APRV_PtInhPressTrigger)
{
    EXPECT_EQ(MyAPRV_PtInhPressTrigger->IsPressureFixed, eFalse);
    EXPECT_EQ(MyAPRV_PtInhPressTrigger->InUnrestrictedPhase, eFalse);
    EXPECT_EQ(MyAPRV_PtInhPressTrigger->PressureLimit, -20);
    EXPECT_EQ(MyAPRV_PtInhPressTrigger->PressSens, 0);
    EXPECT_EQ(MyAPRV_PtInhPressTrigger->PrevFilteredPress_PHigh, 0);
    EXPECT_EQ(MyAPRV_PtInhPressTrigger->PrevFilteredPress_PLow, 0);
}

//test for E_Bool APRV_PtInhPressTrigger::CheckCondition(void)
TEST_F(APRV_PtInhPressTriggerUnitMock, CheckCondition)
{
    MyAPRV_PtInhPressTrigger->TriggerDidFire = eTrue;

    EXPECT_EQ(MyAPRV_PtInhPressTrigger->CheckCondition(), eTrue);
}

//test for E_Bool APRV_PtInhPressTrigger::PreCheckCondition(void)
TEST_F(APRV_PtInhPressTriggerUnitMock, PreCheckCondition_0)
{
    MyAPRV_PtInhPressTrigger->IsEnabled = eTrue;
    MyAPRV_PtInhPressTrigger->PressureLimit = 10;
    EXPECT_CALL(*_MgrBdSetting, MgrBdSettingGetPressTriggOn()).Times(1).WillOnce(Return(eTrue));
    EXPECT_CALL(*_PhaseMgrPtr, BDPhaseMgrGetCurrentPhaseId()).Times(1).WillOnce(Return(eAPRV_PLow_Phase));

    EXPECT_CALL(*_PhaseMgrPtr, APRV_PLowPhaseS_IsInUnrestrictedPhase()).Times(1).WillOnce(Return(eTrue));
    EXPECT_CALL(*_BreathMgr, MgrBreathDataGetMonitoredPressure()).Times(1).WillOnce(Return(1));

    EXPECT_EQ(MyAPRV_PtInhPressTrigger->PreCheckCondition(), eTrue);
}
TEST_F(APRV_PtInhPressTriggerUnitMock, PreCheckCondition_1)
{
    MyAPRV_PtInhPressTrigger->IsEnabled = eTrue;
    MyAPRV_PtInhPressTrigger->PressureLimit = 10;
    EXPECT_CALL(*_MgrBdSetting, MgrBdSettingGetPressTriggOn()).Times(1).WillOnce(Return(eTrue));
    EXPECT_CALL(*_PhaseMgrPtr, BDPhaseMgrGetCurrentPhaseId()).Times(1).WillOnce(Return(eAPRVLowPhase));

    EXPECT_CALL(*_PhaseMgrPtr, APRV_PHighPhaseS_IsInUnrestrictedPhase()).Times(1).WillOnce(Return(eTrue));
    EXPECT_CALL(*_BreathMgr, MgrBreathDataGetMonitoredPressure()).Times(1).WillOnce(Return(1));

    EXPECT_EQ(MyAPRV_PtInhPressTrigger->PreCheckCondition(), eTrue);
}

//test for void APRV_PtInhPressTrigger::EnableTrigger(void)
TEST_F(APRV_PtInhPressTriggerUnitMock, EnableTrigger)
{
    MyAPRV_PtInhPressTrigger->EnableTrigger();

    EXPECT_EQ(MyAPRV_PtInhPressTrigger->InUnrestrictedPhase, eFalse);
    EXPECT_EQ(MyAPRV_PtInhPressTrigger->PrevFilteredPress_PHigh, 0);
    EXPECT_EQ(MyAPRV_PtInhPressTrigger->PrevFilteredPress_PLow, 0);
    EXPECT_EQ(MyAPRV_PtInhPressTrigger->IsPressureFixed, eFalse);
}

//test for void APRV_PtInhPressTrigger::EnableTrigger(UNSIGNED pressure)
TEST_F(APRV_PtInhPressTriggerUnitMock, EnableTrigger_with)
{
    MyAPRV_PtInhPressTrigger->EnableTrigger(1);

    EXPECT_EQ(MyAPRV_PtInhPressTrigger->InUnrestrictedPhase, eFalse);
    EXPECT_EQ(MyAPRV_PtInhPressTrigger->IsPressureFixed, eTrue);
    EXPECT_EQ(MyAPRV_PtInhPressTrigger->PressureLimit, 1);

}

//test for void APRV_PtInhPressTrigger::PhaseInSettingChange(LONG value)
TEST_F(APRV_PtInhPressTriggerUnitMock, PhaseInSettingChange_0)
{
    MyAPRV_PtInhPressTrigger->IsEnabled = eTrue;
    MyAPRV_PtInhPressTrigger->IsPressureFixed = eFalse;
    EXPECT_CALL(*_PhaseMgrPtr, BDPhaseMgrGetCurrentPhaseId()).Times(1).WillOnce(Return(eAPRV_PLow_Phase));

    EXPECT_CALL(*_MgrBdSetting, mgrBdSettingPtrGetCurrentValue(eAPRVLowInhPress)).Times(1).WillOnce(Return(11));


    MyAPRV_PtInhPressTrigger->PhaseInSettingChange(10);

    EXPECT_EQ(MyAPRV_PtInhPressTrigger->PressureLimit, 11 - 10);
}
TEST_F(APRV_PtInhPressTriggerUnitMock, PhaseInSettingChange_1)
{
    MyAPRV_PtInhPressTrigger->IsEnabled = eTrue;
    MyAPRV_PtInhPressTrigger->IsPressureFixed = eFalse;
    EXPECT_CALL(*_PhaseMgrPtr, BDPhaseMgrGetCurrentPhaseId()).Times(1).WillOnce(Return(eAPRVExhPhase_InPHigh));

    EXPECT_CALL(*_MgrBdSetting, mgrBdSettingPtrGetCurrentValue(eAPRVHighInhPress)).Times(1).WillOnce(Return(12));


    MyAPRV_PtInhPressTrigger->PhaseInSettingChange(10);

    EXPECT_EQ(MyAPRV_PtInhPressTrigger->PressureLimit, 12 - 10);
}

//test for E_Bool APRV_PtInhPressTrigger::APRV_PHigh_CheckCondition(void)
TEST_F(APRV_PtInhPressTriggerUnitMock, APRV_PHigh_CheckCondition_0)
{
    EXPECT_CALL(*_MgrBdSetting, MgrBdSettingS_GetInstanceGetPressTriggOn()).Times(1).WillOnce(Return(eTrue));
    MyAPRV_PtInhPressTrigger->PrevFilteredPress_PHigh = 4;
    EXPECT_CALL(*_BreathMgr, MgrBreathDataGetMonitoredPressure()).Times(1).WillOnce(Return(4));
    MyAPRV_PtInhPressTrigger->InUnrestrictedPhase = eTrue;

    EXPECT_CALL(*_MgrBdSetting, mgrBdSettingPtrGetCurrentValue(ePressSens)).Times(1).WillOnce(Return(1));
    EXPECT_CALL(*_MgrBdSetting, mgrBdSettingPtrGetCurrentValue(eAPRVHighInhPress)).Times(1).WillOnce(Return(100));

    EXPECT_EQ(MyAPRV_PtInhPressTrigger->APRV_PHigh_CheckCondition(), eTrue);

}
TEST_F(APRV_PtInhPressTriggerUnitMock, APRV_PHigh_CheckCondition_1)
{
    EXPECT_CALL(*_MgrBdSetting, MgrBdSettingS_GetInstanceGetPressTriggOn()).Times(1).WillOnce(Return(eTrue));
    MyAPRV_PtInhPressTrigger->PrevFilteredPress_PHigh = 4;
    EXPECT_CALL(*_BreathMgr, MgrBreathDataGetMonitoredPressure()).Times(1).WillOnce(Return(4));
    MyAPRV_PtInhPressTrigger->InUnrestrictedPhase = eFalse;

//    EXPECT_CALL(*_BreathMgr, MgrBreathDataGetBdBreathDataPtrGetTimeInInh()).Times(1).WillOnce(Return((UNSIGNED)1));
//    EXPECT_CALL(*_PhaseMgrPtr, APRV_PLowPhaseS_IsInUnrestrictedPhase((UNSIGNED)1)).Times(1).WillOnce(Return(eTrue));
//
//    EXPECT_CALL(*_MgrBdSetting, mgrBdSettingPtrGetCurrentValue(ePressSens)).Times(1).WillOnce(Return(1));
//    EXPECT_CALL(*_MgrBdSetting, mgrBdSettingPtrGetCurrentValue(eAPRVHighInhPress)).Times(1).WillOnce(Return(100));

    EXPECT_EQ(MyAPRV_PtInhPressTrigger->APRV_PHigh_CheckCondition(), eFalse);

}

//test for E_Bool APRV_PtInhPressTrigger::APRV_PLow_CheckCondition()
TEST_F(APRV_PtInhPressTriggerUnitMock, APRV_PLow_CheckCondition_0)
{
    EXPECT_CALL(*_MgrBdSetting, MgrBdSettingS_GetInstanceGetPressTriggOn()).Times(1).WillOnce(Return(eTrue));
    MyAPRV_PtInhPressTrigger->PrevFilteredPress_PLow = 4;
    EXPECT_CALL(*_BreathMgr, MgrBreathDataS_GetInstanceGetMonitoredPressure()).Times(1).WillOnce(Return(4));
    MyAPRV_PtInhPressTrigger->InUnrestrictedPhase = eTrue;

    EXPECT_CALL(*_MgrBdSetting, mgrBdSettingPtrGetCurrentValue(ePressSens)).Times(1).WillOnce(Return(1));
    EXPECT_CALL(*_MgrBdSetting, mgrBdSettingPtrGetCurrentValue(eAPRVLowInhPress)).Times(1).WillOnce(Return(100));

    EXPECT_EQ(MyAPRV_PtInhPressTrigger->APRV_PLow_CheckCondition(), eTrue);
}
TEST_F(APRV_PtInhPressTriggerUnitMock, APRV_PLow_CheckCondition_1)
{
    EXPECT_CALL(*_MgrBdSetting, MgrBdSettingS_GetInstanceGetPressTriggOn()).Times(1).WillOnce(Return(eTrue));
    MyAPRV_PtInhPressTrigger->PrevFilteredPress_PLow = 4;
    EXPECT_CALL(*_BreathMgr, MgrBreathDataS_GetInstanceGetMonitoredPressure()).Times(1).WillOnce(Return(4));
    MyAPRV_PtInhPressTrigger->InUnrestrictedPhase = eFalse;

//    EXPECT_CALL(*_BreathMgr, MgrBreathDataGetBdBreathDataPtrGetTimeInInh()).Times(1).WillOnce(Return((UNSIGNED)1));
//    EXPECT_CALL(*_PhaseMgrPtr, APRV_PLowPhaseS_IsInUnrestrictedPhase((UNSIGNED)1)).Times(1).WillOnce(Return(eTrue));
//
//    EXPECT_CALL(*_MgrBdSetting, mgrBdSettingPtrGetCurrentValue(ePressSens)).Times(1).WillOnce(Return(1));
//    EXPECT_CALL(*_MgrBdSetting, mgrBdSettingPtrGetCurrentValue(eAPRVLowInhPress)).Times(1).WillOnce(Return(100));

    EXPECT_EQ(MyAPRV_PtInhPressTrigger->APRV_PLow_CheckCondition(), eFalse);
}

#endif //end define BD_TRIGGER_UNIT_TEST
