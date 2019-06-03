#ifndef INC_IPCFIXTURE_H_
#define INC_IPCFIXTURE_H_

#include "gtest.h"
#include "BaseModuleMock.h"

//include to register for class


class IPCTestFixture : public ::testing::Test
{
public:
    IPCTestFixture(BaseModuleMock *mocks)
    {
        //Default
        _modulesMocks.reset(mocks);

        //Add mock module service
//        _FunctionUsedInTimer.reset(new ::testing::NiceMock<FunctionUsedInTimer>());

    }

    ~IPCTestFixture(void)
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

};




#endif /* INC_IPCFIXTURE_H_ */
