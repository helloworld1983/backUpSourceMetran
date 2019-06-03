/*
 * The Clear BSD License
 * Copyright (c) 2013 - 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted (subject to the limitations in the disclaimer below) provided
 * that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY THIS LICENSE.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "fsl_debug_console.h"
#include "board.h"
#include "fsl_pwm.h"

#include "pin_mux.h"
#include "clock_config.h"
#include "fsl_common.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define DEMO_PWM_BASEADDR PWM3
#define DEMO_PWM_IRQn PWM3_IRQn
#define DEMO_PWM_IRQHandler PWM3_IRQHandler
/*! @brief PWM period value. PWMO (Hz) = PCLK(Hz) / (period +2) */
#define PWM_PERIOD_VALUE		1000
/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/
volatile uint32_t pwmDutycycle = 0U;
volatile bool pwmDutyUp = true;         /* Indicate PWM Duty cycle is increase or decrease */

void DEMO_PWM_IRQHandler(void)
{
    /* Gets interrupt kPWM_FIFOEmptyFlag */
    if(PWM_GetStatusFlags(DEMO_PWM_BASEADDR) & kPWM_FIFOEmptyFlag)
    {
        if(pwmDutyUp)
        {
            /* Increase duty cycle until it reach limited value. */
            if(++pwmDutycycle > PWM_PERIOD_VALUE)
            {
                pwmDutycycle = PWM_PERIOD_VALUE;
                pwmDutyUp = false;
            }
        }
        else
        {
            /* Decrease duty cycle until it reach limited value. */
            if(--pwmDutycycle == 0U)
            {
                pwmDutyUp = true;
            }
        }
        /* Write duty cycle to PWM sample register.  */
        PWM_SetSampleValue(DEMO_PWM_BASEADDR, pwmDutycycle);
        /* Clear kPWM_FIFOEmptyFlag */
        PWM_clearStatusFlags(DEMO_PWM_BASEADDR, kPWM_FIFOEmptyFlag);
    }
    /* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
      exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}

/*!
 * @brief Main function
 */

int main(void)
{
    char ch;

    /* Init board hardware. */
    /* Board specific RDC settings */
    BOARD_RdcInit();
    
    BOARD_InitPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();
    BOARD_InitMemory();

    pwm_config_t pwmConfig;
    PRINTF("\r\nPWM driver example.\r\n");

    /*!
     * config->enableStopMode = false;
     * config->enableDozeMode = false;
     * config->enableWaitMode = false;
     * config->enableDebugMode = false;
     * config->clockSource = kPWM_LowFrequencyClock;
     * config->prescale = 0U;
     * config->outputConfig = kPWM_SetAtRolloverAndClearAtcomparison;
     * config->fifoWater = kPWM_FIFOWaterMark_2;
     * config->sampleRepeat = kPWM_EachSampleOnce;
     * config->byteSwap = kPWM_ByteNoSwap;
     * config->halfWordSwap = kPWM_HalfWordNoSwap;
     */
    PWM_GetDefaultConfig(&pwmConfig);

    /* Initialize PWM module */
    PWM_Init(DEMO_PWM_BASEADDR, &pwmConfig);

    /* Enable FIFO empty interrupt */
    PWM_EnableInterrupts(DEMO_PWM_BASEADDR, kPWM_FIFOEmptyInterruptEnable);

    /* Three initial samples be written to the PWM Sample Register */
    for(pwmDutycycle = 0u; pwmDutycycle < 3; pwmDutycycle++)
    {
        PWM_SetSampleValue(DEMO_PWM_BASEADDR, pwmDutycycle);
    }

    /* Check and Clear interrupt status flags */
    if(PWM_GetStatusFlags(DEMO_PWM_BASEADDR))
    {
        PWM_clearStatusFlags(DEMO_PWM_BASEADDR, kPWM_FIFOEmptyFlag | kPWM_RolloverFlag | kPWM_CompareFlag | kPWM_FIFOWriteErrorFlag);
    }

    /* Write the period to the PWM Period Register */
    PWM_SetPeriodValue(DEMO_PWM_BASEADDR, PWM_PERIOD_VALUE);

    /* Enable PWM interrupt request */
    EnableIRQ(DEMO_PWM_IRQn);

    PWM_StartTimer(DEMO_PWM_BASEADDR);

    while (1)
    {
    }


    
 

}
