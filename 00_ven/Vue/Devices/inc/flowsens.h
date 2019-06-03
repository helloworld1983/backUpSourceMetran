//******************************************************************************
//$COMMON.H$
//   File Name:  FLOWSENS.H
//   Copyright 2012 MeTranVietNam Co. All Rights Reserved.
//
//    Class:  	FlowSensor
//	  According to 840101/840102 Mass Flow Transducers 1980350SP_F.doc: DOCUMENT
//    This class provides an interface to the TSI Mass Flow Sensor type: 840101/840102
//
//   Revision History:
//
//******************************************************************************
#ifndef    FLOWSENS_H
#define    FLOWSENS_H

#include "sensor.h"
#include "Tsi.h"
#include "util.h"

// Sensor Type field
enum E_FlowType
{
    eAir,
    eOxygen
};

class FlowSensor
{
public:

    FlowSensor(E_DeviceId  id,          // Sensor Id #
               UCHAR flowaddr,          // A2D Channel for flow signal
               UCHAR tempaddr,          // A2D Channel for temp signal
               UCHAR csiaddr,           // EEPROM CSI chip select
               E_FlowType type       // Oxygen/Air
    );

    void ConfigSPI(void);
    E_Bool IsFlowSensorError(void){return FlowSensorError;}
    unsigned char GetOneByte(unsigned char address);
    void GetAllDataFromEEPROM(void);
    void WaitForSPI(void);
    bool Checksum (void);	//check CRC for the data from TSI EEPROM
    bool CheckModel(void);
    void GetTSIGeneralData(void);
    void BuildCoeff(void);
    float GetBi (unsigned char *StartAdd);
    void BuildVfTable(void);
    void BuildAiTable(void);
    void BuildBiTable(void);
    void BuildCiTable(void);
    unsigned char figure_index(float Vfstd);
    signed char figure_T(float VoltTem);

    LONG   GetCurrentReading(void);         // Obtains reading from sensor
    LONG   GetLastReading(void);            // Returns last valid reading

    void   SetTemperatureFactor(void);      // Reads temp AI signal
    LONG   GetCompensatedReading(void);     // Adjusts current reading for altitude

private:
    float calculate_flow(void);			//get current flow

    E_Bool FlowSensorError;
    //$COMMON.ATTRIBUTE$
    //    Name:        TempAddr
    //    Description: Temperature input signal of flow transducer
    //    Units: None
    //    Range: N/A
    UCHAR TempAddr;

    //$COMMON.ATTRIBUTE$
    //    Name:        FlowAddr
    //    Description: Flow input signal of flow transducer
    //    Units: None
    //    Range: N/A
    UCHAR FlowAddr;

    //$COMMON.ATTRIBUTE$
    //    Name:        CurrentReading
    //    Description: Holds current reading in engineering units.
    //    Units: ml/sec * SCALE
    //    Range: 0 - 333333
    LONG   CurrentReading;

    //$COMMON.ATTRIBUTE$
    //    Name:         CalibrationData
    //    Description:  Structure holding TSI calibration data
    //    Units: None
    //    Range: N/A
    struct TSI_Calibration_Data_Struct CalibrationData;

    //$COMMON.ATTRIBUTE$
    //    Name:        CSISelect
    //    Description: Clocked Serial Interface device select id
    //    Units: None
    //    Range: 0 - 7
    UCHAR  CSISelect;     // EEPROM chip select

    //$COMMON.ATTRIBUTE$
    //    Name:        Type
    //    Description: Denotes whether this flow sensor is used for
    //          air or oxygen.
    //    Units: None
    //    Range: N/A
    E_FlowType Type;

    unsigned char RawTSIData[256];
};


//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: GetLastReading()
//
//    Processing:  Returns value of CurrentReading attribute
//
//    Input Parameters: None
//    Output Parameters:None
//    Return Values:    Current flow reading in ml/sec * SCALE
//    Pre-Conditions:   None
//    Miscellaneous:    None
//    Requirements:
//
//******************************************************************************

inline LONG FlowSensor::GetLastReading(void)
{
    // Copy current reading to address specified
    return (CurrentReading);
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: SetTemperatureFactor()
//
//    Processing:  This method reads the Temperature input signal.
//      It then uses the Counts value as an index into a table of 
//      pre-calculated temperature compensation factors which are used
//      as part of the flow rate calculation in the GetCurrentReading() method.
//
//    Input Parameters:     None
//    Output Parameters:    None
//    Return Values:        None
//    Pre-Conditions:       None
//    Miscellaneous:        None
//    Requirements:
//
//******************************************************************************

inline void FlowSensor::SetTemperatureFactor(void)
{
    return;
}

//
//  CONSTANTS
//

#ifdef FLOWSENS_CPP
//$COMMON.ATTRIBUTE$
//    Name:        	Temperature table 
//    Description: 	Used for finding out the temperature based on temperature voltage
//    Units: 		C degree
//    Range: 		From -20 to 100

static const signed char T_table[121]={
        -20,
        -19,
        -18,
        -17,
        -16,
        -15,
        -14,
        -13,
        -12,
        -11,
        -10,
        -9,
        -8,
        -7,
        -6,
        -5,
        -4,
        -3,
        -2,
        -1,
        0,
        1,
        2,
        3,
        4,
        5,
        6,
        7,
        8,
        9,
        10,
        11,
        12,
        13,
        14,
        15,
        16,
        17,
        18,
        19,
        20,
        21,
        22,
        23,
        24,
        25,
        26,
        27,
        28,
        29,
        30,
        31,
        32,
        33,
        34,
        35,
        36,
        37,
        38,
        39,
        40,
        41,
        42,
        43,
        44,
        45,
        46,
        47,
        48,
        49,
        50,
        51,
        52,
        53,
        54,
        55,
        56,
        57,
        58,
        59,
        60,
        61,
        62,
        63,
        64,
        65,
        66,
        67,
        68,
        69,
        70,
        71,
        72,
        73,
        74,
        75,
        76,
        77,
        78,
        79,
        80,
        81,
        82,
        83,
        84,
        85,
        86,
        87,
        88,
        89,
        90,
        91,
        92,
        93,
        94,
        95,
        96,
        97,
        98,
        99,
        100
};

//	  $COMMON.ATTRIBUTE$
//    Name:        	Flow table 
//    Description: 	Voltage value, the according value from this table will map to Temperature table above
//    Units: 		Volt
//    Range: 		From 9.197 to 0.529

static const float Vt_table[121]={
        9.197,
        9.149,
        9.100,
        9.048,
        8.994,
        8.938,
        8.879,
        8.818,
        8.755,
        8.689,
        8.621,
        8.551,
        8.478,
        8.402,
        8.324,
        8.244,
        8.161,
        8.076,
        7.988,
        7.899,
        7.806,
        7.712,
        7.615,
        7.516,
        7.415,
        7.313,
        7.208,
        7.101,
        6.993,
        6.883,
        6.772,
        6.659,
        6.546,
        6.431,
        6.315,
        6.198,
        6.080,
        5.962,
        5.844,
        5.725,
        5.606,
        5.488,
        5.369,
        5.250,
        5.133,
        5.015,
        4.898,
        4.782,
        4.667,
        4.553,
        4.440,
        4.328,
        4.218,
        4.108,
        4.001,
        3.895,
        3.790,
        3.687,
        3.586,
        3.487,
        3.390,
        3.294,
        3.201,
        3.109,
        3.019,
        2.931,
        2.845,
        2.762,
        2.680,
        2.600,
        2.522,
        2.446,
        2.372,
        2.300,
        2.230,
        2.162,
        2.095,
        2.031,
        1.968,
        1.907,
        1.848,
        1.791,
        1.735,
        1.681,
        1.629,
        1.578,
        1.529,
        1.481,
        1.435,
        1.390,
        1.346,
        1.304,
        1.263,
        1.224,
        1.186,
        1.149,
        1.113,
        1.078,
        1.045,
        1.012,
        0.981,
        0.950,
        0.921,
        0.892,
        0.865,
        0.838,
        0.813,
        0.788,
        0.764,
        0.740,
        0.718,
        0.696,
        0.675,
        0.654,
        0.635,
        0.616,
        0.597,
        0.579,
        0.562,
        0.545,
        0.529
}; 

#endif    //FLOWSENS_CPP

#endif  //FLOWSENS_H

