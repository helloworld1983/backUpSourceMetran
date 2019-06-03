--*****************************************************************************/
--$COMMON.CPP$
--   File Name:  Ind32div.s
--   Copyright 1998 Inventive Technologies,Inc. All Rights Reserved.
--
--   Purpose: 
--      This file contains the __UDIV_32_32() and SDIV_32_32() routines.
--		These routines are normally part of the libarch.a library.
--		They have been extracted and placed into ROM as a performance
--		enhancement.
--
--   Interfaces:
--		Calls MUL_32_32() and CLZ32()
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

        .file    "ind32div.s"
--
--*****************************************************************************/
--$COMMON.OPERATION$
--    Operation Name: _DIV_32_32
--
--    Processing: 
--      The following code is the 32 bit divide routine from libarch.a
--
--    Input Parameters:
--      r6, r7 - the two integers to be divided.
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
	    .globl  ___UDIV_32_32
___UDIV_32_32:
   addi    0xffffffd8, sp, sp
   st.w    lp, 0x24[sp]
   st.w    r25, 0x1c[sp]
   st.w    r26, 0x18[sp]
   st.w    r27, 0x14[sp]
   st.w    r28, 0x20[sp]
   st.w    r29, 0x10[sp]
   addi    0x20, sp, r28
   cmp     zero, r7
   bne     tmp1
   mov     -1, r10		-- if divide by 0, return -1
   jr      done
tmp1:
   cmp     r7, r6		
   bh      tmp2
   cmp     r7, r6		-- if r7 < r6, return 0
   setfz   r10			-- if r7 = r6, return 1
   jr      done
tmp2:
   mov     r6, r27		-- r6 > r7
   st.w    r7, 0xc[sp]
   mov     r6, r12
   shr     31, r12
   mov     r7, r10
   shr     15, r10
   or      r12, r10
   bne     tmp3
   jarl    __divh, lp
   br      done
tmp3:
   ld.w    0xc[sp], r6
   mov     0, r14
   st.w    r14, 8[sp]
   mov     0, r29
   jarl    ___CLZ32, lp
   ld.w    0xc[sp], r14
   andi    0xffff, r10, r8
   shl     r8, r14
   st.w    r14, 0xc[sp]
   cmp     zero, r8
   be      tmp4
   shl     r8, r29
   movea   0x20, zero, r9
   sub     r8, r9
   mov     r27, r7
   shr     r9, r7
   or      r7, r29
   shl     r8, r27
tmp4:
   ld.w    0xc[sp], r14
   mov     r14, r26
   mov     r14, r25
   andi    0xffff, r14, r14
   st.w    r14, 0[sp]
   ld.w    0xc[sp], r14
   shl     16, r26
   shr     17, r14
   st.w    r14, 0xc[sp]
   st.h    r14, 4[sp]
   shr     16, r25
   mov     1, r14
tmp8:
   st.h    r14, 6[sp]
   mov     r29, r6
   ld.h    4[sp], r7
   shr     2, r6
   jarl    __divh, lp
   shl     1, r10
   addi    1, r10, r7
   mov     r26, r6
   jarl    ___MUL_32_32, lp
   mov     r10, r8
   mov     r25, r6
   jarl    ___MUL_32_32, lp
   ld.w    0[sp], r6
   mov     r10, r9
   jarl    ___MUL_32_32, lp
   shr     16, r10
   add     r9, r10
   cmp     r8, r27
   setfc   r11
   add     r10, r11
   sub     r11, r29
   ld.w    8[sp], r14
   sub     r8, r27
   add     r7, r14
   st.w    r14, 8[sp]
   br      tmp6
   ld.w    8[sp], r14
tmp5:
   mov     r26, r7
   add     -1, r14
   st.w    r14, 8[sp]
   add     r27, r7
   cmp     r27, r7
   setfc   r9
   add     r25, r9
   add     r9, r29
   mov     r7, r27
tmp6:
   movhi   0x8000, zero, r6
   mov     r29, r7
   and     r6, r7
   bne     tmp5
   ld.h    6[sp], r14
   andi    0xffff, r14, r14
   be      tmp7
   shl     16, r29
   mov     r27, r12
   shr     16, r12
   or      r12, r29
   ld.w    8[sp], r14
   shl     16, r27
   shl     16, r14
   st.w    r14, 8[sp]
   mov     0, r14
   br      tmp8
tmp7:
   ld.w    8[sp], r10
done:
   ld.w    0x1c[sp], r25
   ld.w    0x18[sp], r26
   ld.w    0x14[sp], r27
   ld.w    0x10[sp], r29
   ld.w    0x24[sp], lp
   ld.w    0x20[sp], r28
   addi    0x28, sp, sp
   jmp     [lp]

	    .globl  ___SDIV_32_32
___SDIV_32_32:
   add     -12, sp
   st.w    r28, 4[sp]
   st.w    lp, 8[sp]
   st.w    r29, 0[sp]
   addi    4, sp, r28
   mov     r7, r11
   sar     31, r7
   mov     r7, r29
   mov     r6, r9
   sar     31, r6
   xor     r6, r29
   mov     r6, r8
   xor     r9, r8
   sub     r6, r8
   mov     r8, r6
   mov     r7, r9
   xor     r11, r9
   sub     r7, r9
   mov     r9, r7
   jarl    ___UDIV_32_32, lp
   mov     r29, r6
   xor     r10, r6
   sub     r29, r6
   ld.w    0[sp], r29
   ld.w    4[sp], r28
   ld.w    8[sp], lp
   mov     r6, r10
   add     12, sp
   jmp     [lp]

__divh:
   add     -8, sp
   st.w    lp, 4[sp]
   st.w    r28, 0[sp]
   divh    r7, r6
   ld.w    4[sp], lp
   ld.w    0[sp], r28
   mov     r6, r10
   add     8, sp
   jmp     [lp]
