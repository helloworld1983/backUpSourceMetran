#include "EnableTestCase.h"
#ifdef BD_TRIGGER_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"
#include "LowBaselineAlarmTrigger.h"
#include "DebugTrace.h"

using namespace ::testing;

#include "AlarmMockHeader.h"
#include "BreathMgrMockHeader.h"
#include "MgrBdSettingMockHeader.h"
#include "StepMotorMockHeader.h"
#include "PhaseMgrMockHeader.h"

static const UNSIGNED MIN_LOP_DURATION = 1 * TICKS_PER_SEC;   // 1 sec
LowBaselineAlarmTrigger::LowBaselineAlarmTrigger (void) :
        Trigger (eLowBaselineAlarmTrigger)
{

}   // end LowBaselineAlarmTrigger()
E_Bool LowBaselineAlarmTrigger::CheckCondition (void)
{
    // Check if the monitored pressure is less than the pressure limit.
    // The PressureLimit is initialized in EnableTrigger().

     if (MgrBreathDataGetMonitoredPressure() < PressureLimit)
     {
         // Check if the condition was met on the previous cycle.
         if (eTrue == IsConditionMet)
         {
             // Since the condition has been met before, see how long the
             // condition has been met.  If this value exceeds the minimum
             // time criteria, annunciate the alarm.
             if (BDCalcElapsedTime(TimeConditionFirstMet)>= MIN_LOP_DURATION)
             {
                 BDAlarmTaskSetIsActive(eLowPeep, eActive);
             }

         }
         else
         {
             // The condition was not met on the previous cycle, so set
             // the IsConditionMet flag and store the time stamp of when
             // the condition was met.
             IsConditionMet = eTrue;
//             TimeConditionFirstMet= SystemClock::Retrieve();
         }
     }
     else
     {
         // At least one sample did not meet the alarm criteria.
         IsCondMetEveryCycle = eFalse;

         // Check for auto-reset
         if(eActive == BDAlarmTaskGetIsActive(eLowPeep))
         {
             // Check if the condition was not met on the previous cycle.
             // The alarm condition not being met is the same as the
             // auto-reset condition being met
             if (eFalse == IsConditionMet)
             {
                 // See how long the auto-reset condition has been present.
                 if (BDCalcElapsedTime(TimeConditionFirstMet) >= MIN_LOP_DURATION)
                 {
                     BDAlarmTaskSetIsActive(eLowPeep, eNotActive);
                 }

             }
             else
             {
                 // The condition was not met on the previous cycle, so reset
                 // the IsConditionMet flag and store the time stamp of when
                 // the auto-reset condition was met.
                 IsConditionMet = eFalse;
//                 TimeConditionFirstMet= SystemClock::Retrieve();
             }
         }

         else
         {
             // Failed the pressure condition check.
             IsConditionMet = eFalse;
         }

     }

     // This trigger never returns eTrue to the caller since it does not cause
     // a mode or phase change.  Instead, this trigger handled its own trigger
     // by updating the alarm status.
     return(eFalse);

}   // end CheckCondition()
void LowBaselineAlarmTrigger::EnableTrigger (void)
{
//    MgrBdSetting* mgrBdSettingPtr = MgrBdSetting::S_GetInstance();

    PressureLimit = (SHORT) mgrBdSettingPtrGetCurrentValue(eLowBaselineLimit);

    // What we want to do is put a dead band of 0.2 cm H2O around the Low PEEP
    // alarm when the alarm limit is set to zero (0).  This means that when the
    // Low PEEP alarm is set to zero we really want to use -0.2 cm H2O as the
    // level to trip the alarm rather than 0.0 cm H2O.  This change also effects
    // the Low EPAP alarm but it should never be a problem due to the lowest
    // EPAP limit is 3 cmH2O
    if (0 == PressureLimit)
        PressureLimit = -20;

    // Initialize IsConditionMet.
    IsConditionMet = eFalse;

    // Initialize TimeConditionFirstMet.
//    TimeConditionFirstMet= SystemClock::Retrieve();

    // Assume the condition is met every cycle.  If at least one pressure
    // cycle does not meet the condition, this flag will be set to FALSE.
    // It is used when the exhalation time is less than MIN_LOP_DURATION
    // to reset the alarm if the condition is not met every cycle.
    IsCondMetEveryCycle = eTrue;

    // Invoke Base Class Enable
    Trigger::EnableTrigger();

}   // end EnableTrigger()
void LowBaselineAlarmTrigger::DisableTrigger (void)
{
#ifdef HFO_SYSTEM
    if((ModeMgr::S_GetInstance()->GetCurrentModeId() >= eFirstHFOMode) &&
        (ModeMgr::S_GetInstance()->GetCurrentModeId() <= eLastHFOMode));
    else
#endif
    {
        // Check if the exhalation lasted less than MIN_LOP_DURATION.  If so,
        // the alarm is tripped if the condition is met every cycle.  Otherwise,
        // the alarm is reset.
        if (MIN_LOP_DURATION > ExhPhaseS_CalcTimeSincePhaseStart())
        {
            if (eTrue == IsCondMetEveryCycle)
            {
                BDAlarmTaskSetIsActive(eLowPeep, eActive);
            }
            else
            {
                BDAlarmTaskSetIsActive(eLowPeep, eNotActive);

            }
        }
    }

//    Trigger::DisableTrigger();

}   // end DisableTrigger()

class LowBaselineAlarmTriggerUnitMock : public BDTestFixture
{
public:
    LowBaselineAlarmTriggerUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~LowBaselineAlarmTriggerUnitMock(void){}
    virtual void SetUp(void)
    {
        MyLowBaselineAlarmTrigger = new LowBaselineAlarmTrigger();
    }
    virtual void TearDown(void)
    {
        delete MyLowBaselineAlarmTrigger;
    }
    LowBaselineAlarmTrigger *MyLowBaselineAlarmTrigger;
};

//test for LowBaselineAlarmTrigger::LowBaselineAlarmTrigger (void) :
TEST_F(LowBaselineAlarmTriggerUnitMock, LowBaselineAlarmTrigger)
{
    //no source, not need to test
}

//test for E_Bool LowBaselineAlarmTrigger::CheckCondition (void)
TEST_F(LowBaselineAlarmTriggerUnitMock, CheckCondition_0)
{
    MyLowBaselineAlarmTrigger->PressureLimit = 10;
    EXPECT_CALL(*_BreathMgr, MgrBreathDataGetMonitoredPressure()).Times(1).WillOnce(Return(1));
    MyLowBaselineAlarmTrigger->IsConditionMet = eTrue;
    MyLowBaselineAlarmTrigger->TimeConditionFirstMet = 10;

    EXPECT_CALL(*_BreathMgr, BDCalcElapsedTime(10)).Times(1).WillOnce(Return(MIN_LOP_DURATION));

    EXPECT_CALL(*_Alarm, BDAlarmTaskSetIsActive(eLowPeep, eActive)).Times(1);

    MyLowBaselineAlarmTrigger->CheckCondition();
}
TEST_F(LowBaselineAlarmTriggerUnitMock, CheckCondition_1)
{
    MyLowBaselineAlarmTrigger->PressureLimit = 10;
    EXPECT_CALL(*_BreathMgr, MgrBreathDataGetMonitoredPressure()).Times(1).WillOnce(Return(1));
    MyLowBaselineAlarmTrigger->IsConditionMet = eTrue;
    MyLowBaselineAlarmTrigger->TimeConditionFirstMet = 10;

    MyLowBaselineAlarmTrigger->CheckCondition();
    EXPECT_EQ(MyLowBaselineAlarmTrigger->IsConditionMet, eTrue);
}
TEST_F(LowBaselineAlarmTriggerUnitMock, CheckCondition_2)
{
    MyLowBaselineAlarmTrigger->PressureLimit = 10;
    EXPECT_CALL(*_BreathMgr, MgrBreathDataGetMonitoredPressure()).Times(1).WillOnce(Return(20));

    EXPECT_CALL(*_Alarm, BDAlarmTaskGetIsActive(eLowPeep)).Times(1).WillOnce(Return(eNotActive));

    MyLowBaselineAlarmTrigger->CheckCondition();
    EXPECT_EQ(MyLowBaselineAlarmTrigger->IsConditionMet, eFalse);
}
TEST_F(LowBaselineAlarmTriggerUnitMock, CheckCondition_3)
{
    MyLowBaselineAlarmTrigger->PressureLimit = 10;
    EXPECT_CALL(*_BreathMgr, MgrBreathDataGetMonitoredPressure()).Times(1).WillOnce(Return(20));

    EXPECT_CALL(*_Alarm, BDAlarmTaskGetIsActive(eLowPeep)).Times(1).WillOnce(Return(eActive));
    MyLowBaselineAlarmTrigger->TimeConditionFirstMet = 10;
    MyLowBaselineAlarmTrigger->IsConditionMet = eFalse;

    EXPECT_CALL(*_BreathMgr, BDCalcElapsedTime(10)).Times(1).WillOnce(Return(MIN_LOP_DURATION));
    EXPECT_CALL(*_Alarm, BDAlarmTaskSetIsActive(eLowPeep, eNotActive)).Times(1);


    MyLowBaselineAlarmTrigger->CheckCondition();
    EXPECT_EQ(MyLowBaselineAlarmTrigger->IsConditionMet, eFalse);
}
TEST_F(LowBaselineAlarmTriggerUnitMock, CheckCondition_4)
{
    MyLowBaselineAlarmTrigger->PressureLimit = 10;
    EXPECT_CALL(*_BreathMgr, MgrBreathDataGetMonitoredPressure()).Times(1).WillOnce(Return(20));

    EXPECT_CALL(*_Alarm, BDAlarmTaskGetIsActive(eLowPeep)).Times(1).WillOnce(Return(eActive));

    MyLowBaselineAlarmTrigger->IsConditionMet = eTrue;

    MyLowBaselineAlarmTrigger->CheckCondition();
    EXPECT_EQ(MyLowBaselineAlarmTrigger->IsConditionMet, eFalse);
}

//test for void LowBaselineAlarmTrigger::EnableTrigger (void)
TEST_F(LowBaselineAlarmTriggerUnitMock, EnableTrigger)
{
    MyLowBaselineAlarmTrigger->EnableTrigger();

    EXPECT_EQ(MyLowBaselineAlarmTrigger->IsConditionMet, eFalse);
    EXPECT_EQ(MyLowBaselineAlarmTrigger->IsCondMetEveryCycle, eTrue);
}

//test for void LowBaselineAlarmTrigger::DisableTrigger (void)
TEST_F(LowBaselineAlarmTriggerUnitMock, DisableTrigger_0)
{
    EXPECT_CALL(*_PhaseMgrPtr, ExhPhaseS_CalcTimeSincePhaseStart()).Times(1).WillOnce(Return(MIN_LOP_DURATION - 1));
    MyLowBaselineAlarmTrigger->IsCondMetEveryCycle = eTrue;

    EXPECT_CALL(*_Alarm, BDAlarmTaskSetIsActive(eLowPeep, eActive));

    MyLowBaselineAlarmTrigger->DisableTrigger();
}
TEST_F(LowBaselineAlarmTriggerUnitMock, DisableTrigger_1)
{
    EXPECT_CALL(*_PhaseMgrPtr, ExhPhaseS_CalcTimeSincePhaseStart()).Times(1).WillOnce(Return(MIN_LOP_DURATION - 1));
    MyLowBaselineAlarmTrigger->IsCondMetEveryCycle = eFalse;

    EXPECT_CALL(*_Alarm, BDAlarmTaskSetIsActive(eLowPeep, eNotActive));

    MyLowBaselineAlarmTrigger->DisableTrigger();
}

#endif //end define BD_TRIGGER_UNIT_TEST
