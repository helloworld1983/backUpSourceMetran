--*****************************************************************************/
--$COMMON.CPP$
--   File Name:  Ind32mod.s
--   Copyright 1998 Inventive Technologies,Inc. All Rights Reserved.
--
--   Purpose: 
--      This file contains the __UMOD_32_32() and __SMOD_32_32 routines.
--		These routines are normally part of the libarch.a library.
--		They have been extracted and placed into ROM as a performance
--		enhancement.
--
--   Interfaces:
--		Calls MUL_32_32(), UDIV_32_32, SDIV_32_32 
--
--   Restrictions:
--      This file is located in the V851 ROM space.
--
--   Revision History:
--
--      Rev:  Date:     Engineer:           Project:
--      01    05/05/98  Dan Seibert		    Morph
--      Description: Extracted from libarch.a
--
--*****************************************************************************/

        .file    "ind32mod.s"
--
--*****************************************************************************/
--$COMMON.OPERATION$
--    Operation Name: _UMOD_32_32,  _SMOD_32_32
--
--    Processing: 
--      The following code is the 32 bit mod routines from libarch.a
--
--    Input Parameters:
--      r6, r7 - the two unsigned integers to be operated upon.
--
--    Output Parameters:
--      None
--
--    Return Values:
--      r10 - the result
--
--    Pre-Conditions:
--      None
--
--    Miscellaneous:
--
--    Requirements:  
--
--*****************************************************************************/
	    .section ".romtext",.text
	    .align 4
	    .globl  ___SMOD_32_32
___SMOD_32_32:
   add     -16, sp
   st.w    r28, 8[sp]
   st.w    lp, 0xc[sp]
   st.w    r27, 4[sp]
   addi    8, sp, r28
   st.w    r29, 0[sp]
   mov     r7, r27
   mov     r6, r29
   mov     r7, r6
   mov     r29, r7
   jarl    ___SDIV_32_32, lp
   mov     r29, r6
   mov     r10, r7
   jarl    ___MUL_32_32, lp
   subr    r27, r10
   ld.w    4[sp], r27
   ld.w    0[sp], r29
   ld.w    0xc[sp], lp
   ld.w    8[sp], r28
   addi    0x10, sp, sp
   jmp     [lp]

	    .globl  ___UMOD_32_32
___UMOD_32_32:
     add     -16, sp
     st.w    r28, 8[sp]
     st.w    lp, 0xc[sp]
     st.w    r27, 4[sp]
     addi    8, sp, r28
     st.w    r29, 0[sp]
     mov     r7, r27
     mov     r6, r29
     mov     r7, r6
     mov     r29, r7
     jarl    ___UDIV_32_32, lp
     mov     r29, r6
     mov     r10, r7
     jarl    ___MUL_32_32, lp
     subr    r27, r10
     ld.w    4[sp], r27
     ld.w    0[sp], r29
     ld.w    0xc[sp], lp
     ld.w    8[sp], r28
     addi    0x10, sp, sp
     jmp     [lp]

