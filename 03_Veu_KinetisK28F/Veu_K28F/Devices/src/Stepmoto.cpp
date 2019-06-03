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

#include "Delay.h"
#include "DebugTrace.h"
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
StepMotor::StepMotor(UCHAR selectid,E_ValveID Id)
{
//    this->fd_stepmotor = SAIO_InitPort(serialPort);
//    Liftoff = liftoff;
//    MaxStep = maxstep;
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
//    Operation Name: Init (void)
//
//    Processing: initialize for motor
//
//    Input Parameters:  None
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:    None
//    Miscellaneous:     None
//    Requirements:
//
//******************************************************************************
void StepMotor::Init (UCHAR MotorID)
{
    //initialize motor

    /*=============SPI_OUT_CONF(Add = R: 0x04 - W: 0x84): = 0x4440006B=================
    +SPI_OUT_LOW_TIME (SPI output transfer idle state duration)                   : 4
    +SPI_OUT_HIGH_TIME (SPI output clock high level duration)                     : 4
    +SPI_OUT_LOW_TIME (SPI output clock low level duration)                       : 4
    +COVER_DATA_LENGTH (Datagram length. Set to 0 for TMCx drivers)               : 0
    +POLL_BLOCK_MULT (Exponent for idle state time between two polling datagrams) : 0
    +disable_polling(No transfer of polling datagrams) = A
    +scale_vale_transfer_en(Transfer of scaling values in scaling values alter) = B
             4 (AB = 00) 2(AB = 10) 4(AB = 01) 6(AB = 11)                         : 6
    +spi_output_format(Selection of connected driver/DAC)                         : B
             A: 26x-SPI, B: 26x-S/D
    */
    SendCommand(MotorID, 0x84,0x4440006B);

    /*=============CLK_FREQ(Add = R: 0x31 - W: 0xB1): = 0x00B71B00====================
    + CLK_FREQ(Frequence): 0x007A1200 - 8Mhz
                           0x00B71b00 - 12Mhz
                           0x00F42400 -16Mhz
                           0x01036640 -17Mhz
    */
    SendCommand(MotorID, 0xB1,0x00B71B00);

    /*==============GENERAL_CONF(Add = R: 0x80 - W: 0x00):= 0x00006026=================
    Direct-a, direct - bow
    */
    SendCommand(MotorID, 0x80,0x00006026);

    /*================REFERENCE_CONF(Add = R: 0x90 - W: 0x01): =0x00010001 ============
    StepLength
    */
    SendCommand(MotorID, 0x90,0x00010001);

    /*==============COVER_LOW(Add = R: 0xEC - W: 0x6C):= 0xEC,0x00000000================
    DRV_STATUS response and COVER_LOW send

    Using cover datagram to write to DRVCTRL of TMC26x  sendData   (0xEC,0x00000000)
    0x00000000: single edge steps, disable step interpolation - 256 microstep
    */
    SendCommand(MotorID, 0xEC,0x00000000);

    /*==============COVER_LOW(Add = R: 0xEC - W: 0x6C):= 0xEC,0x00091935================
    DRV_STATUS response and COVER_LOW send

    Using cover datagram to write to CHOPCONF of TMC26x        (0xEC,0x00090585)
    tbl=36, standard chopper, HDEC=16, HEND=11, HSTR=1, TOFF=5, RNDTF=off
    */
    SendCommand(MotorID, 0xEC,0x00091935);

    /*==============COVER_LOW(Add = R: 0xEC - W: 0x6C):= 0xEC,0x000A0000================
    DRV_STATUS response and COVER_LOW send

    Using cover datagram to write to SMARTEN of TMC26x (0xEC,0x000A0000)
    tbl=36, standard chopper, HDEC=16, HEND=11, HSTR=1, TOFF=5, RNDTF=off
    */
    SendCommand(MotorID, 0xEC,0x000A0000);

    /*==============COVER_LOW(Add = R: 0xEC - W: 0x6C):= 0xEC,0x000D05xx================
    DRV_STATUS response and COVER_LOW send

    Using cover datagram to write to SGCSCONF of TMC26x (0xEC,0x000D05xx)
    Disable Smarten Register, SGT=0,CS=24
    */
    if(MotorID == 1)
    {
        // Disable Smarten Register
        SendCommand(MotorID, 0xEC,0x000D0519); // Using cover datagram to write to SGCSCONF of TMC26x 06 for big motor                   (0xEC,0x000C0018)
    }
    else
    {
        // Disable Smarten Register
        SendCommand(MotorID, 0xEC,0x000D0506); // Using cover datagram to write to SGCSCONF of TMC26x 06 for small motor                   (0xEC,0x000C0018)
    }

    /*==============COVER_LOW(Add = R: 0xEC - W: 0x6C):= 0xEC,0x000EF040==============
    DRV_STATUS response and COVER_LOW send

    Using cover datagram to write to DRVCONF of TMC26x     (0xEC,0x000EF000)
    SLPH = 3, SLPL = 3, DISS2G = off, TS2G = 0-3.2µs, SDOFF = on, VSENSE = 0
    */
    SendCommand(MotorID, 0xEC,0x000EF040); // Using cover datagram to write to DRVCONF of TMC26x                     (0xEC,0x000EF000)

    /*==============VMAX(Add = R: 0x24 - W: 0xA4):= 0xA4,0x00000000================
    Actual ramp generator position- x = 0
    */
    SendCommand(MotorID, 0xA4,0x00000000);

    /*==============XACTUAL(Add = R: 0x21 - W: 0xA1):= 0xA4,0x00000000================
    Target ramp generator velocity- v = 0
    */
    SendCommand(MotorID, 0xA1,0x00000000);

    /*==============VSTART(Add = R: 0x37 - W: 0xB7):= 0xB7,0x00000000================
    Start velocity of ramp generator- v = 0
    */
    SendCommand(MotorID, 0xB7,0x00000000);

    /*==============RAMPMODE(Add = R: 0x20 - W: 0xA0):= 0xA0,0x00000005================
      bit 2: Ramp_mode
              1 Positioning: XTARGET is superior objective for velocity ramp
              0 Velocitiy mode: VMAX is superior objective for velocity ramp
      bit 1 - 0: Ramp_type
              00 Hold mode: |-|
              01 Trapezoidal ramp /-\
              10 S-shaped ramp /'-'\

    */
    SendCommand(MotorID, 0xA0,0x00000000);

    /*==============VMAX(Add = R: 0x24 - W: 0xA4):= 0xA0, 0x03E80000================
      03E80000 200K
      0x01860000    100k
    */
    SendCommand(MotorID, 0xA4,0x01860000); // VMAX =  [pps] 03E80000 200K / 186a0=100k

    /*==============AMAX(Add = R: 0x28 - W: 0xA8):= 0xA0, 0x00001000================
    */
    SendCommand(MotorID, 0xA8,0x00001000); // AMAX

    /*==============DMAX(Add = R: 0x29 - W: 0xA9):= 0xA0, 0x00001000================
    */
    SendCommand(MotorID, 0xA9,0x00001000); // DMAX

//    TestMotorSPI::S_GetInstance()->sendData2(MotorID, 0xAD,0x00000100); // bow1
//    TestMotorSPI::S_GetInstance()->sendData2(MotorID, 0xAE,0x00000100); // bow2
//    TestMotorSPI::S_GetInstance()->sendData2(MotorID, 0xAF,0x00000100); // bow3
//    TestMotorSPI::S_GetInstance()->sendData2(MotorID, 0xB0,0x00000100); // bow4
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

void StepMotor::SendCommand (uint8_t MotorId, uint8_t address,
                             uint32_t datagram)
{
    //TMC4361 takes 40 bit data: 8 address and 32 data
     uint8_t i_datagram;

     switch (MotorId)
     {
         case 1:
//             Chip_GPIO_WritePortBit(LPC_GPIO_PORT,CS1_GPIO_PORT, CS1_GPIO_PIN, false);
             break;

         case 2:
//             Chip_GPIO_WritePortBit(LPC_GPIO_PORT,CS2_GPIO_PORT, CS2_GPIO_PIN, false);
             break;

         case 3:
//             Chip_GPIO_WritePortBit(LPC_GPIO_PORT,CS3_GPIO_PORT, CS3_GPIO_PIN, false);
             break;

         case 4:
//             Chip_GPIO_WritePortBit(LPC_GPIO_PORT,CS4_GPIO_PORT, CS4_GPIO_PIN, false);
             break;

         default:
             break;
     }
     delay_ms(1);

     //send address byte
//     Chip_SSP_WriteFrames_Blocking(LPC_SSP, &address, 1);

     //send datagram
//     i_datagram = (uint8_t)((datagram >> 24) & 0xff);
//     Chip_SSP_WriteFrames_Blocking(LPC_SSP, &i_datagram, 1);
//     DEBUG_HFO("%x ", i_datagram);

//     i_datagram = (uint8_t)((datagram >> 16) & 0xff);
//     Chip_SSP_WriteFrames_Blocking(LPC_SSP, &i_datagram, 1);
//     DEBUG_HFO("%x ", i_datagram);

//     i_datagram = (uint8_t)((datagram >> 8) & 0xff);
//     Chip_SSP_WriteFrames_Blocking(LPC_SSP, &i_datagram, 1);
//     DEBUG_HFO("%x ", i_datagram);

//     i_datagram = (uint8_t)(datagram & 0xff);
//     Chip_SSP_WriteFrames_Blocking(LPC_SSP, &i_datagram, 1);
//     DEBUG_HFO("%x \n", i_datagram);

     switch (MotorId)
     {
         case 1:
//             Chip_GPIO_WritePortBit(LPC_GPIO_PORT,CS1_GPIO_PORT, CS1_GPIO_PIN, true);
             break;

         case 2:
//             Chip_GPIO_WritePortBit(LPC_GPIO_PORT,CS2_GPIO_PORT, CS2_GPIO_PIN, true);
             break;

         case 3:
//             Chip_GPIO_WritePortBit(LPC_GPIO_PORT,CS3_GPIO_PORT, CS3_GPIO_PIN, true);
             break;

         case 4:
//             Chip_GPIO_WritePortBit(LPC_GPIO_PORT,CS4_GPIO_PORT, CS4_GPIO_PIN, true);
             break;

         default:
             break;
     }
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
