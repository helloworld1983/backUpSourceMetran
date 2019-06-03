#include "EnableTestCase.h"
#ifdef BD_TRIGGER_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"
#include "ExtremeHighPEEPTrigger.h"
#include "DebugTrace.h"

using namespace ::testing;
#include "MgrBdSettingMockHeader.h"
#include "PhaseMgrMockHeader.h"
#include "AlarmMockHeader.h"
#include "StepMotorMockHeader.h"
#include "ModeMgrMockHeader.h"

ExtremeHighPEEPTrigger* ExtremeHighPEEPTrigger::S_Instance = NULL;

ExtremeHighPEEPTrigger* ExtremeHighPEEPTrigger::S_GetInstance(void)
{
    // If the object has not already been created, create it.
    if (NULL == S_Instance)
    {
        S_Instance = new ExtremeHighPEEPTrigger();
        ASSERTION(S_Instance != NULL);
    }

    return(S_Instance);

}
ExtremeHighPEEPTrigger::ExtremeHighPEEPTrigger()
: Trigger(eExtremeHighPEEPTrigger)
{
    HighPressAlarm  = 20 * SCALE;
    InactiveTimer = 0;
}
E_Bool ExtremeHighPEEPTrigger::CheckCondition(void)
{

    static const USHORT ABOVE15_PRESS_TIME = 15;
    static const USHORT ABOVE15_PRESS_TIME_NCPAP = 200;
    static LONG ExtremeHighPEEPCount = 0;
    //    MgrBdSetting* mgrBdSettingPtr = MgrBdSetting::S_GetInstance();
    E_PhaseId phase = BDPhaseMgrGetCurrentPhaseId();
    LONG ExtremeHighPEEP_Offset =  mgrBdSettingPtrGetCurrentValue(eExtremeHighPEEP_Autoset);

    if (InactiveTimer > 0)
    {
        InactiveTimer--;
        return (eFalse);
    }

    if(!((phase == eExhPhase) || (phase == eNCPAPPhase)))
    {
        TriggerDidFire = eFalse;
        return TriggerDidFire;
    }

    // Add NCPAP case here.
    if (phase == eNCPAPPhase)
    {
        HighPressAlarm = mgrBdSettingPtrGetCurrentValue(eHighNCPAPLimit);

        LONG InhPressSensor = 2;//test
        LONG ExhPressSensor = 2;//test

        BDRequestDevice(eInhPressSensorGetLastReading);
        BDRequestDevice(eExhPressSensorGetLastReading);

        if ((InhPressSensor > HighPressAlarm + ExtremeHighPEEP_Offset)||
                (ExhPressSensor > HighPressAlarm + ExtremeHighPEEP_Offset))
        {
            ExtremeHighPEEPCount++;
            if(ExtremeHighPEEPCount >= ABOVE15_PRESS_TIME_NCPAP)
            {
                TriggerDidFire = eTrue;
                HighPressModeSetCauseOfAlarm(eExtremeHighPeep);
                BDAlarmTaskSetIsActive(eExtremeHighPeep, eActive);
            }
        }
        else
        {
            if(eActive == BDAlarmTaskGetIsActive(eExtremeHighPeep))
            {
                BDAlarmTaskSetIsActive(eExtremeHighPeep, eNotActive);
            }
            ExtremeHighPEEPCount = 0;
        }
    }
    else if(phase == eExhPhase)
    {
        if(ExhPhaseS_CalcTimeSincePhaseStart() >= 500)//200ms
        {
            HighPressAlarm = mgrBdSettingPtrGetCurrentValue(eHighBaselineLimit);

            LONG InhPressSensor = 5;//test
            LONG ExhPressSensor = 5;//test

            BDRequestDevice(eInhPressSensorGetLastReading);
            BDRequestDevice(eExhPressSensorGetLastReading);

            if ((InhPressSensor > HighPressAlarm + ExtremeHighPEEP_Offset)||
                    (ExhPressSensor > HighPressAlarm + ExtremeHighPEEP_Offset))
            {
                ExtremeHighPEEPCount++;
                if(ExtremeHighPEEPCount >= ABOVE15_PRESS_TIME)
                {
                    ExtremeHighPEEPCount = 0;
                    // If the pressure condition is met, set the TriggerDidFire attribute
                    // and update the alarm status.

                    TriggerDidFire = eTrue;
                    HighPressModeSetCauseOfAlarm(eExtremeHighPeep);
                    BDAlarmTaskSetIsActive(eExtremeHighPeep, eActive);
                }
            }
            else
            {
                if(eActive == BDAlarmTaskGetIsActive(eExtremeHighPeep))
                {
                    BDAlarmTaskSetIsActive(eExtremeHighPeep, eNotActive);
                }
                ExtremeHighPEEPCount = 0;
            }
        }
    }

    return (TriggerDidFire);

}   // end CheckCondition()
void ExtremeHighPEEPTrigger::SetInactiveTimer(USHORT value)
{
    InactiveTimer = value;
}
LONG ExtremeHighPEEPTrigger::GetHighPressAlarm(void)
{
    return HighPressAlarm;
}

class ExtremeHighPEEPTriggerUnitMock : public BDTestFixture
{
public:
    ExtremeHighPEEPTriggerUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~ExtremeHighPEEPTriggerUnitMock(void){}
    virtual void SetUp(void)
    {
        MyExtremeHighPEEPTrigger = new ExtremeHighPEEPTrigger();
    }
    virtual void TearDown(void)
    {
        delete MyExtremeHighPEEPTrigger;
    }

    ExtremeHighPEEPTrigger *MyExtremeHighPEEPTrigger;
};

//test for ExtremeHighPEEPTrigger* ExtremeHighPEEPTrigger::S_GetInstance(void)
TEST_F(ExtremeHighPEEPTriggerUnitMock, S_GetInstance)
{
    MyExtremeHighPEEPTrigger->S_GetInstance();

    EXPECT_NE(MyExtremeHighPEEPTrigger->S_Instance, (void*)0);
}

//test for ExtremeHighPEEPTrigger::ExtremeHighPEEPTrigger()
TEST_F(ExtremeHighPEEPTriggerUnitMock, ExtremeHighPEEPTrigger)
{
    EXPECT_EQ(MyExtremeHighPEEPTrigger->HighPressAlarm, 20 * SCALE);
    EXPECT_EQ(MyExtremeHighPEEPTrigger->InactiveTimer, 0);
}

//test for E_Bool ExtremeHighPEEPTrigger::CheckCondition(void)
TEST_F(ExtremeHighPEEPTriggerUnitMock, CheckCondition_0)
{
    EXPECT_CALL(*_PhaseMgrPtr, BDPhaseMgrGetCurrentPhaseId()).Times(1).WillOnce(Return(eExhPhase));
    EXPECT_CALL(*_MgrBdSetting, mgrBdSettingPtrGetCurrentValue(eExtremeHighPEEP_Autoset)).Times(1).WillOnce(Return(1));

    MyExtremeHighPEEPTrigger->InactiveTimer = 10;

    EXPECT_EQ(MyExtremeHighPEEPTrigger->CheckCondition(), eFalse);
    EXPECT_EQ(MyExtremeHighPEEPTrigger->InactiveTimer, 9);

}
TEST_F(ExtremeHighPEEPTriggerUnitMock, CheckCondition_1)
{
    EXPECT_CALL(*_PhaseMgrPtr, BDPhaseMgrGetCurrentPhaseId()).Times(1).WillOnce(Return(eExhPhase));
    EXPECT_CALL(*_MgrBdSetting, mgrBdSettingPtrGetCurrentValue(eExtremeHighPEEP_Autoset)).Times(1).WillOnce(Return(1));

    MyExtremeHighPEEPTrigger->InactiveTimer = 0;

    EXPECT_EQ(MyExtremeHighPEEPTrigger->CheckCondition(), eFalse);

}
TEST_F(ExtremeHighPEEPTriggerUnitMock, CheckCondition_2)
{
    for(int i = 0; i < 200; ++i)
    {
        EXPECT_CALL(*_PhaseMgrPtr, BDPhaseMgrGetCurrentPhaseId()).Times(1).WillOnce(Return(eNCPAPPhase));
        EXPECT_CALL(*_MgrBdSetting, mgrBdSettingPtrGetCurrentValue(eExtremeHighPEEP_Autoset)).Times(1).WillOnce(Return(1));
        MyExtremeHighPEEPTrigger->InactiveTimer = 0;

        EXPECT_CALL(*_MgrBdSetting, mgrBdSettingPtrGetCurrentValue(eHighNCPAPLimit)).Times(1).WillOnce(Return(0));

        MyExtremeHighPEEPTrigger->CheckCondition();
    }

    EXPECT_EQ(MyExtremeHighPEEPTrigger->TriggerDidFire, eTrue);

}
TEST_F(ExtremeHighPEEPTriggerUnitMock, CheckCondition_3)
{
    EXPECT_CALL(*_PhaseMgrPtr, BDPhaseMgrGetCurrentPhaseId()).Times(1).WillOnce(Return(eNCPAPPhase));
    EXPECT_CALL(*_MgrBdSetting, mgrBdSettingPtrGetCurrentValue(eExtremeHighPEEP_Autoset)).Times(1).WillOnce(Return(1));
    MyExtremeHighPEEPTrigger->InactiveTimer = 0;
    MyExtremeHighPEEPTrigger->TriggerDidFire = eTrue;

    EXPECT_CALL(*_MgrBdSetting, mgrBdSettingPtrGetCurrentValue(eHighNCPAPLimit)).Times(1).WillOnce(Return(5));

    EXPECT_CALL(*_Alarm, BDAlarmTaskGetIsActive(eExtremeHighPeep)).Times(1).WillOnce(Return(eActive));
    EXPECT_CALL(*_Alarm, BDAlarmTaskSetIsActive(eExtremeHighPeep, eNotActive)).Times(1);

    MyExtremeHighPEEPTrigger->CheckCondition();

    EXPECT_EQ(MyExtremeHighPEEPTrigger->TriggerDidFire, eTrue);
}
TEST_F(ExtremeHighPEEPTriggerUnitMock, CheckCondition_4)
{
    for(int i = 0; i < 15; ++i)
    {
        EXPECT_CALL(*_PhaseMgrPtr, BDPhaseMgrGetCurrentPhaseId()).Times(1).WillOnce(Return(eExhPhase));
        EXPECT_CALL(*_MgrBdSetting, mgrBdSettingPtrGetCurrentValue(eExtremeHighPEEP_Autoset)).Times(1).WillOnce(Return(1));

        EXPECT_CALL(*_PhaseMgrPtr, ExhPhaseS_CalcTimeSincePhaseStart()).Times(1).WillOnce(Return(1000));

        EXPECT_CALL(*_MgrBdSetting, mgrBdSettingPtrGetCurrentValue(eHighBaselineLimit)).Times(1).WillOnce(Return(1));

        MyExtremeHighPEEPTrigger->InactiveTimer = 0;

        MyExtremeHighPEEPTrigger->CheckCondition();
    }

    EXPECT_EQ(MyExtremeHighPEEPTrigger->TriggerDidFire, eTrue);

}
TEST_F(ExtremeHighPEEPTriggerUnitMock, CheckCondition_5)
{
    EXPECT_CALL(*_PhaseMgrPtr, BDPhaseMgrGetCurrentPhaseId()).Times(1).WillOnce(Return(eExhPhase));
    EXPECT_CALL(*_MgrBdSetting, mgrBdSettingPtrGetCurrentValue(eExtremeHighPEEP_Autoset)).Times(1).WillOnce(Return(1));

    EXPECT_CALL(*_PhaseMgrPtr, ExhPhaseS_CalcTimeSincePhaseStart()).Times(1).WillOnce(Return(1000));

    EXPECT_CALL(*_MgrBdSetting, mgrBdSettingPtrGetCurrentValue(eHighBaselineLimit)).Times(1).WillOnce(Return(10));

    MyExtremeHighPEEPTrigger->InactiveTimer = 0;
    MyExtremeHighPEEPTrigger->TriggerDidFire = eTrue;

    EXPECT_CALL(*_Alarm, BDAlarmTaskGetIsActive(eExtremeHighPeep)).Times(1).WillOnce(Return(eActive));
    EXPECT_CALL(*_Alarm, BDAlarmTaskSetIsActive(eExtremeHighPeep, eNotActive)).Times(1).WillOnce(Return(eActive));

    MyExtremeHighPEEPTrigger->CheckCondition();

    EXPECT_EQ(MyExtremeHighPEEPTrigger->TriggerDidFire, eTrue);
}

//test for void ExtremeHighPEEPTrigger::SetInactiveTimer(USHORT value)
TEST_F(ExtremeHighPEEPTriggerUnitMock, SetInactiveTimer)
{
    MyExtremeHighPEEPTrigger->SetInactiveTimer(10);

    EXPECT_EQ(MyExtremeHighPEEPTrigger->InactiveTimer, 10);
}

//test for LONG ExtremeHighPEEPTrigger::GetHighPressAlarm(void)
TEST_F(ExtremeHighPEEPTriggerUnitMock, GetHighPressAlarm)
{
    MyExtremeHighPEEPTrigger->HighPressAlarm = 90;

    EXPECT_EQ(MyExtremeHighPEEPTrigger->GetHighPressAlarm(), 90);
}

#endif //end define BD_TRIGGER_UNIT_TEST
