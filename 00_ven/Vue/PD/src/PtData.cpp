//$COMMON.CPP$
//   File Name:  PtData.cpp
//   Copyright 1996 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: Computation of Patient related data
//              CompNumBreaths
//              CompCircuitComlianceVolume
//              CompExhTidalVolume
//              CompExhMinuteVolume
//              CompSpontRespRate
//              CompSpontMinuteVolume
//              CompTotalRespRate
//              CompRapidShallowBreathix
//              CompIeRatio
//              CompMeanAirwayPressure
//
//      The Patient Data Application subsystem is responsible for monitoring 
//		and calculating patient data, based on flow, and pressure provided by 
//		the Breath Delivery subsystem. The Patient Data Application subsystem 
//		calculates and monitors the following patient data: Exhaled Minute 
//		Volume, Exhaled Tidal Volume, Spontaneous Minute Volume, Rapid Shallow 
//		Breathing Index, I:E Ratio, Peak Inhalation Pressure, End Inhalation 
//		Pressure, Mean Airway Pressure, Total Respiratory Rate, Spontaneous 
//		Respiratory Rate and End Exhalation Pressure.
//
//      After computation, Patient data is transferred to the GUI software to 
//		be displayed on the user interface.
//
//      The Delivered %O2 is updated at 1 second intervals by retrieving the 
//		average the DataIO Oxygen sensor samples, updated every 100 ms.
//		The Internal %O2 is monitored to be less than 25 percent.  If it is
//		larger, then a non-silenceable alarm is activated.  Internal %O2 is 
//		retrieved and checked against threshold values for High Internal O2
//		Concentration.  An non-silenceable alarm is generated if it is not
//		within spec.
//
//      Calculated patient data is checked against threshold values for High O2
//		Concentration, High Respiratory Rate, Low O2 Concentration, Low Exhaled 
//		Mandatory Tidal Volume, Low Exhaled Spontaneous Tidal Volume, and  
//		Low Inhalation Pressure.  Data not within threshold 
//      limits are passed on to the Alarms subsystem for generating alarms and
//		maintaining status of each alarm on the ventilator.  These Alarm setting 
//		threshold values are retrieved from Breath Delivery's MgrBdSetting 
//		object.
//
//      The Self-Test subsystem provides the compliance compensation factor 
//		calculated during Extended Self-Test (EST) or Short Self-Test (SST).  
//		Circuit Compliance Volume is computed using the compliance compensation 
//		factor.  The compliance volume is subtracted from both the delivered and
//		exhaled volumes in order to compensate them for tubing compliance 
//		losses.  The Patient Data Application subsystem shall compute the 
//		Delivered Tidal Volume and maintain the Compliance Compensation Volume 
//		for internal uses.
//
//      Additionally, Patient Data Application subsystem interfaces with the 
//		Error Handling, RTOS, Watchdog and Utilities subsystems to perform basic 
//		system functions.
//
//   Interfaces: 
//               The Patient Data Application subsystem interfaces with the 
//               Breath Delivery, GUI, Devices, and Alarms application 
//				 subsystems. 
//
//   Restrictions:
//
//		There is only one instance of this class.
//
//      The Patient Data computation is scheduled through the use of event flags 
//		from the Breath Delivery subsystem.  The calculation of the patient data
//		must keep up the Breath Delivery subsystem's provision of flow and 
//		pressure readings otherwise system will go through system reset 
//		condition.  End Inhalation Pressure, End Inhalation Pressure Average, 
//		and Peak Inhalation Pressure are computed during the completion of 
//      Inhalation Phase signaled by the Breath Delivery subsystem.  Exhalation 
//		Tidal Volume, Delivered Tidal Volume, End Exhalation Pressure, Exhaled 
//		Minute Volume, Mean Airway Pressure, Spontaneous Respiratory Rate, 
//		Total Respiratory Rate, Inhalation and Exhalation Ratio (I:E ratio), and 
//		Rapid Shallow Breathing Index will be computed during the end of the 
//		Exhalation Phase signaled by the Breath Delivery subsystem.  Computation 
//		of the Delivered %O2 and Internal %O2 is computed every second.
/******************************************************************************/

#include <DeviceInterface.h>
#include <stdlib.h>
#include <cmath>
#include <limits>

#include "DebugTrace.h"
#include "AlarmInterface.h"

// include Patient Data files
#include "PtData.h"
#include "PdAlarm.h"

#include "AlarmTask.h"

#include "ModeMgr.h"
#include "MgrBreathData.h"
#include "MgrBdSetting.h"

// include GUI files
//#include "GUIInterface.h"
//#include "GuiTask.h"

// include fraction of Oxygen percentage sensor
//#include "fio2.h"


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

using namespace std;

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: S_GetInstance()
//
//    Processing:
//      Since only 1 instance of this object should be created, any access
//      to the object comes via this operation.  If any other object wants
//      access to the object, it invokes S_GetInstance(),
//      which returns a pointer to the object.
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
//      * - pointer to the object instantiated from this class
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
Ptdata* Ptdata::S_GetInstance (void)
{
    if(S_Instance == NULL)
    {
        S_Instance = new Ptdata ();
    }

    ASSERTION(S_Instance != NULL);

    return (S_Instance);
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CompNumBreaths  
//
//    Processing: Compute Number of breaths
//                Traverse through breath history in reverse order and add 
//                length of time for each breath; Counting the number of 
//                breaths that add up to a minute.  Values are used in exhaled
//				  minute volume calculations
//
//                All minute calculations depend on data from previous breaths.
//                Breath period, breath type (mandatory, spontaneous), exhaled
//                volume and mean airway pressure data shall be maintained for
//                the most recent 8 breaths.  The number of breaths included 
//                in the minute calculation for Total Respiratory Rate, 
//                Spontaneous Respiratory Rate, Exhaled Minute Volume and 
//                Spontaneous Minute Volume and in the mean airway pressure 
//                calculation shall be determined based on the number of 
//				  breaths of data available and the sum of the breath periods 
//				  represented by those breaths.
//
//    Input Parameters: None
//
//    Output Parameters: None 
//
//    Return Values: None
//
//    Pre-conditions: all Breath Delivery data is scaled by SCALE
//
//    Miscellaneous: None
//
//    Requirements: None      
//
/******************************************************************************/
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
            break;
        }
        if(breathtime >= MSECPERMIN)
        {
            break;
        }
        breathtime += (PdHist[backup].InhTime + PdHist[backup].ExhTime);
        if(PdSpontaneous (PdHist[backup].Indicator))
        {
            SpontBreaths++;
        }

        backup = (backup - 1) & MAX_BREATHS_MASK;
    }

    breathsofData = i;
    MinCalTime = breathtime;

}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CompCircuitComplianceVolume  
//
//    Processing:  Compute Circuit Compliance Volume
//
//                 The Circuit Compliance computation is used in the computation
//                 of the Exhaled Tidal Volume and Displayed Exhaled Tidal.
//
//                  Compliance volume is the volume calculated to compensate 
//                  for volume lost due to circuit compliance.  During 
//					inspiration,some of the delivered volume is lost in the 
//					tubing and never reaches the patient.  During exhalation, 
//					the gas compressed in the circuit is released through the 
//					exhalation valve and is measured with the exhaled volume.  
//					Therefore, the compliance volume is subtracted from both the 
//					delivered and the exhaled volumes in order to compensate 
//					them for tubing compliance losses.  
//
//    Input Parameters: None
//
//    Output Parameters: None 
//
//    Return Values: None
//
//    Pre-conditions: all breath Delivery data is scaled by SCALE
//                    EndInhPress is in units of ml So when
//                    any scaled variables are multiplied they need
//                    to be divided by SCALE to return the scaling.
//
//    Miscellaneous: None
//
//    Requirements: Shall be calculated at the beginning of inhalation   
//
/******************************************************************************/
void Ptdata::CompCircuitComplianceVolume (void)
{
    ComplVolume = ((CircuitComplianceFactor * PeepAdjEndInhPress) / SCALE);
    if (ComplVolume < 0)
    {
        ComplVolume = 0;
    }
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CompExhTidalVolume  
//
//    Processing:  Compute Exhaled Tidal Volume
//
//                  Exhaled tidal volume is the compliance (if the operator 
//                  setting for compliance compensation is on) and BTPS 
//                  compensated exhaled volume for the last mandatory or 
//                  spontaneous breath.  
//
//    Input Parameters: None
//
//    Output Parameters: None 
//
//    Return Values: None
//
//    Pre-conditions: all breath Delivery data is scaled by SCALE
//                    exhaled tidal volume is in units of ml
//                    breathsofData already computed.
//                    The compliance volume shall already be 
//                    calculated before this method is invoked.
//
//    Miscellaneous: None
//
//    Requirements: Shall be calculated at the beginning of inhalation   
//
/******************************************************************************/
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
    E_PhaseId phaseId = (PhaseMgr::S_GetInstance())->GetCurrentPhaseId();
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

    E_VentilationMode currentMode = ModeMgr::S_GetInstance()->GetCurrentModeId();
    E_VentilationMode nextMode = ModeMgr::S_GetInstance()->GetNextModeId();

    if((currentMode != eNivMode)&&(nextMode != eNivMode))
    {

        ProxyStatus Status;
        DeviceInterface::S_GetInstance()->RequestDevice(eProxySensorLastStatus, &Status);


        if((Status==eCalibrated)||(Status==eResetAndRecovered))
            // check for low exhaled minute alarm conditions
        {

            LONG HiVteLimit = MgrBdSetting::S_GetInstance()->GetCurrentValue(eHighTidalVolLimit);
            LONG LowVteLimit = MgrBdSetting::S_GetInstance()->GetCurrentValue(eLowTidalVolLimit);

            PdAlarms::S_GetInstance()->CheckHiCondition(HiVteLimit,ExhTidalVol, eHighVte);
            PdAlarms::S_GetInstance()->CheckLowCondition(LowVteLimit,ExhTidalVol, eLowVte);
        }
    }

}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CompExhMinuteVolume  
//
//    Processing:  Compute Exhaled Minute Volume
//
//                  Total respiratory rate and minute volume calculations 
//                  represent averaged values normalized to 1 minute.  
//
//    Input Parameters: None
//
//    Output Parameters: None 
//
//    Return Values: None
//
//    Pre-conditions: all breath Delivery data is scaled by SCALE
//                    exhaled tidal volume is in units of ml
//                    breathsofData already computed. time is in ms.
//
//    Miscellaneous: None
//
//    Requirements: Shall be calculated at the beginning of inhalation   
//
/******************************************************************************/
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

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: UpdateMandExhTidalVolume   
//
//    Processing: Compute averaged mandatory tidal volume
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None  
//
/******************************************************************************/
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
        ASSERTION(eFalse);
    }
    else
    {
        MandExhTidalVolAverage = sumMandExhTidalVol / NumMandBreaths;
    }

}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CompRapidShallowBreathix   
//
//    Processing: Compute Rapid Shallow Breathing Index
//
//					The Rapid Shallow Breathing Index is the ratio of the 
//					respiratory rate to exhaled tidal volume. The f/Vt index 
//					is calculated and the display updated at the beginning 
//					of each inhalation.   Because the presence of mandatory 
//					breaths and supported spontaneous breaths can bias the 
//					measurement of the Rapid Shallow Breathing Index, it shall 
//					only be displayed under the following conditions:
//
//					1. during volume or pressure control ventilation, the mode 
//					   is CPAP, and the Pressure Support setting is less than or 
//					   equal to 5 cm H2O
//
//					2. during NPPV when the mode is Spont, and the IPAP setting 
//					   is equal to the EPAP setting
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-conditions: The SpontMinVol and SpontRespRate calculations
//                    have already been made.
//
//    Miscellaneous: None
//
//    Requirements: None  
//
/******************************************************************************/
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

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CompIeRatio   
//
//    Processing: Compute Inhalation and Exhalation Ratio
//
//              The IeRatio shall be calculated and the display updated at the 
//				beginning of each inspiration.  If the number is negative it is
//				the inverse of the IeRatio this is based on whether the 
//				TimeInExh is larger than the TimeInInh. 
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-conditions: TimeInInh and TimeInExh have the current breath 
//                    data available
//
//    Miscellaneous: None
//
//    Requirements: None  
//
/******************************************************************************/
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

// 
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CompMeanAirwayPressure   
//
//    Processing: Compute Mean Airway Pressure
//
//                  Mean Airway pressure for a single breath is the airway 
//                  pressure averaged over an entire breath.  It applies to 
//                  both mandatory and spontaneous breaths.  The Average
//					is then computed base on the number breathsofData. 
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-conditions: NumInhSamples and NumExhSamples update by Bd
//
//    Miscellaneous: None
//
//    Requirements: None  
//
/******************************************************************************/
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

// 
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CompRespRate   
//
//    Processing: 
//					Compute Total Respiratory Rate and Spontaneous Respiratory 
//					Rate.
//
//                  Total respiratory rate is the total number of breaths 
//					normalized to the last 1 minute.  Total Respiratory Rate 
//					shall include both spontaneous and mandatory breaths.  
//					Respiratory rate shall be calculated and the display updated 
//					at the beginning of each inspiration.
//
//                  Spontaneous respiratory rate is the number of spontaneous 
//					breaths within the last 1minute.  Spontaneous Respiratory 
//					Rate shall include only spontaneous breaths.  Spontaneous 
//					Respiratory rate shall be calculated and the display updated 
//					at the beginning of each inspiration.  
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None  
//
/******************************************************************************/
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

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: PdTask::GetExhalationData   
//
//    Processing: Retrieves exhalation data from other domains
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-conditions: For the first breath, the exhalation phase will not be 
//					  done before the inhalation phase.
//
//    Miscellaneous: None
//
//    Requirements: None     
//
/******************************************************************************/
void Ptdata::GetExhalationData (void)
{
    GetEndExhFlow();

    // Retrieve the current mode from BD
    Mode = ModeMgr::S_GetInstance()->GetCurrentModeId();
    BaselinePressSetting = MgrBdSetting::S_GetInstance()->GetCurrentValue(eBaselinePress);
    PressureSum     = MgrBreathData::S_GetInstance()->GetPdBreathDataPtr()->GetPressureSum();
    ExhVolumeCompensated = MgrBreathData::S_GetInstance()->GetPdBreathDataPtr()->GetCompensatedExhVolume();
    ExhVolume = MgrBreathData::S_GetInstance()->GetPdBreathDataPtr()->GetExhVolume();
    EndExhPress     = (LONG) MgrBreathData::S_GetInstance()->GetPdBreathDataPtr()->GetEndExhPress();
    NumExhSamples   = MgrBreathData::S_GetInstance()->GetPdBreathDataPtr()->GetNumExhSamples();
    TimeInExh       = (LONG) MgrBreathData::S_GetInstance()->GetPdBreathDataPtr()->GetTimeInExh() * MS_PER_TICK;
    InhType         = MgrBreathData::S_GetInstance()->GetPdBreathDataPtr()->GetInhType();
    LowBaseLineAlarmLimit   =(LONG)MgrBdSetting::S_GetInstance()->GetCurrentValue(eLowBaselineLimit);

    //Update PEEP Alarm
    if(ModeMgr::S_GetInstance()->GetCurrentModeId() != eAPRVMode)
    {
        SHORT HiPLimit = MgrBdSetting::S_GetInstance()->GetCurrentValue(eHighBaselineLimit);
        SHORT LowPLimit = MgrBdSetting::S_GetInstance()->GetCurrentValue(eLowBaselineLimit);

        PdAlarms::S_GetInstance()->CheckHiBaseLine(HiPLimit,EndExhPress,eHighPeep);
        PdAlarms::S_GetInstance()->CheckLowBaseLine(LowPLimit,EndExhPress,eLowPeep);
    }

    HrrAlarmLimit   =(LONG)MgrBdSetting::S_GetInstance()->GetCurrentValue(eHrrLimit);
    LemvLimit       =(LONG)MgrBdSetting::S_GetInstance()->GetCurrentValue(eLemvLimit);
    HemvLimit       = MgrBdSetting::S_GetInstance()->GetCurrentValue(eHemvLimit);
    LestvLimit      = (LONG)MgrBdSetting::S_GetInstance()->GetCurrentValue(eLestvLimit);
    LemtvLimit      = (LONG)MgrBdSetting::S_GetInstance()->GetCurrentValue(eLemtvLimit);
    SupportPressure = (LONG)MgrBdSetting::S_GetInstance()->GetCurrentValue(eSupportPress);
    CircuitComplianceON = MgrBdSetting::S_GetInstance()->GetCurrentValue(eComplOnOff);

    // indicate patient data is done with BD data base
    MgrBreathData::S_GetInstance()->SetPdDone();

    // store info in history tables
    PdHist[Ix].ExhTime =TimeInExh;
    PdHist[Ix].InhTime =TimeInInh;
    PdHist[Ix].Indicator = InhType;

    PdAlarms::S_GetInstance()->ResetPEEPAlarmActiveTime();

}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GetInhalationData   
//
//    Processing: Retrieves all Breath Delivery inhalation data
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-conditions: The exhalation phase will not be done before the 
//                    inhalation phase.
//
//    Miscellaneous: None
//
//    Requirements: None     
//
/******************************************************************************/
void Ptdata::GetInhalationData (void)
{
    PeakInhPress    = (LONG) MgrBreathData::S_GetInstance()->GetPdBreathDataPtr()->GetPeakInhPress();
    EndInhPress     = (LONG) MgrBreathData::S_GetInstance()->GetPdBreathDataPtr()->GetEndInhPress();
    NumInhSamples   = (LONG) MgrBreathData::S_GetInstance()->GetPdBreathDataPtr()->GetNumInhSamples();
    TimeInInh       = MgrBreathData::S_GetInstance()->GetPdBreathDataPtr()->GetTimeInInh() * MS_PER_TICK;
    InhType         = MgrBreathData::S_GetInstance()->GetPdBreathDataPtr()->GetInhType();

    // mode phase in at the start of exhalation
    Mode = (E_VentilationMode) MgrBdSetting::S_GetInstance()->GetCurrentValue(eMode);

    // check for alarm conditions for LIP on mandatory breaths
    // avoid alarm checks situation when transitioning into spont mode

    if ( (PdMandatory(InhType) ) &&
            !((OldMode != eSpontaneousMode) && (Mode == eSpontaneousMode)) )
    {
        LipLimit = (LONG) MgrBdSetting::S_GetInstance()->GetCurrentValue(eLipLimit);
        PdAlarms::S_GetInstance()->CheckLowCondition(LipLimit, PeakInhPress,eLowInspPressure);
    }
    // save in history table
    PdHist[Ix].InhTidalVol = mInhVol;

}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: DoInhalationData   
//
//    Processing: Performs all inhalation data calulation and alarms checks
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-conditions: Inhalation phase will be done prior to an exhalation
//                    phase. 
//
//    Miscellaneous: None
//
//    Requirements: None     
//
/******************************************************************************/
void Ptdata::DoInhalationData (void)
{
    GetInhalationData ();
    CompInhMinuteVolume ();
    BuildInhMsg ();
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: DoExhalationData   
//
//    Processing: Performs all calculations for data that is valid during the
//				  exhalation phase
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-conditions: The exhalation phase will not be done before the 
//                    inhalation phase.
//
//    Miscellaneous: None
//
//    Requirements: None     
//
/******************************************************************************/
void Ptdata::DoExhalationData (void)
{
    GetExhalationData ();
    CompProxymalLeak();
    CompNumBreaths ();
    CompCircuitComplianceVolume ();
    CompExhTidalVolume ();
    CompExhMinuteVolume();
    CompMeanAirwayPressure ();
    CompIeRatio ();
    CompRespRate ();
    CompRapidShallowBreathix ();
    CompCompliance();
    CompAverageCompliance();
    CompLeakCompensate();

    Ix = (Ix + 1) & MAX_BREATHS_MASK;

    BuildExhMsg ();
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: DoO2  
//
//    Processing: 
//		The delivered %O2 is a measurement of the percent of oxygen in the gas 
//		delivered to the patient. If the oxygen monitor is not present, 
//		the Delivered %O2 display blanked from the patient data screen.
//
//		The internal O2 is also checked.
//
//		The displayed delivered %O2 is
//		1. be updated every one (1)  second  
//		2. be calculated as the average measurement obtained from the oxygen 
//		   monitor for the previous one (1) second, where the oxygen monitor is 
//		   sampled every 100ms, this is performed by Devices
// 
//		The %O2 display is be blanked for one (1) second after these events:
//		1. exit from a non-breathing mode
//		2. system initialization
//
//		If the oxygen monitor has failed the oxygen sensor test in EST or 
//		Diagnostics, the display will be blanked 
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-conditions: O2Ptr needs to be intialized.
//
//    Miscellaneous: None
//
//    Requirements: None     
//
/******************************************************************************/
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

    if(DeviceInterface::S_GetInstance()->RequestDevice(eGetHighO2AlarmLimit, &tempO2concentration))
    {
        HighO2AlarmLimit = (LONG)tempO2concentration;
    }
    else
    {
        //can not get high02 alarm limit
        DEBUG_PTDATA("Can not get high 02 alarm limit\n");
    }

    if(DeviceInterface::S_GetInstance()->RequestDevice(eGetLowO2AlarmLimit, &tempO2concentration))
    {
        HighO2AlarmLimit = (LONG)tempO2concentration;
    }
    else
    {
        //can not get low 02 alarm limit
        DEBUG_PTDATA("Can not get low 02 alarm limit\n");
    }

    if(DeviceInterface::S_GetInstance()->RequestDevice(eIsO2SensorConnect, &IsO2SensorConnect))
    {
        if(eTrue == IsO2SensorConnect)
        {
            if(eStandbyMode != ModeMgr::S_GetInstance()->GetCurrentModeId())
            {
                float rawO2 = INIT_VALUE;
                DeviceInterface::S_GetInstance()->RequestDevice(eFiO2SensorGetCurrentReading, &rawO2);
                float rawPress = INIT_VALUE;
                DeviceInterface::S_GetInstance()->RequestDevice(eFiO2SensorGetAvgPress, &rawPress);
                float rawFlow = INIT_VALUE;
                DeviceInterface::S_GetInstance()->RequestDevice(eFiO2SensorGetAvgFlow, &rawFlow);
                float rawAmplitude = amplitude;

                //do O2 compensation
                //eto added amplitude factor only HFO mode
                if(eHFOMode != ModeMgr::S_GetInstance()->GetCurrentModeId())
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
        PdAlarms::S_GetInstance()->CheckHiO2(HighO2AlarmLimit, O2Concentration, eHighO2);
        PdAlarms::S_GetInstance()->CheckLowO2(LowO2AlarmLimit, O2Concentration, eLowO2);
    }
    else
    {
        AlarmTask::S_GetInstance()->SetIsActive(eHighO2, eNotActive);
        AlarmTask::S_GetInstance()->SetIsActive(eLowO2, eNotActive);
        O2Concentration = BLANKED_LONG_VALUE;
    }

    BuildO2Msg();

}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ClearHistory   
//
//    Processing: ClearHistory
//
//              Patient data displays shall be blanked during all non-breathing
//              modes.
//
//              Patient data displays that are updated at the beginning of 
//				inspiration, with the exception of the breath indicator, shall 
//				be blanked until the start of the second inspiration following 
//				these events:
//
//              1. exit from a non-breathing mode
//              2. system initialization
//
//              The Breath Indicator shall be updated at the start of the first 
//              inspiration following the same events.
//              Patient data displays updated at the start of exhalation shall 
//				be blanked until the exhalation which follows the first 
//				inspiration after these events:
//
//              1. exit from a non-breathing mode
//              2. system initialization
//
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None     
//
/******************************************************************************/
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
    //    MainHandlerDelegate::S_GetInstance()->postCustomEvent<PatientDataGUIEvent>(eAPRVPHighPtDataID, PMsg);
    //    MainHandlerDelegate::S_GetInstance()->postCustomEvent<PatientDataGUIEvent>(eAPRVPLowPtDataID, PMsg);
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: BuildInhMsg   
//
//    Processing: Builds the Inhalation Message for the GUI
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None     
//
/******************************************************************************/
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

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: BuildExhMsg  
//
//    Processing: Builds the Exhalation Message for the GUI
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None     
//
/******************************************************************************/
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

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: BuildO2Msg   
//
//    Processing: Builds the O2 message for the GUI
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None 
//
/******************************************************************************/
void Ptdata::BuildO2Msg (void)

{
    //    O2Msg.Value = O2Concentration;
    //
    //    guiPtr->postCustomEvent<PatientDataGUIEvent>(eMonitoredO2PtDataID, O2Msg);

}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: BlankingData   
//
//    Processing: BlankingData
//
//              Patient data displays shall be blanked during all non-breathing
//              modes.
//
//              Patient data displays that are updated at the beginning of 
//				inspiration, with the exception of the breath indicator, shall 
//				be blanked until the start of the second inspiration following 
//				these events:
//
//              1. exit from a non-breathing mode
//              2. system initialization
//
//              The Breath Indicator shall be updated at the start of the first 
//              inspiration following the same events.
//              Patient data displays updated at the start of exhalation shall 
//				be blanked until the exhalation which follows the first 
//				inspiration after these events:
//
//              1. exit from a non-breathing mode
//              2. system initialization
//
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None     
//
/******************************************************************************/
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

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Ptdata   
//
//    Processing: The patient data class constructor does limited
//                  initialization.
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None     
//
/******************************************************************************/
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
    //    ClearHistory();
    //
    //    CircuitComplianceON = eFalse;
    //    CircuitComplianceFactor = 0;

}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CompInhMinuteVolume
//
//    Processing:  Compute Inhaled Minute Volume
//
//                  Total respiratory rate and minute volume calculations
//                  represent averaged values normalized to 1 minute.
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-conditions: all breath Delivery data is scaled by SCALE
//                    inhaled tidal volume is in units of ml
//                    breathsofData already computed. time is in ms.
//
//    Miscellaneous: None
//
//    Requirements: Shall be calculated at the beginning of inhalation
//
/******************************************************************************/
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

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation
//
//    Name: CompProxymalLeak()
//
//    Processing:
//
//    Input Parameters: None
//
//    Output Parameters:
//
//    Return Values:
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
void Ptdata::CompProxymalLeak (void)
{
    const LONG DEVIDE_ZERO = 0.0;
    E_VentilationMode currentMode = ModeMgr::S_GetInstance()->GetCurrentModeId();
    E_VentilationMode nextMode = ModeMgr::S_GetInstance()->GetNextModeId();

    if((currentMode != eNivMode)&&(nextMode != eNivMode))
    {
        ProxyStatus Status;
        DeviceInterface::S_GetInstance()->RequestDevice(eProxySensorLastStatus, &Status);
        if((Status==eCalibrated)||(Status==eResetAndRecovered))
        {
            LONG InhVolume = MgrBreathData::S_GetInstance()->GetPdBreathDataPtr()->GetInhVolume();
            LONG ExhVolume = MgrBreathData::S_GetInstance()->GetPdBreathDataPtr()->GetExhVolume();

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

            LONG ProxymalLeakHiLimit = MgrBdSetting::S_GetInstance()->GetCurrentValue(eProximalLeakLimit)/SCALE;

            if(ProxymalLeakHiLimit <= 100)
            {
                PdAlarms::S_GetInstance()->CheckHiCondition(ProxymalLeakHiLimit,ProxymalLeakValue,eProximalLeakage);
            }
            else
            {
                AlarmTask::S_GetInstance()->SetIsActive(eProximalLeakage, eNotActive);
            }
        }
        else
        {
            ProxymalLeakValue = BLANKED_LONG_VALUE;
            AlarmTask::S_GetInstance()->SetIsActive(eProximalLeakage, eNotActive);
        }
    }
    else
    {
        ProxymalLeakValue = BLANKED_LONG_VALUE;
        AlarmTask::S_GetInstance()->SetIsActive(eProximalLeakage, eNotActive);
    }

}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CompCompliance
//
//    Processing:
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
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

    E_PhaseId phaseId = (PhaseMgr::S_GetInstance())->GetCurrentPhaseId();
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

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation
//
//    Name: CompAverageCompliance()
//
//    Processing:
//
//    Input Parameters: None
//
//    Output Parameters:
//
//    Return Values:
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
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

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CompLeakCompensate
//
//    Processing:
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
void Ptdata::CompLeakCompensate(void)
{
    const double DEVIDE_FOR_ZERO = 0;
    const double EXH_VOL_LIMIT = 0;
    const double EXH_LEAK_LIMIT = 0;

    double Te = (double)MgrBreathData::S_GetInstance()->GetLeakVolTime()/1000.0;//[s]
    double InhVolOneBRTime = MgrBreathData::S_GetInstance()->GetInhVolOneBRTime();
    double ExhVolOneBRTime = (-1) * MgrBreathData::S_GetInstance()->GetExhVolOneBRTime();

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

    MgrBreathData::S_GetInstance()->ResetVol();
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation
//
//    Name: DoEndExhPSInAPRV()
//
//    Processing:
//
//    Input Parameters: None
//
//    Output Parameters:
//
//    Return Values:
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
void Ptdata::DoEndExhPSInAPRV(void)
{
    const LONG EXH_TIDAL_VOL_LIMIT = 8;
    const LONG EXH_TIDAL_VOL_LIMIT_TO_ZERO = 0;


    VteInAPRV = MgrBreathData::S_GetInstance()->GetPdBreathDataPtr()->GetVte();
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

    ProxyStatus Status;
    DeviceInterface::S_GetInstance()->RequestDevice(eProxySensorLastStatus, &Status);

    // check for low exhaled minute alarm conditions
    if((Status == eCalibrated)||(Status == eResetAndRecovered))
    {
        LONG HiVteLimit = MgrBdSetting::S_GetInstance()->GetCurrentValue(eHighTidalVolLimit);
        LONG LowVteLimit = MgrBdSetting::S_GetInstance()->GetCurrentValue(eLowTidalVolLimit);

        PdAlarms::S_GetInstance()->CheckHiCondition(HiVteLimit, ExhTidalVol, eHighVte);
        PdAlarms::S_GetInstance()->CheckLowCondition(LowVteLimit, ExhTidalVol, eLowVte);
    }

    CompExhMinuteVolume();
    CompCompliance_InAPRV();
    CompAverageCompliance();

    if((Status==eCalibrated)||(Status==eResetAndRecovered))
    {
        //        PatientDataGUIEvent PMsg;
        //        PMsg.Value = Compliance;
        //        MainHandlerDelegate::S_GetInstance()->postCustomEvent<PatientDataGUIEvent>(eCompliancePtDataID, PMsg);
        //        PMsg.Value = VteInAPRV;
        //        MainHandlerDelegate::S_GetInstance()->postCustomEvent<PatientDataGUIEvent>(eExpTidalVolumePtDataID, PMsg);
        //        PMsg.Value = ExhMinuteVol;
        //        MainHandlerDelegate::S_GetInstance()->postCustomEvent<PatientDataGUIEvent>(eExpMinuteVolumePtDataID, PMsg);
    }
    else
    {
        //        PatientDataGUIEvent PMsg;
        //        PMsg.Value = BLANKED_LONG_VALUE;
        //        MainHandlerDelegate::S_GetInstance()->postCustomEvent<PatientDataGUIEvent>(eCompliancePtDataID, PMsg);
        //        PMsg.Value = BLANKED_LONG_VALUE;
        //        MainHandlerDelegate::S_GetInstance()->postCustomEvent<PatientDataGUIEvent>(eExpTidalVolumePtDataID, PMsg);
        //        PMsg.Value = BLANKED_LONG_VALUE;
        //        MainHandlerDelegate::S_GetInstance()->postCustomEvent<PatientDataGUIEvent>(eExpMinuteVolumePtDataID, PMsg);
    }
}
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation
//
//    Name: DoEndInhPSInAPRV()
//
//    Processing:
//
//    Input Parameters: None
//
//    Output Parameters:
//
//    Return Values:
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
void Ptdata::DoEndInhPSInAPRV(void)
{
    GetInhalationData();
    CompInhMinuteVolume();

    //    InhMsg.PeakInspiratoryPressure = PeakInhPress;
    //    InhMsg.EndInhalationPressure = EndInhPress;

    if(InhType == eSupported)
    {
        LipLimit = (LONG) MgrBdSetting::S_GetInstance()->GetCurrentValue(eAPRVHighInhPress)
                                                                                                                                    + (LONG) MgrBdSetting::S_GetInstance()->GetCurrentValue(eSupportPress) - 200;
    }
    else if(InhType == eSpontaneous)
    {
        LipLimit = (LONG) MgrBdSetting::S_GetInstance()->GetCurrentValue(eAPRVLowInhPress) - 200;
    }

    HipLimit = (LONG) MgrBdSetting::S_GetInstance()->GetCurrentValue(eHipLimit);

    PdAlarms::S_GetInstance()->CheckLowPip(LipLimit, PeakInhPress, eLowInspPressure);
    PdAlarms::S_GetInstance()->CheckHiPip(HipLimit, PeakInhPress,  eHip);

    ProxyStatus Status;
    DeviceInterface::S_GetInstance()->RequestDevice(eProxySensorLastStatus, &Status);

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

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: DoPEEP()
//
//    Processing: Compute PEEP
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-conditions:
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
void Ptdata::DoPEEP(void)
{
    const SHORT CURRENP_LIMIT = 0;
    //Update PEEP data
    //    PatientDataGUIEvent PEEPMsg;

    SHORT currenP = MgrBreathData::S_GetInstance()->GetPdBreathDataPtr()->GetEndExhPress();

    if(currenP < CURRENP_LIMIT)
    {
        currenP = CURRENP_LIMIT;
    }
    //        PEEPMsg.Value = currenP;
    //    MainHandlerDelegate::S_GetInstance()->postCustomEvent<PatientDataGUIEvent>(ePEEPPtDataID, PEEPMsg);

    //Update PEEP Alarm
    SHORT HiPLimit = MgrBdSetting::S_GetInstance()->GetCurrentValue(eHighBaselineLimit);
    SHORT LowPLimit = MgrBdSetting::S_GetInstance()->GetCurrentValue(eLowBaselineLimit);

    PdAlarms::S_GetInstance()->CheckHiCondition(HiPLimit,currenP,eHighPeep);
    PdAlarms::S_GetInstance()->CheckLowCondition(LowPLimit,currenP,eLowPeep);
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation
//
//    Name: DoNCPAP()
//
//    Processing:
//
//    Input Parameters: None
//
//    Output Parameters:
//
//    Return Values:
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
void Ptdata::DoNCPAP(void)
{
    const int ALARM_SENSITIVITY = 3;
    SHORT TempValuemNCPAPPressure;

    DeviceInterface::S_GetInstance()->RequestDevice(ePressureSensorGetLastReading, &TempValuemNCPAPPressure);
    mNCPAP_Pressure = (LONG)TempValuemNCPAPPressure;

    LONG HiLimit = MgrBdSetting::S_GetInstance()->GetCurrentValue(eHighNCPAPLimit);
    LONG LowLimit = MgrBdSetting::S_GetInstance()->GetCurrentValue(eLowNCPAPLimit);

    //        NCPAPMsg.Value = mNCPAP_Pressure;

    //        guiPtr->postCustomEvent<PatientDataGUIEvent>(eNCPAPPtDataID, NCPAPMsg);

    PdAlarms::S_GetInstance()->CheckHiCondition(HiLimit,mNCPAP_Pressure,eHighNCPAP,ALARM_SENSITIVITY);
    PdAlarms::S_GetInstance()->CheckLowCondition(LowLimit,mNCPAP_Pressure,eLowNCPAP);
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation
//
//    Name: DoAPRV_PHigh()
//
//    Processing:
//
//    Input Parameters: None
//
//    Output Parameters:
//
//    Return Values:
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
void Ptdata::DoAPRV_PHigh(void)
{
    LONG liplimit = 0;
    const LONG RESET = 0;
    const LONG OFFSET = 200;

    APRV_EndInhPress = MgrBreathData::S_GetInstance()->GetPdBreathDataPtr()->GetEndInhPress();

    LONG endPLowPress = (LONG)MgrBreathData::S_GetInstance()->GetBdBreathDataPtr()->GetAPRV_PHigh();
    SHORT HiPLimit = MgrBdSetting::S_GetInstance()->GetCurrentValue(eAPRVHighPHighLimit);
    SHORT LowPLimit = MgrBdSetting::S_GetInstance()->GetCurrentValue(eAPRVLowPHighLimit);

    PdAlarms::S_GetInstance()->CheckHighPHigh5Time(HiPLimit,endPLowPress,eAPRVHigh_PHigh);
    PdAlarms::S_GetInstance()->CheckLowPHigh5Time(LowPLimit,endPLowPress,eAPRVLow_PHigh);

    if(eActive == AlarmTask::S_GetInstance()->GetIsActive(eAPRVHigh_PHigh))
    {
        AlarmTask::S_GetInstance()->SetIsActive(eAPRVHigh_PHigh, eNotActive);
    }

    DoInhalationData();

    if(InhType == eSupported)
    {
        liplimit = (LONG) MgrBdSetting::S_GetInstance()->GetCurrentValue(eAPRVHighInhPress)
                                                                                                                    + (LONG) MgrBdSetting::S_GetInstance()->GetCurrentValue(eSupportPress) - OFFSET;
    }
    else if(InhType == eSpontaneous)
    {
        liplimit = (LONG) MgrBdSetting::S_GetInstance()->GetCurrentValue(eAPRVLowInhPress) - OFFSET;
    }
    else
    {
        liplimit = RESET;
    }

    PdAlarms::S_GetInstance()->CheckLowPip(liplimit, PeakInhPress, eLowInspPressure);
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation
//
//    Name: DoAPRV_PLow()
//
//    Processing:
//
//    Input Parameters: None
//
//    Output Parameters:
//
//    Return Values:
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
void Ptdata::DoAPRV_PLow(void)
{
    PdAlarms::S_GetInstance()->ResetPLowAlarmActiveTime();
    LONG endPLowPress = (LONG)MgrBreathData::S_GetInstance()->GetPdBreathDataPtr()->GetAPRV_PLow();

    SHORT HiPLimit = MgrBdSetting::S_GetInstance()->GetCurrentValue(eAPRVHighPLowLimit);
    SHORT LowPLimit = MgrBdSetting::S_GetInstance()->GetCurrentValue(eAPRVLowPLowLimit);

    PdAlarms::S_GetInstance()->CheckHighPLow5Time(HiPLimit,endPLowPress,eAPRVHigh_PLow);
    PdAlarms::S_GetInstance()->CheckLowPLow5Time(LowPLimit,endPLowPress,eAPRVLow_PLow);

    GetExhalationData();
    CompProxymalLeak();
    CompNumBreaths();
    CompExhTidalVolumeInAPRV();
    CompExhMinuteVolume();
    CompMeanAirwayPressure();
    CompIeRatio();
    CompRespRate();
    CompRapidShallowBreathix();
    CompCompliance_InAPRV();
    CompAverageCompliance();
    CompLeakCompensate();

    Ix = (Ix + 1) & MAX_BREATHS_MASK;

    BuildAPRVPLow();
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation
//
//    Name: DoAPRVBR()
//
//    Processing:
//
//    Input Parameters: None
//
//    Output Parameters:
//
//    Return Values:
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
void Ptdata::DoAPRVBR(void)
{
    IsFirst_1minute = false;
    //    ExhMsg.TotalRespiratoryRate = MgrBreathData::S_GetInstance()->GetAPRVTotalBreathRate()*SCALE;
    //    ExhMsg.SpontaneousRespiratoryRate = MgrBreathData::S_GetInstance()->GetAPRVSpontBreathRate()*SCALE;

    //    guiPtr->postCustomEvent<EndExhalationDataGUIEvent>(eEndExhalationData, ExhMsg);

    MgrBreathData::S_GetInstance()->ResetAPRVBR();
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation
//
//    Name: DoHFOData()
//
//    Processing:
//
//    Input Parameters: None
//
//    Output Parameters:
//
//    Return Values:
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
void  Ptdata::DoHFOData(void)
{
#ifdef HFO_SYSTEM
    GetHFOData();
    BuildHFOMsg();
#endif
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GetCircuitComplianceFactor
//
//    Processing: Get the Circuit Compliance Factor
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
void Ptdata::GetCircuitComplianceFactor(void)
{
    //    CircuitComplianceFactor = systemConfigFile->getValue(ePtCircuitCompliance);
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ModeInducedAlarmClearing
//
//    Processing: Some of the alarms are automatically cleared when the
//                ventilator changes modes or ventilation type.  These are
//                cleared because these alarms do not pertain to the new
//                ventilation mode or type.
//                If the Alarms domain has not finished processing the last
//                alarms request a sleep is done and the request is reinitiated.
//                It is then assumed that the second request will be satisfied.
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
void Ptdata::ModeInducedAlarmClearing(void)
{
    //    static const UNSIGNED SOMETIME = 25 / MS_PER_TICK;
    E_ReturnStatus  stat0;

    switch (Mode)
    {

        case eSpontaneousMode:
            stat0 = AlarmTask::S_GetInstance()->SetIsActive(eLowInspPressure, eNotActive);

            // allow alarms to process event
            if (stat0 == eError)
            {
                //                usleep((tasks[eAlarms].SchedulingInterval + SOMETIME)*1000);
                AlarmTask::S_GetInstance()->SetIsActive(eLowInspPressure, eNotActive);
            }
            break;
        default:
            // don't clear any alarms
            break;
    };

} // ModeInducedAlarmClearing

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation
//
//    Name: GetAverageVte()
//
//    Processing:
//
//    Input Parameters: None
//
//    Output Parameters: Exh Leak
//
//    Return Values: Exh Leak
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
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

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation
//
//    Name: GetAverageVte12()
//
//    Processing:
//
//    Input Parameters: None
//
//    Output Parameters: Exh vol
//
//    Return Values: Exh vol
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
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

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation
//
//    Name: GetAverageCompliance12()
//
//    Processing:
//
//    Input Parameters: None
//
//    Output Parameters: Compliance
//
//    Return Values: Compliance
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
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

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation
//
//    Name: ClearVteBuffer()
//
//    Processing: Clear ExhTidalVolArray, then Set the lastest ExhTidalVol value
//                to ExhTidalVolArray[0]
//
//    Input Parameters: None
//
//    Output Parameters: Exh Leak
//
//    Return Values: Exh Leak
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
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

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GetEndExhFlow_Average
//
//    Processing: Get the flow at the end of Exhalation
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
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

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation
//
//    Name: GetExhLeak()
//
//    Processing:
//
//    Input Parameters: None
//
//    Output Parameters: Exh Leak
//
//    Return Values: Exh Leak
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
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

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation
//
//    Name: ClearExhLeakArray()
//
//    Processing:
//
//    Input Parameters: None
//
//    Output Parameters:
//
//    Return Values:
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
void Ptdata::ClearExhLeakArray(void)
{
    for(int i = 0;i < EIGHT_SAMPLES; i++)
    {
        ExhLeakArray[i] = 0;
    }
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CompExhTidalVolumeInAPRV
//
//    Processing:
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-conditions:
//
//    Miscellaneous: None
//
//    Requirements: Shall be calculated at the beginning of PHigh
//
/******************************************************************************/
void Ptdata::CompExhTidalVolumeInAPRV(void)
{
    const LONG MAX_NUMBER_OF_VOL = 8;
    const LONG LIMIT_VOL_TO_ZERO = 0;

    VteInAPRV = MgrBreathData::S_GetInstance()->GetPdBreathDataPtr()->GetVte();
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

    ProxyStatus Status;
    DeviceInterface::S_GetInstance()->RequestDevice(eProxySensorLastStatus, &Status);

    if((Status == eCalibrated)||(Status == eResetAndRecovered))
    {
        // check for low exhaled minute alarm conditions
        LONG HiVteLimit = MgrBdSetting::S_GetInstance()->GetCurrentValue(eHighTidalVolLimit);
        LONG LowVteLimit = MgrBdSetting::S_GetInstance()->GetCurrentValue(eLowTidalVolLimit);

        PdAlarms::S_GetInstance()->CheckHiCondition(HiVteLimit,ExhTidalVol, eHighVte);
        PdAlarms::S_GetInstance()->CheckLowCondition(LowVteLimit,ExhTidalVol, eLowVte);
    }
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation
//
//    Name: CompCompliance_InAPRV()
//
//    Processing:
//
//    Input Parameters: None
//
//    Output Parameters:
//
//    Return Values:
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
void Ptdata::CompCompliance_InAPRV(void)
{
    const double DEVIDE_FOR_ZERO = 0;
    const double RESET = 0;

    double tmp = (double)APRV_EndInhPress - (double)MgrBreathData::S_GetInstance()->GetBdBreathDataPtr()->GetAPRV_PLow();

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


/******************************************************************************/
//$COMMON.OPERATION$
//    Operation
//
//    Name: GetMinCompliance(SHORT index)
//
//    Processing:
//
//    Input Parameters: None
//
//    Output Parameters:
//
//    Return Values:
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
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
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation
//
//    Name: GetMaxCompliance(SHORT index)
//
//    Processing:
//
//    Input Parameters: None
//
//    Output Parameters:
//
//    Return Values:
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
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


/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GetHFOData
//
//    Processing: Retrieves all Breath Delivery HFO data
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
void Ptdata::GetHFOData(void)
{
    //    map           = HfoController->GetFilterPressIntegrator();
    //    maxpress      = mgrBdPtr->GetMaxPresData();
    //    minpress      = mgrBdPtr->GetMinPresData();
    //    amplitude      = maxpress - minpress;
    //    hfoBaseFlow = mgrBdPtr->GetHFOBaseFlow();
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: BuildHFOMsg
//
//    Processing: Builds the HFO Message for the GUI
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/

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

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation
//
//    Name: CalculateHFOExh()
//
//    Processing:
//
//    Input Parameters: None
//
//    Output Parameters:
//
//    Return Values:
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
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
        hfoExhFlow += MgrBreathData::S_GetInstance()->GetDeliveredFlow();
        count++;
    }
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation
//
//    Name: BuildAPRVPLow()
//
//    Processing:
//
//    Input Parameters: None
//
//    Output Parameters:
//
//    Return Values:
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
void Ptdata::BuildAPRVPLow(void)
{
    //    ExhMsg.MeanAirwayPressure = MeanAirwayPressAvg;
    //    ExhMsg.SpontaneousMinuteVolume = SpontMinuteVol;
    //    if(IsFirst_1minute)
    //    {
    //        ExhMsg.TotalRespiratoryRate = MgrBreathData::S_GetInstance()->GetAPRVTotalBreathRate()*SCALE;
    //        ExhMsg.SpontaneousRespiratoryRate = MgrBreathData::S_GetInstance()->GetAPRVSpontBreathRate()*SCALE;
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

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GetEndExhFlow
//
//    Processing: Get the flow at the end of Exhalation
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
void Ptdata::GetEndExhFlow(void)
{
    EndExhFlowArray[EndExhFlowIx] = MgrBreathData::S_GetInstance()->GetBdBreathDataPtr()->GetEndExhFlow();
    EndExhFlowIx++;
    EndExhFlowIx = EndExhFlowIx % EIGHT_SAMPLES;
}


/******************************************************************************/
//$COMMON.OPERATION$
//    Operation
//
//    Name: GetMinVteArray()
//
//    Processing: Get min value in ExhTidalVolArray
//
//    Input Parameters: None
//
//    Output Parameters: Exh Leak
//
//    Return Values: Exh Leak
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
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

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation
//
//    Name: GetMinVteArray()
//
//    Processing: Get max value in ExhTidalVolArray
//
//    Input Parameters: None
//
//    Output Parameters: Exh Leak
//
//    Return Values: Exh Leak
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
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

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation
//
//    Name: GetMinComplianceArray12()
//
//    Processing: Get min 3 values in Compliance12
//
//    Input Parameters: None
//
//    Output Parameters: Compliance minimum 3 samples
//
//    Return Values: None
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
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

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation
//
//    Name: GetMaxComplianceArray12()
//
//    Processing: Get axn 3 values in ComplianceArray12
//
//    Input Parameters: None
//
//    Output Parameters: Compliance maximum 3 samples
//
//    Return Values: None
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
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

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation
//
//    Name: GetMinVteArray12()
//
//    Processing: Get min 3 values in ExhTidalVolArray12
//
//    Input Parameters: None
//
//    Output Parameters: Vte minimum 3 samples
//
//    Return Values: None
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
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

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation
//
//    Name: GetMaxVteArray12()
//
//    Processing: Get axn 3 values in ExhTidalVolArray12
//
//    Input Parameters: None
//
//    Output Parameters: Vte maximum 3 samples
//
//    Return Values: None
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
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

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: PdSpontaneous
//
//    Processing: returns true if inhalation is spontaneous
//
//    Input Parameters: E_BreathPhaseIndicator inhtype
//
//    Output Parameters: None
//
//    Return Values: E_Bool
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
E_Bool Ptdata::PdSpontaneous (E_BreathPhaseIndicator inhtype)
{
    E_Bool retval;

    if((inhtype == eSpontaneous) || (inhtype == eSupported))
        retval = eTrue;
    else
        retval = eFalse;

    return (retval);
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: PdMandatory
//
//    Processing: returns true if inhalation is mandatory
//
//    Input Parameters: E_BreathPhaseIndicator inhtype
//
//    Output Parameters: None
//
//    Return Values: E_Bool
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
E_Bool Ptdata::PdMandatory (E_BreathPhaseIndicator inhtype)
{
    E_Bool retval;

    if((inhtype == eAssisted) || (inhtype == eMandatory))
        retval = eTrue;
    else
        retval = eFalse;

    return (retval);
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GetComplianceAverage
//
//    Processing: Get compliance Average
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values: double - Compliance Average
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
double Ptdata::GetComplianceAverage(void)
{
    return ComplianceAverage;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GetVte
//
//    Processing: Get GetVte
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values: LONG ExhTidalVol
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
LONG Ptdata::GetVte(void)
{
    return ExhTidalVol;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GetProxymalLeak
//
//    Processing: Get ProxymalLeak
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values: float ProxymalLeakValue
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
float Ptdata::GetProxymalLeak(void)
{
    return ProxymalLeakValue;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GetComplVolumne
//
//    Processing: Get ComplVolumne
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values: LONG ComplVolume
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
LONG Ptdata::GetComplVolumne(void)
{
    return ComplVolume;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GetComplianceFactor
//
//    Processing: Get compliance compensation
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values: LONG CircuitComplianceFactor
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
LONG Ptdata::GetComplianceFactor(void)
{
    return CircuitComplianceFactor;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GetTimeInInh
//
//    Processing: Get time in inhalation
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values: ULONG TimeInInh
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
ULONG Ptdata::GetTimeInInh(void)
{
    return TimeInInh;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GetLastMandExhVt
//
//    Processing: Get last mandatory exhalation Vt
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values: LONG LastMandExhVt
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
LONG Ptdata::GetLastMandExhVt(void)
{
    return LastMandExhVt;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GetPrevPhaseId
//
//    Processing: Get Previous Phase ID
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values: E_PhaseId prevphaseId_vt
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
E_PhaseId Ptdata::GetPrevPhaseId(void)
{
    return prevphaseId_vt;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GetPrevPhaseId
//
//    Processing: Get Previous of Previous Phase ID
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values: E_PhaseId prevprevphaseId_vt
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
E_PhaseId Ptdata::GetPrevprevPhaseId(void)
{
    return prevprevphaseId_vt;
}
