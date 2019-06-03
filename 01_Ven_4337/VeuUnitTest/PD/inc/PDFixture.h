#ifndef INC_PDFIXTURE_H_
#define INC_PDFIXTURE_H_

#include "gtest.h"
#include "BaseModuleMock.h"

//include to register for class
#include "EventGroupMockFunction.h"
#include "PdAlarmMockFunction.h"
#include "PtDataMockFunction.h"

class PDTestFixture : public ::testing::Test
{
public:
    PDTestFixture(BaseModuleMock *mocks)
    {
        //Default
        _modulesMocks.reset(mocks);

        //Add mock module service
        _EventGroupMock.reset(new ::testing::NiceMock<FunctionUsedInEvenGroup>());
        _PdAlarmMock.reset(new ::testing::NiceMock<FunctionUsedInPdAlarm>());
        _PtDatamMock.reset(new ::testing::NiceMock<FunctionUsedInPtData>());

    }

    ~PDTestFixture(void)
    {
        //Default
        _modulesMocks.reset();

        //Add mock module service
        _EventGroupMock.reset();
        _PdAlarmMock.reset();
        _PtDatamMock.reset();

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
    static std::unique_ptr<FunctionUsedInEvenGroup> _EventGroupMock;
    static std::unique_ptr<FunctionUsedInPdAlarm> _PdAlarmMock;
    static std::unique_ptr<FunctionUsedInPtData> _PtDatamMock;

};

#endif /* INC_PDFIXTURE_H_ */
