#ifndef DELAY_H
#define DELAY_H

/********************************************************************/
/*                                                                  */
/* File Name    : Delay.c                       					*/
/*                                                                  */
/* General       : Support delay procedure in case of OS running 	*/
/*				   or stop running as well							*/
/*                 	 						                        */
/*                                                                  */
/* Product Name  : FH310                                            */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2016/08/22      Viet Le			  	new file        */
/* #0001       														*/
/*                                                                  */
/********************************************************************/

//Hard delay
void delay_HardDelay(unsigned long ms);
//Soft delay
void delay_SoftDelay(unsigned long ms);
//Delay miliseconds
void delay_MS(unsigned long ms);

#endif /* DELAY_H */
/* end of file */
