/******************************************************************************/
//$COMMON.H$
//   File Name:  O2Setting.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: 
//      The O2Setting class contains the operations and attributes that
//      define the desired O2 based upon the operator setting and the status
//      of the 100% O2 request.  This class is derived from the BdSetting class.
/******************************************************************************/
#ifndef O2SETTING_H
#define O2SETTING_H

#include "BdSetting.h"
#include "LogEntry.h"

// The status of the All O2 Operator Request and the O2 Supply Pressure is
// phased in on breath phase boundaries.  This structure is used to keep
// track of current and pending values.
struct PhasedInBool
{
    E_Bool currentValue;
    E_Bool pendingValue;
};

class O2Setting : public BdSetting
{
public:
    static inline LONG GetDesiredAirRatio (void)
    {
        return DesiredAirRatio;
    }
    static inline LONG GetDesiredO2Ratio (void)
    {
        return DesiredO2Ratio;
    }
    static inline LONG CalcAirMixError (LONG measuredRatio)
    {
        return (DesiredAirRatio - measuredRatio);
    }

    // O2Conc = .79 * DesiredRatio + 0.21,  constants scaled by 2**7
    static inline LONG CalcO2MixError (LONG measuredRatio)
    {
        return ((DesiredO2Ratio * 101 + 27) >> 7);
    }

    static O2Setting* S_GetInstance (void);
    virtual void PhaseInSetting(void);

    SHORT GetHighO2AlarmLimit(void);
    SHORT GetLowO2AlarmLimit(void);

    void CalculateAirO2Flows (LONG totalFlow, LONG* desiredAirFlow,
                              LONG* desiredO2Flow);

    void ClearO2ValveFault(void);

    void SetAirValveFault(void);
    void ClearAirValveFault(void);

    void SetO2PressSwitchFault(E_Bool isO2Gone);
    void SetAirPressSwitchFault(E_Bool isAirGone);

    void UpdateAirO2Alarms(void);
    inline E_Bool O2SourceFault(void);
    inline E_Bool AirSourceFault(void);
    void Set100PercentO2(E_Bool);

protected:

private:
    O2Setting(E_BdSettingId);
    O2Setting(const O2Setting&);


    //$COMMON.ATTRIBUTE$
    //  Name: O2ValveFault
    //  Description:  Indicates if an O2 Valve fault has occurred
    //  Units: None
    //  Range: n/a
    E_Bool O2ValveFault;

    //$COMMON.ATTRIBUTE$
    //  Name: AirSourceFault
    //  Description:  Indicates if an Air Valve fault has occurred
    //  Units: None
    //  Range: n/a
    E_Bool AirValveFault;

    //$COMMON.ATTRIBUTE$
    //  Name: SourceFaultTransition
    //  Description:  Indicates if an Air or O2 fault has just occurred.
    //  Units: None
    //  Range: n/a
    E_Bool SourceFaultTransition;

    //$COMMON.ATTRIBUTE$
    //  Name: O2PressSwitchFault
    //  Description:  Indicates if the O2 Pressure Switch Fault has been
    //      detected.
    //  Units: None
    //  Range: n/a
    PhasedInBool O2PressSwitchFault;


    //$COMMON.ATTRIBUTE$
    //  Name: AirPressSwitchFault
    //  Description:  Indicates if the Air Pressure Switch Fault has been
    //      detected.
    //  Units: None
    //  Range: n/a
    PhasedInBool AirPressSwitchFault;
    //$COMMON.ATTRIBUTE$
    //  Name: AllO2Status
    //  Description:  Indicates the status of the 100% O2 request.
    //  Units: None
    //  Range: n/a
    PhasedInBool AllO2Active;

    //$COMMON.ATTRIBUTE$
    //  Name: S_Instance
    //  Description:  Pointer to the single instance of this class.
    //  Units: None
    //  Range: n/a
    static O2Setting* S_Instance;

    //$COMMON.ATTRIBUTE$
    //  Name: DesiredAirRatio
    //  Description:  Desired ratio of total gas from the air source.
    //  Units: None
    //  Range: (0 - 1) * 2**7  (0 - 100%)
    static LONG DesiredAirRatio;

    //$COMMON.ATTRIBUTE$
    //  Name: DesiredO2Ratio
    //  Description:  Desired ratio of total gas from the oxygen source.
    //  Units: None
    //  Range: (0 - 1) * 2**7  (0 - 100%)
    static LONG DesiredO2Ratio;
};

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: O2SourceFault()
//
//    Processing: 
//      This operation returns eTrue if either the O2 Valve Fault condition
//      or the O2 Pressure Switch Fault condition exists.
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      E_Bool - indicates whether or not an oxygen source fault exists
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
inline E_Bool O2Setting::O2SourceFault(void)
{
    if ((eTrue == O2ValveFault) || (eTrue == O2PressSwitchFault.pendingValue))
    {
        return(eTrue);
    }
    else
    {
        return(eFalse);
    }

}   // end O2SourceFault()


/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: AirSourceFault()
//
//    Processing:
//      This operation returns eTrue if either the O2 Valve Fault condition
//      or the O2 Pressure Switch Fault condition exists.
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      E_Bool - indicates whether or not an air source fault exists
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
inline E_Bool O2Setting::AirSourceFault(void)
{
    if ((eTrue == AirValveFault) || (eTrue == AirPressSwitchFault.pendingValue))
    {
        return(eTrue);
    }
    else
    {
        return(eFalse);
    }

}   // end O2SourceFault()

#endif  // O2SETING_H
