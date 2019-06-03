#ifndef HFOSERVO_H_
#define HFOSERVO_H_

#include "Standard.h"
#include "Controll.h"
#include "stdint.h"

typedef enum
{
    backward,
    forward
}E_Direction;

class HfoServo: public Controller
{
    public:
        // static instance of HfoServo class
        static HfoServo *S_GetInstance(void);

        //Check status of HFO piston ON or OFF
        E_Bool PistonInstalled(void);

        //Check motor is stop or not
        E_Bool MotorIsStopped(void);

        //let the servo to stop running the motor
        void StopMotor(void);

        //let the servo to start running the motor
        void StartMotor(void);

        //Init for motor and servo request
        void Init(void);

        //servo control motor
        void Run(void);

        //Reset shift position
        void ResetShiftPosition(void);

        //get current frequency of motor
        long GetCurrentFreq(void);

        //Testting 1 for motor
        int32_t CreateSinPosition(int iFrequence);
        int32_t CreateSinPositionForSmallMotor(int iFrequence);

    private:
        //Constructor for HfoServo
        HfoServo();

        //$COMMON.ATTRIBUTE$
        //    Name: S_Instance
        //
        //    Description: Pointer to instance of HfoServo class
        //
        //    Units: None
        //
        //    Range: N/A
        //
        static HfoServo* S_Instance;

        //$COMMON.ATTRIBUTE$
        //    Name: MotorIsStop
        //
        //    Description: Check the status of motor, it stops or not
        //
        //    Units: None
        //
        //    Range: N/A
        E_Bool motorIsStop;

        //$COMMON.ATTRIBUTE$
        //    Name: servoIsAskedForStop
        //
        //    Description: Servo asks motor stop or not
        //
        //    Units: None
        //
        //    Range: N/A
        E_Bool servoIsAskedForStop;

        //$COMMON.ATTRIBUTE$
        //    Name: shiftPosition
        //
        //    Description: position to shift
        //
        //    Units: None
        //
        //    Range: N/A
        USHORT shiftPosition;

        //$COMMON.ATTRIBUTE$
        //    Name: shiftCounter
        //
        //    Description: Counter of shift
        //
        //    Units: None
        //
        //    Range: N/A
        USHORT shiftCounter;

        //$COMMON.ATTRIBUTE$
        //    Name: freqIntegrator
        //
        //    Description: Frequency of motor
        //
        //    Units: None
        //
        //    Range: N/A
        long freqIntegrator;

};

#endif /* HFOSERVO_H_ */
