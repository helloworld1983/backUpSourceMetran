/******************************************************************************/
//$COMMON.H$
//   File Name:  ExtremeHighPressTimer.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class:
//      This file contains the class declaration for the ExtremeHighPressTimer
/******************************************************************************/
#ifndef EXTREMEHIGHPRESSTIMER_H
#define EXTREMEHIGHPRESSTIMER_H

#include "Timer.h"
#define TICKS_PER_SECOND 1000

class ExtremeHighPressTimer : public Timer
{
public:
    //Constructor
    ExtremeHighPressTimer
    (
            int  initialTime,   // The initial number of timer ticks
                                // for timer expiration.
            bool      enabled    // TRUE if timer is initially enabled.

    );
    //Copy constructor
    ExtremeHighPressTimer(ExtremeHighPressTimer& );

    //Set timer expired
    void setTimerExpired(bool state);

    //Get timer expired
    bool getTimerExpired(void);

    //Set auto reset
    inline void SetIsProcessAutoReset(bool state){isProcessAutoReset = state;}

    //Initialize class
    void initialize(void);
protected:
    // routine to call upon expiration.
    void ExpirationRoutine(void);
private:

    //$COMMON.ATTRIBUTE$
    //    Name: isExpired
    //    Description: Check timer is expired or not
    //    Units:  LONG
    //    Range:
    bool isExpired;

    //$COMMON.ATTRIBUTE$
    //    Name: isProcessAutoReset
    //    Description: Check auto reset state is active or not
    //    Units:  LONG
    //    Range:
    bool isProcessAutoReset;
};

#endif // NORMBREATHINGTIMER_H
