#include "EnableTestCase.h"
#ifdef BD_TRIGGER_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"
#include "PtInhPressTrigger.h"
#include "DebugTrace.h"

using namespace ::testing;
#include "BreathMgrMockHeader.h"
#include "MgrBdSettingMockHeader.h"
#include "PhaseMgrMockHeader.h"

PtInhPressTrigger::PtInhPressTrigger (void) :
Trigger (ePtInhPressTrigger)
{
    IsPressureFixed = eFalse;
    InUnrestrictedPhase = eFalse;
    PressureLimit = -20;
    PressSens = 0.5;
    PrevFilteredPress = 0;

}   // end PtInhPressTrigger()

E_Bool PtInhPressTrigger::CheckCondition (void)
{
    if (MgrBdSettingGetPressTriggOn())
    {
        // Filter the pressure value using the equation:
        // y(n) = 0.25 * y(n-1) + 0.75 * x(n)
        LONG filteredPress = (PrevFilteredPress >> 2) +
                (3*(MgrBreathDataGetMonitoredPressure()) >> 2);

        // Update the Previous value to prepare for the next cycle
        PrevFilteredPress = filteredPress;

        // If it has already been determined that the ventilator is in the
        // unrestricted phase, just look for the trigger condition
        if (eTrue == InUnrestrictedPhase)
        {
            // If measured pressure is less than or equal to the trigger
            // sensitivity, the trigger has fired.

            // Get pointer to the MgrBdSetting object
//            MgrBdSetting* mgrBdSettingPtr = MgrBdSetting::S_GetInstance();

            PressSens = mgrBdSettingPtrGetCurrentValue(ePressSens);

            // PressureLimit is the Pressure Sensitivity below Peep
            PressureLimit = (SHORT) (mgrBdSettingPtrGetCurrentValue(eBaselinePress)
                    - PressSens);

            if (filteredPress <= PressureLimit)
            {
                TriggerDidFire = eTrue;
            }
        }

        else
        {
            // See if the ventilator is in the unrestricted phase of exhalation
            // By not passing any parameters, the default minimum exhalation
            // time is used.
            UNSIGNED timeInh =  MgrBreathDataGetBdBreathDataPtrGetTimeInInh();

            if (ExhPhaseS_IsInUnrestrictedPhase() == eTrue)
            {
                // Set the InUnrestrictedPhase to TRUE so that it is not
                // necessary to call S_IsInUnrestrictedPhase() every cycle
                InUnrestrictedPhase = eTrue;

                // If measured pressure is less than or equal to the trigger
                // sensitivity, the trigger has fired.
//                MgrBdSetting* mgrBdSettingPtr = MgrBdSetting::S_GetInstance();

                PressSens = mgrBdSettingPtrGetCurrentValue(ePressSens);

                // PressureLimit is the Pressure Sensitivity below Peep
                PressureLimit = (SHORT) (mgrBdSettingPtrGetCurrentValue(eBaselinePress)
                        - PressSens);

                if (filteredPress <= PressureLimit)
                {
                    TriggerDidFire = eTrue;
                }
            }
        }

    }

    return(TriggerDidFire);

}   // end CheckCondition()
E_Bool PtInhPressTrigger::PreCheckCondition (void)
{
    E_Bool rtnValue = eFalse;       // return value; indicates if trigger fired

    if ((eTrue == IsEnabled)&&(MgrBdSettingGetPressTriggOn()))
    {

        // See if the ventilator is in the unrestricted phase of exhalation
        // By not passing any parameters, the default minimum exhalation
        // time is used.
        if (ExhPhaseS_IsInUnrestrictedPhase() == eTrue)
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
void PtInhPressTrigger::EnableTrigger (void)
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
    Trigger::EnableTrigger();

}   // end EnableTrigger()

void PtInhPressTrigger::EnableTrigger (UNSIGNED pressure)
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
    Trigger::EnableTrigger();

}   // end EnableTrigger()
void PtInhPressTrigger::PhaseInSettingChange (LONG value)
{
//    MgrBdSetting* mgrBdSettingPtr;   // pointer to the MgrBdSetting object

    PressSens = value;

    // If the trigger is enabled and the pressure limit is not fixed,
    // update the PressureLimit as Baseline Pressure - new pressure sensitivity
    if ((eTrue == IsEnabled) && (eFalse == IsPressureFixed))
    {
//        mgrBdSettingPtr = MgrBdSetting::S_GetInstance();

        PressureLimit = (SHORT)(mgrBdSettingPtrGetCurrentValue(eBaselinePress) - value);
    }

}   // end PhaseInSettingChange()

class PtInhPressTriggerUnitMock : public BDTestFixture
{
public:
    PtInhPressTriggerUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~PtInhPressTriggerUnitMock(void){}
    virtual void SetUp(void)
    {
        MyPtInhPressTrigger = new PtInhPressTrigger();
    }
    virtual void TearDown(void)
    {
        delete MyPtInhPressTrigger;
    }
    PtInhPressTrigger *MyPtInhPressTrigger;
};

//test for PtInhPressTrigger::PtInhPressTrigger (void) :
TEST_F(PtInhPressTriggerUnitMock, PtInhPressTrigger)
{
    EXPECT_EQ(MyPtInhPressTrigger->IsPressureFixed, eFalse);
    EXPECT_EQ(MyPtInhPressTrigger->InUnrestrictedPhase, eFalse);
    EXPECT_EQ(MyPtInhPressTrigger->PressureLimit, -20);
    EXPECT_EQ(MyPtInhPressTrigger->PressSens, 0);
    EXPECT_EQ(MyPtInhPressTrigger->PrevFilteredPress, 0);
}

//test for E_Bool PtInhPressTrigger::CheckCondition (void)
TEST_F(PtInhPressTriggerUnitMock, CheckCondition)
{

}

//test for E_Bool PtInhPressTrigger::PreCheckCondition (void)
TEST_F(PtInhPressTriggerUnitMock, PreCheckCondition)
{
//    MyPtInhPressTrigger->IsEnabled = eTrue;
//    EXPECT_CALL(*_MgrBdSetting, MgrBdSettingGetPressTriggOn()).Times(1).WillOnce(Return(eTrue));
//
//    EXPECT_CALL(*_PhaseMgrPtr, ExhPhaseS_IsInUnrestrictedPhase()).Times(1).WillOnce(Return(eTrue));
//
//    EXPECT_CALL(*_BreathMgr, MgrBreathDataGetMonitoredPressure()).Times(1).WillOnce(Return(1));
//    MyPtInhPressTrigger->PressureLimit = 200;
//
//    EXPECT_EQ(MyPtInhPressTrigger->PreCheckCondition(), eTrue);
}

//test for void PtInhPressTrigger::EnableTrigger (void)
TEST_F(PtInhPressTriggerUnitMock, EnableTrigger)
{
    MyPtInhPressTrigger->EnableTrigger();

    EXPECT_EQ(MyPtInhPressTrigger->InUnrestrictedPhase, eFalse);
    EXPECT_EQ(MyPtInhPressTrigger->PrevFilteredPress, eFalse);
    EXPECT_EQ(MyPtInhPressTrigger->IsPressureFixed, eFalse);
}

//test for void PtInhPressTrigger::EnableTrigger (UNSIGNED pressure)
TEST_F(PtInhPressTriggerUnitMock, EnableTrigger_with)
{
    MyPtInhPressTrigger->EnableTrigger(1);

    EXPECT_EQ(MyPtInhPressTrigger->InUnrestrictedPhase, eFalse);
    EXPECT_EQ(MyPtInhPressTrigger->IsPressureFixed, eTrue);
}

//test for void PtInhPressTrigger::PhaseInSettingChange (LONG value)
TEST_F(PtInhPressTriggerUnitMock, PhaseInSettingChange)
{
    MyPtInhPressTrigger->IsEnabled = eTrue;
    MyPtInhPressTrigger->IsPressureFixed = eFalse;

//    EXPECT_CALL(*_MgrBdSetting, mgrBdSettingPtrGetCurrentValue(eBaselinePress)).Times(1).WillOnce(Return(2));

    MyPtInhPressTrigger->PhaseInSettingChange(1);

//    EXPECT_EQ(MyPtInhPressTrigger->PressureLimit, 1);
}

#endif //end define BD_TRIGGER_UNIT_TEST
