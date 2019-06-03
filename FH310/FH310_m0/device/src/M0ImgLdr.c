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

#ifndef ERROR_LED
/* LED to be blinked when there is an error in loading/starting M0 image */
#define ERROR_LED    1
#endif

#define EX_BLINKY          (1 << 0)
#define EX_USBHOST         (1 << 1)
#define EX_USBDEV          (1 << 2)
#define EX_LWIP            (1 << 3)
#define EX_EMWIN           (1 << 4)

#ifdef OS_UCOS_III
#define OS_SIGNATURE        3
#elif defined(OS_FREE_RTOS)
#define OS_SIGNATURE        2
#else
#define OS_SIGNATURE        1
#endif

#ifndef EXAMPLE_BLINKY
#define EXAMPLE_BLINKY      0
#else
#undef  EXAMPLE_BLINKY
#define EXAMPLE_BLINKY      EX_BLINKY
#endif

#ifndef EXAMPLE_USB_HOST
#define EXAMPLE_USB_HOST    0
#else
#undef  EXAMPLE_USB_HOST
#define EXAMPLE_USB_HOST    EX_USBHOST
#endif

#ifndef EXAMPLE_USB_DEVICE
#define EXAMPLE_USB_DEVICE  0
#else
#undef  EXAMPLE_USB_DEVICE
#define EXAMPLE_USB_DEVICE  EX_USBDEV
#endif

#ifndef EXAMPLE_LWIP
#define EXAMPLE_LWIP        0
#else
#undef  EXAMPLE_LWIP
#define EXAMPLE_LWIP        EX_LWIP
#endif

#ifndef EXAMPLE_EMWIN
#define EXAMPLE_EMWIN       0
#else
#undef  EXAMPLE_EMWIN
#define EXAMPLE_EMWIN       EX_EMWIN
#endif

#define EXAMPLES_INCLUDED (EXAMPLE_BLINKY + EXAMPLE_LWIP + EXAMPLE_USB_HOST + \
	EXAMPLE_USB_DEVICE + EXAMPLE_EMWIN)

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
 * Public types/enumerations/variables
 ****************************************************************************/
#ifdef CORE_M0
/* Structure object that identifies the image as M0 image */
/* NOTE: This symbol is used in startup file (only) */
const struct image_sig __M0Signature = {
	SIGNATURE_M0_MAGIC,	/* M0 Image magic signature */
	EXAMPLES_INCLUDED,
	OS_SIGNATURE,
	__DATE__,
	__TIME__,
};
#endif

/**
 * @}
 */
