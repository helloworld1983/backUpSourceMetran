//$COMMON.H$
//    File Name: PdInterface.h
//    Copyright 1996 InVentive Technologies, Inc.  All Rights Reserved.
//
//    Class:
//			Patient Data Event Flags
//			Used by Breath Delivery to signal transitions of inhalation,
//			exhalation, and non-breathing mode phases.  An additional
//			timed event phase is used to time the processing of O2
//			concentration. 
/******************************************************************************/
#ifndef  PDINTERFACE_H
#define	 PDINTERFACE_H

// start of inhalation event, signals patient data
// to perform exhalation calculations
#define START_INH_EVENT_FLAG 0x01

// start of exhalation event, signals patient data
// to perform inhalation calculations
#define START_EXH_EVENT_FLAG 0x02

// start of timed event, signals patient data to 
// perform O2 calculations
#define START_TIMED_EVENT_FLAG 0x04

// Non breathing mode event (ie SVO), signals patient data to
// to blank patient data displays and data base.
#define NONBREATHING_MODE_EVENT_FLAG 0x08

// Bd's signal to Pd that the O2 settings are now available
#define O2_SETTING_EVENT_FLAG	0x10

// signals any patient data events

// Bd's signal to Pd that ready to process HFO data.
#define HFO_DATA_READY_EVENT_FLAG   0x20

// Bd's signal to Pd that ready to process Exhalation data.
#define EXH_DATA_READY_EVENT_FLAG   0x40

// Bd's signal to Pd that ready to exit standby mode
#define EXIT_STANDBYMODE_FLAG 0x80

// Bd's signal to Pd that ready to update APRV and NCPAP
#define UPDATE_APRV_BR_FLAG 0x100
#define UPDATE_NCPAP_DATA_FLAG 0x200

// APRV P_HIGH
#define APRV_PHIGH_START 0x400

// APRV P_HIGH
#define APRV_PLOW_START 0x800

// Bd's signal to Pd that ready to clear compensate
#define CLEAR_COMPENSATE_DATA 0x1000

// Bd's signal to Pd that ready to end inhalation of APRV
#define END_INH_PS_APRV 0x2000

// Bd's signal to Pd that ready to end exhalation of APRV
#define END_EXH_PS_APRV 0x4000

#define ANY_PT_DATA_EVENT_FLAG (START_INH_EVENT_FLAG|\
        START_EXH_EVENT_FLAG|\
        START_TIMED_EVENT_FLAG|\
        NONBREATHING_MODE_EVENT_FLAG|\
        O2_SETTING_EVENT_FLAG|\
        HFO_DATA_READY_EVENT_FLAG|\
        EXH_DATA_READY_EVENT_FLAG|\
        EXIT_STANDBYMODE_FLAG|\
        UPDATE_APRV_BR_FLAG|\
        UPDATE_NCPAP_DATA_FLAG|\
        APRV_PHIGH_START|\
        APRV_PLOW_START|\
        CLEAR_COMPENSATE_DATA|\
        END_INH_PS_APRV|\
        END_EXH_PS_APRV)

#endif  //PDINTERFACE_H 
