#include "EnableTestCase.h"
#ifdef PD_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "PDFixture.h"
#include "PtData.h"

#include "DebugTrace.h"

using namespace ::testing;
using namespace std;

Ptdata* Ptdata::S_Instance = NULL;

// seconds per minute
static const LONG SECPERMIN = 60;

// milliseconds per minute
static const LONG MSECPERMIN = SECPERMIN * (ULONG) 1000;

//patient data value to indicate that patient data is not valid and
//   should be blanked on the display screen
const LONG BLANKED_LONG_VALUE = 0x80000000;

static const float  O2_NOT_PRESENT   = (-1);
static const float  O2_NOT_VALID     = (-2);

Ptdata* Ptdata::S_GetInstance (void)
{
    if(S_Instance == NULL)
    {
        S_Instance = new Ptdata ();
    }

    ASSERTION(S_Instance != NULL);

    return (S_Instance);
}

void Ptdata::CompNumBreaths (void)
{
    LONG breathtime = 0;
    LONG backup = Ix;
    LONG i;

    SpontBreaths = 0;

    for (i = 0; i < MAX_BREATHS; i++)
    {
        if(PdHist[backup].Indicator == eNoPhase)
        {
            CallThis();
            break;
        }
        if(breathtime >= MSECPERMIN)
        {
            CallThis();
            break;
        }
        breathtime += (PdHist[backup].InhTime + PdHist[backup].ExhTime);
        if(PdSpontaneous (PdHist[backup].Indicator))
        {
            SpontBreaths++;
        }

        backup = (backup - 1) & MAX_BREATHS_MASK;
        CallThis();
    }

    breathsofData = i;
    MinCalTime = breathtime;

}

void Ptdata::CompCircuitComplianceVolume (void)
{
    ComplVolume = ((CircuitComplianceFactor * PeepAdjEndInhPress) / SCALE);
    if (ComplVolume < 0)
    {
        ComplVolume = 0;
    }
}

void Ptdata::CompExhTidalVolume (void)
{
    const LONG MUX_NUMBER_OF_EXH_VOL = 8;
    const LONG MUX_NUMBER_OF_EXH_VOL12 = 12;
    ExhTidalVol = ExhVolume;
    ExhTidalVolArray[ExhTidalVolIx]  = ExhTidalVol;
    ExhTidalVolIx++;
    ExhTidalVolIx = ExhTidalVolIx % EIGHT_SAMPLES;

    if(ExhTidalVolNum < MUX_NUMBER_OF_EXH_VOL)
    {
        ExhTidalVolNum++;
    }

    //for new VA
    //exclude spontaneous breath
    E_PhaseId phaseId = PhaseMgrGetCurrentPhaseId();
#ifndef VA_WITH_PS
    if(prevphaseId_vt != eSpontInhPhase)
    {
#endif
        //wait for 12 breath
        if(sample12_fill_flg == true)
        {
            ExhTidalVolArray12[ExhTidalVolIx12]  = ExhTidalVol;
        }
        else
        {
            ExhTidalVolArray12[ExhTidalVolIx12] = 0;
        }

        if(sample12_fill_flg == true)
        {
            if(ExhTidalVolNum12 < MUX_NUMBER_OF_EXH_VOL12)
            {
                ExhTidalVolNum12++;
            }
        }

        ExhTidalVolIx12++;
        if(ExhTidalVolIx12 >= TWELVE_SAMPLES)
        {
            sample12_fill_flg = true;
        }
        ExhTidalVolIx12 = ExhTidalVolIx12 % TWELVE_SAMPLES;
        LastMandExhVt = ExhTidalVol;
#ifndef VA_WITH_PS
    }
#endif
    prevprevphaseId_vt = prevphaseId_vt;
    prevphaseId_vt = phaseId;


    // Limit volume to 0
    if (ExhTidalVol < 0)
    {
        ExhTidalVol = 0;
    }

    PdHist[Ix].ExhTidalVol = ExhTidalVol;       // save in history table

    E_VentilationMode currentMode = ModeMgrGetCurrentModeId();
    E_VentilationMode nextMode = ModeMgrGetNextModeId();

    if((currentMode != eNivMode)&&(nextMode != eNivMode))
    {

        ProxyStatus Status = RequestDevice();

        if((Status==eCalibrated)||(Status==eResetAndRecovered))
            // check for low exhaled minute alarm conditions
        {

            LONG HiVteLimit = MgrBdSettingGetCurrentValue(eHighTidalVolLimit);
            LONG LowVteLimit = MgrBdSettingGetCurrentValue(eLowTidalVolLimit);

            PdAlarmsCheckHiCondition(HiVteLimit, ExhTidalVol, eHighVte);
            PdAlarmsCheckLowCondition(LowVteLimit, ExhTidalVol, eLowVte);
        }
    }

}
void Ptdata::CompExhMinuteVolume (void)
{
    const LONG DEVIDE_ZERO = 0;
    LONG i;
    double sumInhTidalVol = 0;
    LONG backup = Ix;

    for (i = 0; i < breathsofData; i++)
    {
        sumInhTidalVol += PdHist[backup].InhTidalVol;
        backup = (backup - 1) & MAX_BREATHS_MASK;
    }

    if (DEVIDE_ZERO == MinCalTime)
    {
        InhMinuteVol = BLANKED_LONG_VALUE;
    }
    else
    {
        InhMinuteVol = (LONG)((sumInhTidalVol * MSECPERMIN/1000) / (MinCalTime));
    }
}

void Ptdata::UpdateMandExhTidalVolume (void)
{
    int i;
    LONG sumMandExhTidalVol = 0;

    NumMandBreaths++;

    if(NumMandBreaths > MAX_BREATHS)
    {
        NumMandBreaths = MAX_BREATHS;
    }

    ManExhTidalVol[MandBreathsIx] = ExhTidalVol;
    MandBreathsIx = (MandBreathsIx + 1) & MAX_BREATHS_MASK;

    for (i = 0; i < NumMandBreaths; i++)
    {
        sumMandExhTidalVol += ManExhTidalVol[i];
    }

    if(NumMandBreaths == 0)
    {
        CallThis();
        ASSERTION(eFalse);
    }
    else
    {
        MandExhTidalVolAverage = sumMandExhTidalVol / NumMandBreaths;
    }

}
void Ptdata::CompRapidShallowBreathix (void)
{
    const LONG LIMIT = 5 * SCALE;

    if((((VentType != eNonInvasiveVentilation) && (Mode == eSpontaneousMode)
            && (SupportPressure <= LIMIT))
            || ((VentType == eNonInvasiveVentilation)
                    && (Mode == eSpontaneousMode)
                    && (SupportPressure == BaselinePressSetting)))
            && (SpontMinuteVol != 0) && (SpontMinuteVol != BLANKED_LONG_VALUE))
    {
        RapidShallowBreathix = (SpontRespRate * SpontRespRate) / SpontMinuteVol;
    }
    else
    {
        RapidShallowBreathix = BLANKED_LONG_VALUE;
    }
}
void Ptdata::CompIeRatio (void)
{
    const ULONG DEVIDE_FOR_ZERO = 0;

    if(TimeInExh >= TimeInInh)
    {
        if(TimeInInh == DEVIDE_FOR_ZERO)
        {
            IeRatio = BLANKED_LONG_VALUE;
        }
        else
        {
            IeRatio = -1 * (LONG) ((TimeInExh * SCALE) / TimeInInh);
        }
    }
    else
    {
        if(TimeInExh == DEVIDE_FOR_ZERO)
        {
            IeRatio = BLANKED_LONG_VALUE;
        }
        else
        {
            IeRatio = ((TimeInInh * SCALE) / TimeInExh);
        }
    }

}

void Ptdata::CompMeanAirwayPressure (void)
{
    LONG numSamples;
    LONG i, sumMeanAirwayPress = 0;
    LONG backup = Ix;

    numSamples = NumInhSamples + NumExhSamples;

    // compute current mean airway pressure and store in history table
    if(numSamples == 0)
    {
        PdHist[Ix].MeanAirwayPress = 0;
    }
    else
    {
        PdHist[Ix].MeanAirwayPress = PressureSum / numSamples;
    }

    // compute average mean air way pressure
    for (i = 0; i < breathsofData; i++)
    {
        sumMeanAirwayPress += PdHist[backup].MeanAirwayPress;
        backup = (backup - 1) & MAX_BREATHS_MASK;
    }

    if(breathsofData == 0)
    {
        MeanAirwayPressAvg = BLANKED_LONG_VALUE;
    }
    else
    {
        MeanAirwayPressAvg = sumMeanAirwayPress / breathsofData;
    }
}
void Ptdata::CompRespRate (void)
{
    if(MinCalTime == 0)
    {
        TotalRespRate = BLANKED_LONG_VALUE;
        SpontRespRate = BLANKED_LONG_VALUE;
    }
    else
    {
        TotalRespRate = (breathsofData * MSECPERMIN * SCALE) / MinCalTime;
        SpontRespRate = (SpontBreaths * MSECPERMIN * SCALE) / MinCalTime;
    }

}

void Ptdata::GetExhalationData (void)
{
    //    GetEndExhFlow();

    // Retrieve the current mode from BD
    Mode = ModeMgrGetCurrentModeId();
    BaselinePressSetting = MgrBdSettingGetCurrentValue(eBaselinePress);
    PressureSum     = MgrBreathDataGetPdBreathDataPtrGetPressureSum();
    ExhVolumeCompensated = MgrBreathDataGetPdBreathDataPtrGetCompensatedExhVolume();
    ExhVolume = MgrBreathDataGetPdBreathDataPtrGetExhVolume();
    EndExhPress     = (LONG) MgrBreathDataGetPdBreathDataPtrGetEndExhPress();
    NumExhSamples   = MgrBreathDataGetPdBreathDataPtrGetNumExhSamples();
    TimeInExh       = (LONG) MgrBreathDataGetPdBreathDataPtrGetTimeInExh() * MS_PER_TICK;
    InhType         = MgrBreathDataGetPdBreathDataPtrGetInhType();
    LowBaseLineAlarmLimit   =(LONG)MgrBdSettingGetCurrentValue(eLowBaselineLimit);

    //Update PEEP Alarm
    if(ModeMgrGetCurrentModeId() != eAPRVMode)
    {
        SHORT HiPLimit = MgrBdSettingGetCurrentValue(eHighBaselineLimit);
        SHORT LowPLimit = MgrBdSettingGetCurrentValue(eLowBaselineLimit);

        PdAlarmsCheckHiBaseLine(HiPLimit,EndExhPress,eHighPeep);
        PdAlarmsCheckLowBaseLine(LowPLimit,EndExhPress,eLowPeep);
    }

    HrrAlarmLimit   =(LONG)MgrBdSettingGetCurrentValue(eHrrLimit);
    LemvLimit       =(LONG)MgrBdSettingGetCurrentValue(eLemvLimit);
    HemvLimit       = MgrBdSettingGetCurrentValue(eHemvLimit);
    LestvLimit      = (LONG)MgrBdSettingGetCurrentValue(eLestvLimit);
    LemtvLimit      = (LONG)MgrBdSettingGetCurrentValue(eLemtvLimit);
    SupportPressure = (LONG)MgrBdSettingGetCurrentValue(eSupportPress);
    CircuitComplianceON = MgrBdSettingGetCurrentValue(eComplOnOff);

    // indicate patient data is done with BD data base
    MgrBreathDataSetPdDone();

    // store info in history tables
    PdHist[Ix].ExhTime =TimeInExh;
    PdHist[Ix].InhTime =TimeInInh;
    PdHist[Ix].Indicator = InhType;

    PdAlarmsResetPEEPAlarmActiveTime();

}
void Ptdata::GetInhalationData (void)
{
    PeakInhPress    = (LONG) MgrBreathDataGetPdBreathDataPtrGetPeakInhPress();
    EndInhPress     = (LONG) MgrBreathDataGetPdBreathDataPtrGetEndInhPress();
    NumInhSamples   = (LONG) MgrBreathDataGetPdBreathDataPtrGetNumInhSamples();
    TimeInInh       = MgrBreathDataGetPdBreathDataPtrGetTimeInInh() * MS_PER_TICK;
    InhType         = MgrBreathDataGetPdBreathDataPtrGetInhType();

    // mode phase in at the start of exhalation
    Mode = (E_VentilationMode) MgrBdSettingGetCurrentValue(eMode);

    // check for alarm conditions for LIP on mandatory breaths
    // avoid alarm checks situation when transitioning into spont mode

    if ( (PdMandatory(InhType) ) &&
            !((OldMode != eSpontaneousMode) && (Mode == eSpontaneousMode)) )
    {
        LipLimit = (LONG) MgrBdSettingGetCurrentValue(eLipLimit);
        PdAlarmsCheckLowCondition(LipLimit, PeakInhPress,eLowInspPressure);
    }
    // save in history table
    PdHist[Ix].InhTidalVol = mInhVol;

}
void Ptdata::DoInhalationData (void)
{
    GetInhalationData ();
    CompInhMinuteVolume ();
    BuildInhMsg ();
}

void Ptdata::DoExhalationData (void)
{
    //    GetExhalationData ();
    //    CompProxymalLeak();
    //    CompNumBreaths ();
    //    CompCircuitComplianceVolume ();
    //    CompExhTidalVolume ();
    //    CompExhMinuteVolume();
    //    CompMeanAirwayPressure ();
    //    CompIeRatio ();
    //    CompRespRate ();
    //    CompRapidShallowBreathix ();
    //    CompCompliance();
    //    CompAverageCompliance();
    //    CompLeakCompensate();

    Ix = (Ix + 1) & MAX_BREATHS_MASK;

    //    BuildExhMsg ();
}

void Ptdata::DoO2 (void)
{
    SHORT tempO2concentration = 0;
    bool IsO2SensorConnect = false;
    const float RAW_O2_LIMIT = 21.0;
    const float INIT_VALUE = 0;
    const float RESET = 0;
    const float O2_HIGHER_LIMIT = 100.0;
    const LONG O2_CONCENTRATOR_DEFAULT = 10000;
    const int SCALE = 100;

    //    if(DeviceInterfaceRequestDevice(eGetHighO2AlarmLimit))
    //    {
    HighO2AlarmLimit = (LONG)DeviceInterfaceRequestDevice(eGetHighO2AlarmLimit);
    //    }
    //    else
    //    {
    //can not get high02 alarm limit
    DEBUG_PTDATA("Can not get high 02 alarm limit\n");
    //    }

    //    if(DeviceInterfaceRequestDevice(eGetLowO2AlarmLimit))
    //    {
    HighO2AlarmLimit = (LONG)DeviceInterfaceRequestDevice(eGetLowO2AlarmLimit);
    //    }
    //    else
    //    {
    //        //can not get low 02 alarm limit
    //        DEBUG_PTDATA("Can not get low 02 alarm limit\n");
    //    }

    if(DeviceInterfaceRequestDevice(eIsO2SensorConnect))
    {
        if(eTrue == IsO2SensorConnect)
        {
            if(eStandbyMode != ModeMgrGetCurrentModeId())
            {
                float rawO2 = INIT_VALUE;
                DeviceInterfaceRequestDevice(eFiO2SensorGetCurrentReading);
                float rawPress = INIT_VALUE;
                DeviceInterfaceRequestDevice(eFiO2SensorGetAvgPress);
                float rawFlow = INIT_VALUE;
                DeviceInterfaceRequestDevice(eFiO2SensorGetAvgFlow);
                float rawAmplitude = amplitude;

                //do O2 compensation
                //eto added amplitude factor only HFO mode
                if(eHFOMode != ModeMgrGetCurrentModeId())
                {
                    rawAmplitude = RESET;
                }

                //modified formula O2 factor to amplitude factor.
                float compO2 =  (0.0465*exp(0.0203*rawO2))*(rawFlow - 1) + (0.0008*rawO2 - 0.0066)*rawPress
                        + (0.02*rawAmplitude/100)*((rawO2-21)/(100-21));

                if(rawO2 <= RAW_O2_LIMIT)
                {
                    compO2 = RESET;
                }
                O2Concentration = (LONG)((rawO2 - compO2) * SCALE);

                //added O2 higher side limitation
                if(rawO2 - compO2 > O2_HIGHER_LIMIT)
                {
                    O2Concentration = O2_CONCENTRATOR_DEFAULT;
                }

            }
            else
            {
                //do not read FiO2 sensor in standby mode
                O2Concentration = O2_NOT_PRESENT;
            }

        }
        else
        {
            O2Concentration = O2_NOT_PRESENT;
        }
    }
    else
    {
        //can not get status of 02 sensor
        DEBUG_PTDATA("can not get status of 02 sensor\n");
    }

    if ((O2Concentration != O2_NOT_PRESENT) && (O2Concentration != O2_NOT_VALID))
    {
        // check for alarm conditions for Hi and Low O2
        PdAlarmsCheckHiO2(HighO2AlarmLimit, O2Concentration, eHighO2);
        PdAlarmsCheckLowO2(LowO2AlarmLimit, O2Concentration, eLowO2);
    }
    else
    {
        AlarmTaskSetIsActive(eHighO2, eNotActive);
        AlarmTaskSetIsActive(eLowO2, eNotActive);
        O2Concentration = BLANKED_LONG_VALUE;
    }

    BuildO2Msg();

}

void Ptdata::ClearHistory (void)
{
    int i;
    for (i = 0; i < MAX_BREATHS; i++)
    {
        PdHist[i].Indicator = eNoPhase;
        PdHist[i].InhTime = 0;
        PdHist[i].ExhTime = 0;
        PdHist[i].MeanAirwayPress = 0;
    }
    IsFirst_1minute = true;
    MinCalTime = 0;
    NumMandBreaths = 0;
    MandBreathsIx = 0;
    SpontBreaths = 0;
    numBreaths = 0;
    breathsofData = 0;
    Ix = 0;

    CircuitComplianceON = eFalse;
    CircuitComplianceFactor = 0;
    PeepAdjEndInhPress = 0;
    manInspHoldTime = 0;
    maxpress = minpress = 0;
    TimeInExh = TimeInInh = 0;
    SupportPressure = 0;
    BaselinePressSetting = 0;

    NumInhSamples = 0;
    NumExhSamples = 0;

    ExhVolumeCompensated = 0;
    PressureSum = 0;
    EndExhPress = 0;
    EndInhPress = 0;
    PeakInhPress = 0;
    map = 0;
    amplitude = 0;
    Mode = eStandbyMode;
    HighO2AlarmLimit = 0;
    LowO2AlarmLimit  = 0;
    LowBaseLineAlarmLimit   = 0;
    LipLimit = HipLimit = 0;
    HemvLimit = LetvLimit = 0;

    HrrAlarmLimit = LemvLimit = 0;
    LestvLimit = LemtvLimit = 0;
    O2Concentration = 0;
    hfoVe = hfoBaseFlow = 0;
    MandExhTidalVolAverage=0;
    ExhMinuteVol = 0;
    SpontRespRate = 0;
    SpontMinuteVol =0;
    DeliveredO2 = 0;
    RapidShallowBreathix = 0;
    IeRatio = 0;
    ComplVolume = 0;
    TotalRespRate = 0;
    totalTime = 0;
    MinCalTime = 0;
    breathsofData = 0;
    numBreaths = 0;
    Ix = 0;
    NumMandBreaths = 1;
    MandBreathsIx = 0;
    SpontBreaths = 0;
    InhType = eNoPhase;

    //    ExhMsg.PEEP = 0;
    //    ExhMsg.MeanAirwayPressure = 0;
    //    ExhMsg.ExhaledMinuteVolume = 0;
    //    ExhMsg.SpontaneousMinuteVolume = 0;
    //    ExhMsg.SpontaneousRespiratoryRate = 0;
    //    ExhMsg.TotalRespiratoryRate = 0;
    //    ExhMsg.IERatio = 0;
    //    ExhMsg.RapidShallowBreathingIndex = 0;
    //    ExhMsg.TidalVolume = 0;
    //    ExhMsg.TimeInExhalation = 0;
    //    ExhMsg.ProxymalLeak = 0;

    //    O2Msg.Value = 0;
    //    PEEPMsg.Value = 0;
    //    HFOMsg.MeanAirPressure = 0;
    //    HFOMsg.TidalVolume = 0;
    //    HFOMsg.Freshgas = 0;
    //    HFOMsg.Exhflow = 0;
    //    HFOMsg.MaxPress = 0;
    //    HFOMsg.MinPress = 0;
    //    HFOMsg.Amplitude = 0;
    //    HFOMsg.ManInspHoldTime = 0;
    //    InhMsg.PeakInspiratoryPressure = 0;
    //    InhMsg.EndInhalationPressure = 0;
    //
    //    InhMsg.TimeInInhalation = 0;
    hfoExhFlow = 0;
    ProxymalLeakValue = 0;
    MeanAirwayPressAvg = 0;
    ExhTidalVol = 0;

    ComplianceSampleIx = 0;
    for(int i = 0; i < NUM_OF_SAMPLE_COMPLIANCE; i++)
    {
        ComplianceSample[i]=0;
    }

    EndExhFlowIx = 0;
    ExhLeakIx = 0;

    for(int i = 0;i < EIGHT_SAMPLES; i++)
    {
        EndExhFlowArray[i] = 0;
        ExhLeakArray[i] = 0;
        ExhTidalVolArray[i] = 0;
    }
    ExhTidalVolIx = 0;
    ExhTidalVolNum = 0;

    //for new VA
    for(int i = 0; i< TWELVE_SAMPLES; i++)
    {
        ExhTidalVolArray12[i] = 0;
        ComplianceArray12[i] = 0;
    }
    ExhTidalVolIx12 = 0;
    ExhTidalVolNum12 = 0;
    ComplianceIx12 = 0;
    ComplianceNum12 = 0;
    LastMandExhVt = 0;
    sample12_fill_flg = false;
    sample12_fill_flg_comp = false;
    prevphaseId_vt = eInitPhase;
    prevphaseId_comp = eInitPhase;
    prevprevphaseId_vt = eInitPhase;
    prevprevphaseId_comp = eInitPhase;
    //for new VA

    mNumOfComplianceSample = 0;
    Compliance = 0;
    ComplianceAverage = 0;

    mNCPAP_Pressure = 0;
    mInhaledTidalVolume = 0;

    BlankingData();
    BuildInhMsg();
    BuildExhMsg();
    BuildO2Msg();
    BuildHFOMsg();

    //    PatientDataGUIEvent PMsg;
    //
    //    PMsg.Value = BLANKED_LONG_VALUE;
    //    MainHandlerDelegatepostCustomEvent<PatientDataGUIEvent>(eAPRVPHighPtDataID, PMsg);
    //    MainHandlerDelegatepostCustomEvent<PatientDataGUIEvent>(eAPRVPLowPtDataID, PMsg);
}

void Ptdata::BuildInhMsg (void)
{
    //    InhMsg.PeakInspiratoryPressure = PeakInhPress;
    //    InhMsg.EndInhalationPressure = EndInhPress;
    //    InhMsg.TimeInInhalation = TimeInInh;
    //    InhMsg.InhalationType = InhType;
    //
    //    ProxyStatus Status = proxySensor->getLastStatus();
    //    if((Status == eCalibrated)||(Status == eResetAndRecovered))
    //    {
    //        InhMsg.InhaledTidalVolume = mInhVol;
    //        InhMsg.InhaledMinuteVolume = InhMinuteVol;
    //    }
    //    else
    //    {
    //        InhMsg.InhaledTidalVolume = BLANKED_LONG_VALUE;
    //        InhMsg.InhaledMinuteVolume = BLANKED_LONG_VALUE;
    //    }
    //
    //    guiPtr->postCustomEvent<EndInhalationDataGUIEvent>(eEndInhalationData, InhMsg);


}

void Ptdata::BuildExhMsg (void)
{
    //    ExhMsg.MeanAirwayPressure = MeanAirwayPressAvg;
    //    ExhMsg.SpontaneousMinuteVolume = SpontMinuteVol;
    //    ExhMsg.TotalRespiratoryRate = TotalRespRate;
    //    ExhMsg.SpontaneousRespiratoryRate = SpontRespRate;
    //    ExhMsg.IERatio = IeRatio;
    //    ExhMsg.RapidShallowBreathingIndex = RapidShallowBreathix;
    //    ExhMsg.TimeInExhalation = TimeInExh;
    //    ExhMsg.ProxymalLeak = ProxymalLeakValue*100.0;
    //    ExhMsg.Compliance = (LONG)Compliance;
    //
    //    ProxyStatus Status = proxySensor->getLastStatus();
    //    if((Status == eCalibrated)||(Status == eResetAndRecovered))
    //    {
    //        ExhMsg.ExhaledMinuteVolume = ExhMinuteVol;
    //        ExhMsg.TidalVolume = ExhTidalVol;
    //    }
    //    else
    //    {
    //        ExhMsg.TidalVolume = BLANKED_LONG_VALUE;
    //        ExhMsg.ExhaledMinuteVolume = BLANKED_LONG_VALUE;
    //    }
    //    ExhMsg.PEEP = EndExhPress;
    //
    //    guiPtr->postCustomEvent<EndExhalationDataGUIEvent>(eEndExhalationData, ExhMsg);

}

void Ptdata::BuildO2Msg (void)

{
    //    O2Msg.Value = O2Concentration;
    //
    //    guiPtr->postCustomEvent<PatientDataGUIEvent>(eMonitoredO2PtDataID, O2Msg);

}
void Ptdata::BlankingData (void)
{

    O2Concentration = BLANKED_LONG_VALUE;

    PeakInhPress = BLANKED_LONG_VALUE;
    EndInhPress = BLANKED_LONG_VALUE;

    DisplayedExhTidalVol = BLANKED_LONG_VALUE;
    ExhTidalVol = BLANKED_LONG_VALUE;
    ExhMinuteVol = BLANKED_LONG_VALUE;
    SpontMinuteVol = BLANKED_LONG_VALUE;
    TotalRespRate = BLANKED_LONG_VALUE;
    SpontRespRate = BLANKED_LONG_VALUE;
    IeRatio = BLANKED_LONG_VALUE;
    MeanAirwayPressAvg = BLANKED_LONG_VALUE;
    RapidShallowBreathix = BLANKED_LONG_VALUE;
    EndExhPress = BLANKED_LONG_VALUE;
}

Ptdata::Ptdata (void)

{

    //    // initialize interface pointers
    //
    //    alarmPtr = PdAlarms::S_GetInstance();
    //    ASSERTION (alarmPtr != NULL);
    //
    //    mgrBdSettingPtr = MgrBdSetting::S_GetInstance();
    //    ASSERTION (mgrBdSettingPtr != NULL);
    //
    //    mgrBdPtr = MgrBreathData::S_GetInstance();
    //    ASSERTION (mgrBdPtr != NULL);
    //
    //    guiPtr = GUITask::S_GetInstance();
    //    ASSERTION (guiPtr != NULL);
    //
    //    O2Ptr = O2Setting::S_GetInstance();
    //    ASSERTION(O2Ptr != NULL);
    //
        ClearHistory();
    //
    //    CircuitComplianceON = eFalse;
    //    CircuitComplianceFactor = 0;

}

void Ptdata::CompInhMinuteVolume (void)
{
    LONG i;
    double sumInhTidalVol = 0;
    LONG backup = Ix;

    for (i = 0; i < breathsofData; i++)
    {
        sumInhTidalVol += PdHist[backup].InhTidalVol;
        backup = (backup - 1) & MAX_BREATHS_MASK;
    }

    if(MinCalTime == 0)
    {
        InhMinuteVol = BLANKED_LONG_VALUE;
    }
    else
    {
        InhMinuteVol = (LONG) ((sumInhTidalVol * MSECPERMIN / 1000)
                / (MinCalTime));
    }
}

void Ptdata::CompProxymalLeak (void)
{
    const LONG DEVIDE_ZERO = 0.0;
    E_VentilationMode currentMode = ModeMgrGetCurrentModeId();
    E_VentilationMode nextMode = ModeMgrGetNextModeId();

    if((currentMode != eNivMode)&&(nextMode != eNivMode))
    {
        ProxyStatus Status = DeviceInterfaceRequestDevice(eProxySensorLastStatus);
        if((Status==eCalibrated)||(Status==eResetAndRecovered))
        {
            LONG InhVolume = MgrBreathDataGetPdBreathDataPtrGetInhVolume();
            LONG ExhVolume = MgrBreathDataGetPdBreathDataPtrGetExhVolume();

            if(InhVolume != DEVIDE_ZERO)
            {
                ProxymalLeakValue = ((float)(InhVolume - ExhVolume))/float(InhVolume)*100.0;
                if(ProxymalLeakValue < 0)
                {
                    ProxymalLeakValue = 0;
                }
                if(ProxymalLeakValue > 100)
                {
                    ProxymalLeakValue = 100;
                }
            }
            else
            {
                ProxymalLeakValue = 0;
            }

            LONG ProxymalLeakHiLimit = MgrBdSettingGetCurrentValue(eProximalLeakLimit)/SCALE;

            if(ProxymalLeakHiLimit <= 100)
            {
                PdAlarmsCheckHiCondition(ProxymalLeakHiLimit,ProxymalLeakValue,eProximalLeakage);
            }
            else
            {
                AlarmTaskSetIsActive(eProximalLeakage, eNotActive);
            }
        }
        else
        {
            ProxymalLeakValue = BLANKED_LONG_VALUE;
            AlarmTaskSetIsActive(eProximalLeakage, eNotActive);
        }
    }
    else
    {
        ProxymalLeakValue = BLANKED_LONG_VALUE;
        AlarmTaskSetIsActive(eProximalLeakage, eNotActive);
    }

}

void Ptdata::CompCompliance (void)
{
    const double DEVIDE_ZERO = 0;
    const LONG MAX_NUMBER_OF_SAMPLE = 12;
    const double RESET = 0;

    double tmp = (double)EndInhPress - (double)EndExhPress;
    double TidalVol = (double)ExhTidalVol;

    if(tmp < 0)
    {
        tmp = -tmp;
    }
    if(DEVIDE_ZERO != tmp)
    {
        Compliance = ((TidalVol/(tmp))*((double)SCALE));
    }
    else
    {
        Compliance = RESET;
    }
    //insert Compliance to ComplianceSample Array
    ComplianceSample[ComplianceSampleIx] = Compliance;
    ComplianceSampleIx++;
    ComplianceSampleIx = ComplianceSampleIx % NUM_OF_SAMPLE_COMPLIANCE;
    if(mNumOfComplianceSample < NUM_OF_SAMPLE_COMPLIANCE)
    {
        mNumOfComplianceSample++;
    }

    E_PhaseId phaseId = PhaseMgrGetCurrentPhaseId();
#ifndef VA_WITH_PS
    if(phaseId != eSpontInhPhase)
    {
#endif
        if((DEVIDE_ZERO != tmp) && (sample12_fill_flg_comp == true))
        {
            ComplianceArray12[ComplianceIx12] = ((TidalVol/(tmp))*((double)SCALE));
        }
        else
        {
            ComplianceArray12[ComplianceIx12] = RESET;
        }

        if(sample12_fill_flg_comp == true)
        {
            if(ComplianceNum12 < MAX_NUMBER_OF_SAMPLE)
            {
                ComplianceNum12++;
            }
        }
        ComplianceIx12++;

        if(ComplianceIx12 >= TWELVE_SAMPLES)
        {
            sample12_fill_flg_comp = true;
        }
        ComplianceIx12= ComplianceIx12 % TWELVE_SAMPLES;
#ifndef VA_WITH_PS
    }
#endif
    prevprevphaseId_comp = prevphaseId_comp;
    prevphaseId_comp = phaseId;
}

void Ptdata::CompAverageCompliance(void)
{
    double minCompliance;
    double maxCompliance;
    double Sum = 0.0;
    const SHORT MIN_NUMBER_OF_COMPLIANCE_SAMPLE = 1;

    if(mNumOfComplianceSample < (NUM_OF_SAMPLE_COMPLIANCE))
    {
        minCompliance = GetMinCompliance(mNumOfComplianceSample);

        for(int i = 0; i < mNumOfComplianceSample; i++)
        {
            Sum += ComplianceSample[i];
        }
        if(mNumOfComplianceSample == MIN_NUMBER_OF_COMPLIANCE_SAMPLE)
        {
            ComplianceAverage = Sum;
        }
        else
        {
            ComplianceAverage = ((Sum - minCompliance)/((double)mNumOfComplianceSample - 1.0));
        }
    }
    else
    {
        minCompliance = GetMinCompliance(NUM_OF_SAMPLE_COMPLIANCE);
        maxCompliance = GetMaxCompliance(NUM_OF_SAMPLE_COMPLIANCE);

        for(int i = 0; i < NUM_OF_SAMPLE_COMPLIANCE; i++)
        {
            Sum += ComplianceSample[i];
        }
        ComplianceAverage = ((Sum - minCompliance - maxCompliance)/((double)NUM_OF_SAMPLE_COMPLIANCE - 2.0));
    }
}

void Ptdata::CompLeakCompensate(void)
{
    const double DEVIDE_FOR_ZERO = 0;
    const double EXH_VOL_LIMIT = 0;
    const double EXH_LEAK_LIMIT = 0;

    double Te = (double)MgrBreathDataGetLeakVolTime()/1000.0;//[s]
    double InhVolOneBRTime = MgrBreathDataGetInhVolOneBRTime();
    double ExhVolOneBRTime = (-1) * MgrBreathDataGetExhVolOneBRTime();

    double InhVol = InhVolOneBRTime/(double)SCALE;
    double ExhVol = ExhVolOneBRTime/(double)SCALE;

    if(ExhVol < EXH_VOL_LIMIT)
    {
        ExhVol = EXH_VOL_LIMIT;
    }

    double ExhLeak;
    if(DEVIDE_FOR_ZERO != Te)
    {
        ExhLeak = (((InhVol - ExhVol)/Te)*60.0)/1000.0;//[lpm]
    }
    else
    {
        ExhLeak = EXH_LEAK_LIMIT;
    }
    //case of leak ratio less than 0%
    if(ProxymalLeakValue <=0 )
    {
        ExhLeak = EXH_LEAK_LIMIT;
    }

    if(ExhLeak < EXH_LEAK_LIMIT)
    {
        ExhLeak = EXH_LEAK_LIMIT;
    }

    ExhLeakArray[ExhLeakIx] = ExhLeak;
    ExhLeakIx++;
    ExhLeakIx = ExhLeakIx % EIGHT_SAMPLES;

    MgrBreathDataResetVol();
}
void Ptdata::DoEndExhPSInAPRV(void)
{
    const LONG EXH_TIDAL_VOL_LIMIT = 8;
    const LONG EXH_TIDAL_VOL_LIMIT_TO_ZERO = 0;


    VteInAPRV = MgrBreathDataGetPdBreathDataPtrGetVte();
    ExhTidalVol = VteInAPRV;
    ExhTidalVolArray[ExhTidalVolIx] = ExhTidalVol;
    ExhTidalVolIx++;
    ExhTidalVolIx = ExhTidalVolIx % EIGHT_SAMPLES;

    if(ExhTidalVolNum < EXH_TIDAL_VOL_LIMIT)
    {
        ExhTidalVolNum++;
    }

    // Limit volume to 0
    if (ExhTidalVol < EXH_TIDAL_VOL_LIMIT_TO_ZERO)
    {
        ExhTidalVol = EXH_TIDAL_VOL_LIMIT_TO_ZERO;
    }

    // save in history table
    PdHist[Ix].ExhTidalVol = ExhTidalVol;

    ProxyStatus Status = RequestDevice();

    // check for low exhaled minute alarm conditions
    if((Status == eCalibrated)||(Status == eResetAndRecovered))
    {
        LONG HiVteLimit = MgrBdSettingGetCurrentValue(eHighTidalVolLimit);
        LONG LowVteLimit = MgrBdSettingGetCurrentValue(eLowTidalVolLimit);

        PdAlarmsCheckHiCondition(HiVteLimit, ExhTidalVol, eHighVte);
        PdAlarmsCheckLowCondition(LowVteLimit, ExhTidalVol, eLowVte);
    }

    //    CompExhMinuteVolume();
    //    CompCompliance_InAPRV();
    //    CompAverageCompliance();

    if((Status==eCalibrated)||(Status==eResetAndRecovered))
    {
        //        PatientDataGUIEvent PMsg;
        //        PMsg.Value = Compliance;
        //        MainHandlerDelegatepostCustomEvent<PatientDataGUIEvent>(eCompliancePtDataID, PMsg);
        //        PMsg.Value = VteInAPRV;
        //        MainHandlerDelegatepostCustomEvent<PatientDataGUIEvent>(eExpTidalVolumePtDataID, PMsg);
        //        PMsg.Value = ExhMinuteVol;
        //        MainHandlerDelegatepostCustomEvent<PatientDataGUIEvent>(eExpMinuteVolumePtDataID, PMsg);
    }
    else
    {
        //        PatientDataGUIEvent PMsg;
        //        PMsg.Value = BLANKED_LONG_VALUE;
        //        MainHandlerDelegatepostCustomEvent<PatientDataGUIEvent>(eCompliancePtDataID, PMsg);
        //        PMsg.Value = BLANKED_LONG_VALUE;
        //        MainHandlerDelegatepostCustomEvent<PatientDataGUIEvent>(eExpTidalVolumePtDataID, PMsg);
        //        PMsg.Value = BLANKED_LONG_VALUE;
        //        MainHandlerDelegatepostCustomEvent<PatientDataGUIEvent>(eExpMinuteVolumePtDataID, PMsg);
    }
}

void Ptdata::DoEndInhPSInAPRV(void)
{
    //    GetInhalationData();
    //    CompInhMinuteVolume();

    //    InhMsg.PeakInspiratoryPressure = PeakInhPress;
    //    InhMsg.EndInhalationPressure = EndInhPress;

    if(InhType == eSupported)
    {
        LipLimit = (LONG) MgrBdSettingGetCurrentValue(eAPRVHighInhPress)
                                                                                 + (LONG) MgrBdSettingGetCurrentValue(eSupportPress) - 200;
    }
    else if(InhType == eSpontaneous)
    {
        LipLimit = (LONG) MgrBdSettingGetCurrentValue(eAPRVLowInhPress) - 200;
    }

    HipLimit = (LONG) MgrBdSettingGetCurrentValue(eHipLimit);

    PdAlarmsCheckLowPip(LipLimit, PeakInhPress, eLowInspPressure);
    PdAlarmsCheckHiPip(HipLimit, PeakInhPress,  eHip);

    ProxyStatus Status = RequestDevice();

    if((Status == eCalibrated)||(Status == eResetAndRecovered))
    {
        //        InhMsg.InhaledTidalVolume = mInhVol;
        //        InhMsg.InhaledMinuteVolume = InhMinuteVol;
    }
    else
    {
        //        InhMsg.InhaledTidalVolume = BLANKED_LONG_VALUE;
        //        InhMsg.InhaledMinuteVolume = BLANKED_LONG_VALUE;
    }

    //    guiPtr->postCustomEvent<EndInhalationDataGUIEvent>(eEndInhalationData, InhMsg);
}

void Ptdata::DoPEEP(void)
{
    const SHORT CURRENP_LIMIT = 0;
    //Update PEEP data
    //    PatientDataGUIEvent PEEPMsg;

    SHORT currenP = MgrBreathDataGetPdBreathDataPtrGetEndExhPress();

    if(currenP < CURRENP_LIMIT)
    {
        currenP = CURRENP_LIMIT;
    }
    //        PEEPMsg.Value = currenP;
    //    MainHandlerDelegatepostCustomEvent<PatientDataGUIEvent>(ePEEPPtDataID, PEEPMsg);

    //Update PEEP Alarm
    SHORT HiPLimit = MgrBdSettingGetCurrentValue(eHighBaselineLimit);
    SHORT LowPLimit = MgrBdSettingGetCurrentValue(eLowBaselineLimit);

    PdAlarmsCheckHiCondition(HiPLimit,currenP,eHighPeep);
    PdAlarmsCheckLowCondition(LowPLimit,currenP,eLowPeep);
}

void Ptdata::DoNCPAP(void)
{
    const int ALARM_SENSITIVITY = 3;
    SHORT TempValuemNCPAPPressure;

    DeviceInterfaceRequestDevice(ePressureSensorGetLastReading);
    mNCPAP_Pressure = (LONG)TempValuemNCPAPPressure;

    LONG HiLimit = MgrBdSettingGetCurrentValue(eHighNCPAPLimit);
    LONG LowLimit = MgrBdSettingGetCurrentValue(eLowNCPAPLimit);

    //        NCPAPMsg.Value = mNCPAP_Pressure;

    //        guiPtr->postCustomEvent<PatientDataGUIEvent>(eNCPAPPtDataID, NCPAPMsg);

    PdAlarmsCheckHiCondition(mNCPAP_Pressure,eHighNCPAP,eLowNCPAP);
    PdAlarmsCheckLowCondition(mNCPAP_Pressure,eHighNCPAP,eLowNCPAP);
}

void Ptdata::DoAPRV_PHigh(void)
{
    LONG liplimit = 0;
    const LONG RESET = 0;
    const LONG OFFSET = 200;

    APRV_EndInhPress = MgrBreathDataGetPdBreathDataPtrGetEndInhPress();

    LONG endPLowPress = (LONG)MgrBreathDataGetBdBreathDataPtrGetAPRV_PHigh();
    SHORT HiPLimit = MgrBdSettingGetCurrentValue(eAPRVHighPHighLimit);
    SHORT LowPLimit = MgrBdSettingGetCurrentValue(eAPRVLowPHighLimit);

    PdAlarmsCheckHighPHigh5Time(HiPLimit,endPLowPress,eAPRVHigh_PHigh);
    PdAlarmsCheckLowPHigh5Time(LowPLimit,endPLowPress,eAPRVLow_PHigh);

    if(eActive == AlarmTaskGetIsActive(eAPRVHigh_PHigh))
    {
        AlarmTaskSetIsActive(eAPRVHigh_PHigh, eNotActive);
    }

    //    DoInhalationData();

    if(InhType == eSupported)
    {
        liplimit = (LONG) MgrBdSettingGetCurrentValue(eAPRVHighInhPress)
                                                           + (LONG) MgrBdSettingGetCurrentValue(eSupportPress) - OFFSET;
    }
    else if(InhType == eSpontaneous)
    {
        liplimit = (LONG) MgrBdSettingGetCurrentValue(eAPRVLowInhPress) - OFFSET;
    }
    else
    {
        liplimit = RESET;
    }

    PdAlarmsCheckLowPip(liplimit, PeakInhPress, eLowInspPressure);
}
void Ptdata::DoAPRV_PLow(void)
{
    PdAlarmsResetPLowAlarmActiveTime();
    LONG endPLowPress = (LONG)MgrBreathDataGetPdBreathDataPtrGetAPRV_PLow();

    SHORT HiPLimit = MgrBdSettingGetCurrentValue(eAPRVHighPLowLimit);
    SHORT LowPLimit = MgrBdSettingGetCurrentValue(eAPRVLowPLowLimit);

    PdAlarmsCheckHighPLow5Time(HiPLimit,endPLowPress,eAPRVHigh_PLow);
    PdAlarmsCheckLowPLow5Time(LowPLimit,endPLowPress,eAPRVLow_PLow);

    //    GetExhalationData();
    //    CompProxymalLeak();
    //    CompNumBreaths();
    //    CompExhTidalVolumeInAPRV();
    //    CompExhMinuteVolume();
    //    CompMeanAirwayPressure();
    //    CompIeRatio();
    //    CompRespRate();
    //    CompRapidShallowBreathix();
    //    CompCompliance_InAPRV();
    //    CompAverageCompliance();
    //    CompLeakCompensate();
    //
    //    Ix = (Ix + 1) & MAX_BREATHS_MASK;
    //
    //    BuildAPRVPLow();
}

void Ptdata::DoAPRVBR(void)
{
    IsFirst_1minute = false;
    //    ExhMsg.TotalRespiratoryRate = MgrBreathDataGetAPRVTotalBreathRate()*SCALE;
    //    ExhMsg.SpontaneousRespiratoryRate = MgrBreathDataGetAPRVSpontBreathRate()*SCALE;

    //    guiPtr->postCustomEvent<EndExhalationDataGUIEvent>(eEndExhalationData, ExhMsg);

    MgrBreathDataResetAPRVBR();
}

void  Ptdata::DoHFOData(void)
{
#ifdef HFO_SYSTEM
    GetHFOData();
    BuildHFOMsg();
#endif
}
void Ptdata::GetCircuitComplianceFactor(void)
{
    //    CircuitComplianceFactor = systemConfigFile->getValue(ePtCircuitCompliance);
}
void Ptdata::ModeInducedAlarmClearing(void)
{
    //    static const UNSIGNED SOMETIME = 25 / MS_PER_TICK;
    E_ReturnStatus  stat0;

    switch (Mode)
    {

        case eSpontaneousMode:
            stat0 = TaskSetIsActive(eLowInspPressure, eNotActive);

            // allow alarms to process event
            if (stat0 == eError)
            {
                //                usleep((tasks[eAlarms].SchedulingInterval + SOMETIME)*1000);
                AlarmTaskSetIsActive(eLowInspPressure, eNotActive);
            }
            break;
        default:
            // don't clear any alarms
            break;
    };

}

LONG Ptdata::GetAverageVte(void)
{
    const LONG EXH_TIDAL_VOL_LIMIT = 6;
    const SHORT FIRST_ELEMENT = 0;
    const LONG INDEX_FOR_LOWER_VOL_LIMIT = 1;
    const LONG INDEX_FOR_HIGHER_VOL_LIMIT = 1;
    const LONG NO_ELEMENT = 0;

    LONG avg = 0;

    for (SHORT j = FIRST_ELEMENT; j < ExhTidalVolNum; ++j)
    {
        avg += ExhTidalVolArray[j];
    }

    if(ExhTidalVolNum <= EXH_TIDAL_VOL_LIMIT )
    {
        avg = avg - GetMinVteArray();

        if((ExhTidalVolNum - INDEX_FOR_LOWER_VOL_LIMIT) != NO_ELEMENT )
        {
            avg /= ((LONG)(ExhTidalVolNum - INDEX_FOR_LOWER_VOL_LIMIT));
        }
        else
        {
            avg = ExhTidalVolArray[FIRST_ELEMENT];
        }
    }
    else
    {
        avg = avg - GetMinVteArray() - GetMaxVteArray();

        if((ExhTidalVolNum - INDEX_FOR_HIGHER_VOL_LIMIT) != NO_ELEMENT )
        {
            avg /= ((LONG)(ExhTidalVolNum - INDEX_FOR_HIGHER_VOL_LIMIT));
        }
        else
        {
            avg = ExhTidalVol;
        }
    }
    return(avg);
}

LONG Ptdata::GetAverageVte12(void)
{
    LONG avg = 0;
    LONG VteArray_min[3] = {};
    LONG VteArray_max[3] = {};

    const LONG THREE_DATA = 2;
    const LONG FOUR_DATA = 3;
    const LONG FIVE_DATA = 4;
    const LONG SIX_DATA = 5;
    const LONG SEVEN_DATA = 6;

    const LONG INDEX_FOR_3DATA = 1;
    const LONG INDEX_FOR_4DATA = 2;
    const LONG INDEX_FOR_5DATA = 3;
    const LONG INDEX_FOR_6DATA = 4;
    const LONG INDEX_FOR_7DATA = 5;
    const LONG INDEX_FOR_HIGHER_7DATA = 6;


    const SHORT FIRT_ELEMENT = 0;
    const SHORT SECOND_ELEMENT = 1;
    const SHORT THIRD_ELEMENT = 2;
    const LONG NO_ELEMENT = 0;

    for (SHORT j = FIRT_ELEMENT; j < ExhTidalVolNum12; j++)
    {
        avg += ExhTidalVolArray12[j];
    }

    GetMinVteArray12(&VteArray_min[FIRT_ELEMENT]);
    GetMaxVteArray12(&VteArray_max[FIRT_ELEMENT]);

    if(true == sample12_fill_flg)
    {
        if(ExhTidalVolNum12 <= THREE_DATA) //3 data
        {
            avg = avg - VteArray_min[FIRT_ELEMENT];

            if((ExhTidalVolNum12 - INDEX_FOR_3DATA) != NO_ELEMENT)
            {
                avg= avg/((LONG)(ExhTidalVolNum12 - INDEX_FOR_3DATA));
            }
            else
            {
                avg = ExhTidalVolArray12[FIRT_ELEMENT];
            }
        }
        else if(ExhTidalVolNum12 <= FOUR_DATA) //4 data
        {
            avg = avg - VteArray_min[0] - VteArray_max[FIRT_ELEMENT];
            if((ExhTidalVolNum12 - INDEX_FOR_4DATA) != NO_ELEMENT)
            {
                avg = avg/((LONG)(ExhTidalVolNum12 - INDEX_FOR_4DATA));
            }
            else
            {
                avg = ExhTidalVolArray12[FIRT_ELEMENT];
            }
        }
        else if(ExhTidalVolNum12 <= FIVE_DATA) //5 data
        {
            avg = avg - VteArray_min[FIRT_ELEMENT] - VteArray_max[FIRT_ELEMENT] - VteArray_min[SECOND_ELEMENT];
            if((ExhTidalVolNum12 - INDEX_FOR_5DATA) != NO_ELEMENT)
            {
                avg = avg/((LONG)(ExhTidalVolNum12 - INDEX_FOR_5DATA));
            }
            else
            {
                avg = ExhTidalVolArray12[FIRT_ELEMENT];
            }
        }
        else if(ExhTidalVolNum12 <= SIX_DATA) //6 data
        {
            avg = avg - VteArray_min[0] - VteArray_max[FIRT_ELEMENT] - VteArray_min[SECOND_ELEMENT] - VteArray_max[SECOND_ELEMENT];
            if((ExhTidalVolNum12 - INDEX_FOR_6DATA) != NO_ELEMENT)
            {
                avg = avg/((LONG)(ExhTidalVolNum12 - INDEX_FOR_6DATA));
            }
            else
            {
                avg = ExhTidalVolArray12[FIRT_ELEMENT];
            }
        }
        else if(ExhTidalVolNum12 <= SEVEN_DATA) //7 data
        {
            avg = avg - VteArray_min[FIRT_ELEMENT] - VteArray_max[FIRT_ELEMENT]
                                                                  - VteArray_min[SECOND_ELEMENT] - VteArray_max[SECOND_ELEMENT]- VteArray_min[2];
            if((ExhTidalVolNum12 - INDEX_FOR_7DATA) != NO_ELEMENT)
            {
                avg = avg/((LONG)(ExhTidalVolNum12 - INDEX_FOR_7DATA));
            }
            else
            {
                avg = ExhTidalVolArray12[FIRT_ELEMENT];
            }
        }
        else
        {
            avg = avg - VteArray_min[FIRT_ELEMENT] - VteArray_max[FIRT_ELEMENT]
                                                                  - VteArray_min[SECOND_ELEMENT] - VteArray_max[SECOND_ELEMENT]
                                                                                                                - VteArray_min[THIRD_ELEMENT]- VteArray_max[THIRD_ELEMENT];

            if((ExhTidalVolNum12 - INDEX_FOR_HIGHER_7DATA) != NO_ELEMENT)
            {
                avg = avg/((LONG)(ExhTidalVolNum12 - INDEX_FOR_HIGHER_7DATA));
            }
            else
            {
                avg = ExhTidalVol;
            }
        }
    }
    else
    {
        avg = ExhTidalVol;
    }

    return(avg);
}

double Ptdata::GetAverageCompliance12(void) //for new VA
{
    double avg = 0;
    double CompArray_min[3] = {};
    double CompArray_max[3] = {};

    const LONG THREE_DATA = 2;
    const LONG FOUR_DATA = 3;
    const LONG FIVE_DATA = 4;
    const LONG SIX_DATA = 5;
    const LONG SEVEN_DATA = 6;

    const LONG INDEX_FOR_3DATA = 1;
    const LONG INDEX_FOR_4DATA = 2;
    const LONG INDEX_FOR_5DATA = 3;
    const LONG INDEX_FOR_6DATA = 4;
    const LONG INDEX_FOR_7DATA = 5;
    const LONG INDEX_FOR_HIGHER_7DATA = 6;


    const SHORT FIRT_ELEMENT = 0;
    const SHORT SECOND_ELEMENT = 1;
    const SHORT THIRD_ELEMENT = 2;
    const LONG NO_ELEMENT = 0;

    for (SHORT j = FIRT_ELEMENT; j < ComplianceNum12; j++)
    {
        avg += ComplianceArray12[j];
    }

    GetMinComplianceArray12(&CompArray_min[FIRT_ELEMENT]);
    GetMaxComplianceArray12(&CompArray_max[FIRT_ELEMENT]);

    if(sample12_fill_flg_comp == true)
    {
        if(ComplianceNum12 <= THREE_DATA) //3 data
        {
            avg = avg - CompArray_min[FIRT_ELEMENT];
            if((ComplianceNum12 - INDEX_FOR_3DATA) != 0)
            {
                avg = avg/((LONG)(ComplianceNum12 - INDEX_FOR_3DATA));
            }
            else
            {
                avg = ComplianceArray12[FIRT_ELEMENT];
            }
        }
        else if(ComplianceNum12 <= FOUR_DATA) //4 data
        {
            avg = avg - CompArray_min[0] - CompArray_max[FIRT_ELEMENT];
            if((ComplianceNum12 - INDEX_FOR_4DATA) != NO_ELEMENT )
            {
                avg = avg/((double)(ComplianceNum12 - INDEX_FOR_4DATA));
            }
            else
            {
                avg = ComplianceArray12[FIRT_ELEMENT];
            }
        }
        else if(ComplianceNum12 <= FIVE_DATA) //5 data
        {
            avg = avg - CompArray_min[FIRT_ELEMENT] - CompArray_max[FIRT_ELEMENT] - CompArray_min[SECOND_ELEMENT];
            if((ComplianceNum12 - INDEX_FOR_5DATA) != NO_ELEMENT)
            {
                avg= avg/((double)(ComplianceNum12 - INDEX_FOR_5DATA));
            }
            else
            {
                avg = ComplianceArray12[FIRT_ELEMENT];
            }
        }
        else if(ComplianceNum12 <= SIX_DATA) //6 data
        {
            avg = avg - CompArray_min[FIRT_ELEMENT] - CompArray_max[FIRT_ELEMENT]
                                                                    - CompArray_min[SECOND_ELEMENT] - CompArray_max[SECOND_ELEMENT];
            if((ComplianceNum12 - INDEX_FOR_6DATA) != NO_ELEMENT)
            {
                avg = avg/((double)(ComplianceNum12 - INDEX_FOR_6DATA));
            }
            else
            {
                avg = ComplianceArray12[FIRT_ELEMENT];
            }
        }
        else if(ComplianceNum12 <= SEVEN_DATA) //7 data
        {
            avg = avg - CompArray_min[FIRT_ELEMENT] - CompArray_max[FIRT_ELEMENT]
                                                                    - CompArray_min[SECOND_ELEMENT] - CompArray_max[SECOND_ELEMENT]- CompArray_min[THIRD_ELEMENT];
            if((ComplianceNum12 - INDEX_FOR_7DATA) != NO_ELEMENT)
            {
                avg = avg/((double)(ComplianceNum12 - INDEX_FOR_7DATA));
            }
            else
            {
                avg = ComplianceArray12[FIRT_ELEMENT];
            }
        }
        else
        {
            avg = avg - CompArray_min[FIRT_ELEMENT] - CompArray_max[FIRT_ELEMENT]
                                                                    - CompArray_min[SECOND_ELEMENT] - CompArray_max[SECOND_ELEMENT]
                                                                                                                    - CompArray_min[THIRD_ELEMENT]- CompArray_max[THIRD_ELEMENT];

            if((ComplianceNum12 - INDEX_FOR_HIGHER_7DATA) != NO_ELEMENT )
            {
                avg = avg/((double)(ComplianceNum12 - INDEX_FOR_HIGHER_7DATA));
            }
            else
            {
                avg = Compliance;
            }
        }
    }
    else
    {
        avg = Compliance;
    }

    return(avg);
}
void Ptdata::ClearVteBuffer(void)
{
    const LONG RESET = 0;
    const int FIRST_ELEMENT = 0;

    for(int i = FIRST_ELEMENT; i < ExhTidalVolNum; i++)
    {
        ExhTidalVolArray[i] = RESET;
    }
    ExhTidalVolNum = RESET;
    ExhTidalVolIx = RESET;
    ExhTidalVolArray[FIRST_ELEMENT] = ExhTidalVol;

    ExhTidalVolIx++;
    ExhTidalVolIx = ExhTidalVolIx % EIGHT_SAMPLES;
    ExhTidalVolNum++;
    //for new VA
    for(int i = FIRST_ELEMENT; i < ExhTidalVolNum12; i++)
    {
        ExhTidalVolArray12[i] = RESET;
        ComplianceArray12[i] = RESET;
    }
    ExhTidalVolNum12 = RESET;
    ExhTidalVolIx12 = RESET;
    ExhTidalVolArray12[FIRST_ELEMENT] = LastMandExhVt;

    ComplianceNum12 = RESET;
    ComplianceIx12 = RESET;
    if(EndInhPress - EndExhPress != 0)
    {
        ComplianceArray12[FIRST_ELEMENT] = (double)SCALE*LastMandExhVt/(EndInhPress-EndExhPress);
    }

    sample12_fill_flg = false;
}
LONG Ptdata::GetEndExhFlow_Average(void)
{
    LONG avg = 0;

    for (SHORT j=0; j < EIGHT_SAMPLES; j++)
    {
        avg += EndExhFlowArray[j];
    }

    avg /= EIGHT_SAMPLES;

    return(avg);
}
double Ptdata::GetExhLeak(void)
{
    double avg = 0.0;
    const double RESET = 0;

    for (SHORT j = 0; j < EIGHT_SAMPLES; j++)
    {
        avg += ExhLeakArray[j];
    }

    avg /= (double)EIGHT_SAMPLES;

    if(avg < RESET)
    {
        avg = RESET;
    }

    return(avg);
}
void Ptdata::ClearExhLeakArray(void)
{
    for(int i = 0;i < EIGHT_SAMPLES; i++)
    {
        ExhLeakArray[i] = 0;
    }
}
void Ptdata::CompExhTidalVolumeInAPRV(void)
{
    const LONG MAX_NUMBER_OF_VOL = 8;
    const LONG LIMIT_VOL_TO_ZERO = 0;

    VteInAPRV = MgrBreathDataGetPdBreathDataPtrGetVte();
    ExhTidalVol = VteInAPRV;// - ComplVolume;

    ExhTidalVolArray[ExhTidalVolIx]  = ExhTidalVol;
    ExhTidalVolIx++;
    ExhTidalVolIx = ExhTidalVolIx % EIGHT_SAMPLES;

    if(ExhTidalVolNum < MAX_NUMBER_OF_VOL)
    {
        ExhTidalVolNum++;
    }

    // Limit volume to 0
    if (ExhTidalVol < LIMIT_VOL_TO_ZERO)
    {
        ExhTidalVol = LIMIT_VOL_TO_ZERO;
    }

    PdHist[Ix].ExhTidalVol = ExhTidalVol;

    ProxyStatus Status = RequestDevice();

    if((Status == eCalibrated)||(Status == eResetAndRecovered))
    {
        // check for low exhaled minute alarm conditions
        LONG HiVteLimit = MgrBdSettingGetCurrentValue(eHighTidalVolLimit);
        LONG LowVteLimit = MgrBdSettingGetCurrentValue(eLowTidalVolLimit);

        PdAlarmsCheckHiCondition(HiVteLimit,ExhTidalVol, eHighVte);
        PdAlarmsCheckLowCondition(LowVteLimit,ExhTidalVol, eLowVte);
    }
}
void Ptdata::CompCompliance_InAPRV(void)
{
    const double DEVIDE_FOR_ZERO = 0;
    const double RESET = 0;

    double tmp = (double)APRV_EndInhPress - (double)MgrBreathDataGetBdBreathDataPtrGetAPRV_PLow();

    if(tmp < 0)
    {
        tmp = -tmp;

    }
    if(DEVIDE_FOR_ZERO != tmp)
    {
        Compliance = ((VteInAPRV/(tmp))*((double)SCALE));
    }
    else
    {
        Compliance = RESET;
    }
}
double Ptdata::GetMinCompliance(SHORT index)
{
    LONG min = numeric_limits<long>::max();

    for(int i = 0; i < index; i++)
    {
        if(ComplianceSample[i] < min)
        {
            min = ComplianceSample[i];
        }
    }
    return min;
}
double Ptdata::GetMaxCompliance(SHORT index)
{
    LONG max = 0;
    for(int i = 0;i < index; i++)
    {
        if(ComplianceSample[i] > max)
        {
            max = ComplianceSample[i];
        }
    }
    return max;
}

void Ptdata::GetHFOData(void)
{
    //    map           = HfoController->GetFilterPressIntegrator();
    //    maxpress      = mgrBdPtr->GetMaxPresData();
    //    minpress      = mgrBdPtr->GetMinPresData();
    //    amplitude      = maxpress - minpress;
    //    hfoBaseFlow = mgrBdPtr->GetHFOBaseFlow();
}

void Ptdata::BuildHFOMsg(void)
{

    //    HFOMsg.MeanAirPressure = map;
    //    HFOMsg.Freshgas = hfoBaseFlow;
    //    HFOMsg.MaxPress = maxpress;
    //    HFOMsg.MinPress = minpress;
    //    HFOMsg.Amplitude = amplitude;
    //
    //    HFOMsg.ManInspHoldTime = manInspHoldTime;
    //    HFOMsg.TidalVolume = hfoVe;
    //
    //    guiPtr->postCustomEvent<HFODataGUIEvent>(eHFOData, HFOMsg);
}
void Ptdata::CalculateHFOExh(void)
{
    const int COUNT_MAX = 500;
    const int RESET = 0;
    static int count = 0;

    if(count > COUNT_MAX)
    {
        hfoExhFlow = RESET;
        count = RESET;
    }
    else
    {
        hfoExhFlow += MgrBreathDataGetDeliveredFlow();
        count++;
    }
}

void Ptdata::BuildAPRVPLow(void)
{
    //    ExhMsg.MeanAirwayPressure = MeanAirwayPressAvg;
    //    ExhMsg.SpontaneousMinuteVolume = SpontMinuteVol;
    //    if(IsFirst_1minute)
    //    {
    //        ExhMsg.TotalRespiratoryRate = MgrBreathDataGetAPRVTotalBreathRate()*SCALE;
    //        ExhMsg.SpontaneousRespiratoryRate = MgrBreathDataGetAPRVSpontBreathRate()*SCALE;
    //    }
    //    ExhMsg.IERatio = IeRatio;
    //    ExhMsg.RapidShallowBreathingIndex = RapidShallowBreathix;
    //    ExhMsg.TimeInExhalation = TimeInExh;
    //    ExhMsg.ProxymalLeak = ProxymalLeakValue*100.0;
    //    ExhMsg.APRVPLow = (LONG)bdPtr->GetAPRV_PLow();
    //
    //    ProxyStatus Status = proxySensor->getLastStatus();
    //    if((Status == eCalibrated)||(Status == eResetAndRecovered))
    //    {
    //        ExhMsg.ExhaledMinuteVolume = ExhMinuteVol;
    //        ExhMsg.TidalVolume = VteInAPRV;
    //        ExhMsg.Compliance = Compliance;
    //    }
    //    else
    //    {
    //        ExhMsg.TidalVolume = BLANKED_LONG_VALUE;
    //        ExhMsg.ExhaledMinuteVolume = BLANKED_LONG_VALUE;
    //        ExhMsg.Compliance = BLANKED_LONG_VALUE;
    //    }
    //    guiPtr->postCustomEvent<EndExhalationDataGUIEvent>(eEndAPRVPLowData, ExhMsg);
}
void Ptdata::GetEndExhFlow(void)
{
    EndExhFlowArray[EndExhFlowIx] = MgrBreathDataGetBdBreathDataPtrGetEndExhFlow();
    EndExhFlowIx++;
    EndExhFlowIx = EndExhFlowIx % EIGHT_SAMPLES;
}
LONG Ptdata::GetMinVteArray(void)
{
    LONG min = numeric_limits<long>::max();
    for(int i = 0; i < ExhTidalVolNum; i++)
    {
        if(ExhTidalVolArray[i] < min)
        {
            min = ExhTidalVolArray[i];
        }
    }
    return min;
}
LONG Ptdata::GetMaxVteArray(void)
{
    LONG max = 0;
    for(int i = 0; i < ExhTidalVolNum; i++)
    {
        if(ExhTidalVolArray[i] > max)
        {
            max = ExhTidalVolArray[i];
        }
    }
    return max;
}
void Ptdata::GetMinComplianceArray12(double *add) //for new VA
{
    double min1, min2, min3;
    short pos1, pos2;
    min1 = min2 = min3 = numeric_limits<long>::max(); //min1:small - min3:large
    pos1 = pos2 = 0;

    for(int i = 0; i < ComplianceNum12; i++)
    {
        if(ComplianceArray12[i] <= min1)
        {
            min1 = ComplianceArray12[i];
            pos1 = i;
        }
    }

    for(int i = 0; i < ComplianceNum12; i++)
    {
        if(ComplianceArray12[i] <= min2)
        {
            if(pos1 != i)
            {
                min2 = ComplianceArray12[i];
                pos2 = i;
            }
        }
    }

    for(int i = 0; i < ComplianceNum12; i++)
    {
        if(ComplianceArray12[i] <= min3)
        {
            if(pos1 != i && pos2 != i)
            {
                min3 = ComplianceArray12[i];
            }
        }
    }

    if(min1 == numeric_limits<long>::max())
    {
        min1 = 0;
    }

    if(min2 == numeric_limits<long>::max())
    {
        min2 = 0;
    }

    if(min3 == numeric_limits<long>::max())
    {
        min3 = 0;
    }

    *add = min1;
    *(add+1) = min2;
    *(add+2) = min3;
}
void Ptdata::GetMaxComplianceArray12(double *add) //for new VA
{

    //    DOUBLE max1, max2, max3;
    //    short pos1, pos2;
    //    max1 = max2 = max3 = 0; //max1:large - max3:small
    //    pos1 = pos2 = 0;
    //
    //    for(int i = 0; i < ComplianceNum12; i++)
    //    {
    //        if(ComplianceArray12[i] >= max1)
    //        {
    //            max1 = ComplianceArray12[i];
    //            pos1 = i;
    //        }
    //    }
    //
    //    for(int i = 0; i < ComplianceNum12; i++)
    //    {
    //        if(ComplianceArray12[i] >= max2)
    //        {
    //            if(pos1 != i)
    //            {
    //                max2 = ComplianceArray12[i];
    //                pos2 = i;
    //            }
    //        }
    //    }
    //
    //    for(int i = 0; i < ComplianceNum12; i++)
    //    {
    //        if(ComplianceArray12[i] >= max3)
    //        {
    //            if(pos1 != i && pos2 != i)
    //            {
    //                max3 = ComplianceArray12[i];
    //            }
    //        }
    //    }
    //
    //    *add = max1;
    //    *(add+1) = max2;
    //    *(add+2) = max3;

}
void Ptdata::GetMinVteArray12(LONG *add)//for new VA
{
    LONG min1, min2, min3;
    short pos1, pos2;
    min1 = min2 = min3 = numeric_limits<long>::max(); //min1:small - min3:large
    pos1 = pos2 = 0;

    for(int i = 0; i < ExhTidalVolNum12; i++)
    {
        if(ExhTidalVolArray12[i] <= min1)
        {
            min1 = ExhTidalVolArray12[i];
            pos1 = i;
        }
    }

    for(int i = 0; i < ExhTidalVolNum12; i++)
    {
        if(ExhTidalVolArray12[i] <= min2)
        {
            if(pos1 != i)
            {
                min2 = ExhTidalVolArray12[i];
                pos2 = i;
            }
        }
    }

    for(int i = 0; i < ExhTidalVolNum12; i++)
    {
        if(ExhTidalVolArray12[i] <= min3)
        {
            if(pos1 != i && pos2 != i)
            {
                min3 = ExhTidalVolArray12[i];
            }
        }
    }

    if(min1 == numeric_limits<long>::max())
    {
        min1 = 0;
    }

    if(min2 == numeric_limits<long>::max())
    {
        min2 = 0;
    }

    if(min3 == numeric_limits<long>::max())
    {
        min3 = 0;
    }

    *add = min1;
    *(add+1) = min2;
    *(add+2) = min3;
}

void Ptdata::GetMaxVteArray12(LONG *add) //for new VA
{
    LONG max1, max2, max3;
    short pos1, pos2;
    max1 = max2 = max3 = 0; //max1:large - max3:small
    pos1 = pos2 = 0;

    for(int i = 0; i < ExhTidalVolNum12; i++)
    {
        if(ExhTidalVolArray12[i] >= max1)
        {
            max1 = ExhTidalVolArray12[i];
            pos1 = i;
        }
    }

    for(int i = 0; i < ExhTidalVolNum12; i++)
    {
        if(ExhTidalVolArray12[i] >= max2)
        {
            if(pos1 != i)
            {
                max2 = ExhTidalVolArray12[i];
                pos2 = i;
            }
        }
    }

    for(int i = 0; i < ExhTidalVolNum12; i++)
    {
        if(ExhTidalVolArray12[i] >= max3)
        {
            if(pos1 != i && pos2 != i)
            {
                max3 = ExhTidalVolArray12[i];
            }
        }
    }

    *add = max1;
    *(add+1) = max2;
    *(add+2) = max3;
}

E_Bool Ptdata::PdSpontaneous (E_BreathPhaseIndicator inhtype)
{
    E_Bool retval;

    if((inhtype == eSpontaneous) || (inhtype == eSupported))
        retval = eTrue;
    else
        retval = eFalse;

    return (retval);
}

E_Bool Ptdata::PdMandatory (E_BreathPhaseIndicator inhtype)
{
    E_Bool retval;

    if((inhtype == eAssisted) || (inhtype == eMandatory))
        retval = eTrue;
    else
        retval = eFalse;

    return (retval);
}

double Ptdata::GetComplianceAverage(void)
{
    return ComplianceAverage;
}

LONG Ptdata::GetVte(void)
{
    return ExhTidalVol;
}

float Ptdata::GetProxymalLeak(void)
{
    return ProxymalLeakValue;
}

LONG Ptdata::GetComplVolumne(void)
{
    return ComplVolume;
}

LONG Ptdata::GetComplianceFactor(void)
{
    return CircuitComplianceFactor;
}

ULONG Ptdata::GetTimeInInh(void)
{
    return TimeInInh;
}

LONG Ptdata::GetLastMandExhVt(void)
{
    return LastMandExhVt;
}

E_PhaseId Ptdata::GetPrevPhaseId(void)
{
    return prevphaseId_vt;
}

E_PhaseId Ptdata::GetPrevprevPhaseId(void)
{
    return prevprevphaseId_vt;
}

class PtDataUnitMock : public PDTestFixture
{
public:
    PtDataUnitMock(void) : PDTestFixture(new BaseModuleMock)
{
}
    virtual ~PtDataUnitMock(void){}
    virtual void SetUp(void)
    {
        MyPtdata = new Ptdata();
    }
    virtual void TearDown(void)
    {
        delete MyPtdata;
    }
    Ptdata *MyPtdata;
};

//test for Ptdata* Ptdata::S_GetInstance (void)
TEST_F(PtDataUnitMock, PtDataUnitMock_S_GetInstance)
{
    EXPECT_NE((void*)0, MyPtdata->S_GetInstance());
}

//test for void Ptdata::CompNumBreaths (void)
TEST_F(PtDataUnitMock, PtDataUnitMock_CompNumBreaths_1)
{
    MyPtdata->Ix = 0;
    MyPtdata->PdHist[0].Indicator = eNoPhase;

    EXPECT_CALL(*_PtDatamMock, CallThis()).Times(1);

    MyPtdata->CompNumBreaths();

}
TEST_F(PtDataUnitMock, PtDataUnitMock_CompNumBreaths_2)
{
    MyPtdata->Ix = 0;
    MyPtdata->PdHist[0].Indicator = eNoVentilation;
    MyPtdata->PdHist[0].InhTime = MSECPERMIN + 1;

    EXPECT_CALL(*_PtDatamMock, CallThis()).Times(2);

    MyPtdata->CompNumBreaths();
}

TEST_F(PtDataUnitMock, PtDataUnitMock_CompNumBreaths_3)
{
    MyPtdata->Ix = 0;
    MyPtdata->PdHist[0].Indicator = eSpontaneous;
    EXPECT_CALL(*_PtDatamMock, CallThis()).Times(2);

    MyPtdata->CompNumBreaths();

    EXPECT_EQ(1, MyPtdata->breathsofData);
}

//test for void Ptdata::CompCircuitComplianceVolume (void)
TEST_F(PtDataUnitMock, PtDataUnitMock_CompCircuitComplianceVolume_positive)
{
    MyPtdata->CircuitComplianceFactor = 100;
    MyPtdata->PeepAdjEndInhPress = 100;

    MyPtdata->CompCircuitComplianceVolume();

    EXPECT_EQ(MyPtdata->ComplVolume, 100*100 / SCALE);

}
TEST_F(PtDataUnitMock, PtDataUnitMock_CompCircuitComplianceVolume_negative)
{
    MyPtdata->CircuitComplianceFactor = -100;
    MyPtdata->PeepAdjEndInhPress = 100;

    MyPtdata->CompCircuitComplianceVolume();

    EXPECT_EQ(MyPtdata->ComplVolume, 0);
}

//test for void Ptdata::CompExhTidalVolume (void)
TEST_F(PtDataUnitMock, PtDataUnitMock_CompExhTidalVolume_1)
{
    const LONG MUX_NUMBER_OF_EXH_VOL = 8;

    MyPtdata->ExhTidalVolNum = 1;
    MyPtdata->CompExhTidalVolume();

    EXPECT_EQ(2, MyPtdata->ExhTidalVolNum);

    MyPtdata->ExhTidalVolNum = MUX_NUMBER_OF_EXH_VOL;
    MyPtdata->CompExhTidalVolume();
    EXPECT_EQ(MUX_NUMBER_OF_EXH_VOL, MyPtdata->ExhTidalVolNum);

}
TEST_F(PtDataUnitMock, PtDataUnitMock_CompExhTidalVolume_2)
{
    EXPECT_CALL(*_PtDatamMock, PhaseMgrGetCurrentPhaseId()).Times(1);
    MyPtdata->CompExhTidalVolume();
}
TEST_F(PtDataUnitMock, PtDataUnitMock_CompExhTidalVolume_3)
{
    MyPtdata->prevphaseId_vt = eMandInhPhase;
    MyPtdata->sample12_fill_flg = eMandInhPhase;
    MyPtdata->ExhTidalVolIx12 = 0;
    MyPtdata->sample12_fill_flg = false;

    MyPtdata->CompExhTidalVolume();

    EXPECT_EQ(0, MyPtdata->ExhTidalVolArray12[0]);

}
TEST_F(PtDataUnitMock, PtDataUnitMock_CompExhTidalVolume_4)
{
    MyPtdata->prevphaseId_vt = eMandInhPhase;
    MyPtdata->ExhTidalVolIx12 = 0;
    MyPtdata->ExhVolume = 1;
    MyPtdata->sample12_fill_flg = true;
    for(int i = 0; i < 12; ++i)
    {
        MyPtdata->ExhTidalVolArray12[i] = 0;
    }

    MyPtdata->CompExhTidalVolume();

    EXPECT_EQ(1, MyPtdata->ExhTidalVolArray12[0]);

}
TEST_F(PtDataUnitMock, PtDataUnitMock_CompExhTidalVolume_5)
{
    MyPtdata->ExhTidalVolIx12 = TWELVE_SAMPLES;
    MyPtdata->sample12_fill_flg = false;

    MyPtdata->CompExhTidalVolume();

    EXPECT_EQ(true, MyPtdata->sample12_fill_flg);
}
TEST_F(PtDataUnitMock, PtDataUnitMock_CompExhTidalVolume_6)
{
    MyPtdata->ExhTidalVolIx12 = TWELVE_SAMPLES - 2;
    MyPtdata->sample12_fill_flg = false;

    MyPtdata->CompExhTidalVolume();

    EXPECT_EQ(false, MyPtdata->sample12_fill_flg);
}
TEST_F(PtDataUnitMock, PtDataUnitMock_CompExhTidalVolume_7)
{
    MyPtdata->ExhVolume = -1;

    MyPtdata->CompExhTidalVolume();

    EXPECT_EQ(0, MyPtdata->ExhTidalVol);
}
TEST_F(PtDataUnitMock, PtDataUnitMock_CompExhTidalVolume_8)
{
    EXPECT_CALL(*_PtDatamMock, ModeMgrGetCurrentModeId()).Times(1).WillOnce(Return(eVcvSpontMode));
    EXPECT_CALL(*_PtDatamMock, ModeMgrGetNextModeId()).Times(1).WillOnce(Return(eVcvSpontMode));
    EXPECT_CALL(*_PtDatamMock, RequestDevice()).Times(1).WillOnce(Return(eCalibrated));

    EXPECT_CALL(*_PtDatamMock, MgrBdSettingGetCurrentValue(eHighTidalVolLimit)).Times(1).WillOnce(Return(1));
    EXPECT_CALL(*_PtDatamMock, MgrBdSettingGetCurrentValue(eLowTidalVolLimit)).Times(1).WillOnce(Return(2));

    EXPECT_CALL(*_PtDatamMock, PdAlarmsCheckHiCondition(1, 3, eHighVte)).Times(1);
    EXPECT_CALL(*_PtDatamMock, PdAlarmsCheckLowCondition(2, 3, eLowVte)).Times(1);

    MyPtdata->ExhVolume = 3;

    MyPtdata->CompExhTidalVolume();
}

//test for void Ptdata::CompExhMinuteVolume (void)
TEST_F(PtDataUnitMock, PtDataUnitMock_CompExhMinuteVolume_DevideZero)
{
    MyPtdata->MinCalTime = 0;
    MyPtdata->CompExhMinuteVolume();

    EXPECT_EQ(MyPtdata->InhMinuteVol, BLANKED_LONG_VALUE);

}
TEST_F(PtDataUnitMock, PtDataUnitMock_CompExhMinuteVolume_DevideNonZero)
{
    MyPtdata->MinCalTime = 1;
    MyPtdata->breathsofData = 1;
    MyPtdata->Ix = 0;
    MyPtdata->PdHist[0].InhTidalVol = 1;

    MyPtdata->CompExhMinuteVolume();

    EXPECT_EQ(MyPtdata->InhMinuteVol, (LONG)((1 * MSECPERMIN/1000) / (1)));
}

//test for void Ptdata::UpdateMandExhTidalVolume (void)
TEST_F(PtDataUnitMock, PtDataUnitMock_UpdateMandExhTidalVolume_1)
{
    MyPtdata->NumMandBreaths = MAX_BREATHS - 2;
    MyPtdata->UpdateMandExhTidalVolume();

    EXPECT_EQ(MAX_BREATHS - 1, MyPtdata->NumMandBreaths);

}
TEST_F(PtDataUnitMock, PtDataUnitMock_UpdateMandExhTidalVolume_2)
{
    MyPtdata->NumMandBreaths = MAX_BREATHS ;
    MyPtdata->UpdateMandExhTidalVolume();

    EXPECT_EQ(MAX_BREATHS, MyPtdata->NumMandBreaths);

}
TEST_F(PtDataUnitMock, PtDataUnitMock_UpdateMandExhTidalVolume_3)
{
    MyPtdata->NumMandBreaths = MAX_BREATHS ;
    MyPtdata->ExhTidalVol = 2 ;
    MyPtdata->MandBreathsIx = 2 ;
    for(int i =0; i < 8; ++i)
    {
        MyPtdata->ManExhTidalVol[i] = 1;
    }

    MyPtdata->UpdateMandExhTidalVolume();

    EXPECT_EQ(1, MyPtdata->MandExhTidalVolAverage);
}

//test for void Ptdata::CompRapidShallowBreathix (void)

TEST_F(PtDataUnitMock, PtDataUnitMock_CompRapidShallowBreathix)
{
    const LONG LIMIT = 5 * SCALE;

    MyPtdata->VentType = eNoVentType;
    MyPtdata->Mode = eSpontaneousMode;
    MyPtdata->SupportPressure = LIMIT;

    MyPtdata->SpontRespRate = 2;
    MyPtdata->SpontMinuteVol = 4;

    MyPtdata->CompRapidShallowBreathix();

    EXPECT_EQ(MyPtdata->RapidShallowBreathix, 2*2/4);

}

TEST_F(PtDataUnitMock, PtDataUnitMock_CompRapidShallowBreathix_1)
{
    MyPtdata->VentType = eNonInvasiveVentilation;
    MyPtdata->Mode = eSpontaneousMode;
    MyPtdata->SupportPressure = MyPtdata->BaselinePressSetting;

    MyPtdata->SpontRespRate = 2;
    MyPtdata->SpontMinuteVol = 4;

    MyPtdata->CompRapidShallowBreathix();

    EXPECT_EQ(MyPtdata->RapidShallowBreathix, 2*2/4);
}
TEST_F(PtDataUnitMock, PtDataUnitMock_CompRapidShallowBreathix_2)
{
    const LONG LIMIT = 5 * SCALE;

    MyPtdata->VentType = eNoVentType;
    MyPtdata->Mode = eSpontaneousMode;
    MyPtdata->SupportPressure = LIMIT + 4;

    MyPtdata->SpontRespRate = 2;
    MyPtdata->SpontMinuteVol = 4;

    MyPtdata->CompRapidShallowBreathix();

    EXPECT_EQ(MyPtdata->RapidShallowBreathix, BLANKED_LONG_VALUE);

}

//test for void Ptdata::CompIeRatio (void)
TEST_F(PtDataUnitMock, PtDataUnitMock_CompIeRatio_1)
{
    MyPtdata->TimeInExh = 4;
    MyPtdata->TimeInInh = 2;
    MyPtdata->CompIeRatio();

    EXPECT_EQ(MyPtdata->IeRatio, -1 * (LONG) ((4 * SCALE) / 2));

}
TEST_F(PtDataUnitMock, PtDataUnitMock_CompIeRatio_2)
{
    MyPtdata->TimeInExh = 1;
    MyPtdata->TimeInInh = 0;
    MyPtdata->CompIeRatio();

    EXPECT_EQ(MyPtdata->IeRatio, BLANKED_LONG_VALUE);

}

TEST_F(PtDataUnitMock, PtDataUnitMock_CompIeRatio_3)
{
    MyPtdata->TimeInExh = 2;
    MyPtdata->TimeInInh = 4;
    MyPtdata->CompIeRatio();

    EXPECT_EQ(MyPtdata->IeRatio, ((4 * SCALE) / 2));

}
TEST_F(PtDataUnitMock, PtDataUnitMock_CompIeRatio_4)
{
    MyPtdata->TimeInExh = 0;
    MyPtdata->TimeInInh = 4;
    MyPtdata->CompIeRatio();

    EXPECT_EQ(MyPtdata->IeRatio, BLANKED_LONG_VALUE);

}

//test for void Ptdata::CompMeanAirwayPressure (void)
TEST_F(PtDataUnitMock, PtDataUnitMock_CompMeanAirwayPressure_1)
{
    MyPtdata->NumInhSamples = 0;
    MyPtdata->NumExhSamples = 0;
    MyPtdata->Ix = 0;

    MyPtdata->CompMeanAirwayPressure();

    EXPECT_EQ(MyPtdata->PdHist[0].MeanAirwayPress, 0);

}
TEST_F(PtDataUnitMock, PtDataUnitMock_CompMeanAirwayPressure_2)
{
    MyPtdata->NumInhSamples = 1;
    MyPtdata->NumExhSamples = 1;
    MyPtdata->PressureSum = 4;
    MyPtdata->Ix = 0;

    MyPtdata->CompMeanAirwayPressure();

    EXPECT_EQ(MyPtdata->PdHist[0].MeanAirwayPress, 4/(1+1));
}
TEST_F(PtDataUnitMock, PtDataUnitMock_CompMeanAirwayPressure_3)
{
    MyPtdata->breathsofData = 0;

    MyPtdata->CompMeanAirwayPressure();

    EXPECT_EQ(MyPtdata->MeanAirwayPressAvg, BLANKED_LONG_VALUE);
}
TEST_F(PtDataUnitMock, PtDataUnitMock_CompMeanAirwayPressure_4)
{
    MyPtdata->breathsofData = 1;
    MyPtdata->Ix = 0;
    MyPtdata->PdHist[0].MeanAirwayPress = 1;

    MyPtdata->CompMeanAirwayPressure();

    EXPECT_EQ(MyPtdata->MeanAirwayPressAvg, 0);
}

//test for void Ptdata::CompRespRate (void)
TEST_F(PtDataUnitMock, PtDataUnitMock_CompRespRate_1)
{
    MyPtdata->MinCalTime = 0;

    MyPtdata->CompRespRate();

    EXPECT_EQ(MyPtdata->TotalRespRate, BLANKED_LONG_VALUE);
    EXPECT_EQ(MyPtdata->SpontRespRate, BLANKED_LONG_VALUE);
}
TEST_F(PtDataUnitMock, PtDataUnitMock_CompRespRate_2)
{
    MyPtdata->MinCalTime = 1;
    MyPtdata->breathsofData = 2;
    MyPtdata->SpontBreaths = 4;

    MyPtdata->CompRespRate();

    EXPECT_EQ(MyPtdata->TotalRespRate, (2 * MSECPERMIN * SCALE) / 1);
    EXPECT_EQ(MyPtdata->SpontRespRate, (4 * MSECPERMIN * SCALE) / 1);
}

//test for void Ptdata::GetExhalationData (void)
TEST_F(PtDataUnitMock, PtDataUnitMock_GetExhalationData_1)
{
    EXPECT_CALL(*_PtDatamMock, ModeMgrGetCurrentModeId()).Times(2);
    EXPECT_CALL(*_PtDatamMock, MgrBreathDataGetPdBreathDataPtrGetPressureSum()).Times(1);
    EXPECT_CALL(*_PtDatamMock, MgrBreathDataGetPdBreathDataPtrGetCompensatedExhVolume()).Times(1);
    EXPECT_CALL(*_PtDatamMock, MgrBreathDataGetPdBreathDataPtrGetExhVolume()).Times(1);
    EXPECT_CALL(*_PtDatamMock, MgrBreathDataGetPdBreathDataPtrGetEndExhPress()).Times(1);
    EXPECT_CALL(*_PtDatamMock, MgrBreathDataGetPdBreathDataPtrGetNumExhSamples()).Times(1);
    EXPECT_CALL(*_PtDatamMock, MgrBreathDataGetPdBreathDataPtrGetTimeInExh()).Times(1);
    EXPECT_CALL(*_PtDatamMock, MgrBreathDataGetPdBreathDataPtrGetInhType()).Times(1);

    MyPtdata->GetExhalationData();
}
TEST_F(PtDataUnitMock, PtDataUnitMock_GetExhalationData_2)
{

    EXPECT_CALL(*_PtDatamMock, MgrBreathDataSetPdDone()).Times(1);
    EXPECT_CALL(*_PtDatamMock, PdAlarmsResetPEEPAlarmActiveTime()).Times(1);

    MyPtdata->GetExhalationData();
}

//test for void Ptdata::GetInhalationData (void)
TEST_F(PtDataUnitMock, PtDataUnitMock_GetInhalationData_1)
{
    EXPECT_CALL(*_PtDatamMock, MgrBreathDataGetPdBreathDataPtrGetPeakInhPress()).Times(1);
    EXPECT_CALL(*_PtDatamMock, MgrBreathDataGetPdBreathDataPtrGetEndInhPress()).Times(1);
    EXPECT_CALL(*_PtDatamMock, MgrBreathDataGetPdBreathDataPtrGetNumInhSamples()).Times(1);
    EXPECT_CALL(*_PtDatamMock, MgrBreathDataGetPdBreathDataPtrGetTimeInInh()).Times(1);
    EXPECT_CALL(*_PtDatamMock, MgrBreathDataGetPdBreathDataPtrGetInhType()).Times(1);

    EXPECT_CALL(*_PtDatamMock, MgrBdSettingGetCurrentValue(eMode)).Times(1);

    MyPtdata->GetInhalationData();
}

TEST_F(PtDataUnitMock, PtDataUnitMock_GetInhalationData_2)
{
    MyPtdata->Ix = 0;
    MyPtdata->mInhVol = 10;

    MyPtdata->GetInhalationData();

    EXPECT_EQ(MyPtdata->PdHist[0].InhTidalVol, 10);
}

//test for void Ptdata::DoInhalationData (void)
TEST_F(PtDataUnitMock, PtDataUnitMock_DoInhalationData)
{
    //this function call other function is tested
}

//test for void Ptdata::DoExhalationData (void)
TEST_F(PtDataUnitMock, PtDataUnitMock_DoExhalationData)
{
    MyPtdata->Ix = 9;

    MyPtdata->DoExhalationData();

    EXPECT_EQ(MyPtdata->Ix, (9 + 1) & MAX_BREATHS_MASK);
}

//test for void Ptdata::DoO2 (void)
TEST_F(PtDataUnitMock, PtDataUnitMock_DoO2_1)
{
    MyPtdata->O2Concentration = 0;

    EXPECT_CALL(*_PtDatamMock, PdAlarmsCheckHiO2(0,0,eHighO2)).Times(1);
    EXPECT_CALL(*_PtDatamMock, PdAlarmsCheckLowO2(0,0,eLowO2)).Times(1);


    MyPtdata->DoO2();
}
TEST_F(PtDataUnitMock, PtDataUnitMock_DoO2_2)
{
    MyPtdata->O2Concentration = O2_NOT_PRESENT;
    EXPECT_CALL(*_PtDatamMock, AlarmTaskSetIsActive(eHighO2, eNotActive)).Times(1);
    EXPECT_CALL(*_PtDatamMock, AlarmTaskSetIsActive(eLowO2, eNotActive)).Times(1);

    MyPtdata->DoO2();

    EXPECT_EQ(MyPtdata->O2Concentration,BLANKED_LONG_VALUE);
}

//test for void Ptdata::ClearHistory (void)
TEST_F(PtDataUnitMock, PtDataUnitMock_ClearHistory)
{
    MyPtdata->ClearHistory();

    for (int i = 0; i < MAX_BREATHS; i++)
    {
        EXPECT_EQ(eNoPhase, MyPtdata->PdHist[i].Indicator);
        EXPECT_EQ(0, MyPtdata->PdHist[i].InhTime);
        EXPECT_EQ(0, MyPtdata->PdHist[i].ExhTime);
        EXPECT_EQ(0, MyPtdata->PdHist[i].MeanAirwayPress);
    }

    EXPECT_EQ(true, MyPtdata->IsFirst_1minute);
    EXPECT_EQ(0, MyPtdata->MinCalTime);
    EXPECT_EQ(1, MyPtdata->NumMandBreaths);
    EXPECT_EQ(0, MyPtdata->MandBreathsIx);
    EXPECT_EQ(0, MyPtdata->SpontBreaths);
    EXPECT_EQ(0, MyPtdata->numBreaths);
    EXPECT_EQ(0, MyPtdata->breathsofData);
    EXPECT_EQ(0, MyPtdata->Ix);

    EXPECT_EQ(eFalse, MyPtdata->CircuitComplianceON);
    EXPECT_EQ(0, MyPtdata->CircuitComplianceFactor);
    EXPECT_EQ(0, MyPtdata->PeepAdjEndInhPress);
    EXPECT_EQ(0, MyPtdata->manInspHoldTime);
    EXPECT_EQ(0, MyPtdata->maxpress);
    EXPECT_EQ(0, MyPtdata->minpress);
    EXPECT_EQ(0, MyPtdata->TimeInExh);
    EXPECT_EQ(0, MyPtdata->TimeInInh);
    EXPECT_EQ(0, MyPtdata->SupportPressure);
    EXPECT_EQ(0, MyPtdata->BaselinePressSetting);

    EXPECT_EQ(0, MyPtdata->NumInhSamples);
    EXPECT_EQ(0, MyPtdata->NumExhSamples);

    EXPECT_EQ(0, MyPtdata->ExhVolumeCompensated);
    EXPECT_EQ(0, MyPtdata->PressureSum);
//    EXPECT_EQ(0, MyPtdata->EndExhPress);
//    EXPECT_EQ(0, MyPtdata->EndInhPress);
//    EXPECT_EQ(0, MyPtdata->PeakInhPress);
    EXPECT_EQ(0, MyPtdata->map);
    EXPECT_EQ(0, MyPtdata->amplitude);
    EXPECT_EQ(eStandbyMode, MyPtdata->Mode);
    EXPECT_EQ(0, MyPtdata->HighO2AlarmLimit);
    EXPECT_EQ(0, MyPtdata->LowO2AlarmLimit);
    EXPECT_EQ(0, MyPtdata->LowBaseLineAlarmLimit);
    EXPECT_EQ(0, MyPtdata->LipLimit);
    EXPECT_EQ(0, MyPtdata->HipLimit);
    EXPECT_EQ(0, MyPtdata->HemvLimit);
    EXPECT_EQ(0, MyPtdata->LetvLimit);

    EXPECT_EQ(0, MyPtdata->HrrAlarmLimit);
    EXPECT_EQ(0, MyPtdata->LemvLimit);
    EXPECT_EQ(0, MyPtdata->LestvLimit);
    EXPECT_EQ(0, MyPtdata->LemtvLimit);
//    EXPECT_EQ(0, MyPtdata->O2Concentration);
    EXPECT_EQ(0, MyPtdata->hfoBaseFlow);
    EXPECT_EQ(0, MyPtdata->MandExhTidalVolAverage);
//    EXPECT_EQ(0, MyPtdata->ExhMinuteVol);
//    EXPECT_EQ(0, MyPtdata->SpontRespRate);
//    EXPECT_EQ(0, MyPtdata->SpontMinuteVol);
    EXPECT_EQ(0, MyPtdata->DeliveredO2);
//    EXPECT_EQ(0, MyPtdata->RapidShallowBreathix);
//    EXPECT_EQ(0, MyPtdata->IeRatio);
    EXPECT_EQ(0, MyPtdata->ComplVolume);
//    EXPECT_EQ(0, MyPtdata->TotalRespRate);

    EXPECT_EQ(0, MyPtdata->totalTime);
    EXPECT_EQ(0, MyPtdata->MinCalTime);
    EXPECT_EQ(0, MyPtdata->breathsofData);
    EXPECT_EQ(0, MyPtdata->numBreaths);
    EXPECT_EQ(1, MyPtdata->NumMandBreaths);
    EXPECT_EQ(0, MyPtdata->MandBreathsIx);
    EXPECT_EQ(0, MyPtdata->SpontBreaths);
    EXPECT_EQ(eNoPhase, MyPtdata->InhType);
    EXPECT_EQ(0, MyPtdata->Ix);
    EXPECT_EQ(0, MyPtdata->hfoExhFlow);
    EXPECT_EQ(0, MyPtdata->ProxymalLeakValue);
//    EXPECT_EQ(0, MyPtdata->MeanAirwayPressAvg);
//    EXPECT_EQ(0, MyPtdata->ExhTidalVol);

}

//test for void Ptdata::BuildInhMsg (void)
TEST_F(PtDataUnitMock, PtDataUnitMock_BuildInhMsg)
{
    //source code is not available yet
}

//test for void Ptdata::BuildExhMsg (void)
TEST_F(PtDataUnitMock, PtDataUnitMock_BuildExhMsg)
{
    //source code is not available yet
}
//test for void Ptdata::BuildO2Msg (void)
TEST_F(PtDataUnitMock, PtDataUnitMock_BuildO2Msg)
{
    //source code is not available yet
}

//test for void Ptdata::BlankingData (void)
TEST_F(PtDataUnitMock, PtDataUnitMock_BlankingData)
{
    MyPtdata->BlankingData();

    EXPECT_EQ(BLANKED_LONG_VALUE, MyPtdata->O2Concentration);

    EXPECT_EQ(BLANKED_LONG_VALUE, MyPtdata->PeakInhPress);
    EXPECT_EQ(BLANKED_LONG_VALUE, MyPtdata->EndInhPress);

    EXPECT_EQ(BLANKED_LONG_VALUE, MyPtdata->DisplayedExhTidalVol);
    EXPECT_EQ(BLANKED_LONG_VALUE, MyPtdata->ExhTidalVol);
    EXPECT_EQ(BLANKED_LONG_VALUE, MyPtdata->ExhMinuteVol);
    EXPECT_EQ(BLANKED_LONG_VALUE, MyPtdata->SpontMinuteVol);
    EXPECT_EQ(BLANKED_LONG_VALUE, MyPtdata->TotalRespRate);
    EXPECT_EQ(BLANKED_LONG_VALUE, MyPtdata->SpontRespRate);
    EXPECT_EQ(BLANKED_LONG_VALUE, MyPtdata->IeRatio);
    EXPECT_EQ(BLANKED_LONG_VALUE, MyPtdata->MeanAirwayPressAvg);
    EXPECT_EQ(BLANKED_LONG_VALUE, MyPtdata->RapidShallowBreathix);
    EXPECT_EQ(BLANKED_LONG_VALUE, MyPtdata->EndExhPress);

}

//test for Ptdata::Ptdata (void)
TEST_F(PtDataUnitMock, PtDataUnitMock_Ptdata)
{
    //source code is not available yet
}

//test for void Ptdata::CompInhMinuteVolume (void)
TEST_F(PtDataUnitMock, PtDataUnitMock_CompInhMinuteVolume_1)
{
    MyPtdata->Ix = 0;
    MyPtdata->breathsofData = 1;

    MyPtdata->PdHist[0].InhTidalVol = 1;
    MyPtdata->MinCalTime = 1;

    MyPtdata->CompInhMinuteVolume();
    EXPECT_EQ(MyPtdata->InhMinuteVol, (LONG) ((1 * MSECPERMIN / 1000) / (1)));
}
TEST_F(PtDataUnitMock, PtDataUnitMock_CompInhMinuteVolume_0)
{
    MyPtdata->MinCalTime = 0;

    MyPtdata->CompInhMinuteVolume();
    EXPECT_EQ(MyPtdata->InhMinuteVol, BLANKED_LONG_VALUE);
}

//test for void Ptdata::CompProxymalLeak (void)
TEST_F(PtDataUnitMock, PtDataUnitMock_CompProxymalLeak_0)
{
    EXPECT_CALL(*_PtDatamMock, ModeMgrGetCurrentModeId()).Times(1);
    EXPECT_CALL(*_PtDatamMock, ModeMgrGetNextModeId()).Times(1);

    MyPtdata->CompProxymalLeak();
}

TEST_F(PtDataUnitMock, PtDataUnitMock_CompProxymalLeak_1)
{
    EXPECT_CALL(*_PtDatamMock, ModeMgrGetCurrentModeId()).Times(1).WillOnce(Return(eNivMode));
    EXPECT_CALL(*_PtDatamMock, ModeMgrGetNextModeId()).Times(1).WillOnce(Return(eNivMode));

    EXPECT_CALL(*_PtDatamMock, AlarmTaskSetIsActive(eProximalLeakage, eNotActive)).Times(1);

    MyPtdata->CompProxymalLeak();

    EXPECT_EQ(MyPtdata->ProxymalLeakValue, BLANKED_LONG_VALUE);
}
TEST_F(PtDataUnitMock, PtDataUnitMock_CompProxymalLeak_2)
{
    EXPECT_CALL(*_PtDatamMock, ModeMgrGetCurrentModeId()).Times(1).WillOnce(Return(eVcvSpontMode));
    EXPECT_CALL(*_PtDatamMock, ModeMgrGetNextModeId()).Times(1).WillOnce(Return(eVcvSpontMode));

    EXPECT_CALL(*_PtDatamMock, DeviceInterfaceRequestDevice(eProxySensorLastStatus)).Times(1).WillOnce(Return(eNotCalibrated));

    EXPECT_CALL(*_PtDatamMock, AlarmTaskSetIsActive(eProximalLeakage, eNotActive)).Times(1);


    MyPtdata->CompProxymalLeak();

    EXPECT_EQ(MyPtdata->ProxymalLeakValue, BLANKED_LONG_VALUE);
}


TEST_F(PtDataUnitMock, PtDataUnitMock_CompProxymalLeak_3)
{
    EXPECT_CALL(*_PtDatamMock, ModeMgrGetCurrentModeId()).Times(1).WillOnce(Return(eVcvSpontMode));
    EXPECT_CALL(*_PtDatamMock, ModeMgrGetNextModeId()).Times(1).WillOnce(Return(eVcvSpontMode));

    EXPECT_CALL(*_PtDatamMock, DeviceInterfaceRequestDevice(eProxySensorLastStatus)).Times(1).WillOnce(Return(eCalibrated));


    EXPECT_CALL(*_PtDatamMock, MgrBreathDataGetPdBreathDataPtrGetInhVolume()).Times(1).WillOnce(Return(1));
    EXPECT_CALL(*_PtDatamMock, MgrBreathDataGetPdBreathDataPtrGetExhVolume()).Times(1).WillOnce(Return(2));

    MyPtdata->CompProxymalLeak();


    EXPECT_EQ(MyPtdata->ProxymalLeakValue, 0);
}

TEST_F(PtDataUnitMock, PtDataUnitMock_CompProxymalLeak_4)
{
    EXPECT_CALL(*_PtDatamMock, ModeMgrGetCurrentModeId()).Times(1).WillOnce(Return(eVcvSpontMode));
    EXPECT_CALL(*_PtDatamMock, ModeMgrGetNextModeId()).Times(1).WillOnce(Return(eVcvSpontMode));

    EXPECT_CALL(*_PtDatamMock, DeviceInterfaceRequestDevice(eProxySensorLastStatus)).Times(1).WillOnce(Return(eCalibrated));


    EXPECT_CALL(*_PtDatamMock, MgrBreathDataGetPdBreathDataPtrGetInhVolume()).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*_PtDatamMock, MgrBreathDataGetPdBreathDataPtrGetExhVolume()).Times(1).WillOnce(Return(2));

    MyPtdata->CompProxymalLeak();


    EXPECT_EQ(MyPtdata->ProxymalLeakValue, 0);
}

TEST_F(PtDataUnitMock, PtDataUnitMock_CompProxymalLeak_5)
{
    EXPECT_CALL(*_PtDatamMock, ModeMgrGetCurrentModeId()).Times(1).WillOnce(Return(eVcvSpontMode));
    EXPECT_CALL(*_PtDatamMock, ModeMgrGetNextModeId()).Times(1).WillOnce(Return(eVcvSpontMode));

    EXPECT_CALL(*_PtDatamMock, DeviceInterfaceRequestDevice(eProxySensorLastStatus)).Times(1).WillOnce(Return(eCalibrated));


    EXPECT_CALL(*_PtDatamMock, MgrBreathDataGetPdBreathDataPtrGetInhVolume()).Times(1).WillOnce(Return(-10000));
    EXPECT_CALL(*_PtDatamMock, MgrBreathDataGetPdBreathDataPtrGetExhVolume()).Times(1).WillOnce(Return(2));

    MyPtdata->CompProxymalLeak();


    EXPECT_EQ(MyPtdata->ProxymalLeakValue, 100);
}

TEST_F(PtDataUnitMock, PtDataUnitMock_CompProxymalLeak_6)
{
    EXPECT_CALL(*_PtDatamMock, ModeMgrGetCurrentModeId()).Times(1).WillOnce(Return(eVcvSpontMode));
    EXPECT_CALL(*_PtDatamMock, ModeMgrGetNextModeId()).Times(1).WillOnce(Return(eVcvSpontMode));

    EXPECT_CALL(*_PtDatamMock, DeviceInterfaceRequestDevice(eProxySensorLastStatus)).Times(1).WillOnce(Return(eCalibrated));

    EXPECT_CALL(*_PtDatamMock, MgrBreathDataGetPdBreathDataPtrGetInhVolume()).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*_PtDatamMock, MgrBreathDataGetPdBreathDataPtrGetExhVolume()).Times(1).WillOnce(Return(2));

    EXPECT_CALL(*_PtDatamMock, MgrBdSettingGetCurrentValue(eProximalLeakLimit)).Times(1).WillOnce(Return(1));


    EXPECT_CALL(*_PtDatamMock, PdAlarmsCheckHiCondition(1/SCALE, 0, eProximalLeakage)).Times(1);

    MyPtdata->CompProxymalLeak();
}

TEST_F(PtDataUnitMock, PtDataUnitMock_CompProxymalLeak_7)
{
    EXPECT_CALL(*_PtDatamMock, ModeMgrGetCurrentModeId()).Times(1).WillOnce(Return(eVcvSpontMode));
    EXPECT_CALL(*_PtDatamMock, ModeMgrGetNextModeId()).Times(1).WillOnce(Return(eVcvSpontMode));

    EXPECT_CALL(*_PtDatamMock, DeviceInterfaceRequestDevice(eProxySensorLastStatus)).Times(1).WillOnce(Return(eCalibrated));

    EXPECT_CALL(*_PtDatamMock, MgrBreathDataGetPdBreathDataPtrGetInhVolume()).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*_PtDatamMock, MgrBreathDataGetPdBreathDataPtrGetExhVolume()).Times(1).WillOnce(Return(2));

    EXPECT_CALL(*_PtDatamMock, MgrBdSettingGetCurrentValue(eProximalLeakLimit)).Times(1).WillOnce(Return(10000000));


    EXPECT_CALL(*_PtDatamMock, AlarmTaskSetIsActive(eProximalLeakage, eNotActive)).Times(1);

    MyPtdata->CompProxymalLeak();
}

TEST_F(PtDataUnitMock, PtDataUnitMock_CompProxymalLeak_8)
{
    EXPECT_CALL(*_PtDatamMock, ModeMgrGetCurrentModeId()).Times(1).WillOnce(Return(eVcvSpontMode));
    EXPECT_CALL(*_PtDatamMock, ModeMgrGetNextModeId()).Times(1).WillOnce(Return(eVcvSpontMode));

    EXPECT_CALL(*_PtDatamMock, DeviceInterfaceRequestDevice(eProxySensorLastStatus)).Times(1).WillOnce(Return(eResetAndRecovered));


    EXPECT_CALL(*_PtDatamMock, MgrBreathDataGetPdBreathDataPtrGetInhVolume()).Times(1).WillOnce(Return(1));
    EXPECT_CALL(*_PtDatamMock, MgrBreathDataGetPdBreathDataPtrGetExhVolume()).Times(1).WillOnce(Return(2));

    MyPtdata->CompProxymalLeak();


    EXPECT_EQ(MyPtdata->ProxymalLeakValue, 0);
}

TEST_F(PtDataUnitMock, PtDataUnitMock_CompProxymalLeak_9)
{
    EXPECT_CALL(*_PtDatamMock, ModeMgrGetCurrentModeId()).Times(1).WillOnce(Return(eVcvSpontMode));
    EXPECT_CALL(*_PtDatamMock, ModeMgrGetNextModeId()).Times(1).WillOnce(Return(eVcvSpontMode));

    EXPECT_CALL(*_PtDatamMock, DeviceInterfaceRequestDevice(eProxySensorLastStatus)).Times(1).WillOnce(Return(eResetAndRecovered));


    EXPECT_CALL(*_PtDatamMock, MgrBreathDataGetPdBreathDataPtrGetInhVolume()).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*_PtDatamMock, MgrBreathDataGetPdBreathDataPtrGetExhVolume()).Times(1).WillOnce(Return(2));

    MyPtdata->CompProxymalLeak();


    EXPECT_EQ(MyPtdata->ProxymalLeakValue, 0);
}

TEST_F(PtDataUnitMock, PtDataUnitMock_CompProxymalLeak_10)
{
    EXPECT_CALL(*_PtDatamMock, ModeMgrGetCurrentModeId()).Times(1).WillOnce(Return(eVcvSpontMode));
    EXPECT_CALL(*_PtDatamMock, ModeMgrGetNextModeId()).Times(1).WillOnce(Return(eVcvSpontMode));

    EXPECT_CALL(*_PtDatamMock, DeviceInterfaceRequestDevice(eProxySensorLastStatus)).Times(1).WillOnce(Return(eResetAndRecovered));


    EXPECT_CALL(*_PtDatamMock, MgrBreathDataGetPdBreathDataPtrGetInhVolume()).Times(1).WillOnce(Return(-10000));
    EXPECT_CALL(*_PtDatamMock, MgrBreathDataGetPdBreathDataPtrGetExhVolume()).Times(1).WillOnce(Return(2));

    MyPtdata->CompProxymalLeak();


    EXPECT_EQ(MyPtdata->ProxymalLeakValue, 100);
}

TEST_F(PtDataUnitMock, PtDataUnitMock_CompProxymalLeak_11)
{
    EXPECT_CALL(*_PtDatamMock, ModeMgrGetCurrentModeId()).Times(1).WillOnce(Return(eVcvSpontMode));
    EXPECT_CALL(*_PtDatamMock, ModeMgrGetNextModeId()).Times(1).WillOnce(Return(eVcvSpontMode));

    EXPECT_CALL(*_PtDatamMock, DeviceInterfaceRequestDevice(eProxySensorLastStatus)).Times(1).WillOnce(Return(eResetAndRecovered));

    EXPECT_CALL(*_PtDatamMock, MgrBreathDataGetPdBreathDataPtrGetInhVolume()).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*_PtDatamMock, MgrBreathDataGetPdBreathDataPtrGetExhVolume()).Times(1).WillOnce(Return(2));

    EXPECT_CALL(*_PtDatamMock, MgrBdSettingGetCurrentValue(eProximalLeakLimit)).Times(1).WillOnce(Return(1));


    EXPECT_CALL(*_PtDatamMock, PdAlarmsCheckHiCondition(1/SCALE, 0, eProximalLeakage)).Times(1);

    MyPtdata->CompProxymalLeak();
}

TEST_F(PtDataUnitMock, PtDataUnitMock_CompProxymalLeak_12)
{
    EXPECT_CALL(*_PtDatamMock, ModeMgrGetCurrentModeId()).Times(1).WillOnce(Return(eVcvSpontMode));
    EXPECT_CALL(*_PtDatamMock, ModeMgrGetNextModeId()).Times(1).WillOnce(Return(eVcvSpontMode));

    EXPECT_CALL(*_PtDatamMock, DeviceInterfaceRequestDevice(eProxySensorLastStatus)).Times(1).WillOnce(Return(eResetAndRecovered));

    EXPECT_CALL(*_PtDatamMock, MgrBreathDataGetPdBreathDataPtrGetInhVolume()).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*_PtDatamMock, MgrBreathDataGetPdBreathDataPtrGetExhVolume()).Times(1).WillOnce(Return(2));

    EXPECT_CALL(*_PtDatamMock, MgrBdSettingGetCurrentValue(eProximalLeakLimit)).Times(1).WillOnce(Return(10000000));


    EXPECT_CALL(*_PtDatamMock, AlarmTaskSetIsActive(eProximalLeakage, eNotActive)).Times(1);

    MyPtdata->CompProxymalLeak();
}

//test for void Ptdata::CompCompliance (void)
TEST_F(PtDataUnitMock, PtDataUnitMock_CompCompliance_1)
{
    MyPtdata->EndInhPress = 3;
    MyPtdata->EndExhPress = 1;
    MyPtdata->ExhTidalVol = 2;

    MyPtdata->CompCompliance();

    EXPECT_EQ(MyPtdata->Compliance, ((2/(2))*((double)SCALE)));

}
TEST_F(PtDataUnitMock, PtDataUnitMock_CompCompliance_2)
{
    MyPtdata->EndInhPress = 3;
    MyPtdata->EndExhPress = 3;
    MyPtdata->ExhTidalVol = 2;

    MyPtdata->CompCompliance();

    EXPECT_EQ(MyPtdata->Compliance, 0);
}
TEST_F(PtDataUnitMock, PtDataUnitMock_CompCompliance_3)
{
    EXPECT_CALL(*_PtDatamMock, PhaseMgrGetCurrentPhaseId()).Times(1);

    MyPtdata->CompCompliance();
}
TEST_F(PtDataUnitMock, PtDataUnitMock_CompCompliance_4)
{
    EXPECT_CALL(*_PtDatamMock, PhaseMgrGetCurrentPhaseId()).Times(1).WillOnce(Return(eMandInhPhase));

    MyPtdata->sample12_fill_flg_comp = true;

    MyPtdata->EndInhPress = 3;
    MyPtdata->EndExhPress = 1;
    MyPtdata->ExhTidalVol = 2;

    MyPtdata->ComplianceIx12 = 0;

    MyPtdata->CompCompliance();

    EXPECT_EQ(MyPtdata->ComplianceArray12[0], ((2/(2))*((double)SCALE)));
}
TEST_F(PtDataUnitMock, PtDataUnitMock_CompCompliance_5)
{
    EXPECT_CALL(*_PtDatamMock, PhaseMgrGetCurrentPhaseId()).Times(1).WillOnce(Return(eMandInhPhase));

    MyPtdata->sample12_fill_flg_comp = false;

    MyPtdata->EndInhPress = 3;
    MyPtdata->EndExhPress = 1;
    MyPtdata->ExhTidalVol = 2;

    MyPtdata->ComplianceIx12 = 0;

    MyPtdata->CompCompliance();

    EXPECT_EQ(MyPtdata->ComplianceArray12[0], 0);
}
TEST_F(PtDataUnitMock, PtDataUnitMock_CompCompliance_6)
{
    const LONG MAX_NUMBER_OF_SAMPLE = 12;

    EXPECT_CALL(*_PtDatamMock, PhaseMgrGetCurrentPhaseId()).Times(1).WillOnce(Return(eMandInhPhase));

    MyPtdata->sample12_fill_flg_comp = true;

    MyPtdata->EndInhPress = 3;
    MyPtdata->EndExhPress = 1;
    MyPtdata->ExhTidalVol = 2;

    MyPtdata->ComplianceIx12 = 0;
    MyPtdata->ComplianceNum12 = MAX_NUMBER_OF_SAMPLE - 2;

    MyPtdata->CompCompliance();

    EXPECT_EQ(MyPtdata->ComplianceNum12, MAX_NUMBER_OF_SAMPLE - 1);
}
TEST_F(PtDataUnitMock, PtDataUnitMock_CompCompliance_7)
{
    EXPECT_CALL(*_PtDatamMock, PhaseMgrGetCurrentPhaseId()).Times(1).WillOnce(Return(eMandInhPhase));

    MyPtdata->sample12_fill_flg_comp = true;

    MyPtdata->EndInhPress = 3;
    MyPtdata->EndExhPress = 1;
    MyPtdata->ExhTidalVol = 2;

    MyPtdata->ComplianceIx12 = 0;
    MyPtdata->ComplianceIx12 = TWELVE_SAMPLES;

    MyPtdata->CompCompliance();

    EXPECT_EQ(MyPtdata->sample12_fill_flg_comp, true);
}

//test for void Ptdata::CompAverageCompliance(void)
TEST_F(PtDataUnitMock, PtDataUnitMock_CompAverageCompliance_1)
{
    MyPtdata->mNumOfComplianceSample = NUM_OF_SAMPLE_COMPLIANCE + 1;

    for(int i = 0; i < 5; ++i)
    {
        MyPtdata->ComplianceSample[i] = i;
    }
    MyPtdata->CompAverageCompliance();

    EXPECT_EQ(MyPtdata->ComplianceAverage, 1);
}
TEST_F(PtDataUnitMock, PtDataUnitMock_CompAverageCompliance_2)
{
    const SHORT MIN_NUMBER_OF_COMPLIANCE_SAMPLE = 1;

    MyPtdata->mNumOfComplianceSample = MIN_NUMBER_OF_COMPLIANCE_SAMPLE;

    for(int i = 0; i < 5; ++i)
    {
        MyPtdata->ComplianceSample[i] = i;
    }

    MyPtdata->CompAverageCompliance();

    EXPECT_EQ(MyPtdata->ComplianceAverage, 0);
}

TEST_F(PtDataUnitMock, PtDataUnitMock_CompAverageCompliance_3)
{

    MyPtdata->mNumOfComplianceSample = NUM_OF_SAMPLE_COMPLIANCE - 1;

    for(int i = 0; i < 5; ++i)
    {
        MyPtdata->ComplianceSample[i] = i;
    }

    MyPtdata->CompAverageCompliance();

    EXPECT_EQ(MyPtdata->ComplianceAverage, (double)10/6);
}

//test for void Ptdata::CompLeakCompensate(void)
TEST_F(PtDataUnitMock, PtDataUnitMock_CompLeakCompensate_1)
{
    EXPECT_CALL(*_PtDatamMock, MgrBreathDataGetLeakVolTime()).Times(1).WillOnce(Return(1000));
    EXPECT_CALL(*_PtDatamMock, MgrBreathDataGetInhVolOneBRTime()).Times(1).WillOnce(Return(2000));
    EXPECT_CALL(*_PtDatamMock, MgrBreathDataGetExhVolOneBRTime()).Times(1).WillOnce(Return(-1000));

    EXPECT_CALL(*_PtDatamMock, MgrBreathDataResetVol()).Times(1);


    MyPtdata->ExhLeakIx = 0;

    MyPtdata->CompLeakCompensate();

    EXPECT_EQ(MyPtdata->ExhLeakArray[0], 0);

}

//test for void Ptdata::DoEndExhPSInAPRV(void)
TEST_F(PtDataUnitMock, PtDataUnitMock_DoEndExhPSInAPRV_0)
{
    EXPECT_CALL(*_PtDatamMock, MgrBreathDataGetPdBreathDataPtrGetVte()).Times(1).WillOnce(Return(1));

    MyPtdata->ExhTidalVolIx = 0;
    MyPtdata->ExhTidalVolArray[0] = 0;

    MyPtdata->DoEndExhPSInAPRV();

    EXPECT_EQ(MyPtdata->ExhTidalVolArray[0], 1);

}
TEST_F(PtDataUnitMock, PtDataUnitMock_DoEndExhPSInAPRV_1)
{
    const LONG EXH_TIDAL_VOL_LIMIT = 8;

    MyPtdata->ExhTidalVolNum = EXH_TIDAL_VOL_LIMIT - 1;

    MyPtdata->DoEndExhPSInAPRV();

    EXPECT_EQ(MyPtdata->ExhTidalVolNum, EXH_TIDAL_VOL_LIMIT);

}

TEST_F(PtDataUnitMock, PtDataUnitMock_DoEndExhPSInAPRV_2)
{
    MyPtdata->Ix = 0;
    EXPECT_CALL(*_PtDatamMock, MgrBreathDataGetPdBreathDataPtrGetVte()).Times(1).WillOnce(Return(2));

    MyPtdata->DoEndExhPSInAPRV();

    EXPECT_EQ(MyPtdata->PdHist[0].ExhTidalVol, 2);

}
TEST_F(PtDataUnitMock, PtDataUnitMock_DoEndExhPSInAPRV_3)
{
    EXPECT_CALL(*_PtDatamMock, MgrBreathDataGetPdBreathDataPtrGetVte()).Times(1).WillOnce(Return(3));

    EXPECT_CALL(*_PtDatamMock, RequestDevice()).Times(1).WillOnce(Return(eCalibrated));

    EXPECT_CALL(*_PtDatamMock,  MgrBdSettingGetCurrentValue(eHighTidalVolLimit)).Times(1).WillOnce(Return(1));
    EXPECT_CALL(*_PtDatamMock,  MgrBdSettingGetCurrentValue(eLowTidalVolLimit)).Times(1).WillOnce(Return(2));

    EXPECT_CALL(*_PtDatamMock,  PdAlarmsCheckHiCondition(1, 3, eHighVte)).Times(1);
    EXPECT_CALL(*_PtDatamMock,  PdAlarmsCheckLowCondition(2, 3, eLowVte)).Times(1);


    MyPtdata->DoEndExhPSInAPRV();

}
TEST_F(PtDataUnitMock, PtDataUnitMock_DoEndExhPSInAPRV_4)
{
    EXPECT_CALL(*_PtDatamMock, MgrBreathDataGetPdBreathDataPtrGetVte()).Times(1).WillOnce(Return(3));

    EXPECT_CALL(*_PtDatamMock, RequestDevice()).Times(1).WillOnce(Return(eResetAndRecovered));

    EXPECT_CALL(*_PtDatamMock,  MgrBdSettingGetCurrentValue(eHighTidalVolLimit)).Times(1).WillOnce(Return(1));
    EXPECT_CALL(*_PtDatamMock,  MgrBdSettingGetCurrentValue(eLowTidalVolLimit)).Times(1).WillOnce(Return(2));

    EXPECT_CALL(*_PtDatamMock,  PdAlarmsCheckHiCondition(1, 3, eHighVte)).Times(1);
    EXPECT_CALL(*_PtDatamMock,  PdAlarmsCheckLowCondition(2, 3, eLowVte)).Times(1);


    MyPtdata->DoEndExhPSInAPRV();

}

//test for void Ptdata::DoEndInhPSInAPRV(void)
TEST_F(PtDataUnitMock, PtDataUnitMock_DoEndInhPSInAPRV_0)
{
    MyPtdata->InhType = eSupported;

    EXPECT_CALL(*_PtDatamMock,  MgrBdSettingGetCurrentValue(eAPRVHighInhPress)).Times(1).WillOnce(Return(200));
    EXPECT_CALL(*_PtDatamMock,  MgrBdSettingGetCurrentValue(eSupportPress)).Times(1).WillOnce(Return(100));
    EXPECT_CALL(*_PtDatamMock,  MgrBdSettingGetCurrentValue(eHipLimit)).Times(1).WillOnce(Return(1));

    MyPtdata->DoEndInhPSInAPRV();

    EXPECT_EQ(MyPtdata->LipLimit, 100);
    EXPECT_EQ(MyPtdata->HipLimit, 1);
}
//test for void Ptdata::DoPEEP(void)
TEST_F(PtDataUnitMock, PtDataUnitMock_DoPEEP_0)
{
    EXPECT_CALL(*_PtDatamMock,  MgrBreathDataGetPdBreathDataPtrGetEndExhPress()).Times(1).WillOnce(Return(3));

    EXPECT_CALL(*_PtDatamMock,  MgrBdSettingGetCurrentValue(eHighBaselineLimit)).Times(1).WillOnce(Return(1));
    EXPECT_CALL(*_PtDatamMock,  MgrBdSettingGetCurrentValue(eLowBaselineLimit)).Times(1).WillOnce(Return(2));


    EXPECT_CALL(*_PtDatamMock,  PdAlarmsCheckHiCondition(1,3,eHighPeep)).Times(1);
    EXPECT_CALL(*_PtDatamMock,  PdAlarmsCheckLowCondition(2,3,eLowPeep)).Times(1);

    MyPtdata->DoPEEP();

}
TEST_F(PtDataUnitMock, PtDataUnitMock_DoPEEP_1)
{
    EXPECT_CALL(*_PtDatamMock,  MgrBreathDataGetPdBreathDataPtrGetEndExhPress()).Times(1).WillOnce(Return(0));

    EXPECT_CALL(*_PtDatamMock,  MgrBdSettingGetCurrentValue(eHighBaselineLimit)).Times(1).WillOnce(Return(1));
    EXPECT_CALL(*_PtDatamMock,  MgrBdSettingGetCurrentValue(eLowBaselineLimit)).Times(1).WillOnce(Return(2));


    EXPECT_CALL(*_PtDatamMock,  PdAlarmsCheckHiCondition(1,0,eHighPeep)).Times(1);
    EXPECT_CALL(*_PtDatamMock,  PdAlarmsCheckLowCondition(2,0,eLowPeep)).Times(1);

    MyPtdata->DoPEEP();

}

//test for void Ptdata::DoNCPAP(void)
TEST_F(PtDataUnitMock, PtDataUnitMock_DoNCPAP_0)
{
    EXPECT_CALL(*_PtDatamMock,  DeviceInterfaceRequestDevice(ePressureSensorGetLastReading)).Times(1);

    EXPECT_CALL(*_PtDatamMock,  MgrBdSettingGetCurrentValue(eHighNCPAPLimit)).Times(1).WillOnce(Return(1));
    EXPECT_CALL(*_PtDatamMock,  MgrBdSettingGetCurrentValue(eLowNCPAPLimit)).Times(1).WillOnce(Return(2));


    EXPECT_CALL(*_PtDatamMock,  PdAlarmsCheckHiCondition(0,eHighNCPAP,eLowNCPAP)).Times(1);
    EXPECT_CALL(*_PtDatamMock,  PdAlarmsCheckLowCondition(0,eHighNCPAP,eLowNCPAP)).Times(1);

    MyPtdata->DoNCPAP();
}

//test for void Ptdata::DoAPRV_PHigh(void)
TEST_F(PtDataUnitMock, PtDataUnitMock_DoAPRV_PHigh_0)
{
    EXPECT_CALL(*_PtDatamMock,  AlarmTaskGetIsActive(eAPRVHigh_PHigh)).Times(1).WillOnce(Return(eActive));
    EXPECT_CALL(*_PtDatamMock,  AlarmTaskSetIsActive(eAPRVHigh_PHigh, eNotActive)).Times(1);


    MyPtdata->DoAPRV_PHigh();

}
TEST_F(PtDataUnitMock, PtDataUnitMock_DoAPRV_PHigh_1)
{
    EXPECT_CALL(*_PtDatamMock,  MgrBreathDataGetPdBreathDataPtrGetEndInhPress()).Times(1).WillOnce(Return(2));

    EXPECT_CALL(*_PtDatamMock,  MgrBreathDataGetBdBreathDataPtrGetAPRV_PHigh()).Times(1).WillOnce(Return(3));

    EXPECT_CALL(*_PtDatamMock,  MgrBdSettingGetCurrentValue(eAPRVHighPHighLimit)).Times(1).WillOnce(Return(1));
    EXPECT_CALL(*_PtDatamMock,  MgrBdSettingGetCurrentValue(eAPRVLowPHighLimit)).Times(1).WillOnce(Return(2));

    EXPECT_CALL(*_PtDatamMock,  PdAlarmsCheckHighPHigh5Time(1, 3, eAPRVHigh_PHigh)).Times(1);
    EXPECT_CALL(*_PtDatamMock,  PdAlarmsCheckLowPHigh5Time(2, 3, eAPRVLow_PHigh)).Times(1);

    EXPECT_CALL(*_PtDatamMock,  AlarmTaskGetIsActive(eAPRVHigh_PHigh)).Times(1).WillOnce(Return(eNotActive));

    MyPtdata->InhType = eSupported;

    EXPECT_CALL(*_PtDatamMock,  MgrBdSettingGetCurrentValue(eAPRVHighInhPress)).Times(1).WillOnce(Return(200));
    EXPECT_CALL(*_PtDatamMock,  MgrBdSettingGetCurrentValue(eSupportPress)).Times(1).WillOnce(Return(100));


    MyPtdata->DoAPRV_PHigh();

}

TEST_F(PtDataUnitMock, PtDataUnitMock_DoAPRV_PHigh_2)
{
    EXPECT_CALL(*_PtDatamMock,  MgrBreathDataGetPdBreathDataPtrGetEndInhPress()).Times(1).WillOnce(Return(2));

    EXPECT_CALL(*_PtDatamMock,  MgrBreathDataGetBdBreathDataPtrGetAPRV_PHigh()).Times(1).WillOnce(Return(3));

    EXPECT_CALL(*_PtDatamMock,  MgrBdSettingGetCurrentValue(eAPRVHighPHighLimit)).Times(1).WillOnce(Return(1));
    EXPECT_CALL(*_PtDatamMock,  MgrBdSettingGetCurrentValue(eAPRVLowPHighLimit)).Times(1).WillOnce(Return(2));

    EXPECT_CALL(*_PtDatamMock,  PdAlarmsCheckHighPHigh5Time(1, 3, eAPRVHigh_PHigh)).Times(1);
    EXPECT_CALL(*_PtDatamMock,  PdAlarmsCheckLowPHigh5Time(2, 3, eAPRVLow_PHigh)).Times(1);

    EXPECT_CALL(*_PtDatamMock,  AlarmTaskGetIsActive(eAPRVHigh_PHigh)).Times(1).WillOnce(Return(eNotActive));

    MyPtdata->InhType = eSpontaneous;

    EXPECT_CALL(*_PtDatamMock,  MgrBdSettingGetCurrentValue(eAPRVLowInhPress)).Times(1).WillOnce(Return(100));


    MyPtdata->DoAPRV_PHigh();

}

//test for void Ptdata::DoAPRV_PLow(void)
TEST_F(PtDataUnitMock, PtDataUnitMock_DoAPRV_PLow_0)
{
    EXPECT_CALL(*_PtDatamMock,  PdAlarmsResetPLowAlarmActiveTime()).Times(1);

    EXPECT_CALL(*_PtDatamMock,  MgrBreathDataGetPdBreathDataPtrGetAPRV_PLow()).Times(1).WillOnce(Return(3));

    EXPECT_CALL(*_PtDatamMock,  MgrBdSettingGetCurrentValue(eAPRVHighPLowLimit)).Times(1).WillOnce(Return(1));
    EXPECT_CALL(*_PtDatamMock,  MgrBdSettingGetCurrentValue(eAPRVLowPLowLimit)).Times(1).WillOnce(Return(2));

    EXPECT_CALL(*_PtDatamMock,  PdAlarmsCheckHighPLow5Time(1, 3, eAPRVHigh_PLow)).Times(1);
    EXPECT_CALL(*_PtDatamMock,  PdAlarmsCheckLowPLow5Time(2, 3, eAPRVLow_PLow)).Times(1);

    MyPtdata->DoAPRV_PLow();
}

//test for void Ptdata::DoAPRVBR(void)
TEST_F(PtDataUnitMock, PtDataUnitMock_DoAPRVBR_0)
{
    MyPtdata->IsFirst_1minute = true;
    MyPtdata->DoAPRVBR();

    EXPECT_EQ(MyPtdata->IsFirst_1minute, false);
}

//test for void  Ptdata::DoHFOData(void)
TEST_F(PtDataUnitMock, PtDataUnitMock_DoHFOData_0)
{
    MyPtdata->DoHFOData();
}

//test for void Ptdata::GetCircuitComplianceFactor(void)
TEST_F(PtDataUnitMock, PtDataUnitMock_GetCircuitComplianceFactor_0)
{
    MyPtdata->GetCircuitComplianceFactor();
}

//test for void Ptdata::ModeInducedAlarmClearing(void)
TEST_F(PtDataUnitMock, PtDataUnitMock_ModeInducedAlarmClearing_0)
{
    MyPtdata->Mode = eSpontaneousMode;

    EXPECT_CALL(*_PtDatamMock,  TaskSetIsActive(eLowInspPressure, eNotActive)).Times(1);

    MyPtdata->ModeInducedAlarmClearing();
}
TEST_F(PtDataUnitMock, PtDataUnitMock_ModeInducedAlarmClearing_1)
{
    MyPtdata->Mode = eSpontaneousMode;

    EXPECT_CALL(*_PtDatamMock,  TaskSetIsActive(eLowInspPressure, eNotActive)).Times(1).WillOnce(Return(eError));
    EXPECT_CALL(*_PtDatamMock,  AlarmTaskSetIsActive(eLowInspPressure, eNotActive)).Times(1);

    MyPtdata->ModeInducedAlarmClearing();
}

//test for LONG Ptdata::GetAverageVte(void)
TEST_F(PtDataUnitMock, PtDataUnitMock_GetAverageVte_0)
{
    MyPtdata->ExhTidalVolNum =  1;

    EXPECT_EQ(MyPtdata->GetAverageVte(), 0);

}
TEST_F(PtDataUnitMock, PtDataUnitMock_GetAverageVte_1)
{
    const LONG EXH_TIDAL_VOL_LIMIT = 6;
    MyPtdata->ExhTidalVolNum =  EXH_TIDAL_VOL_LIMIT - 2;

    EXPECT_EQ(MyPtdata->GetAverageVte(), 0);

}
TEST_F(PtDataUnitMock, PtDataUnitMock_GetAverageVte_2)
{
    const LONG EXH_TIDAL_VOL_LIMIT = 6;
    MyPtdata->ExhTidalVolNum =  EXH_TIDAL_VOL_LIMIT + 1;

    EXPECT_EQ(MyPtdata->GetAverageVte(), 0);

}
TEST_F(PtDataUnitMock, PtDataUnitMock_GetAverageVte_3)
{
    const LONG EXH_TIDAL_VOL_LIMIT = 6;
    MyPtdata->ExhTidalVolNum =  EXH_TIDAL_VOL_LIMIT + 4;

    EXPECT_EQ(MyPtdata->GetAverageVte(), 1);

}

//test for LONG Ptdata::GetAverageVte12(void)
struct GetAverage
{
    int in;
    int out;
};
struct GetAverage GetAverageTestCase[] =
{
        {1,0},
        {2,1},
        {3,1},
        {4,2},
        {5,2},
        {6,3},
        {7,3},
        {8,3},
        {9,4},

};

class TestGetAverageVte12 : public ::testing::TestWithParam<GetAverage>
{
public:
    virtual void SetUp(void)
    {
        MyPtdata = new Ptdata();
    }
    virtual void TearDown(void)
    {
        delete MyPtdata;
    }
    Ptdata *MyPtdata;
};
TEST_P(TestGetAverageVte12, PtDataUnitMock_GetAverageVte12)
{
    MyPtdata->ExhTidalVolNum12 = GetParam().in;
    MyPtdata->sample12_fill_flg = true;

    for(SHORT i = 0; i < MyPtdata->ExhTidalVolNum12; ++i)
    {
        MyPtdata->ExhTidalVolArray12[i] = i;
    }

    EXPECT_EQ(MyPtdata->GetAverageVte12(), GetParam().out);
}
INSTANTIATE_TEST_CASE_P(TestGetAverageVte12,
                        TestGetAverageVte12,
                        testing::ValuesIn(GetAverageTestCase)
);

struct GetAverage GetAverageTestCaseCom[] =
{
        {1, 0},
        {2, 1},
        {3, 3},
        {4, 5},
        {5, 9},
        {6, 12},
        {7, 18},
        {9, 11},

};
class TestGetAverageCom : public ::testing::TestWithParam<GetAverage>
{
public:
    virtual void SetUp(void)
    {
        MyPtdata = new Ptdata();
    }
    virtual void TearDown(void)
    {
        delete MyPtdata;
    }
    Ptdata *MyPtdata;
};
TEST_P(TestGetAverageCom, PtDataUnitMock_GetAverageCompliance12)
{
    MyPtdata->ComplianceNum12 = GetParam().in;
    MyPtdata->sample12_fill_flg_comp = true;

    for(SHORT i = 0; i < MyPtdata->ComplianceNum12; ++i)
    {
        MyPtdata->ComplianceArray12[i] = i;
    }

    EXPECT_EQ(MyPtdata->GetAverageCompliance12(), GetParam().out);
}
INSTANTIATE_TEST_CASE_P(TestGetAverageCom,
                        TestGetAverageCom,
                        testing::ValuesIn(GetAverageTestCaseCom)
);

//test for void Ptdata::ClearVteBuffer(void)
TEST_F(PtDataUnitMock, PtDataUnitMock_ClearVteBuffer_0)
{
    MyPtdata->ClearVteBuffer();
    EXPECT_EQ(MyPtdata->ExhTidalVolNum12, 0);
    EXPECT_EQ(MyPtdata->ExhTidalVolIx12, 0);
    EXPECT_EQ(MyPtdata->ComplianceNum12, 0);
    EXPECT_EQ(MyPtdata->ComplianceIx12, 0);
    EXPECT_EQ(MyPtdata->sample12_fill_flg, false);
}

//test for LONG Ptdata::GetEndExhFlow_Average(void)
TEST_F(PtDataUnitMock, PtDataUnitMock_GetEndExhFlow_Average)
{
    for (LONG j=0; j < 8; j++)
    {
        MyPtdata->EndExhFlowArray[j] = j;
    }

    EXPECT_EQ(MyPtdata->GetEndExhFlow_Average(), 3);

}

//test for double Ptdata::GetExhLeak(void)
TEST_F(PtDataUnitMock, PtDataUnitMock_GetExhLeak)
{
    for (LONG j=0; j < 8; j++)
    {
        MyPtdata->ExhLeakArray[j] = j;
    }

    EXPECT_EQ(MyPtdata->GetExhLeak(), 3.5);
}

//test for void Ptdata::ClearExhLeakArray(void)
TEST_F(PtDataUnitMock, PtDataUnitMock_ClearExhLeakArray)
{
    MyPtdata->ClearExhLeakArray();

    for(int i = 0;i < 8; i++)
    {
        EXPECT_EQ(MyPtdata->ExhLeakArray[i], 0);
    }
}

//test for void Ptdata::CompExhTidalVolumeInAPRV(void)
TEST_F(PtDataUnitMock, PtDataUnitMock_CompExhTidalVolumeInAPRV_0)
{
    EXPECT_CALL(*_PtDatamMock, MgrBreathDataGetPdBreathDataPtrGetVte()).Times(1).WillOnce(Return(1));
    MyPtdata->Ix = 0;

    EXPECT_CALL(*_PtDatamMock, RequestDevice()).Times(1).WillOnce(Return(eCalibrated));

    EXPECT_CALL(*_PtDatamMock,  MgrBdSettingGetCurrentValue(eHighTidalVolLimit)).Times(1).WillOnce(Return(1));
    EXPECT_CALL(*_PtDatamMock,  MgrBdSettingGetCurrentValue(eLowTidalVolLimit)).Times(1).WillOnce(Return(2));

    EXPECT_CALL(*_PtDatamMock,  PdAlarmsCheckHiCondition(1, 1, eHighVte)).Times(1);
    EXPECT_CALL(*_PtDatamMock,  PdAlarmsCheckLowCondition(2, 1, eLowVte)).Times(1);


    MyPtdata->CompExhTidalVolumeInAPRV();

    EXPECT_EQ(MyPtdata->ExhTidalVol, 1);
    EXPECT_EQ(MyPtdata->PdHist[0].ExhTidalVol, 1);
}
TEST_F(PtDataUnitMock, PtDataUnitMock_CompExhTidalVolumeInAPRV_1)
{
    EXPECT_CALL(*_PtDatamMock, MgrBreathDataGetPdBreathDataPtrGetVte()).Times(1).WillOnce(Return(1));
    MyPtdata->Ix = 0;

    EXPECT_CALL(*_PtDatamMock, RequestDevice()).Times(1).WillOnce(Return(eResetAndRecovered));

    EXPECT_CALL(*_PtDatamMock,  MgrBdSettingGetCurrentValue(eHighTidalVolLimit)).Times(1).WillOnce(Return(1));
    EXPECT_CALL(*_PtDatamMock,  MgrBdSettingGetCurrentValue(eLowTidalVolLimit)).Times(1).WillOnce(Return(2));

    EXPECT_CALL(*_PtDatamMock,  PdAlarmsCheckHiCondition(1, 1, eHighVte)).Times(1);
    EXPECT_CALL(*_PtDatamMock,  PdAlarmsCheckLowCondition(2, 1, eLowVte)).Times(1);


    MyPtdata->CompExhTidalVolumeInAPRV();

    EXPECT_EQ(MyPtdata->ExhTidalVol, 1);
    EXPECT_EQ(MyPtdata->PdHist[0].ExhTidalVol, 1);
}

//test for void Ptdata::CompCompliance_InAPRV(void)
TEST_F(PtDataUnitMock, PtDataUnitMock_CompCompliance_InAPRV_0)
{
    EXPECT_CALL(*_PtDatamMock, MgrBreathDataGetBdBreathDataPtrGetAPRV_PLow()).Times(1).WillOnce(Return(1));
    MyPtdata->APRV_EndInhPress = 2;
    MyPtdata->VteInAPRV = 3;

    MyPtdata->CompCompliance_InAPRV();

    EXPECT_EQ(MyPtdata->Compliance, ((3/((2-1))*((double)SCALE))));

}
TEST_F(PtDataUnitMock, PtDataUnitMock_CompCompliance_InAPRV_1)
{
    EXPECT_CALL(*_PtDatamMock, MgrBreathDataGetBdBreathDataPtrGetAPRV_PLow()).Times(1).WillOnce(Return(1));
    MyPtdata->APRV_EndInhPress = 1;
    MyPtdata->VteInAPRV = 3;

    MyPtdata->CompCompliance_InAPRV();

    EXPECT_EQ(MyPtdata->Compliance, 0);

}

//test for double Ptdata::GetMinCompliance(SHORT index)
TEST_F(PtDataUnitMock, PtDataUnitMock_GetMinCompliance)
{
    for(int i = 0; i < 8; ++i)
    {
        MyPtdata->ComplianceSample[i] = (double)i;
    }

    EXPECT_EQ(MyPtdata->GetMinCompliance(8), 0);
}

//test for double Ptdata::GetMaxCompliance(SHORT index)
TEST_F(PtDataUnitMock, PtDataUnitMock_GetMaxCompliance)
{
    for(int i = 0; i < 8; ++i)
    {
        MyPtdata->ComplianceSample[i] = (double)i;
    }

    EXPECT_EQ(MyPtdata->GetMaxCompliance(8), 7);
}

//test for void Ptdata::GetHFOData(void)
TEST_F(PtDataUnitMock, PtDataUnitMock_GetHFOData)
{
    MyPtdata->GetHFOData();
}

//test for void Ptdata::BuildHFOMsg(void)
TEST_F(PtDataUnitMock, PtDataUnitMock_BuildHFOMsg)
{
    MyPtdata->BuildHFOMsg();
}

//test for void Ptdata::CalculateHFOExh(void)
TEST_F(PtDataUnitMock, PtDataUnitMock_CalculateHFOExh_0)
{
    const int COUNT_MAX = 500;

    for(int i = 0; i < COUNT_MAX + 1; ++i)
    {
        MyPtdata->CalculateHFOExh();
        EXPECT_EQ(MyPtdata->hfoExhFlow, 0);
    }

}
TEST_F(PtDataUnitMock, PtDataUnitMock_CalculateHFOExh_1)
{
    const int COUNT_MAX = 500;

    EXPECT_CALL(*_PtDatamMock, MgrBreathDataGetDeliveredFlow()).Times(500);

    for(int i = 0; i < COUNT_MAX + 1; ++i)
    {
        MyPtdata->CalculateHFOExh();
    }

    EXPECT_EQ(MyPtdata->hfoExhFlow, 0);
}
TEST_F(PtDataUnitMock, PtDataUnitMock_CalculateHFOExh_2)
{
    const int COUNT_MAX = 500;

    EXPECT_CALL(*_PtDatamMock, MgrBreathDataGetDeliveredFlow()).Times(499);

    for(int i = 0; i < COUNT_MAX; ++i)
    {
        MyPtdata->CalculateHFOExh();
    }

    EXPECT_EQ(MyPtdata->hfoExhFlow, 0);
}

//test for void Ptdata::BuildAPRVPLow(void)
TEST_F(PtDataUnitMock, PtDataUnitMock_BuildAPRVPLow)
{
    MyPtdata->BuildAPRVPLow();

}

//test for void Ptdata::GetEndExhFlow(void)
TEST_F(PtDataUnitMock, PtDataUnitMock_GetEndExhFlow)
{
    EXPECT_CALL(*_PtDatamMock, MgrBreathDataGetBdBreathDataPtrGetEndExhFlow()).Times(1).WillOnce(Return(1));
    MyPtdata->EndExhFlowIx = 0;

    MyPtdata->GetEndExhFlow();

    EXPECT_EQ(MyPtdata->EndExhFlowArray[0], 1);
}

//test for LONG Ptdata::GetMinVteArray(void)
TEST_F(PtDataUnitMock, PtDataUnitMock_GetMinVteArray)
{
    MyPtdata->ExhTidalVolNum = 8;
    for(int i = 0; i < 8; ++i)
    {
        MyPtdata->ExhTidalVolArray[i] = i;
    }

    EXPECT_EQ(MyPtdata->GetMinVteArray(), 0);
}

//test for LONG Ptdata::GetMaxVteArray(void)
TEST_F(PtDataUnitMock, PtDataUnitMock_GetMaxVteArray)
{
    MyPtdata->ExhTidalVolNum = 8;
    for(int i = 0; i < 8; ++i)
    {
        MyPtdata->ExhTidalVolArray[i] = i;
    }

    EXPECT_EQ(MyPtdata->GetMaxVteArray(), 7);
}

//test for void Ptdata::GetMinComplianceArray12(double *add)
//test for void Ptdata::GetMaxComplianceArray12(double *add)
//test for void Ptdata::GetMinVteArray12(LONG *add)
//test for void Ptdata::GetMaxVteArray12(LONG *add)
//test for E_Bool Ptdata::PdSpontaneous (E_BreathPhaseIndicator inhtype)
TEST_F(PtDataUnitMock, PtDataUnitMock_PdSpontaneous)
{
    EXPECT_EQ(MyPtdata->PdSpontaneous(eSpontaneous), eTrue);
    EXPECT_EQ(MyPtdata->PdSpontaneous(eSupported), eTrue);
    EXPECT_EQ(MyPtdata->PdSpontaneous(eNCPAP), eFalse);
}

//test for E_Bool Ptdata::PdMandatory (E_BreathPhaseIndicator inhtype)
TEST_F(PtDataUnitMock, PtDataUnitMock_PdMandatory)
{
    EXPECT_EQ(MyPtdata->PdMandatory(eAssisted), eTrue);
    EXPECT_EQ(MyPtdata->PdMandatory(eMandatory), eTrue);
    EXPECT_EQ(MyPtdata->PdMandatory(eNCPAP), eFalse);
}

//test for double Ptdata::GetComplianceAverage(void)
TEST_F(PtDataUnitMock, PtDataUnitMock_GetComplianceAverage)
{
    MyPtdata->ComplianceAverage = 2;

    EXPECT_EQ(MyPtdata->GetComplianceAverage(), 2);
}

//test for LONG Ptdata::GetVte(void)
TEST_F(PtDataUnitMock, PtDataUnitMock_GetVte)
{
    MyPtdata->ExhTidalVol = 2;

    EXPECT_EQ(MyPtdata->GetVte(), 2);
}

//test for float Ptdata::GetProxymalLeak(void)
TEST_F(PtDataUnitMock, PtDataUnitMock_GetProxymalLeak)
{
    MyPtdata->ProxymalLeakValue = 2;

    EXPECT_EQ(MyPtdata->GetProxymalLeak(), 2);
}

//test for LONG Ptdata::GetComplVolumne(void)
TEST_F(PtDataUnitMock, PtDataUnitMock_GetComplVolumne)
{
    MyPtdata->ComplVolume = 2;

    EXPECT_EQ(MyPtdata->GetComplVolumne(), 2);
}

//test for LONG Ptdata::GetComplianceFactor(void)
TEST_F(PtDataUnitMock, PtDataUnitMock_GetComplianceFactor)
{
    MyPtdata->CircuitComplianceFactor = 2;

    EXPECT_EQ(MyPtdata->GetComplianceFactor(), 2);
}

//test for ULONG Ptdata::GetTimeInInh(void)
TEST_F(PtDataUnitMock, PtDataUnitMock_GetTimeInInh)
{
    MyPtdata->TimeInInh = 2;

    EXPECT_EQ(MyPtdata->GetTimeInInh(), 2);
}

//test for LONG Ptdata::GetLastMandExhVt(void)
TEST_F(PtDataUnitMock, PtDataUnitMock_GetLastMandExhVt)
{
    MyPtdata->LastMandExhVt = 2;

    EXPECT_EQ(MyPtdata->GetLastMandExhVt(), 2);
}

//test for E_PhaseId Ptdata::GetPrevPhaseId(void)
TEST_F(PtDataUnitMock, PtDataUnitMock_GetPrevPhaseId)
{
    MyPtdata->prevphaseId_vt = eSelfTestPhase;

    EXPECT_EQ(MyPtdata->GetPrevPhaseId(), eSelfTestPhase);
}

//test for E_PhaseId Ptdata::GetPrevprevPhaseId(void)
TEST_F(PtDataUnitMock, PtDataUnitMock_GetPrevprevPhaseId)
{
    MyPtdata->prevprevphaseId_vt = eSelfTestPhase;

    EXPECT_EQ(MyPtdata->GetPrevprevPhaseId(), eSelfTestPhase);
}

#endif  //end define PD_UNIT_TEST
