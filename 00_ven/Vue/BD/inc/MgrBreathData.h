/******************************************************************************/
//$COMMON.H$
//   File Name:  MgrBreathData.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: 
//      This class provides an interface between the Breath Delivery
//      Task and the Patient Data Task.  The Breath Data Manager object
//      manages the 2 BreathData objects which are contain flow, pressure
//      and volume data updated by BD and used by both BD and Patient Data.
/******************************************************************************/
#ifndef MGRBREATHDATA_H
#define MGRBREATHDATA_H  

#include "BreathData.h"
#include "Timer.h"
#include "databuffer.h"

class MgrBreathData : public Timer
{
public:
    static MgrBreathData* S_GetInstance (void);

    // These methods are invoked by the Breath Delivery Task
    //Get BdBreathData Pointer
    BreathData* GetBdBreathDataPtr (void);

    //Get monitor pressure
    SHORT GetMonitoredPressure (void);

    //Get Exhalation flow
    LONG GetExhaledFlow (void);

    //Get deleverd flow
    LONG GetDeliveredFlow (void);

    //Get hfo base flow
    LONG GetHFOBaseFlow(void);

    //Get Peak deliverd flow
    LONG GetPeakDeliveredFlow (void);

    //get HFO exhalation flow
    LONG GetExhFlow(void);

    // This method is invoked by the Patient Data Task
    BreathData* GetPdBreathDataPtr (void);

    //Clear compute sate buffer
    void ClearCompensateBuffer(void);

    //Get Proxy flow
    LONG GetProxyFlow(void);

    //Get leak volume time
    LONG GetLeakVolTime(void);

    //GEt inhalation volume on one BR times
    LONG GetInhVolOneBRTime(void);

    //Get exhalation volume on one BR times
    LONG GetExhVolOneBRTime(void);

    //Get peak proxy flow
    LONG GetPeakProxyFlow(void);

    //Get hfo amplitude
    LONG GetHFOAmplitude(void);

    //Get APRV Total Breath Rate
    LONG GetAPRVTotalBreathRate(void);

    //Get APRV Spon Total Breath Rate
    LONG GetAPRVSpontBreathRate(void);

    //set manual breath or not
    void SetManInspActive(E_Bool active);

    //APRV exhalation phase start in PHigh, PLow
    void APRV_ExhPhaseStart_InPHigh(E_BreathPhaseIndicator inhType);
    void APRV_ExhPhaseStart_InPLow(E_BreathPhaseIndicator inhType);

    //Get max, min pressure data
    LONG GetMaxPresData(void);
    LONG GetMinPresData(void);

    //get current amptitute
    LONG GetCurrentAmp(void);

    //call funtion when timer is expired
    virtual void ExpirationRoutine(void);

    //APRV Plow Phase start
    void APRV_PLowPhaseStart(void);

    //Process NCPAP
    void ProcessNCPAP(void);

    //get tidal volume end inhalation
    LONG GetTidalVolumeEndInh(void);

    //set first breath
    void SetFirstBreath(void);

    //Initialize breath data pointer
    void InitializeBreathDataPtr(void);

    //get hfo tital volume
    LONG GetHFOTidalVolume(void);

    // These methods are invoked by the Breath Delivery Task
    void InhalationStart (E_BreathPhaseIndicator inhType);
    void ExhalationStart (void);
    void PlateauStart (E_BreathPhaseIndicator inhType);
    void NonVentStart (void);
    void HFOStart(void);
    void NCPAPStart(void);
    void ProcessBdCycle (void);

    void ProcessIMVVentilationData(void);
    void ProcessHFOVentilationData(void);
    void ProcessAlarmMode(void);

    // This method is invoked by the Patient Data Task
    void SetPdDone (void);

    //Reset APRV BR
    void ResetAPRVBR(void);
    void APRV_InhPhaseStart(E_BreathPhaseIndicator inhType);
    void APRV_PHighPhaseStart(E_BreathPhaseIndicator inhType);
    void APRV_ExhPhaseEnd_InPHigh(void);

    //Reset Volume value
    void ResetVol(void);

protected:

private:
    MgrBreathData (void);
    MgrBreathData (const MgrBreathData& bpd);

    //Initialize class
    void Initialize (void);
    //Update PEEP Alarm
    void UpdatePEEPAlarm(void);
    //Update PEEP data
    void UpdatePEEPData(void);
    //Update Phigh alarm
    void UpDatePHigh_Alarm(void);
    //Update PLow Alarm
    void UpDatePLow_Alarm(void);

    //Compute hfo vopume
    void CompHfoVolume(LONG netFlow,LONG pressure);

    //Process APRV High pressure
    void ProcessAPRVHighPress(void);
    void ProcessAPRVLowPress(void);

    //compute hfo base flow
    void CompHfoBaseFlow(void);

    static LONG mAPRVSpontBreathRate;
    static LONG mAPRVTotalBreathRate;
    //$COMMON.ATTRIBUTE$
    //  Name: LeakVolTime
    //  Description:  Time for leak volume count
    //  Units: None
    //  Range: n/a
    LONG LeakVolTime;

    //$COMMON.ATTRIBUTE$
    //  Name: InhVolumeOneBRTime
    //  Description:  Plus Volume in Exhalathion
    //  Units: None
    //  Range: n/a
    LONG InhVolumeOneBRTime;

    //$COMMON.ATTRIBUTE$
    //  Name: ExhVolumeOneBRTime
    //  Description:  Minus Volume in Exhalathion
    //  Units: None
    //  Range: n/a
    LONG ExhVolumeOneBRTime;

    //$COMMON.ATTRIBUTE$
    //  Name: S_Instance
    //  Description:  Pointer to the single instance of this class.
    //  Units: None
    //  Range: n/a
    static MgrBreathData* S_Instance;

    //$COMMON.ATTRIBUTE$
    //    Name: BreathData1
    //    Description: Object used to store 1 breath's worth of monitored
    //          data.  Used in conjunction with BreathData2 to double
    //          buffer a breath's worth of data so that Patient Data can
    //          be calculating previous breath values on one BreathData
    //          object while the Breath Delivery Task is storing data in
    //          the other
    //    Units:  none
    //    Range:  n/a
    static BreathData BreathData1;

    //$COMMON.ATTRIBUTE$
    //    Name: BreathData2
    //    Description: Object used to store 1 breath's worth of monitored
    //          data.  Used in conjunction with BreathData1 to double
    //          buffer a breath's worth of data so that Patient Data can
    //          be calculating previous breath values on one BreathData
    //          object while the Breath Delivery Task is storing data in
    //          the other
    //    Units:  none
    //    Range:  n/a
    static BreathData BreathData2;

    //$COMMON.ATTRIBUTE$
    //    Name: BdBreathDataPtr
    //    Description: Pointer to the BreathData object that the Breath
    //          Delivery Task is using to store current breath data.
    //    Units:  none
    //    Range:  &BreathData1, &BreathData2
    BreathData* BdBreathDataPtr;

    //$COMMON.ATTRIBUTE$
    //    Name: PdBreathDataPtr
    //    Description: Pointer to the BreathData object that the Patient
    //          Data Task is using to calculate monitored values.
    //
    //    Units:  none
    //    Range:  &BreathData1, &BreathData2
    BreathData* PdBreathDataPtr;

    //$COMMON.ATTRIBUTE$
    //    Name: GuiReady
    //    Description: This attibute indicates if the GUI Task is ready
    //          to receive period pressure/phase data from the BD Task.
    //
    //    Units:  none
    //    Range:  eTrue, False
    E_Bool GuiReady;

    //$COMMON.ATTRIBUTE$
    //    Name: IsFirstBreath
    //    Description: This attibute indicates whether the current breath
    //          is the first since initialization or since exiting from
    //          an non-ventilation mode.  During the first breath,
    //          monitored data values are not calculated because
    //          there is no data for the previous breath to report.
    //
    //    Units:  none
    //    Range:  eTrue, False
    E_Bool IsFirstBreath;

    //$COMMON.ATTRIBUTE$
    //    Name: MonitoredPressure
    //    Description: This attibute contains the most current pressure
    //          read by the BD Task from the ActivePressureSensor object.
    //          The active pressure sensor is the exhalation pressure
    //          sensor unless an auto-zero of the exhalation pressure
    //          sensor is active.  This value is stored here so the BD Task
    //          uses the same value throughout the entire cycle.  This
    //          value is used for trigger and alarm detection in addition
    //          to patient data.
    //
    //    Units:  cmH2O * SCALE
    //    Range:  -100 - 200 cmH2O
    SHORT MonitoredPressure;

    //$COMMON.ATTRIBUTE$
    //    Name: DeliveredFlow
    //    Description: This attibute contains the most current BTPS
    //          compensated delivered flow.  This value is stored here so
    //          that the BD Task uses the same value throughout the entire
    //          cycle.  This value is used for trigger and alarm detection
    //          in addition to patient data.
    //
    //    Units:  ml/sec * SCALE
    //    Range:  0 - 3333.33 ml/sec (200 LPM), scaled
    LONG DeliveredFlow;

    //$COMMON.ATTRIBUTE$
    //    Name: ExhaledFlow
    //    Description: This attibute contains the most current BTPS
    //          compensated exhaled Flow.     This value is stored here so
    //          that the BD Task uses the same value throughout the entire
    //          cycle.  This value is used for trigger and alarm detection
    //          in addition to patient data.
    //
    //    Units:  ml/sec * SCALE
    //    Range:  0 - 3333.33 ml/sec (200 LPM), scaled
    LONG ExhaledFlow;

    //$COMMON.ATTRIBUTE$
    //    Name: PeakDeliveredFlow
    //    Description: Measured Peak Delivered Flow
    //    Units:  ml/sec * SCALE
    //    Range:  0 - 3333.33 ml/sec (200 LPM), scaled
    LONG PeakDeliveredFlow;

    //$COMMON.ATTRIBUTE$
    //    Name: GuiData
    //    Description:
    //      Data sent to the GUI on a periodic basis.  This data includes
    //      the breath phase indicator, and pressure sample.
    //    Units:  None
    //    Range:  n/a
    //        MonitoredPressureGUIEvent GuiData;

    //$COMMON.ATTRIBUTE$
    //    Name: CycleCount
    //    Description:
    //      Counter used to determine if data is sent to the GUI or not.
    //    Units:  None
    //    Range:  n/a
    SHORT CycleCount;

    //$COMMON.ATTRIBUTE$
    //    Name: PeakProxyFlow
    //    Description: Measured Peak Proxy Flow
    //    Units:  ml/sec * SCALE
    //    Range:  0 - 3333.33 ml/sec (200 LPM), scaled
    LONG PeakProxyFlow;

    LONG ProxyFlow;

    //$COMMON.ATTRIBUTE$
    //    Name: TidalVolumeEndInh
    //    Description:
    //
    //    Units:  None
    //    Range:  n/a
    LONG TidalVolumeEndInh;

    //$COMMON.ATTRIBUTE$
    //    Name: BreathPhase
    //    Description:
    //
    //    Units:  None
    //    Range:  n/a
    E_BreathPhaseIndicator BreathPhase;

    //$COMMON.ATTRIBUTE$
    //    Name: LastMonitoredPressure
    //    Units:  cmH2O * SCALE
    //    Range:  -100 - 200 cmH2O
    LONG LastMonitoredPressure;

    //$COMMON.ATTRIBUTE$
    //    Name: DelayColorCount
    //    Description:
    //      Counter used to delay display color.
    //    Units:  None
    //    Range:  n/a
    SHORT DelayColorCount;

    //$COMMON.ATTRIBUTE$
    //    Name: PressureFilterCount
    //    Description:
    //      Counter used to avoid noise.
    //    Units:  None
    //    Range:  n/a
    SHORT PressureFilterCount;

    //$COMMON.ATTRIBUTE$
    //    Name: LeakVolStartFlg
    //    Description:
    //      start Flag for Leak Volume calduration
    //    Units:  None
    //    Range:  n/a
    E_Bool LeakVolStartFlg;

    //$COMMON.ATTRIBUTE$
    //    Name: LastNetFlowExh
    //    Description:
    //      Check for mandatory exhalation flow
    //    Units:  None
    //    Range:  n/a
    LONG LastNetFlowExh;

    //$COMMON.ATTRIBUTE$
    //    Name: LastNetFlowExh
    //    Description:
    //      Check for mandatory exhalation flow
    //    Units:  None
    //    Range:  n/a
    LONG CurrentNetFlowExh;

    //$COMMON.ATTRIBUTE$
    //    Name: LeakVolTimeDummy
    //    Description:
    //      Time for leak volume count
    //    Units:  None
    //    Range:  n/a
    LONG LeakVolTimeDummy;

    //$COMMON.ATTRIBUTE$
    //    Name: InhNetFlowSum
    //    Description:
    //      sum of Inhalation net flow
    //    Units:  None
    //    Range:  n/a
    LONG InhNetFlowSum;

    //$COMMON.ATTRIBUTE$
    //    Name: LONG ExhNetFlowSum;
    //    Description:
    //      sum of exhalation net flow
    //    Units:  None
    //    Range:  n/a
    LONG ExhNetFlowSum;

    //$COMMON.ATTRIBUTE$
    //    Name: PosiLastPeriodNetFlowAry
    //    Description:
    //      position of array for last 100ms
    //    Units:  None
    //    Range:  n/a
    int PosiLastPeriodNetFlowAry;

    //$COMMON.ATTRIBUTE$
    //    Name: LastPeriodNetFlowAry[50];
    //    Description:
    //      array for stored the 100ms flow data
    //    Units:  None
    //    Range:  n/a
    LONG LastPeriodNetFlowAry[50];

    //$COMMON.ATTRIBUTE$
    //    Name: LastPeriodNetFlowSum
    //    Description:
    //      sum of flow for last 100ms
    //    Units:  None
    //    Range:  n/a
    LONG LastPeriodNetFlowSum;

    //$COMMON.ATTRIBUTE$
    //    Name: NetFlowSum
    //    Description:
    //
    //    Units:
    //    Range:
    LONG NetFlowSum;

    //$COMMON.ATTRIBUTE$
    //    Name: InhPhaseIndicator
    //    Description:
    //      use to stored inhalation phase sate.
    //    Units:  None
    //    Range:  n/a
    E_BreathPhaseIndicator InhPhaseIndicator;

    //$COMMON.ATTRIBUTE$
    //    Name: HFOBaseFlow
    //    Description:
    //
    //    Units:  None
    //    Range:  n/a
    double HFOBaseFlow;

    //$COMMON.ATTRIBUTE$
    //    Name: ManInspIsActive
    //    Description: not manual breath
    //
    //    Units:  None
    //    Range:  n/a
    E_Bool ManInspIsActive;

    //$COMMON.ATTRIBUTE$
    //    Name: hfoExhFlow
    //    Description: hfo exhalation flow
    //
    //    Units:  None
    //    Range:  n/a
    LONG hfoExhFlow;

    //$COMMON.ATTRIBUTE$
    //    Name: PresDataBuf
    //    Description:
    //            This buffer will hold the data of the last 1 second
    //            during HFO.
    //    Units:  None
    //    Range:  n/a
    DataBuffer* PressDataBufPtr;

    //$COMMON.ATTRIBUTE$
    //    Name: MinPressData
    //    Description:
    //    Units:cmH2O scaled by SCALE
    //    Range:  n/a
    LONG MinPressData;

    //$COMMON.ATTRIBUTE$
    //    Name: MaxPressData
    //    Description:
    //    Units:cmH2O scaled by SCALE
    //    Range:  n/a
    LONG MaxPressData;

    //$COMMON.ATTRIBUTE$
    //    Name: InhStartEventNotSent
    //    Description: use to indicate inhalation start event is sent or not
    //
    //    Units:  None
    //    Range:  n/a
    E_Bool InhStartEventNotSent;

    //$COMMON.ATTRIBUTE$
    //    Name: NetFlowInExh
    //    Description: new flow in exhalation to calculate HFO tidal volume
    //
    //    Units:  None
    //    Range:  n/a
    LONG NetFlowInExh;

    //$COMMON.ATTRIBUTE$
    //    Name: PressDataCount
    //    Description:
    //      Counter used to determine if data fill the data buffer or not.
    //    Units:  None
    //    Range:  n/a
    SHORT PressDataCount;

    //$COMMON.ATTRIBUTE$
    //    Name: hfoInhFlow
    //    Description:
    //      HFO inhalation flow
    //    Units:  None
    //    Range:  n/a
    LONG hfoInhFlow;

    //$COMMON.ATTRIBUTE$
    //    Name: HFOVolume
    //    Description:
    //      HFO volume
    //    Units:  None
    //    Range:  n/a
    LONG HFOVolume;

    //$COMMON.ATTRIBUTE$
    //    Name: MonitoredFlow
    //    Description: This value is used for Phillips Monitor
    //    Units:  None
    //    Range:  n/a
    LONG MonitoredFlow;

    //$COMMON.ATTRIBUTE$
    //    Name: displayVolInHfo
    //    Description: display volume in hfo
    //    Units:  None
    //    Range:  n/a
    float displayVolInHfo;

    //$COMMON.ATTRIBUTE$
    //    Name: p_HighDelay
    //    Description: delay in Phigh
    //    Units:  None
    //    Range:  n/a
    int p_HighDelay;

    //$COMMON.ATTRIBUTE$
    //    Name: p_LowDelay
    //    Description: delay in PLow
    //    Units:  None
    //    Range:  n/a
    int p_LowDelay;
};

#endif //MGRBREATHDATA_H
