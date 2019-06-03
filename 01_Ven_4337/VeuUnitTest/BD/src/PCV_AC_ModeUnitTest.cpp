#include "EnableTestCase.h"
#ifdef BD_MODE_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"
#include "PCV_AC_Mode.h"
#include "DebugTrace.h"

using namespace ::testing;

#include "ModeMgrMockHeader.h"
#include "PhaseMgrMockHeader.h"
#include "AlarmMockHeader.h"
#include "TriggerMgrMockHeader.h"

PCV_AC_Mode* PCV_AC_Mode::S_Instance = NULL;

PCV_AC_Mode* PCV_AC_Mode::S_GetInstance(void)
{
    // If the pointer to the instance is NULL, create the object.
    if (NULL == S_Instance)
    {
        S_Instance = new PCV_AC_Mode();
        ASSERTION(S_Instance != NULL);
    }

    return(S_Instance);

}
PCV_AC_Mode::PCV_AC_Mode(void) : MixedMode()
{

}
void PCV_AC_Mode::Enter(E_VentilationMode fromModeId)
{
//    MixedMode::Enter(fromModeId);

    PhaseMgrProcessNewVentType(ePressureVentilation);
    if(fromModeId != eApneaBackupMode)
    {
        MgrControllerCMVServoControllerResetIMVStaticVariables();
    }

    MgrControllerCMVServoControllerEnable();

    PhaseMgrProcessNewPhase(eExhPhase);

}
void PCV_AC_Mode::ProcessPtInhTrigger(void)
{

    ProcessPtInhTriggerAC();

}   // end ProcessPtInhTrigger()

void PCV_AC_Mode::ProcessTriggerID (E_TriggerId id)
{
    switch (id)
    {
        case ePtInhTrigger:
            ProcessPtInhTrigger();
            break;

        default:
        ASSERTION(eFalse);
            break;
    }
}

class PCV_AC_ModeUnitMock : public BDTestFixture
{
public:
    PCV_AC_ModeUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~PCV_AC_ModeUnitMock(void){}
    virtual void SetUp(void)
    {
        MyPCV_AC_Mode = new PCV_AC_Mode();
    }
    virtual void TearDown(void)
    {
        delete MyPCV_AC_Mode;
    }
    PCV_AC_Mode *MyPCV_AC_Mode;
};

//test for PCV_AC_Mode* PCV_AC_Mode::S_GetInstance(void)
TEST_F(PCV_AC_ModeUnitMock, S_GetInstance)
{
    MyPCV_AC_Mode->S_GetInstance();

    EXPECT_NE(MyPCV_AC_Mode->S_Instance, (void*)0);
}

//test for PCV_AC_Mode::PCV_AC_Mode(void) : MixedMode()
TEST_F(PCV_AC_ModeUnitMock, PCV_AC_Mode)
{
    //no source, not need to test
}

//test for void PCV_AC_Mode::Enter(E_VentilationMode fromModeId)
TEST_F(PCV_AC_ModeUnitMock, Enter)
{
    EXPECT_CALL(*_PhaseMgrPtr, PhaseMgrProcessNewVentType(ePressureVentilation)).Times(1);

    EXPECT_CALL(*_StepMotor, MgrControllerCMVServoControllerResetIMVStaticVariables()).Times(1);

    EXPECT_CALL(*_StepMotor, MgrControllerCMVServoControllerEnable()).Times(1);

    EXPECT_CALL(*_PhaseMgrPtr, PhaseMgrProcessNewPhase(eExhPhase)).Times(1);

    MyPCV_AC_Mode->Enter(eHardwareErrorMode);
}

//test for void PCV_AC_Mode::ProcessPtInhTrigger(void)
TEST_F(PCV_AC_ModeUnitMock, ProcessPtInhTrigger)
{
    //calling function is tested, not need to test
}

//test for void PCV_AC_Mode::ProcessTriggerID (E_TriggerId id)
TEST_F(PCV_AC_ModeUnitMock, ProcessTriggerID)
{
    //calling function is tested, not need to test
}

#endif //end define BD_MODE_UNIT_TEST
