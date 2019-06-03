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
#include "I2CInterface.h"
#include "TaskCommon.h"
#include "GuiInterface.h"


/******************************************************************************/
//COMMON.OPERATION
//    Function Name: I2CInterfaceTransfer
//
//    Detail: Excusion control  call for Chip I2C transfer
//
//    Arguments: (I)
//				 (O)
//
//    Return Values:  None
//
//
//    Using Global Data : (I)
//						  (O)
//
//
//    Note: None
//
/******************************************************************************/
Status I2CInterfaceTransfer(LPC_I2C_T *pI2C, I2CM_XFER_T *xfer)
{
	Status result = SUCCESS;
	if(xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED)	//scheduler is not started yet
	{
		if (Chip_I2CM_XferBlocking(pI2C, xfer) == 0)
		{
			result = ERROR;
		}
	}
	else
	{
		vTaskSuspendAll();
		if (Chip_I2CM_XferBlocking(pI2C, xfer) == 0)
		{
			result = ERROR;
			DEBUGOUT("Chip_I2CM_XferBlocking result: %d\n",result);
		}
		xTaskResumeAll();
	}
	return result;
}


