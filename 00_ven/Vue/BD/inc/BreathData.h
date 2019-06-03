/******************************************************************************/
//$COMMON.H$
//   File Name:  BreathData.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: 
//      This class holds pressure, flow and volume data for a single breath.
/******************************************************************************/
#ifndef BREATHDATA_H
#define BREATHDATA_H  

#include "CommonConstants.h"
#include "BdSetting.h"

const SHORT NUM_END_P_SAMPLES = 4;
const SHORT NUM_EXH_FLOW_SAMPLES = 200;

class BreathData
{
public:

    // Invoked in BD Task thread of control.
    //Get sum of flow in inhalation
    LONG GetNetFlowSumInInh (void) const;

    //Get sum of flow in exhalation
    LONG GetNetFlowSumInExh (void) const;

    //Get sum of compensate net flow in inhalation, exhalation
    LONG GetCompensatedNetFlowSumInInh(void) const;
    LONG GetCompensatedNetFlowSumInExh(void) const;


    // Invoked in Patient Data Task thread of control.
    //Get peak inhalation pressure
    SHORT GetPeakInhPress (void) const;

    //Reset Peak Inhalation pressure
    void ResetPeakInhPress(void);

    //Get number of inhalation sample
    SHORT GetNumInhSamples (void) const;

    //GEt number of exhalation sample
    SHORT GetNumExhSamples (void) const;

    //Get sum of pressure
    LONG GetPressureSum (void) const;

    //Get exhalation volume
    LONG GetExhVolume (void) const;

    //Get time in inhalation
    UNSIGNED GetTimeInInh (void) const;

    //Get time in exhalation
    UNSIGNED GetTimeInExh (void) const;

    //Get inhalation type
    E_BreathPhaseIndicator GetInhType (void) const;

    //Set pd Done with value
    void SetPdDone (E_Bool flag);

    //Get Pd Done Flag
    E_Bool GetPdDoneFlag(void);

    //Reset Vte
    void ResetVte(void);

    //Get Vte
    LONG GetVte(void);

    //Plateu start
    void PlateauStart(E_BreathPhaseIndicator inhType);
    //APRV trigger inhalation start
    void APRVTriggerInhStart(E_BreathPhaseIndicator inhType);

    //Set time exhalation in first breath
    void SetTimeExhInFirstBreath(UNSIGNED value);

    //Update Time in APRV Low Phase, Phigh Phase
    void UpdateTimeIn_APRVPLowPhase(void);
    void UpdateTimeIn_APRVPHighPhase(void);

    //use to calculate monitor volume
    LONG GetMoniInhVolume(void);
    LONG GetMoniExhVolume(void);

    //Get compensate exhalation and inhalation volume
    LONG GetCompensatedExhVolume(void);
    LONG GetCompensatedInhVolume(void);

    //Get inhalation volume
    LONG GetInhVolume(void);

    //Clear inhalation pressure data
    void clearInhPressData(void);

    // Constructor
    BreathData (void);

    // Invoked in BD Task thread of control.
    void InhalationStart (E_BreathPhaseIndicator inhType);
    void ExhalationStart (void);

    //Update time in exhalation
    void UpdateTimeInExhalation (void);

    //Process Bd Cycle in Inhalation and exhalation
    LONG ProcessBdCycleInInh (SHORT inhPressure, LONG netFlow);
    LONG ProcessBdCycleInExh (SHORT exhPressure, LONG netFlow);

    // Invoked by both the BD and Patient Data Tasks.
    SHORT GetEndInhPress (E_Bool compensated = eFalse) const;

    // Invoked in Patient Data Task thread of control.
    SHORT GetEndExhPress (void) const;

    //Compute Vte in Phase
    void ComputeVteInPhase(LONG netFlow);

    //Get APRV in Phigh and PLow
    SHORT GetAPRV_PHigh(void);
    SHORT GetAPRV_PLow(void);

    //Get end inhalation and exhalation flow
    LONG GetEndInhFlow(void);
    LONG GetEndExhFlow(void);

    //Get average exhalation flow
    LONG GetAgvExhFlow(void);

    //Process Bd Cycle APRV in Phigh, Plow
    void ProcessBdCycleAPRV_PHigh (SHORT pressure);
    void ProcessBdCycleAPRV_PLow (SHORT pressure);

protected:

private:
    // copy constructor
    BreathData (const BreathData& breathData);

    //$COMMON.ATTRIBUTE$
    //  Name: InhPress
    //  Description:  Array of last NUM_END_P_SAMPLES of inhalation pressure
    //  Units: None
    //  Range: n/a
    SHORT InhPress[NUM_END_P_SAMPLES];

    //$COMMON.ATTRIBUTE$
    //  Name: ExhPress
    //  Description:  Array of last NUM_END_P_SAMPLES of exhalation pressure
    //  Units: None
    //  Range: n/a
    SHORT ExhPress[NUM_END_P_SAMPLES];

    //$COMMON.ATTRIBUTE$
    //  Name: NetFlowSumInInh
    //  Description:  Sum of (delivered flow - exh flow) during inhalation.
    //  Units: ml/sec * SCALE
    //  Range: 0 - Max LONG
    LONG NetFlowSumInInh;

    //$COMMON.ATTRIBUTE$
    //  Name: NetFlowSumInExh
    //  Description:  Sum of (exh flow - deliverd flow) during exhalation.
    //  Units: ml/sec * SCALE
    //  Range: 0 - Max LONG
    LONG NetFlowSumInExh;

    //$COMMON.ATTRIBUTE$
    //  Name: Compensated_ExhVolume
    //  Description:  Calculated exhaled volume during exhalation.
    //  Units: ml * SCALE
    //  Range: 0 - Max LONG
    LONG Compensated_ExhVolume;

    //$COMMON.ATTRIBUTE$
    //  Name: Compensated_InhVolume
    //  Description:  Calculated exhaled volume during exhalation.
    //  Units: ml * SCALE
    //  Range: 0 - Max LONG
    LONG Compensated_InhVolume;

    //$COMMON.ATTRIBUTE$
    //  Name: ExhVolume
    //  Description:  Calculated exhaled volume during exhalation.
    //  Units: ml * SCALE
    //  Range: 0 - Max LONG
    LONG ExhVolume;

    //$COMMON.ATTRIBUTE$
    //  Name: InhVolume
    //  Description:  Calculated exhaled volume during exhalation.
    //  Units: ml * SCALE
    //  Range: 0 - Max LONG
    LONG InhVolume;

    //$COMMON.ATTRIBUTE$
    //  Name: PressureSum
    //  Description:  Sum of pressure readings sample in the exhalation limb
    //  Units: cmH2O * SCALE
    //  Range: Min LONG - Max LONG
    LONG PressureSum;

    //$COMMON.ATTRIBUTE$
    //  Name: PeakInhPress
    //  Description:  Peak pressure measured during inhalation.
    //  Units: cmH2O * SCALE
    //  Range: -100 to 200, scaled
    SHORT PeakInhPress;

    //$COMMON.ATTRIBUTE$
    //  Name: InhPix
    //  Description:  Index into the InhPress array
    //  Units: none
    //  Range: 0 - (NUM_END_P_SAMPLES - 1)
    SHORT InhPix;

    //$COMMON.ATTRIBUTE$
    //  Name: ExhPix
    //  Description:  Index into the ExhPress array
    //  Units: none
    //  Range: 0 - (NUM_END_P_SAMPLES - 1)
    SHORT ExhPix;

    //$COMMON.ATTRIBUTE$
    //  Name: InhPix
    //  Description:  Index into the InhFlow array
    //  Units: none
    //  Range: 0 - (NUM_END_P_SAMPLES - 1)
    SHORT InhFix;

    //$COMMON.ATTRIBUTE$
    //  Name: NumInhSamples
    //  Description:  Number of inhalation samples collected.
    //  Units: none
    //  Range: 0 - (Max Inhalation Time/BD Cycle Time)
    SHORT NumInhSamples;

    //$COMMON.ATTRIBUTE$
    //  Name: NumExhSamples
    //  Description:  Number of inhalation samples collected.
    //  Units: none
    //  Range: 0 - (Max Apnea Interval / BD Cycle Time)
    SHORT NumExhSamples;    // number of exh samples collected

    //$COMMON.ATTRIBUTE$
    //  Name: TimeInInh
    //  Description:  Inhalation Time
    //  Units: System TICKS
    //  Range: 0 - 9 seconds, in TICKS
    UNSIGNED TimeInInh;

    //$COMMON.ATTRIBUTE$
    //  Name: TimeInExh
    //  Description:  Exhalation Time
    //  Units: System TICKS
    //  Range: 0 - Max Apnea Interval, in TICKS
    UNSIGNED TimeInExh;

    //$COMMON.ATTRIBUTE$
    //  Name: InhType
    //  Description:  Inhalation type (mandatory, assisted, spontaneous,
    //      supported).
    //  Units: none
    //  Range: n/a
    E_BreathPhaseIndicator InhType;

    //$COMMON.ATTRIBUTE$
    //  Name: PdDone
    //  Description:  Indicates if the Patient Data Task is finished using
    //      the data in the BreathData object.  The BD Task uses this flag
    //      to make sure that the Patient Data Task has not fallen so far
    //      behind that the BD task is about to stomp on data not yet used
    //      by Patient Data.
    //  Units: none
    //  Range: n/a
    E_Bool PdDone;

    LONG VteInPhase;

    //$COMMON.ATTRIBUTE$
    //  Name: MoniInhVolume
    //  Description:  inhalation volume that use to calculate to display in volume graph
    //  Units: none
    //  Range: n/a
    LONG MoniInhVolume;

    //$COMMON.ATTRIBUTE$
    //  Name: MoniExhVolume
    //  Description:  exhalation volume that use to calculate to display in volume graph
    //  Units: none
    //  Range: n/a
    LONG MoniExhVolume;

    //$COMMON.ATTRIBUTE$
    //  Name: Compensated_NetFlowSumInIn
    //  Description:  Sum of (delivered flow - exh flow) during inhalation with compensation.
    //  Units: ml/sec * SCALE
    //  Range: 0 - Max LONG
    LONG Compensated_NetFlowSumInInh;

    //$COMMON.ATTRIBUTE$
    //  Name: NetFlowSumInExh_Compensate
    //  Description:  Sum of (exh flow - deliverd flow) during exhalation with compensation..
    //  Units: ml/sec * SCALE
    //  Range: 0 - Max LONG
    LONG Compensated_NetFlowSumInExh;

    //$COMMON.ATTRIBUTE$
    //  Name: InhFlow
    //  Description:  Array of last NUM_END_P_SAMPLES of inhalation flow
    //  Units: None
    //  Range: n/a
    LONG InhFlow[NUM_END_P_SAMPLES];

    //$COMMON.ATTRIBUTE$
    //  Name: ExhFlow
    //  Description:  Array of last NUM_END_P_SAMPLES of exhalation flow
    //  Units: None
    //  Range: n/a
    LONG ExhFlow[NUM_END_P_SAMPLES];

    //$COMMON.ATTRIBUTE$
    //  Name: PHigh
    //  Description:  Array of last NUM_END_P_SAMPLES of exhalation flow
    //  Units: None
    //  Range: n/a
    SHORT PHigh[NUM_END_P_SAMPLES];

    //$COMMON.ATTRIBUTE$
    //  Name: PLow
    //  Description:  Array of last NUM_END_P_SAMPLES of exhalation flow
    //  Units: None
    //  Range: n/a
    SHORT PLow[NUM_END_P_SAMPLES];

    //$COMMON.ATTRIBUTE$
    //  Name: ExhFlow
    //  Description:  Array of last NUM_END_P_SAMPLES of exhalation flow
    //  Units: None
    //  Range: n/a
    LONG ExhFlowData[NUM_EXH_FLOW_SAMPLES];

    //$COMMON.ATTRIBUTE$
    //  Name: NetFlowSumIn
    //  Description:  Sum of flow during
    //  Units: ml/sec * SCALE
    //  Range: 0 - Max LONG
    LONG NetFlowSum;

    //$COMMON.ATTRIBUTE$
    //  Name: InhFlowSum
    //  Description:  sum of inhalation flow that is not be truntcast
    //  Units: none
    //  Range: n/a
    LONG InhFlowSum;

    //$COMMON.ATTRIBUTE$
    //  Name: ExhFlowSum
    //  Description:  sum of exhalation flow that is not be truntcast
    //  Units: none
    //  Range: n/a
    LONG ExhFlowSum;

    //$COMMON.ATTRIBUTE$
    //  Name: ExhFDataix
    //  Description:  exhalatoin data ix
    //  Units: none
    //  Range: n/a
    int ExhFDataix;

    //$COMMON.ATTRIBUTE$
    //  Name: timer200ms
    //  Description:  2ooms
    //  Units: none
    //  Range: n/a
    int timer200ms;

    //$COMMON.ATTRIBUTE$
    //  Name: BaselinePress
    //  Description:  Baseline pressure setting used for the exhalation
    //      phase associated with this BreathData object.
    //  Units: none
    //  Range: n/a
    LONG BaselinePress;

    //$COMMON.ATTRIBUTE$
    //  Name: ExhFix
    //  Description:  Index into the ExhFlow array
    //  Units: none
    //  Range: 0 - (NUM_END_P_SAMPLES - 1)
    SHORT ExhFix;

    //$COMMON.ATTRIBUTE$
    //  Name: ExhFlowCnt
    //  Description:
    //  Units: none
    //  Range: n/a
    LONG ExhFlowCnt;

    //$COMMON.ATTRIBUTE$
    //  Name: Compliance
    //  Description:
    //  Units: none
    //  Range: n/a
    LONG Compliance;

    //$COMMON.ATTRIBUTE$
    //  Name: mPHighIndex
    //  Description: Phigh index
    //  Units: none
    //  Range: n/a
    SHORT mPHighIndex;

    //$COMMON.ATTRIBUTE$
    //  Name: mPLowIndex
    //  Description: Plow index
    //  Units: none
    //  Range: n/a
    SHORT mPLowIndex;

};

#endif //BREATHDATA_H
