/********************************************************************/
/*                                                                  */
/* File Name     : Adc.c                                            */
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

#define ADC_MAX_VOLTAGE (3.3)
#define ADC_10BIT_MASK (1023)

class AdcInterFace
{
    public:
        AdcInterFace(){};
        float adc_Read(uint16_t dataADC)
        {
            return dataADC*ADC_MAX_VOLTAGE/ADC_10BIT_MASK;
        };
        virtual ~AdcInterFace(){};
};

class AdcUnitTest : public ::testing::Test, AdcInterFace
{
    protected:
        virtual void SetUp()
        {
        };

        virtual void TearDown()
        {
        };
};

TEST_F(AdcUnitTest, adc_Read)
{

}
