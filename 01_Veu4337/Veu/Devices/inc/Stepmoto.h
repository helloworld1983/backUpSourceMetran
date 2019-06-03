//******************************************************************************//
//$COMMON.H$
//    Filename: stepmoto.h
//    Copyright 1996 InVentived Technologies, Inc. All Rights Reserved.
//
//    Class:  StepMotor
//    This class is the class for the Step Motor Controller Devices
//******************************************************************************//
#ifndef    STEPMOTO_H
#define    STEPMOTO_H

#include "standard.h"
#include "CommonConstants.h"
#include "Errorhan.h"
#include "stdint.h"

#define PI 3.14159265
// Step Motor Constants
static const   USHORT MIN_STEP_POSITION = 0;
static const   USHORT MAX_STEP_POSITION = 2200;
static const   USHORT STEP_COMMAND_SIZE = 3;
static const   USHORT LIFTOFF_OFFSET    = 200;

#define SCK_PORT                    (0x1)
#define SCK_PIN                     (19)

#define MISO_PORT                   (0x1)
#define MISO_PIN                    (3)

#define MOSI_PORT                   (0x1)
#define MOSI_PIN                    (4)

#define CS1_PORT                     (0x1)
#define CS1_PIN                      (5)
#define CS1_GPIO_PORT                (1)
#define CS1_GPIO_PIN                 (8)

#define CS2_PORT                     (0x1)
#define CS2_PIN                      (6)
#define CS2_GPIO_PORT                (1)
#define CS2_GPIO_PIN                 (9)

#define CS3_PORT                     (0x1)
#define CS3_PIN                      (7)
#define CS3_GPIO_PORT                (1)
#define CS3_GPIO_PIN                 (0)

#define CS4_PORT                     (0x1)
#define CS4_PIN                      (8)
#define CS4_GPIO_PORT                (1)
#define CS4_GPIO_PIN                 (1)

#define LPC_SSP           LPC_SSP1
#define SSP_IRQ           SSP0_IRQn
#define SSP_BIT_RATE                (1500000)

#define CLK0                        (0)

#define MAX_STROKE_VOL_ADJUSTMENT  200
#define MIN_STROKE_VOL_ADJUSTMENT  0
#define NUM_STROKE_INDEX (MAX_STROKE_VOL_ADJUSTMENT - MIN_STROKE_VOL_ADJUSTMENT + 1)

#define NUM_FREQUENCY_INDEX 14

enum E_ValveID
{
    eAirValve,
    eO2Valve,
    eExhValve,
    eHfoValve
};

class StepMotor
{
public:
    StepMotor(UCHAR  selectid, E_ValveID Id);

    ~StepMotor(void);

    void Init(UCHAR MotorID);                        // Init step motor controller
    void SetStepPosition(int step);  // Command step motor to position
    void RampToPosition(int position, E_Bool fastFlag = eFalse);
    void Close(void);
    void PowerDownSetStepPosition(USHORT step);
    E_ValveID GetValveID(){return ValveID;}

    void SetMotorFree(void)
    {
//        Switch->SetDesiredState(eOff);
    }

    void RetrieveRevision();
    void GetResponse(UCHAR * buf, UCHAR * count);
    void WaitForInitComplete();
    USHORT GetRevision();
    int GetStepPosition();
    USHORT GetDesiredStepPosition(void) {return (DesiredStep);}
    USHORT CheckDiffStep(USHORT toStep);

    static void S_Init();             // checks command array
    static void S_Close();

    void DisableMotorErrorCheck(){IsNeedMotorErrorCheck = false;}
    void EnableMotorErrorCheck(){IsNeedMotorErrorCheck = true;}

    inline  USHORT  GetLiftoff(){return Liftoff;}

    void RunHFOMode();
    void SetFreq(int);
    void SetStrokeVol(int);
    void SetOffsetPosition(int);
    void SendtoMotor();
//    QByteArray RecievefromMotor();
//    void setBaudRate(speed_t baud);

    USHORT GetMotorStatus();

    bool CheckMotorInPOST();

    USHORT  Liftoff;
    USHORT  MaxStep;

//    DiscreteOutput *Switch;

    bool sendEnable;
    bool receiveEnable;
//    QByteArray dataToSend;
    char dataReceive[1024];
    int pointer;



    bool enableRamToPosition;

    char GetMotorErrorFlag(){return motorErrorFlag;}
    bool GetMotorCommLostFlag(){return motorCommLostFlag;}
    void SendStatusQuestion(void);
    void CheckMotorStatus(void);

private:
    void SendCommand(uint8_t MotorId, uint8_t address, uint32_t datagram);

    static E_ReturnStatus BuildCommandArray(); // Builds array of commands
    static void CalcCRC3( UCHAR * command );   // Calculates 3-bit CRC value

    bool IsNeedMotorErrorCheck;
    bool FirstTime;
    E_EventCode ErrorID;

    //$COMMON.ATTRIBUTE$
    //    Name:       CurrentStep
    //    Description:Step position acheived by motor controller
    //    Units: None
    //    Range: 0 - 2000
    int CurrentStep;

    //$COMMON.ATTRIBUTE$
    //    Name:        RedundantCount
    //    Description: Counts the consecutive commands to the same
    //          step position.
    //    Units: None
    //    Range: N/A
    LONG   RedundantCount;

    //$COMMON.ATTRIBUTE$
    //    Name:       DesiredStep
    //    Description:step position to command step motor to.
    //    Units: step position
    //    Range: 0 - 2000
    USHORT DesiredStep;


    //$COMMON.ATTRIBUTE$
    //    Name:        CSISelect
    //    Description: Clocked Serial Interface device select ID
    //    Units: None
    //    Range: 0-6
    UCHAR CSISelect;

    E_ValveID ValveID;

    int Freq;
    int OffsetPosition;
    int StrokeVol;
    int OffsetCurPos;
    int T,t;

    int fd_stepmotor;
    char motorErrorFlag;
    bool motorCommLostFlag;
    int counter;
    int counterMotorError;
    int counterMotorCommLost;
};
#endif  //STEPMOTO_H

