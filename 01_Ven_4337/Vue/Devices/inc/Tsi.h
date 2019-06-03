//******************************************************************************
//$COMMON.H$
//   File Name:  tsi.H
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: TSI_Calibration_Data_Struct - this structure defines the format of
//          the flow sensor calibration data.
//******************************************************************************
#ifndef INC_TSI_H_
#define INC_TSI_H_

#include "CommonTypes.h"

// constants
static const USHORT MAX_COEF = 30;

// These values specify the practical limits for Tcal and Toper read
// from the TSI flow sensor EEPROM.
static const float MIN_TCAL = 0; //1500;     // 15 scaled by 100
static const float MAX_TCAL = 600; //4000;     // 40 scaled by 100
static const float MIN_TOPER = 0; //10000;   // 100 scaled by 100
static const float MAX_TOPER = 600; //45000;   // 450 scaled by 100


// These values specify the limits for the coefficients A, B and C as specified
// in the TSI Mas Flow Transducers Application Notes & Specifications Oct 16, 1997.
// The value for B and Z are smaller than those specified.  These "Practical
// Limits" on B & Z were obtained from TSI on September 18, 1998.

static const float MIN_A = -320; //(-41943040);  // -320 * 2**17
static const float MAX_A =  320; //  41943040;   //  320 * 2**17
static const float MIN_B =  0.04; // 5242;       //  0.04 * 2**17
static const float MAX_B =  80; // 10485760;    //  60 * 2**17
static const float MIN_C =  -3.2; //(-419430);    // -3.2 * 2**17
static const float MAX_C =   3.2; //419430;     //  3.2 * 2**17
static const float MIN_Z =   1; //131072;     //  1 * 2**17
static const float MAX_Z =   5; //655360;     //  5.0 * 2**17

//
// Calibration coefficient structure
//
struct CalibrationCoefficients
{
    float  Vf;
    float  A;
    float  B;
    float  C;
};

//
// TSI Calibration Data Structure
//  (in the format that the TSI Sensor stores the data in EEPROM)
//
struct TSI_Calibration_Data_Struct
{
    USHORT Checksum;
    USHORT ModelNumber;
    ULONG  SerialNumber;
    UCHAR  Revision;
    UCHAR   Day;
    UCHAR   Month;
    UCHAR   Year;
    //ULONG  CalibrationDate;
    float  Tcorr; // TempCorrection;
    float  K;
    float  Z;
    UCHAR  NumberOfAir;
    UCHAR  NumberOfO2;
    CalibrationCoefficients Coefficients[MAX_COEF];
    float  Tcal;
    float  Toper;
};

    //address of general information
    #define SerNumAddr      2
    #define RevisionAddr    6
    #define YearAddr        9
    #define MonthAddr       8
    #define DayAddr         7
    //address of coefficient
    #define Tcor_Addr       10
    #define Tcorr_Divisor   100
    #define K_Addr          11
    #define K_Divisor       100000
    #define Z_Addr          12
    #define Z_Divisor       1000
    #define NumOfAirAddr    14
    #define NumOfO2Addr     15
    #define Tcal_Addr       252
    #define Tcal_Divisor    100
    #define Toper_Addr      254
    #define Toper_Divisor   100
    //address of table
    #define StartAddrVf     16
    #define StartAddrAi     18
    #define StartAddrBi     20
    #define StartAddrCi     22
    //divisor number
    #define Vf_Divisor     1000
    #define Ai_Divisor     100
    #define Bi_Divisor     1       //not use
    #define Ci_Divisor     10000

    //define Vf, Vt chanel
    #define Vf_chanel_O2    5
    #define Vt_chanel_O2    6
    #define Vf_chanel_Air   7
    #define Vt_chanel_Air   8

#endif /* INC_TSI_H_ */
