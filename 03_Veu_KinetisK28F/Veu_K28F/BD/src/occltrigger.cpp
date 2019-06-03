/******************************************************************************/
//$COMMON.CPP$
//   File Name:  OcclTrigger.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: 
//      This file contains the definition of the Occlusion Trigger, which
//      checks for the occluded circuit condition.
//
//   Interfaces:
//      The CheckCondition() operation retrieves the current pressure measured 
//      in the inhalation and exhalation limbs by calling the GetCurrentReading()
//      operation of the InhPressSensor and ExhPressSensor objects. 
//      CheckConditon retrieves the auto-zero state from the PInhAzTest and
//      and PExhAzTest objects.  Occlusion detection is not performed during
//      an auto-zero.
//
//   Restrictions:
//      None
/******************************************************************************/
#include <DeviceInterface.h>
#include <Util.h>
#include "OcclTrigger.h"
#include "ModeMgr.h"
#include "DebugTrace.h"

OcclTrigger* OcclTrigger::S_Instance = NULL;

const SHORT MAX_INH_PRESS_SCALED_IN_IMV = 125 * SCALE;     // 125 cmH2O, scaled
const SHORT MAX_EXH_PRESS_SCALED_IN_IMV = 125 * SCALE;     // 125 cmH2O, scaled
const SHORT MAX_PRESS_SCALED_IN_HFO = 200 * SCALE;     // 125 cmH2O, scaled

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: S_GetInstance()
//
//    Processing:
//      Since only 1 instance of this object should be created, any access
//      to the object comes via this operation.  If any other object wants
//      access to the OcclTrigger object, it invokes
//      Hip2AlarmTrigger::S_GetInstance(), which returns a pointer
//      to the OcclTrigger object.
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
//      Hip2AlarmTrigger* - pointer to the instance of this class
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
OcclTrigger* OcclTrigger::S_GetInstance(void)
{
    // If the object has not already been created, create it.
    if (NULL == S_Instance)
    {
        S_Instance = new OcclTrigger();
        ASSERTION(S_Instance != NULL);
    }

    return(S_Instance);

}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: OcclTrigger()
//
//    Processing: 
//      This operation is the constructor for the OcclTrigger.
//      This operation invokes the Trigger base class constructor.
//
//    Input Parameters:
//      triggerId - Trigger ID 
//      resistance - resistance of the circuit downstream of the pressure 
//          transducer
//      offset - pressure margin allowed in the occlusion check
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
OcclTrigger::OcclTrigger (E_TriggerId triggerId, LONG resistance, LONG offset) :
                                Trigger (triggerId)
{
    // Store the Resistance and Offset in the private attributes.
    Resistance = resistance;
    Offset = offset;

    // Initialize condition status to FALSE.    
    OcclCondition[ePvFlowRes].IsCondMet = eFalse;
    OcclCondition[ePexhPdiff].IsCondMet = eFalse;

    // Time limits are each 100 ms    
    OcclCondition[ePvFlowRes].TimeLimit = 100 / MS_PER_TICK;
    OcclCondition[ePexhPdiff].TimeLimit = 100 / MS_PER_TICK;

}   // end OcclTrigger()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: OcclTrigger()
//
//    Processing:
//      This operation is the constructor for the OcclTrigger.
//
//    Input Parameters:
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
OcclTrigger::OcclTrigger(): Trigger(eOcclusionTrigger)
{

    LastExhPress = 0;
    Resistance = 0;
    Offset = 0;

    for(int i = 0;i< eNumOcclIds; i++)
    {
        OcclCondition[i].TimeFirstMet = 0;
        OcclCondition[i].TimeLimit = 0;
        OcclCondition[i].IsCondMet = eFalse;
    }

}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: EnableTrigger()
//
//    Processing: 
//      This method is invoked when the Occlusion Trigger is enabled.
//      This operation sets the IsCondMet fields of the OcclCondition array
//      to FALSE, initializes the LastExhPress attribute with the current
//      exhalation pressure reading and invokes the base class method
//      Trigger::EnableTrigger().
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
void OcclTrigger::EnableTrigger (void)
{
    //    LastExhPress = ExhPressSensor->GetCurrentReading();

    OcclCondition[ePvFlowRes].IsCondMet = eFalse;
    OcclCondition[ePexhPdiff].IsCondMet = eFalse;

    // Invoke base class method.    
    Trigger::EnableTrigger ();

}   // end EnableTrigger()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CheckCondition()
//
//    Processing: 
//      This operation retrieves the current pressure measured in the exhalation
//      limb and inhalation limb by calling the GetCurrentReading() operation
//      of the ExhPressSensor object and the InhPressSensor object.   
//      GetFlow() is also called to get the flow rate.   Flow rate is either
//      the delivered flow or the exhaled flow rate (BTPS compensated).
//
//      If any of the following conditions are met, the TriggerDidFire
//      attribute is set to eTrue:
//
//          Pinh,i > 145 cmH2O
//
//                  or
//
//          Pexh,i > 130 cmH2O
//
//                  or
//
//          (Pinh,i - Pexh,i) > (Flow * Resistance + Offset) for 100 ms
//
//                  or
//
//          (-0.5 cmH2O <= Pexh,i - Pexh,i-1 <= 0.5 cmH2O) && 
//          (Pinh,i - Pexh,i > 50 cmH2O)    for 100 ms
//
//      The check is only performed if auto-zeroing is not in progress.
//
//      The value of TriggerDidFire is always returned to the caller.
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      E_Bool - indicates if the trigger fired or not
//
//    Pre-Conditions:
//      This operation assumes that the TriggerMgr only invokes it while
//      the trigger is enabled.
//
//    Miscellaneous:
//      None
//
//    Requirements:  
//
/******************************************************************************/
E_Bool OcclTrigger::CheckCondition (void)
{
    SHORT exhPress;             // pressure measured in the exhalation limb
    SHORT inhPress;             // pressure measured in the inhalation limb

    DeviceInterface::S_GetInstance()->RequestDevice(eInhPressSensorGetLastReading, &inhPress);
    DeviceInterface::S_GetInstance()->RequestDevice(eExhPressSensorGetLastReading, &exhPress);

    E_VentilationMode currentMode = ModeMgr::S_GetInstance()->GetCurrentModeId();
    switch (currentMode)
    {
    case eHFOMode:
        if ((inhPress > MAX_PRESS_SCALED_IN_HFO) || (exhPress > MAX_PRESS_SCALED_IN_HFO))
        {
            TriggerDidFire = eTrue;
        }
        break;
    case eMixedMode:
    case eSpontaneousMode:
        if ((inhPress > MAX_INH_PRESS_SCALED_IN_IMV) || (exhPress > MAX_EXH_PRESS_SCALED_IN_IMV))
        {
            TriggerDidFire = eTrue;
        }
        break;
    default:
        TriggerDidFire = eFalse;
        break;
    }

    return(TriggerDidFire);

}   // end CheckCondition()


