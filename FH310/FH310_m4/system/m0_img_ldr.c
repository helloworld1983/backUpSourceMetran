/*
 * @brief M0 Image loader module
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2013
 * All rights reserved.
 *
 * @par
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * LPC products.  This software is supplied "AS IS" without any warranties of
 * any kind, and NXP Semiconductors and its licensor disclaim any and
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

#include <string.h>
#include "board.h"

/** @defgroup EXAMPLE_DUALCORE_CMN_M0LOADER LPC43xx M0 Image loader
 * @ingroup EXAMPLES_DUALCORE_43XX_COMMON
 * <b>Example description</b><br>
 * The M0 image loader is a common M0 bootloader that is used with all the
 * dual-core examples. The image loader is used by the M4 core to M0 boot
 * image is valid prior to starting the M0 core.<br>
 *
 * <b>Special connection requirements</b><br>
 * There are no special connection requirements for this example.<br>
 *
 * <b>Build procedures:</b><br>
 * @ref LPCOPEN_18XX43XX_BUILDPROCS_KEIL<br>
 * @ref LPCOPEN_18XX43XX_BUILDPROCS_IAR<br>
 * @ref LPCOPEN_18XX43XX_BUILDPROCS_LPCXPRESSO<br>
 *
 * <b>Supported boards and board setup:</b><br>
 * @ref LPCOPEN_18XX_BOARD_HITEX1850<br>
 * @ref LPCOPEN_43XX_BOARD_HITEX4350<br>
 * @ref LPCOPEN_18XX_BOARD_KEIL1857<br>
 * @ref LPCOPEN_43XX_BOARD_KEIL4357<br>
 * @ref LPCOPEN_18XX_BOARD_NGX1830<br>
 * @ref LPCOPEN_43XX_BOARD_NGX4330<br>
 *
 * <b>Submitting LPCOpen issues:</b><br>
 * @ref LPCOPEN_COMMUNITY
 * @{
 */

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

/* Magic numbers to identify M0/M4 images */
#define SIGNATURE_M4_MAGIC     0xF00D4BAD
#define SIGNATURE_M0_MAGIC     0xBEEFF00D

/* Image signature structure */
/* NOTE: Although the M0 structure created using this type is used in
 * the startup assembly file, the assembly file just needs the symbol and
 * not the type, hence keeping it as private structure.
 */
struct image_sig {
	uint32_t signature;		/* Signature M0/M4 image */
	uint32_t capability;	/* Examples included */
	uint32_t os;			/* OS used */
	int8_t build_date[32];	/* Build Date string */
	int8_t build_time[32];	/* Build Time string */
};

/*****************************************************************************
 * Public functions
 ****************************************************************************/

#ifdef CORE_M4
/* M0 Boot loader */
int M0Image_Boot(uint32_t m0_image_addr)
{
	/* Make sure the alignment is OK */
	if (m0_image_addr & 0xFFF) {
		return -1;
	}

	/* Make sure the M0 core is being held in reset via the RGU */
	Chip_RGU_TriggerReset(RGU_M0APP_RST);

	Chip_Clock_Enable(CLK_M4_M0APP);

	/* Keep in mind the M0 image must be aligned on a 4K boundary */
	Chip_CREG_SetM0AppMemMap(m0_image_addr);

	Chip_RGU_ClearReset(RGU_M0APP_RST);

	return 0;
}

#endif

/**
 * @}
 */
