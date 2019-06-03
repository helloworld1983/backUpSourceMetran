#ifndef INC_ILLUMINANCESENSOR_H_
#define INC_ILLUMINANCESENSOR_H_

/******************************************************************************/
//
//  File Name		: IlluminanceSensor.h
//  
//	General       	: This file contains function for communicate with iiluminance sensor
//
//  Product Name	: FH310_m0
//
//
//  Revision History:
//          Rev:      Date:       		Editor:         
//          01        Oct 15, 2016	        Quyen Ngo(MV)         
//          Description:  InitialRevision
//
/******************************************************************************/

typedef enum
{
	INIT,
	BRIGHT,
	MEDIUM,
	DARK
}E_BrightnessType;

E_BrightnessType illuminance_ReadValue(void);

float illuminance_ReadVoltValue(void);

#endif /* end of file */
