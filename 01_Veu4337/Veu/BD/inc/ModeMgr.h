/******************************************************************************/
//$COMMON.H$
//   File Name:  MODEMGR.H
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: 
//      This file contains the declaration of the Mode Manager.  The Mode
//      Manager provides a common interface to the Mode objects.
/******************************************************************************/
#ifndef MODEMGR_H
#define MODEMGR_H

#include "Mode.h"

const SHORT NUM_ALARM_MODE_IDS = (SHORT) (eLastAlarmMode - eFirstAlarmMode + 1);

class ModeMgr
{
public:
    //Return current mode ID
    E_VentilationMode GetCurrentModeId (void);

    //Return the next Mode ID
    E_VentilationMode GetNextModeId(void);

    //Pointer to the object instantiated from this class
    static ModeMgr* S_GetInstance (void);

    //This operation causes the ventilator to exit its current mode
    //Put the ventilator in the operator set mode
    void GoToOperatorSetMode (void);

    //Transition out of the current mode into the requested alarm mode
    void GoToAlarmMode (E_VentilationMode nextMode);

    //Transition out of the current mode into the requested ME mode
    void GoToMEMode(void);

    //Transition out of the current mode into the requested Selftest mode
    void GoToSelfTestMode(void);

    //Transition out of the current mode into the last mode in the active list
    void ReturnToLastMode (void);

    //Get current pointer to mode
    inline Mode* GetCurrentModePtr (void)
    {
        return ModeList[CurrentModeId];
    }

    //Decrements the ActiveIx value
    //Removing the current mode ID from the active list
    void ProcessAutoReset (void);

    //Goes through the ActiveModeIdList
    //Removes any mode IDs for modes that are reset by the alarm reset request
    void ProcessManualReset (void);

    //Reset extreme high press or map when change mode
    void ClearExtremeHighCounter();


protected:

private:
    //Constructor of ModeMgr class
    ModeMgr (void);

    //Copy constructor of ModeMgr class
    ModeMgr (const ModeMgr&);

    //$COMMON.ATTRIBUTE$
    //  Name: S_Instance
    //  Description:  Pointer to the single instance of this class.
    //  Units: None
    //  Range: n/a
    static ModeMgr* S_Instance;

    //$COMMON.ATTRIBUTE$
    //  Name: ModeList
    //  Description:  Array of pointers to the Mode objects.
    //  Units: None
    //  Range: n/a
    Mode* ModeList[eNumberOfModes];

    //$COMMON.ATTRIBUTE$
    //  Name: ActiveModeIdList
    //  Description:
    //      The ActiveModeIdList is used to maintain a list of alarm modes
    //      that are active simultaneously although the ventilator is only
    //      in the highest priority active alarm mode.
    //
    //      For example, if the ventilator transitions to Apnea Ventilation
    //      then to Occlusion Mode, both of these alarm modes are in the
    //      ActiveModeList.  The ActiveModeIdList is used by the
    //      ReturnToLastMode() operation to return the ventilator to the
    //      last active mode.  So, in the same example, if the Occlusion
    //      condition resets, the ventilator returns to the Apnea Mode.
    //  Units: None
    //  Range: n/a
    E_VentilationMode ActiveModeIdList[NUM_ALARM_MODE_IDS];

    //$COMMON.ATTRIBUTE$
    //  Name: ActiveIx
    //  Description:  Index into the ActiveModeIdList.  This index points
    //      to the next available location.
    //  Units: None
    //  Range: n/a
    SHORT ActiveIx;

    //$COMMON.ATTRIBUTE$
    //  Name: CurrentModeId
    //  Description:  ID of the currently active mode.
    //  Units: None
    //  Range: n/a
    E_VentilationMode CurrentModeId;

    //$COMMON.ATTRIBUTE$
    //  Name: NextModeId
    //  Description:  ID of the mode being transitioned to; only valid on
    //      mode transitions
    //  Units: None
    //  Range: n/a
    E_VentilationMode NextModeId;
};

#endif // MODEMGR_H
