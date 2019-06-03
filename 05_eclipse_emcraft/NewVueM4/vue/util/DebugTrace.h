/*
 * DebugTrace.h
 *
 *  Created on: 11-10-2018
 *      Author: Quyen Ngo
 */

#ifndef DEBUGTRACE_H_
#define DEBUGTRACE_H_

#include "fsl_debug_console.h"
#ifdef DEBUG_ENABLE             //it is defined in board.h file
#define ASSERT_EN               //define for using assert function
//#define DEBUG_ADCSENSOR_EN
//#define DEBUG_I2CSENSOR_EN
//#define DEBUG_DIGITALIO_EN
//#define DEBUG_RS485SENSOR_EN
//#define DEBUG_MOTORCONTROLLER_EN

#ifdef ASSERT_EN
#include "assert.h"
#include "stdlib.h"
#include "stdbool.h"
#define ASSERTION( x ) if( ( x ) == false ) { PRINTF("Error at file: %s \n\t\tFunction: %s \n\t\tLine %d\n",__FILE__, __func__, __LINE__ );\
                for( ;; ); }
#endif
#else
#define ASSERTION(...)
#endif  //define ASSERT_EN

#ifdef DEBUG_ADCSENSOR_EN
#define DEBUG_ADCSENSOR(...) PRINTF(__VA_ARGS__)
#else
#define DEBUG_ADCSENSOR(...)
#endif

#ifdef DEBUG_I2CSENSOR_EN
#define DEBUG_I2CSENSOR(...) PRINTF(__VA_ARGS__)
#else
#define DEBUG_I2CSENSOR(...)
#endif

#ifdef DEBUG_DIGITALIO_EN
#define DEBUG_DIGITALIO(...) PRINTF(__VA_ARGS__)
#else
#define DEBUG_DIGITALIO(...)
#endif

#ifdef DEBUG_RS485SENSOR_EN
#define DEBUG_RS485SENSOR(...) PRINTF(__VA_ARGS__)
#else
#define DEBUG_RS485SENSOR(...)
#endif

#ifdef DEBUG_MOTORCONTROLLER_EN
#define DEBUG_MOTORCONTROLLER(...) PRINTF(__VA_ARGS__)
#else
#define DEBUG_MOTORCONTROLLER(...)
#endif
#endif /* DEBUGTRACE_H_ */
