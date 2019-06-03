/********************************************************************/
/*                                                                  */
/* File Name    : Lcd.c                                    			*/
/*                                                                  */
/* General       :Lcd configuration                                 */
/*                 	 						                        */
/*                                                                  */
/* Product Name  : FH310                                            */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2016/08/11      Quyen Ngo		  	new file        */
/* #0001       														*/
/*                                                                  */
/* #0002	   2017/09/06	   Quyen Ngo			modify			*/
/*	-clear frame buffer before setup in lcd_init function(bug1799)	*/
/* #0003       2017/09/29	   Quyen Ngo			modify			*/
/*	-add function to handle touch irq detection and call it from GUI*/
/*	task															*/
/* #0004       2017/10/07	   Quyen Ngo			modify			*/
/*	Change the way to handle touch IRQ for avoid the IRQ signal not	*/
/*	stable when read SPI data from touch controller.				*/
/* #0004       2017/10/07	   Quyen Ngo			modify			*/
/*	Add detect rising interrupt for touch IRQ signal to improve the	*/
/*	response of touch screen										*/
/* #0005       2017/10/12	   Viet Le			    modify			*/
/*	Add define TOUCH_INT_PORT and TOUCH_INT_BIT fix bug 1971		*/
/* #0006       2017/10/13	   Quyen Ngo			modify			*/
/*	Back to touch screen handle algorithm on V0101044				*/
/********************************************************************/


#include "Lcd.h"
#include "board.h"
#include "GUI.h"
#include "TaskCommon.h"
#include "DualCoreCommon.h"

static uint16_t *framebuffer = (uint16_t *) FRAMEBUFFER_ADDR;

extern bool display_Landscape;
extern bool display_MirrorY;

#define TOUCH_PININT_INDEX   5	/* PININT index used for GPIO mapping */
#define TOUCH_PININT_IRQ_HANDLER  GPIO5_IRQHandler	/* GPIO interrupt IRQ function name */
#define TOUCH_PININT_NVIC_NAME    PIN_INT5_IRQn	/* GPIO interrupt NVIC interrupt name */
#define TOUCH_INT_PORT			  GPIO_PORT(4)
#define TOUCH_INT_BIT			  GPIO_BIT(1)

/****************************************************************************************/
/*                                                                                      */
/* Function name 	-lcd_Init														    */
/*                                                                                      */
/* Details  -init the LCD							 									*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE					 					  							*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/* local valiables   : uint32_t timerFreq			                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void lcd_Init(void)
{
	Board_LCD_Init();
	Chip_LCD_Init(LPC_LCD, (LCD_CONFIG_T *) &EA4357_LCD);
	Board_InitTouchController();
	memset((void*) (framebuffer), 0x00, LCD_WIDTH * LCD_HEIGHT * LCD_NUM_BYTES_PER_PIXEL);
	Chip_LCD_SetUPFrameBuffer(LPC_LCD,  (void *) framebuffer);
	Chip_LCD_PowerOn(LPC_LCD);
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name 	-touch_Init														    */
/*                                                                                      */
/* Details  -init the touch						 									    */
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE					 					  							*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/* local valiables   : uint32_t timerFreq			                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void lcd_TouchInit(void)
{
	/* Configure GPIO pin as input */
	Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT, TOUCH_INT_PORT, TOUCH_INT_BIT);

	/* Configure interrupt channel for the GPIO pin in SysCon block */
	Chip_SCU_GPIOIntPinSel(TOUCH_PININT_INDEX, TOUCH_INT_PORT, TOUCH_INT_BIT);

	/* Configure channel interrupt as edge sensitive and falling edge interrupt */
	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(TOUCH_PININT_INDEX));
	Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH(TOUCH_PININT_INDEX));
	Chip_PININT_EnableIntLow(LPC_GPIO_PIN_INT, PININTCH(TOUCH_PININT_INDEX)); //falling edge

	/* Enable interrupt in the NVIC */
	NVIC_ClearPendingIRQ(TOUCH_PININT_NVIC_NAME);
	NVIC_SetPriority(TOUCH_PININT_NVIC_NAME,TOUCH_INT_PRIO);
	NVIC_EnableIRQ(TOUCH_PININT_NVIC_NAME);
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	PININT_IRQ_HANDLER									                */
/*                                                                                      */
/* Details:         Handle encoder PB GPIO interrupt				    				*/
/*                                                                                      */
/*                                                                                      */
/* ReturnValue : 	void		                                                        */
/*                                                                                      */
/* Using Global Data : 	uint8_t gs_stepVal			                           			*/
/* Local variable :  	uint8_t phaseBVal												*/
/*                                                                                      */
/****************************************************************************************/
void TOUCH_PININT_IRQ_HANDLER(void)
{
	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(TOUCH_PININT_INDEX));
	static GUI_PID_STATE StatePID;
	static int IsTouched = 0;
	int16_t tmp_x = -1, tmp_y = -1;

	if (Board_GetTouchPos(&tmp_x, &tmp_y)) {
		// The way emWin works forces the touch coordinates to be
		// transformed before they are handed over to emWin. This
		// transformation is based on the DISPLAY_ORIENTATION
		// define in LCDConf.c, but is stored in the global display_*
		// variables.
		if (display_Landscape) {
			if (display_MirrorY) {
				GUI_TOUCH_StoreState(LCD_HEIGHT - tmp_y, tmp_x);
			} else {
				GUI_TOUCH_StoreState(tmp_y, tmp_x);
			}
		} else {
			if (display_MirrorY) {
				GUI_TOUCH_StoreState(tmp_x, LCD_HEIGHT - tmp_y);
			} else {
				if(IsTouched == 0)
				{
					IsTouched = 1;

					StatePID.Pressed = 1;
					StatePID.x = tmp_x;
					StatePID.y = tmp_y;
					GUI_PID_StoreState(&StatePID);
				}
			}
		}
	}
	else
	{
		if(IsTouched == 1)
		{
			IsTouched = 0;
			StatePID.Pressed = 0;
			GUI_PID_StoreState(&StatePID);
		}
	}

	NVIC_DisableIRQ(TOUCH_PININT_NVIC_NAME);
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	lcd_TouchHandle									                    */
/*                                                                                      */
/* Details:         Enable touch irq					 			    				*/
/*                                                                                      */
/* Macros:		TOUCH_PININT_NVIC_NAME													*/
/*				TOUCH_SCR_INT_PRIOR														*/
/*                                                                                      */
/* ReturnValue : 	void		                                                        */
/*                                                                                      */
/* Using Global Data : 	NONE						                           			*/
/*                                                                                      */
/****************************************************************************************/
void lcd_TouchHandle(void)
{
	NVIC_SetPriority(TOUCH_PININT_NVIC_NAME,TOUCH_INT_PRIO);
	NVIC_EnableIRQ(TOUCH_PININT_NVIC_NAME);
	return;
}
