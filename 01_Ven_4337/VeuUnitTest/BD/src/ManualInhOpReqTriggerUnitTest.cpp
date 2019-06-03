#include "EnableTestCase.h"
#ifdef BD_TRIGGER_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"
#include "ManualInhOpReqTrigger.h"
#include "DebugTrace.h"

using namespace ::testing;

#include "PhaseMgrMockHeader.h"

// define and initialize statics
ManualInhOpReqTrigger* ManualInhOpReqTrigger::S_Instance = NULL;

ManualInhOpReqTrigger* ManualInhOpReqTrigger::S_GetInstance (void)
{
    // If the object has not already been created, create it.
    if(NULL == S_Instance)
    {
        S_Instance = new ManualInhOpReqTrigger ();
        ASSERTION(S_Instance != NULL);
    }

    return (S_Instance);

}   // end S_GetInstance()
ManualInhOpReqTrigger::ManualInhOpReqTrigger (void) :
                        Trigger (eManualInhOpReqTrigger), OpRequest ()
{

}   // end ManualInhOpReqTrigger()
void ManualInhOpReqTrigger::ProcessRequest (E_OpRequestCommand command)
{
    // Check if the request is to start a manual inhalation
    if (eStart == command)
    {
        // Check if the trigger is enabled and the ventilator is in the
        // unrestricted phase of exhalation
        if ((eTrue == IsEnabled) &&
                (ExhPhaseS_IsInUnrestrictedPhase() == eTrue))
        {

            // Set the TriggerDidFire attribute so that the BD Task will start
            // a mandatory inhalation.
            TriggerDidFire = eTrue;

            // Provide feedback to the operator
            AcceptRequest();
        }

        else
        {
            // If the request was received during inhalation or the restricted
            // phase of exhalation, it must be rejected.  Provide feedback
            // to the operator
            RejectRequest();
        }
    }
    else
    {
        // A manual inhalation can only be requested to start.
        ASSERTION(eStart == command);
    }

}   // end ProcessRequest()

class ManualInhOpReqTriggerUnitMock : public BDTestFixture
{
public:
    ManualInhOpReqTriggerUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~ManualInhOpReqTriggerUnitMock(void){}
    virtual void SetUp(void)
    {
        MyManualInhOpReqTrigger = new ManualInhOpReqTrigger();
    }
    virtual void TearDown(void)
    {
        delete MyManualInhOpReqTrigger;
    }
    ManualInhOpReqTrigger *MyManualInhOpReqTrigger;
};

//test for ManualInhOpReqTrigger* ManualInhOpReqTrigger::S_GetInstance (void)
TEST_F(ManualInhOpReqTriggerUnitMock, S_GetInstance)
{
    MyManualInhOpReqTrigger->S_GetInstance();

    EXPECT_NE(MyManualInhOpReqTrigger->S_Instance, (void*)0);
}

//test for ManualInhOpReqTrigger::ManualInhOpReqTrigger (void) :
TEST_F(ManualInhOpReqTriggerUnitMock, ManualInhOpReqTrigger)
{
    //no source, not need to test
}

//test for void ManualInhOpReqTrigger::ProcessRequest (E_OpRequestCommand command)
TEST_F(ManualInhOpReqTriggerUnitMock, ProcessRequest)
{
    MyManualInhOpReqTrigger->IsEnabled = eTrue;
    EXPECT_CALL(*_PhaseMgrPtr, ExhPhaseS_IsInUnrestrictedPhase()).Times(1).WillOnce(Return(eTrue));

    MyManualInhOpReqTrigger->ProcessRequest(eStart);

    EXPECT_EQ(MyManualInhOpReqTrigger->TriggerDidFire, eTrue);
}

#endif //end define BD_TRIGGER_UNIT_TEST
