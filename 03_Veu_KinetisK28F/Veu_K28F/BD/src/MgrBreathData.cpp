/******************************************************************************/
//$COMMON.CPP$
//   File Name:  MgrBreathData.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: 
//      This file contains the implementation of the Breath Data Manager.
//      MgrBreathData has 3 primary responsibilities.  First, it contains
//      the interface with the flow and pressure sensors.  And, it contains 
//      operations to retrieve the last sensor reading at the start of 
//      each BD cycle.  It stores these values so that the BD task can use 
//      these readings throughout its entire cycle.  
//
//      Secondly, the Breath Data Manager contains 2 BreathData objects.  
//      Each of these objects holds data pertinent to a single breath.  The
//      Breath Delivery Task updates the data in the objects and then the
//      Patient Data Task uses this data to calculate monitored values and
//      to determine the status of Patient Data Alarms.
//      
//      Finally, operations in this class are responsible for sending 
//      pressure and breath phase data to the GUI.
//
//   Interfaces:
//      This class provides a primary interface between the Breath Delivery
//      Task and the GUI and Patient Data Tasks.
//
//      The InhalationStart(), PlateauStart() and ExhalationStart() are invoked
//      from the BD Task thread of control.  The InhalationStart() and 
//      ExhalationStart() operations interface with the Patient Data Task 
//      by signaling the start of inhalation and the start of exhalation 
//      event flags.
//
//      The ProcessBdCycle() operation interfaces with the Devices subsystem
//      to obtain the latest flow and pressure readings.  It uses these values
//      to determine net flow, exhaled volume and pressure, which it 
//      sends to the GUI task.
//
//   Restrictions:
//      There is only 1 instance of this class.
/******************************************************************************/
#include <DeviceInterface.h>
#include "ProxyFlowSensor.h"
#include "MgrBreathData.h" 
#include "MgrBdSetting.h" 
#include <PhaseMgr.h>
#include "DebugTrace.h"
#include "ModeMgr.h"
#include "PdTask.h"

// Define statics
BreathData MgrBreathData::BreathData1;
BreathData MgrBreathData::BreathData2;
MgrBreathData* MgrBreathData::S_Instance = NULL;

LONG MgrBreathData::mAPRVSpontBreathRate = 0;
LONG MgrBreathData::mAPRVTotalBreathRate = 0;

// Send GUI the pressure data every 100ms.  Timing is based upon the
// BD Task cycle time.
static const UNSIGNED BD_CYCLE_TIME_IN_MS = 20;
static const SHORT GUI_CYCLE_COUNT = 100 / BD_CYCLE_TIME_IN_MS;

// Size of the circular queue used to hold pressure data is based upon the
// frequency that pressure is sent.  
static const SHORT PRESS_Q_SIZE = 20;
const LONG REALTIME_DATA_NUM = 5;

static const UNSIGNED HFO_CONTROL_INTERVAL  = 2;
static const UNSIGNED MAN_INSP_FALL_TIME   = 1000/HFO_CONTROL_INTERVAL;
static const SHORT HFO_DATA_CYCLE_COUNT    = 10/HFO_CONTROL_INTERVAL;    // 10ms
static const SHORT HFO_MANINSP_CYCLE_COUNT = 70/HFO_CONTROL_INTERVAL;    // 100ms

#define PRESS_FILTER_LIMIT 3
#define HFO_BASE_FLOW_SAMPLES 1000
#define DELAY_COLOR_LIMIT 200
#define TIME_500_MS 500

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: S_GetInstance()
//
//    Processing:
//      Since only 1 instance of this object should be created, any access
//      to the object comes via this operation.  If any other object wants to
//      access the MgrBreathData object, it calls MgrBreathData::S_GetInstance()
//      which returns a pointer to the MgrBreathData object.
//
//      If the object has not already been created, this method instantiates
//      it and returns a pointer to the object.  If the object has already been
//      instantiated, a pointer to the object is returned.
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      MgrBreathData* - pointer to the object instantiated from this class
//
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//      None
//
//    Requirements:
//
/******************************************************************************/
MgrBreathData* MgrBreathData::S_GetInstance (void)
{
    // If the object has not already been created, create it.
    if(NULL == S_Instance)
    {
        S_Instance = new MgrBreathData ();
        ASSERTION(S_Instance != NULL);
    }

    return (S_Instance);

}   // end S_GetInstance()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: InhalationStart()
//
//    Processing: 
//      This operation is invoked at the start of every inhalation by the
//      active inhalation phase.  This operation updates the Phase attribute
//      and checks if the IsFirstBreath attribute is TRUE; if so, it is set 
//      to FALSE and the Patient Data task is not notified of the start of 
//      inhalation.  If it is FALSE, the Patient Data Task is notified of the 
//      start of inhalation.
//
//      After notifying Patient Data, this operation switches the BD BreathData
//      pointer to point to the other BreathData object and invokes the
//      InhalationStart() method of the now active BreathData object.  Two 
//      BreathData objects are maintained.  Breath Delivery fills one with data,
//      then notifies the Patient Data task that it is available at the 
//      start of inhalation.  The BD Task then switches its pointer and
//      fills the other Breath Data object with breath data.
//
//      Note: the GUI is not notified of the start of the new phase here.
//      However, the ProcessBdCycle() operation is invoked in the same 
//      BD cycle, after this operation is called.  It is at that time that
//      the GUI will be notified of the phase update.
//
//    Input Parameters:
//      inhType -- the type of inhalation (spontaneous, supported, assisted,
//          or mandatory)
//
//    Output Parameters:
//      None
//
//    Return Values:
//      None
//
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//      The BdBreathDataPtr must be pointing at the BreathData1 or the 
//      BreathData2 objects.  If not, error handling is performed.
//
//    Requirements:  
//
/******************************************************************************/
void MgrBreathData::InhalationStart (E_BreathPhaseIndicator inhType)
{
    InhStartEventNotSent = eFalse;
    // Update the Phase
    BreathPhase = inhType;
    // Reset the CycleCount so that the GUI is aware of the start of a new phase
    CycleCount = 0;
    DelayColorCount = DELAY_COLOR_LIMIT;

    // Set Done for the previous breath
    if (eTrue == IsFirstBreath)
    {
        IsFirstBreath = eFalse;
    }
    else
    {
        // Update exhalation Time in the Structure; this must be done before
        // updating the BdBreathDataPtr.
        BdBreathDataPtr->UpdateTimeInExhalation();

        // Notify Patient Data of Start of Inhalation; Patient Data is only
        // notified if there is previous breath data available to do 
        // patient data calculations

        if((PdTask::S_GetInstance())->S_SetEventFlag(START_INH_EVENT_FLAG) == 0)
        {
            InhStartEventNotSent = eTrue;
        }
    }
    // Switch BreathDataPtr
    if (BdBreathDataPtr == &BreathData1)
    {
        BdBreathDataPtr = &BreathData2;
    }
    else if (BdBreathDataPtr == &BreathData2)
    {
        BdBreathDataPtr = &BreathData1;
    }
    else
    {
        ASSERTION(eFalse);
    }


    // Set the PeakDeliveredFlow to the minimum flow reading.  The first
    // reading will be greater than this value and it will be updated; or, it
    // will be equal to this value and it won't matter.
    PeakDeliveredFlow = MIN_FLOW_RATE;
    PeakProxyFlow = MIN_FLOW_RATE;

    // Initialize the Breath Data for the new inhalation
    BdBreathDataPtr->InhalationStart(inhType);


    //    Disable(); //disable timer

}    // end InhalationStart()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ExhalationStart()
//
//    Processing: 
//      This operation is invoked at the start of every exhalation by the
//      active exhalation phase.  This operation updates the breath phase 
//      indicator in the GuiData structure.
//
//      It then checks if the IsFirstBreath attribute is TRUE; if so, it is set 
//      to FALSE and the Patient Data task is not notified of the start of 
//      exhalation.  If it is FALSE, the inhalation time in the BreathData 
//      object is updated by invoking the ExhaltionStart() operation of the 
//      BreathData object that BD is currently storing data in, and the 
//      Patient Data Task is notified of the start of exhalation.
//
//      Note: the GUI is not notified of the start of the new phase here.
//      However, the ProcessBdCycle() operation is invoked in the same 
//      BD cycle, after this operation is called.  It is at that time that
//      the GUI will be notified of the phase update.
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      None
//
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//      None
//
//    Requirements:  
//
/******************************************************************************/
void MgrBreathData::ExhalationStart (void)
{
    // Update the Phase
    BreathPhase = eExhalation;

    // Reset the CycleCount so that the GUI is aware of the start of a new phase
    CycleCount = 0;

    if (eFalse == IsFirstBreath)
    {
        // Update inhalation Time in the BreathData object !!!!
        BdBreathDataPtr->ExhalationStart();
        E_Bool isGetTidalVolFlag = PhaseMgr::S_GetInstance()->GetVCVInhPhaseStartedFlag();
        if(isGetTidalVolFlag == eTrue)
        {
            TidalVolumeEndInh = GetPdBreathDataPtr()->GetInhVolume();
            PhaseMgr::S_GetInstance()->SetVCVInhPhaseStartedFlag(eFalse);
        }

        // Notify Patient Data of Start of Exh
        (PdTask::S_GetInstance())->S_SetEventFlag(START_EXH_EVENT_FLAG);
    }

    Stop();
    Reset(TIME_500_MS);
    Start();

}    // end ExhalationStart()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: PlateauStart()
//
//    Processing: 
//      This operation is invoked at the start of every plateau by the
//      active plateau phase.  This operation updates the breath phase 
//      indicator in the GuiData structure to plateau.  
//
//      Note: the GUI is not notified of the start of the new phase here.
//      However, the ProcessBdCycle() operation is invoked in the same 
//      BD cycle, after this operation is called.  It is at that time that
//      the GUI will be notified of the phase update.
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      None
//
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//      None
//
//    Requirements:  
//
/******************************************************************************/
void MgrBreathData::PlateauStart (E_BreathPhaseIndicator inhType)
{
    BdBreathDataPtr->PlateauStart(inhType);

}   // end PlateauStart()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: NonVentStart()
//
//    Processing: 
//      This operation is invoked at the start of the non-ventilation phase.
//      This operation updates the Phase attribute and notifies the Patient
//      data task of the start of the phase.  This operation also invokes
//      the private Initialize() operation to initialize all attributes as if
//      the vent was just starting up.  Since it is impossible to tell how
//      long the ventilator will remain in the NonVentPhase, it is undesirable
//      to compute Patient Data on the last breath before the ventilator 
//      transitioned to the phase.
//
//      Note: the GUI is not notified of the start of the new phase here.
//      However, the ProcessBdCycle() operation is invoked in the same 
//      BD cycle, after this operation is called.  It is at that time that
//      the GUI will be notified of the phase update.
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      None
//
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//
//    Requirements:  
//
/******************************************************************************/
void MgrBreathData::NonVentStart (void)
{
    ResetVol();
    // Update the Phase
    BreathPhase = eNoVentilation;

    // Reset the CycleCount so that the GUI is aware of the start of a new phase
    CycleCount = 0;

    // Initialize private attributes.
    Initialize();

    //  Notify the Patient Data task that the ventilator is in a Non-Vent mode
    Stop();
    (PdTask::S_GetInstance())->S_SetEventFlag(NONBREATHING_MODE_EVENT_FLAG);

    //clear hfo tidalVolume
    //    PatientDataGUIEvent hfoTidalVolume;
    //    hfoTidalVolume.Value = BLANKED_LONG_VALUE;
    //    MainHandlerDelegate::S_GetInstance()->
    //            postCustomEvent<PatientDataGUIEvent>(eHFOTidalVolumePtDataID, hfoTidalVolume);


}    // end NonVentStart()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessBdCycle()
//
//    Processing: 
//      This operation is invoked every BD Cycle from the BD Task main loop.
//      This operation interfaces with the ActivePressureSensor, the
//      AirFlowSensor and the O2FlowSensor via the GetLastReading()
//      operation to retrieve the latest exhalation pressure and flow readings.
//      It also interfaces with ExhFlowSensor via GetCurrentReading() to
//      sample and retrieved exhaled flow readings.  This operation then 
//      interfaces with the PhaseMgr to determine the currently active breath 
//      phase.  
//      
//      If the ventilator is in an inhalation or plateau phase, the exhaled
//      volume is set to 0 and delivered flow is summed.  If the delivered
//      flow exceeds the PeakDeliveredFlow, PeakDeliveredFlow is updated with
//      the current flow sum.  The ProcessBdCycleInInh() operation of the
//      currently active BD BreathData object is invoked with the exhalation
//      pressure and net flow values.
//
//      If the ventilator is in an exhalation phase, the ProcessBdCycleInExh() 
//      operation of the currently active BD BreathData object is called with 
//      the exhalation pressure pressure and net flow values.
//
//      Finally, this operation sends pressure readings to the GUI.
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      None
//
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//      None
//
//    Requirements:  
//
/******************************************************************************/
void MgrBreathData::ProcessBdCycle (void)
{
    DEBUG_MGR_BREATH("MgrBreathData ProcessBdCycle\n");
    E_VentilationMode currentModeId = ModeMgr::S_GetInstance()->GetCurrentModeId();

    if((currentModeId >= eFirstHFOMode) &&
            (currentModeId <= eLastHFOMode))
    {
        ProcessHFOVentilationData();
    }
    else if((currentModeId >= eFirstSIMVMode) &&
            (currentModeId <= eLastSIMVMode))
    {
        ProcessIMVVentilationData();
    }
    else if((currentModeId >= eFirstCommonMode) &&
            (currentModeId <= eLastCommonMode))
    {
        ProcessAlarmMode();
    }
    //GUI
    //    RealtimeDataPoint PointData;
    //    PointData.MonitoredPressure = MonitoredPressure;
    //    PointData.MonitoredFlow = MonitoredFlow;
    //    PointData.BreathPhase = BreathPhase;
    //    PointData.ModeIndicator = GuiData.ModeIndicator;
    //
    //    E_Monitor currentMonitor = DataIOTask::S_GetInstance()->getActiveMonitor();
    //
    //    if(currentMonitor==eMetranMonitor)
    //    {
    //        SRMetranProtocol::S_GetInstance()->SaveRealtimeData(PointData);
    //    }
    //    else if(currentMonitor==ePhillipMonitor)
    //    {
    //        SRPhilipsProtocol::S_GetInstance()->SaveRealtimeData(PointData);
    //    }
    //
    //    if(InhStartEventNotSent == eTrue)
    //    {
    //        if((PdTask::S_GetInstance())->S_SetEventFlag(START_INH_EVENT_FLAG) != NULL)
    //        {
    //            InhStartEventNotSent = eFalse;
    //        }
    //    }

}   // end ProcessBdCycle()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: SetPdDone()
//
//    Processing: 
//      This operation is invoked from the Patient Data Task thread of control
//      once that task has finished calculating the monitored data from the
//      data in the Breath Data object.  
//
//      This operation invokes the BreathData object's SetPdDone() operation
//      to set the PdDone flag in the object.  This operation then switches
//      the PdBreathDataPtr to point to the other BreathData object.
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      None
//
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//      The PdBreathDataPtr must be pointing at the BreathData1 or the 
//      BreathData2 objects.  If not, error handling is performed.
//
//    Requirements:  
//
/******************************************************************************/
void MgrBreathData::SetPdDone (void)
{

    // Patient Data is done with that breath now
    PdBreathDataPtr->SetPdDone (eTrue);

    // Switch BreathDataPtr
    if(PdBreathDataPtr == &BreathData1)
    {
        PdBreathDataPtr = &BreathData2;
    }
    else if(PdBreathDataPtr == &BreathData2)
    {
        PdBreathDataPtr = &BreathData1;
    }
    else
    {
        ASSERTION(eFalse);
    }

}   // end SetPdDone()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: MgrBreathData()
//
//    Processing: 
//      This operation is the constructor for the Breath Data Manager object.
//      It initializes the Breath Data Pointers for BD and Patient Data.
//      It also initializes its other attributes.
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      None
//
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//      None
//
//    Requirements:  
//
/******************************************************************************/
MgrBreathData::MgrBreathData (void) : Timer((CHAR*)"MgrBreathData", TIME_500_MS, pdFALSE)
{
    InhNetFlowSum = 0;
     ExhNetFlowSum = 0;
     InhVolumeOneBRTime = 0;
     ExhVolumeOneBRTime = 0;
     LastNetFlowExh = 0;
     CurrentNetFlowExh = 0;
     LeakVolStartFlg = eFalse;
     LeakVolTime = 0;
     LastPeriodNetFlowSum = 0;
     PosiLastPeriodNetFlowAry = 0;
     LeakVolTimeDummy = 0;
     CycleCount = 0;
     PressDataCount = 0;
     DelayColorCount = 0;
     InhPhaseIndicator = eNoPhase;
     LastMonitoredPressure = 0;
     PressureFilterCount = 0;
     HFOBaseFlow = 0.0;
//     GuiData.ModeIndicator = eFalse;

//     for(int i = 0; i < REALTIME_DATA_NUM; i++)
//     {
//         GuiData.IMVData.MonitoredPressure[i] = 0;
//         GuiData.IMVData.TheoryPressure[i] = 0;
//         GuiData.IMVData.MonitoredFlow[i] = 0;
//         GuiData.IMVData.MonitoredVolume[i] = 0;
//         GuiData.IMVData.BreathPhase[i] =  eNoPhase;
//     }


     #ifdef HFO_SYSTEM
         for(int i = 0; i < REALTIME_DATA_NUM; i++)
         {
             GuiData.HFOData.MonitoredPressure[i] = 0;
             GuiData.HFOData.MonitoredMAP[i] = 0;
             GuiData.HFOData.MonitoredFlow[i] = 0;
             GuiData.HFOData.MonitoredVolume[i] = 0;
             GuiData.HFOData.MonitoredHFOCycle[i] = 0;
         }
      #endif


     // The GUI isn't ready until the GUI Task tells the BD Task that it is
     // ready.  At start-up, it is undesirable to bombard the GUI Task with
     // data that it cannot deal with.  Once the GUI Task completes its
     // initialization, it'll notify the BD Task.
     mAPRVSpontBreathRate = 0;
     mAPRVTotalBreathRate = 0;
     hfoInhFlow = 0;
     hfoExhFlow = 0;
     GuiReady = eFalse;
     NetFlowSum = 0;
     NetFlowInExh = 0;
     HFOVolume = 0;
     MonitoredPressure = -100;
     DeliveredFlow = 0;
     ProxyFlow = 0;
     ExhaledFlow = 0;
     MonitoredFlow = 10;

     BreathPhase = eNoPhase;

     // Prepare message to send to the GUI task. Do it here to save some time.
     //    GuiData.ID = eMonitoredRealTimeData;

     ManInspIsActive = eFalse;
     displayVolInHfo = 0.0;

     p_HighDelay = 0;
     p_LowDelay = 0;
     TidalVolumeEndInh = 0;
     InhStartEventNotSent = eFalse;

//     PressDataBufPtr = new DataBuffer(500);

     Initialize();

}   // end MgrBreathData()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GetBdBreathDataPtr (void)
//
//    Processing: Get BdBreathData Pointer
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      BdBreathDataPtr - BdBreathData Pointer
//
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//      None
//
//    Requirements:
//
/******************************************************************************/
BreathData* MgrBreathData::GetBdBreathDataPtr (void)
{
    return BdBreathDataPtr;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GetMonitoredPressure (void)
//
//    Processing: Get monitor pressure
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      MonitoredPressure - monitor pressure
//
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//      None
//
//    Requirements:
//
/******************************************************************************/
SHORT MgrBreathData::GetMonitoredPressure (void)
{
    return MonitoredPressure;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GetExhaledFlow (void)
//
//    Processing: Get Exhalation flow
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      ExhaledFlow - Exhalation flow
//
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//      None
//
//    Requirements:
//
/******************************************************************************/
LONG MgrBreathData::GetExhaledFlow (void)
{
    return ExhaledFlow;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GetDeliveredFlow (void)
//
//    Processing: Get deleverd flow
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      DeliveredFlow - deleverd flow
//
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//      None
//
//    Requirements:
//
/******************************************************************************/
LONG MgrBreathData::GetDeliveredFlow (void)
{
    return DeliveredFlow;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GetPeakDeliveredFlow (void)
//
//    Processing: Get Peak deliverd flow
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      PeakDeliveredFlow - Peak deliverd flow
//
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//      None
//
//    Requirements:
//
/******************************************************************************/
LONG MgrBreathData::GetPeakDeliveredFlow (void)
{
    return PeakDeliveredFlow;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GetPdBreathDataPtr (void)
//
//    Processing: This method is invoked by the Patient Data Task
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      PdBreathDataPtr - Pd Breath Data Pointer
//
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//      None
//
//    Requirements:
//
/******************************************************************************/
BreathData* MgrBreathData::GetPdBreathDataPtr (void)
{
    return PdBreathDataPtr;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GetProxyFlow (void)
//
//    Processing: Get Proxy flow
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      ProxyFlow - Proxy flow value
//
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//      None
//
//    Requirements:
//
/******************************************************************************/
LONG MgrBreathData::GetProxyFlow (void)
{
    return ProxyFlow;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GetLeakVolTime (void)
//
//    Processing: Get leak volume time
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      LeakVolTime - leak volume time
//
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//      None
//
//    Requirements:
//
/******************************************************************************/
LONG MgrBreathData::GetLeakVolTime (void)
{
    return LeakVolTime;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GetInhVolOneBRTime (void)
//
//    Processing: GEt inhalation volume on one BR times
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      InhVolumeOneBRTime - inhalation volume on one BR times
//
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//      None
//
//    Requirements:
//
/******************************************************************************/
LONG MgrBreathData::GetInhVolOneBRTime (void)
{
    return InhVolumeOneBRTime;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GetExhVolOneBRTime (void)
//
//    Processing: GEt exhalation volume on one BR times
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      ExhVolumeOneBRTime - exhalation volume on one BR times
//
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//      None
//
//    Requirements:
//
/******************************************************************************/
LONG MgrBreathData::GetExhVolOneBRTime (void)
{
    return ExhVolumeOneBRTime;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GetPeakProxyFlow (void)
//
//    Processing: Get peak proxy flow
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      PeakProxyFlow - peak proxy flow
//
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//      None
//
//    Requirements:
//
/******************************************************************************/
LONG MgrBreathData::GetPeakProxyFlow (void)
{
    return PeakProxyFlow;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GetTidalVolumeEndInh (void)
//
//    Processing: get tidal volume end inhalation
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      TidalVolumeEndInh - tidal volume end inhalation
//
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//      None
//
//    Requirements:
//
/******************************************************************************/
LONG MgrBreathData::GetTidalVolumeEndInh (void)
{
    return TidalVolumeEndInh;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Initialize()
//
//    Processing: 
//      This operation is invoked from the constructor and from the NonVent
//      mode when ventilation is about to resume.  It sets up the
//      BreathData Pointers for BD and Patient Data.  It also initializes 
//      its other attributes.
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      None
//
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//      None
//
//    Requirements:  
//
/******************************************************************************/
void MgrBreathData::Initialize (void)
{
    // Set up the BD BreathData pointer to point to one of the BreathData
    // objects.  Set up the Patient Data BreathData pointer to point to the
    // other one.   Typically, the BD Task will be putting data in one of
    // the BreathData objects while the Patient Data Task is using data in
    // the other object to calculated monitored values for the previous breath.
    BdBreathDataPtr = &BreathData1;
    PdBreathDataPtr = &BreathData2;

    // Set the PdDone flags in BreathData1 and BreathData2 to indicate that
    // the Patient Data task is done with those objects.
    BdBreathDataPtr->SetPdDone (eTrue);
    PdBreathDataPtr->SetPdDone (eTrue);

    // Set the IsFirstBreath attribute to TRUE.  On the first breath, there
    // is no valid data for the "previous breath" for the Patient Data task
    // to do its calculations on.  This attribute is used by operations in this
    // class to determine whether or not the Patient Data Task is notified
    // of the start of a breath phase.
    IsFirstBreath = eTrue;

    // Set the PeakDeliveredFlow to the minimum flow value that can be read.
    // The first reading will be greater than this value and it will be updated; 
    // or, it will be equal to this value and it won't matter.
    PeakDeliveredFlow = MIN_FLOW_RATE;

    mAPRVSpontBreathRate = 0;
    mAPRVTotalBreathRate = 0;

}   // end Initialize()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: APRV_InhPhaseStart(E_BreathPhaseIndicator inhType)
//
//    Processing:
//
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      LONG PIP
//
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//      None
//
//    Requirements:
//
/******************************************************************************/
void MgrBreathData::APRV_InhPhaseStart(E_BreathPhaseIndicator inhType)
{
    BreathPhase = inhType;
    mAPRVSpontBreathRate++;
    mAPRVTotalBreathRate++;
    BdBreathDataPtr->APRVTriggerInhStart(inhType);
    Stop();
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: APRV_PHighPhaseStart(E_BreathPhaseIndicator inhType)
//
//    Processing:
//
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      LONG PIP
//
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//      None
//
//    Requirements:
//
/******************************************************************************/
void MgrBreathData::APRV_PHighPhaseStart(E_BreathPhaseIndicator inhType)
{
    mAPRVTotalBreathRate++;

    // Update the Phase
    BreathPhase = inhType;

    // Reset the CycleCount so that the GUI is aware of the start of a new phase
    CycleCount = 0;

    // Set Done for the previous breath
    if (eTrue == IsFirstBreath)
    {
    }
    else
    {
        // Notify Patient Data of Start of Inhalation; Patient Data is only
        // notified if there is previous breath data available to do
        // patient data calculations

        (PdTask::S_GetInstance())->S_SetEventFlag(APRV_PHIGH_START);
    }
    BdBreathDataPtr->UpdateTimeIn_APRVPLowPhase();
    // Switch BreathDataPtr
    if (BdBreathDataPtr == &BreathData1)
    {
        BdBreathDataPtr = &BreathData2;
        if(eTrue == IsFirstBreath)
        {
            BdBreathDataPtr->SetTimeExhInFirstBreath(BreathData1.GetTimeInExh());
            IsFirstBreath = eFalse;
        }
    }
    else if (BdBreathDataPtr == &BreathData2)
    {
        BdBreathDataPtr = &BreathData1;
        if(eTrue == IsFirstBreath)
        {
            BdBreathDataPtr->SetTimeExhInFirstBreath(BreathData2.GetTimeInExh());
            IsFirstBreath = eFalse;
        }
    }
    else
    {
        ASSERTION(eFalse);
    }

    // Set the PeakDeliveredFlow to the minimum flow reading.  The first
    // reading will be greater than this value and it will be updated; or, it
    // will be equal to this value and it won't matter.
    PeakDeliveredFlow = MIN_FLOW_RATE;
    PeakProxyFlow = MIN_FLOW_RATE;

    // Initialize the Breath Data for the new inhalation
    BdBreathDataPtr->InhalationStart(inhType);

    Stop();
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name:APRV_ExhPhaseStart_InPHigh(E_BreathPhaseIndicator inhType)
//
//    Processing:
//
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      LONG PIP
//
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//      None
//
//    Requirements:
//
/******************************************************************************/
void MgrBreathData::APRV_ExhPhaseStart_InPHigh(E_BreathPhaseIndicator inhType)
{
    (PdTask::S_GetInstance())->S_SetEventFlag(END_INH_PS_APRV);

    BreathPhase = inhType;

    //Reset Vte In APRV Exh phase
    BdBreathDataPtr->ResetVte();
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: APRV_ExhPhaseStart_InPLow(E_BreathPhaseIndicator inhType)
//
//    Processing:
//
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      LONG PIP
//
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//      None
//
//    Requirements:
//
/******************************************************************************/
void MgrBreathData::APRV_ExhPhaseStart_InPLow(E_BreathPhaseIndicator inhType)
{
    (PdTask::S_GetInstance())->S_SetEventFlag(END_INH_PS_APRV);
    BreathPhase = inhType;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: APRV_PLowPhaseStart()
//
//    Processing:
//
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      LONG PIP
//
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//      None
//
//    Requirements:
//
/******************************************************************************/
void MgrBreathData::APRV_PLowPhaseStart(void )
{
    BdBreathDataPtr->ResetVte();
    // Update the Phase
    //    BreathPhase = eExhalation;

    // Reset the CycleCount so that the GUI is aware of the start of a new phase
    CycleCount = 0;

    if (eFalse == IsFirstBreath)
    {
        // Update inhalation Time in the BreathData object !!!!
        //        BdBreathDataPtr->UpdateTimeIn_APRVPHighPhase();

        (PdTask::S_GetInstance())->S_SetEventFlag(APRV_PLOW_START);
    }

}
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessNCPAP()
//
//    Processing:
//
//    Input Parameters:
//      LONG value
//
//    Output Parameters:
//      None
//
//    Return Values:
//      None
//
//    Pre-Conditions:
//      The input command must be Start.
//
//    Requirements:
//
/******************************************************************************/
void MgrBreathData::ProcessNCPAP(void)
{
    PdTask::S_GetInstance()->S_SetEventFlag(UPDATE_NCPAP_DATA_FLAG);
}

//*******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ClearCompensateBuffer()
//
//    Processing:
//     Clear compensate buffer after calibrating proxy sensor
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      None
//
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//      None
//
//    Requirements:
//
/******************************************************************************/
void MgrBreathData::ClearCompensateBuffer()
{
    ResetVol();
    (PdTask::S_GetInstance())->S_SetEventFlag(CLEAR_COMPENSATE_DATA);
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessIMVVentialationData()
//
//    Processing:
//      This operation is invoked every BD Cycle from the BD Task main loop.
//      This operation interfaces with the ActivePressureSensor, the
//      AirFlowSensor and the O2FlowSensor via the GetLastReading()
//      operation to retrieve the latest exhalation pressure and flow readings.
//      It also interfaces with ExhFlowSensor via GetCurrentReading() to
//      sample and retrieved exhaled flow readings.  This operation then
//      interfaces with the PhaseMgr to determine the currently active breath
//      phase.
//
//      If the ventilator is in an inhalation or plateau phase, the exhaled
//      volume is set to 0 and delivered flow is summed.  If the delivered
//      flow exceeds the PeakDeliveredFlow, PeakDeliveredFlow is updated with
//      the current flow sum.  The ProcessBdCycleInInh() operation of the
//      currently active BD BreathData object is invoked with the exhalation
//      pressure and net flow values.
//
//      If the ventilator is in an exhalation phase, the ProcessBdCycleInExh()
//      operation of the currently active BD BreathData object is called with
//      the exhalation pressure pressure and net flow values.
//
//      Finally, this operation sends pressure readings to the GUI.
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      None
//
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//      None
//
//    Requirements:
//
/******************************************************************************/
void MgrBreathData::ProcessIMVVentilationData(void)
{
    static int delay100ms = 0;
    static int delay400ms = 0;
    LONG monitoredVolume = 0;         // exhalation volume.
    LONG netFlow = 0;
    // net flow is the difference between delivered and
    // exhaled flow
    E_PhaseId phaseId;      // phase ID

    // Get the latest Pressure and Delivered Air and O2 Flow Readings
    // Pressure is usually measured in the exhalation limb, but during an
    // auto-zero, the inhalation pressure sensor is used.
    //    MonitoredPressure = ActivePressureSensor->GetLastReading();

    //    DeliveredFlow = ((O2FlowSensor->GetLastReading() * Btps::S_GetBtpsO2Factor()) +
    //                 (AirFlowSensor->GetLastReading() * Btps::S_GetBtpsAirFactor())) / SCALE;

    ProxyStatus Status = ProxyFlowSensor::S_GetInstance()->getLastStatus();

    if((Status == eCalibrated) || (Status == eResetAndRecovered))
    {
        ProxyFlow = ProxyFlowSensor::S_GetInstance()->GetLastReading();
    }
    else
    {
        ProxyFlow = 0;
    }

#ifdef TESTING
    DeliveredFlow =
            ((O2FlowSensor->GetLastReading() * Btps::S_GetBtpsO2Factor()) +
                    (AirFlowSensor->GetLastReading() * Btps::S_GetBtpsAirFactor())) / SCALE;
#else
    if((Status == eCalibrated)||(Status == eResetAndRecovered))
    {
        //        netFlow = proxySensor->GetLastReading()*Btps::S_GetBtpsExhFactor() / SCALE;
    }
    else
    {
        netFlow = 0;
    }
#endif

    // Processing depends upon the current breath phase
    phaseId = (PhaseMgr::S_GetInstance())->GetCurrentPhaseId();

    switch (phaseId)
    {
        case eSpontInhPhase_InAPRV_PHigh:
        case eSpontInhPhase_InAPRV_PLow:
        case eMandInhPhase:
        case eSpontInhPhase:
        case eMandInhTestBreathPhase:
        case eMandInhVABreathPhase:
        case ePlateauPhase:
            // If the current flow reading exceeds the current peak,
            // update the peak.  PeakDeliveredFlow is used to determine
            // if the exhalation flow criteria have been met.
            if (DeliveredFlow > PeakDeliveredFlow)
            {
                PeakDeliveredFlow = DeliveredFlow;
            }

            if (ProxyFlow > PeakProxyFlow)
            {
                PeakProxyFlow = ProxyFlow;
            }

            BdBreathDataPtr->ProcessBdCycleInInh(MonitoredPressure, netFlow);

            monitoredVolume = BdBreathDataPtr->GetMoniInhVolume() - BdBreathDataPtr->GetMoniExhVolume();
            delay100ms = 0;
            delay400ms = 0;
            LastMonitoredPressure = MonitoredPressure;
            break;
        case eExhPhase:

            if (DelayColorCount > 0)
            {
                if (MonitoredPressure >= LastMonitoredPressure)
                {
                    DelayColorCount--;
                    LastMonitoredPressure = MonitoredPressure;
                    PressureFilterCount = 0;
                }
                else
                {
                    PressureFilterCount++;
                    if (PressureFilterCount > PRESS_FILTER_LIMIT)
                    {
                        DelayColorCount = 0;
                    }

                }
            }

            BdBreathDataPtr->ProcessBdCycleInExh(MonitoredPressure, -netFlow);
            monitoredVolume = BdBreathDataPtr->GetMoniInhVolume() - BdBreathDataPtr->GetMoniExhVolume();

            //compare the data for mandatory exh flow check
            LastNetFlowExh = CurrentNetFlowExh;
            CurrentNetFlowExh = netFlow;

            //wait for Transient state
            if(delay100ms < 100)
            {
                delay100ms += BD_CYCLE_TIME_IN_MS;
            }
            else
            {
                //check mandatory exh flow
                if((LastNetFlowExh < 0) && (CurrentNetFlowExh >= 0))
                {
                    if(LeakVolStartFlg != eTrue)
                    {
                        LeakVolStartFlg = eTrue;
                    }
                }
                //Flow data gather start for Plus and Minus flow in exhalation
                if(LeakVolStartFlg == eTrue)
                {
                    //delay 400ms for wait servo control
                    if(delay400ms < 400)
                    {
                        delay400ms += BD_CYCLE_TIME_IN_MS;
                    }
                    else
                    {
                        //time count for leak volume count
                        LeakVolTimeDummy += BD_CYCLE_TIME_IN_MS;
                        LeakVolTime = LeakVolTimeDummy;

                        if(netFlow >= 0)
                        {
                            InhNetFlowSum += netFlow;
                        }
                        else
                        {
                            ExhNetFlowSum += netFlow;
                        }
                        InhVolumeOneBRTime = InhNetFlowSum*2.0/60.0;    //Plus side volume
                        ExhVolumeOneBRTime = ExhNetFlowSum*2.0/60.0;    //Minus side volume

                        LastPeriodNetFlowAry[PosiLastPeriodNetFlowAry] = netFlow;
                        PosiLastPeriodNetFlowAry++;

                        //return to the initial position
                        if(PosiLastPeriodNetFlowAry >= 50)
                        {
                            PosiLastPeriodNetFlowAry = 0;
                        }

                        //sum of net flow for 100ms
                        for(int i = 0; i < 50; i++)
                        {
                            LastPeriodNetFlowSum += LastPeriodNetFlowAry[i];
                        }

                        //plus side volume
                        if(LastPeriodNetFlowSum > 0)
                        {
                            InhVolumeOneBRTime = (InhNetFlowSum - LastPeriodNetFlowSum)*2.0/60.0;
                        }
                        //minus side volume
                        else if(LastPeriodNetFlowSum < 0)
                        {
                            ExhVolumeOneBRTime = (ExhNetFlowSum - LastPeriodNetFlowSum)*2.0/60.0;
                        }

                        LastPeriodNetFlowSum = 0;                   //initialization at every cycle

                        LeakVolTime -= 100;                     //ignor the last 100ms

                        if(LeakVolTime < 0)
                        {
                            LeakVolTime = 0;
                        }
                    }
                }
            }
            UpdatePEEPAlarm();
            break;

        case eAPRVExhPhase_InPHigh:
            if (DeliveredFlow > PeakDeliveredFlow)
            {
                PeakDeliveredFlow = DeliveredFlow;
            }
            if (ProxyFlow > PeakProxyFlow)
            {
                PeakProxyFlow = ProxyFlow;
            }
            ProcessAPRVHighPress();

            //Compute Vte in a Breath
            BdBreathDataPtr->ProcessBdCycleInExh(MonitoredPressure, -netFlow);

            monitoredVolume = BdBreathDataPtr->GetMoniInhVolume() - BdBreathDataPtr->GetMoniExhVolume();

            //Compute Vte In APRV Exh Phase
            BdBreathDataPtr->ComputeVteInPhase(-netFlow);

            //Compute Pressure at the end of PHigh Phase
            BdBreathDataPtr->ProcessBdCycleInInh(MonitoredPressure, 0);

            break;

        case eAPRVExhPhase_InPLow:
            BdBreathDataPtr->ProcessBdCycleInExh(MonitoredPressure, -netFlow);
            monitoredVolume = BdBreathDataPtr->GetMoniInhVolume() - BdBreathDataPtr->GetMoniExhVolume();
            break;
        case eAPRVLowPhase:

            ProcessAPRVLowPress();

            //Compute Vte in a Breath
            BdBreathDataPtr->ProcessBdCycleInExh(MonitoredPressure, -netFlow);

            monitoredVolume = BdBreathDataPtr->GetMoniInhVolume() - BdBreathDataPtr->GetMoniExhVolume();

            //Compute Vte in APRVLowPhase
            BdBreathDataPtr->ComputeVteInPhase(-netFlow);


            break;

        case eAPRVHighPhase:
            if (DeliveredFlow > PeakDeliveredFlow)
            {
                PeakDeliveredFlow = DeliveredFlow;
            }
            if (ProxyFlow > PeakProxyFlow)
            {
                PeakProxyFlow = ProxyFlow;
            }

            BdBreathDataPtr->ProcessBdCycleInInh(MonitoredPressure, netFlow);

            monitoredVolume = BdBreathDataPtr->GetMoniInhVolume();

            ProcessAPRVHighPress();
            break;
        case eSelfTestPhase:
        case eNonVentPhase:
        case eInitPhase:
        case eDiagnosticPhase:
            break;
        case eNCPAPPhase:
            BreathPhase = eNCPAP;
            break;

        default:
            ASSERTION(eFalse);
            break;

    }

    NetFlowSum = (NetFlowSum + netFlow);

    //    GuiData.ModeIndicator = eTrue;        // flag indicate IMV pressure
    static int rtix = 0;
    //    GuiData.IMVData.MonitoredPressure[rtix]= (LONG)MonitoredPressure;
    //    GuiData.IMVData.TheoryPressure[rtix]=(LONG)MonitoredPressure
    //            - (LONG)(CMVServoController->CompCompensatePressure(100.0, false)*100.0);

    if((Status == eCalibrated)||(Status == eResetAndRecovered))
    {
        //        GuiData.IMVData.MonitoredFlow[rtix]= proxySensor->GetLastReading()*Btps::S_GetBtpsExhFactor() / SCALE;
        //        MonitoredFlow = GuiData.IMVData.MonitoredFlow[rtix];
    }
    else
    {
        //        GuiData.IMVData.MonitoredFlow[rtix]= 0;
        //        MonitoredFlow = 10;
    }

    if(IsFirstBreath)
    {
        monitoredVolume = 0;
    }

    //    GuiData.IMVData.MonitoredVolume[rtix]= (LONG)(monitoredVolume);

    if (phaseId != eExhPhase)
    {
        //        GuiData.IMVData.BreathPhase[rtix] = BreathPhase;
        //        InhPhaseIndicator = BreathPhase;
    }
    else
    {
        if (DelayColorCount > 0)
        {
            //             GuiData.IMVData.BreathPhase[rtix] = InhPhaseIndicator;
        }
        else
        {
            //             GuiData.IMVData.BreathPhase[rtix] = eExhalation;
        }
    }

    rtix++;

    if(rtix == REALTIME_DATA_NUM)
    {
        rtix =0;
        // Put message on queue to be picked up by the GUI Task
        //        MainHandlerDelegate::S_GetInstance()->postCustomEvent<MonitoredRealTimeDataGUIEvent>(eMonitoredRealTimeData, GuiData);
    }
}   // end ProcessBdCycle()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessHFOVentilationData()
//
//    Processing:
//
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      None
//
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//      None
//
//    Requirements:
//
/******************************************************************************/
void MgrBreathData::ProcessHFOVentilationData(void)
{
    static LONG delayAfterManInsp = 0;
//    LONG currentProxyData = 0;
//    LONG currentPressure = 0;
//    static E_Bool IsNextCycle = eFalse;

    //    MonitoredPressure = ActivePressureSensor->GetLastReading();

    //    DeliveredFlow =
    //            ((O2FlowSensor->GetLastReading() * Btps::S_GetBtpsO2Factor()) +
    //             (AirFlowSensor->GetLastReading() * Btps::S_GetBtpsAirFactor())) / SCALE;
    CompHfoBaseFlow();

    // Save data every 2 ms.
    //    PressDataBufPtr->InsertNewValue(MonitoredPressure);

    // Only send data to the GUI and analog outputs every GUI_CYCLE_COUNT.

    //    GuiData.ModeIndicator = eFalse;        // flag indicate HFO pressure

    static int rtix = 0;
    //    GuiData.HFOData.MonitoredPressure[rtix] = MonitoredPressure;
    //    GuiData.HFOData.MonitoredMAP[rtix] = HfoController->GetFilterPressIntegrator();

    ProxyStatus Status = ProxyFlowSensor::S_GetInstance()->getLastStatus();
    if((Status == eCalibrated)||(Status == eResetAndRecovered))
    {
        //        currentProxyData = proxySensor->GetLastReading()*Btps::S_GetBtpsExhFactor() / SCALE;
        //        currentPressure = ActivePressureSensor->GetLastReading();

        //        CompHfoVolume(currentProxyData,currentPressure);

        //        GuiData.HFOData.MonitoredFlow[rtix] = currentProxyData;
        //        GuiData.HFOData.MonitoredVolume[rtix] = (LONG)displayVolInHfo;
        //        GuiData.HFOData.MonitoredHFOCycle[rtix] = CURRENT_HFO_CYCLE;
        //        MonitoredFlow = currentProxyData;

        //Detect new HFO cycle
        //        if ( (IsNextCycle == eFalse) && (currentProxyData >= DETECT_INHALATION_FLOW))
        //        {
        //            IsNextCycle = eTrue;
        //            GuiData.HFOData.MonitoredHFOCycle[rtix] = NEXT_HFO_CYCLE;
        //        }
        //        if( (IsNextCycle == eTrue) && (currentProxyData <= DETECT_EXHALATION_FLOW))
        //        {
        //            IsNextCycle = eFalse;
        //        }
    }
    else
    {
        //        GuiData.HFOData.MonitoredFlow[rtix] = 0;
        //        GuiData.HFOData.MonitoredHFOCycle[rtix] = CURRENT_HFO_CYCLE;
        //        GuiData.HFOData.MonitoredVolume[rtix] = 0;
        //        MonitoredFlow = 10;
        //
        //        PatientDataGUIEvent hfoTidalVolume;
        //        hfoTidalVolume.Value=BLANKED_LONG_VALUE;
        //        MainHandlerDelegate::S_GetInstance()->
        //                postCustomEvent<PatientDataGUIEvent>(eHFOTidalVolumePtDataID, hfoTidalVolume);
    }

    rtix++;

    if(rtix == REALTIME_DATA_NUM)
    {
        rtix = 0;
        // Put message on queue to be picked up by the GUI Task
        //        MainHandlerDelegate::S_GetInstance()->postCustomEvent<MonitoredRealTimeDataGUIEvent>(eMonitoredRealTimeData, GuiData);
    }

    // Increment CycleCount to keep track of when to send data to the GUI.
    CycleCount++;

    if(!ManInspIsActive)    // not manual breath ?
    {
        if(delayAfterManInsp <= 0)
        {
            if (CycleCount >= HFO_DATA_CYCLE_COUNT)
            {
                CycleCount = 0;
            }
        }
        else
        {
            delayAfterManInsp--;
            if (CycleCount >= HFO_MANINSP_CYCLE_COUNT)
            {
                CycleCount = 0;
            }
        }
    }
    else
    {
        delayAfterManInsp = MAN_INSP_FALL_TIME;

        if (CycleCount >= HFO_MANINSP_CYCLE_COUNT)
        {
            CycleCount = 0;
        }
    }

    static SHORT PressDataCount = 0;
    PressDataCount++;
    // Update PressDataCount every 500ms to keep track of when to send HFO data
    // to the GUI every one second
    if(PressDataCount >= 500)
    {
        PressDataCount = 0;
        //  Notify the Patient Data task that the ventilator is in a Non-Vent mode
        (PdTask::S_GetInstance())->S_SetEventFlag(HFO_DATA_READY_EVENT_FLAG);
    }
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessAlarmMode()
//
//    Processing:
//
//    Input Parameters:
//      LONG value
//
//    Output Parameters:
//      None
//
//    Return Values:
//      None
//
//    Pre-Conditions:
//      The input command must be Start.
//
//    Requirements:
//
/******************************************************************************/
void MgrBreathData::ProcessAlarmMode(void)
{

}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: UpdatePEEPAlarm()
//
//    Processing: update PEEP Alarm
//
//    Input Parameters:
//      LONG value
//
//    Output Parameters:
//      None
//
//    Return Values:
//      None
//
//    Pre-Conditions:
//
//    Requirements:
//
/******************************************************************************/
void MgrBreathData::UpdatePEEPAlarm(void)
{
//    SHORT currenP = BdBreathDataPtr->GetEndExhPress();
    //Update PEEP Alarm
//    SHORT HiPLimit = MgrBdSetting::S_GetInstance()->GetCurrentValue(eHighBaselineLimit);
//    SHORT LowPLimit = MgrBdSetting::S_GetInstance()->GetCurrentValue(eLowBaselineLimit);
    //    PdAlarms::S_Instance->CheckHiBaseLine2Secs(HiPLimit,currenP,Alarm[ALARM_IDX(eHighPeep)]);
    //    PdAlarms::S_Instance->CheckLowBaseLine2Secs(LowPLimit,currenP,Alarm[ALARM_IDX(eLowPeep)]);
}

//*******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessAPRVHighPress()
//
//    Processing:
//      This operation updates the pressure data and alarm in PHigh phase in APRV mode
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//        MaxpressData
//
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//      None
//
//    Requirements:
//
/******************************************************************************/
void MgrBreathData::ProcessAPRVHighPress()
{
    //    SHORT Pressure = ActivePressureSensor->GetLastReading();
    //    BdBreathDataPtr->ProcessBdCycleAPRV_PHigh(Pressure);

    //    p_LowDelay = 0;


    //    p_HighDelay = p_HighDelay+2;
    //    if(p_HighDelay > 1000) //update every 1 second
    //    {
    //        PatientDataGUIEvent PMsg;

    //        p_HighDelay = 0;
    //        PMsg.Value = BdBreathDataPtr->GetAPRV_PHigh();
    //        MainHandlerDelegate::S_GetInstance()->postCustomEvent<PatientDataGUIEvent>(eAPRVPHighPtDataID, PMsg);
    //    }

    //    UpDatePHigh_Alarm();
}

//*******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessAPRVLowPress()
//
//    Processing:
//      This operation updates the pressure data and alarm in Plow phase in APRV mode
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None

//
//    Return Values:
//        MaxpressData
//
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//      None
//
//    Requirements:
//
/******************************************************************************/
void MgrBreathData::ProcessAPRVLowPress()
{
    //    SHORT Pressure = ActivePressureSensor->GetLastReading();
    //    BdBreathDataPtr->ProcessBdCycleAPRV_PLow(Pressure);
    //    UpDatePLow_Alarm();
    //    p_HighDelay = 0;
    //
    //    p_LowDelay = p_LowDelay+2;
    //
    //    if(p_LowDelay > 500)//update every 500 ms
    //    {
    //        PatientDataGUIEvent PMsg;
    //        p_LowDelay = 0;
    //        PMsg.Value = BdBreathDataPtr->GetAPRV_PLow();
    //        MainHandlerDelegate::S_GetInstance()->postCustomEvent<PatientDataGUIEvent>(eAPRVPLowPtDataID, PMsg);
    //
    //    }

}

//*******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ResetVol()
//
//    Processing:
//      Reset volume
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None

//
//    Return Values:
//        NOne
//
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//      None
//
//    Requirements:
//
/******************************************************************************/
void MgrBreathData::ResetVol (void)
{
    InhNetFlowSum = 0;
    ExhNetFlowSum = 0;
    InhVolumeOneBRTime = 0;
    ExhVolumeOneBRTime = 0;
    LastNetFlowExh = 0;
    CurrentNetFlowExh = 0;
    LeakVolStartFlg = eFalse;
    LeakVolTime = 0;

    LeakVolTimeDummy = 0;
    LastPeriodNetFlowSum = 0;
    PosiLastPeriodNetFlowAry = 0;
    for(int i = 0; i < 50; i++)
    {
        LastPeriodNetFlowAry[i] = 0;
    }
}

//*******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GetHFOBaseFlow()
//
//    Processing:
//     return hfo basee flow
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      HFOBaseFlow - hfo base flow
//
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//      None
//
//    Requirements:
//
/******************************************************************************/
LONG MgrBreathData::GetHFOBaseFlow (void)
{
    return (LONG)HFOBaseFlow;
}

//*******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GetExhFlow()
//
//    Processing:
//     return HFO exhalation flow
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      hfoExhFlow - HFO exhalation flow
//
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//      None
//
//    Requirements:
//
/******************************************************************************/
LONG MgrBreathData::GetExhFlow (void)
{
    return hfoExhFlow;
}

//*******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GetMaxPressData()
//
//    Processing:
//      This operation is invoked from PtData to get max value of pressure.
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//        MaxpressData
//
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//      None
//
//    Requirements:
//
/******************************************************************************/
LONG MgrBreathData::GetMaxPresData(void)
{
    MaxPressData = PressDataBufPtr->GetMaxValue();
    return MaxPressData;
}

//*******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GetMinPressData()
//
//    Processing:
//      This operation is invoked from PtData to get min value of pressure.
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//        MinpressData
//
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//      None
//
//    Requirements:
//
/******************************************************************************/
LONG MgrBreathData::GetMinPresData(void)
{
    MinPressData = PressDataBufPtr->GetMinValue();
    return MinPressData;
}

//*******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GetMaxPressData()
//
//    Processing:
//      This operation is invoked from PtData to get current Amp
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//        Amplitute
//
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//      None
//
//    Requirements:
//
/******************************************************************************/
LONG MgrBreathData::GetCurrentAmp(void)
{
    return (GetMaxPresData() - GetMinPresData());
}

//*******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GetHFOAmplitude()
//
//    Processing:
//      Get hfo amplitude
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//        MaxPressData - MinPressData = hfo amplitude
//
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//      None
//
//    Requirements:
//
/******************************************************************************/
LONG MgrBreathData::GetHFOAmplitude (void)
{
    return (MaxPressData - MinPressData);
}

//*******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GetAPRVTotalBreathRate()
//
//    Processing:
//      Get APRV Total Breath Rate
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//        mAPRVTotalBreathRate - APRV Total Breath Rate
//
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//      None
//
//    Requirements:
//
/******************************************************************************/
LONG MgrBreathData::GetAPRVTotalBreathRate (void)
{
    return mAPRVTotalBreathRate;
}

//*******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GetAPRVSpontBreathRate()
//
//    Processing:
//      Get APRV Spon Total Breath Rate
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//        mAPRVSpontBreathRate - APRV Spon total Breath Rate
//
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//      None
//
//    Requirements:
//
/******************************************************************************/
LONG MgrBreathData::GetAPRVSpontBreathRate (void)
{
    return mAPRVSpontBreathRate;
}

//*******************************************************************************/
//$COMMON.OPERATION$ResetAPRVBR
//    Operation Name: GetAPRVSpontBreathRate()
//
//    Processing:
//      Reset APRV BR
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//        None
//
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//      None
//
//    Requirements:
//
/******************************************************************************/
void MgrBreathData::ResetAPRVBR (void)
{
    mAPRVTotalBreathRate = 0;
    mAPRVSpontBreathRate = 0;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name:HFOStart()
//
//    Processing:
//
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      None
//
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//      None
//
//    Requirements:
//
/******************************************************************************/
void MgrBreathData::HFOStart()
{
    PressDataBufPtr->ClearDataBuffer();
    HFOBaseFlow = 0.0;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation NCPAPStart()
//
//    Processing:
//
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      None
//
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//      None
//
//    Requirements:
//
/******************************************************************************/
void MgrBreathData::NCPAPStart (void)
{
    Stop();
    Reset(TIME_500_MS);
    Start();
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation SetFirstBreath()
//
//    Processing: set first breath
//
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      None
//
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//      None
//
//    Requirements:
//
/******************************************************************************/
void MgrBreathData::SetFirstBreath (void)
{
    IsFirstBreath = eTrue;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation InitializeBreathDataPtr()
//
//    Processing: Initialize breath data pointer
//
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      None
//
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//      None
//
//    Requirements:
//
/******************************************************************************/
void MgrBreathData::InitializeBreathDataPtr (void)
{
    Initialize();
}

//*******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GetHFOTidalVolume()
//
//    Processing:
//      This operation returns HFO Tidal Volume
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      HFOTidalVolume
//
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//      None
//
//    Requirements:
//
/******************************************************************************/
LONG MgrBreathData::GetHFOTidalVolume(void)
{
    LONG hfoTidalVol = NetFlowInExh * 2 / 60;
    NetFlowInExh = 0;
    return hfoTidalVol;
}

//*******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: SetManInspActive (E_Bool active)
//
//    Processing:
//      Set manual breath base on active value
//
//    Input Parameters:
//      E_Bool active
//
//    Output Parameters:
//      None
//
//    Return Values:
//      None
//
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//      None
//
//    Requirements:
//
/******************************************************************************/
void MgrBreathData::SetManInspActive (E_Bool active)
{
    ManInspIsActive = active;
}

//*******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CompHfoVolume()
//
//    Processing:
//      This operation computes Volume in HFO mode
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      None
//
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//      None
//
//    Requirements:
//
/******************************************************************************/
void MgrBreathData::CompHfoVolume(LONG netFlow,LONG pressure)
{
#ifdef HFO_SYSTEM
    static E_Bool isInhalation = eFalse;
    static SHORT oneSecondCounter = 0;
    static SHORT numOfCycle=0;
    PatientDataGUIEvent hfoTidalVolume;

    displayVolInHfo +=netFlow*2.0/60.0;

    if ( (isInhalation == eFalse) &&(netFlow > DETECT_INHALATION_FLOW))
    {
        isInhalation = eTrue;
        numOfCycle++;
        displayVolInHfo = 0;

    }
    if( (isInhalation == eTrue)&&(netFlow < DETECT_INHALATION_FLOW))
    {
        isInhalation = eFalse;

    }
    oneSecondCounter++;
    //exh phase
    if(isInhalation == eFalse)
        NetFlowInExh += (netFlow);

    if(oneSecondCounter>=500)
    {
        LONG currenFreq =  MgrBdSetting::S_GetInstance()->GetCurrentValue(eHFOFrequency)/100;

        LONG temp = 0;
        if(currenFreq != 0)
            temp = GetHFOTidalVolume()/currenFreq;
        hfoTidalVolume.Value = (temp>=0)?temp:-temp;
        MainHandlerDelegate::S_GetInstance()->
                postCustomEvent<PatientDataGUIEvent>(eHFOTidalVolumePtDataID, hfoTidalVolume);

        oneSecondCounter = 0;
        numOfCycle = 0;
    }

#endif

}

//*******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CompHfoBaseFlow()
//
//    Processing:
//     Clear compensate buffer after calibrating proxy sensor
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      None
//
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//      None
//
//    Requirements:
//
/******************************************************************************/
void MgrBreathData::CompHfoBaseFlow()
{
    static double EMAFactor = 2.0/(1.0 + HFO_BASE_FLOW_SAMPLES);
    HFOBaseFlow = EMAFactor * (double) DeliveredFlow + (1.0 - EMAFactor) * HFOBaseFlow;
}

//*******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: APRV_ExhPhaseEnd_InPHigh()
//
//    Processing:
//      This operation is invoked from APRV_ExhPhase_InPHigh to compute the compliance and Vte at the end
//      of APRV Exh Phase in PHigh.
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      None
//
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//      None
//
//    Requirements:
//
/******************************************************************************/
void MgrBreathData::APRV_ExhPhaseEnd_InPHigh()
{
    E_PhaseId phaseId = (PhaseMgr::S_GetInstance())->GetCurrentPhaseId();
    if(phaseId == eAPRVLowPhase)
    {
        return;
    }
    else
    {
        (PdTask::S_GetInstance())->S_SetEventFlag(END_EXH_PS_APRV);
    }
}

//*******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ExpirationRoutine()
//
//    Processing:
//      This operation handles the Timer 1s Expiration.It updates PEEP data
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//        MaxpressData
//
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//      None
//
//    Requirements:
//
/******************************************************************************/
void MgrBreathData::ExpirationRoutine()
{
    E_PhaseId phaseId = (PhaseMgr::S_GetInstance())->GetCurrentPhaseId();

    switch (phaseId)
    {
        case eExhPhase:
            UpdatePEEPData();
            break;
        default:
            break;

    }
}

//*******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: UpdatePEEPData()
//
//    Processing:
//      This operation is invoked from ExpirationRoutine to update PEEP data and alarm every 1 second.
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      None
//
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//      None
//
//    Requirements:
//
/******************************************************************************/
void MgrBreathData::UpdatePEEPData(void)
{
    //Update PEEP data

    //    PatientDataGUIEvent PEEPMsg;
    SHORT currenP = BdBreathDataPtr->GetEndExhPress();
    if(currenP < 0)
    {
        currenP = 0;
    }
    //    PEEPMsg.Value = currenP;
    //    MainHandlerDelegate::S_GetInstance()->postCustomEvent<PatientDataGUIEvent>(ePEEPPtDataID, PEEPMsg);
}

//*******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: UpDatePHigh_Alarm()
//
//    Processing:
//      This operation checks alarm in PHigh phase in APRV mode
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//        MaxpressData
//
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//      None
//
//    Requirements:
//
/******************************************************************************/
void MgrBreathData::UpDatePHigh_Alarm()
{
    SHORT currenP = BdBreathDataPtr->GetAPRV_PHigh();

    SHORT HiPLimit = MgrBdSetting::S_GetInstance()->GetCurrentValue(eAPRVHighPHighLimit);
    SHORT LowPLimit = MgrBdSetting::S_GetInstance()->GetCurrentValue(eAPRVLowPHighLimit);

//    PdAlarms::S_Instance->CheckLow_PHigh(LowPLimit,currenP,Alarm[ALARM_IDX(eAPRVLow_PHigh)]);
}

//*******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: UpDatePLow_Alarm()
//
//    Processing:
//      This operation checks alarm in PLow phase in APRV mode
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//        MaxpressData
//
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//      None
//
//    Requirements:
//
/******************************************************************************/
void MgrBreathData::UpDatePLow_Alarm()
{
//    SHORT currenP = BdBreathDataPtr->GetAPRV_PLow();

//    SHORT HiPLimit = MgrBdSetting::S_GetInstance()->GetCurrentValue(eAPRVHighPLowLimit);
//    SHORT LowPLimit = MgrBdSetting::S_GetInstance()->GetCurrentValue(eAPRVLowPLowLimit);

//    PdAlarms::S_Instance->CheckHigh_PLow(HiPLimit,currenP,Alarm[ALARM_IDX(eAPRVHigh_PLow)]);
//    PdAlarms::S_Instance->CheckLow_PLow(LowPLimit,currenP,Alarm[ALARM_IDX(eAPRVLow_PLow)]);
}
