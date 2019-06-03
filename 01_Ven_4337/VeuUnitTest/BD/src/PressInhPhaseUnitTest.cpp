#include "EnableTestCase.h"
#ifdef BD_PHASE_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"
#include "PressInhPhase.h"
#include "DebugTrace.h"

using namespace ::testing;

#include "BreathMgrMockHeader.h"
#include "MgrBdSettingMockHeader.h"
#include "ModeMgrMockHeader.h"

static const UNSIGNED RT_CUTOFF = 150 / MS_PER_TICK;     // 150 ms in ticks

static const UNSIGNED CONTROL_CUTOFF = 17 * SCALE;

PressInhPhase::PressInhPhase (E_BdSettingId inhPressId,
                              E_BdSettingId baselinePressId) :
        InhPhase (), InhPressId (inhPressId), BaselinePressId (baselinePressId)
{
    BaseLine = -100;
    TargetPressure = 2;
    RiseTime = 0;

}   // end PressInhPhase()
void PressInhPhase::Start (void)
{
    // Set up a pointer to the BD Setting Manager
//    MgrBdSetting* mgrBdSettingPtr = MgrBdSetting::S_GetInstance();

    MgrBdSettingPhaseInAllSettings();

    // Retrieve the baseline pressure setting for the exhalation phase that
    // has just ended (before phasing in the settings).  This is the starting
    // point for the breath about to start.  This value is used to determine
    // the pressure command given to the pressure controller during inhalation
    //    InitialPressure = BDmgrBdSettingPtrGetCurrentValue(eBaselinePress);

    // Invoke base class method to perform processing common to all inhalation
    // phases.  This call must be made before accessing setting values since
    // it phases in the settings at the start of the phase.
//    InhPhase::Start();

    // Get the operator set pressure control or support level, the offset
    // pressure and the baseline pressure.  Offset pressure is 0 for NPPV and
    // PEEP otherwise.  Baseline pressure is EPAP for NPPV and PEEP otherwise.
    // The calls to GetCurrentValue() must be made AFTER the base class Start()
    // is called to phase in any new settings.

    if((BDModeMgrGetCurrentModeId() == ePcvMixedMode)
            ||((BDMgrBdSettingGetCurrentValue(eMode) == ePcvMixedMode)
               &&(BDModeMgrGetCurrentModeId() == eApneaBackupMode)))
    {
        if(eSwitchTypeOn == BDMgrBdSettingGetCurrentValue(eSyncPlus))
        {
            TargetPressure = BDMgrBdSettingGetCurrentValue(eSupportPress)
                    + BDMgrBdSettingGetCurrentValue(eNasalCPAPFlow);
        }
        else
        {
            TargetPressure = BDMgrBdSettingGetCurrentValue(eLowNCPAPLimit);
        }
    }
    else
    {
        TargetPressure = BDMgrBdSettingGetCurrentValue(eProximalLeakLimit);
    }
    BaseLine = BDMgrBdSettingGetCurrentValue(eHighBaselineLimit);

    // Store the Rise Time in seconds, scaled by 100.
    RiseTime = BDMgrBdSettingGetCurrentValue(eRiseTime);

}   // Start()
void PressInhPhase::End (void)
{

}   // end End()
void PressInhPhase::ProcessBdCycle (void)
{
    UNSIGNED timeElapsed;   // time elapsed since start of inhalation in ticks
    LONG expTerm;           // exponential term
    LONG desiredPress;      // desired pressure sent to the controllers
    E_BreathPhaseIndicator breathType;
    E_PatientType patientType;

    // Set up a pointer to the BD Setting Manager
//    MgrBdSetting* mgrBdSettingPtr = MgrBdSetting::S_GetInstance ();

    // Set up a pointer to the Breath Data Manager
//    MgrBreathData* mgrBreathDataPtr = MgrBreathData::S_GetInstance ();

    // Get patient type and breath type
    patientType = (E_PatientType) mgrBdSettingPtrGetCurrentValue (ePatientType);
    breathType = GetBdBreathDataPtrGetInhType ();

    if((breathType == eAssisted) || (breathType == eMandatory))
    {
        // PCV:  if rise time < 0.15 AND control level is < 17cmH20
        // go straight to the desired pressure level
        // (don't use the exponetial curve)
        if((RiseTime < RT_CUTOFF)
                && (TargetPressure - InitialPressure < CONTROL_CUTOFF)
                && (patientType == eAdult))
        {
            desiredPress = TargetPressure;
        }
        else
        {
            // Calculate the time elapsed since the start of inhalation.
//            timeElapsed = InhPhase::S_CalcTimeSincePhaseStart ();

            // Offset the time elapsed by the amount of 1 inhalation pressure control
            // cycle time.
//            timeElapsed = timeElapsed + InhController->GetSamplePeriod();

            // Get the exponential term based on the rise time and elapsed time
//            expTerm = Exponential::GetExpTerm (RiseTime, timeElapsed);

            // Calculate the desired pressure
//            desiredPress = (((TargetPressure - InitialPressure)
//                    * (EXP_DATA_SF - expTerm)) >> EXP_DATA_SHIFT)
//                    + InitialPressure;

        }
    }
    else
    {
        // PSV: if rise time < .015 and PatientType == Adult
        // go straight to the desired pressure level
        // (don't use the exponetial curve)
        if((RiseTime < RT_CUTOFF) && (patientType == eAdult))
        {
            desiredPress = TargetPressure;
        }
        else
        {
            // Calculate the time elapsed since the start of inhalation.
//            timeElapsed = InhPhase::S_CalcTimeSincePhaseStart ();

            // Offset the time elapsed by the amount of 1 inhalation pressure control
            // cycle time.
//            timeElapsed = timeElapsed + InhController->GetSamplePeriod();

            // Get the exponential term based on the rise time and elapsed time
//            expTerm = Exponential::GetExpTerm (RiseTime, timeElapsed);

            // Calculate the desired pressure
//            desiredPress = (((TargetPressure - InitialPressure)
//                    * (EXP_DATA_SF - expTerm)) >> EXP_DATA_SHIFT)
//                    + InitialPressure;

        }

    }

}   // end ProcessBdCycle()

class PressInhPhaseUnitMock : public BDTestFixture
{
public:
    PressInhPhaseUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~PressInhPhaseUnitMock(void){}
    virtual void SetUp(void)
    {
        MyPressInhPhase = new PressInhPhase(eNumBdSettings, eNumBdSettings);
    }
    virtual void TearDown(void)
    {
        delete MyPressInhPhase;
    }
    PressInhPhase *MyPressInhPhase;
};

//test for PressInhPhase::PressInhPhase
TEST_F(PressInhPhaseUnitMock, PressInhPhase)
{
    EXPECT_EQ(MyPressInhPhase->BaseLine, -100);
    EXPECT_EQ(MyPressInhPhase->TargetPressure, 2);
    EXPECT_EQ(MyPressInhPhase->RiseTime, 0);
}

//test for void PressInhPhase::Start (void)
TEST_F(PressInhPhaseUnitMock, Start_0)
{

    EXPECT_CALL(*_MgrBdSetting, MgrBdSettingPhaseInAllSettings()).Times(1);

    EXPECT_CALL(*_ModeMgr, BDModeMgrGetCurrentModeId()).WillOnce(Return(eLastSIMVMode));

    MyPressInhPhase->Start();
}

//test for void PressInhPhase::End (void)
TEST_F(PressInhPhaseUnitMock, End)
{
    //no soruce, not need to test
}

//test for void PressInhPhase::ProcessBdCycle (void)
TEST_F(PressInhPhaseUnitMock, ProcessBdCycle_0)
{
    //all calling function is tested, not need to test
}












#endif //end define BD_PHASE_UNIT_TEST
