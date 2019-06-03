//******************************************************************************//
//$COMMON.CPP$
//   File Name:  stepmoto.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: This file contains the Step Motor class 
//
//   Interfaces:
//      ClockedSerialInterface
//
//   Restrictions:  None
//
//   Revision History:
//
//      Rev:  Date:     Engineer:           Project:
//      1.0   01/29/97  Dan Seibert         Morph
//      Description: Initial Revision
//
//******************************************************************************
#define STEPMOTO_CPP
#include "math.h"
#include "Stepmoto.h"

#include <stdio.h>
#include <unistd.h>
#include "flowsens.h"
//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: StepMotor()
//
//    Processing: This is the constructor for the StepMotor class.
//
//    Input Parameters:  selectid - CSI device select ID#
//    Output Parameters: none
//    Return Values:    ptr to StepMotor object
//    Pre-Conditions:   none
//    Miscellaneous:    none
//    Requirements:
//
//******************************************************************************
StepMotor::StepMotor(UCHAR selectid,CHAR* serialPort, USHORT liftoff, USHORT maxstep, LONG maxflow, E_ValveID Id)
{
//    this->fd_stepmotor = SAIO_InitPort(serialPort);
    Liftoff = liftoff;
    MaxStep = maxstep;
    RedundantCount = 0;
    motorErrorFlag = 0;
    motorCommLostFlag = false;
    counter = 0;
    CSISelect = selectid;
    CurrentStep = 0;
    pointer = 0;
    Freq = 0;
    OffsetPosition = 0;
    StrokeVol = 0;
    OffsetCurPos = 0;
    T = t = 0;
    enableRamToPosition = false;
    DesiredStep=0;

    counterMotorError = 0;
    counterMotorCommLost = 0;

    sendEnable = false;
    receiveEnable = false;
    ValveID = Id;
//    dataToSend.clear();

    IsNeedMotorErrorCheck = true;
    FirstTime = true;

//    Switch = new DiscreteOutput(eGate, Digitali2317, CSISelect);
//    ASSERTION( Switch != NULL );
    switch(Id)
    {
    case eO2Valve:
        ErrorID = eCbitO2MotorError;
        break;
    case eAirValve:
        ErrorID = eCbitAirMotorError;
        break;
    case eExhValve:
        ErrorID = eCbitExhMotorError;
        break;
    case eHfoValve:
//        ErrorID = eCbitHfoMotorError;
        break;
    default:
        ErrorID = eEmptyRecord;
        break;
    }
}
StepMotor::~StepMotor()
{
//    delete Switch;
    if(fd_stepmotor >= 0)
    {
        close(fd_stepmotor);
    }
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: SetStepPosition()
//
//    Processing: This routine commands the motor controller to move the
//    valve to the desired step position.  In order to save throughput in
//    Normal mode, the commanded step position is compared against the 
//    previous commanded position.  If the commanded position has been the
//    same for the last three times, the step position is not sent to the
//    motor again until the commanded position changes.
//
//    In Diagnostics mode, the commaned step position is sent every time.
//    This ensures that the Hardware screen updates the step position display
//    on a continuous basis.
//
//    Input Parameters:  step - desired step position.
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:
//      This code is mapped to the v851 on-chip ROM.
//      This code cannot be modified and still run on existing CPU boards.
//
//    Miscellaneous:  Unchanged commanded positions are sent three times,
//    so that the response from the command can be fully received by the
//    system.
//
//    Requirements:
//
//******************************************************************************
void StepMotor::SetStepPosition(int step)
{
    // check upper boundary
//    static int min = 2000;
//    static int max = 0;
//    if (step > MaxStep)
//    {
//        step = MaxStep;
//    }

//    CurrentStep = step;
//    step = -step;

//    if(this == HfoValve)
//    {
//        if(CurrentStep > max)
//            max = CurrentStep;
//        if(CurrentStep < min)
//            min = CurrentStep;
//    }

//    char position[4];
//
//    if(counter == 0)
//        position[0] = 0x81;
//    else
//        position[0] = 0x82;
//    position[1] = (char)(step & 0x7f);
//    position[2] = (char)((step >> 7) & 0x7f);
//    position[3] = (position[0] + position[1] + position[2]) & 0x7f;
//    ::write(this->fd_stepmotor, position,4);

//    counter++;
//    if(counter == 250)
//    {
//        if(IsNeedMotorErrorCheck)
//            CheckMotorStatus();
//    }
//    if(counter >= 500)
//    {
//        counter = 0;
//        min = 2000;
//        max =0;
//    }


}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: GetStepPosition()
//
//    Processing: This routine returns the last step position
//      reported by the stepper motor controller.
//
//    Input Parameters:  None
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:    None
//    Miscellaneous:
//      Motor Position returned is actually + 100 steps.
//    Requirements:
//
//******************************************************************************
int StepMotor::GetStepPosition()
{
    return CurrentStep;
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: S_Init()
//
//    Processing: Checkout S_CommandArray in flash
//                If is corrupted, Build a new one.
//
//    Input Parameters:  None
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:    None
//    Miscellaneous:     None
//    Requirements:
//
//******************************************************************************
void StepMotor::S_Init()
{
//    O2Valve = new StepMotor(0,"/dev/ttyUSB0",LIFTOFF_OFFSET,2000,100*SCALE, eO2Valve);
//    O2Valve->Liftoff = systemConfigFile->getValue(eO2ValveLiftOff);
//
//    AirValve =  new StepMotor(1,"/dev/ttyUSB1",LIFTOFF_OFFSET,2000,100*SCALE, eAirValve);
//    AirValve->Liftoff = systemConfigFile->getValue(eAirValveLiftOff);
//
//    ExhValve = new StepMotor(2,"/dev/ttyUSB2",LIFTOFF_OFFSET,MAX_STEP_POSITION,100*SCALE, eExhValve);
//
//    HfoValve = new StepMotor(3,"/dev/ttyUSB3",LIFTOFF_OFFSET,MAX_STEP_POSITION,100*SCALE, eHfoValve);
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: StepMotor()
//
//    Processing:
//******************************************************************************
USHORT StepMotor::CheckDiffStep(USHORT toStep)
{
    if(toStep < DesiredStep)
        return toStep;
    else
        return DesiredStep;
}
//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: Close()
//
//    Processing:
//******************************************************************************
void StepMotor::Close()
{
    ::close(fd_stepmotor);
}
//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: S_Close()
//
//    Processing:
//
//    Input Parameters:  None
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:    None
//    Miscellaneous:     None
//    Requirements:
//
//******************************************************************************
void StepMotor::S_Close()
{
//    AirValve->Close();
//    O2Valve->Close();
//    ExhValve->Close();
}
//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: SendStatusQuestion()
//
//    Processing:
//
//    Input Parameters:  None
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:    None
//    Miscellaneous:     None
//    Requirements:
//
//******************************************************************************
void StepMotor::SendStatusQuestion()
{
//    ::tcflush(this->fd_stepmotor,TCIOFLUSH);
//    dataToSend = QString("?99").toAscii();
//    dataToSend[dataToSend.size()] = 0x0d;
//    SendtoMotor();
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: CheckMotorStatus()
//
//    Processing:
//
//    Input Parameters:  None
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:    None
//    Miscellaneous:     None
//    Requirements:
//
//******************************************************************************
void StepMotor::CheckMotorStatus()
{
//    int i = 0;
//    char MotorResponse[5];
//    char checkSum;
//    i = read(this->fd_stepmotor,MotorResponse,5);
//    ::tcflush(this->fd_stepmotor,TCIOFLUSH);
//    if(i>= 5)
//    {
//
//        if((MotorResponse[0]&0x000000ff) == 0x91)
//        {
//            checkSum = (MotorResponse[0] + MotorResponse[1] + MotorResponse[2] + MotorResponse[3]) & 0x7f;
//            if(checkSum == MotorResponse[4])
//            {
//
//                if(MotorResponse[3] != 0)
//                {
//                    counterMotorError++;
//                    if(counterMotorError>5)
//                    {
//                        motorErrorFlag = (short)MotorResponse[3];
//                        counterMotorError = 0;
//                        ErrorHandler::S_GetInstance()->LogEvent(ErrorID, eSTR_LOG_MotorError,
//                                                                QString::number(MotorResponse[0]&0x000000ff)
//                                                                + "  "
//                                                                + QString::number(MotorResponse[1]&0x000000ff)
//                                                                + "  "
//                                                                + QString::number(MotorResponse[2]&0x000000ff)
//                                                                + "  "
//                                                                + QString::number(MotorResponse[3]&0x000000ff)
//                                                                + "  "
//                                                                + QString::number(MotorResponse[4]&0x000000ff)
//                                                                );
//                    }
//                }
//                else
//                {
//
//                    counterMotorError=0;
//                    motorErrorFlag = 0;
//                }
//
//            }
//        }
//
//        motorCommLostFlag = false;
//        counterMotorCommLost = 0;
//
//    }
//    else if(i<=0)
//    {
//        counterMotorCommLost++;
//        if(counterMotorCommLost > 5)
//        {
//            motorCommLostFlag = true;
//            counterMotorCommLost = 0;
//        }
//
//    }
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: RampToPosition(USHORT position)
//
//    Processing:
//
//    Input Parameters:  None
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:    None
//    Miscellaneous:     None
//    Requirements:
//
//******************************************************************************
void StepMotor::RampToPosition(int position, E_Bool fastFlag)
{
//    int step = 5;
//    if(this == HfoValve)
//        step = 5;

//    if (fastFlag)
//        step = 35;
//
//    if ( position > CurrentStep)
//    {
//        CurrentStep+=step;
//
//        if(CurrentStep > position)
//            CurrentStep =position;
//
//        SetStepPosition(CurrentStep);
//    }
//    else if (position < CurrentStep)
//    {
//        CurrentStep= CurrentStep - step;
//
//        if(CurrentStep < position)
//            CurrentStep = position;
//
//        SetStepPosition(CurrentStep);
//    }
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: RunHFOMode()
//
//    Processing:
//
//    Input Parameters:  None
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:    None
//    Miscellaneous:     None
//    Requirements:
//
//******************************************************************************
void StepMotor::RunHFOMode()//in standby mode
{

//    int position = 0;
//    if (OffsetCurPos < OffsetPosition)
//        OffsetCurPos++;
//    else if (OffsetCurPos > OffsetPosition)
//        OffsetCurPos--;
//
//    T = 1.0/(double)Freq*1000; //(ms)
//    if(t>=T)
//        t=0;
//    position =((double)(StrokeVol)/2.0)*sin((2.0*PI*t)/T) +
//            (double)(StrokeVol)/2.0 + OffsetCurPos;
//
//
//    this->SetStepPosition(position);
//    t=t+2;

    return;
}
//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: SetFreq(int value)
//
//    Processing:
//
//    Input Parameters:  None
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:    None
//    Miscellaneous:     None
//    Requirements:
//
//******************************************************************************
void StepMotor::SetFreq(int value)
{
    Freq = value;
}
//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: SetStrokeVol(int value)
//
//    Processing:
//
//    Input Parameters:  None
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:    None
//    Miscellaneous:     None
//    Requirements:
//
//******************************************************************************
void StepMotor::SetStrokeVol(int value)
{
    StrokeVol = value;
}
//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: SetOffsetPosition(int value)
//
//    Processing:
//
//    Input Parameters:  None
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:    None
//    Miscellaneous:     None
//    Requirements:
//
//******************************************************************************
void StepMotor::SetOffsetPosition(int value)
{
    OffsetPosition = value;
}
//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: SendtoMotor()
//
//    Processing:
//
//    Input Parameters:  None
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:    None
//    Miscellaneous:     None
//    Requirements:
//
//******************************************************************************
void StepMotor::SendtoMotor()
{
//    if(FirstTime)
//    {
//        ::tcflush(this->fd_stepmotor,TCIOFLUSH);
//        FirstTime = false;
//    }

//    ::write(this->fd_stepmotor,dataToSend,dataToSend.size());
//    dataToSend.clear();
}
//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: RecievefromMotor()
//
//    Processing:
//
//    Input Parameters:  None
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:    None
//    Miscellaneous:     None
//    Requirements:
//
//******************************************************************************
//QByteArray StepMotor::RecievefromMotor()
//{
//    QByteArray data;
//    data.clear();
//    int numofbyte = -1;
//    numofbyte = read(this->fd_stepmotor,dataReceive,1024);
//    if(numofbyte>0)
//    {
//        data.resize(numofbyte);
//        for(int i=0;i<data.size();i++)
//        {
//            data[i] = dataReceive[i];
//        }
//    }
//    return data;
//}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: setBaudRate(speed_t baud)
//
//    Processing:
//
//    Input Parameters:  None
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:    None
//    Miscellaneous:     None
//    Requirements:
//
//******************************************************************************
//void StepMotor::setBaudRate(speed_t baud)
//{
//    SAIO_SetBaudRatePort(baud,this->fd_stepmotor);
//}
