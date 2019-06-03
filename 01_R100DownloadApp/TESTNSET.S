--//******************************************************************************//
--//$COMMON.CPP$
--//   File Name:  testnset.s
--//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
--//
--//   Purpose: This file contains assembly language utilities.
--//			_TestnSet:  this routine sets a bit and returns whether the bit
--//						was previously set or not.
--//
--//   Interfaces: None
--//
--//   Restrictions: None
--//
--//   Revision History:
--//      Rev:  Date:     Engineer:           Project:
--//      02    02/23/98  R.P. Rush           Morph
--//      Description: SCR-149. Added Code review comments
--//
--//      Rev:  Date:   	Engineer:     		Project:
--//      01    02/07/97    Dan S.  			Morph
--//      Description: Initial Revision
--//
--//******************************************************************************

--//******************************************************************************
--//$COMMON.OPERATION$
--//    Operation Name: TestAndSet()
--//
--//    Processing: This routine sets bit 0 of the given address.
--//				and returns whether the bit was previously set or not.
--//			This routine is used as a poor mans race condition handler.
--//
--//
--//    Input Parameters:
--//			r6 - address of bitfield
--//
--//    Output Parameters: Value at addr may be modified.
--//
--//    Return Values: r10  E_Bool
--//			eTrue  if bit was previously 0
--//			eFalse if bit was previously 1
--//
--//    Pre-Conditions: None
--//
--//    Miscellaneous: None
--//
--//    Requirements:  
--//
--//******************************************************************************
--E_Bool  TestAndSet(UCHAR *addr)
--{
	.text
	.globl _TestAndSet
_TestAndSet:
    set1  	0,0[r6]      -- perform test and set instruction
    bz  	_ITWASSET    -- if condition code z=1 bit was previously 0, jump
	mov		r0,r10	     -- ccode z=0, bit was already set, return false
	jmp		[lp]
_ITWASSET:
	mov 	1,r10	     -- condition code z=1, bit was prev. 0, return True
	jmp		[lp]
--}

