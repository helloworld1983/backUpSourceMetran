#include "EnableTestCase.h"
#ifdef BD_MODE_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"
#include "VCV_AC_Mode.h"
#include "DebugTrace.h"

using namespace ::testing;

#include "PhaseMgrMockHeader.h"
#include "TriggerMgrMockHeader.h"

#include "StepMotorMockHeader.h"

VCV_AC_Mode* VCV_AC_Mode::S_Instance = NULL;

VCV_AC_Mode* VCV_AC_Mode::S_GetInstance(void)
{
    // If the pointer to the instance is NULL, create the object.
    if (NULL == S_Instance)
    {
        S_Instance = new VCV_AC_Mode();
        ASSERTION(S_Instance != NULL);
    }

    return(S_Instance);

}
VCV_AC_Mode::VCV_AC_Mode(void) : MixedMode()
{
}
void VCV_AC_Mode::Enter(E_VentilationMode fromModeId)
{
//    MixedMode::Enter(fromModeId);
    PhaseMgrProcessNewVentType(eVolumeVentilation);

    if(fromModeId != eApneaBackupMode)
    {
        MgrControllerVCVServoControllerResetIMVStaticVariables();
        PhaseMgrResetVolumeAdjustmentOfVCVInhPhase();
    }

    MgrControllerVCVServoControllerEnable();
    PhaseMgrProcessNewPhase(eExhPhase);
}
void VCV_AC_Mode::ProcessPtInhTrigger(void)
{
    ProcessPtInhTriggerAC();
}   // end ProcessPtInhTrigger()
void VCV_AC_Mode::ProcessTriggerID (E_TriggerId id)
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

class VCV_AC_ModeUnitMock : public BDTestFixture
{
public:
    VCV_AC_ModeUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~VCV_AC_ModeUnitMock(void){}
    virtual void SetUp(void)
    {
        MyVCV_AC_Mode = new VCV_AC_Mode();
    }
    virtual void TearDown(void)
    {
        delete MyVCV_AC_Mode;
    }
    VCV_AC_Mode *MyVCV_AC_Mode;
};

//test for VCV_AC_Mode* VCV_AC_Mode::S_GetInstance(void)
TEST_F(VCV_AC_ModeUnitMock, S_GetInstance)
{
    MyVCV_AC_Mode->S_GetInstance();

    EXPECT_NE(MyVCV_AC_Mode->S_Instance, (void*)0);
}

//test for VCV_AC_Mode::VCV_AC_Mode(void) : MixedMode()
TEST_F(VCV_AC_ModeUnitMock, VCV_AC_Mode)
{
    //no source, not need to test
}

//test for void VCV_AC_Mode::Enter(E_VentilationMode fromModeId)
TEST_F(VCV_AC_ModeUnitMock, Enter)
{
    EXPECT_CALL(*_PhaseMgrPtr, PhaseMgrProcessNewVentType(eVolumeVentilation)).Times(1);

    EXPECT_CALL(*_StepMotor,MgrControllerVCVServoControllerResetIMVStaticVariables()).Times(1);

    EXPECT_CALL(*_PhaseMgrPtr, PhaseMgrResetVolumeAdjustmentOfVCVInhPhase()).Times(1);

    EXPECT_CALL(*_StepMotor,MgrControllerVCVServoControllerEnable()).Times(1);

    EXPECT_CALL(*_PhaseMgrPtr, PhaseMgrProcessNewPhase(eExhPhase)).Times(1);

    MyVCV_AC_Mode->Enter(eHardwareErrorMode);
}

//test for void VCV_AC_Mode::ProcessPtInhTrigger(void)
TEST_F(VCV_AC_ModeUnitMock, ProcessPtInhTrigger)
{
    //all calling function is tested, not need to test
}

//test for void VCV_AC_Mode::ProcessTriggerID (E_TriggerId id)
TEST_F(VCV_AC_ModeUnitMock, ProcessTriggerID)
{
    //all calling function is tested, not need to test
}

#endif //end define BD_MODE_UNIT_TEST
