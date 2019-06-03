#include "EnableTestCase.h"
#ifdef BD_PHASE_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"
#include "Phase.h"
#include "DebugTrace.h"

using namespace ::testing;

class PhaseUnitMock : public BDTestFixture
{
public:
    PhaseUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~PhaseUnitMock(void){}
    virtual void SetUp(void)
    {
        MyPhase = new Phase();
    }
    virtual void TearDown(void)
    {
        delete MyPhase;
    }
    Phase *MyPhase;
};

//all calling function is pure virtual function, not need to test

#endif //end define BD_PHASE_UNIT_TEST
