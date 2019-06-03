#include "EnableTestCase.h"
#ifdef BD_MODE_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"
#include "MixedMode.h"
#include "DebugTrace.h"

using namespace ::testing;

#include "PhaseMgrMockHeader.h"
#include "TriggerMgrMockHeader.h"

MixedMode* MixedMode::S_Instance = NULL;

MixedMode* MixedMode::S_GetInstance (void)
{
    // If the pointer to the instance is NULL, create the object.
    if(NULL == S_Instance)
    {
        S_Instance = new MixedMode ();
        ASSERTION(S_Instance != NULL);
    }

    return (S_Instance);

}   // end S_GetInstance()
MixedMode::MixedMode (void) :
        NormBreathingMode ()
{
    Window = eMandWindow;
}   // end MixedMode()
void MixedMode::Enter (E_VentilationMode fromModeId)
{

    UNSIGNED timeSinceInhStart = 0;     // time elaspsed since start of last inh
    UNSIGNED breathPeriod;          // current breath period

    Window = eMandWindow;

    // Initialize pointers to the TriggerMgr object and the PhaseMgr object
//    TriggerMgr* triggerMgrPtr = TriggerMgr::S_GetInstance ();
//    PhaseMgr* phaseMgrPtr = PhaseMgr::S_GetInstance ();

    // Enable triggers common to all breathing modes such as the Apnea Time
    // trigger and the Mode change trigger.
//    UpdateCommonTriggers ();

    // Enable the exhalation hold request trigger.
    triggerMgrPtrEnableTrigger (eExhHoldOpReqTrigger);

    // Set the window to mandatory.  Any breath triggered while the Mandatory
    // window is open will be a mandatory breath.
    Window = eMandWindow;

    // Determine processing based upon the mode that is being exited
    switch (fromModeId)
    {
        case eStandbyMode:
        case eOcclusionMode:
        case eNoGasMode:

            // When entering the SIMV or Spont/Timed Mode from Start-Up,
            // go right into a mandatory inhalation.
            phaseMgrPtrProcessNewPhase (eMandInhPhase);

            // Enable the Breath Period time trigger.  Since a time interval
            // is not passed to the EnableTrigger() method, the trigger will
            // enable the breath period timer based on the current respiratory
            // rate setting
            triggerMgrPtrEnableTrigger (eBpTimeTrigger);

            break;

        case eSpontaneousMode:
        case eApneaMode:
        case eMixedMode:

            // The mode change should only occur in exhalation.
            ASSERTION(phaseMgrPtr->GetCurrentPhaseId() == eExhPhase);

            // Determine the time elasped since the start of the last
            // inhalation phase
//            timeSinceInhStart = InhPhase::S_CalcTimeSincePhaseStart ();

            // The value associated with the Breath Period Time Trigger is
            // the breath period.  Retrieve it.
            breathPeriod = triggerMgrPtrGetTimePeriod (eBpTimeTrigger);

            // When entering the Mixed Mode from any breathing mode,
            // determine the time elapsed since last inhalation.
            if(timeSinceInhStart >= breathPeriod)
            {
                // If the time elapsed exceeds the breath period, transition
                // to inhalation right away.
                phaseMgrPtrProcessNewPhase (eMandInhPhase);

                // Enable the Breath Period time trigger.  Since no
                // parameters are being passed, the trigger will enable
                // the breath period timer based on the current respiratory
                // rate setting
                triggerMgrPtrEnableTrigger (eBpTimeTrigger);
            }
            else
            {
                // If the time elapsed since the start of the last
                // inhalation is less than the breath period, enable the
                // trigger for the difference between the breath period and
                // the time elapsed since the start of the last inhlation.
                triggerMgrPtrEnableTriggerWith (eBpTimeTrigger, 0);
            }

            break;

        default:
            // Illegal mode
            ASSERTION(eFalse);
            break;
    }

}   // end Enter()
void MixedMode::ProcessPtInhTrigger (void)
{

    // Initialize pointer to the PhaseMgr object
//    PhaseMgr* phaseMgrPtr = PhaseMgr::S_GetInstance ();

    // A patient trigger should only fire during the exhalation phase.
    ASSERTION(phaseMgrPtr->GetCurrentPhaseId() == eExhPhase);

    // Start a new inhalation.  The type of inhalation (mand or
    // spont) depends upon the window that is currently
    // open.  If the mandatory window is open, deliver a mand
    // breath and close the mandatory window since this breath
    // will meet the mandatory breath criteria for this breath
    // period
    if(eMandWindow == Window)
    {
        // Start a mandatory inhalation
        phaseMgrPtrProcessNewPhase (eMandInhPhase);

        // Close the mandatory window; open the spontaneous window.
        Window = eSpontWindow;
    }

    else if(eSpontWindow == Window)
    {
        // If the spontaneous window is open, deliver a spontaneous
        // inhalation.  The spontaneous window remains open until the
        // breath period timer elapses.
        phaseMgrPtrProcessNewPhase (eSpontInhPhase);
    }
    else
    {
        // Window contains illegal value
        ASSERTION((Window == eMandWindow) || (Window == eSpontWindow));
    }

}   // end ProcessPtInhTrigger()
void MixedMode::ProcessBpTimeTrigger (void)
{

    E_PhaseId phaseId;          // Current Phase ID
//    PhaseMgr* phaseMgrPtr;      // Pointer to the PhaseMgr object

    switch (Window)
    {
        case eMandWindow:
            // Initialize pointer to the PhaseMgr object
//            phaseMgrPtr = PhaseMgr::S_GetInstance ();

            // Make sure the ventilator is in the exhalation phase.
            phaseId = phaseMgrPtrGetCurrentPhaseId ();
            ASSERTION(phaseId == eExhPhase);
            phaseMgrPtrProcessNewPhase (eMandInhPhase);
            break;

        case eSpontWindow:
            // If the breath period fires while the spontaneous window is open,
            // the ventilator just re-opens the mandatory window.
            Window = eMandWindow;
            break;

        default:
            // Trigger fired in wrong window
            ASSERTION((Window == eSpontWindow) || (Window == eMandWindow));
            break;
    }

}   // end ProcessBpTimeTrigger()
void MixedMode::ProcessManualInhOpReqTrigger (void)
{
    // Initialize pointer to the PhaseMgr object
//    PhaseMgr* phaseMgrPtr = PhaseMgr::S_GetInstance ();

    // This trigger should only fire during the exhalation phase.
    ASSERTION(phaseMgrPtr->GetCurrentPhaseId() == eExhPhase);

    // Start a mandatory inhalation
    phaseMgrPtrProcessNewPhase (eMandInhPhase);

    // An operator initiated manual inhalation while the mand
    // window is open causes that window to close and the spont
    // window to open.  If the spont window is already open,
    // it remains open.
    Window = eSpontWindow;

}   // end ProcessManualInhOpReqTrigger()

void MixedMode::ProcessPtInhTriggerAC (void)
{
    // Initialize pointer to the PhaseMgr object
//    PhaseMgr* phaseMgrPtr = PhaseMgr::S_GetInstance();

    // A patient trigger should only fire during the exhalation phase.
    ASSERTION(phaseMgrPtr->GetCurrentPhaseId() == eExhPhase);


    // Start a mandatory inhalation
    phaseMgrPtrProcessNewPhase(eMandInhPhase);

    // Close the mandatory window; open the spontaneous window.
    Window = eMandWindow;
    triggerMgrPtrDisableTrigger(eBpTimeTrigger);
    UNSIGNED breathPeriod = triggerMgrPtrGetTimePeriod(eBpTimeTrigger);

    triggerMgrPtrEnableTriggerWith(eBpTimeTrigger,breathPeriod);
}
void MixedMode::ProcessExhHoldTrigger (void)
{

    // Initialize pointer to the PhaseMgr object
//    PhaseMgr* phaseMgrPtr = PhaseMgr::S_GetInstance ();

    // This trigger should only occur in the exhalation hold phase.
    ASSERTION(phaseMgrPtr->GetCurrentPhaseId() == eExhHoldPhase);

    // Transition to the mandatory inhalation phase since a mandatory
    // inhalation was being held off to perform the hold.
    triggerMgrPtrEnableTrigger(eBpTimeTrigger);
    phaseMgrPtrProcessNewPhase (eMandInhPhase);

}   // end ProcessExhHoldTrigger()
void MixedMode::ProcessTriggerID (E_TriggerId id)
{
    switch (id)
    {
        case ePtInhTrigger:
            ProcessPtInhTrigger();
            break;

        case eBpTimeTrigger:
            ProcessBpTimeTrigger();
            break;

        case eManualInhOpReqTrigger:
            ProcessManualInhOpReqTrigger();
            break;

        case eExhHoldTrigger:
            ProcessExhHoldTrigger();
            break;

        default:
            ASSERTION(eFalse);
            break;
    }
}

E_WindowType MixedMode::GetWindow(void)
{
    return Window;
}

class MixedModeUnitMock : public BDTestFixture
{
public:
    MixedModeUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~MixedModeUnitMock(void){}
    virtual void SetUp(void)
    {
        MyMixedMode = new MixedMode();
    }
    virtual void TearDown(void)
    {
        delete MyMixedMode;
    }
    MixedMode *MyMixedMode;
};

//test for MixedMode* MixedMode::S_GetInstance (void)
TEST_F(MixedModeUnitMock, S_GetInstance)
{
    MyMixedMode->S_GetInstance();

    EXPECT_NE(MyMixedMode->S_Instance, (void*)0);
}

//test for MixedMode::MixedMode (void) :
TEST_F(MixedModeUnitMock, MixedMode)
{
    EXPECT_EQ(MyMixedMode->Window, eMandWindow);
}

//test for void MixedMode::Enter (E_VentilationMode fromModeId)
TEST_F(MixedModeUnitMock, Enter_0)
{
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger (eExhHoldOpReqTrigger)).Times(1);

    EXPECT_CALL(*_PhaseMgrPtr, phaseMgrPtrProcessNewPhase (eMandInhPhase)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger (eBpTimeTrigger)).Times(1);

    MyMixedMode->Enter(eStandbyMode);
}
TEST_F(MixedModeUnitMock, Enter_1)
{
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger (eExhHoldOpReqTrigger)).Times(1);

    EXPECT_CALL(*_PhaseMgrPtr, phaseMgrPtrProcessNewPhase (eMandInhPhase)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger (eBpTimeTrigger)).Times(1);

    MyMixedMode->Enter(eOcclusionMode);
}
TEST_F(MixedModeUnitMock, Enter_2)
{
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger (eExhHoldOpReqTrigger)).Times(1);

    EXPECT_CALL(*_PhaseMgrPtr, phaseMgrPtrProcessNewPhase (eMandInhPhase)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger (eBpTimeTrigger)).Times(1);

    MyMixedMode->Enter(eNoGasMode);
}
TEST_F(MixedModeUnitMock, Enter_3)
{
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger (eExhHoldOpReqTrigger)).Times(1);

    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrGetTimePeriod (eBpTimeTrigger)).Times(1).WillOnce(Return(0));

    EXPECT_CALL(*_PhaseMgrPtr, phaseMgrPtrProcessNewPhase (eMandInhPhase)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger (eBpTimeTrigger)).Times(1);

    MyMixedMode->Enter(eSpontaneousMode);
}
TEST_F(MixedModeUnitMock, Enter_4)
{
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger (eExhHoldOpReqTrigger)).Times(1);

    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrGetTimePeriod (eBpTimeTrigger)).Times(1).WillOnce(Return(0));

    EXPECT_CALL(*_PhaseMgrPtr, phaseMgrPtrProcessNewPhase (eMandInhPhase)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger (eBpTimeTrigger)).Times(1);

    MyMixedMode->Enter(eApneaMode);
}
TEST_F(MixedModeUnitMock, Enter_5)
{
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger (eExhHoldOpReqTrigger)).Times(1);

    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrGetTimePeriod (eBpTimeTrigger)).Times(1).WillOnce(Return(0));

    EXPECT_CALL(*_PhaseMgrPtr, phaseMgrPtrProcessNewPhase (eMandInhPhase)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger (eBpTimeTrigger)).Times(1);

    MyMixedMode->Enter(eMixedMode);
}
TEST_F(MixedModeUnitMock, Enter_6)
{
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger (eExhHoldOpReqTrigger)).Times(1);

    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrGetTimePeriod (eBpTimeTrigger)).Times(1).WillOnce(Return(3));

    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTriggerWith (eBpTimeTrigger, 0)).Times(1);

    MyMixedMode->Enter(eSpontaneousMode);
}
TEST_F(MixedModeUnitMock, Enter_7)
{
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger (eExhHoldOpReqTrigger)).Times(1);

    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrGetTimePeriod (eBpTimeTrigger)).Times(1).WillOnce(Return(3));

    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTriggerWith (eBpTimeTrigger, 0)).Times(1);

    MyMixedMode->Enter(eApneaMode);
}
TEST_F(MixedModeUnitMock, Enter_8)
{
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger (eExhHoldOpReqTrigger)).Times(1);

    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrGetTimePeriod (eBpTimeTrigger)).Times(1).WillOnce(Return(3));

    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTriggerWith (eBpTimeTrigger, 0)).Times(1);

    MyMixedMode->Enter(eMixedMode);
}

//test for void MixedMode::ProcessPtInhTrigger (void)
TEST_F(MixedModeUnitMock, ProcessPtInhTrigger_0)
{
    MyMixedMode->Window = eMandWindow;

    EXPECT_CALL(*_PhaseMgrPtr, phaseMgrPtrProcessNewPhase (eMandInhPhase)).Times(1);

    MyMixedMode->ProcessPtInhTrigger();

    EXPECT_EQ(MyMixedMode->Window, eSpontWindow);
}
TEST_F(MixedModeUnitMock, ProcessPtInhTrigger_1)
{
    MyMixedMode->Window = eSpontWindow;

    EXPECT_CALL(*_PhaseMgrPtr, phaseMgrPtrProcessNewPhase (eSpontInhPhase)).Times(1);

    MyMixedMode->ProcessPtInhTrigger();
}

//test for void MixedMode::ProcessManualInhOpReqTrigger (void)
TEST_F(MixedModeUnitMock, ProcessManualInhOpReqTrigger)
{
    EXPECT_CALL(*_PhaseMgrPtr, phaseMgrPtrProcessNewPhase (eMandInhPhase)).Times(1);

    MyMixedMode->ProcessManualInhOpReqTrigger();

    EXPECT_EQ(MyMixedMode->Window, eSpontWindow);
}

//test for void MixedMode::ProcessPtInhTriggerAC (void)
TEST_F(MixedModeUnitMock, ProcessPtInhTriggerAC)
{
    EXPECT_CALL(*_PhaseMgrPtr,phaseMgrPtrProcessNewPhase(eMandInhPhase)).Times(1);
    EXPECT_CALL(*_TriggerMgr,triggerMgrPtrDisableTrigger(eBpTimeTrigger)).Times(1);

    EXPECT_CALL(*_TriggerMgr,triggerMgrPtrGetTimePeriod(eBpTimeTrigger)).Times(1).WillOnce(Return(3));

    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTriggerWith(eBpTimeTrigger,3)).Times(1);

    MyMixedMode->ProcessPtInhTriggerAC();
    EXPECT_EQ(MyMixedMode->Window, eMandWindow);

}

//test for void MixedMode::ProcessExhHoldTrigger (void)
TEST_F(MixedModeUnitMock, ProcessExhHoldTrigger)
{
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger(eBpTimeTrigger)). Times(1);

    EXPECT_CALL(*_PhaseMgrPtr, phaseMgrPtrProcessNewPhase (eMandInhPhase)). Times(1);

    MyMixedMode->ProcessExhHoldTrigger();
}

//test for void MixedMode::ProcessTriggerID (E_TriggerId id)
TEST_F(MixedModeUnitMock, ProcessTriggerID)
{
    //all calling fucntion is tested, not need to test
}

//test for E_WindowType MixedMode::GetWindow(void)
TEST_F(MixedModeUnitMock, GetWindow)
{
    MyMixedMode->Window =  eMandWindow;

    EXPECT_EQ(MyMixedMode->Window, eMandWindow);
}
#endif //end define BD_MODE_UNIT_TEST
