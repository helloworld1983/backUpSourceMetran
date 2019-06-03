/********************************************************************/
/*                                                                  */
/* File Name    : IRQHandler.h                                		*/
/*                                                                  */
/* General      : This block contains function handle for different */
/*					IRQ which are used in bootloader				*/
/* 								                                    */
/*                 	 						                        */
/*                                                                  */
/* Product Name  : Portable                                         */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2016/10/25       Thao Ha		  	    new file        */
/*                                                                  */
/********************************************************************/
#ifndef INC_IRQHANDLE_H_
#define INC_IRQHANDLE_H_


void IRQ_timer0Init(void);
void IRQ_timer1Init(void);
void IRQ_timer2Init(void);
void IRQ_timer3Init(void);

void IRQ_timer2Enalble(void);

void IRQ_timer0Disable(void);
void IRQ_timer1Disable(void);
void IRQ_timer2Disable(void);
void IRQ_timer3Disable(void);



#endif /* INC_IRQHANDLE_H_ */
