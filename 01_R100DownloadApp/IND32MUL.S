--*****************************************************************************/
--$COMMON.CPP$
--   File Name:  Ind32mul.s
--   Copyright 1998 Inventive Technologies,Inc. All Rights Reserved.
--
--   Purpose: 
--      This file contains the __MUL_32_32 routine.
--		This routine in normally part of the libarch.a library.
--		It has been extracted and modified to 
--
--   Interfaces:
--
--   Restrictions:
--      This file is located in the V851 PROM space.
--
--   Revision History:
--
--      Rev:  Date:     Engineer:           Project:
--      01    04/10/98  Dan Seibert		    Morph
--      Description: Extracted from libarch.a
--
--*****************************************************************************/

        .file    "ind32mul.s"
--
--*****************************************************************************/
--$COMMON.OPERATION$
--    Operation Name: _MUL_32_32
--
--    Processing: 
--      The following code is the multiply routine from libarch.a
--
--    Input Parameters:
--      r6, r7 - the two integers to be multiplied together
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
	    .globl  ___MUL_U16_U16
___MUL_U16_U16:
		add     -8, sp
		st.w    r6, 0[sp]
		st.w    r7, 4[sp]
		mov     r7, r10
		mulh    r6, r10
		shl     16, r6
		shl     16, r7
		bp      tmp1
		add     r6, r10
tmp1:	or      r6, r6
		bp      tmp2
		add     r7, r10
tmp2:	ld.w    4[sp], r7
		ld.w    0[sp], r6
		add     8, sp
		jmp     [lp]

	    .globl  ___MUL_S16_32
___MUL_S16_32:
		add     -8, sp
		st.w    r6, 0[sp]
		st.w    r7, 4[sp]
		mov     r7, r10
		mulh    r6, r10
		shr     16, r7
		mulh    r6, r7
		bnc     tmp3
		shl     16, r6
		add     r6, r10
tmp3:	shl     16, r7
		add     r7, r10
		ld.w    4[sp], r7
		ld.w    0[sp], r6
		add     8, sp
		jmp     [lp]

	    .globl  ___MUL_U16_32
___MUL_U16_32:
		add     -16, sp
		st.w    r6, 0[sp]
		st.w    r7, 4[sp]
		st.w    r8, 8[sp]
		st.w    r9, 0xc[sp]
		mov     r7, r10
		mulh    r6, r10
		mov     r7, r8
		shr     16, r8
		bnc     tmp4
		mov     r6, r9
		shl     16, r9
		add     r9, r10
tmp4:	mulh    r6, r8
		shl     16, r8
		add     r8, r10
		shr     16, r6
		bnc     tmp5
		shl     16, r7
		add     r7, r10
tmp5:	ld.w    0xc[sp], r9
		ld.w    8[sp], r8
		ld.w    4[sp], r7
		ld.w    0[sp], r6
		addi    0x10, sp, sp
		jmp     [lp]

	    .globl  ___MUL_32_32
___MUL_32_32:
		add     -16, sp
		st.w    r6, 0[sp]
		st.w    r7, 4[sp]
		st.w    r8, 8[sp]
		st.w    r9, 0xc[sp]
		mov     r6, r10
		mulh    r7, r10
		mov     r6, r8
		shr     16, r8
		bnc     tmp6
		mov     r7, r9
		shl     16, r9
		add     r9, r10
tmp6:	mulh    r7, r8
		shl     16, r8
		add     r8, r10
		shr     16, r7
		mulh    r6, r7
		bnc     tmp7
		shl     16, r6
		add     r6, r10
tmp7:	shl     16, r7
		add     r7, r10
		ld.w    0xc[sp], r9
		ld.w    8[sp], r8
		ld.w    4[sp], r7
		ld.w    0[sp], r6
		addi    0x10, sp, sp
		jmp     [lp]
		nop
