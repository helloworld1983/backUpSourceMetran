#include "PDFixture.h"

// Modules mocks
std::unique_ptr<BaseModuleMock> PDTestFixture::_modulesMocks;

//add service
std::unique_ptr<FunctionUsedInEvenGroup> PDTestFixture::_EventGroupMock;
std::unique_ptr<FunctionUsedInPdAlarm> PDTestFixture::_PdAlarmMock;
std::unique_ptr<FunctionUsedInPtData> PDTestFixture::_PtDatamMock;
