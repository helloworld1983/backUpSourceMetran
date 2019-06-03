/******************************************************************************/
//$COMMON.H$
//   File Name:  FaultMacro.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: FaultMacro
//
//
//   Revision History:
//
//      Rev:  Date:       Engineer:             Project:
//      05    05/20/98    Dan Seibert	    Morph
//      Description: Modified ASSERTION macro to call jump table vector
//          for Fault::S_SoftFailure(). 
//          The jump table provides fixed addresses for FLASH routines 
//          called from ROM.
//
//      Rev:  Date:       Engineer:             Project:
//      06    11/19/97    R. P. Rush		    Morph
//      Description: Added else clause to fix assertion check.  Makes ASSERTION
//					 macro behave as a function call
//
//      Rev:  Date:       Engineer:             Project:
//      05    08/15/97    R. L. Stephenson      Morph
//      Description: Added #ifndef NO_RS232 to disable the assertion check and
//      output to the RS-232 port.
//
//      Rev:  Date:       Engineer:             Project:
//      04    07/31/97    R. L. Stephenson      Morph
//      Description: Removed DOWNLOAD fault definitions since they are no
//      longer used.
//
//      Rev:  Date:       Engineer:             Project:
//      03    05/07/97      R. P. Rush          Morph
//      Description: Took out redundent semicolons on CLASS_FAILURE macro
//        creates compiler errors when redundent braces are not used.
//
//      Rev:  Date:       Engineer:             Project:
//      02    03/17/97    K. A. McGuigan        Morph
//      Description: Changed references from eClassPost to eClassPostCondition
//          to match fault.h and remove compiler errors.
//
//      Rev:  Date:       Engineer:             Project:
//      01    01/29/97    Jung Verheiden        Morph
//      Description: Initial Revision
//
/******************************************************************************/
// This file defines macros to check software failure conditions.

#ifndef    FaultMacro_H
#define FaultMacro_H

// Enums that define Software failure condition types.
enum E_ConditionId
{
    eClassPreCondition,
    eClassAssert,
    eClassPostCondition,
    eFunctionPreCondition,
    eFunctionAssert,
    eFunctionPostCondition
};

enum FaultTypes
{
    eSoftwareFault,
    eHardwareFault,
    eException
};

#include "Fault.h"

#if    defined(WIN32)
#include "assert.h"
#endif //defined(WIN32)

//define Software Failure handling macros
#if defined(WIN32)
#define CLASS_FAILURE(cId, expr ) \
    assert((int) expr);
#else
#ifndef NO_RS232
#define CLASS_FAILURE(cId, expr ) \
    if (!(expr)) Fault::J_SoftFailure(__FILE__, __LINE__, #expr, cId); else NULL
#else
// Disable the assertion check and the printout
#define CLASS_FAILURE(cId, expr) \
    expr

#endif //NO_RS232
#endif //defined(WIN32)

#define PRE_CONDITION(expr)  CLASS_FAILURE( eClassPreCondition, expr)
#define ASSERTION(expr)      CLASS_FAILURE( eClassAssert, expr)
#define POST_CONDITION(expr) CLASS_FAILURE( eClassPostCondition, expr)

#if    defined(PRODUCTION_BUILD)             
#define DEBUG_PRE_CONDITION(expr) 
#define DEBUG_ASSERTION(expr)
#define DEBUG_POST_CONDITION(expr)

#else

#define DEBUG_PRE_CONDITION(expr)  CLASS_FAILURE(eClassPreCondition, expr)
#define DEBUG_ASSERTION(expr)            CLASS_FAILURE(eClassAssert, expr)
#define DEBUG_POST_CONDITION(expr) CLASS_FAILURE(eClassPostCondition, expr)

#endif //defined(PRODUCTION_BUILD)
                
#endif //FaultMacro_H
