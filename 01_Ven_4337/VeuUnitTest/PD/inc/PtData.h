//$COMMON.H$
//    File Name: PtData.h
//    Copyright 1996 InVentive Technologies, Inc.  All Rights Reserved.
//
//    Class: Ptdata - current Patient data of last breath
//
/******************************************************************************/
#ifndef  PTDATA_H
#define  PTDATA_H

// Include standard	project	definitions
#include <PdHist.h>
#include "CommonConstants.h"
#include "CommonTypes.h"
#include "BdInterface.h"
#include "DeviceInterface.h"

// Patient Data breath data history
#include "PdHist.h"
#include "EPhaseId.h"

// MAX_BREATHS must be a power of 2
// MAX_BREATHS must be a number so that MAX_BREATHS - 1
// is all bits set less the MSB

static const LONG MAX_BREATHS = 8;
static const LONG MAX_BREATHS_MASK = (MAX_BREATHS - 1);
const SHORT EIGHT_SAMPLES = 8;
const SHORT NUM_OF_SAMPLE_COMPLIANCE = 8;
const SHORT TWELVE_SAMPLES = 12;//for new VA


//define function to mock
E_PhaseId PhaseMgrGetCurrentPhaseId(void);
E_VentilationMode ModeMgrGetCurrentModeId(void);
E_VentilationMode ModeMgrGetNextModeId(void);

ProxyStatus RequestDevice(void);

LONG MgrBdSettingGetCurrentValue(E_BdSettingId Id);

void PdAlarmsCheckHiCondition(LONG x, LONG y, E_GUIEventID z);
void PdAlarmsCheckLowCondition(LONG x, LONG y, E_GUIEventID z);

LONG MgrBreathDataGetPdBreathDataPtrGetPressureSum(void);

void PdAlarmsCheckHiBaseLine(SHORT x, LONG y, E_GUIEventID z);
void PdAlarmsCheckLowBaseLine(SHORT x, LONG y, E_GUIEventID z);

LONG MgrBreathDataGetPdBreathDataPtrGetCompensatedExhVolume(void);
void MgrBreathDataSetPdDone(void);
void PdAlarmsResetPEEPAlarmActiveTime(void);

LONG MgrBreathDataGetPdBreathDataPtrGetExhVolume(void);
LONG MgrBreathDataGetPdBreathDataPtrGetEndExhPress(void);
LONG MgrBreathDataGetPdBreathDataPtrGetNumExhSamples(void);
LONG MgrBreathDataGetPdBreathDataPtrGetTimeInExh(void);
E_BreathPhaseIndicator MgrBreathDataGetPdBreathDataPtrGetInhType(void);

LONG MgrBreathDataGetPdBreathDataPtrGetPeakInhPress(void);
LONG MgrBreathDataGetPdBreathDataPtrGetEndInhPress(void);
LONG MgrBreathDataGetPdBreathDataPtrGetNumInhSamples(void);
ULONG MgrBreathDataGetPdBreathDataPtrGetTimeInInh(void);

void PdAlarmsCheckHiO2(LONG x, LONG y, E_GUIEventID z);
void PdAlarmsCheckLowO2(LONG x, LONG y, E_GUIEventID z);

void AlarmTaskSetIsActive(E_GUIEventID x, E_AlarmStatus y);

ProxyStatus DeviceInterfaceRequestDevice(RequestId id);

LONG MgrBreathDataGetPdBreathDataPtrGetInhVolume(void);

double MgrBreathDataGetLeakVolTime(void);
double MgrBreathDataGetInhVolOneBRTime(void);
double MgrBreathDataGetExhVolOneBRTime(void);

void MgrBreathDataResetVol(void);
LONG MgrBreathDataGetPdBreathDataPtrGetVte(void);

void PdAlarmsCheckLowPip(LONG x, LONG y, E_GUIEventID z);
void PdAlarmsCheckHiPip(LONG x, LONG y, E_GUIEventID z);

LONG MgrBreathDataGetBdBreathDataPtrGetAPRV_PHigh(void);

void PdAlarmsCheckHighPHigh5Time(SHORT x, LONG y, E_GUIEventID z);
void PdAlarmsCheckLowPHigh5Time(SHORT x, LONG y, E_GUIEventID z);

E_AlarmStatus AlarmTaskGetIsActive(E_GUIEventID z);

void PdAlarmsResetPLowAlarmActiveTime(void);
LONG MgrBreathDataGetPdBreathDataPtrGetAPRV_PLow(void);

void PdAlarmsCheckHighPLow5Time(SHORT x, LONG y, E_GUIEventID z);
void PdAlarmsCheckLowPLow5Time(SHORT x, LONG y, E_GUIEventID z);

void MgrBreathDataResetAPRVBR(void);

E_ReturnStatus TaskSetIsActive(E_GUIEventID x, E_AlarmStatus y);

double MgrBreathDataGetBdBreathDataPtrGetAPRV_PLow(void);

LONG MgrBreathDataGetDeliveredFlow(void);

LONG MgrBreathDataGetBdBreathDataPtrGetEndExhFlow(void);

void CallThis(void);

class Ptdata
{
public:
    static Ptdata* S_GetInstance (void);

    // Compute Inhalation Patient Data
    void DoInhalationData (void);

    // Compute Exhalation Patient Data
    void DoExhalationData (void);

    // Compute O2 data
    void DoO2 (void);

    //Compute end exh/inh PS in APRV
    void DoEndExhPSInAPRV(void);
    void DoEndInhPSInAPRV(void);

    // Compute PEEP
    void DoPEEP(void);

    //Compute NCPAP
    void DoNCPAP(void);

    //Copute APRV_PHigh and APRV_PLow
    void DoAPRV_PHigh(void);
    void DoAPRV_PLow(void);

    //COmpute APRVBR
    void DoAPRVBR(void);

    // Compute HFO data
    void DoHFOData(void);

    //Get the Circuit Compliance Factor
    void GetCircuitComplianceFactor(void);

    // Get compliance Average
    double GetComplianceAverage(void);

    //Get Vte
    LONG GetVte(void);

    //Get average Vte
    LONG GetAverageVte(void);

    //average of 12 samples for VA
    LONG GetAverageVte12(void);
    double GetAverageCompliance12(void);

    //Clear Vte Buffer
    void ClearVteBuffer(void);

    //Get proxymal leak
    float GetProxymalLeak(void);

    //Get compliance volume
    LONG GetComplVolumne(void);
    LONG GetComplianceFactor(void);

    //Get the flow at the end of Exhalation
    LONG GetEndExhFlow_Average(void);

    //Get exhalation leak
    double GetExhLeak(void);

    //Clear exhalation leark array
    void ClearExhLeakArray(void);

    //Get time in inhalation
    ULONG GetTimeInInh(void);

    //Get last mandatory exhalation Vt
    LONG GetLastMandExhVt(void);

    //Get Previous Phase ID
    E_PhaseId GetPrevPhaseId(void);

    //Get Previous of Previous Phase ID
    E_PhaseId GetPrevprevPhaseId(void);

    // Blank Patient Data and Clear history data
    void ClearHistory (void);

    //$COMMON.ATTRIBUTE$
    //    Name: S_Instance
    //
    //    Description: static pointer of Pd Data
    //
    //    Units: None
    //
    //    Range: N/A
    //
    static Ptdata* S_Instance;

    //Constructor of PtData
    Ptdata (void);

    //$COMMON.ATTRIBUTE$
    //    Name: ComplianceSample[NUM_OF_SAMPLE_COMPLIANCE]
    //
    //    Description:  This array contains the Compliance of 8 breath
    //
    //    Units:
    //
    //    Range: N/A
    //
    double ComplianceSample[NUM_OF_SAMPLE_COMPLIANCE];

    //$COMMON.ATTRIBUTE$
    //    Name: ComplianceSampleIx
    //
    //    Description:  order of compliance sample
    //
    //    Units:
    //
    //    Range: N/A
    //
    SHORT ComplianceSampleIx;

    //$COMMON.ATTRIBUTE$
    //    Name: Compliance
    //
    //    Description:  Compliance value
    //
    //    Units:
    //
    //    Range: N/A
    //
    double Compliance;

    //$COMMON.ATTRIBUTE$
    //    Name: ComplianceAverage
    //
    //    Description:  Compliance average value
    //
    //    Units:
    //
    //    Range: N/A
    //
    double ComplianceAverage;

    //$COMMON.ATTRIBUTE$
    //    Name: mNumOfComplianceSample
    //
    //    Description:  number of Compliance Sample
    //
    //    Units:
    //
    //    Range: N/A
    //
    SHORT mNumOfComplianceSample;

    //$COMMON.ATTRIBUTE$
    //    Name: LastMandExhVt
    //
    //    Description:  the last mandatory exhalation Vt
    //
    //    Units:
    //
    //    Range: N/A
    //
    LONG LastMandExhVt;

    //$COMMON.ATTRIBUTE$
    //    Name: APRV_EndInhPress
    //
    //    Description:  APRV end inhalation pressure
    //
    //    Units:
    //
    //    Range: N/A
    //
    SHORT APRV_EndInhPress;

    //$COMMON.ATTRIBUTE$
    //    Name: hfoExhFlow
    //
    //    Description:  HFO exhalation flow
    //
    //    Units:
    //
    //    Range: N/A
    //
    LONG hfoExhFlow;

    bool IsFirst_1minute;

    //$COMMON.ATTRIBUTE$
    //    Name: ExhLeakArray[]
    //
    //    Description:  This array contains the Exh leak of 8 breath
    //
    //    Units:
    //
    //    Range: N/A
    //
    double ExhLeakArray[EIGHT_SAMPLES];

    //$COMMON.ATTRIBUTE$
    //    Name: EndExhFlowArray[]
    //
    //    Description:  This array contains the Flow at the end of Exhalation of 8 breath
    //
    //    Units:
    //
    //    Range: N/A
    //
    LONG EndExhFlowArray[EIGHT_SAMPLES];

    //$COMMON.ATTRIBUTE$
    //    Name: EndExhFlowIx
    //
    //    Description: Index to Current Value in EndExhFlowArray[]
    //
    //    Units: None
    //
    //    Range: 0 - (EIGHT_SAMPLES - 1)
    //
    SHORT EndExhFlowIx;

    //$COMMON.ATTRIBUTE$
    //    Name: ExhLeakIx
    //
    //    Description:  Index to Current Value in ExhLeakArray[]
    //
    //    Units:
    //
    //    Range: N/A
    //
    SHORT ExhLeakIx;

    //$COMMON.ATTRIBUTE$
    //    Name: mNCPAP_Pressure
    //
    //    Description:  The pressure in NCPAP mode
    //
    //    Units: CMH2O scaled by SCALE
    //
    //    Range: N/A
    //
    LONG mNCPAP_Pressure;

    //$COMMON.ATTRIBUTE$
    //    Name: MeanAirwayPressAvg
    //
    //    Description:  The mean airway pressure average
    //
    //    Units: CMH2O scaled by SCALE
    //
    //    Range: N/A
    //
    LONG MeanAirwayPressAvg;

    //$COMMON.ATTRIBUTE$
    //    Name: ExhTidalVol
    //
    //    Description:  The exhaled minute volume is a
    //                  calculation of the compliance and BTPS
    //                  compensated volume exhaled for both
    //                  spontaneous or mandatory breaths for
    //                  the current breath.
    //
    //    Units: Milli-Liters scaled by SCALE
    //
    //    Range: N/A
    //
    LONG ExhTidalVol;

    //$COMMON.ATTRIBUTE$
    //    Name: DisplayedExhTidalVol
    //
    //    Description:  The displayed exhaled tidal volume is a
    //                  calculation of the compliance and BTPS
    //                  compensated volume exhaled for both
    //                  spontaneous or mandatory breaths for
    //                  the current breath.  Smoothing algorthms
    //					are applied to displayed values. Used for alarm
    //					detection
    //
    //    Units: Mill-Liters scaled by SCALE
    //
    //    Range: N/A
    //
    LONG DisplayedExhTidalVol;

    //$COMMON.ATTRIBUTE$
    //    Name: MandExhTidalVolAverage
    //
    //    Description:  The MandExhTidalVolAverage is a
    //                  calculation of the compliance and BTPS
    //                  compensated volume exhaled for
    //                  mandatory breaths for the current breath.
    //
    //    Units: Milli-Liters scaled by SCALE
    //
    //    Range: N/A
    //
    LONG MandExhTidalVolAverage;

    //$COMMON.ATTRIBUTE$
    //    Name: ExhMinuteVol
    //
    //    Description:  The exhaled minute volume is a
    //                  calculation of the compliance and BTPS
    //                  compensated volume exhaled for both
    //                  spontaneous and mandatory breaths over
    //                  the previous one minute interval.
    //
    //    Units: Liters scaled by SCALE
    //
    //    Range: N/A
    //
    LONG ExhMinuteVol;

    //$COMMON.ATTRIBUTE$
    //    Name: SpontRespRate
    //
    //    Description:  Spontaneous respiratory rate is the
    //                  number of spontaneous breaths within
    //                  the last 1 minute.  Spontaneous
    //                  Respiratory Rate shall include only
    //                  spontaneous breaths.
    //
    //    Units: Breaths Per Minute (BPM)
    //
    //    Range: N/A
    //
    LONG SpontRespRate;

    //$COMMON.ATTRIBUTE$
    //    Name: SpontMinuteVol
    //
    //    Description:  Spontaneous minute volume is the sum of
    //                  spontaneous exhaled volume, BTPS and
    //                  compensated and normalized to 1 minute.
    //
    //    Units: Liters (L) scaled by SCALE
    //
    //    Range: N/A
    //
    LONG SpontMinuteVol;

    //$COMMON.ATTRIBUTE$
    //    Name: DeliveredO2
    //
    //    Description:  Percentage of Oxygen concentrentration
    //
    //    Units: Percentage of Oxygen
    //
    //    Range: N/A
    //
    LONG DeliveredO2;

    //$COMMON.ATTRIBUTE$
    //    Name:  RapidShallowBreathix
    //
    //    Description:  The Rapid Shallow Breathing Index is the ratio of
    //                  the respiratory rate to exhaled tidal volume.
    //
    //    Units:    BPM/L not scaled by SCALE
    //
    //    Range: N/A
    //
    LONG RapidShallowBreathix;

    //$COMMON.ATTRIBUTE$
    //    Name: IeRatio
    //
    //    Description:  I:E Ratio is the ratio of Inhalation to
    //                  expiratory time for the previous breath,
    //                  mandatory or spontaneous.
    //
    //    Units: Percentage of Inhalation/Exhalation
    //
    //    Range: N/A
    //
    LONG IeRatio;

    //$COMMON.ATTRIBUTE$
    //    Name: ComplVolume
    //
    //    Description:  The compliance volume is used to compensate the
    //                  monitored exhaled tidal volumes and the monitored
    //                  delivered tidal volumes.
    //
    //    Units: ml/cmH2O
    //
    //    Range: N/A
    //
    LONG ComplVolume;

    //$COMMON.ATTRIBUTE$
    //    Name:TotalRespRate
    //
    //    Description   Total respiratory rate is the total number of
    //                  breaths normalized to the last 1 minute.
    //                  Total Respiratory Rate shall include both
    //                  spontaneous and mandatory breaths.
    //
    //    Units: BPM
    //
    //    Range: N/A
    //
    LONG TotalRespRate;

    //$COMMON.ATTRIBUTE$
    //    Name: totalTime
    //
    //    Description: Total time in breath history table
    //                  used to compute minute calculations
    //
    //    Units: Milli-seconds
    //
    //    Range: None
    //
    LONG totalTime;

    //$COMMON.ATTRIBUTE$
    //    Name: minCalTime
    //
    //    Description: Total time used to compute minute calculations
    //
    //    Units: Milli-seconds
    //
    //    Range: None
    //
    LONG MinCalTime;

    //$COMMON.ATTRIBUTE$
    //    Name: breathsofData
    //
    //    Description: Number of breaths used to for minute calculations
    //
    //    Units: None
    //
    //    Range: 0 - 8
    //
    LONG breathsofData;

    //$COMMON.ATTRIBUTE$
    //    Name: numBreaths
    //
    //    Description: Number of breaths total with a MAX of MAX_BREATHS
    //
    //    Units: None
    //
    //    Range: 0 - 8
    //
    LONG numBreaths;

    //$COMMON.ATTRIBUTE$
    //    Name: Ix
    //
    //    Description: Index to Current Value in Breath History table
    //
    //    Units: None
    //
    //    Range: 0 - (MAX_BREATHS - 1)
    //
    LONG Ix;

    //$COMMON.ATTRIBUTE$
    //    Name: PdHist
    //
    //    Description: breath data history table
    //
    //    Units: None
    //
    //    Range: 0 - (MAX_BREATHS - 1)
    //
    PdHistory PdHist[MAX_BREATHS];

    //$COMMON.ATTRIBUTE$
    //    Name: ManExhTidalVol
    //
    //    Description: Mandatory Exhaled Tidal Volume
    //
    //    Units: milli-liters scaled by SCALE
    //
    //    Range: N/A
    //
    LONG ManExhTidalVol[MAX_BREATHS];

    //$COMMON.ATTRIBUTE$
    //    Name: NumMandBreaths
    //
    //    Description: Mandatory Breaths counter if counter
    //                  goes over 8 it is railed to be 8
    //
    //    Units: None
    //
    //    Range: 1 - 8
    //
    LONG NumMandBreaths;

    //$COMMON.ATTRIBUTE$
    //    Name: MandBreathsIx
    //
    //    Description: Mandatory Breaths index
    //
    //    Units: None
    //
    //    Range: 0 - 7
    //
    LONG MandBreathsIx;

    //$COMMON.ATTRIBUTE$
    //    Name: SpontBreaths
    //
    //    Description: Spontaneous  Breaths counter if counter
    //                  goes over 8 it is railed to be 8
    //
    //    Units: None
    //
    //    Range: 1 - 8
    //
    LONG SpontBreaths;

    //$COMMON.ATTRIBUTE$
    //    Name:CircuitComplianceFactor
    //
    //    Description: Compliance factor computed during EST or SST
    //
    //    Units: ml/cmH20 scaled by SCALE
    //
    //    Range: N/A
    //
    LONG CircuitComplianceFactor;

    //$COMMON.ATTRIBUTE$
    //    Name: CircuitComplianceON
    //
    //    Description: If the operator selects to have compliance compensation
    //                 this flag is set else it is not set
    //
    //    Units: eTrue or eFlase
    //
    //    Range: N/A
    //
    LONG CircuitComplianceON;

    //$COMMON.ATTRIBUTE$
    //    Name: NumExhSamples
    //
    //    Description: Numper of Bd application control cycles
    //                  during exhalation
    //
    //    Units: None
    //
    //    Range: N/A
    //
    LONG NumExhSamples;

    //$COMMON.ATTRIBUTE$
    //    Name: NumInhSamples
    //
    //    Description: Numper of Bd application control cycles
    //                  during inhalation
    //
    //    Units: None
    //
    //    Range: N/A
    //
    LONG NumInhSamples;

    //$COMMON.ATTRIBUTE$
    //    Name: ExhVolume
    //
    //    Description: The exhaled volume calculated by BD
    //
    //    Units: ml scaled by SCALE
    //
    //    Range: N/A
    //
    LONG ExhVolume;

    //$COMMON.ATTRIBUTE$
    //    Name: PressureSum
    //
    //    Description: Sum of Pressure measured during appication control cycle
    //
    //    Units: cmH20 scaled by SCALE
    //
    //    Range: N/A
    //
    LONG PressureSum;

    //$COMMON.ATTRIBUTE$
    //    Name: ExhVolumeCompensated
    //
    //    Description: The exhaled volume calculated by BD
    //
    //    Units: ml scaled by SCALE
    //
    //    Range: N/A
    //
    LONG ExhVolumeCompensated;

    //$COMMON.ATTRIBUTE$
    //    Name: EndExhPress
    //
    //    Description:  End exhalation pressure is the pressure measured at
    //                  the end of the exhalation phase.  End exhalation
    //                  pressure shall be the average pressure measured in
    //                  the expiratory limb during the last 4 application
    //                  control cycles of exhalation.
    //
    //    Units: cmH2O scaled by SCALE
    //
    //    Range: N/A
    //
    LONG EndExhPress;

    //$COMMON.ATTRIBUTE$
    //    Name: EndInhPress
    //
    //    Description:  The average of the last for application control
    //                  cycles of Inhalation
    //
    //    Units: cmH2O scaled by SCALE
    //
    //    Range: N/A
    //
    LONG EndInhPress;

    //$COMMON.ATTRIBUTE$
    //    Name: PeakInhPress
    //
    //    Description:  Peak Inhalation pressure is the
    //                  maximum pressure measured during the
    //                  Inhalation phase of the current breath,
    //                  regardless of the breath type.
    //
    //    Units: cmH2O scaled by SCALE
    //
    //    Range: N/A
    //
    LONG PeakInhPress;

    //$COMMON.ATTRIBUTE$
    //    Name: manInspHoldTime
    //
    //    Description:
    //
    //    Units: cmH2O scaled by SCALE
    //
    //    Range: N/A
    //
    LONG manInspHoldTime;

    //$COMMON.ATTRIBUTE$
    //    Name: maxvalue
    //
    //    Description:
    //
    //    Units: cmH2O scaled by SCALE
    //
    //    Range: N/A
    //
    LONG maxpress;

    //$COMMON.ATTRIBUTE$
    //    Name: minvalue
    //
    //    Description:
    //
    //    Units: cmH2O scaled by SCALE
    //
    //    Range: N/A
    //
    LONG minpress;

    //$COMMON.ATTRIBUTE$
    //    Name: amplitude
    //
    //    Description:
    //
    //    Units: cmH2O scaled by SCALE
    //
    //    Range: N/A
    //
    LONG amplitude;

    //$COMMON.ATTRIBUTE$
    //    Name: map
    //
    //    Description:  Mean Air Pressure
    //
    //    Units: cmH2O scaled by SCALE
    //
    //    Range: N/A
    //
    LONG map;

    //$COMMON.ATTRIBUTE$
    //    Name: TimeInInh
    //
    //    Description:  Time in Inhalation phase
    //
    //    Units: ms
    //
    //    Range: N/A
    //
    ULONG TimeInInh;

    //$COMMON.ATTRIBUTE$
    //    Name: TimeInExh
    //
    //    Description: Time in Exhalation phase
    //
    //    Units: ms
    //
    //    Range: N/A
    //
    ULONG TimeInExh;

    //$COMMON.ATTRIBUTE$
    //    Name: SupportPressure
    //
    //    Description:  Pressure Support used in computation of
    //                  Rapid Shallow Breathing index
    //
    //    Units: CMH2O scaled by SCALE
    //
    //    Range: N/A
    //
    LONG SupportPressure;

    //$COMMON.ATTRIBUTE$
    //    Name: BaselinePressSetting
    //
    //    Description:  Baseline Pressure Setting used in the
    //                 computation of rapid shallow breathing index
    //
    //    Units: CMH2O scaled by SCALE
    //
    //    Range: N/A
    //
    LONG BaselinePressSetting;

    //$COMMON.ATTRIBUTE$
    //    Name: TvSetting
    //
    //    Description:  Current Tidal Volume Setting used in the computation
    //                  of the Displayed Tidal Volume can only be retrieved
    //					during volume ventilation type
    //
    //    Units: milli-liters scaled by SCALE
    //
    //    Range: N/A
    //
    LONG TvSetting;

    //$COMMON.ATTRIBUTE$
    //    Name: OldTvSetting
    //
    //    Description:  Previous Tidal Volume Setting used in the computation
    //                  of the Displayed Tidal Volume
    //
    //    Units: milli-liters scaled by SCALE
    //
    //    Range: N/A
    //
    LONG OldTvSetting;

    //$COMMON.ATTRIBUTE$
    //    Name: InhType
    //
    //    Description: Inhalation Type
    //
    //    Units: N/A
    //
    //    Range: N/A
    //
    E_BreathPhaseIndicator InhType;

    //$COMMON.ATTRIBUTE$
    //    Name: VentType
    //
    //    Description: Current Ventilation Type
    //
    //    Units: N/A
    //
    //    Range: N/A
    //
    E_VentilationType VentType;

    //$COMMON.ATTRIBUTE$
    //    Name: OldVentType
    //
    //    Description: Previous Ventilation Type
    //
    //    Units: N/A
    //
    //    Range: N/A
    //
    E_VentilationType OldVentType;

    //$COMMON.ATTRIBUTE$
    //    Name: Mode
    //
    //    Description: Mode of ventilation
    //
    //    Units: N/A
    //
    //    Range: N/A
    //
    E_VentilationMode Mode;

    //$COMMON.ATTRIBUTE$
    //    Name: OldMode
    //
    //    Description: Previous Mode of ventilation
    //
    //    Units: N/A
    //
    //    Range: N/A
    //
    E_VentilationMode OldMode;

    //$COMMON.ATTRIBUTE$
    //    Name: HighO2AlarmLimit
    //
    //    Description: High O2 alarm limit used to detect hi O2 alarms
    //
    //    Units: percent
    //
    //    Range: N/A
    //
    LONG HighO2AlarmLimit;

    //$COMMON.ATTRIBUTE$
    //    Name: LowO2AlarmLimit
    //
    //    Description: Low O2 alarm limit used to detect low O2 alarms
    //
    //    Units: percent scaled by SCALE
    //
    //    Range: N/A
    //


    LONG LowO2AlarmLimit;

    //$COMMON.ATTRIBUTE$
    //    Name: LowBaseLineAlarmLimit
    //
    //    Description:  Low Base Line Alarm Limit
    //
    //    Units: BPM
    //
    //    Range: N/A
    //
    LONG LowBaseLineAlarmLimit;

    //$COMMON.ATTRIBUTE$
    //    Name: HrrAlarmLimit
    //
    //    Description: High Respiratory Rate Alarm Limit
    //
    //    Units: BPM
    //
    //    Range: N/A
    //
    LONG HrrAlarmLimit;

    //$COMMON.ATTRIBUTE$
    //    Name: LemvLimit
    //
    //    Description: Low exhaled mandatory volume Limit for alarms
    //
    //    Units: milli-liters scaled by SCALE
    //
    //    Range: N/A
    //
    LONG LemvLimit;

    //$COMMON.ATTRIBUTE$
    //    Name: LestvLimit
    //
    //    Description:  Low exhaled spontaneous tidal volume limit for
    //                  alarms checking
    //
    //    Units: milli-liters scaled by SCALE
    //
    //    Range: N/A
    //
    LONG LestvLimit;

    //$COMMON.ATTRIBUTE$
    //    Name: LemtvLimit
    //
    //    Description:  Low exhaled mandatory tidal volume limit
    //
    //    Units: milli-liters scaled by SCALE
    //
    //    Range: N/A
    //
    LONG LemtvLimit;

    //$COMMON.ATTRIBUTE$
    //    Name: LipLimit
    //
    //    Description:  Low Inhalation pressure limit for alarm detection
    //
    //    Units: CMH2O scaled by SCALE
    //
    //    Range: N/A
    //
    LONG LipLimit;

    //$COMMON.ATTRIBUTE$
    //    Name: HipLimit
    //
    //    Description:  Low Inhalation pressure limit for alarm detection
    //
    //    Units: CMH2O scaled by SCALE
    //
    //    Range: N/A
    //
    LONG HipLimit;

    //$COMMON.ATTRIBUTE$
    //    Name: HemvLimit
    //
    //    Description:  High exhalated minute volume
    //
    //    Units: milli-liters scaled by SCALE
    //
    //    Range: N/A
    //
    LONG HemvLimit;

    //$COMMON.ATTRIBUTE$
    //    Name: LetvLimit
    //
    //    Description:  Low exhaled tidal volume limit
    //
    //    Units: milli-liters scaled by SCALE
    //
    //    Range: N/A
    //
    LONG LetvLimit;

    //$COMMON.ATTRIBUTE$
    //    Name: prevphaseId_vt
    //
    //    Description:  Previous phase ID of Vt
    //
    //    Units:
    //
    //    Range: N/A
    //
    E_PhaseId prevphaseId_vt;

    //$COMMON.ATTRIBUTE$
    //    Name: prevphaseId_comp
    //
    //    Description:  Previous phase ID of comp
    //
    //    Units:
    //
    //    Range: N/A
    //
    E_PhaseId prevphaseId_comp;

    //$COMMON.ATTRIBUTE$
    //    Name: prevprevphaseId_vt
    //
    //    Description:  Previous of previous phase ID of Vt
    //
    //    Units:
    //
    //    Range: N/A
    //
    E_PhaseId prevprevphaseId_vt;

    //$COMMON.ATTRIBUTE$
    //    Name: prevprevphaseId_comp
    //
    //    Description:  Previous of previous phase ID of Comp
    //
    //    Units:
    //
    //    Range: N/A
    //
    E_PhaseId prevprevphaseId_comp;

    //$COMMON.ATTRIBUTE$
    //    Name: sample12_fill_flg
    //
    //    Description:  flag to indicate for 12 breath
    //
    //    Units:
    //
    //    Range: N/A
    //
    bool sample12_fill_flg;

    //$COMMON.ATTRIBUTE$
    //    Name: sample12_fill_flg
    //
    //    Description:  flag to indicate for 12 breath in comp
    //
    //    Units:
    //
    //    Range: N/A
    //
    bool sample12_fill_flg_comp;

    //$COMMON.ATTRIBUTE$
    //    Name: O2Concentration
    //
    //    Description: Oxygen concentration percentage
    //
    //    Units: percent scaled by SCALE
    //
    //    Range: N/A
    //
    LONG O2Concentration;

    //$COMMON.ATTRIBUTE$
    //    Name: PeepAdjEndInhPress
    //
    //    Description:  End inhalation pressure, compensated for Baseline
    //      pressure.
    //
    //    Units: cmH2O scaled by SCALE
    //
    //    Range: N/A
    //
    LONG PeepAdjEndInhPress;

    //$COMMON.ATTRIBUTE$
    //    Name: O2Present
    //
    //    Description: If Oxygen sensor is present eTrue else eFalse
    //
    //    Units: eTrue or eFalse
    //
    //    Range: N/A
    //
    E_Bool O2Present;

    //$COMMON.ATTRIBUTE$
    //    Name: InhMinuteVol
    //
    //    Description: Inhalation minutes Volume
    //
    //    Units:
    //
    //    Range: N/A
    //
    LONG InhMinuteVol;

    //$COMMON.ATTRIBUTE$
    //    Name: ProxymalLeakValue
    //
    //    Description: Proxymal leak value
    //
    //    Units:
    //
    //    Range: N/A
    //
    float ProxymalLeakValue;

    //$COMMON.ATTRIBUTE$
    //    Name: hfoVe
    //
    //    Description: HFO ventilator
    //
    //    Units:
    //
    //    Range: N/A
    //
    LONG hfoVe;

    //$COMMON.ATTRIBUTE$
    //    Name: hfoBaseFlow
    //
    //    Description: HFO base flow
    //
    //    Units:
    //
    //    Range: N/A
    //
    LONG hfoBaseFlow;

    //$COMMON.ATTRIBUTE$
    //    Name: mInhaledTidalVolume
    //
    //    Description: Inhalation tidal volume
    //
    //    Units:
    //
    //    Range: N/A
    //
    LONG mInhaledTidalVolume;

    //$COMMON.ATTRIBUTE$
    //    Name: mInhVol
    //
    //    Description: Inhalation volume
    //
    //    Units:
    //
    //    Range: N/A
    //
    LONG mInhVol;

    //$COMMON.ATTRIBUTE$
    //    Name: VteInAPRV
    //
    //    Description: Vte in APRV
    //
    //    Units:
    //
    //    Range: N/A
    //
    LONG VteInAPRV;

    //$COMMON.ATTRIBUTE$
    //    Name: ExhTidalVolArray
    //
    //    Description: Array of 8 Exh Tidal Volume
    //
    //    Units: percent scaled by SCALE
    //
    //    Range: N/A
    //
    LONG ExhTidalVolArray[EIGHT_SAMPLES];

    //$COMMON.ATTRIBUTE$
    //    Name: ExhTidalVolIx
    //
    //    Description: Current Index of ExhTidalVolArray[]
    //
    //    Units: percent scaled by SCALE
    //
    //    Range: N/A
    //
    LONG ExhTidalVolIx;

    //$COMMON.ATTRIBUTE$
    //    Name: ExhTidalVolNum
    //
    //    Description: Number samples of ExhTidalVolArray[]
    //
    //    Units: percent scaled by SCALE
    //
    //    Range: N/A
    //
    LONG ExhTidalVolNum;

    //$COMMON.ATTRIBUTE$
    //    Name: ExhTidalVolArray12
    //
    //    Description: Array of 12 Exh Tidal Volume for VA
    //
    //    Units: percent scaled by SCALE
    //
    //    Range: N/A
    //
    LONG ExhTidalVolArray12[TWELVE_SAMPLES]; //for new VA

    //$COMMON.ATTRIBUTE$
    //    Name: ExhTidalVolIx12
    //
    //    Description: Current Index of ExhTidalVolArray12[]
    //
    //    Units: percent scaled by SCALE
    //
    //    Range: N/A
    //
    LONG ExhTidalVolIx12; //for new VA

    //$COMMON.ATTRIBUTE$
    //    Name: ExhTidalVolNum12
    //
    //    Description: Number samples of ExhTidalVolArray12[]
    //
    //    Units: percent scaled by SCALE
    //
    //    Range: N/A
    //
    LONG ExhTidalVolNum12; //for new VA

    //$COMMON.ATTRIBUTE$
    //    Name: ComplianceArray12
    //
    //    Description: Array of 12 Compliance for VA
    //
    //    Units: percent scaled by SCALE
    //
    //    Range: N/A
    //
    double ComplianceArray12[TWELVE_SAMPLES]; //for new VA

    //$COMMON.ATTRIBUTE$
    //    Name: ComplianceIx12
    //
    //    Description: Current Index of ComplianceArray12[]
    //
    //    Units: percent scaled by SCALE
    //
    //    Range: N/A
    //
    LONG ComplianceIx12; //for new VA

    //$COMMON.ATTRIBUTE$
    //    Name: ComplianceNum12
    //
    //    Description: Number samples of ComplianceArray12[]
    //
    //    Units: percent scaled by SCALE
    //
    //    Range: N/A
    //
    LONG ComplianceNum12; //for new VA

    // Compute Number of breaths used in minute calculations
    void CompNumBreaths (void);

    //  Compute Circuit Compliance Volume and return
    void CompCircuitComplianceVolume (void);

    //compute complicance
    void CompCompliance(void);

    //Compute average compliance
    void CompAverageCompliance(void);

    //Get min/max of compliance
    double GetMinCompliance(SHORT);
    double GetMaxCompliance(SHORT);

    //compute compliance in APRV
    void CompCompliance_InAPRV(void);

    // Compute the Mandatory Exhaled Tidal Volume
    void UpdateMandExhTidalVolume (void);

    //  Compute Exhaled Tidal Volume and return
    void CompExhTidalVolume (void);

    //  Compute Exhaled Tidal Volume in APRV and return
    void CompExhTidalVolumeInAPRV(void);

    //  Compute Exhaled Minute Volume and return
    void CompExhMinuteVolume (void);

    //  Compute Inhaled Minute Volume and return
    void CompInhMinuteVolume(void);

    // Compute Rapid Shallow Breathing Index
    void CompRapidShallowBreathix (void);

    // Compute Inhalation and Exhalation Ratio
    void CompIeRatio (void);

    // Compute total respiratory rate
    void CompRespRate (void);
    void CompProxymalLeak(void);

    //Get HFO data
    void GetHFOData(void);

    // Compute Mean Airway pressure
    void CompMeanAirwayPressure (void);

    // Get Inhalation Data
    void GetInhalationData (void);

    // Clear certain alarms when mode and ventilation change
    void ModeInducedAlarmClearing (void);

    // Get Exhalation Data
    void GetExhalationData (void);

    // Build Inhalation Message
    void BuildInhMsg (void);

    // Build Exhalation Message
    void BuildExhMsg (void);

    // Build O2 Message
    void BuildO2Msg (void);

    // Build HFO Message
    void BuildHFOMsg(void);

    // Blank Patient Data
    void BlankingData (void);

    //Compute leak compensate
    void CompLeakCompensate(void);

    //Calculate HDO exhalation
    void CalculateHFOExh(void);

    //build APRV PLow
    void BuildAPRVPLow(void);

    //Get end exhalation flow
    void GetEndExhFlow(void);

    //Get min/max Vte array
    LONG GetMinVteArray(void);
    LONG GetMaxVteArray(void);

    //Minimum compliance in 12 samples for VA
    void GetMinComplianceArray12(double *add);

    //Maximum compliance in 12 samples for VA
    void GetMaxComplianceArray12(double *add);

    //Minimum Vte in 12 samples for VA
    void GetMinVteArray12(LONG *add);

    //Maximum Vte in 12 samples for VA
    void GetMaxVteArray12(LONG *add);

    //returns true if inhalation is spontaneous
    E_Bool PdSpontaneous (E_BreathPhaseIndicator inhtype);
    //returns true if inhalation is mandatory
    E_Bool PdMandatory (E_BreathPhaseIndicator inhtype);
};

#endif  //PTDATA_H

