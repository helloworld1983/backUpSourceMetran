/******************************************************************************/
//
//  File Name		: I2CInterface.h
//
//	General       	: Contains functions for interface with I2C module
//
//  Product Name	: FH310_m4
//
//
//  Revision History:
//          Rev:      Date:       		Editor:
//          01         Jul 20, 2017	        Quyen Ngo(MV)
//          Description:  InitialRevision
//
/******************************************************************************/

#ifndef INC_I2CINTERFACE_H_
#define INC_I2CINTERFACE_H_

#include <lpc_types.h>
#include "board.h"

Status I2CInterfaceTransfer(LPC_I2C_T *pI2C, I2CM_XFER_T *xfer);

#endif /* INC_I2CINTERFACE_H_ */
