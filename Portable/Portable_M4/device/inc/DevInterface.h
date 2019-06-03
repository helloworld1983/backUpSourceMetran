/*
 * DevInterface.h
 *
 *  Created on: Oct 5, 2017
 *      Author: WINDOWS
 */

#ifndef DEVICE_INC_DEVINTERFACE_H_
#define DEVICE_INC_DEVINTERFACE_H_

#include "Battery.h"

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
E_ChargeSource devInterface_GetChargeSource(void);

#endif /* DEVICE_INC_DEVINTERFACE_H_ */
