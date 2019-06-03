#include "EnableTestCase.h"
#ifdef BD_MODE_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"
#include "PCV_CPAP_Mode.h"
#include "DebugTrace.h"

#include "PhaseMgrMockHeader.h"
#include "StepMotorMockHeader.h"

using namespace ::testing;

PCV_CPAP_Mode* PCV_CPAP_Mode::S_Instance = NULL;
PCV_CPAP_Mode* PCV_CPAP_Mode::S_GetInstance(void)
{
    // If the pointer to the instance is NULL, create the object.
    if (NULL == S_Instance)
    {
        S_Instance = new PCV_CPAP_Mode();
        ASSERTION(S_Instance != NULL);
    }

    return(S_Instance);

}
PCV_CPAP_Mode::PCV_CPAP_Mode(void): SpontMode()
{
}
void PCV_CPAP_Mode::Enter(E_VentilationMode fromModeId)
{
    PhaseMgrProcessNewVentType(ePressureVentilation);
    if(fromModeId != eApneaBackupMode)
    {
        MgrControllerCMVServoControllerResetIMVStaticVariables();
    }

//    SpontMode::Enter(fromModeId);
    MgrControllerCMVServoControllerEnable();

    PhaseMgrProcessNewPhase(eExhPhase);
}

class PCV_CPAP_ModeUnitMock : public BDTestFixture
{
public:
    PCV_CPAP_ModeUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~PCV_CPAP_ModeUnitMock(void){}
    virtual void SetUp(void)
    {
        MyPCV_CPAP_Mode = new PCV_CPAP_Mode();
    }
    virtual void TearDown(void)
    {
        delete MyPCV_CPAP_Mode;
    }
    PCV_CPAP_Mode *MyPCV_CPAP_Mode;
};

//test for PCV_CPAP_Mode* PCV_CPAP_Mode::S_GetInstance(void)
TEST_F(PCV_CPAP_ModeUnitMock, S_GetInstance)
{
    MyPCV_CPAP_Mode->S_GetInstance();

    EXPECT_NE(MyPCV_CPAP_Mode->S_Instance, (void*)0);
}

//test for PCV_CPAP_Mode::PCV_CPAP_Mode(void): SpontMode()
TEST_F(PCV_CPAP_ModeUnitMock, PCV_CPAP_Mode)
{
    //no source, not need to test
}

//test for void PCV_CPAP_Mode::Enter(E_VentilationMode fromModeId)
TEST_F(PCV_CPAP_ModeUnitMock, Enter)
{
    EXPECT_CALL(*_PhaseMgrPtr, PhaseMgrProcessNewVentType(ePressureVentilation)).Times(1);

    EXPECT_CALL(*_StepMotor, MgrControllerCMVServoControllerResetIMVStaticVariables()).Times(1);
    EXPECT_CALL(*_StepMotor, MgrControllerCMVServoControllerEnable()).Times(1);

    EXPECT_CALL(*_PhaseMgrPtr, PhaseMgrProcessNewPhase(eExhPhase)).Times(1);

    MyPCV_CPAP_Mode->Enter(eHardwareErrorMode);
}

#endif //end define BD_MODE_UNIT_TEST
