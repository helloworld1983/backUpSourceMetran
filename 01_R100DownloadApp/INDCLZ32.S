--*****************************************************************************/
--$COMMON.CPP$
--   File Name:  Indclz32.s
--   Copyright 1998 Inventive Technologies,Inc. All Rights Reserved.
--
--   Purpose: 
--      This file contains the ___CLZ32() routine.
--		This routine in normally part of the libarch.a library.
--		It has been extracted and moved into the v851 on-chip ROM
--		as a performance enhancement.
--
--   Interfaces:  None
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
--
--*****************************************************************************/
--$COMMON.OPERATION$
--    Operation Name: ___CLZ32
--
--    Processing: 
--      The following code is the clz32() routine from libarch.a
--
--    Input Parameters:
--      r6
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
        .file    "indclz32.s"
	    .section ".romtext",.text
	    .align 4
	    .globl  ___CLZ32
___CLZ32:
   add     -8, sp
   st.w    r28, 0[sp]
   st.w    lp, 4[sp]
   mov     sp, r28
   mov     r6, r7
   cmp     zero, r6
   bne     tmp1
   movea   0x20, zero, r10
   br      done
tmp1:
   mov     0, r6
   mov     r7, r12
   shr     16, r12
   bne     tmp2
   shl     16, r7
   movea   0x10, zero, r6
tmp2:
   mov     r7, r11
   shr     24, r11
   bne     tmp3
   shl     8, r7
   add     8, r6
tmp3:
   mov     r7, r10
   shr     28, r10
   bne     tmp4
   shl     4, r7
   add     4, r6
tmp4:
   mov     r7, r9
   shr     30, r9
   bne     tmp5
   shl     2, r7
   add     2, r6
tmp5:
   shr     31, r7
   bne     tmp6
   add     1, r6
tmp6:
   mov     r6, r10
done:
   ld.w    4[sp], lp
   ld.w    0[sp], r28
   add     8, sp
   jmp     [lp]
