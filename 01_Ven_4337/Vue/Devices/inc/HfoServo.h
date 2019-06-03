/******************************************************************************/
//$COMMON.H$
//    Filename: HfoServo.h
//    Copyright 1996 InVentive Technologies, Inc. All Rights Reserved.
//
//    Class:  HfoServo
//    This class controls hfo servo
/******************************************************************************/
#ifndef HFOSERVO_H
#define HFOSERVO_H

#include "standard.h"
//#include "Controll.h"
#include "stdint.h"

typedef enum
{
    backward,
    forward
}E_Direction;

class HfoServo //: public Controller
{
public:
    static HfoServo* S_GetInstance(void);

    bool PistonInstalled(void);
    bool MotorIsStopped(void);
    void StopMotor(void);
    void ServoMotor(void);
    void Init(void);
    void Run(void);

    void  ClearStaticVariables(void);
    void  Enable(void);
    void Disable(void);



    void SetSIRequest(E_Bool SIrequest);
    inline E_Bool GetSIRequest(void) {return SIRequest;}
    inline void ResetShiftPosition(void)
    {
        shiftPosition = 0;
        shiftCounter = 0;
    }
    inline int32_t GetCurrentFreq(void){return freqIntegrator;}

    void Reset_20sTimer(void);
    void ResetPIDStatic(void);

    void RequestDoAmpCtr(void);
    void RequestDoSVCtr(void){IsDoAmpCtr = false;}
    void StopDoAmpCtr(void);
    LONG GetCurrentAmp(void){return currentAmp;}
    void SetHummingVON(E_Bool value);
    E_Bool GetHummingVON(void){return isHummingVON;}

protected:
    void	CalculatePhaseAngle(void);
    void	AmplitudeFilter(void);
    void	DirectAmplitudeControl(void);
    void	UpdateControlStatus(void);
    void	HfoPositionFilter(void);
    void	GetCurrentSettings(void);
    void	RunOpenLoop(void);
    void	RunDirectAmpControl(void);
    void	DoTargetMaking(void);
    void	RunOpenLoopBasedOnAmpControl(void);
    void	SetParameterForMotorAtOrgionalPos(void);


private:
    HfoServo(void);

    static HfoServo* S_Instance;


    void SetStatic(void);

    double T,t;

    void DoServo(void);

    bool IsDoAmpCtr;

    int16_t DoPID(void);

    E_Bool isHummingVON;

    USHORT shiftPosition;
    USHORT shiftCounter;
    E_Direction shiftDirection;

    LONG currentAmp;
    E_Bool SIRequest;
    bool servo_asked_for_stop;
    bool stop_the_motor;

    int32_t Dat_Omic_integrator;
    int32_t DAmp_integrator;
    int32_t ZeroOrAmp_integrator1;
    int32_t ZeroOrAmp_integrator2;
    int32_t EHPoint_integrator1;
    int32_t EHPoint_integrator2;

    int32_t integrator1;
    int32_t integrator2;
    int32_t freqIntegrator;
    int32_t dAmpIntegrator;
    int32_t zeroIntegrator1;
    int32_t zeroIntegrator2;

    int32_t	shifting_offset;
    int32_t	DAS_Counter;

    int32_t timeToGetTargetPress;
    int32_t PresAmp_Cmd;
    int32_t Pres_Amp;
    int32_t FPres_Amp;
    int32_t Amp_ByDAC;
    int32_t prevAmp_ByDAC;
    int32_t OpenMtr_Amp;
    int32_t integrator_DAC1;
    int32_t integrator_DAC2;
    int32_t integrator_DAC3;
    int32_t integrator_DAC4;
    int32_t UpperAmpLimit;
    int32_t LowerAmpLimit;

    int32_t CurrentAmp;

    unsigned char Stop_DirectAmpCtrl;


    uint32_t	general_timer;
    uint32_t shift_timer;
    int32_t final_step_for_vol;	/* this is multiplied by the current sine */
    /* value to scale the sine to the correct volume */
    bool restrictor_open;
    int16_t Phase_Angle;
    int16_t shifting_inc;
    int16_t prevStrokeVol;
    bool disconnectFlag;

    int16_t strokeVol;

    int16_t Freq;
    E_Bool prevSI;

    int16_t Phase;
    int16_t ZeroOrAmp;
    int16_t SinPos;
    int32_t HFO_Mposition;
    int SVUpdatecounter;



    int delayAfterSI;//when SI stop, alarm check condition delay 3s
    bool currentSIRequest;
    bool prevSIRequest;
    int numOfSample;
    int sum;

    LONG Amp_integral;
    LONG Amp_derivative;
    LONG prev_Amp_error;

    unsigned int Timer_20s;
    bool finished_20s;
    USHORT delay_afterSI;

};


#endif
