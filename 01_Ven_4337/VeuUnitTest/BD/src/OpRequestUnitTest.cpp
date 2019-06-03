#include "EnableTestCase.h"
#ifdef BD_TRIGGER_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"
#include "OpRequest.h"
#include "DebugTrace.h"

using namespace ::testing;

void OpRequest::AcceptRequest(E_State state)
{

    // If there is an LED associated with this operator request, update its
    // state.
//    if (NULL != LedPtr)
//    {
//        LedPtr->SetState(state);
//    }

    // Provide audible feedback that the request was accepted.
//    (Audible::S_GetInstance())->SetFeedback(eGoodBeep);

}   // AcceptRequest()
void OpRequest::CompleteRequest (void)
{
    // If there is an LED associated with this request, extinguish it.
//    if (NULL != LedPtr)
//    {
//        LedPtr->SetState(eOff);
//    }

}   // end CompleteRequest()
void OpRequest::RejectRequest (void)
{

//    (Audible::S_GetInstance())->SetFeedback(eBadBeep);

}   // RejectRequest()
OpRequest::OpRequest (void)
{
}

void OpRequest::ProcessRequest (E_OpRequestCommand command)
{
}

class OpRequestUnitMock : public BDTestFixture
{
public:
    OpRequestUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~OpRequestUnitMock(void){}
    virtual void SetUp(void)
    {
        MyOpRequest = new OpRequest();
    }
    virtual void TearDown(void)
    {
        delete MyOpRequest;
    }
    OpRequest *MyOpRequest;
};

//test for void OpRequest::AcceptRequest(E_State state)
TEST_F(OpRequestUnitMock, AcceptRequest)
{
    //no source, not need to test
}

//test for void OpRequest::CompleteRequest (void)
TEST_F(OpRequestUnitMock, CompleteRequest)
{
    //no source, not need to test
}

//test for void OpRequest::RejectRequest (void)
TEST_F(OpRequestUnitMock, RejectRequest)
{
    //no source, not need to test
}

//test for OpRequest::OpRequest (void)
TEST_F(OpRequestUnitMock, OpRequest)
{
    //no source, not need to test
}

//test for void OpRequest::ProcessRequest (E_OpRequestCommand command)
TEST_F(OpRequestUnitMock, ProcessRequest)
{
    //pure virtual function, not need to test
}

#endif //end define BD_TRIGGER_UNIT_TEST
