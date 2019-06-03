#ifndef FIXTURE_H_
#define FIXTURE_H_

#include "gtest.h"
#include "BaseModuleMock.h"
#include "ServiceMockForFuntionNeedToTest.h"


class TestFixture : public ::testing::Test
{
public:
    TestFixture(BaseModuleMock *mocks)
    {
        //Default
        _modulesMocks.reset(mocks);

        //Add mock module service
        _oscillator.reset(new ::testing::NiceMock<OscillatorService>());
        _gpio.reset(new ::testing::NiceMock<GpioService>());

    }

    ~TestFixture(void)
    {
        //Default
        _modulesMocks.reset();

        //Add mock module service
        _oscillator.reset();
        _gpio.reset();

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
    static std::unique_ptr<OscillatorService> _oscillator;
    static std::unique_ptr<GpioService> _gpio;

};

#endif /* FIXTURE_H_ */
