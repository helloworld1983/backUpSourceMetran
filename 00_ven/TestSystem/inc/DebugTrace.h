/******************************************************************************/
//$COMMON.H$
//   File Name:  DebugTrace.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class:
//      This file contains all define macro for debug
//
/******************************************************************************/
#ifndef DEBUGTRACE_H_
#define DEBUGTRACE_H_

#include "board.h"

#ifdef DEBUG_ENABLE             //it is defined in board.h file

//Define all macro to debug, using in program
#define ASSERT_EN               //define for using assert function
//#define MAIN_DEBUG_EN           //debug in main program
//#define ALARM_DEBUG_EN          //debug in Alarm task
//#define BD_DEBUG_EN             //debug in Breath Delivery(DB) task
//#define PD_DEBUG_EN             //debug in Patient Data(PD) task
//#define DEBUG_EVENT_GROUP_EN    //debug in event group class
//#define TRIGGER_EN              //debug in Trigger module
//#define MODE_EN                 //define in Mode module
//#define OCCL_MODE_EN            //define in OcclMode
//#define AC_MODE_EN              //define in AcMode

#define PROXYFLOWTASK           //define in ProxyFlowTask
//#define FLOWTASK           //define in TestFlowSensorTask
//#define FLOWTASKI2C1           //define in TestFlowSensorTask
//#define ADC                     //define in adc
//#define IPC

//expand macro
#ifdef ASSERT_EN
#include "assert.h"
#include "stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#define ASSERTION( x ) if( ( x ) == pdFALSE ) { taskDISABLE_INTERRUPTS();\
                Board_UARTDebug("Error at file: %s \n\t\tFunction: %s \n\t\tLine %d\n",__FILE__, __func__, __LINE__ );\
                for( ;; ); }
#else
#define ASSERTION(...)
#endif  //define ASSERT_EN

#ifdef MAIN_DEBUG_EN
#define DEBUG_MAIN(...) Board_UARTDebug(__VA_ARGS__)
#else
#define DEBUG_MAIN(...)
#endif  //#define MAIN_DEBUG_EN

#ifdef ALARM_DEBUG_EN
#define DEBUG_ALARM(...) Board_UARTDebug(__VA_ARGS__)
#else
#define DEBUG_ALARM(...)
#endif  //#define ALARM_DEBUG_EN

#ifdef BD_DEBUG_EN
#define DEBUG_BD(...) Board_UARTDebug(__VA_ARGS__)
#else
#define DEBUG_BD(...)
#endif  //#define BD_DEBUG_EN

#ifdef PD_DEBUG_EN
#define DEBUG_PD(...) Board_UARTDebug(__VA_ARGS__)
#else
#define DEBUG_PD(...)
#endif  //#define PD_DEBUG_EN

#ifdef DEBUG_EVENT_GROUP_EN
#define DEBUG_EVENT_GROUP(...) Board_UARTDebug(__VA_ARGS__)
#else
#define DEBUG_EVENT_GROUP(...)
#endif  //#define DEBUG_EVENT_GROUP_EN

#ifdef TRIGGER_EN
#define DEBUG_TRIGGER(...) Board_UARTDebug(__VA_ARGS__)
#else
#define DEBUG_TRIGGER(...)
#endif  //#define TRIGGER_EN

#ifdef MODE_EN
#define DEBUG_MODE(...) Board_UARTDebug(__VA_ARGS__)
#else
#define DEBUG_MODE(...)
#endif  //#define MODE_EN

#ifdef OCCL_MODE_EN
#define DEBUG_OCCL_MODE(...) Board_UARTDebug(__VA_ARGS__)
#else
#define DEBUG_OCCL_MODE(...)
#endif  //#define OCCL_MODE_EN

#ifdef AC_MODE_EN
#define DEBUG_AC_MODE(...) Board_UARTDebug(__VA_ARGS__)
#else
#define DEBUG_AC_MODE(...)
#endif  //#define DEBUG_AC_MODE

#ifdef PROXYFLOWTASK
#define DEBUG_PROXYFLOWTASK(...) Board_UARTDebug(__VA_ARGS__)
#else
#define DEBUG_PROXYFLOWTASK(...)
#endif  //#define PROXYFLOWTASK

#ifdef FLOWTASK
#define DEBUG_FLOWTASK(...) Board_UARTDebug(__VA_ARGS__)
#else
#define DEBUG_FLOWTASK(...)
#endif  //#define PROXYFLOWTASK

#ifdef FLOWTASKI2C1
#define DEBUG_FLOWTASKI2C1(...) Board_UARTDebug(__VA_ARGS__)
#else
#define DEBUG_FLOWTASKI2C1(...)
#endif  //#define FLOWTASKI2C1

#ifdef ADC
#define DEBUG_ADC(...) Board_UARTDebug(__VA_ARGS__)
#else
#define DEBUG_ADC(...)
#endif  //#define ADC

#ifdef IPC
#define DEBUG_IPC(...) Board_UARTDebug(__VA_ARGS__)
#else
#define DEBUG_IPC(...)
#endif  //#define IPC

#endif //define DEBUG_ENABLE
#endif /* DEBUGTRACE_H_ */
