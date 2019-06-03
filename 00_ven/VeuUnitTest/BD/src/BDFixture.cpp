#include "BDFixture.h"

// Modules mocks
std::unique_ptr<BaseModuleMock> BDTestFixture::_modulesMocks;

//Add service
std::unique_ptr<StepMotorMockFunction> BDTestFixture::_StepMotor;
std::unique_ptr<PhaseMgrMockFunction> BDTestFixture::_PhaseMgrPtr;
std::unique_ptr<ModeMgrMockFunction> BDTestFixture::_ModeMgr;
std::unique_ptr<TriggerMgrMockFunction> BDTestFixture::_TriggerMgr;
std::unique_ptr<MgrBreath> BDTestFixture::_BreathMgr;
std::unique_ptr<AlarmMockFunction> BDTestFixture::_Alarm;
std::unique_ptr<PdMockFunction> BDTestFixture::_PdTask;
std::unique_ptr<TimerMockFunction> BDTestFixture::_Timer;
std::unique_ptr<MgrBdSettingMockFucntion> BDTestFixture::_MgrBdSetting;
