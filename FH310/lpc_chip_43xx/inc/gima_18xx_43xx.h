/*
 * @brief LPC18xx/43xx GIMA driver
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2012
 * All rights reserved.
 *
 * @par
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * LPC products.  This software is supplied "AS IS" without any warranties of
 * any kind, and NXP Semiconductors and its licenser disclaim any and
 * all warranties, express or implied, including all implied warranties of
 * merchantability, fitness for a particular purpose and non-infringement of
 * intellectual property rights.  NXP Semiconductors assumes no responsibility
 * or liability for the use of the software, conveys no license or rights under any
 * patent, copyright, mask work right, or any other intellectual property rights in
 * or to any products. NXP Semiconductors reserves the right to make changes
 * in the software without notification. NXP Semiconductors also makes no
 * representation or warranty that such application will be suitable for the
 * specified use without further testing or modification.
 *
 * @par
 * Permission to use, copy, modify, and distribute this software and its
 * documentation is hereby granted, under NXP Semiconductors' and its
 * licensor's relevant copyrights in the software, without fee, provided that it
 * is used in conjunction with NXP Semiconductors microcontrollers.  This
 * copyright, permission, and disclaimer notice must appear in all copies of
 * this code.
 */

#ifndef __GIMA_18XX_43XX_H_
#define __GIMA_18XX_43XX_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup GIMA_18XX_43XX CHIP: LPC18xx/43xx GIMA driver
 * @ingroup CHIP_18XX_43XX_Drivers
 * @{
 */

/** Bit location for Timer0 capture */
#define T0_CAP3_SELECT   0x01 << 4

/**
 * @brief Global Input Multiplexer Array (GIMA) register block structure
 */
typedef struct {						/*!< GIMA Structure */
	__IO uint32_t  CAP0_IN[4][4];		/*!< Timer x CAP0_y capture input multiplexer (GIMA output ((x*4)+y)) */
	__IO uint32_t  CTIN_IN[8];			/*!< SCT CTIN_x capture input multiplexer (GIMA output (16+x)) */
	__IO uint32_t  ADCHS_TRIGGER_IN;	/*!< ADCHS trigger input multiplexer (GIMA output 24) */
	__IO uint32_t  EVENTROUTER_13_IN;	/*!< Event router input 13 multiplexer (GIMA output 25) */
	__IO uint32_t  EVENTROUTER_14_IN;	/*!< Event router input 14 multiplexer (GIMA output 26) */
	__IO uint32_t  EVENTROUTER_16_IN;	/*!< Event router input 16 multiplexer (GIMA output 27) */
	__IO uint32_t  ADCSTART0_IN;		/*!< ADC start0 input multiplexer (GIMA output 28) */
	__IO uint32_t  ADCSTART1_IN;		/*!< ADC start1 input multiplexer (GIMA output 29) */
} LPC_GIMA_T;
/**
 * @}
 */

/**
 * note: Q_add
 * @brief	Select Tn_Capm as the input fo CAP_n_m in GIMA
 * @param	pGIMA       : pointer to GIMA base address
 * @param	timeNum		: Timer num
 * @param	matchnum	: Capture num
 */
STATIC INLINE void Chip_TIMER_Select_Capture(LPC_GIMA_T *pGIMA, int8_t timerNum, int8_t capNum)
{
	//TODO
	pGIMA->CAP0_IN[timerNum][capNum] |= T0_CAP3_SELECT;
	return;
}


#ifdef __cplusplus
}
#endif

#endif /* __GIMA_18XX_43XX_H_ */
