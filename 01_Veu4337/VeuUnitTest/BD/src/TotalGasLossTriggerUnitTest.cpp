
#include "EnableTestCase.h"
#ifdef BD_TRIGGER_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"
#include "TotalGasLossTrigger.h"
#include "DebugTrace.h"
#include "PhaseMgrMockHeader.h"
#include "MgrBdSettingMockHeader.h"
#include "AlarmMockHeader.h"

using namespace ::testing;
TotalGasLossTrigger* TotalGasLossTrigger::S_Instance = NULL;

TotalGasLossTrigger::TotalGasLossTrigger(void)
    : Trigger(eTotalGasLossTrigger), gasLossDuration(0)
{

}   // end TotalGasLossTrigger()
E_Bool TotalGasLossTrigger::CheckCondition(void)
{
    if ( (eTrue == O2SettingO2SourceFault()) &&
         (eTrue == O2SettingAirSourceFault()) )
    {
        TriggerDidFire = eTrue;
    }
    return (TriggerDidFire);
}   // end CheckCondition()
TotalGasLossTrigger* TotalGasLossTrigger::S_GetInstance(void)
{
    // If the object has not already been created, create it.
    if (NULL == S_Instance)
    {
        S_Instance = new TotalGasLossTrigger();
        ASSERTION(S_Instance != NULL);
    }

    return(S_Instance);

}   // end TotalGasLossTrigger()

class TotalGasLossTriggerUnitMock : public BDTestFixture
{
public:
    TotalGasLossTriggerUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~TotalGasLossTriggerUnitMock(void){}
    virtual void SetUp(void)
    {
        MyTotalGasLossTrigger = new TotalGasLossTrigger();
    }
    virtual void TearDown(void)
    {
        delete MyTotalGasLossTrigger;
    }
    TotalGasLossTrigger *MyTotalGasLossTrigger;
};

//test for TotalGasLossTrigger::TotalGasLossTrigger(void)
TEST_F(TotalGasLossTriggerUnitMock, TotalGasLossTrigger)
{
    //no source, not need to test
}

//test for E_Bool TotalGasLossTrigger::CheckCondition(void)
TEST_F(TotalGasLossTriggerUnitMock, CheckCondition)
{
    MyTotalGasLossTrigger->TriggerDidFire = eFalse;

    EXPECT_CALL(*_Alarm, O2SettingO2SourceFault()).Times(1).WillOnce(Return(eTrue));
    EXPECT_CALL(*_MgrBdSetting, O2SettingAirSourceFault()).Times(1).WillOnce(Return(eTrue));

    EXPECT_EQ(MyTotalGasLossTrigger->CheckCondition(), eTrue);
}

//test for TotalGasLossTrigger* TotalGasLossTrigger::S_GetInstance(void)
TEST_F(TotalGasLossTriggerUnitMock, S_GetInstance)
{
    MyTotalGasLossTrigger->S_GetInstance();

    EXPECT_NE(MyTotalGasLossTrigger->S_Instance, (void*)0);
}

#endif //end define BD_TRIGGER_UNIT_TEST
