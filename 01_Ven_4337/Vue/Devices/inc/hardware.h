//******************************************************************************
//$COMMON.H$
//   File Name:  HARDWARE.H
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: 
//      This file contains many of the hardware related constants.
//******************************************************************************

#ifndef  HARDWARE_H
#define  HARDWARE_H

#include "CommonTypes.h"

// SPICONTROL -- SPX Control Register
#define SPX_CONTROL_ADDRESS_OFFSET  0x0
#define SPX_CONTROL_POR     0x0

#define SPX_CONTROL_CPOL_MASK            0x1
#define SPX_CONTROL_CPOL_SHIFT           7
#define SPX_CONTROL_CPOL_LOW          0x0     // SCLK Idles Low
#define SPX_CONTROL_CPOL_HIGH         0x1 // SCLK Idles High

#define SPX_CONTROL_CPHA_MASK            0x1
#define SPX_CONTROL_CPHA_SHIFT           6
#define SPX_CONTROL_CPHA_RISING       0x0     // Data is Read on Rising edge of SCLK
#define SPX_CONTROL_CPHA_FALLING  0x1 // Data is Read on Falling edge of SCLK

#define SPX_CONTROL_SPILEN_MASK          0x3    // 2 bits
#define SPX_CONTROL_SPILEN_SHIFT           4

#define SPX_CONTROL_SPILEN_8BIT       0x0     // 8-bit SPI Frame Length
#define SPX_CONTROL_SPILEN_16BIT      0x1     // 16-bit SPI Frame Length
#define SPX_CONTROL_SPILEN_24BIT      0x2     // 24-bit SPI Frame Length
#define SPX_CONTROL_SPILEN_32BIT      0x3     // 32-bit SPI Frame Length

#define SPX_CONTROL_MANSS_MASK          0x1
#define SPX_CONTROL_MANSS_SHIFT           3
#define SPX_CONTROL_MANSS_AUTO        0x0     // Slave Select lines operate automatically when write ot SPIDATA3 occurs
#define SPX_CONTROL_MANSS_MANUAL      0x1     // Slave Select lines are set manual using SS(2:0) bits

#define SPX_CONTROL_SS_MASK             0x7     // 3 bits
#define SPX_CONTROL_SS_SHIFT            0
#define SPX_CONTROL_SS_NONE   0x0     // None
#define SPX_CONTROL_SS_SS0    0x1     // SS0#
#define SPX_CONTROL_SS_SS1    0x2     // SS1#
#define SPX_CONTROL_SS_SS2    0x3     // SS2#
#define SPX_CONTROL_SS_SS3    0x4     // SS3#
#define SPX_CONTROL_SS_ADC    0x5     // ADC_SS#      -- Note these last 3 defines vary from product to product
#define SPX_CONTROL_SS_DIO0   0x6     // DIO_0_SS#
#define SPX_CONTROL_SS_DIO1   0x7     // DIO_1_SS#


// SPISTATUS -- SPX Status (and some Control) Register
#define SPX_STATUS_ADDRESS_OFFSET  0x1
#define SPX_STATUS_POR     0x0

#define SPX_STATUS_IRQSEL_MASK            0x3
#define SPX_STATUS_IRQSEL_SHIFT           6
#define SPX_STATUS_IRQSEL_IRQ3        0x0
#define SPX_STATUS_IRQSEL_IRQ4        0x1
#define SPX_STATUS_IRQSEL_IRQ5        0x2
#define SPX_STATUS_IRQSEL_IRQ10       0x3

#define SPX_STATUS_SPICLK_MASK            0x3
#define SPX_STATUS_SPICLK_SHIFT           4
#define SPX_STATUS_SPICLK_1MHZ        0x0
#define SPX_STATUS_SPICLK_2MHZ        0x1
#define SPX_STATUS_SPICLK_4MHZ        0x2
#define SPX_STATUS_SPICLK_8MHZ        0x3

#define SPX_STATUS_HWIRQEN_MASK            0x1
#define SPX_STATUS_HWIRQEN_SHIFT           3
#define SPX_STATUS_HWIRQEN_DISABLE    0x0
#define SPX_STATUS_HWIRQEN_ENABLE     0x1

#define SPX_STATUS_LSBIT1ST_MASK            0x1
#define SPX_STATUS_LSBIT1ST_SHIFT           2
#define SPX_STATUS_LSBIT1ST_LEFT      0x0     // Left shift data (MS Bit First)
#define SPX_STATUS_LSBIT1ST_RIGHT     0x1     // Right shift data (LS Bit First)

#define SPX_STATUS_HWINT_MASK            0x1    // HW Interrupt Status bit
#define SPX_STATUS_HWINT_SHIFT           1
#define SPX_STATUS_HWINT_CLEAR        0x0     // interrupt is not set
#define SPX_STATUS_HWINT_SET          0x1     // interrupt is set  (asserted)

#define SPX_STATUS_BUSY_MASK            0x1     // HW Interrupt Status bit
#define SPX_STATUS_BUSY_SHIFT           0
#define SPX_STATUS_BUSY_NO    0x0     // Not busy
#define SPX_STATUS_BUSY_YES   0x1     // busy

// SPIDATA0,1,2,3 -- Data Registers
// Writes to SPIDATA3 start automatic cycles
#define SPX_DATA0_ADDRESS_OFFSET  0x2
#define SPX_DATA1_ADDRESS_OFFSET  0x3
#define SPX_DATA2_ADDRESS_OFFSET  0x4
#define SPX_DATA3_ADDRESS_OFFSET  0x5



// PCR -- Product Code and PLED Register
#define FPGA_PCR_ADDRESS_OFFSET  0x0
#define FPGA_PCR_POR    0x0

#define FPGA_PCR_PLED_MASK            0x1
#define FPGA_PCR_PLED_SHIFT           7
#define FPGA_PCR_PLED_OFF  0x0
#define FPGA_PCR_PLED_ON   0x1
#define FPGA_PCR_PRODUCT_CODE_MASK    0x7F
#define FPGA_PCR_PRODUCT_CODE_SHIFT   0
#    define FPGA_PCR_PRODUCT_CODE  0x5   // EBX-37 product code

// PSR -- Product Status Reg (Revision, Ext Temp, Custom, Beta)
#define FPGA_PSR_ADDRESS_OFFSET  0x1
#define FPGA_PSR_POR    0x0

#define FPGA_PSR_REV_LEVEL_MASK   0x1F
#define FPGA_PSR_REV_LEVEL_SHIFT  3
#define FPGA_PSR_EXTEMP_MASK      0x1
#define FPGA_PSR_EXTEMP_SHIFT     2
#define FPGA_PSR_CUSTOM_MASK      0x1
#define FPGA_PSR_CUSTOM_SHIFT     1
#define FPGA_PSR_BETA_MASK        0x1
#define FPGA_PSR_BETA_SHIFT       0

// SCR - Jumpers and Debug Reg (sometimes called Status Control REg)
#define FPGA_SCR_ADDRESS_OFFSET  0x2
#define FPGA_SCR_POR    0x0

#define FPGA_SCR_BIOS_JMP_MASK         0x1
#define FPGA_SCR_BIOS_JMP_SHIFT        7
#define FPGA_SCR_BIOS_OR_MASK       0x1
#define FPGA_SCR_BIOS_OR_SHIFT      6
#define FPGA_SCR_BIOS_SEL_MASK       0x1
#define FPGA_SCR_BIOS_SEL_SHIFT      5
#define FPGA_SCR_LED_DEBUG_MASK       0x1
#define FPGA_SCR_LED_DEBUG_SHIFT      4
#define FPGA_SCR_WORKVER_MASK         0x1
#define FPGA_SCR_WORKVER_SHIFT        3
// bits 2:0 undefined

// ICR - Interrupt Control Reg
#define FPGA_ICR_ADDRESS_OFFSET  0x3
#define FPGA_ICR_POR    0x0

#define FPGA_ICR_IRQEN_MASK       0x1
#define FPGA_ICR_IRQEN_SHIFT      7
#        define FPGA_ICR_IRQEN_DISABLE  0
#        define FPGA_ICR_IRQEN_ENABLE   1
#define FPGA_ICR_IRQSEL_MASK      0x7
#define FPGA_ICR_IRQSEL_SHIFT     4
#        define FPGA_ICR_IRQSEL_IRQ3  0x0
#        define FPGA_ICR_IRQSEL_IRQ4  0x1
#        define FPGA_ICR_IRQSEL_IRQ5  0x2
#        define FPGA_ICR_IRQSEL_IRQ10 0x3
#        define FPGA_ICR_IRQSEL_IRQ6  0x4
#        define FPGA_ICR_IRQSEL_IRQ7  0x5
#        define FPGA_ICR_IRQSEL_IRQ9  0x6
#        define FPGA_ICR_IRQSEL_IRQ11 0x7
// Bit 3 undefined
#define FPGA_ICR_IMASK_TC2_MASK     0x1
#define FPGA_ICR_IMASK_TC2_SHIFT    2
#define FPGA_ICR_IMASK_TC1_MASK     0x1
#define FPGA_ICR_IMASK_TC1_SHIFT    1
#define FPGA_ICR_IMASK_TC0_MASK     0x1
#define FPGA_ICR_IMASK_TC0_SHIFT    0

// ISR - Interrupt Status and Timer Test Reg (some of these are read-only/write-1-to-clear bits)
#define FPGA_ISR_ADDRESS_OFFSET  0x4
#define FPGA_ISR_POR    0x0

#define FPGA_ISR_INTRTEST_MASK     0x1  // Interrupt Test
#define FPGA_ISR_INTRTEST_SHIFT    7
#    define FPGA_ISR_INTRTEST_DEASSERT  0
#    define FPGA_ISR_INTRTEST_ASSERT    1  // Unmasked
#define FPGA_ISR_TMRTEST_MASK     0x1   // Timer Test
#define FPGA_ISR_TMRTEST_SHIFT    6
#    define FPGA_ISR_TMRTEST_NORMAL  0
#    define FPGA_ISR_TMRTEST_TEST    1
#define FPGA_ISR_TMRIN1_MASK      0x1  // Timer Test Input#  1
#define FPGA_ISR_TMRIN1_SHIFT     5
#define FPGA_ISR_TMRIN0_MASK      0x1  // Timer Test Input#  0
#define FPGA_ISR_TMRIN0_SHIFT     4
// Bit 3 undefined
#define FPGA_ISR_ISTAT_TC2_MASK     0x1
#define FPGA_ISR_ISTAT_TC2_SHIFT    2
#define FPGA_ISR_ISTAT_TC1_MASK     0x1
#define FPGA_ISR_ISTAT_TC1_SHIFT    1
#define FPGA_ISR_ISTAT_TC0_MASK     0x1
#define FPGA_ISR_ISTAT_TC0_SHIFT    0
// This is mask for all the ISR Bits (used to mask them out when clearing)
#define FPGA_ISR_ISTAT_ALL_MASK     0x7
#define FPGA_ISR_ISTAT_ALL_SHIFT    0

// TCR - Timer Control/Status Reg
#define FPGA_TCR_ADDRESS_OFFSET  0x5
#define FPGA_TCR_POR    0x0

#define FPGA_TCR_TIM2GATE_MASK       0x1
#define FPGA_TCR_TIM2GATE_SHIFT      7
#define FPGA_TCR_TIM1GATE_MASK       0x1
#define FPGA_TCR_TIM1GATE_SHIFT      6
#define FPGA_TCR_TIM0GATE_MASK       0x1
#define FPGA_TCR_TIM0GATE_SHIFT      5
#    define FPGA_TCR_TIMGATE_OFF    0  // same for all 3 gates
#    define FPGA_TCR_TIMGATE_ON     1
#define FPGA_TCR_TM1MODE_MASK        0x1
#define FPGA_TCR_TM1MODE_SHIFT       4
#    define FPGA_TCR_TM1MODE_16BIT    0  // normal timer
#    define FPGA_TCR_TM1MODE_32BIT    1  // cascade mode
#define FPGA_TCR_TM1SEL_MASK         0x1
#define FPGA_TCR_TM1SEL_SHIFT        3
#define FPGA_TCR_TM0SEL_MASK         0x1
#define FPGA_TCR_TM0SEL_SHIFT        2
#    define FPGA_TCR_TMSEL_INTCLK   0 // same for both timers
#    define FPGA_TCR_TMSEL_EXTCLK   1
// Bits 1:0 undefined


// TBM - 8254 Timers I/O Base Offset (MS 8 bits)
#define FPGA_TBM_ADDRESS_OFFSET  0x6
#define FPGA_TBM_POR    0xFF

#define FPGA_TBM_BASE_ADDR_MASK      0xFF
#define FPGA_TBM_BASE_ADDR_SHIFT     0

// TBL - 8254 Timers I/O Base Offset (LS 8 bits but LS 2 bits are zero)
// Notice there is no shift ... this is so they can be treated like 8-bits
#define FPGA_TBL_ADDRESS_OFFSET  0x7
#define FPGA_TBL_POR    0xFC

#define FPGA_TBL_BASE_ADDR_MASK      0xFC
#define FPGA_TBL_BASE_ADDR_SHIFT     0
// bits 1:0 undefined

// 8254 Register start (4 register in consecutive addresses)
// Definitions are beyond the scope of this include file.
#define FPGA_SPX_BASE_ADDRESS_OFFSET    0x8

// The SPX Register Bit Definitions are used in many other products so they aren't defined here.
// Defined in Offsets 0x8 to 0xD

// SPI - Custom SPI Control Register
#define FPGA_SPI_ADDRESS_OFFSET  0xE
#define FPGA_SPI_POR    0x0

// Bits 7:2 Undefined
#define FPGA_SPI_SPILB_MASK         0x1
#define FPGA_SPI_SPILB_SHIFT        1
#    define FPGA_SPI_SPILB_OFF    0
#    define FPGA_SPI_SPILB_ON     1
#define FPGA_SPI_ADIOMODE_MASK      0x1
#define FPGA_SPI_ADIOMODE_SHIFT     0
#    define FPGA_SPI_ADIOMODE_NORMAL   0  // Normal SPX Modes (EBX-11 Mode)
#    define FPGA_SPI_ADIOMODE_ADIOB    1  // ADIO "B" Modes

// ADM - A/D and D/A Control/Status Register
#define FPGA_ADM_ADDRESS_OFFSET  0xF
#define FPGA_ADM_POR    0x0

// Bits 7:6 Undefined
#define FPGA_ADM_DACLDA_MASK         0x3
#define FPGA_ADM_DACLDA_SHIFT        4
#    define FPGA_ADM_DACLDA_NOP     0x0  // No Operation
#    define FPGA_ADM_DACLDA_DAC0    0x1  // Load DAC0
#    define FPGA_ADM_DACLDA_DAC1    0x2  // Load DAC1
#    define FPGA_ADM_DACLDA_BOTH    0x3  // Load Both DACs
#define FPGA_ADM_ADCBUSY_MASK         0x3
#define FPGA_ADM_ADCBUSY_SHIFT        2
#    define FPGA_ADM_ADCBUSY_NO      0x0  // Neither ADC is busy
#    define FPGA_ADM_ADCBUSY_ADC0    0x1  // ADC0 is busy
#    define FPGA_ADM_ADCBUSY_ADC1    0x2  // ADC1 is busy
#    define FPGA_ADM_ADCBUSY_BOTH    0x3  // Both ADCs are busy
#define FPGA_ADM_ADCCONVST_MASK       0x3
#define FPGA_ADM_ADCCONVST_SHIFT      0
#    define FPGA_ADM_ADCCONVST_NOP     0x0   // No operation
#    define FPGA_ADM_ADCCONVST_ADC0    0x1   // writing this starts ADC0 conversion
#    define FPGA_ADM_ADCCONVST_ADC1    0x2   // writing this starts ADC1 conversion
#    define FPGA_ADM_ADCCONVST_BOTH    0x3   // writing this starts both ADC conversions




// Access Macros

// Get and Set specific fields in registers
// Get value from a register and return the value (e.g.,  val = GETVAL(reg,mask,shift) )
#define GETVAL(_reg,_mask,_shift)    ( (_reg >> _shift) & _mask )
// Set a value in a register and return the updated register (e.g., newReg = SETVAL(currReg,val,mask,shift) )
#define SETVAL(_val,_reg,_mask,_shift)    ( (_reg & ~(_mask << _shift)) | ((_val & _mask) << _shift) )


// Macros wrapped around inp() and outp()
#define INP(_val,_addr)  _val = inb(_addr)
#define OUTP(_addr,_val) outb(_val, _addr)

/**************************************************************
  Analog / Digital Conversion Constants
**************************************************************/

// Voltage Constants
static const USHORT VOLTAGE_SCALE      = 1000;  // Voltage values are * 1000
static const USHORT MAX_SCALED_VOLTAGE = 5000;
static const USHORT MIN_SCALED_VOLTAGE = 0;
static const USHORT VOLT_CURRENT_SCALE = 6 * VOLTAGE_SCALE;
                                                // Voltage and current values
                                                // read are scaled by 6

// Analog-to-Digital Constants
static const USHORT MAX_ADC_COUNTS   = 4095;    // Max reading from A2D Converter
static const USHORT MAX_VOLTS        =    5;    // Max voltage input to A2D Converter
static const USHORT A2D_ERROR_VALUE  = 0xFFFF;  // Value returned when Out of Range
static const USHORT MAX_A2D_FAILURES = 3;

// Digital-to-Analog Constants
static const USHORT MAX_DAC_COUNTS = 255; // D2A is 8-bit data (0-255)
static const USHORT MAX_DAC_VOLTS  = 5;   // D2A max output = 5 Volts

#endif   /* HARDWARE_H */


