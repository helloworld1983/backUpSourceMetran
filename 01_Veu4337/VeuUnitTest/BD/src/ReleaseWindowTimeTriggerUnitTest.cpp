#include "EnableTestCase.h"
#ifdef BD_TRIGGER_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"
#include "ReleaseWindowTimeTrigger.h"
#include "DebugTrace.h"

using namespace ::testing;

ReleaseWindowTimeTrigger::ReleaseWindowTimeTrigger()
{

}

class ReleaseWindowTimeTriggerUnitMock : public BDTestFixture
{
public:
    ReleaseWindowTimeTriggerUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~ReleaseWindowTimeTriggerUnitMock(void){}
    virtual void SetUp(void)
    {
        MyReleaseWindowTimeTrigger = new ReleaseWindowTimeTrigger();
    }
    virtual void TearDown(void)
    {
        delete MyReleaseWindowTimeTrigger;
    }
    ReleaseWindowTimeTrigger *MyReleaseWindowTimeTrigger;
};

//test for ReleaseWindowTimeTrigger::ReleaseWindowTimeTrigger()
TEST_F(ReleaseWindowTimeTriggerUnitMock, ReleaseWindowTimeTrigger)
{
    //no soruce, not need to test
}

#endif //end define BD_TRIGGER_UNIT_TEST
