#include "EnableTestCase.h"
#ifdef BD_PHASE_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"
#include "DiagnosticPhase.h"
#include "DebugTrace.h"

using namespace ::testing;

#include "PhaseMgrMockHeader.h"
#include "TriggerMgrMockHeader.h"
#include "BreathMgrMockHeader.h"
#include "MgrBdSettingMockHeader.h"
#include "StepMotorMockHeader.h"
// Define statics
#define VERIABLE_SIZE   13

DiagnosticPhase* DiagnosticPhase::S_Instance = NULL;

DiagnosticPhase* DiagnosticPhase::S_GetInstance(void)
{
    // If the object has not already been created, create it.
    if (NULL == S_Instance)
    {
        S_Instance = new DiagnosticPhase();
        ASSERTION(S_Instance != NULL);
    }
    return(S_Instance);

}   // end S_GetInstance()

DiagnosticPhase::DiagnosticPhase(void) : Phase()
{
//    EVAPtr = NULL;
    receivedSize = 0;

//    DiagnosticQueuePtr = new Q("DIANOSTIC QUEUE", (20 * (DIAGNOSTIC_Q_MSG_SIZE+1)), VERIABLE_SIZE, DIAGNOSTIC_Q_MSG_SIZE);
//    ASSERTION(DiagnosticQueuePtr != NULL);

//    TheHardwareDiagnostic();

}   // end DiagnosticPhase()

DiagnosticPhase::~DiagnosticPhase()
{
//    delete DiagnosticQueuePtr;
}

void DiagnosticPhase::Start(void)
{
//    EVAPtr = ExhValveAlign::S_GetInstance();
//    ASSERTION( EVAPtr != NULL );

    AirFlowControllerDisable();
    O2FlowControllerDisable();
    ExhControllerDisable();

    // Update Patient Data
    MgrBreathDataNonVentStart();
}   // end Start()
void DiagnosticPhase::End(void)
{
}   // end End()

void DiagnosticPhase::ProcessBdCycle(void)
{
//    STATUS status;

        // get event message
//    status = DiagnosticQueuePtr->Retrieve
//    (
//        &diagnosticMessage,
//        DIAGNOSTIC_Q_MSG_SIZE,
//        &receivedSize
//    );

//    switch (status)
//    {
//        case Q_SUCCESS:
//            // route the message to the Diagnostic class
//            TheHardwareDiagnostic().RouteMessage(diagnosticMessage.eventStruct);
//            break;
//        case QUEUE_EMPTY:
//            break;
//        default:
//            ASSERTION(0);
//    }
//    TheHardwareDiagnostic().Run();
}   // end ProcessBdCycle()
STATUS DiagnosticPhase::Send(CommandData &event)
{
    // This value specifies the size of a Basic Event.
//    const UNSIGNED EVENT_SIZE =
//            (sizeof(CommandData) + sizeof(UNSIGNED) - 1)/sizeof(UNSIGNED);

    //    return (DiagnosticQueuePtr->Send(&event, EVENT_SIZE));
    return 0;
}
STATUS DiagnosticPhase::Send(MotorEventData &event)
{
    // This value specifies the size of a Basic Event.
//    const UNSIGNED EVENT_SIZE =
//            (sizeof(MotorEventData) + sizeof(UNSIGNED) - 1)/sizeof(UNSIGNED);

    //    return (DiagnosticQueuePtr->Send(&event, EVENT_SIZE));
    return 0;
}

class DiagnosticPhaseUnitMock : public BDTestFixture
{
public:
    DiagnosticPhaseUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~DiagnosticPhaseUnitMock(void){}
    virtual void SetUp(void)
    {
        MyDiagnosticPhase = new DiagnosticPhase();
    }
    virtual void TearDown(void)
    {
        delete MyDiagnosticPhase;
    }
    DiagnosticPhase *MyDiagnosticPhase;
};

//test for DiagnosticPhase* DiagnosticPhase::S_GetInstance(void)
TEST_F(DiagnosticPhaseUnitMock, S_GetInstance)
{
    MyDiagnosticPhase->S_GetInstance();

    EXPECT_NE(MyDiagnosticPhase->S_Instance, (void*)0);
}

//test for DiagnosticPhase::DiagnosticPhase(void) : Phase()
TEST_F(DiagnosticPhaseUnitMock, DiagnosticPhase)
{
    EXPECT_EQ(MyDiagnosticPhase->receivedSize, 0);
}

//test for DiagnosticPhase::~DiagnosticPhase()
TEST_F(DiagnosticPhaseUnitMock, DiagnosticPhaseDes)
{
    //no source, not need to test
}

//test for void DiagnosticPhase::Start(void)
TEST_F(DiagnosticPhaseUnitMock, Start)
{
    EXPECT_CALL(*_StepMotor, AirFlowControllerDisable()).Times(1);
    EXPECT_CALL(*_StepMotor, O2FlowControllerDisable()).Times(1);
    EXPECT_CALL(*_StepMotor, ExhControllerDisable()).Times(1);

    EXPECT_CALL(*_BreathMgr, MgrBreathDataNonVentStart()).Times(1);

    MyDiagnosticPhase->Start();
}

//test for void DiagnosticPhase::End(void)
TEST_F(DiagnosticPhaseUnitMock, End)
{
    //no source, not need to test
}

//test for void DiagnosticPhase::ProcessBdCycle(void)
TEST_F(DiagnosticPhaseUnitMock, ProcessBdCycle)
{
    //no source, not need to test
}

//test for STATUS DiagnosticPhase::Send(CommandData &event)
TEST_F(DiagnosticPhaseUnitMock, Send_CommandData)
{
    //no source, not need to test
}

//test for STATUS DiagnosticPhase::Send(MotorEventData &event)
TEST_F(DiagnosticPhaseUnitMock, Send_MotorEventData)
{
    //no source, not need to test
}

#endif //end define BD_PHASE_UNIT_TEST
