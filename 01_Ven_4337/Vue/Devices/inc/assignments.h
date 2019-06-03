//******************************************************************************
//$COMMON.H$
//   File Name:  
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: Assignments.h
//
//	 This file hold the specific port and bit signal assignments for
//	 the various object instantiations for the Morph project.
//******************************************************************************
#ifndef    ASSIGNMENTS_H
#define    ASSIGNMENTS_H

//
// Clocked Serial Interface Device Assignments
//
static const UCHAR	CSI_D2A_SELECT       = 0;	// Digital to Analog Converer
static const UCHAR	CSI_AIR_MOTOR_SELECT = 1;	// Step Motor #1  (Air) 
static const UCHAR	CSI_O2_MOTOR_SELECT  = 2;	// Step Motor #2  (Oxygen)
static const UCHAR	CSI_EXH_MOTOR_SELECT = 3;	// Step Motor #3  (Exhalation)
static const UCHAR	CSI_AIR_FLOW_SELECT  = 4;	// Air Flow Sensor EEPROM
static const UCHAR	CSI_O2_FLOW_SELECT   = 5;	// O2  Flow Sensor EEPROM
static const UCHAR	CSI_EXH_FLOW_SELECT  = 6;	// Exhalation Flow Sensor EEPROM
static const USHORT  NUMBER_OF_CSI_DEVICES =  7;
//
// A2D Converter channel assignments
//
static const UCHAR A2D_CHANNEL_00 = 0;  // Inhalation Pressure Transducer
static const UCHAR A2D_CHANNEL_01 = 1;  // Exhalation Pressure Transducer
static const UCHAR A2D_CHANNEL_02 = 2;	// O2  Flow Sensor - Flow
static const UCHAR A2D_CHANNEL_03 = 3;	// Air Flow Sensor - Flow

// O2 Flow Sensor  - Temp
// Air Flow Sensor - Temp
static const UCHAR A2D_CHANNEL_04 = 4;	// Exh Flow Sensor - Flow
static const UCHAR A2D_CHANNEL_05 = 5;	// Exh Flow Sensor - Temp
static const UCHAR A2D_CHANNEL_06 = 6;	
static const UCHAR A2D_CHANNEL_07 = 7;	
static const UCHAR A2D_CHANNEL_08 = 8;
static const UCHAR A2D_CHANNEL_09 = 9;	// FiO2 Sensor
static const UCHAR A2D_CHANNEL_10 = 10;
static const UCHAR A2D_CHANNEL_11 = 11;	// Enclosure Temperature
static const UCHAR A2D_CHANNEL_12 = 12;	// Battery Voltage
static const UCHAR A2D_CHANNEL_13 = 13;	// Battery Current
static const UCHAR A2D_CHANNEL_14 = 14;	// Internal O2
static const UCHAR A2D_CHANNEL_15 = 15;	// ADC Wrap


// Analog outputs
//         - The Address attribute is formatted as 4-bits, LSB first.
//        ( Ex: 1000 = Ch 1, 0100 = Ch 2, 0001 = Ch 8...)
static const UCHAR  DAC_CHANNEL_1 = 0x8;		// Spare
static const UCHAR  DAC_CHANNEL_2 = 0x4;		// Spare
static const UCHAR  DAC_CHANNEL_3 = 0xc;		// Spare
static const UCHAR  DAC_CHANNEL_4 = 0x2;		// Spare
static const UCHAR  DAC_CHANNEL_5 = 0xa;		// Blower Speed Control
static const UCHAR  DAC_CHANNEL_6 = 0x6;		// Spare
static const UCHAR  DAC_CHANNEL_7 = 0xe;		// Spare
static const UCHAR  DAC_CHANNEL_8 = 0x1;		// Voltage Wrap


#endif  // ASSIGNMENTS_H

