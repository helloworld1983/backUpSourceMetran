--//******************************************************************************//
--//$COMMON.CPP$
--//   File Name:  maskableinterrupts.s
--//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
--//
--//   Purpose: This file contains assembly language utilities.
--//		for enabling and disabling individual interrupts
--//			_EnableMaskableInt:  this routine clears the XXMKn bit (bit 6)
--//						 of the byte at the address specified
--//			_DisableMaskableInt:  this routine sets the XXMKn bit (bit 6)
--//						 of the byte at the address specified
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
--//      01    12/17/97    Dan S. 				Morph
--//      Description: Initial Revision
--//
--//******************************************************************************

--//******************************************************************************
--//$COMMON.OPERATION$
--//    Operation Name: EnableMaskableInt()
--//
--//    Processing: This routine clears the XXMKn bit (bit 6)
--//			    of the byte at the address specified 
--//
--//
--//    Input Parameters:
--//			r6 - address of bitfield
--//
--//    Output Parameters: None 
--//
--//    Return Values: None 
--//
--//    Pre-Conditions: None
--//
--//    Miscellaneous: None
--//
--//    Requirements:  
--//
--//******************************************************************************
--void  EnableMaskableInt(UCHAR *addr)
--{
	.text
	.globl _EnableMaskableInt
_EnableMaskableInt:
    clr1  	6,0[r6]      -- perform test and set instruction
	jmp		[lp]
--}


--//******************************************************************************
--//$COMMON.OPERATION$
--//    Operation Name: DisableMaskableInt()
--//
--//    Processing: This routine sets the XXMKn bit (bit 6)
--//				of the byte at the address specified 
--//
--//    Input Parameters:
--//			r6 - address of bitfield
--//
--//    Output Parameters: None 
--//
--//    Return Values: None 
--//
--//    Pre-Conditions: None
--//
--//    Miscellaneous: None
--//
--//    Requirements:  
--//
--//******************************************************************************
--void  DisableMaskableInt(UCHAR *addr)
--{
	.text
	.globl _DisableMaskableInt
_DisableMaskableInt:
    set1  	6,0[r6]      -- perform test and set instruction
	jmp		[lp]
--}

