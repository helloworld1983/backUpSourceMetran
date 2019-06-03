#include "EnableTestCase.h"
#ifdef BD_PHASE_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"
#include "VcvInhPhase.h"
#include "DebugTrace.h"

using namespace ::testing;

#include "PhaseMgrMockHeader.h"
#include "TriggerMgrMockHeader.h"
#include "BreathMgrMockHeader.h"
#include "MgrBdSettingMockHeader.h"
#include "StepMotorMockHeader.h"
#include "AlarmMockHeader.h"
#include "ModeMgrMockHeader.h"

// Define statics
VcvInhPhase* VcvInhPhase::S_Instance = NULL;

static const LONG STEP_EXP_THRESHOLD = 150 * SCALE;  // 9.00 lpm = 150 ml/sec
static const LONG LPM_24  = 400 * SCALE;  // 24  lpm =  400 ml/sec
static const LONG ML_7   = 7 * SCALE;     // 7 ml
static const LONG ADJUSTED_VT_SETTING   = 3 * SCALE;     // 4 ml
static const LONG ML_150 = 150 * SCALE;   // 150 ml
const SHORT PURGE_FLOW_IN_MLPM = 200;
const int Kp = 1;
const int TIDAL_VOLUME_ERROR = 50;

VcvInhPhase* VcvInhPhase::S_GetInstance (void)
{
    // If the object has not already been created, create it.
    if(NULL == S_Instance)
    {
        S_Instance = new VcvInhPhase ();
        ASSERTION(S_Instance != NULL);
    }

    return (S_Instance);

}   // end S_GetInstance()
void VcvInhPhase::Start (void)
{
    E_FlowPattern flowPattern;      // operator set flow pattern
    LONG setTidalVol;               // operator set tidal volume
    LONG compTidalVol;              // compliance compensated tidal volume
    LONG setPeakFlow;               // operator set peak flow
    UNSIGNED inhTime;               // calculated inhalation time in TICKS
    LONG desiredAirFlow;
    LONG desiredO2Flow;
    static bool firstChange = false;

    // Get a handle to the BD Setting Manager and the TriggerMgr
//    MgrBdSetting* mgrBdSettingPtr = MgrBdSetting::S_GetInstance();
//    TriggerMgr* triggerMgrPtr = TriggerMgr::S_GetInstance();

    // Invoke base class method to do processing common to all inhalation
    // phases
    MgrBdSettingPhaseInAllSettings();

    IsVCVInhPhaseStarted = eTrue;
    // Retrieve current setting values for TV, PF and Flow Pattern.  These
    // calls must be made AFTER the base class Start() is called since any
    // pending setting values are phased in the base class operation.
    ProxyStatus Status = ProxyFlowSensorgetLastStatus();

    if((Status == eCalibrated)||(Status == eResetAndRecovered))
    {
//        setTidalVol = DoP();
        firstChange = true;
    }
    else
    {
        if(firstChange == true)
        {
            firstChange = false;
//            InitErrorCompensate();
            MgrBreathDataGetPdBreathDataPtrclearInhPressData();
        }
        setTidalVol = BDMgrBdSettingGetCurrentValue(eTidalVolume); // ml*SCALE
    }

    setPeakFlow = BDMgrBdSettingGetCurrentValue(eInspFlow)*1000/60; // convert from LPM to mL/sec


    flowPattern = (E_FlowPattern) BDMgrBdSettingGetCurrentValue(eWaveForm);

    // Set the FlowPatternPtr based upon the operator set flow pattern
    switch (flowPattern)
    {
    case eSquare:
//        FlowPatternPtr = SquareFlowPatternS_GetInstance();
        break;

    case eRamp:

        // There are 2 versions of the Ramp Flow Pattern.  If the
        // operator set peak flow is greater than or equal to
        // PF_THRESHOLD_IN_SCALED_MLPS, the flow profile starts at the
        // operator set peak flow and decelerates to
        // PF_THRESHOLD_IN_SCALED_MLPS.  If the operator set peak flow is
        // less than this threshold, the flow profile starts at the
        // operator set peak flow and decelerates to 1/2 of the operator
        // set peak flow.
        if (setPeakFlow >= PF_THRESHOLD_IN_SCALED_MLPS*SCALE)
        {
//            FlowPatternPtr = RampHighPfFlowPatternS_GetInstance();
        }
        else
        {
//            FlowPatternPtr = RampLowPfFlowPatternS_GetInstance();
        }
        break;

    default:
        // Illegal flow pattern
        ASSERTION((flowPattern == eSquare) || (flowPattern == eRamp));
        break;
    }

    // Calculate the target inhalation time based upon flow pattern, set TV
    // and set PF.  Note that the inhalation time is calculated based upon
    // operator settings, not compliance compensated values.
    if ((flowPattern == eSquare) &&
            (setTidalVol <= ML_150)  &&
            (setPeakFlow <= LPM_24))
        {
            setTidalVol += ADJUSTED_VT_SETTING;
        }

    setPeakFlow = setPeakFlow + PURGE_FLOW_IN_MLPM * SCALE / SEC_PER_MIN;
    inhTime = FlowPatternPtrCalcTargetInhTime(setTidalVol, setPeakFlow);
    InhTime = inhTime;
    // Reset error buffer if high ins pressure occur
    if(eActive == BDAlarmTaskGetIsActive(eHip))
    {
        InitErrorCompensate();
    }
    // compliance compensation is on, invoke the private method
    // CalcComplCompVolume to calculate the volume
    compTidalVol = CalcComplCompVol(setTidalVol);
    // Then, calculate the compliance compensated peak flow based upon
    // flow pattern, compliance compensated volume and inhalation time.
    FlowPatternPtrCalcCompensatedPeakFlow(compTidalVol);

    // Enable the VCV Inhalation Time for the inhalation time calculated by
    // the active flow pattern and based upon the TV, PF and flow pattern
    triggerMgrPtrEnableTriggerWith(eVcvInhTimeTrigger, inhTime);

    // Enable the Hip2 trigger; Hip1 was enabled in the InhPhase base class.
    triggerMgrPtrEnableTrigger(eHip2AlarmTrigger);

    // Update patient data;  need to know how the breath was triggered
    // to determine whether or not it is a mandatory or assisted breath
    if (ModeS_IsInhPtTriggered() == eTrue)
    {
        MgrBreathDataInhalationStart(eAssisted);
    }
    else
    {
        MgrBreathDataInhalationStart(eMandatory);
    }

    // Get the initial desired air and o2 flows for this breath
//    InhPhase::Start();
//    FlowPatternPtrGetDesiredFlows(InhPhase::S_CalcTimeSincePhaseStart(),
//                                    &desiredAirFlow, &desiredO2Flow);

//    MgrControllerVCVServoControllerSetSepecificPhase(eVcvInhPhase);
//    MgrControllerVCVServoControllerSetDesiredFlow(desiredAirFlow*60/1000,desiredO2Flow*60/1000);

}   // end Start()

void VcvInhPhase::End (void)
{
    // Disable the VCV inhalation time trigger
    TriggerMgrDisableTrigger(eVcvInhTimeTrigger);

}   // end End()

void VcvInhPhase::ProcessBdCycle (void)
{
    LONG desiredAirFlow;
    LONG desiredO2Flow;

    // Get the desired air and o2 flows for this cycle
//    FlowPatternPtr->GetDesiredFlows(InhPhase::S_CalcTimeSincePhaseStart(),
//                                    &desiredAirFlow, &desiredO2Flow);


//    MgrControllerVCVServoControllerSetDesiredFlow(desiredAirFlow*60/1000,desiredO2Flow*60/1000);

}   // end ProcessBdCycle()
void VcvInhPhase::ResetEndInhPress (void)
{
    // An average of the End Inhalation Pressure samples from previous breaths
    // is used to determine compliance compensation for breath delivery.
    // Initialize the pressure samples to 0 and the index to 0.  By setting
    // the pressure to 0, the first VCV breath will not be compensated.
    for (SHORT ii = 0; ii < NUM_INH_PRESS_SAMPLES; ii++)
    {
        EndInhPress[ii] = 0;
    }

    // Reset the index and the number of samples.
    EndInhPressIx = 0;
    NumEndInhPressSamples = 0;

}   // end ResetEndInhPress()
VcvInhPhase::VcvInhPhase (void) :
        InhPhase ()
{
    // Instantiate all of the flow patterns.
//    RampHighPfFlowPattern::S_GetInstance ();
//    RampLowPfFlowPattern::S_GetInstance ();
//    SquareFlowPattern::S_GetInstance ();

}   // end VcvInhPhase()
LONG VcvInhPhase::CalcComplCompVol (LONG setTidalVol)
{
    LONG avg = 0;           // average end inhalation pressure
    LONG complCompVol;      // compliance compensated volume
    LONG complCompFactor;   // compliance compensated volume

    // use end Ip buffer to average end IP for volume breaths...
    for (SHORT jj = 0; jj < NUM_INH_PRESS_SAMPLES; jj++)
    {
        avg += EndInhPress[jj];
    }

    // Average the value
    avg = avg / NUM_INH_PRESS_SAMPLES;

    // Don't compensate if the value is less than 0.
    if(avg < 0)
    {
        avg = 0;
    }

    complCompFactor = BDMgrBdSettingGetCurrentValue (eComplFactor);

    // Add the compliance volume to the set tidal volume to determine the
    // compliance compensated tidal volume.  The divide by the scale factor
    // is needed since both the compliance factor and the avg are scaled.
    complCompVol = setTidalVol + ((complCompFactor * avg) / SCALE);

    return (complCompVol);

}   // end CalcComplCompVol()
void VcvInhPhase::InitErrorCompensate (void)
{
    PrevcomplCompVol = 0;
    PrevsetTidalVol = 0;
    errorvt = 0;
    prev_errorvt = 0;
    VolumeBrCnt = 0;
    VolumeErrIx = 0;
    for(int i = 0; i < NUM_INH_PRESS_SAMPLES;i++)
    {
        VolumeErrBuf[i] = 0;
    }
}
LONG VcvInhPhase::GetComplVol (void)
{
    return ComplVol;
}
FlowPattern* VcvInhPhase::GetFlowPatternPtr (void)
{
    return FlowPatternPtr;
}
void VcvInhPhase::ResetVolumeAdjustment (void)
{
    VolumeAdjustment = 0;
}
void VcvInhPhase::SetPhaseStarted (E_Bool value)
{
    IsVCVInhPhaseStarted = value;
}
E_Bool VcvInhPhase::GetPhaseStarted (void)
{
    //Get phase start
    return IsVCVInhPhaseStarted;
}
LONG VcvInhPhase::DoP (void)
{
    LONG tidalVolSetting = BDMgrBdSettingGetCurrentValue(eTidalVolume);
    LONG currentTidalVolume = MgrBreathDataGetTidalVolumeEndInh();

    if((tidalVolSetting != 0) && (currentTidalVolume * 100/tidalVolSetting < 25))
    {
        return tidalVolSetting;
    }
    LONG error =  tidalVolSetting - currentTidalVolume;
    if(error > 50)
    {
        VolumeAdjustment += (Kp * TIDAL_VOLUME_ERROR);
    }
    else if(error < -50)
    {
        VolumeAdjustment -= (Kp * TIDAL_VOLUME_ERROR);
    }
    else
    {
        //Do nothing
    }
    return tidalVolSetting + VolumeAdjustment;
}

class VcvInhPhaseUnitMock : public BDTestFixture
{
public:
    VcvInhPhaseUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~VcvInhPhaseUnitMock(void){}
    virtual void SetUp(void)
    {
        MyVcvInhPhase = new VcvInhPhase();
    }
    virtual void TearDown(void)
    {
        delete MyVcvInhPhase;
    }
    VcvInhPhase *MyVcvInhPhase;
};

//test for VcvInhPhase* VcvInhPhase::S_GetInstance (void)
TEST_F(VcvInhPhaseUnitMock, S_GetInstance)
{
    MyVcvInhPhase->S_GetInstance();

    EXPECT_NE(MyVcvInhPhase->S_Instance, (void*)0);
}

//test for void VcvInhPhase::End (void)
TEST_F(VcvInhPhaseUnitMock, End)
{
    EXPECT_CALL(*_TriggerMgr, TriggerMgrDisableTrigger(eVcvInhTimeTrigger)).Times(1);

    MyVcvInhPhase->End();
}

//test for void VcvInhPhase::ProcessBdCycle (void)
TEST_F(VcvInhPhaseUnitMock, ProcessBdCycle)
{
    //all calling function is tested, not need to test
}

//test for void VcvInhPhase::ResetEndInhPress (void)
TEST_F(VcvInhPhaseUnitMock, ResetEndInhPress)
{
    EXPECT_EQ(MyVcvInhPhase->EndInhPressIx, 0);
    EXPECT_EQ(MyVcvInhPhase->NumEndInhPressSamples, 0);
    for (SHORT ii = 0; ii < NUM_INH_PRESS_SAMPLES; ii++)
    {
        EXPECT_EQ(MyVcvInhPhase->EndInhPress[ii], 0);
    }
}

//test for VcvInhPhase::VcvInhPhase (void) :
TEST_F(VcvInhPhaseUnitMock, VcvInhPhase)
{
    //no source, not need to test
}

//test for LONG VcvInhPhase::CalcComplCompVol (LONG setTidalVol)
TEST_F(VcvInhPhaseUnitMock, CalcComplCompVol)
{
    for (SHORT jj = 0; jj < NUM_INH_PRESS_SAMPLES; jj++)
    {
        MyVcvInhPhase->EndInhPress[jj] = jj;
    }
    EXPECT_CALL(*_MgrBdSetting, BDMgrBdSettingGetCurrentValue (eComplFactor)).Times(1).WillOnce(Return(1));

    EXPECT_EQ(MyVcvInhPhase->CalcComplCompVol(1), 1);
}

//test for void VcvInhPhase::InitErrorCompensate (void)
TEST_F(VcvInhPhaseUnitMock, InitErrorCompensate)
{
    MyVcvInhPhase->InitErrorCompensate();

    EXPECT_EQ(MyVcvInhPhase->PrevcomplCompVol, 0);
    EXPECT_EQ(MyVcvInhPhase->PrevsetTidalVol, 0);
    EXPECT_EQ(MyVcvInhPhase->errorvt, 0);
    EXPECT_EQ(MyVcvInhPhase->prev_errorvt, 0);
    EXPECT_EQ(MyVcvInhPhase->VolumeBrCnt, 0);
    EXPECT_EQ(MyVcvInhPhase->VolumeErrIx, 0);

    for(int i = 0; i < NUM_INH_PRESS_SAMPLES;i++)
    {
        EXPECT_EQ(MyVcvInhPhase->VolumeErrBuf[i], 0);
    }
}

//test for LONG VcvInhPhase::GetComplVol (void)
TEST_F(VcvInhPhaseUnitMock, GetComplVol)
{
    MyVcvInhPhase->ComplVol = 9;

    EXPECT_EQ(MyVcvInhPhase->GetComplVol(), 9);
}

//test for FlowPattern* VcvInhPhase::GetFlowPatternPtr (void)
TEST_F(VcvInhPhaseUnitMock, GetFlowPatternPtr)
{
    //not need to test
}

//test for void VcvInhPhase::ResetVolumeAdjustment (void)
TEST_F(VcvInhPhaseUnitMock, ResetVolumeAdjustment)
{
    MyVcvInhPhase->VolumeAdjustment = 10;

    MyVcvInhPhase->ResetVolumeAdjustment();

    EXPECT_EQ(MyVcvInhPhase->VolumeAdjustment, 0);
}

//test for void VcvInhPhase::SetPhaseStarted (E_Bool value)
TEST_F(VcvInhPhaseUnitMock, SetPhaseStarted)
{
    MyVcvInhPhase->SetPhaseStarted(eTrue);

    EXPECT_EQ(MyVcvInhPhase->IsVCVInhPhaseStarted, eTrue);

    MyVcvInhPhase->SetPhaseStarted(eFalse);

    EXPECT_EQ(MyVcvInhPhase->IsVCVInhPhaseStarted, eFalse);
}

//test for LONG VcvInhPhase::DoP (void)
TEST_F(VcvInhPhaseUnitMock, DoP_0)
{
    EXPECT_CALL(*_MgrBdSetting, BDMgrBdSettingGetCurrentValue(eTidalVolume)).Times(1).WillOnce(Return(1));

    EXPECT_CALL(*_BreathMgr, MgrBreathDataGetTidalVolumeEndInh()).Times(1).WillOnce(Return(1));

    EXPECT_EQ(MyVcvInhPhase->DoP(), 1);
}

TEST_F(VcvInhPhaseUnitMock, DoP_1)
{
    EXPECT_CALL(*_MgrBdSetting, BDMgrBdSettingGetCurrentValue(eTidalVolume)).Times(1).WillOnce(Return(52));

    EXPECT_CALL(*_BreathMgr, MgrBreathDataGetTidalVolumeEndInh()).Times(1).WillOnce(Return(1));

    EXPECT_EQ(MyVcvInhPhase->DoP(), 52);
}
TEST_F(VcvInhPhaseUnitMock, DoP_2)
{
    EXPECT_CALL(*_MgrBdSetting, BDMgrBdSettingGetCurrentValue(eTidalVolume)).Times(1).WillOnce(Return(10));

    EXPECT_CALL(*_BreathMgr, MgrBreathDataGetTidalVolumeEndInh()).Times(1).WillOnce(Return(70));

    MyVcvInhPhase->VolumeAdjustment = 100;

    EXPECT_EQ(MyVcvInhPhase->DoP(), 60);
}

#endif //end define BD_PHASE_UNIT_TEST
