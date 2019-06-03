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


//#define PROXYFLOWTASK            //define in ProxyFlowTask
#define FLOWTASK                 //define in TestFlowSensorTask
#define FLOWTASKI2C1             //define in TestFlowSensorTask
//#define ADC                     //define in adc
//#define IPC
//#define IO_TASK                 //define in test digital IO
//#define HFO_MOTOR_EN                //define in hfo motor
//#define USB_EN

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

#ifdef IO_TASK
#define DEBUG_IO(...) Board_UARTDebug(__VA_ARGS__)
#else
#define DEBUG_IO(...)
#endif  //#define IPC

#ifdef HFO_MOTOR_EN
#define DEBUG_HFO(...) Board_UARTDebug(__VA_ARGS__)
#else
#define DEBUG_HFO(...)
#endif  //#define IPC

#ifdef USB_EN
#define DEBUG_USB(...) Board_UARTDebug(__VA_ARGS__)
#else
#define DEBUG_USB(...)
#endif  //#define IPC

#endif //define DEBUG_ENABLE
#endif /* DEBUGTRACE_H_ */
