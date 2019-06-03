#ifndef INC_ALARMFIXTURE_H_
#define INC_ALARMFIXTURE_H_

#include "gtest.h"
#include "BaseModuleMock.h"

//include to register for class
#include "MockFunctionForAlarmControl.h"

class AlarmTestFixture : public ::testing::Test
{
public:
    AlarmTestFixture(BaseModuleMock *mocks)
    {
        //Default
        _modulesMocks.reset(mocks);

        //Add mock module service
        _AlarmControlMocks.reset(new ::testing::NiceMock<AlarmControlService>());

    }

    ~AlarmTestFixture(void)
    {
        //Default
        _modulesMocks.reset();

        //Add mock module service
        _AlarmControlMocks.reset();

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
    static std::unique_ptr<AlarmControlService> _AlarmControlMocks;

};

#endif /* INC_ALARMFIXTURE_H_ */
