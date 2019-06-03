#ifndef INC_LCD_H_
	#define INC_LCD_H_

/********************************************************************/
/*                                                                  */
/* File Name    : Lcd.h                                    			*/
/*                                                                  */
/* General       : Lcd configuration 		                        */
/*                 	 						                        */
/*                                                                  */
/* Product Name  : FH310                                            */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2016/09/20     Quyen Ngo  	    new file        	*/
/*                                                                  */
/*#0001		   2017/09/06	  Quyen Ngo			modify				*/
/*		-Add define	LCD_NUM_BYTES_PER_PIXEL							*/
/*		-Remove file lcd_TouchHandle not use when change to new 	*/
/*		method for handle irq touch handle							*/
/*#0002		   2017/10/14	  Quyen Ngo			modify				*/
/*		-Back to touch screen handle alorithm on V0101044			*/
/********************************************************************/

#define LCD_WIDTH       		 	800
#define LCD_HEIGHT     			 	480
#define LCD_NUM_BYTES_PER_PIXEL		2

//Initialize LCD
void lcd_Init(void);

//Initialize touch
void lcd_TouchInit(void);

void lcd_TouchHandle(void);

#endif /* INC_LCD_H_ */
