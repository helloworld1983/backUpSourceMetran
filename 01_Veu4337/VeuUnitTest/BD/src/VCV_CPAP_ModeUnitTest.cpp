#include "EnableTestCase.h"
#ifdef BD_MODE_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"
#include "VCV_CPAP_Mode.h"
#include "DebugTrace.h"

using namespace ::testing;

#include "ModeMgrMockHeader.h"
#include "PhaseMgrMockHeader.h"
#include "TriggerMgrMockHeader.h"
#include "StepMotorMockHeader.h"

VCV_CPAP_Mode* VCV_CPAP_Mode::S_Instance = NULL;

VCV_CPAP_Mode* VCV_CPAP_Mode::S_GetInstance(void)
{
    // If the pointer to the instance is NULL, create the object.
    if (NULL == S_Instance)
    {
        S_Instance = new VCV_CPAP_Mode();
        ASSERTION(S_Instance != NULL);
    }

    return(S_Instance);

}
VCV_CPAP_Mode::VCV_CPAP_Mode(void) : SpontMode()
{
}
void VCV_CPAP_Mode::Enter(E_VentilationMode fromModeId)
{
    PhaseMgrProcessNewVentType(eVolumeVentilation);
//    SpontMode::Enter(fromModeId);

    if(fromModeId != eApneaBackupMode)
    {
        MgrControllerVCVServoControllerResetIMVStaticVariables();
        PhaseMgrResetVolumeAdjustmentOfVCVInhPhase();
    }

    MgrControllerVCVServoControllerEnable();
    PhaseMgrProcessNewPhase(eExhPhase);
}

class VCV_CPAP_ModeUnitMock : public BDTestFixture
{
public:
    VCV_CPAP_ModeUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~VCV_CPAP_ModeUnitMock(void){}
    virtual void SetUp(void)
    {
        MyVCV_CPAP_Mode = new VCV_CPAP_Mode();
    }
    virtual void TearDown(void)
    {
        delete MyVCV_CPAP_Mode;
    }
    VCV_CPAP_Mode *MyVCV_CPAP_Mode;
};

//test for VCV_CPAP_Mode* VCV_CPAP_Mode::S_GetInstance(void)
TEST_F(VCV_CPAP_ModeUnitMock, S_GetInstance)
{
    MyVCV_CPAP_Mode->S_GetInstance();

    EXPECT_NE(MyVCV_CPAP_Mode->S_Instance, (void*)0);
}

//test for VCV_CPAP_Mode::VCV_CPAP_Mode(void) : SpontMode()
TEST_F(VCV_CPAP_ModeUnitMock, VCV_CPAP_Mode)
{
    //no source, not need to test
}

//test for void VCV_CPAP_Mode::Enter(E_VentilationMode fromModeId)
TEST_F(VCV_CPAP_ModeUnitMock, Enter)
{
    EXPECT_CALL(*_PhaseMgrPtr, PhaseMgrProcessNewVentType(eVolumeVentilation)).Times(1);

    EXPECT_CALL(*_StepMotor, MgrControllerVCVServoControllerResetIMVStaticVariables()).Times(1);

    EXPECT_CALL(*_PhaseMgrPtr, PhaseMgrResetVolumeAdjustmentOfVCVInhPhase()).Times(1);

    EXPECT_CALL(*_StepMotor, MgrControllerVCVServoControllerEnable()).Times(1);

    EXPECT_CALL(*_PhaseMgrPtr, PhaseMgrProcessNewPhase(eExhPhase)).Times(1);

    MyVCV_CPAP_Mode->Enter(eHardwareErrorMode);
}

#endif //end define BD_MODE_UNIT_TEST
