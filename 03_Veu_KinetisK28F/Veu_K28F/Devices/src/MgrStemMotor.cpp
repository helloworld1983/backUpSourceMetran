//******************************************************************************//
//$COMMON.H$
//    Filename: MgrStemMotor.h
//    Copyright 1996 InVentived Technologies, Inc. All Rights Reserved.
//
//    Class:  StepMotor
//    This class is the class for the Step Motor Controller Devices
//******************************************************************************//
#include "MgrStemMotor.h"
#include "DebugTrace.h"

MgrStemMotor *MgrStemMotor::S_Instance = NULL;
StepMotor *MgrStemMotor::AirValve = NULL;
StepMotor *MgrStemMotor::O2Valve = NULL;
StepMotor *MgrStemMotor::ExhValve = NULL;
StepMotor *MgrStemMotor::HfoValve = NULL;

//P3.3 SCK(1)   => (2) SSP0_SCK - R39. xanh la
//P3.6 MISO(1)  => (0) GPIO 0[6] - R41 tim
//P3.7 MOSI(1)  => (2) SSPO_MISO  - R40 trang
//P3.8 CS(4) - GPIO 5(11) => (2)SSPO -MOSI



/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: S_GetInstance
//
//    Processing: returns the static instance of the MgrStemMotor class
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values: pointer to MgrStemMotor class
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
MgrStemMotor* MgrStemMotor::S_GetInstance (void)
{
    //Create object if it is null
    if (S_Instance == NULL)
    {
        S_Instance = new MgrStemMotor();
    }
    //check object is null or not
    ASSERTION(S_Instance != NULL);

    return(S_Instance);
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Init
//
//    Processing: init for MgrStemMotor
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
void MgrStemMotor::Init (void)
{
    //Initialize for SPI
    //PIN SCK
//    Chip_SCU_PinMuxSet(SCK_PORT, SCK_PIN, (SCU_PINIO_FAST |SCU_MODE_FUNC1));
//    //PIN MISO
//    Chip_SCU_PinMuxSet(MISO_PORT, MISO_PIN, (SCU_PINIO_FAST |SCU_MODE_FUNC5));
//    //PIN MOSI
//    Chip_SCU_PinMuxSet(MOSI_PORT, MOSI_PIN, (SCU_PINIO_FAST |SCU_MODE_FUNC5));
//
//    //PIN CS
//    Chip_SCU_PinMuxSet(CS1_PORT, CS1_PIN, (SCU_PINIO_FAST |SCU_MODE_FUNC0));
//    Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, CS1_GPIO_PORT, CS1_GPIO_PIN);
//
//    Chip_SCU_PinMuxSet(CS2_PORT, CS2_PIN, (SCU_PINIO_FAST |SCU_MODE_FUNC0));
//    Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, CS2_GPIO_PORT, CS2_GPIO_PIN);
//
//    Chip_SCU_PinMuxSet(CS3_PORT, CS3_PIN, (SCU_PINIO_FAST |SCU_MODE_FUNC0));
//    Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, CS3_GPIO_PORT, CS3_GPIO_PIN);
//
//    Chip_SCU_PinMuxSet(CS4_PORT, CS4_PIN, (SCU_PINIO_FAST |SCU_MODE_FUNC0));
//    Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, CS4_GPIO_PORT, CS4_GPIO_PIN);
//
//    //Set PIN to HIGH - default
//    Chip_GPIO_WritePortBit(LPC_GPIO_PORT,CS1_GPIO_PORT, CS1_GPIO_PIN,true);
//    Chip_GPIO_WritePortBit(LPC_GPIO_PORT,CS2_GPIO_PORT, CS2_GPIO_PIN,true);
//    Chip_GPIO_WritePortBit(LPC_GPIO_PORT,CS3_GPIO_PORT, CS3_GPIO_PIN,true);
//    Chip_GPIO_WritePortBit(LPC_GPIO_PORT,CS4_GPIO_PORT, CS4_GPIO_PIN,true);
//
//    //power for motor
//    Chip_SCU_PinMuxSet(0x2, 2, (SCU_PINIO_FAST |SCU_MODE_FUNC4));
//    Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, 5, 2);
//    Chip_GPIO_WritePortBit(LPC_GPIO_PORT,5, 2, true);
//
//    // initialize SSP
//    Chip_SSP_Init(LPC_SSP);
//    Chip_SSP_SetMaster(LPC_SSP, true);
//    Chip_SSP_SetBitRate(LPC_SSP, SSP_BIT_RATE);
//    Chip_SSP_SetFormat(LPC_SSP, SSP_BITS_8, SSP_FRAMEFORMAT_SPI, SSP_CLOCK_MODE3);
//    Chip_SSP_Enable(LPC_SSP);
//    Chip_SSP_Int_Enable(LPC_SSP);   /* enable interrupt */
//
//
//    //set up clock out put = CLKIN_MAINPLL/ 12
//    //initialize function for CLK0 pin
//    Chip_SCU_ClockPinMuxSet(CLK0, SCU_MODE_FUNC1);
//
//    Chip_Clock_SetDivider(CLK_IDIV_E, CLKIN_MAINPLL, 17);
//    Chip_Clock_SetBaseClock(CLK_BASE_OUT, CLKIN_IDIVE, true, false);


    O2Valve = new StepMotor(0, eO2Valve);

    AirValve =  new StepMotor(1,eAirValve);

    ExhValve = new StepMotor(2, eExhValve);

    HfoValve = new StepMotor(3, eHfoValve);

    ASSERTION(NULL != O2Valve);
    ASSERTION(NULL != AirValve);
    ASSERTION(NULL != ExhValve);
    ASSERTION(NULL != HfoValve);

    O2Valve->Init(0);
    AirValve->Init(0);
    ExhValve->Init(0);
    HfoValve->Init(1);
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: MgrFlowController
//
//    Processing: Constructor of  MgrFlowController
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
MgrStemMotor::MgrStemMotor (void)
{
}
