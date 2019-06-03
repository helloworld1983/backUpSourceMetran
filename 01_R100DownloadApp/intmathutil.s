--//**************************************************************************//
--//$COMMON.CPP$
--//   File Name:  intmathutil.s
--//   Copyright 1999 Respironics, Inc. All Rights Reserved.
--//
--//   Purpose: This file contains assembly language utilities used for integer
--//            integer math.
--//            _AddSat:  this routine adds two signed integers and returns an
--//                      unsigned integer as the result.  If the result exceeds
--//                      the maximum positive value 0x7FFFFFFH, 0x7FFFFFFH is 
--//                      returned.  If the result exceeds the maximum negative
--//                      value 0x80000000H, 0x80000000H is returned.
--//
--//   Interfaces: None
--//
--//   Restrictions: None
--//
--//   Revision History:
--//      Rev:  Date:       Engineer:           Project:
--//      01    01/14/99    K. A. McGuigan      Morph
--//      Description: SCR 418 - Initial Revision
--//
--//****************************************************************************

--//****************************************************************************
--//$COMMON.OPERATION$
--//    Operation Name: AddSat()
--//
--//    Processing: This routine adds two signed integers and returns an
--//        unsigned integer as the result.  If the result exceeds
--//        the maximum positive value 0x7FFFFFFH, 0x7FFFFFFH is 
--//        returned.  If the result exceeds the maximum negative
--//        value 0x80000000H, 0x80000000H is returned.
--//
--//    Input Parameters:
--//            r6, r7 - values to be added
--//
--//    Output Parameters: None
--//
--//    Return Values: r10  LONG - sum of r6 and r7 contents
--//
--//    Pre-Conditions: None
--//
--//    Miscellaneous: This routine does not check or clear the SAT flag.  The
--//        caller of this routine does not care if the value actually 
--//        overflowed; the caller only wants to prevent the overflow.
--//
--//    Requirements:  
--//
--//****************************************************************************
--LONG AddSat(LONG, LONG)
--{
    .text
    .globl _AddSat
_AddSat:
    satadd  r6,r7        -- perform saturated add instruction
    mov     r7,r10       -- put the answer in r10
    jmp     [lp]
--}
