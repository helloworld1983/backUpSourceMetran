#include "EnableTestCase.h"
#ifdef BD_PHASE_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"
#include "MandPressInhPhase.h"
#include "DebugTrace.h"

using namespace ::testing;

#include "TriggerMgrMockHeader.h"
#include "BreathMgrMockHeader.h"
#include "MgrBdSettingMockHeader.h"
#include "ModeMgrMockHeader.h"

MandPressInhPhase::MandPressInhPhase (E_BdSettingId targetPressId,
                                      E_BdSettingId baselinePressId)
     //   PressInhPhase (targetPressId, baselinePressId)
{

}   // end MandPressInhPhase()

void MandPressInhPhase::Start (void)
{
    // Invoke base class method to perform processing common to all
    // pressure based inhalation phases.
//    PressInhPhase::Start ();

    // Enable the Inh Pressure Controller!  Target and Initial pressure are used
    // to set up the gain factors.  Mandatory and assisted breaths
    // are treated the same by the controller, so assume it's mandatory.
//    InhController->Enable(TargetPressure, InitialPressure, eMandatory);

    // Get a handle to the Trigger Manager
//    TriggerMgr* triggerMgrPtr = TriggerMgr::S_GetInstance ();

    // Enable the inhalation time trigger
    triggerMgrPtrEnableTrigger (eMandPressInhTimeTrigger);

    // Enable the Hip2 trigger; Hip1 was enabled in the InhPhase base class.
    triggerMgrPtrEnableTrigger (eHip2AlarmTrigger);

    // Update patient data;  need to know how the breath was triggered
    // to determine whether or not it is a mandatory or assisted breath
    if(ModeS_IsInhPtTriggered () == eTrue)
    {
        MgrBreathDataInhalationStart (eAssisted);
    }
    else
    {
        MgrBreathDataInhalationStart (eMandatory);
    }

}   // end Start()

void MandPressInhPhase::End (void)
{

    // Invoke base class method to do perfrom processing common to all
    // Pressure Based inhalation phases.
//    PressInhPhase::End ();

    // Disable the inhalation time trigger.
    TriggerMgrDisableTrigger (eMandPressInhTimeTrigger);

}   // end End()

class MandPressInhPhaseUnitMock : public BDTestFixture
{
public:
    MandPressInhPhaseUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~MandPressInhPhaseUnitMock(void){}
    virtual void SetUp(void)
    {
        MyMandPressInhPhase = new MandPressInhPhase();
    }
    virtual void TearDown(void)
    {
        delete MyMandPressInhPhase;
    }
    MandPressInhPhase *MyMandPressInhPhase;
};

//test for MandPressInhPhase::MandPressInhPhase
TEST_F(MandPressInhPhaseUnitMock, MandPressInhPhase)
{
    //no source, not need to test
}

//test for void MandPressInhPhase::Start (void)
TEST_F(MandPressInhPhaseUnitMock, Start_0)
{
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger (eMandPressInhTimeTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger (eHip2AlarmTrigger)).Times(1);

    EXPECT_CALL(*_ModeMgr, ModeS_IsInhPtTriggered()).Times(1).WillOnce(Return(eTrue));

    EXPECT_CALL(*_BreathMgr, MgrBreathDataInhalationStart (eAssisted)).Times(1);


    MyMandPressInhPhase->Start();
}
TEST_F(MandPressInhPhaseUnitMock, Start_1)
{
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger (eMandPressInhTimeTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger (eHip2AlarmTrigger)).Times(1);

    EXPECT_CALL(*_ModeMgr, ModeS_IsInhPtTriggered()).Times(1).WillOnce(Return(eFalse));

    EXPECT_CALL(*_BreathMgr, MgrBreathDataInhalationStart (eMandatory)).Times(1);


    MyMandPressInhPhase->Start();
}

//test for void MandPressInhPhase::End (void)
TEST_F(MandPressInhPhaseUnitMock, End)
{
    EXPECT_CALL(*_TriggerMgr, TriggerMgrDisableTrigger (eMandPressInhTimeTrigger)).Times(1);

    MyMandPressInhPhase->End();
}

#endif //end define BD_PHASE_UNIT_TEST
