/******************************************************************************/
//$COMMON.CPP$
//   File Name:  InhTestBreathPhase.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: InhVABreathPhase is the Mand Press Inh phase in VA mode
//
//
/******************************************************************************/
#ifndef INHVABREATHPHASE_H
#define INHVABREATHPHASE_H

#include "PressInhPhase.h"
//#include "PhaseMgr.h"

class InhVABreathPhase : public PressInhPhase
{
public:
    //Constructor
    InhVABreathPhase
    (
            E_BdSettingId targetPressId,
            E_BdSettingId offsetPressId
    );
    //pointer to the object instantiated from this class
    static InhVABreathPhase* S_GetInstance(void);

    //Start phase
    virtual void Start(void);

    //end phase
    virtual void End(void);

    //Process bd cycle
    virtual void ProcessBdCycle(void);

    // static instance of alarm controls
    static InhVABreathPhase* S_Instance;

    //Set previous PIP
    void SetPrevPIP(LONG prevPIP);

    //GEt PIP target
    LONG GetPIPTarget(void);

    //Clear PIP Parameter
    void ClearPIDParameter(void);

    //Set previous inhalation type
    void SetPrevInhType(void);

    //Reset alarm counter
    void ResetAlarmCnt(void);

    //Copute PIP
    LONG CompPIP();

    //Constructor
    InhVABreathPhase(void);
    //copy constructor
    InhVABreathPhase(const InhVABreathPhase&);

    //$COMMON.ATTRIBUTE$
    //    Name: alarmHighCnt
    //
    //    Description: high alarm counter
    //
    //    Units: None
    //
    //    Range: N/A
    SHORT alarmHighCnt;

    //$COMMON.ATTRIBUTE$
    //    Name: alarmLowCnt
    //
    //    Description: alarm counter
    //
    //    Units: None
    //
    //    Range: N/A
    SHORT alarmLowCnt;

    //$COMMON.ATTRIBUTE$
    //    Name: mPrevPIP
    //
    //    Description: previous PIP
    //
    //    Units: None
    //
    //    Range: N/A
    LONG mPrevPIP;

    //$COMMON.ATTRIBUTE$
    //    Name: mPIP
    //
    //    Description: mPIP
    //
    //    Units: None
    //
    //    Range: N/A
    LONG mPIP;

    //$COMMON.ATTRIBUTE$
    //    Name: PIP
    //
    //    Description: PIP
    //
    //    Units: None
    //
    //    Range: N/A
    LONG PIP;

    //$COMMON.ATTRIBUTE$
    //    Name: Vt_integral
    //
    //    Description: Vt integral
    //
    //    Units: None
    //
    //    Range: N/A
    LONG Vt_integral;

    //$COMMON.ATTRIBUTE$
    //    Name: Vt_derivative
    //
    //    Description: Vt derivative
    //
    //    Units: None
    //
    //    Range: N/A
    LONG Vt_derivative;

    //$COMMON.ATTRIBUTE$
    //    Name: prev_Vt_error
    //
    //    Description: previous Vt error
    //
    //    Units: None
    //
    //    Range: N/A
    LONG prev_Vt_error;

    //$COMMON.ATTRIBUTE$
    //    Name: currentVte
    //
    //    Description: current Vte
    //
    //    Units: None
    //
    //    Range: N/A
    LONG currentVte;

    //$COMMON.ATTRIBUTE$
    //    Name: IsFirstBreath
    //
    //    Description: is first breath or not
    //
    //    Units: None
    //
    //    Range: N/A
    bool IsFirstBreath;

    //$COMMON.ATTRIBUTE$
    //    Name: breathCounter
    //
    //    Description: breath counter
    //
    //    Units: None
    //
    //    Range: N/A
    SHORT breathCounter;

    //$COMMON.ATTRIBUTE$
    //    Name: prevInhType
    //
    //    Description: previous inhalation type
    //
    //    Units: None
    //
    //    Range: N/A
    E_PhaseId prevInhType ;

    //variable for Vt direction
    //$COMMON.ATTRIBUTE$
    //    Name: Vt_dir
    //
    //    Description: Vt_dir
    //
    //    Units: None
    //
    //    Range: N/A
    int Vt_dir;

    //$COMMON.ATTRIBUTE$
    //    Name: prevVt_buf
    //
    //    Description: buffer of Previous Vt
    //
    //    Units: None
    //
    //    Range: N/A
    long prevVt_buf[3];

    //$COMMON.ATTRIBUTE$
    //    Name: avg_chg_vol
    //
    //    Description: average volume
    //
    //    Units: None
    //
    //    Range: N/A
    LONG avg_chg_vol;

    //$COMMON.ATTRIBUTE$
    //    Name: gKp
    //
    //    Description: gKp
    //
    //    Units: None
    //
    //    Range: N/A
    short gKp;

    //$COMMON.ATTRIBUTE$
    //    Name: gKi
    //
    //    Description: gKi
    //
    //    Units: None
    //
    //    Range: N/A
    short gKi;

    //$COMMON.ATTRIBUTE$
    //    Name: gKd
    //
    //    Description: gKd
    //
    //    Units: None
    //
    //    Range: N/A
    short gKd;

    //$COMMON.ATTRIBUTE$
    //    Name: mPrevPIPset
    //
    //    Description: Previous PIP set value
    //
    //    Units: None
    //
    //    Range: N/A
    LONG mPrevPIPset;

    //$COMMON.ATTRIBUTE$
    //    Name: mPrevVtset
    //
    //    Description: fPrevious Vt reset value
    //
    //    Units: None
    //
    //    Range: N/A
    LONG mPrevVtset; //for new VA

    //$COMMON.ATTRIBUTE$
    //    Name: mPrevBRset
    //
    //    Description: Previous BR reset value
    //
    //    Units: None
    //
    //    Range: N/A
    LONG mPrevBRset;
    int mTestBreathCount;
};


#endif // INHVABREATHPHASE_H
