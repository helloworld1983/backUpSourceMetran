#ifndef INC_CONTROLFIXTURE_H_
#define INC_CONTROLFIXTURE_H_

#include "gtest.h"
#include "BaseModuleMock.h"

//include to register for Timer class



class ControlTestFixture : public ::testing::Test
{
public:
    ControlTestFixture(BaseModuleMock *mocks)
    {
        //Default
        _modulesMocks.reset(mocks);

        //Add mock module service
//        _FunctionUsedInTimer.reset(new ::testing::NiceMock<FunctionUsedInTimer>());

    }

    ~ControlTestFixture(void)
    {
        //Default
        _modulesMocks.reset();

        //Add mock module service
//        _FunctionUsedInTimer.reset();

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
//    static std::unique_ptr<FunctionUsedInTimer> _FunctionUsedInTimer;

};

#endif /* INC_CONTROLFIXTURE_H_ */
