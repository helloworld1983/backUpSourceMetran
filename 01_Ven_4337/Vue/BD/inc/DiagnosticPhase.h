/******************************************************************************/
//$COMMON.H$
//   File Name:  DiagnosticPhase.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: 
//      The non-ventilation Phase class declared in this file is derived from
//      the Phase class.  The non-ventilation phase is the breath phase entered
//      when the ventilator is in a non-ventilation mode like total gas loss
//      or occlusion.
/******************************************************************************/
#ifndef DIAGNOSTICPHASE_H
#define DIAGNOSTICPHASE_H

#include "Phase.h"
//#include "ExhValveAlign.h"
//#include "queue.h"

enum E_DiagnosticID
{

    eNoEvent,

    eAirValveEvent,
    eO2ValveEvent,
    eExhValveEvent,
    eHfoValveEvent,
    eAllValveEvent, //for command ram to position 0
    eCommandEvent
};

enum E_MotorCommandType
{
    eIdleCmd,
    ePositionCmd,
    eRampCmd,
    eFlowControlCmd,
    eHFOStrokeVolumeCmd,
    eHFOFrequencyCmd,
    eHFOOffsetCmd
};

typedef struct
{
    E_DiagnosticID M_ID;
    E_MotorCommandType M_Type;
    USHORT M_Data;
} MotorEventData;

typedef enum
{
    eNotACmd = 0,
    eDoExhAlignmentCmd,
    eProxyResetLineCmd,
    eInpedanceLineCmd,
    eSafetyValveCmd,
    eExhPressureZeroCmd,
    eInhPressureZeroCmd,
    eDebugPortCmd,
    ePurgeFlowCmd,
    eOilPumpCmd,
    eGateCmd,
} E_DiagnosticCommand;

enum E_CommandStatus
{
    eStartStt = 0x55,
    eOnStt = eStartStt,
    eStopStt = 0xaa,
    eOffStt = eStopStt
};

typedef struct
{
    E_DiagnosticID ID;
    E_DiagnosticCommand Cmd;
    E_CommandStatus Stt;
} CommandData;

typedef struct
{
    E_DiagnosticID ID;
} BasicEvent;

typedef union
{
    //Used to HardwareDiagnostic
    BasicEvent basicDiagnosticEvent;
    CommandData Command;
    MotorEventData Motor;
} DiagnosticEventUnion;

const UNSIGNED DIAGNOSTIC_Q_MSG_SIZE = (sizeof(DiagnosticEventUnion) + sizeof(UNSIGNED) - 1)/sizeof(UNSIGNED);

union DiagnosticEntry
{
    DiagnosticEventUnion eventStruct;
    LONG Pad[DIAGNOSTIC_Q_MSG_SIZE];
};

class DiagnosticPhase : public Phase
{
public:
    //pointer to the object instantiated from this class
    static DiagnosticPhase* S_GetInstance(void);

    //Start phase
    virtual void Start(void);

    //end phase
    virtual void End(void);

    //Process bd cycle
    virtual void ProcessBdCycle(void);

    //sen comment
    STATUS Send(CommandData &event);
    STATUS Send(MotorEventData &event);

protected:

private:
    //Construcotr
    DiagnosticPhase(void);
    //Copy contructor
    DiagnosticPhase(const DiagnosticPhase&);
    //Destructor
    ~DiagnosticPhase();

    //$COMMON.ATTRIBUTE$
    //  Name: S_Instance
    //  Description:  Pointer to the single instance of this class.
    //  Units: None
    //  Range: n/a
    static DiagnosticPhase* S_Instance;

    //        Q *DiagnosticQueuePtr;

    //        ExhValveAlign * EVAPtr;

    //$COMMON.ATTRIBUTE$
    //  Name: diagnosticMessage
    //  Description:
    //  Units: None
    //  Range: n/a
    DiagnosticEntry diagnosticMessage;

    //$COMMON.ATTRIBUTE$
    //  Name: receivedSize
    //  Description:
    //  Units: None
    //  Range: n/a
    UNSIGNED receivedSize;

};

#endif // NONVENTPHASE_H
