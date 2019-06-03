//******************************************************************************//
//$COMMON.CPP$
//   File Name:  FLOWSENS.CPP
//   Copyright 2012 MeTranVietNam Co. All Rights Reserved.
//
//   Purpose: 	This file contains the FlowSensor class which
//          	represents a TSI Flow Sensor.
//
//   Interfaces:
//          BinarySearch - utility routine
//          interpolate  - utility routine
//          ipow         - utility routine
//
//   Restrictions:
//          Flow sensor input signals are assumed to have range of 0-5 Volts.
//          A2D output range for the input signals is assumed to be 0 - 4095.
//******************************************************************************
#include "flowsens.h"
#include <math.h>

#include "util.h"
#include "Errorhan.h"

#include "DebugTrace.h"

/* Air flow sensor model number. */
static const USHORT AIR_MODEL_NUM = 101;

/* O2 flow sensor model number. */
static const USHORT O2_MODEL_NUM = 102;

// Number of bytes read in the header portion of the EEPROM for the flow sensor.
// This number includes the checksum up to the coefficient data.
static const UCHAR EEPROM_HDR_BYTES = 18; 

/* Maximum flow reading in LPM, scaled by 2**8; 300 LPM */
static const LONG MAX_READING_IN_SCALED_LPM = 76800;

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: FlowSensor()
//
//    Processing: This is the constructor for the flow sensor class.
//
//    Input Parameters:
//          flowaddr:  A2D channel # of the flow signal.
//          tempaddr:  A2D channel # of the temperature signal.
//          csiaddr :  CSI select # of the calibration data EEPROM
//          type    :  air or oxygen
//
//    Output Parameters: None
//    Return Values:     ptr to FlowSensor object.
//    Pre-Conditions:    None
//    Miscellaneous:     None
//    Requirements:
//
//******************************************************************************

FlowSensor::FlowSensor(
    E_DeviceId  id,
    UCHAR flowaddr,
    UCHAR tempaddr,
    UCHAR csiselect,
    E_FlowType type)
{
    CalibrationData.Checksum = 0;               // edit
    CalibrationData.ModelNumber = 0;
    CalibrationData.SerialNumber = 0;
    CalibrationData.Revision = 0;
    CalibrationData.Day = 0;
    CalibrationData.Month = 0;
    CalibrationData.Year = 0;
    CalibrationData.Tcorr = 0;
    CalibrationData.K = 0;
    CalibrationData.Z = 0;

    for(int i = 0;i<MAX_COEF;i++ )
    {
        CalibrationData.Coefficients[i].Vf = 0;
        CalibrationData.Coefficients[i].A = 0;
        CalibrationData.Coefficients[i].B = 0;
        CalibrationData.Coefficients[i].C = 0;

    }

    CalibrationData.Tcal = 0;
    CalibrationData.Toper = 0;

    CalibrationData.NumberOfAir = 0;
    CalibrationData.NumberOfO2 = 0;

    //check input address
    ASSERTION( flowaddr < 15 );
    ASSERTION( tempaddr < 15 );
    
    TempAddr = tempaddr;
    FlowAddr = flowaddr;
    CSISelect = csiselect;
    FlowSensorError = eTrue;
    // Assign sensor type
    Type = type;

    CurrentReading = 0;
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: ConfigSPI()
//
//    Processing: 	configure the SPI interface to communicate with
//					all TSI flow sensor
//
//    Input Parameters: None
//    Output Parameters:None
//    Return Values:    None 
//    Pre-Conditions:   None
//    Miscellaneous:    None
//    Requirements:  
//
//******************************************************************************

void FlowSensor::ConfigSPI (void)
{
//    unsigned char reg_in;
    //clear SPI register to communicate with TSI Air sensor
//    OUTP(SPIADDR, 0x00);
//
//    reg_in = SPI_CLK_0 | SPI_FRAME_24BIT | SPI_AUTO_SS | CSISelect;
//
//    OUTP(SPICON1, reg_in);
//
//    reg_in = SPI_SCLK_2MHZ | SPI_SHIFT_MSB;
//    OUTP(SPISTATUS, reg_in);
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: 	WaitForSPI()
//
//    Processing: 		Wait for SPI interface not busy
//
//    Input Parameters: None
//    Output Parameters:None
//    Return Values:    None
//    Pre-Conditions:   None
//    Miscellaneous:    None
//    Requirements:  
//
//******************************************************************************
void FlowSensor::WaitForSPI (void)
{
    bool busy = true;
    unsigned char reg_out;
    while(busy)
    {
//        INP(reg_out, SPISTATUS);
        //          printf("the value of spistatus is: %x h \n", reg_out);
        reg_out = reg_out & 0x01;
        if(reg_out == 0)
            busy = false;
    }
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: GetAllDataFromEEPROM()
//
//    Processing: 	Get all data from EEPROM of TSI Flow sensor
//					This data contents coefficients to convert from
//					temperature voltage and flow voltage to Flow
//
//    Input Parameters: None
//    Output Parameters:None
//    Return Values:    None
//    Pre-Conditions:   None
//    Miscellaneous:    None
//
//    Requirements:  
//
//******************************************************************************
void FlowSensor::GetAllDataFromEEPROM (void)
{
    ConfigSPI();
    //unsigned char data;
    for (unsigned int i=0; i<256; i++){
        RawTSIData[i] = GetOneByte(i);
    }
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: GetOneByte()
//
//    Processing: 	Get the data at input address
//
//    Input Parameters: the address need to get data
//
//    Return Values:     data at input address
//
//    Pre-Conditions:    None
//    Miscellaneous:     None
//
//    Requirements:  
//
//******************************************************************************
unsigned char FlowSensor::GetOneByte(unsigned char address)
{
    unsigned char value = 0;
    //transmit command and address
//    OUTP(SPIDATA2, address);    //address
//    OUTP(SPIDATA3, OPREAD_256); //read command
    //wait until finish transaction
    WaitForSPI();
    //get data from buffer
//    INP(value, SPIDATA1);   //lower byte
    //return value
    return value;
}


//$COMMON.OPERATION$
//    Operation Name: Checksum()
//
//    Processing: 		Check error after download data from EEPROM of TSI sensor
//
//    Input Parameters: None
//
//	  Output Paramerters: None
//
//    Return Values:     Checking status:	eError: has any error
//											eSuccess: has no error
//    Pre-Conditions:    None
//    Miscellaneous:     None
//
//    Requirements:  
//
//******************************************************************************
bool FlowSensor::Checksum (void)
{	//check CRC for the data from TSI EEPROM
    unsigned int sum;
    unsigned int i;
    unsigned int sumCode;
    
    //get sum code
    sumCode = RawTSIData[0]*256 + RawTSIData[1];

    //calculate sum code
    sum = 0;
    for (i = 2; i < 256; i++){
        sum += RawTSIData[i];
    }
    //compare
    if(sum == sumCode) {
        CalibrationData.Checksum = sumCode;
        FlowSensorError = eFalse;
        return true; //no error
    }
    else {
        return false;	//error
    }
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: GetTSIGeneralData()
//
//    Processing: 		Generate the general information of TSI sensor
//
//    Input Parameters: None
//
//    Return Values:     Status of the process:
//							eError: can not generate the general information
//							eSuccess: generate successfully
//    Pre-Conditions:    None
//    Miscellaneous:     None
//
//    Requirements:  
//
//******************************************************************************
void FlowSensor::GetTSIGeneralData(void)
{
    //   GeneralInfor WriteGenData;
    unsigned long srn = 0;
    unsigned char i;

    //get Serial Number
    for (i = SerNumAddr; i < SerNumAddr+4; i++){
        srn += RawTSIData[i];
        if (i < SerNumAddr+3)    //do not shift the last value of integer number
            srn = srn << 8;
    }
    CalibrationData.SerialNumber = srn;

    //get and write Revision character
    CalibrationData.Revision = RawTSIData[RevisionAddr];

    //Day, Month, Year
    CalibrationData.Day = RawTSIData[DayAddr];
    CalibrationData.Month = RawTSIData[MonthAddr];
    CalibrationData.Year = RawTSIData[YearAddr];

    //get number of coefficients
    CalibrationData.NumberOfAir = RawTSIData[NumOfAirAddr];
    CalibrationData.NumberOfO2 = RawTSIData[NumOfO2Addr];

}
//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: CheckModel()
//
//    Processing:   Check model
//
//    Input Parameters: None
//
//    Return Values:     None
//
//    Pre-Conditions:    None
//    Miscellaneous:     None
//
//    Requirements:
//
//******************************************************************************
bool FlowSensor::CheckModel(void)
{
    bool rtn = false;
    int model = CalibrationData.SerialNumber/10000000;

    if(Type == eAir)
    {
        if((model==105)||(model==101))    //101 is model number of air flow sensor
        {
            rtn = true;
            FlowSensorError = eFalse;
        }
        else
            FlowSensorError = eTrue;
    }
    else
    {
        if((model==106)||(model==102))    //101 is model number of oxygen flow sensor
        {
            rtn = true;
            FlowSensorError = eFalse;
        }
        else
            FlowSensorError = eTrue;
    }
    return rtn;
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: BuildCoeff()
//
//    Processing: 	Get the data at input address
//
//    Input Parameters: the address need to get data
//
//    Return Values:     data at input address
//
//    Pre-Conditions:    None
//    Miscellaneous:     None
//
//    Requirements:  
//
//******************************************************************************
void FlowSensor::BuildCoeff(void)
{
    //Tcor
    signed char Tcorr_temp;
    Tcorr_temp = RawTSIData[Tcor_Addr];
    CalibrationData.Tcorr = (float)Tcorr_temp / Tcorr_Divisor;

    // K
    unsigned char K_temp;
    K_temp = RawTSIData[K_Addr];
    CalibrationData.K = (float)K_temp / (float)K_Divisor;

    //Z
    unsigned short Z_temp;
    Z_temp = RawTSIData[Z_Addr]*256 + RawTSIData[Z_Addr+1];
    CalibrationData.Z = (float)Z_temp / Z_Divisor;

    // Tcal
    unsigned short Tcal_temp;
    Tcal_temp = RawTSIData[Tcal_Addr]*256+RawTSIData[Tcal_Addr+1];
    CalibrationData.Tcal = (float)Tcal_temp / Tcal_Divisor;

    // Toper
    unsigned short Toper_temp;
    Toper_temp = RawTSIData[Toper_Addr]*256+RawTSIData[Toper_Addr+1];
    CalibrationData.Toper = (float)Toper_temp / Toper_Divisor;
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: GetBi()
//
//    Processing: 	Get the data at input address
//
//    Input Parameters: the address need to get data
//
//    Return Values:     data at input address
//
//    Pre-Conditions:    None
//    Miscellaneous:     None
//
//    Requirements:  
//
//******************************************************************************
float FlowSensor::GetBi (unsigned char *StartAdd)
{
    //byte1: bit7->bit5: Exponent, bit4 -> bit0: mantissa
    //byte2: Mantissa
    /***************************calucate Exponent*************************************/
    // exp = 10^(e+1)
    unsigned char exp;
    exp = ((*StartAdd)>>5)&0x07; //get bit7 -> bit5
    /***************************calucate Mantissa*************************************/
    //sum = Mantissa
    unsigned short mantissa;
    mantissa = (*StartAdd & 0x1f)*256 + *(StartAdd + 1);
    /*************************** recover the coefficient *******************************/
    //return = Mantissa / 10^(e+1)
    float coeff;
    coeff = (float)mantissa;
    for(char i = 0; i <= exp; i++){
        coeff /= 10;
    }
    return coeff;
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: BuildVfTable()
//
//    Processing: 	Get the data at input address
//
//    Input Parameters: the address need to get data
//
//    Return Values:     data at input address
//
//    Pre-Conditions:    None
//    Miscellaneous:     None
//
//    Requirements:  
//
//******************************************************************************
void FlowSensor::BuildVfTable (void)
{
    //determine what kind of talbe (air or O2) need to create
    unsigned char num;
    unsigned char startAddr;
    unsigned short temp;
    float coeff = 0;

    startAddr = StartAddrVf;

    if(Type == eAir)
        num = CalibrationData.NumberOfAir;
    else
        num = CalibrationData.NumberOfO2;

    //making table
    for(char i = 0; i < num; i++){
        //get 2 bytes that contains the coefficient
        temp = RawTSIData[startAddr+i*8]*256 + RawTSIData[startAddr+i*8+1];
        //devide by "Vf_Divisor"
        coeff = ((float)temp)/Vf_Divisor;
        //write to memmory
//        CalibrationData.Coefficients[i].Vf = coeff;
    }
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: BuildAiTable()
//
//    Processing: 	Get the data at input address
//
//    Input Parameters: the address need to get data
//
//    Return Values:     data at input address
//
//    Pre-Conditions:    None
//    Miscellaneous:     None
//
//    Requirements:  
//
//******************************************************************************
void FlowSensor::BuildAiTable(void)
{
    //determine what kind of talbe (air or O2) need to create
    unsigned char num;
    unsigned char startAddr;
    signed short temp;
    float coeff = 0;

    startAddr = StartAddrAi;

    if(Type == eAir)
        num = CalibrationData.NumberOfAir;
    else
        num = CalibrationData.NumberOfO2;

    //making table
    for(char i = 0; i < num; i++){
        //get 2 bytes that contains the coefficient
        temp = RawTSIData[startAddr+i*8]*256 + RawTSIData[startAddr+i*8+1];
        //devide by "Vf_Divisor"
        coeff = ((float)temp)/Ai_Divisor;
        //write to memmory
//        CalibrationData.Coefficients[i].A = coeff;
    }
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: BuildBiTable()
//
//    Processing: 	Get the data at input address
//
//    Input Parameters: the address need to get data
//
//    Return Values:     data at input address
//
//    Pre-Conditions:    None
//    Miscellaneous:     None
//
//    Requirements:  
//
//******************************************************************************
void FlowSensor::BuildBiTable(void)
{
    //determine what kind of talbe (air or O2) need to create
    unsigned char num;
    unsigned char startAddr;
    unsigned char *current_addr;

    startAddr = StartAddrBi;

    if(Type == eAir)
        num = CalibrationData.NumberOfAir;
    else
        num = CalibrationData.NumberOfO2;

    //making table
    float coeff = 0;
    for(char i = 0; i < num; i++){
        current_addr = &RawTSIData[startAddr + 8*i];
        coeff = GetBi(current_addr);
        //write to memmory
//        CalibrationData.Coefficients[i].B = coeff;
    }
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: BuildCiTable()
//
//    Processing: 	Get the data at input address
//
//    Input Parameters: the address need to get data
//
//    Return Values:     data at input address
//
//    Pre-Conditions:    None
//    Miscellaneous:     None
//
//    Requirements:  
//
//******************************************************************************
void FlowSensor::BuildCiTable(void)
{
    //determine what kind of talbe (air or O2) need to create
    unsigned char num;
    unsigned char startAddr;
    signed short temp;
    float coeff = 0;

    startAddr = StartAddrCi;

    if(Type == eAir)
        num = CalibrationData.NumberOfAir;
    else
        num = CalibrationData.NumberOfO2;

    //making table
    for(char i = 0; i < num; i++){
        //get 2 bytes that contains the coefficient
        temp = RawTSIData[startAddr+i*8]*256 + RawTSIData[startAddr+i*8+1];
        //devide by "Vf_Divisor"
        coeff = ((float)temp)/Ci_Divisor;
        //write to memmory
//        CalibrationData.Coefficients[i].C = coeff;
    }
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: calculate_flow()
//
//    Processing: 	Get the data at input address
//
//    Input Parameters: the address need to get data
//
//    Return Values:     data at input address
//
//    Pre-Conditions:    None
//    Miscellaneous:     None
//
//    Requirements:  
//
//******************************************************************************
float FlowSensor::calculate_flow(void)
{
    float Vf_temp;
    float VoltFlow;
//    float VoltTem;
    float Temp;
    unsigned char index;
    float Q;
    float Vfstd;
    static float A, B, C;
//    static float  Tcorr = CalibrationData.Tcorr;
    static float  K = CalibrationData.K;
    static float  Z = CalibrationData.Z;
    static float  Tcal = CalibrationData.Tcal;
    static float  Toper = CalibrationData.Toper;

    //1. get Vt value
//    VoltTem = AnalogInput->getCurrentValue(TempAddr);	//range +5v for Vt input
    //printf("Vt = %f \n", VoltTem);

    //2. get  Vf value
//    VoltFlow = AnalogInput->getCurrentValue(FlowAddr);
//    printf("Vf = %f \n", VoltFlow);
    
    //3. Figure T value according to Vt
//    Temp = figure_T(VoltTem);
    //printf("T = %f \n", Temp);

    //4. Add the temperature correction
//    Temp = Temp + Tcorr;
    //printf("T = %f \n", Temp);

    //5. Normalize Vf to the standard
    Vf_temp = (Toper-Tcal)/((Toper-Temp)*(1+K*(Temp-Tcal)));
    Vfstd = ((VoltFlow + Z)*sqrt(Vf_temp))-Z;
    
    //6. figure the index of Vf, A, B, C table
    index = figure_index(Vfstd);
    //printf("index = %d \n", index);

    //Calculate mass flow
    A = CalibrationData.Coefficients[index].A;
    B = CalibrationData.Coefficients[index].B;
    C = CalibrationData.Coefficients[index].C;
    
    Q = A + B*Vfstd*Vfstd + B*C*Vfstd*Vfstd*Vfstd*Vfstd*Vfstd;

    //printf("mass flow = %f \n", Q);
    if(Q < 0)
        Q = 0;
    return Q;
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: figure_index()
//
//    Processing: 	Get the data at input address
//
//    Input Parameters: the address need to get data
//
//    Return Values:     data at input address
//
//    Pre-Conditions:    None
//    Miscellaneous:     None
//
//    Requirements:  
//
//******************************************************************************
unsigned char FlowSensor::figure_index(float Vfstd)
{
    unsigned char i, numOfCoeff;
    numOfCoeff = (Type==eAir)?CalibrationData.NumberOfAir:CalibrationData.NumberOfO2;
    
    for (i = 0; i < numOfCoeff; i++){
        if(Vfstd < CalibrationData.Coefficients[i].Vf){
            if(i == 0)
                return i;
            else
                return (i-1);
        }
    }
    return (i-1);
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: figure_T()
//
//    Processing: 	Get the data at input address
//
//    Input Parameters: the address need to get data
//
//    Return Values:     data at input address
//
//    Pre-Conditions:    None
//    Miscellaneous:     None
//
//    Requirements:  
//
//******************************************************************************
signed char FlowSensor::figure_T (float VoltTem)
{
    for (unsigned char i = 0; i < 121; i++)
    {
//        if(2*VoltTem >= Vt_table[i])
//            return T_table[i];
    }
    return 0xff;
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: GetCurrentReading()
//
//    Processing: 	Get the data at input address
//
//    Input Parameters: the address need to get data
//
//    Return Values:     data at input address
//
//    Pre-Conditions:    None
//    Miscellaneous:     None
//
//    Requirements:  
//
//******************************************************************************
LONG  FlowSensor::GetCurrentReading(void)
{
    CurrentReading = (LONG)(100*calculate_flow());
    return CurrentReading;
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: GetCompensatedReading()
//
//    Processing: 	Get the data at input address
//
//    Input Parameters: the address need to get data
//
//    Return Values:     data at input address
//
//    Pre-Conditions:    None
//    Miscellaneous:     None
//
//    Requirements:  
//
//******************************************************************************
LONG   FlowSensor::GetCompensatedReading(void)
{
    return CurrentReading;
}
