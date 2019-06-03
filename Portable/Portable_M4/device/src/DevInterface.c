/*
 * DevInterface.c
 *
 *  Created on: Oct 5, 2017
 *      Author: WINDOWS
 */

#include "DevInterface.h"
#include "TaskCommon.h"


/****************************************************************************************/
/*                                                                                      */
/* Function name: 	devInterface_GetChargeSource				              				*/
/*                                                                                      */
/* Details:     Check which source of voltage is using to supply power for the system   */
/*                                                                                      */
/* Arguments:	None																	*/
/*                                                                                      */
/* ReturnValue : E_ChargeSource			                                            */
/*                                                                                      */
/****************************************************************************************/

E_ChargeSource devInterface_GetChargeSource(void)
{
	vTaskSuspendAll();
	E_ChargeSource chargeState;
	chargeState = battery_GetChargeSource();
	xTaskResumeAll();
	return chargeState;
}
