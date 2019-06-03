/******************************************************************************/
//$COMMON.H$
//   File Name:  Exponential.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: 
//      The Exponential class is declared in this file.  The exponential is
//      used for determining pressure and flow profiles.
/******************************************************************************/
#ifndef EXPONENTIAL_H
#define EXPONENTIAL_H
#include "CommonConstants.h"

// All data items in the Exponential data table are scaled by 2**6
const LONG EXP_DATA_SF = 64;

const LONG EXP_DATA_SHIFT = 6;

/******************************************************************************/
//      CLASS DECLARATIONS
/******************************************************************************/
class Exponential
{
    public:
        Exponential (void);
        static LONG GetExpTerm (UNSIGNED riseTime, UNSIGNED elapsedTime);

    protected:

    private:
        Exponential (const Exponential&);    // copy constructor not used

};

#endif // EXPONENTIAL_H

