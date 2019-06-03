#ifndef INC_BDFIXTURE_H_
#define INC_BDFIXTURE_H_

#include "gtest.h"
#include "BaseModuleMock.h"

//include to register for class
#include "StepMotorMockFunction.h"
#include "ModeMgrMockFunction.h"
#include "PhaseMgrMockFunction.h"
#include "TriggerMgrMockFunction.h"
#include "BreathMgrMockFunction.h"
#include "AlarmMockFunction.h"
#include "PdTaskMockFunction.h"
#include "TimerMockFunction.h"
#include "MgrBdSettingMockFunction.h"

class BDTestFixture : public ::testing::Test
{
public:
    BDTestFixture(BaseModuleMock *mocks)
    {
        //Default
        _modulesMocks.reset(mocks);

        //Add mock module service
        _StepMotor.reset(new ::testing::NiceMock<StepMotorMockFunction>());
        _PhaseMgrPtr.reset(new ::testing::NiceMock<PhaseMgrMockFunction>());
        _ModeMgr.reset(new ::testing::NiceMock<ModeMgrMockFunction>());
        _TriggerMgr.reset(new ::testing::NiceMock<TriggerMgrMockFunction>());
        _BreathMgr.reset(new ::testing::NiceMock<MgrBreath>());
        _Alarm.reset(new ::testing::NiceMock<AlarmMockFunction>());
        _PdTask.reset(new ::testing::NiceMock<PdMockFunction>());
        _Timer.reset(new ::testing::NiceMock<TimerMockFunction>());
        _MgrBdSetting.reset(new ::testing::NiceMock<MgrBdSettingMockFucntion>());


    }

    ~BDTestFixture(void)
    {
        //Default
        _modulesMocks.reset();

        //Add mock module service
        _StepMotor.reset();
        _PhaseMgrPtr.reset();
        _ModeMgr.reset();
        _TriggerMgr.reset();
        _BreathMgr.reset();
        _Alarm.reset();
        _PdTask.reset();
        _Timer.reset();
        _MgrBdSetting.reset();

    }

//    //Default
//    template<typename T>
//    static T& GetMock()
//    {
//        auto ptr = dynamic_cast<T*>(_modulesMocks.get());
//        if (ptr == nullptr)
//        {
//            auto errMsg = "The test does not provide mock of \"" + std::string(typeid(T).name()) + "\"";
//            throw std::exception(errMsg.c_str());
//        }
//        return *ptr;
//    }

    // Modules mocks - Default
    static std::unique_ptr<BaseModuleMock> _modulesMocks;

    // Add Services to test
    static std::unique_ptr<StepMotorMockFunction> _StepMotor;
    static std::unique_ptr<PhaseMgrMockFunction> _PhaseMgrPtr;
    static std::unique_ptr<ModeMgrMockFunction> _ModeMgr;
    static std::unique_ptr<TriggerMgrMockFunction> _TriggerMgr;
    static std::unique_ptr<MgrBreath> _BreathMgr;
    static std::unique_ptr<AlarmMockFunction> _Alarm;
    static std::unique_ptr<PdMockFunction> _PdTask;
    static std::unique_ptr<TimerMockFunction> _Timer;
    static std::unique_ptr<MgrBdSettingMockFucntion> _MgrBdSetting;


};

#endif /* INC_BDFIXTURE_H_ */
