///******************************************************************************/
////$COMMON.H$
////   File Name:  APRV_PtInhPressTrigger.h
////   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
////
////   Class:
////      This file contains the class declaration for the APRV_PtInhPressTrigger
///******************************************************************************/
//#ifndef APRV_PTINHPRESSTRIGGER_H
//#define APRV_PTINHPRESSTRIGGER_H
//
//#include "Trigger.h"
//
//class APRV_PtInhPressTrigger : public Trigger
//{
//public:
//    //Constructor
//    APRV_PtInhPressTrigger(void);
//
//    //Check condition and process action
//    virtual E_Bool CheckCondition(void);
//
//    //Enable trigger
//    virtual void EnableTrigger(void);
//    virtual void EnableTrigger(UNSIGNED pressure);
//
//    //updates the PressureLimit as baseline pressure
//    virtual void PhaseInSettingChange(LONG newValue);
//
//    //Process trigger base on trigger ID
//    virtual inline void ProcessTrigger(Mode* mode);
//
//    //Check previous condition
//    virtual E_Bool PreCheckCondition(void);
//
//protected:
//
//
//private:
//    //Copy constructor
//    APRV_PtInhPressTrigger(const APRV_PtInhPressTrigger&);
//
//    //APRV high, low check condition
//    E_Bool APRV_PHigh_CheckCondition(void);
//    E_Bool APRV_PLow_CheckCondition(void);
//
//
//    //$COMMON.ATTRIBUTE$
//    //  Name: IsPressureFixed
//    //  Description:  Indicates if the PressureLimit is fixed for the
//    //      currently active exhalation phase.
//    //  Units: None
//    //  Range: eTrue, eFalse
//    E_Bool IsPressureFixed;
//
//    //$COMMON.ATTRIBUTE$
//    //  Name: InUnrestrictedPhase
//    //  Description:  Indicates if the unrestricted phase of exhalation
//    //      is active.
//    //  Units: None
//    //  Range: eTrue, eFalse
//    E_Bool InUnrestrictedPhase;
//
//    //$COMMON.ATTRIBUTE$
//    //  Name: PressureLimit
//    //  Description:  Baseline Pressure - Pressure Sensitivity
//    //  Units: cmH2O * SCALE
//    //  Range: -20 to 34.9 cmH2O
//    SHORT PressureLimit;
//
//    //$COMMON.ATTRIBUTE$
//    //  Name: PressureLimit
//    //  Description:  Baseline Pressure - Pressure Sensitivity
//    //  Units: cmH2O * SCALE
//    //  Range: 0.5 to 10.5 cmH2O
//    SHORT PressSens;
//
//    //$COMMON.ATTRIBUTE$
//    //  Name: PrevFilteredPress
//    //  Description:  Filtered pressure for the previous BD cycle;
//    //      used to filter the monitor press for the current cycle.
//    //  Units: cmH2O * SCALE
//    //  Range:  0 - 12000
//    LONG PrevFilteredPress_PHigh;
//    LONG PrevFilteredPress_PLow;
//
//};
//
///******************************************************************************/
////$COMMON.OPERATION$
////    Operation Name: ProcessTrigger()
////
////    Processing:
////      This operation invokes the ProcessPtInhTrigger() operation of the active
////      mode.
////
////    Input Parameters:
////      mode - pointer to the currently active Mode object
////
////    Output Parameters:
////      None
////
////    Return Values:
////      None
////
////    Pre-Conditions:
////      None
////
////    Miscellaneous:
////      None
////
////    Requirements:
////
///******************************************************************************/
//inline void APRV_PtInhPressTrigger::ProcessTrigger(Mode* mode)
//{
//    // Invoke the operation to process the patient pressure trigger in the
//    // currently active mode.
//    mode->ProcessTriggerID(eAPRV_PtInhPressTrigger);
//
//}   // end APRV_PtInhPressTrigger()
//
//#endif // APRV_APRV_PtInhPressTrigger_H
