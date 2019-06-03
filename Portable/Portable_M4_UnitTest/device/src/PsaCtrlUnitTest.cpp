/********************************************************************/
/*                                                                  */
/* File Name     : PsaCtrlUnitTest.cpp                              */
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

#define PSA_START_MAXPVSA_ID 14
#define PSA_START_SOL1ONOFFSET_ID 17
#define PSA_START_SOL3ONOFFSET_ID 18
#define PSA_START_SOL2OPENA_ID 19
#define PSA_START_SOL2OPENB_ID 20
#define PSA_START_2S_ID 21
#define PSA_START_3S_ID 22
#define PSA_START_5M_ID 23
#define PSA_START_SOL1DELAYOPEN_ID 24
#define PSA_START_SOL3DELAYOPEN_ID 25
#define PSA_LOW_TEMPERATURE 35
#define PSA_HIGH_TEMPERATURE 60 //42

enum
{
    eFlowRateZeroOneZero,
    eFlowRateZeroTwoFive,
    eFlowRateZeroFiveZero,
    eFlowRateZeroSevenFive,
    eFlowRateOneZeroZero,
    eFlowRateOneTwoFive,
    eFlowRateOneFiveZero,
    eFlowRateOneSevenFive,
    eFlowRateTwoZeroZero,
    eFlowRateTwoTowFive,
    eFlowRateTwoFiveZero,
    eFlowRateTwoSevenFive,
    eFlowRateThreeZeroZero,
    eFlowRateZeroZeroZero,
    eFlowRateLast
};

typedef enum{
    ePSALowTemperature,
    ePSANormalTemperature,
    ePSAHighTemperature
}E_PSATemperature;

typedef enum
{
    eSynchronizedMode=0,
    eContinuosMode
} E_ModeSetting;

uint32_t psa_TimerCallback( uint32_t timerId )
{
    switch (timerId)
    {
    case PSA_START_MAXPVSA_ID:
        return PSA_START_MAXPVSA_ID;
        break;
    case PSA_START_SOL1ONOFFSET_ID:
        return PSA_START_SOL1ONOFFSET_ID;
        break;
    case PSA_START_SOL3ONOFFSET_ID:
        return PSA_START_SOL3ONOFFSET_ID;
        break;
    case PSA_START_SOL1DELAYOPEN_ID:
        return PSA_START_SOL1DELAYOPEN_ID;
        break;
    case PSA_START_SOL3DELAYOPEN_ID:
        return PSA_START_SOL3DELAYOPEN_ID;
        break;
    case PSA_START_SOL2OPENA_ID:
        return PSA_START_SOL2OPENA_ID;
        break;
    case PSA_START_SOL2OPENB_ID:
        return PSA_START_SOL2OPENB_ID;
        break;
    case PSA_START_2S_ID:
        return PSA_START_2S_ID;
        break;
    case PSA_START_3S_ID:
        return PSA_START_3S_ID;
        break;
    case PSA_START_5M_ID:
        return PSA_START_5M_ID;
        break;
    default:
        break;
    }
}
class TimerCallbackTest : public ::TestWithParam<uint32_t>
{
};
uint32_t TimerCallback[] =
{
        PSA_START_MAXPVSA_ID, PSA_START_SOL1ONOFFSET_ID, PSA_START_SOL3ONOFFSET_ID,
        PSA_START_SOL1DELAYOPEN_ID, PSA_START_SOL3DELAYOPEN_ID, PSA_START_SOL2OPENA_ID,
        PSA_START_SOL2OPENB_ID, PSA_START_2S_ID, PSA_START_3S_ID, PSA_START_5M_ID

};
TEST_P(TimerCallbackTest, TimerCallbackTestCase)
{
    EXPECT_EQ(GetParam(), psa_TimerCallback(GetParam()));
}
INSTANTIATE_TEST_CASE_P(TimerCallbackTest,
                        TimerCallbackTest,
                        testing::ValuesIn(TimerCallback)
);

E_PSATemperature psa_SetCurrentTemperature(uint16_t compBoardTemp)
{
    E_PSATemperature temperature = ePSALowTemperature;
   if(compBoardTemp!=0)
   {

       if(compBoardTemp <= PSA_LOW_TEMPERATURE)
       {
           temperature = ePSALowTemperature;
       }
       else
           if(compBoardTemp > PSA_HIGH_TEMPERATURE)
           {
               temperature = ePSAHighTemperature;
           }
           else
           {
               temperature = ePSANormalTemperature;
           }
   }
   return temperature;
}

struct SetCurrentTemperature
{
        uint16_t compBoardTemp;
        E_PSATemperature temperature;

};
struct SetCurrentTemperature SetCurrentTemperatureTestCase[] =
{
        {0, ePSALowTemperature},

        {PSA_LOW_TEMPERATURE - 1 , ePSALowTemperature},
        {PSA_LOW_TEMPERATURE , ePSALowTemperature},

        {PSA_HIGH_TEMPERATURE + 1 , ePSAHighTemperature},
        {PSA_LOW_TEMPERATURE + 1 , ePSANormalTemperature}
};
class SetCurrentTemperatureTest : public ::testing::TestWithParam<SetCurrentTemperature>
{
};
TEST_P(SetCurrentTemperatureTest, SetCurrentTemperatureTestCase)
{
    EXPECT_EQ(GetParam().temperature, psa_SetCurrentTemperature(GetParam().compBoardTemp));
}
INSTANTIATE_TEST_CASE_P(SetCurrentTemperatureTest,
                        SetCurrentTemperatureTest,
                        testing::ValuesIn(SetCurrentTemperatureTestCase)
);

uint32_t psa_select(E_PSATemperature gs_currentTemperature,E_ModeSetting mode, uint8_t flowRequest)
{

    if(gs_currentTemperature == ePSALowTemperature)
    {
        if(mode == eContinuosMode)
        {
            switch(flowRequest)
            {
                case eFlowRateZeroOneZero:
                    return eFlowRateZeroOneZero;
                    break;
                case eFlowRateZeroTwoFive:
                    return eFlowRateZeroTwoFive;
                    break;
                case eFlowRateZeroFiveZero:
                    return eFlowRateZeroFiveZero;
                    break;
                case eFlowRateZeroSevenFive:
                    return eFlowRateZeroSevenFive;
                    break;
                case eFlowRateOneZeroZero:
                    return eFlowRateOneZeroZero;
                    break;
                default:
                    break;
            }
        }
        else
        {
            switch(flowRequest)
            {
                case eFlowRateOneTwoFive:
                    return eFlowRateOneTwoFive;
                    break;
                case eFlowRateOneFiveZero:
                    return eFlowRateOneFiveZero;
                    break;
                case eFlowRateOneSevenFive:
                    return eFlowRateOneSevenFive;
                    break;
                case eFlowRateTwoZeroZero:
                    return eFlowRateTwoZeroZero;
                    break;
                case eFlowRateTwoFiveZero:
                    return eFlowRateTwoFiveZero;
                    break;
                case eFlowRateThreeZeroZero:
                    return eFlowRateThreeZeroZero;
                    break;
                default:
                    break;
            }
        }
    }
    else
        if(gs_currentTemperature==ePSANormalTemperature)
        {
            if(mode == eContinuosMode)
            {
                switch(flowRequest)
                {
                    case eFlowRateZeroOneZero:
                        return eFlowRateZeroOneZero;
                        break;
                    case eFlowRateZeroTwoFive:
                        return eFlowRateZeroTwoFive;
                        break;
                    case eFlowRateZeroFiveZero:
                        return eFlowRateZeroFiveZero;
                        break;
                    case eFlowRateZeroSevenFive:
                        return eFlowRateZeroSevenFive;
                        break;
                    case eFlowRateOneZeroZero:
                        return eFlowRateOneZeroZero;
                        break;
                    default:
                        break;
                }
            }
            else
            {
                switch(flowRequest)
                {
                    case eFlowRateOneTwoFive:
                        return eFlowRateOneTwoFive;
                        break;
                    case eFlowRateOneFiveZero:
                        return eFlowRateOneFiveZero;
                        break;
                    case eFlowRateOneSevenFive:
                        return eFlowRateOneSevenFive;
                        break;
                    case eFlowRateTwoZeroZero:
                        return eFlowRateTwoZeroZero;
                        break;
                    case eFlowRateTwoFiveZero:
                        return eFlowRateTwoFiveZero;
                        break;
                    case eFlowRateThreeZeroZero:
                        return eFlowRateThreeZeroZero;
                        break;
                    default:
                        break;
                }
            }
        }
        else
            if(gs_currentTemperature==ePSAHighTemperature)
            {
                if(mode == eContinuosMode)
                {
                    switch(flowRequest)
                    {
                        case eFlowRateZeroOneZero:
                            return eFlowRateZeroOneZero;
                            break;
                        case eFlowRateZeroTwoFive:
                            return eFlowRateZeroTwoFive;
                            break;
                        case eFlowRateZeroFiveZero:
                            return eFlowRateZeroFiveZero;
                            break;
                        case eFlowRateZeroSevenFive:
                            return eFlowRateZeroSevenFive;
                            break;
                        case eFlowRateOneZeroZero:
                            return eFlowRateOneZeroZero;
                            break;
                        default:
                            break;
                    }
                }
                else
                {
                    switch(flowRequest)
                    {
                        case eFlowRateOneTwoFive:
                            return eFlowRateOneTwoFive;
                            break;
                        case eFlowRateOneFiveZero:
                            return eFlowRateOneFiveZero;
                            break;
                        case eFlowRateOneSevenFive:
                            return eFlowRateOneSevenFive;
                            break;
                        case eFlowRateTwoZeroZero:
                            return eFlowRateTwoZeroZero;
                            break;
                        case eFlowRateTwoFiveZero:
                            return eFlowRateTwoFiveZero;
                            break;
                        case eFlowRateThreeZeroZero:
                            return eFlowRateThreeZeroZero;
                            break;
                        default:
                            break;
                    }
                }
            }
}

struct S_psa_select
{

        E_PSATemperature gs_currentTemperature;
        E_ModeSetting mode;
        uint8_t flowRequest;
        uint32_t Result;
};

struct S_psa_select selectTest[] =
{
        {ePSALowTemperature, eContinuosMode, eFlowRateZeroOneZero, eFlowRateZeroOneZero},
        {ePSALowTemperature, eContinuosMode, eFlowRateZeroTwoFive, eFlowRateZeroTwoFive},
        {ePSALowTemperature, eContinuosMode, eFlowRateZeroFiveZero, eFlowRateZeroFiveZero},
        {ePSALowTemperature, eContinuosMode, eFlowRateZeroSevenFive, eFlowRateZeroSevenFive},
        {ePSALowTemperature, eContinuosMode, eFlowRateOneZeroZero, eFlowRateOneZeroZero},

        {ePSALowTemperature, eSynchronizedMode, eFlowRateOneTwoFive, eFlowRateOneTwoFive},
        {ePSALowTemperature, eSynchronizedMode, eFlowRateOneFiveZero, eFlowRateOneFiveZero},
        {ePSALowTemperature, eSynchronizedMode, eFlowRateOneSevenFive, eFlowRateOneSevenFive},
        {ePSALowTemperature, eSynchronizedMode, eFlowRateTwoZeroZero, eFlowRateTwoZeroZero},
        {ePSALowTemperature, eSynchronizedMode, eFlowRateTwoFiveZero, eFlowRateTwoFiveZero},
        {ePSALowTemperature, eSynchronizedMode, eFlowRateThreeZeroZero, eFlowRateThreeZeroZero},

        {ePSANormalTemperature, eContinuosMode, eFlowRateZeroOneZero, eFlowRateZeroOneZero},
        {ePSANormalTemperature, eContinuosMode, eFlowRateZeroTwoFive, eFlowRateZeroTwoFive},
        {ePSANormalTemperature, eContinuosMode, eFlowRateZeroFiveZero, eFlowRateZeroFiveZero},
        {ePSANormalTemperature, eContinuosMode, eFlowRateZeroSevenFive, eFlowRateZeroSevenFive},
        {ePSANormalTemperature, eContinuosMode, eFlowRateOneZeroZero, eFlowRateOneZeroZero},

        {ePSANormalTemperature, eSynchronizedMode, eFlowRateOneTwoFive, eFlowRateOneTwoFive},
        {ePSANormalTemperature, eSynchronizedMode, eFlowRateOneFiveZero, eFlowRateOneFiveZero},
        {ePSANormalTemperature, eSynchronizedMode, eFlowRateOneSevenFive, eFlowRateOneSevenFive},
        {ePSANormalTemperature, eSynchronizedMode, eFlowRateTwoZeroZero, eFlowRateTwoZeroZero},
        {ePSANormalTemperature, eSynchronizedMode, eFlowRateTwoFiveZero, eFlowRateTwoFiveZero},
        {ePSANormalTemperature, eSynchronizedMode, eFlowRateThreeZeroZero, eFlowRateThreeZeroZero},

        {ePSAHighTemperature, eContinuosMode, eFlowRateZeroOneZero, eFlowRateZeroOneZero},
        {ePSAHighTemperature, eContinuosMode, eFlowRateZeroTwoFive, eFlowRateZeroTwoFive},
        {ePSAHighTemperature, eContinuosMode, eFlowRateZeroFiveZero, eFlowRateZeroFiveZero},
        {ePSAHighTemperature, eContinuosMode, eFlowRateZeroSevenFive, eFlowRateZeroSevenFive},
        {ePSAHighTemperature, eContinuosMode, eFlowRateOneZeroZero, eFlowRateOneZeroZero},

        {ePSAHighTemperature, eSynchronizedMode, eFlowRateOneTwoFive, eFlowRateOneTwoFive},
        {ePSAHighTemperature, eSynchronizedMode, eFlowRateOneFiveZero, eFlowRateOneFiveZero},
        {ePSAHighTemperature, eSynchronizedMode, eFlowRateOneSevenFive, eFlowRateOneSevenFive},
        {ePSAHighTemperature, eSynchronizedMode, eFlowRateTwoZeroZero, eFlowRateTwoZeroZero},
        {ePSAHighTemperature, eSynchronizedMode, eFlowRateTwoFiveZero, eFlowRateTwoFiveZero},
        {ePSAHighTemperature, eSynchronizedMode, eFlowRateThreeZeroZero, eFlowRateThreeZeroZero}
};


class Testpsa_select : public ::testing::TestWithParam<S_psa_select>
{

};
TEST_P(Testpsa_select, Testpsa_selectTestCase)
{
    uint8_t flowRequest = GetParam().flowRequest;
    E_ModeSetting mode = GetParam().mode;
    E_PSATemperature gs_currentTemperature = GetParam().gs_currentTemperature;
    uint32_t Result = GetParam().Result;

    EXPECT_EQ(Result, psa_select(gs_currentTemperature, mode, flowRequest));

}
INSTANTIATE_TEST_CASE_P(Testpsa_select,
                        Testpsa_select,
                        testing::ValuesIn(selectTest)
);












