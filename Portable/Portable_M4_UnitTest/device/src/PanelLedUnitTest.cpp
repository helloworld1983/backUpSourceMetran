/********************************************************************/
/*                                                                  */
/* File Name     : PanelLedUnitTest.cpp                             */
/*                                                                  */
/* General       : This module contain function for Unit Test       */
/*                                                                  */
/*                                                                  */
/* Product Name  : Portable                                         */
/*                                                                  */
/* Company       : Metran                                           */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*                                                                  */
/*                                                                  */
/********************************************************************/
#include "gtest.h"
using ::testing::TestWithParam;

typedef enum{
    eLevelBrightnessLow,
    eLevelBrightnessMed,
    eLevelBrightnessHigh,
}E_LEVEL_BRIGHTNESS;

typedef enum{
    D1,     //  O2_3
    D2,     //  O2_2
    D3,     //  O2_3
    D4,     //  O2_0

    D5,     //  BATT2
    D6,     //  BATT1
    D7,     //  BATT0

    D8,     //  AC_IN
    D9,     //  Consv Mode only During BATT operation
    D10,    //  Continuous Mode
    D11,    //  Synchronized Mode
    D12,    //  CALL    ALARM
    D13,    //  BREATH  ALARM
    D14,    //  CHARGRE ALARM
    D15     //  CHECK   ALARM
}E_ID_LED;

typedef enum{
    eErrorBreath = 1,
    eErrorAbnormalOxygenConcentration,
    eErrorLowOxygenConcentration,
    eErrorOxygenTankPressure,
    eErrorBatteryTemperature,
    eErrorBatteryCharge,
    eErrorBatteryRemainingCapacityLower,
    eErrorPowerLost,
    eErrorCompressorSupplyPressure,
    eErrorCompressorVacuumPressure,
    eErrorFlowcontrol = 11,
    eErrorSupplyPressureSensor,
    eErrorVacuumPressureSensor,
    eErrorTankPressureSensor,
    eErrorDifferentialPressureSensorCommunication,
    eErrorO2Sensor,
    eErrorFlowControllerCommunicationNoResponse,
    eErrorFlowControllerPressureSensor,
    eErrorFlowControllerFlowSensor,
    eErrorFlowControllerTemperatureSensor,
    eErrorFlowControllerCommunicationCheckSum,
    eErrorFlowControllerCommunication ,//22
    eErrorFlowControllerCommunicationCommand,
    eErrorFlowControllerCommunicationParameter,
    eFlowControllerResponseCheckSumError,//25
    eFlowControllerResponseCodeError,//26
    eFlowControllerResponseDataLenghtError,//27
    eErrorCompressorCommunicationNoResponse,
    eErrorCompressorDCOverVoltage,
    eErrorCompressorDCLowerVoltage,
    eErrorCompressorSoftwareOverCurrent,
    eErrorCompressorHardwareOverCurrent,
    eErrorCompressorBoardTemperature,
    eErrorCompressorMotorTemperature,
    eErrorCompressorRotationSpeed ,
    eErrorCompressorStepOut,
    eErrorCompressorCommunication,
    eErrorCompressorCommunicationCheckSum,
    eErrorCompressorCommunicationCommand,
    eErrorCompressorCommunicationParameter,
    eErrorCompressorResponseCheckSumError,//41
    eErrorCompressorResponseCodeError,//42
    eErrorCompressorResponseDataLenghtError,//43
    eErrorLEDDriverCommunication,
    eErrorAudioCommunication,
    eErrorSpeaker,
    eErrorSPIFlashMemory,
    eErrorBatteryType,  //48
    eErrorTachypnea,

}E_ID_ERRORCODE;

typedef enum{
    eNoneUI,
    eWithoutChargeLower = 1,
    eWithoutChargeLow,
    eWithoutChargeMed,
    eWithoutChargeHigh,

    eDuringChargeLower,
    eDuringChargeLow,
    eDuringChargeMed,
    eDuringChargeHigh,

    eACDisconnect,
    eACConnect,

    eContinuousModePattern1,
    eContinuousModePattern2,

    eSynchronizedModeClose,
    eSynchronizedModeOpen,

    eACPowerSupply,
    eBatteryPowerSupply,


    eContactErrorNoTrigger,
    eContactErrorTrigger,

    eBreathErrorNoTrigger,
    eBreathErrorTrigger,

    eLowerBatteryNoTrigger,
    eLowerBatteryTrigger,

    eServiceRequireNoTrigger,
    eServiceRequireTrigger,

    eValueZeroOneZero,
    eValueZeroTwoFive,
    eValueZeroFiveZero,
    eValueZeroSevenFive,
    eValueOneZeroZero,
    eValueOneTwoFive,
    eValueOneFiveZero,
    eValueOneSevenFive,
    eValueTwoZeroZero,
    eValueTwoTowFive,
    eValueTwoFiveZero,
    eValueTwoSevenFive,
    eValueThreeZeroZero,

    eOffSevenSeg,
    eDisplayWordSET,
    eDisplayWordE,
    eDisplayWordEn,
    eDisplayWordEnd,
    eDisplayWordUSB,
    eOffAllLeds,
    eOnAllLeds,
    eOffAlarmLeds,

}E_ID_CONDITION;

E_LEVEL_BRIGHTNESS panelLed_SetBrightness(E_LEVEL_BRIGHTNESS level)
{
    switch (level)
    {
        case eLevelBrightnessLow:
            return eLevelBrightnessLow;
            break;
        case eLevelBrightnessMed:
            return eLevelBrightnessMed;
            break;
        case eLevelBrightnessHigh:
            return eLevelBrightnessMed;
            break;
        default:
            break;
    }
}

class SetBrightnessTest : public::testing::TestWithParam<E_LEVEL_BRIGHTNESS>
{
};
E_LEVEL_BRIGHTNESS TestCaseList[] =
{
        eLevelBrightnessLow, eLevelBrightnessMed, eLevelBrightnessMed
};
TEST_P(SetBrightnessTest, SetBrightnessTestCaseList)
{
    EXPECT_EQ(GetParam(), panelLed_SetBrightness(GetParam()));
}
INSTANTIATE_TEST_CASE_P(SetBrightnessTest,
                        SetBrightnessTest,
                        testing::ValuesIn(TestCaseList)
);

uint8_t panelLed_TurnOnLED(E_ID_LED idLED)
{
    uint8_t gs_dataLed = 0;
    switch(idLED)
    {
        case D1:
            gs_dataLed |= 0b00010000;
            break;
        case D2:
            gs_dataLed |= 0b00100000;
            break;
        case D3:
            gs_dataLed |= 0b01000000;
            break;
        case D4:
            gs_dataLed |= 0b10000000;
            break;
        case D5:
            gs_dataLed |= 0b00000010;
            break;
        case D6:
            gs_dataLed |= 0b00000100;
            break;
        case D7:
            gs_dataLed |= 0b00001000;
            break;
        case D8:
            gs_dataLed |= 0b00000001;
            break;
        case D9:
            gs_dataLed |= 0b00001000;
            break;
        case D10:
            gs_dataLed |= 0b00000100;
            break;
        case D11:
            gs_dataLed |= 0b00000010;
            break;
        case D12:
            gs_dataLed |= 0b10000000;
            break;
        case D13:
            gs_dataLed |= 0b01000000;
            break;
        case D14:
            gs_dataLed |= 0b00100000;
            break;
        case D15:
            gs_dataLed |= 0b00010000;
            break;
        default:
            break;
    }
    return gs_dataLed;
}

struct TurnOnLED
{
        E_ID_LED idLED;
        uint8_t gs_dataLed;

};
struct TurnOnLED TurnOnLEDTestCase[] =
{
        {D1, 0b00010000},
        {D2, 0b00100000},
        {D3, 0b01000000},
        {D4, 0b10000000},
        {D5, 0b00000010},
        {D6, 0b00000100},
        {D7, 0b00001000},
        {D8, 0b00000001},
        {D9, 0b00001000},
        {D10, 0b00000100},
        {D11, 0b00000010},
        {D12, 0b10000000},
        {D13, 0b01000000},
        {D14, 0b00100000},
        {D15, 0b00010000}
};
class TurnOnLEDTest: public ::testing::TestWithParam<TurnOnLED>
{
};
TEST_P(TurnOnLEDTest, TurnOnLEDTestCase)
{
    EXPECT_EQ(GetParam().gs_dataLed, panelLed_TurnOnLED(GetParam().idLED));
}
INSTANTIATE_TEST_CASE_P(TurnOnLEDTestCase,
                        TurnOnLEDTest,
                        testing::ValuesIn(TurnOnLEDTestCase)
);

uint8_t panelLed_TurnOffLED(E_ID_LED idLED)
{
    uint8_t gs_dataLed = 1;
    switch(idLED)
    {
        case D1:
            gs_dataLed = 0b11101111;
            break;
        case D2:
            gs_dataLed = 0b11011111;
            break;
        case D3:
            gs_dataLed = 0b10111111;
            break;
        case D4:
            gs_dataLed = 0b01111111;
            break;
        case D5:
            gs_dataLed = 0b11111101;
            break;
        case D6:
            gs_dataLed = 0b11111011;
            break;
        case D7:
            gs_dataLed = 0b11110111;
            break;
        case D8:
            gs_dataLed = 0b11111110;
            break;
        case D9:
            gs_dataLed = 0b11110111;
            break;
        case D10:
            gs_dataLed = 0b11111011;
            break;
        case D11:
            gs_dataLed = 0b11111101;
            break;
        case D12:
            gs_dataLed = 0b01111111;
            break;
        case D13:
            gs_dataLed = 0b10111111;
            break;
        case D14:
            gs_dataLed = 0b11011111;
            break;
        case D15:
            gs_dataLed = 0b11101111;
            break;
        default:
            break;
    }
    return gs_dataLed;
}

struct TurnOffLED
{
        E_ID_LED idLED;
        uint8_t gs_dataLed;

};
struct TurnOffLED TurnOffLEDTestCase[] =
{
        {D1, 0b11101111},
        {D2, 0b11011111},
        {D3, 0b10111111},
        {D4, 0b01111111},
        {D5, 0b11111101},
        {D6, 0b11111011},
        {D7, 0b11110111},
        {D8, 0b11111110},
        {D9, 0b11110111},
        {D10, 0b11111011},
        {D11, 0b11111101},
        {D12, 0b01111111},
        {D13, 0b10111111},
        {D14, 0b11011111},
        {D15, 0b11101111}
};
class TurnOffLEDTest: public ::testing::TestWithParam<TurnOffLED>
{
};
TEST_P(TurnOffLEDTest, TurnOffLEDTestCase)
{
    EXPECT_EQ(GetParam().gs_dataLed, panelLed_TurnOffLED(GetParam().idLED));
}
INSTANTIATE_TEST_CASE_P(TurnOffLEDTestCase,
                        TurnOffLEDTest,
                        testing::ValuesIn(TurnOffLEDTestCase)
);

E_ID_CONDITION panelLed_LedDisplay(E_ID_CONDITION idCondition)
{
    switch (idCondition)
    {
        case eBreathErrorNoTrigger:
            return eBreathErrorNoTrigger;
            break;
        case eLowerBatteryNoTrigger:
            return eLowerBatteryNoTrigger;
            break;
        default:
            break;
    }
    switch(idCondition)
    {
        case eWithoutChargeLower:
            return eWithoutChargeLower;
            break;
        case eWithoutChargeLow:
            return eWithoutChargeLow;
            break;
        case eWithoutChargeMed:
            return eWithoutChargeMed;
            break;
        case eWithoutChargeHigh:
            return eWithoutChargeHigh;
            break;
        case eDuringChargeLower:
            return eDuringChargeLower;
            break;
        case eDuringChargeLow:
            return eDuringChargeLow;
            break;
        case eDuringChargeMed:
            return eDuringChargeMed;
            break;
        case eDuringChargeHigh:
            return eDuringChargeHigh;
            break;
        case eACConnect:
            return eACConnect;
            break;
        case eACDisconnect:
            return eACDisconnect;
            break;
        case eContinuousModePattern1:
            return eContinuousModePattern1;
            break;
        case eContinuousModePattern2:
            return eContinuousModePattern2;
            break;
        case eSynchronizedModeOpen:
            return eSynchronizedModeOpen;
            break;
        case eSynchronizedModeClose:
            return eSynchronizedModeClose;
            break;
        case eContactErrorTrigger:
            return eContactErrorTrigger;
            break;
        case eContactErrorNoTrigger:
            return eContactErrorNoTrigger;
            break;
        case eBreathErrorTrigger:
            return eBreathErrorTrigger;
            break;
        case eBreathErrorNoTrigger:
            return eBreathErrorNoTrigger;
            break;
        case eLowerBatteryTrigger:
            return eLowerBatteryTrigger;
            break;
        case eLowerBatteryNoTrigger:
            return eLowerBatteryNoTrigger;
            break;
        case eServiceRequireTrigger:
            return eServiceRequireTrigger;
            break;
        case eServiceRequireNoTrigger:
            return eServiceRequireNoTrigger;
            break;
        case eValueZeroOneZero:
            return eValueZeroOneZero;
            break;
        case eValueZeroTwoFive:
            return eValueZeroTwoFive;
            break;
        case eValueZeroFiveZero:
            return eValueZeroFiveZero;
            break;
        case eValueZeroSevenFive:
            return eValueZeroSevenFive;
            break;
        case eValueOneZeroZero:
            return eValueOneZeroZero;
            break;
        case eValueOneTwoFive:
            return eValueOneTwoFive;
            break;
        case eValueOneSevenFive:
            return eValueOneSevenFive;
            break;
        case eValueTwoZeroZero:
            return eValueTwoZeroZero;
            break;
        case eValueTwoTowFive:
            return eValueTwoTowFive;
            break;
        case eValueTwoFiveZero:
            return eValueTwoFiveZero;
            break;
        case eValueTwoSevenFive:
            return eValueTwoSevenFive;
            break;
        case eValueThreeZeroZero:
            return eValueThreeZeroZero;
            break;
        case eOffSevenSeg:
            return eOffSevenSeg;
            break;
        case eDisplayWordSET:
            return eDisplayWordSET;
            break;
        case eDisplayWordE:
            return eDisplayWordE;
            break;
        case eDisplayWordEn:
            return eDisplayWordEn;
            break;
        case eDisplayWordEnd:
            return eDisplayWordEnd;
            break;
        case eDisplayWordUSB:
            return eDisplayWordUSB;
            break;
        case eOffAllLeds:
            return eOffAllLeds;
            break;
        case eOnAllLeds:
            return eOnAllLeds;
            break;
        case eOffAlarmLeds:
            return eOffAlarmLeds;
        default:
            break;
    }
}

class LedDisplayTest : public ::testing::TestWithParam<E_ID_CONDITION>
{
};
E_ID_CONDITION LedDisplayTestCase[] =
{
        eBreathErrorNoTrigger, eLowerBatteryNoTrigger, eWithoutChargeLower, eWithoutChargeLow,
        eWithoutChargeMed, eWithoutChargeHigh, eDuringChargeLower, eDuringChargeLow,
        eDuringChargeMed, eDuringChargeHigh, eACConnect, eACDisconnect,
        eContinuousModePattern1, eContinuousModePattern2, eSynchronizedModeOpen, eSynchronizedModeClose,
        eContactErrorTrigger, eContactErrorNoTrigger, eBreathErrorTrigger, eBreathErrorNoTrigger,
        eLowerBatteryTrigger, eLowerBatteryNoTrigger, eServiceRequireTrigger, eServiceRequireNoTrigger,
        eValueZeroOneZero, eValueZeroTwoFive, eValueZeroFiveZero, eValueZeroSevenFive,
        eValueOneZeroZero, eValueOneTwoFive, eValueOneSevenFive, eValueTwoZeroZero,
        eValueTwoTowFive, eValueTwoFiveZero, eValueTwoSevenFive, eValueThreeZeroZero,
        eOffSevenSeg, eDisplayWordSET, eDisplayWordE, eDisplayWordEn,
        eDisplayWordEnd, eDisplayWordUSB, eOffAllLeds, eOnAllLeds,
        eOffAlarmLeds
};
TEST_P(LedDisplayTest, LedDisplayTestCase)
{
    EXPECT_EQ(GetParam(), panelLed_LedDisplay(GetParam()));
}

INSTANTIATE_TEST_CASE_P(LedDisplayTest,
                        LedDisplayTest,
                        testing::ValuesIn(LedDisplayTestCase)
);

E_ID_ERRORCODE panelLed_DisplayErrorCode(E_ID_ERRORCODE errorCode)
{
    switch(errorCode)
    {
    case eErrorBreath:
        return eErrorBreath;
        break;
    case eErrorAbnormalOxygenConcentration:
        return eErrorAbnormalOxygenConcentration;
        break;
    case eErrorLowOxygenConcentration:
        return eErrorLowOxygenConcentration;
        break;
    case eErrorOxygenTankPressure:
        return eErrorOxygenTankPressure;
        break;
    case eErrorBatteryTemperature:
        return eErrorBatteryTemperature;
        break;
    case eErrorBatteryCharge:
        return eErrorBatteryCharge;
        break;
    case eErrorBatteryRemainingCapacityLower:
        return eErrorBatteryRemainingCapacityLower;
        break;
    case eErrorPowerLost:
        return eErrorPowerLost;
        break;
    case eErrorCompressorSupplyPressure:
        return eErrorCompressorSupplyPressure;
        break;
    case eErrorCompressorVacuumPressure:
        return eErrorCompressorVacuumPressure;
        break;
    case eErrorFlowcontrol:
        return eErrorFlowcontrol;
        break;
    case eErrorSupplyPressureSensor:
        return eErrorSupplyPressureSensor;
        break;
    case eErrorVacuumPressureSensor:
        return eErrorVacuumPressureSensor;
        break;
    case eErrorTankPressureSensor:
        return eErrorTankPressureSensor;
        break;
    case eErrorDifferentialPressureSensorCommunication:
        return eErrorDifferentialPressureSensorCommunication;
        break;
    case eErrorO2Sensor:
        return eErrorO2Sensor;
        break;
    case eErrorFlowControllerCommunicationNoResponse:
        return eErrorFlowControllerCommunicationNoResponse;
        break;
    case eErrorFlowControllerPressureSensor:
        return eErrorFlowControllerPressureSensor;
        break;
    case eErrorFlowControllerFlowSensor:
        return eErrorFlowControllerFlowSensor;
        break;
    case eErrorFlowControllerTemperatureSensor:
        return eErrorFlowControllerTemperatureSensor;
        break;
    case eErrorFlowControllerCommunication:
        return eErrorFlowControllerCommunication;
        break;
    case eErrorFlowControllerCommunicationCheckSum:
        return eErrorFlowControllerCommunicationCheckSum;
        break;
    case eErrorFlowControllerCommunicationCommand:
        return eErrorFlowControllerCommunicationCommand;
        break;
    case eErrorFlowControllerCommunicationParameter:
        return eErrorFlowControllerCommunicationParameter;
        break;
    case eFlowControllerResponseCheckSumError:
        return eFlowControllerResponseCheckSumError;
        break;
    case eFlowControllerResponseCodeError:
        return eFlowControllerResponseCodeError;
        break;
    case eFlowControllerResponseDataLenghtError:
        return eFlowControllerResponseDataLenghtError;
        break;
    case eErrorCompressorCommunicationNoResponse:
        return eErrorCompressorCommunicationNoResponse;
        break;
    case eErrorCompressorDCOverVoltage:
        return eErrorCompressorDCOverVoltage;
        break;
    case eErrorCompressorDCLowerVoltage:
        return eErrorCompressorDCLowerVoltage;
        break;
    case eErrorCompressorSoftwareOverCurrent:
        return eErrorCompressorSoftwareOverCurrent;
        break;
    case eErrorCompressorHardwareOverCurrent:
        return eErrorCompressorHardwareOverCurrent;
        break;
    case eErrorCompressorBoardTemperature:
        return eErrorCompressorBoardTemperature;
        break;
    case eErrorCompressorMotorTemperature:
        return eErrorCompressorMotorTemperature;
        break;
    case eErrorCompressorRotationSpeed:
        return eErrorCompressorRotationSpeed;
        break;
    case eErrorCompressorStepOut:
        return eErrorCompressorStepOut;
        break;
    case eErrorCompressorCommunication:
        return eErrorCompressorCommunication;
        break;
    case eErrorCompressorCommunicationCheckSum:
        return eErrorCompressorCommunicationCheckSum;
        break;
    case eErrorCompressorCommunicationCommand:
        return eErrorCompressorCommunicationCommand;
        break;
    case eErrorCompressorCommunicationParameter:
        return eErrorCompressorCommunicationParameter;
        break;
    case eErrorCompressorResponseCheckSumError:
        return eErrorCompressorResponseCheckSumError;
        break;
    case eErrorCompressorResponseCodeError:
        return eErrorCompressorResponseCodeError;
        break;
    case eErrorCompressorResponseDataLenghtError:
        return eErrorCompressorResponseDataLenghtError;
        break;
    case eErrorLEDDriverCommunication:
        return eErrorLEDDriverCommunication;
        break;
    case eErrorAudioCommunication:
        return eErrorAudioCommunication;
        break;
    case eErrorSpeaker:
        return eErrorSpeaker;
        break;
    case eErrorSPIFlashMemory:
        return eErrorSPIFlashMemory;
        break;
    case eErrorBatteryType:
        return eErrorBatteryType;
        break;
    case eErrorTachypnea:
        return eErrorTachypnea;
        break;
    }
}

class DisplayErrorCodeTest : public ::TestWithParam<E_ID_ERRORCODE>
{
};
E_ID_ERRORCODE DisplayErrorCodeTestCase[] =
{
        eErrorBreath, eErrorAbnormalOxygenConcentration, eErrorLowOxygenConcentration, eErrorOxygenTankPressure,
        eErrorBatteryTemperature, eErrorBatteryCharge, eErrorBatteryRemainingCapacityLower, eErrorPowerLost,
        eErrorCompressorSupplyPressure, eErrorCompressorVacuumPressure, eErrorFlowcontrol, eErrorSupplyPressureSensor,
        eErrorVacuumPressureSensor, eErrorTankPressureSensor, eErrorDifferentialPressureSensorCommunication, eErrorO2Sensor,
        eErrorFlowControllerCommunicationNoResponse, eErrorFlowControllerPressureSensor, eErrorFlowControllerFlowSensor,
        eErrorFlowControllerTemperatureSensor, eErrorFlowControllerCommunication, eErrorFlowControllerCommunicationCheckSum,
        eErrorFlowControllerCommunicationCommand, eErrorFlowControllerCommunicationParameter, eFlowControllerResponseCheckSumError,
        eFlowControllerResponseCodeError, eFlowControllerResponseDataLenghtError, eErrorCompressorCommunicationNoResponse,
        eErrorCompressorDCOverVoltage, eErrorCompressorDCLowerVoltage, eErrorCompressorSoftwareOverCurrent,
        eErrorCompressorHardwareOverCurrent, eErrorCompressorBoardTemperature, eErrorCompressorMotorTemperature,
        eErrorCompressorRotationSpeed, eErrorCompressorStepOut, eErrorCompressorCommunication, eErrorCompressorCommunicationCheckSum,
        eErrorCompressorCommunicationCommand, eErrorCompressorCommunicationParameter, eErrorCompressorResponseCheckSumError,
        eErrorCompressorResponseCodeError, eErrorCompressorResponseDataLenghtError, eErrorLEDDriverCommunication,
        eErrorAudioCommunication, eErrorSpeaker, eErrorSPIFlashMemory, eErrorBatteryType, eErrorTachypnea

};
TEST_P(DisplayErrorCodeTest, DisplayErrorCodeTest)
{
    EXPECT_EQ(GetParam(), panelLed_DisplayErrorCode(GetParam()));
}

INSTANTIATE_TEST_CASE_P(DisplayErrorCodeTest,
                        DisplayErrorCodeTest,
                        testing::ValuesIn(DisplayErrorCodeTestCase)
);
