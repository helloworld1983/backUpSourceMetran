
#ifndef ACMODE_H
#define ACMODE_H

#include "NormBreathingMode.h"

class AcMode : public NormBreathingMode
{
public:
    //Pointer to the object instantiated from this class
    static AcMode* S_GetInstance (void);

    //Process doing something when another mode enters this mode
    virtual void Enter (E_VentilationMode fromModeId, UNSIGNED Time = 0);

    //Constructor for class
    AcMode (void);

    //Copy constructor for class
    AcMode (const AcMode&);

    //Process action for each ID trigger
    virtual void ProcessTriggerID(E_TriggerId id);

    //This operation handles the processing of the Breath Period Time Trigger for AC mode
    virtual void ProcessBpTimeTrigger (void);

    //This operation handles the processing of the Patient Inhalation Trigger for Ac Mode
    virtual void ProcessPtInhTrigger (void);

    //This operation handles the processing of the Exhalation Hold Trigger  for Ac Mode
    virtual void ProcessExhHoldTrigger (void);

    //Update alarm status
    virtual void UpdateAlarmStatus (void);

    //it forces alarms that cannot exist in the A/C mode into an auto-reset state
    void ReportModeChangeInducedReset (void);

    //$COMMON.ATTRIBUTE$
    //  Name: S_Instance
    //  Description:  Pointer to the single instance of this class.
    //  Units: None
    //  Range: n/a
    static AcMode* S_Instance;
};

#endif // ACMODE_H
