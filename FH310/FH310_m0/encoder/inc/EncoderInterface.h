#ifndef INC_ENCODERINTERFACE_H_
#define INC_ENCODERINTERFACE_H_

#include <lpc_types.h>

/********************************************************************/
/*                                                                  */
/* File Name    : EncoderInterface.h                                */
/*                                                                  */
/* General       :This file contains function for interface with    */
/*                encoder module			                        */
/*                 	 						                        */
/*                                                                  */
/* Product Name  : FH310                                            */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2017/08/02     Quyen Ngo(MV)  	    new file        */
/*                                                                  */
/* #0002	   2017/11/17	  Quyen Ngo(MV)			modify			*/
/*		Remove function not use for fix Coverity issue				*/
/********************************************************************/

void encoderInterface_DisableEncoder(void);

void encoderInterface_SetEnable(bool isEnable);

bool encoderInterface_GetIsEnable(void);

#endif /* INC_ENCODERINTERFACE_H_ */
