
#ifndef INC_BATTERY_H_
#define INC_BATTERY_H_
/********************************************************************/
/*                                                                  */
/* File Name    : battery.h		                                 	*/
/*                                                                  */
/* General      : battery command to get current status of smart	*/
/* 				battery												*/
/*                 	 						                        */
/*                                                                  */
/* Product Name  : Portable                                         */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*            April 18,2017     Thao Ha(MV)		  	    new file    */
/*                                                                  */
/********************************************************************/
typedef enum
{
	eCarCharging,
	eACCharging,
	eStopCharging
} E_ChargeType;
void battery_ChangeInputCurrentLimit(E_ChargeType chargeType);
void battery_UpdateChargeInputCurrentLimit(void);



#endif /* INC_BATTERY_H_ */
