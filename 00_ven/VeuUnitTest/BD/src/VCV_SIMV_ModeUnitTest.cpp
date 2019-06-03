#include "EnableTestCase.h"
#ifdef BD_MODE_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"
#include "VCV_SIMV_Mode.h"
#include "DebugTrace.h"

using namespace ::testing;

#include "ModeMgrMockHeader.h"
#include "PhaseMgrMockHeader.h"
#include "TriggerMgrMockHeader.h"
#include "StepMotorMockHeader.h"

VCV_SIMV_Mode* VCV_SIMV_Mode::S_Instance = NULL;
VCV_SIMV_Mode* VCV_SIMV_Mode::S_GetInstance(void)
{
    // If the pointer to the instance is NULL, create the object.
    if (NULL == S_Instance)
    {
        S_Instance = new VCV_SIMV_Mode();
        ASSERTION(S_Instance != NULL);
    }

    return(S_Instance);

}
VCV_SIMV_Mode::VCV_SIMV_Mode(void): MixedMode()
{
}
void VCV_SIMV_Mode::Enter(E_VentilationMode fromModeId)
{
    PhaseMgrProcessNewVentType(eVolumeVentilation);
//    MixedMode::Enter(fromModeId);

    if(fromModeId != eApneaBackupMode)
    {
        MgrControllerVCVServoControllerResetIMVStaticVariables();
        PhaseMgrResetVolumeAdjustmentOfVCVInhPhase();
    }

    MgrControllerVCVServoControllerEnable();
    PhaseMgrProcessNewPhase(eExhPhase);;
}

class VCV_SIMV_ModeUnitMock : public BDTestFixture
{
public:
    VCV_SIMV_ModeUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~VCV_SIMV_ModeUnitMock(void){}
    virtual void SetUp(void)
    {
        MyVCV_SIMV_Mode = new VCV_SIMV_Mode();
    }
    virtual void TearDown(void)
    {
        delete MyVCV_SIMV_Mode;
    }
    VCV_SIMV_Mode *MyVCV_SIMV_Mode;
};

//test for VCV_SIMV_Mode* VCV_SIMV_Mode::S_GetInstance(void)
TEST_F(VCV_SIMV_ModeUnitMock, S_GetInstance)
{
    MyVCV_SIMV_Mode->S_GetInstance();

    EXPECT_NE(MyVCV_SIMV_Mode->S_Instance, (void*)0);
}

//test for VCV_SIMV_Mode::VCV_SIMV_Mode(void): MixedMode()
TEST_F(VCV_SIMV_ModeUnitMock, MixedMode)
{
    //no source, not need to test
}

//test for void VCV_SIMV_Mode::Enter(E_VentilationMode fromModeId)
TEST_F(VCV_SIMV_ModeUnitMock, Enter)
{
    EXPECT_CALL(*_PhaseMgrPtr, PhaseMgrProcessNewVentType(eVolumeVentilation)).Times(1);

    EXPECT_CALL(*_StepMotor, MgrControllerVCVServoControllerResetIMVStaticVariables()).Times(1);

    EXPECT_CALL(*_PhaseMgrPtr, PhaseMgrResetVolumeAdjustmentOfVCVInhPhase()).Times(1);

    EXPECT_CALL(*_StepMotor, MgrControllerVCVServoControllerEnable()).Times(1);

    EXPECT_CALL(*_PhaseMgrPtr, PhaseMgrProcessNewPhase(eExhPhase)).Times(1);

    MyVCV_SIMV_Mode->Enter(eHardwareErrorMode);
}

#endif //end define BD_MODE_UNIT_TEST
