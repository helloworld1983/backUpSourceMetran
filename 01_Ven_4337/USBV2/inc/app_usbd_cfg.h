#ifndef APP_USBD_CFG_H_
#define APP_USBD_CFG_H_

#include "lpc_types.h"
#include "error.h"
#include "usbd_rom_api.h"

/* Comment below and uncomment USE_USB1 to enable USB1 */
#define USE_USB0
//#define USE_USB1

/* Manifest constants used by USBD ROM stack. These values SHOULD NOT BE CHANGED
   for advance features which require usage of USB_CORE_CTRL_T structure.
   Since these are the values used for compiling USB stack.
 */
#define USB_MAX_IF_NUM          8       /*!< Max interface number used for building USBD ROM. DON'T CHANGE. */
#define USB_MAX_EP_NUM          6       /*!< Max number of EP used for building USBD ROM. DON'T CHANGE. */
#define USB_MAX_PACKET0         64      /*!< Max EP0 packet size used for building USBD ROM. DON'T CHANGE. */
#define USB_FS_MAX_BULK_PACKET  64      /*!< MAXP for FS bulk EPs used for building USBD ROM. DON'T CHANGE. */
#define USB_HS_MAX_BULK_PACKET  512     /*!< MAXP for HS bulk EPs used for building USBD ROM. DON'T CHANGE. */
#define USB_DFU_XFER_SIZE       2048    /*!< Max DFU transfer size used for building USBD ROM. DON'T CHANGE. */

/* Manifest constants to select appropriate USB instance */
#ifdef USE_USB0
#define LPC_USB_BASE            LPC_USB0_BASE
#define LPC_USB                 LPC_USB0
#define LPC_USB_IRQ             USB0_IRQn
#define USB_IRQHandler          USB0_IRQHandler
#define USB_init_pin_clk        Chip_USB0_Init
#else
#define LPC_USB_BASE            LPC_USB1_BASE
#define LPC_USB                 LPC_USB1
#define LPC_USB_IRQ             USB1_IRQn
#define USB_IRQHandler          USB1_IRQHandler
#define USB_init_pin_clk        Chip_USB1_Init
#endif

#define LUSB_PID                        0x8A
#define LUSB_IN_EP                      0x81
#define LUSB_OUT_EP                     0x01
#define LUSB_INT_EP                     0x82

/* On LPC18xx/43xx the USB controller requires endpoint queue heads to start on
   a 4KB aligned memory. Hence the mem_base value passed to USB stack init should
   be 4KB aligned. The following manifest constants are used to define this memory.
 */
#define USB_STACK_MEM_BASE      0x20000000
#define USB_STACK_MEM_SIZE      0x00002000

/* USB descriptor arrays defined *_desc.c file */
extern const uint8_t USB_DeviceDescriptor[];
extern uint8_t USB_HsConfigDescriptor[];
extern uint8_t USB_FsConfigDescriptor[];
extern const uint8_t USB_StringDescriptor[];
extern const uint8_t USB_DeviceQualifier[];
extern const uint8_t WCID_String_Descriptor[];
extern const uint8_t WCID_CompatID_Descriptor[];



#endif /* APP_USBD_CFG_H_ */
