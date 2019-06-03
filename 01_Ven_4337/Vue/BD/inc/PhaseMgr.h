/******************************************************************************/
//$COMMON.H$
//   File Name:  PHASEMGR.H
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: 
//      The Phase Manager defines a common interface to the breath phases.
/******************************************************************************/
#ifndef PHASEMGR_H
#define PHASEMGR_H

#include <Phase.h>
#include "CommonTypes.h"

// E_PhaseId is used to define the general identifiers associated with breath
// phases.  For most of the BD Application subsystem, it is only important to
// know what the general phase ID is.
enum E_PhaseId
{
    eInitPhase,
    eNonVentPhase, eExhHoldPhase, eAlarmPhase = eNonVentPhase,
    eDiagnosticPhase,
    eSelfTestPhase,
    eMandInhPhase,

    eSpontInhPhase,
    eSpontInhPhase_InAPRV_PLow,
    eSpontInhPhase_InAPRV_PHigh,
    ePlateauPhase,

    eMandInhTestBreathPhase,
    eMandInhVABreathPhase,

#ifdef HFO_SYSTEM
    eHFOPhase,
#endif
    eExhPhase,
    eAPRV_PLow_Phase,
    eAPRVLowPhase,
    eAPRVHighPhase,
    eAPRVExhPhase_InPHigh,
    eAPRVExhPhase_InPLow,

#ifdef  VA_WITH_PS
    eNCPAPPhase,
    ePsvInhVABreathPhase //for new VA
#else
    eNCPAPPhase
#endif
};

// E_PhaseId is used to define the specific phase ID.  For example, most of
// the BD Application code only needs to know if a Mandatory Inhalation Phase
// is active, but the PhaseMgr must keep track of the specific phase ID.  It
// needs to know if that mandatory inhalation is VCV, PCV or NIPPV-Mand.  This
// enumeration is used solely by the PhaseMgr to know the specific breath
// phase that is active.
enum E_SpecificPhaseId
{
    // There is only 1 Plateau and Exhalation Pause Phase. 
    eThePlateauPhase,
#ifdef HFO_SYSTEM
    // Specific HFO Phases
    eTheHFOPhase,
#endif
    // Specific Mandatory Phases
    eVcvInhPhase,
    ePcvInhPhase,

    eTheAPRVLowPhase,
    eTheAPRVHighPhase,
    eTheAPRVExhPhase_InPHigh,
    eTheAPRVExhPhase_InPLow,


    eTheMandInhTestBreathPhase,
    eTheMandInhVABreathPhase,

    // Specific Spontaneous Phases
    ePsvInhPhase,
    ePsvInhPhase_InAPRV_PLow,
    ePsvInhPhase_InAPRV_PHigh,

    // Specific Exhalation Phases
    eFlowTrigExhPhase,
    ePressTrigExhPhase,
    eNoTrigExhPhase,

    // Specific None Vent Phases
    eTheNonePhase,

    eTheDiagPhase,

    eTheSelfTestPhase,

    eTheNCAPPhase,

#ifdef  VA_WITH_PS
    eThePsvVAInhPhase, //for new VA
#endif

    // Total number of specific breath phases.  This entry must remain as the
    // last one in the list.
    eNumSpecificPhases
};

class PhaseMgr
{
public:
    //pointer to the object instantiated from this class
    static PhaseMgr* S_GetInstance (void);

    //Process Bd Cycle
    void ProcessBdCycle (void);

    //Process action when begin new phase
    void ProcessNewPhase (E_PhaseId newPhase);
    void ProcessNewVentType (E_VentilationType newVentType);
    void ProcessNewTriggerType (E_TriggerType newTriggerType);

    //Get current phase ID
     E_PhaseId GetCurrentPhaseId (void);

    //Get current venType
    E_VentilationType GetCurrentVentType(void);

    //Set Spontaneous Inhalation and Exhalation Phase Ids
    void SetExhPhaseId(void);

    //Get VCV Inhalation start flag
    E_Bool GetVCVInhPhaseStartedFlag(void);

    //set VCV Inhalation start flag
    void SetVCVInhPhaseStartedFlag(E_Bool);

    //Reset volume of adjment of VCV Inhalation Phase
    void ResetVolumeAdjustmentOfVCVInhPhase(void);

protected:

private:
    PhaseMgr (void);
    PhaseMgr (const PhaseMgr&);

    void SetCommonPressVolumePhases (void);
    void SetCurrentPhasePtr (E_SpecificPhaseId PhaseId);

    //$COMMON.ATTRIBUTE$
    //  Name: S_Instance
    //  Description:  Pointer to the single instance of this class.
    //  Units: None
    //  Range: n/a
    static PhaseMgr* S_Instance;

    //$COMMON.ATTRIBUTE$
    //  Name: PhaseList
    //  Description:  Array of pointers to the different breath phases.
    //  Units: None
    //  Range: n/a
    Phase* PhaseList[eNumSpecificPhases];

    //$COMMON.ATTRIBUTE$
    //  Name: CurrentPhasePtr
    //  Description:  Pointer to the currently active breath phase
    //  Units: None
    //  Range: n/a
    Phase* CurrentPhasePtr;

    //$COMMON.ATTRIBUTE$
    //  Name: MandInhPhaseId
    //  Description:  ID of the specific mandatory inhalation phase
    //  Units: None
    //  Range: eVcvInhPhase, ePcvInhPhase, eNippvMandInhPhase
    E_SpecificPhaseId MandInhPhaseId;

    //$COMMON.ATTRIBUTE$
    //  Name: MandInhPhaseId
    //  Description:  ID of the specific spontanous inhalation phase
    //  Units: None
    //  Range: eCpapInhPhase, ePsvInhPhase, eNippvSpontInhPhase
    E_SpecificPhaseId SpontInhPhaseId;

    //$COMMON.ATTRIBUTE$
    //  Name: ExhPhaseId
    //  Description:  ID of the specific exhalation phase
    //  Units: None
    //  Range: eFlowTrigExhPhase, ePressTrigExhPhase
    E_SpecificPhaseId ExhPhaseId;

    //$COMMON.ATTRIBUTE$
    //  Name: CurrentPhaseId
    //  Description:  general ID of the current phase
    //  Units: None
    //  Range: eExhPhase, eMandInhPhase, eSpontInhPhase,
    //      ePlateauPhase, eExhHoldPhase
    E_PhaseId CurrentPhaseId;

    //$COMMON.ATTRIBUTE$
    //  Name: CurrentTriggerType
    //  Description:  general ID of the current phase
    //  Units: None
    //  Range:
    E_TriggerType CurrentTriggerType;

    //$COMMON.ATTRIBUTE$
    //  Name: CurrentVentType
    //  Description:  general ID of the current phase
    //  Units: None
    //  Range:
    E_VentilationType CurrentVentType;
};

#endif // PHASEMGR_H
