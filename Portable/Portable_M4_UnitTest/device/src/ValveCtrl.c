/********************************************************************/
/*                                                                  */
/* File Name    : ValveCtrl.c    	                    	        */
/*                                                                  */
/* General      : contain function to control valves                */
/*                 	 						                        */
/*                                                                  */
/* Product Name  : Portable                                         */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*       April 18, 2017     Thao Ha(MV)  		new file        	*/
/*                                                                  */
/********************************************************************/
#include <assert.h>
#include <Gpio.h>
#include <ValveCtrl.h>
static E_ValStatus gs_val1Status = VAL_OFF;
static E_ValStatus gs_val2Status = VAL_OFF;
static E_ValStatus gs_val3Status = VAL_OFF;
//#define DEBUG_VALVE_EN
#ifdef DEBUG_VALVE_EN
#define DEBUG_VALVE(...) printf(__VA_ARGS__)
#else
#define DEBUG_VALVE(...)
#endif
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	valve_Init								              				*/
/*                                                                                      */
/* Details:        Init GPIO for valve control and set default value for vale			*/
/*                                                                                      */
/* Arguments:	None																	*/
/*                                                                                      */
/* ReturnValue : void			                                                        */
/*                                                                                      */
/****************************************************************************************/
void valve_Init(void)
{
	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT,VAL1_PORT_NUM, VAL1_BIT_NUM);		/* output */
	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT,VAL2_PORT_NUM, VAL2_BIT_NUM);		/* output */
	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT,VAL3_PORT_NUM, VAL3_BIT_NUM);		/* output */

	// Init value for out put pin
	Chip_GPIO_SetPinOutLow(LPC_GPIO_PORT,VAL1_PORT_NUM, VAL1_BIT_NUM);
	Chip_GPIO_SetPinOutLow(LPC_GPIO_PORT,VAL2_PORT_NUM, VAL2_BIT_NUM);
	Chip_GPIO_SetPinOutLow(LPC_GPIO_PORT,VAL3_PORT_NUM, VAL3_BIT_NUM);
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	valve_getStatus							              				*/
/*                                                                                      */
/* Details:    Get current Valve status													*/
/*                                                                                      */
/* Arguments:	E_ValveNum valNum														*/
/*                                                                                      */
/* ReturnValue : E_ValStatus	                                                        */
/*                                                                                      */
/****************************************************************************************/
E_ValStatus valve_getStatus(E_ValveNum valNum)
{
	E_ValStatus value = VAL_OFF;
	switch (valNum) {
	case VAL1:
		value = gs_val1Status;
		break;
	case VAL2:
		value = gs_val2Status;
		break;
	case VAL3:
		value = gs_val3Status;
		break;
	default:
		assert(false);
		break;
	}
	return value;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	valve_openVal1							              				*/
/*                                                                                      */
/* Details:    Open Valve 1																*/
/*                                                                                      */
/* Arguments:	None																	*/
/*                                                                                      */
/* ReturnValue : None			                                                        */
/*                                                                                      */
/****************************************************************************************/
void valve_openVal1()
{
	gs_val1Status = VAL_ON;
	Chip_GPIO_SetPinOutHigh(LPC_GPIO_PORT,VAL1_PORT_NUM, VAL1_BIT_NUM);
	DEBUG_VALVE("Open Val 1\n");
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	valve_openVal2							              				*/
/*                                                                                      */
/* Details:    Open Valve 2																*/
/*                                                                                      */
/* Arguments:	None																	*/
/*                                                                                      */
/* ReturnValue : None			                                                        */
/*                                                                                      */
/****************************************************************************************/
void valve_openVal2()
{
	gs_val2Status = VAL_ON;
	Chip_GPIO_SetPinOutHigh(LPC_GPIO_PORT,VAL2_PORT_NUM, VAL2_BIT_NUM);
	DEBUG_VALVE("Open Val 2\n");
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	valve_openVal3							              				*/
/*                                                                                      */
/* Details:    Open Valve 3																*/
/*                                                                                      */
/* Arguments:	None																	*/
/*                                                                                      */
/* ReturnValue : None			                                                        */
/*                                                                                      */
/****************************************************************************************/
void valve_openVal3()
{
	gs_val3Status = VAL_ON;
	Chip_GPIO_SetPinOutHigh(LPC_GPIO_PORT,VAL3_PORT_NUM, VAL3_BIT_NUM);
	DEBUG_VALVE("Open Val 3\n");
}


/****************************************************************************************/
/*                                                                                      */
/* Function name: 	valve_closeVal1							              				*/
/*                                                                                      */
/* Details:    Close Valve 1															*/
/*                                                                                      */
/* Arguments:	None																	*/
/*                                                                                      */
/* ReturnValue : None			                                                        */
/*                                                                                      */
/****************************************************************************************/
void valve_closeVal1()
{
	gs_val1Status = VAL_OFF;
	Chip_GPIO_SetPinOutLow(LPC_GPIO_PORT,VAL1_PORT_NUM, VAL1_BIT_NUM);
	DEBUG_VALVE("Close Val 1\n");
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	valve_closeVal2							              				*/
/*                                                                                      */
/* Details:    Close Valve 2															*/
/*                                                                                      */
/* Arguments:	None																	*/
/*                                                                                      */
/* ReturnValue : None			                                                        */
/*                                                                                      */
/****************************************************************************************/
void valve_closeVal2()
{
	gs_val2Status = VAL_OFF;
	Chip_GPIO_SetPinOutLow(LPC_GPIO_PORT,VAL2_PORT_NUM, VAL2_BIT_NUM);
	DEBUG_VALVE("Close Val 2\n");
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	valve_closeVal3							              				*/
/*                                                                                      */
/* Details:    Close Valve 3															*/
/*                                                                                      */
/* Arguments:	None																	*/
/*                                                                                      */
/* ReturnValue : None			                                                        */
/*                                                                                      */
/****************************************************************************************/
void valve_closeVal3()
{
	gs_val3Status = VAL_OFF;
	Chip_GPIO_SetPinOutLow(LPC_GPIO_PORT,VAL3_PORT_NUM, VAL3_BIT_NUM);
	DEBUG_VALVE("Close Val 3\n");
}
