/*
 * ADCSensor.c
 *
 *  Created on: 2 thg 1, 2019
 *      Author: haxua
 */
#include <board/i2c.h>
#include "ADCSensor.h"
#include "Delay.h"
#include "I2CSensor.h"
#include "ActivePressureSensor.h"
#define FIO2_BUFSIZE 100
#define VOLTAGE_BUFSIZE 150
#define O2_SENSOR_LIMIT   0xFFFFF0
#define O2_INVALID  O2_SENSOR_LIMIT + 1
static const float  O2_NOT_PRESENT   = (-1);
static const float  O2_NOT_VALID     = (-2);
static const int32_t UNINITIALIZED_LONG_VALUE = 0x80000001;
static const int32_t BLANKED_LONG_VALUE = 0x80000000 ;
static const int32_t SCALE = 100;

static FiO2DataStruct gs_CountBuffer[FIO2_BUFSIZE];
static float gs_CurrentReading;
static uint16_t gs_Index;
static bool gs_IsPresent;
static float gs_CurrentFiO2Volt;
static float gs_ConversionFactor;
static float gs_Offset;
static bool gs_IsCalibrated;
static uint16_t gs_VoltageBuffIndex;
static float gs_VoltageBuff[VOLTAGE_BUFSIZE];
const float O2SensorLowerVolt = 0.25;
const float O2SensorUpperVolt = 5.0;

static float gs_CurrentTempVolt=0;


static float gs_CurrentExhVolt=0;
static int16_t gs_CurrentExhReading=0;
static float gs_ZeroOffSetExhInVolt=0;
static float gs_CurrentInhVolt=0;
static int16_t gs_CurrentInhReading=0;
static float gs_ZeroOffSetInhInVolt=0;

#define FIO2_EX_CHANNEL 4
#define PRESSURE_EXH_CHANNEL 3
#define PRESSURE_INH_CHANNEL 2
#define BOARD_TEMP_CHANNEL 1

#define FIO2_IN_CHANNEL 0
#define ADC_I2C_CHANNEL I2C4
#define ADC_I2C_BAUDRATE 100000U
#define I2C_ADC_ADDR_7BIT 0x48 //0x4B
#define ADC_MAX_VOLTAGE 3.3
#define ADC_12_BIT 4096
/**
 * Read voltage from desired ADC channel
 * @param channel	desired ADC channel
 * @return value	voltage reading in Volt
 */
static float ADCSensor_ReadVoltage(int channel)
{
	uint8_t GetADCCommand[1]={'\0'};
	switch(channel)
	{
	case 0:
		GetADCCommand[0]=0x84;
		break;
	case 1:
		GetADCCommand[0]=0xC4;
		break;
	case 2:
		GetADCCommand[0]=0x94;
		break;
	case 3:
		GetADCCommand[0]=0xD4;
		break;
	case 4:
		GetADCCommand[0]=0xA4;
		break;
	default:
		break;
	}

    uint8_t result[2] = {'\0'};

	i2c_Write(ADC_I2C_CHANNEL, I2C_ADC_ADDR_7BIT, GetADCCommand, 1);

	delay_us(10);

	i2c_Read(ADC_I2C_CHANNEL, I2C_ADC_ADDR_7BIT, result, 2);

	int raw_adc = ((result[0] & 0x0F) * 256 + result[1]);
	float value = (float)raw_adc * ADC_MAX_VOLTAGE / ADC_12_BIT;
	return value;
}
/**
 * Read current voltage from Exhalation pressure sensor and convert it to pressure value
 * @return int16_t gs_CurrentExhReading Pressure reading in cmH20 * SCALE
 */
int16_t ADCSensorExh_GetCurrentReading()
{
	gs_CurrentExhVolt =  ADCSensor_ReadVoltage(PRESSURE_EXH_CHANNEL);
	float currentUnit=((1/(9.0/550.0)) * (((gs_CurrentExhVolt-gs_ZeroOffSetExhInVolt) / ADC_MAX_VOLTAGE) -0.5))-0.2;
	gs_CurrentExhReading = currentUnit*100 ;
	return gs_CurrentExhReading;
}
/**
 * Get the previously obtained reading pressure from the Exhalation pressure sensor
 * @return int16_t gs_CurrentExhReading Pressure reading in cmH2O * SCALE
 */
int16_t ADCSensorExh_GetLastReading()
{
	return gs_CurrentExhReading;
}
/**
 * Get the previously obtained voltage from the Exhalation pressure sensor
 * @return float gs_CurrentExhVolt Voltage reading in Volt
 */
float ADCSensorExh_GetLastVolt()
{
	return gs_CurrentExhVolt;
}
/**
 * Get current voltage from Exhalation pressure sensor
 * @return float gs_CurrentExhVolt Voltage in Volt
 */
float ADCSensorExh_GetCurrentVolt()
{
	gs_CurrentExhVolt =  ADCSensor_ReadVoltage(PRESSURE_EXH_CHANNEL);
	return gs_CurrentExhVolt;
}
/**
 * Set Offset value for Exhalation pressure sensor
 * @param offset Offset value in Volt
 */
void ADCSensorExh_SetZeroOffSet(float offset)
{
	gs_ZeroOffSetExhInVolt = offset;
}
/**
 * Read current voltage from Inhalation pressure sensor and convert it to pressure value
 * @return int16_t gs_CurrentInhReading Pressure reading in cmH20 * SCALE
 */
int16_t ADCSensorInh_GetCurrentReading()
{
	gs_CurrentInhVolt =  ADCSensor_ReadVoltage(PRESSURE_INH_CHANNEL);
	float currentUnit=((1/(9.0/550.0)) * (((gs_CurrentInhVolt-gs_ZeroOffSetInhInVolt) / ADC_MAX_VOLTAGE) -0.5))-0.2;
	gs_CurrentInhReading =currentUnit*100  ;
	return gs_CurrentInhReading;
}
/**
 * Get the previously obtained reading pressure from the Inhalation pressure sensor
 * @return int16_t gs_CurrentInhReading Pressure reading in cmH2O * SCALE
 */
int16_t ADCSensorInh_GetLastReading()
{
	return gs_CurrentInhReading;
}
/**
 * Get the previously obtained voltage from the Inhalation pressure sensor
 * @return float gs_CurrentInhVolt Voltage reading in Volt
 */
float ADCSensorInh_GetLastVolt()
{
	return gs_CurrentInhVolt;
}
/**
 * Get current voltage from Inhalation pressure sensor
 * @return float gs_CurrentInhVolt Voltage in Volt
 */
float ADCSensorInh_GetCurrentVolt()
{
	gs_CurrentInhVolt =  ADCSensor_ReadVoltage(PRESSURE_INH_CHANNEL);
	return gs_CurrentInhVolt;
}
/**
 * Set Offset value for Inhaltion pressure sensor
 * @param offset Offset value in Volt
 */
void ADCSensorInh_SetZeroOffSet(float offset)
{
	gs_ZeroOffSetInhInVolt = offset;
}
/**
 * Average FiO2 reading voltage
 * @return float longVal Voltage in Volt
 */
static float ADCSensorFiO2_AvgVoltage()
{
	int i;
	float longVal = 0;

	// Add up all count values
	for (i = 0; i < VOLTAGE_BUFSIZE; i++)
		longVal += gs_VoltageBuff[i];

	longVal /= VOLTAGE_BUFSIZE;

	return (longVal);
}
/**
 * Initialize global static variable used for FiO2 sensor
 */
void ADCSensorFiO2_Init()
{
	i2c_Init(ADC_I2C_CHANNEL,ADC_I2C_BAUDRATE);
	gs_ConversionFactor = 0;
	gs_Offset = 0;

	gs_CurrentReading = O2_NOT_PRESENT;
	gs_Index = 0;
	gs_VoltageBuffIndex = 0;
	gs_IsPresent = false;
	gs_IsCalibrated = false;
	gs_CurrentFiO2Volt = 0;
	for (int i = 0; i < FIO2_BUFSIZE; i++)
	{
		gs_CountBuffer[i].FiO2Value = 0;
		gs_CountBuffer[i].FlowValue = 0;
		gs_CountBuffer[i].PressValue = 0;
	}
	for (int i = 0; i < VOLTAGE_BUFSIZE; i++)
	{
		gs_VoltageBuff[i] = 0;
	}
	// Initialize conversion factor
	int32_t fio2Factor = 0;//systemConfigFile->getValue(eFiO2ConversionFactor);
	int32_t fio2Offset = 0;//systemConfigFile->getValue(eFiO2Offset);
	int32_t passCalibration = 0;//systemConfigFile->getValue(eFiO2PassCalibration);

	if (passCalibration != UNINITIALIZED_LONG_VALUE)
	{
		gs_IsCalibrated = (bool)passCalibration;
	}

	if ((fio2Factor==0)||(fio2Factor==O2_INVALID)||(fio2Factor== UNINITIALIZED_LONG_VALUE))
		fio2Factor = 0;

	if((fio2Offset==0)||(fio2Offset==O2_INVALID)||(fio2Offset==UNINITIALIZED_LONG_VALUE))
		fio2Offset = 0;

	float factorFloat = (float)((float)fio2Factor/(float)(SCALE*SCALE));
	float offsetFloat = (float)((float)fio2Offset/(float)(SCALE*SCALE));

	if(gs_IsCalibrated)
		ADCSensorFiO2_SetConversionFactor(factorFloat, offsetFloat);
}

/**
 * Get the average reading from the FiO2 sensor
 * @return float gs_CurrentReading FiO2 Value in percentage scale
 */
float ADCSensorFiO2_GetCurrentReading()
{
	float currentO2Value;

	if(gs_IsCalibrated == false)
		return(O2_NOT_VALID);

	if (ADCSensorFiO2_GetIsPresent() == true)
	{
		//get average value
		currentO2Value = ADCSensorFiO2_AvgFiO2();
		gs_CurrentReading = (float)(currentO2Value*gs_ConversionFactor+gs_Offset);
	}
	else
	{
		gs_CurrentReading = O2_NOT_PRESENT;
	}

	return(gs_CurrentReading);
}

/**
 * None use function
 * @param Pressure
 * @param amplitude
 * @return
 */
int32_t ADCSensorFiO2_PressureCompensate(int32_t Pressure,int32_t amplitude)
{
	// atmospheric Pressure
	float Patm;
	float Concentration;
	float PressCompFactor;  //pressure compenstate factor
	float AmplitudeCompFactor;  //amplitude compensate factor
	float Compensated;
	float altitude;
	int32_t rtnValue;

	Concentration = ADCSensorFiO2_GetCurrentReading();

	if ((O2_NOT_VALID == Concentration) ||
			(O2_NOT_PRESENT == Concentration))
		return(Concentration);

	if (Pressure == BLANKED_LONG_VALUE)
		PressCompFactor = 1.0;
	else
	{
		altitude = /*(float)(systemConfigFile->getValue(eElevationSetting)) **/ 3.281; //convert m to ft
		Patm = 0.00045889 * altitude;
		Patm = 14.70 - Patm;
		Patm *= 70.309;
		PressCompFactor = (Patm / (Patm + ((float)Pressure/(float)SCALE)));
	}
	Compensated = PressCompFactor * Concentration;

	Compensated = Compensated - (((Compensated-20.6)/79.4)*(1 + ((Compensated-20.6)/79.4)*2.5));
	//calculate amplitude compensate factor
	AmplitudeCompFactor = 1-((amplitude/100)*0.04/150)*((Compensated-21)/79);
	Compensated *= AmplitudeCompFactor;

	if(Compensated >= 100)
		Compensated = 100;
	if(Compensated <= 21)
		Compensated = 21;

	rtnValue = (int32_t)(Compensated * SCALE);
	return(rtnValue);
}

/**
 * Allows other domains to set conversion factor
 * @param factor
 * @param offset value to change ConversionFactor
 */
void ADCSensorFiO2_SetConversionFactor(float factor, float offset)
{
	if(factor == 0){
		gs_IsCalibrated = false;
	}
	else {
		gs_IsCalibrated = true;
		gs_ConversionFactor = factor;
	}
	gs_Offset = offset;
}

/**
 * This routine determines whether a valid FiO2 sensor
 * is connected to the system.
 * @return bool gs_IsPresent return True if valid FiO2 sensor is connected
 * 				otherwise return False
 */
bool ADCSensorFiO2_GetIsPresent()
{
	// Read current value in a2d counts  (0-4095)
	float value = ADCSensorFiO2_AvgVoltage();

	if((value > O2SensorLowerVolt)&&(value < O2SensorUpperVolt))
		gs_IsPresent = true;
	else
	{
		gs_IsPresent = false;
		gs_IsCalibrated = false;
	}

	return (gs_IsPresent);
}

/**
 *This routine gets the raw counts data from the A2DConverter.
 *         The readings are saved in the buffer to be averaged by
 *          FiO2::GetCurrentReading().
 */
void ADCSensorFiO2_AddCounts()
{
	float reading;

	// Get Counts from sensor class
	reading = ADCSensorFiO2_GetLastVolt();

	// check that value is in range
	{
		gs_CountBuffer[gs_Index].FiO2Value = reading;
		gs_CountBuffer[gs_Index].FlowValue = (float)I2CSensorAirFlow_GetLastReading()/100.0 + (float)I2CSensorO2Flow_GetLastReading()/100.0;
		gs_CountBuffer[gs_Index].PressValue = (float)ActivePressureSensor_GetLastReading()/100.0;
		gs_Index++;
		if (gs_Index >= FIO2_BUFSIZE){
			gs_Index = 0;
		}
	}
}

/**
 * Return Averages FiO2 reading value
 * @return float longVal FiO2 in percentage scale
 */
float ADCSensorFiO2_AvgFiO2()
{
	int i;
	float longVal = 0;

	// Add up all count values
	for (i = 0; i < FIO2_BUFSIZE; i++)
		longVal += gs_CountBuffer[i].FiO2Value;

	longVal /= FIO2_BUFSIZE;

	return (longVal);
}

/**
 * Averages pressure in pressure buffer
 * @return float avgVal Pressure reading in cmH20
 */
float ADCSensorFiO2_AvgPress()
{
	int i;
	float avgVal = 0;

	// Add up all count values
	for (i = 0; i < FIO2_BUFSIZE; i++)
		avgVal += gs_CountBuffer[i].PressValue;

	avgVal /= FIO2_BUFSIZE;

	return (avgVal);
}

/**
 * Averages flow in flow buffer
 * @return float avgVal Flow reading in Lpm
 */
float ADCSensorFiO2_AvgFlow()
{
	int i;
	float avgVal = 0;

	// Add up all count values
	for (i = 0; i < FIO2_BUFSIZE; i++)
		avgVal += gs_CountBuffer[i].FlowValue;

	avgVal /= FIO2_BUFSIZE;

	return (avgVal);
}

/**
 * Get current voltage from FiO2 sensor to update gs_CurrentFiO2Volt variable
 */
void ADCSensorFiO2_GetCurrentVolt()
{
	gs_CurrentFiO2Volt = ADCSensor_ReadVoltage(FIO2_EX_CHANNEL);
}

/**
 * Get the previously obtained voltage from the FiO2 sensor
 * @return float gs_CurrentFiO2Volt Voltage reading in Volt
 */
float ADCSensorFiO2_GetLastVolt()
{
	return gs_CurrentFiO2Volt;
}

/**
 * Set FiO2 sensor's calibration state
 * @param state Calibration state
 */
void ADCSensorFiO2_SetIsCalibrated(bool state)
{
	gs_IsCalibrated = state;
}

/**
 * Return whether FiO2 Sensor is connected (Connect means
 * present and pass calibration)
 * @return bool gs_IsCalibrated
 */
bool ADCSensorFiO2_IsO2SensorConnect()
{
	return (bool)(gs_IsCalibrated);
}

/**
 * Get the previously obtained voltage from the FiO2 sensor
 * and add to gs_VoltageBuff array
 */
void ADCSensorFiO2_ReadVoltageValue()
{
	float reading;

	reading = ADCSensorFiO2_GetLastVolt();

	gs_VoltageBuff[gs_VoltageBuffIndex] = reading;

	if (gs_VoltageBuffIndex++ >= VOLTAGE_BUFSIZE){
		gs_VoltageBuffIndex = 0;
	}
}
float ADCSensorTemp_GetCurrentVolt()
{
	gs_CurrentTempVolt =  ADCSensor_ReadVoltage(BOARD_TEMP_CHANNEL);
	return gs_CurrentTempVolt;
}

