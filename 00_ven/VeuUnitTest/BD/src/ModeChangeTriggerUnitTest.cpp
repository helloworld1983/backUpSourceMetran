#include "EnableTestCase.h"
#ifdef BD_TRIGGER_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"
#include "ModeChangeTrigger.h"
#include "DebugTrace.h"

using namespace ::testing;

#include "ModeMgrMockHeader.h"
#include "AlarmMockHeader.h"
#include "StepMotorMockHeader.h"
#include "TriggerMgrMockHeader.h"
#include "PhaseMgrMockHeader.h"

ModeChangeTrigger::ModeChangeTrigger (void) :
        Trigger (eModeChangeTrigger), CurrentMode (eNumberOfModes),
        IsArmed (eFalse)
{
    CurrentMode = eNumberOfModes;       // set to an invalid value so that first
    // setting is phased in

}   // end ModeChangeTrigger()
void ModeChangeTrigger::EnableTrigger (void)
{
    IsArmed = eFalse;
    Trigger::EnableTrigger ();

}   // end EnableTrigger()
E_Bool ModeChangeTrigger::CheckCondition (void)
{
    // Check if a mode change request has been received (indicated by IsArmed)
    IsArmed = eTrue;


    if (eTrue == IsArmed)
    {
        {
            if((HfoValveGetStepPosition() != 0)||(O2ValveGetStepPosition() != 0)
                    ||(AirValveGetStepPosition() != 0)||(ExhValveGetStepPosition() != 0))
            {


                HfoValveRampToPosition(0);
                O2ValveRampToPosition((USHORT)0);
                AirValveRampToPosition((USHORT)0);
                ExhValveRampToPosition((USHORT)0);
                return eFalse;
            }
        }
        BDAlarmTaskClearAllAlarm();
        BDAlarmTaskClearHFOAlarm();

        MgrControllerResetIMVStaticVariables();


        IsArmed = eFalse;
        TriggerDidFire = eTrue;

    }
    return(TriggerDidFire);

}   // end CheckCondition()
void ModeChangeTrigger::PhaseInSettingChange (LONG value)
{
    if (CurrentMode != (E_VentilationMode) value)
    {
        if(CurrentMode == eHFOMode)
        {
//            BdTask::S_GetInstance()->Send(eStopAutoSetRequest);
        }
        CurrentMode = (E_VentilationMode) value;
//
//        // If the trigger is enabled, it is not armed because the mode change
//        // request came in.
        TriggerMgrDisableAllTriggers();
        TriggerMgrEnableTrigger(eModeChangeTrigger);
//
        HfoServoDisable();
//        HfoController->Disable();
        MgrControllerCMVServoControllerDisable();
        MgrControllerCMVServoControllerDisable();

        NCPAPControllerDisable();
        O2FlowControllerDisable();
        AirFlowControllerDisable();
        PhaseMgrProcessNewPhase(eNonVentPhase);
        IsArmed = IsEnabled;
    }

}   // end PhaseInSettingChange()

class ModeChangeTriggerUnitMock : public BDTestFixture
{
public:
    ModeChangeTriggerUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~ModeChangeTriggerUnitMock(void){}
    virtual void SetUp(void)
    {
        MyModeChangeTrigger = new ModeChangeTrigger();
    }
    virtual void TearDown(void)
    {
        delete MyModeChangeTrigger;
    }
    ModeChangeTrigger *MyModeChangeTrigger;
};

//test for ModeChangeTrigger::ModeChangeTrigger (void) :
TEST_F(ModeChangeTriggerUnitMock, ModeChangeTrigger)
{
    EXPECT_EQ(MyModeChangeTrigger->CurrentMode, eNumberOfModes);
}

//test for void ModeChangeTrigger::EnableTrigger (void)
TEST_F(ModeChangeTriggerUnitMock, EnableTrigger)
{
    MyModeChangeTrigger->EnableTrigger();

    EXPECT_EQ(MyModeChangeTrigger->IsArmed, eFalse);
}

//test for E_Bool ModeChangeTrigger::CheckCondition (void)
TEST_F(ModeChangeTriggerUnitMock, CheckCondition_0)
{
    MyModeChangeTrigger->IsArmed = eTrue;

    EXPECT_CALL(*_StepMotor, O2ValveGetStepPosition()).Times(1).WillOnce(Return(1));

    EXPECT_CALL(*_StepMotor, HfoValveRampToPosition(0)).Times(1);
    EXPECT_CALL(*_StepMotor, O2ValveRampToPosition((USHORT)0)).Times(1);
    EXPECT_CALL(*_StepMotor, AirValveRampToPosition((USHORT)0)).Times(1);
    EXPECT_CALL(*_StepMotor, ExhValveRampToPosition((USHORT)0)).Times(1);

    EXPECT_EQ(MyModeChangeTrigger->CheckCondition(), eFalse);
}
TEST_F(ModeChangeTriggerUnitMock, CheckCondition_1)
{
    MyModeChangeTrigger->IsArmed = eTrue;

    EXPECT_CALL(*_StepMotor, HfoValveGetStepPosition()).Times(1).WillOnce(Return(1));

    EXPECT_CALL(*_StepMotor, HfoValveRampToPosition(0)).Times(1);
    EXPECT_CALL(*_StepMotor, O2ValveRampToPosition((USHORT)0)).Times(1);
    EXPECT_CALL(*_StepMotor, AirValveRampToPosition((USHORT)0)).Times(1);
    EXPECT_CALL(*_StepMotor, ExhValveRampToPosition((USHORT)0)).Times(1);

    EXPECT_EQ(MyModeChangeTrigger->CheckCondition(), eFalse);
}
TEST_F(ModeChangeTriggerUnitMock, CheckCondition_2)
{
    MyModeChangeTrigger->IsArmed = eTrue;

    EXPECT_CALL(*_StepMotor, AirValveGetStepPosition()).Times(1).WillOnce(Return(1));

    EXPECT_CALL(*_StepMotor, HfoValveRampToPosition(0)).Times(1);
    EXPECT_CALL(*_StepMotor, O2ValveRampToPosition((USHORT)0)).Times(1);
    EXPECT_CALL(*_StepMotor, AirValveRampToPosition((USHORT)0)).Times(1);
    EXPECT_CALL(*_StepMotor, ExhValveRampToPosition((USHORT)0)).Times(1);

    EXPECT_EQ(MyModeChangeTrigger->CheckCondition(), eFalse);
}
TEST_F(ModeChangeTriggerUnitMock, CheckCondition_3)
{
    MyModeChangeTrigger->IsArmed = eTrue;

    EXPECT_CALL(*_StepMotor, ExhValveGetStepPosition()).Times(1).WillOnce(Return(1));

    EXPECT_CALL(*_StepMotor, HfoValveRampToPosition(0)).Times(1);
    EXPECT_CALL(*_StepMotor, O2ValveRampToPosition((USHORT)0)).Times(1);
    EXPECT_CALL(*_StepMotor, AirValveRampToPosition((USHORT)0)).Times(1);
    EXPECT_CALL(*_StepMotor, ExhValveRampToPosition((USHORT)0)).Times(1);

    EXPECT_EQ(MyModeChangeTrigger->CheckCondition(), eFalse);
}
TEST_F(ModeChangeTriggerUnitMock, CheckCondition_4)
{
    MyModeChangeTrigger->IsArmed = eTrue;

    EXPECT_CALL(*_StepMotor, HfoValveGetStepPosition()).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*_StepMotor, O2ValveGetStepPosition()).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*_StepMotor, AirValveGetStepPosition()).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*_StepMotor, ExhValveGetStepPosition()).Times(1).WillOnce(Return(0));

    EXPECT_CALL(*_Alarm, BDAlarmTaskClearAllAlarm()).Times(1);
    EXPECT_CALL(*_Alarm, BDAlarmTaskClearHFOAlarm()).Times(1);

    EXPECT_CALL(*_StepMotor, MgrControllerResetIMVStaticVariables()).Times(1);



    EXPECT_EQ(MyModeChangeTrigger->CheckCondition(), eTrue);
    EXPECT_EQ(MyModeChangeTrigger->IsArmed, eFalse);
}

//test for void ModeChangeTrigger::PhaseInSettingChange (LONG value)
TEST_F(ModeChangeTriggerUnitMock, PhaseInSettingChange)
{
}

#endif //end define BD_TRIGGER_UNIT_TEST
