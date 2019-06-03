/******************************************************************************
 *
 *    File Name: i2c.c
 *    @brief Copyright (c) 2018, Metran.  All Rights Reserved.
 *
 *
 *    Revision History:
 *
 *       Rev:      Date:       	Engineer:         Project:
 *        01       11/29/2018    Quyen Ngo         NewVue
 *        Description: New file
 ******************************************************************************/
#include <board/i2c.h>
#include "fsl_i2c_freertos.h"
#include "fsl_debug_console.h"

#define I2C2_IRQN_PRIOR              (2)
#define I2C3_IRQN_PRIOR              (2)
#define I2C4_IRQN_PRIOR              (2)

#define I2C4_CLK_FREQ                                                                \
		CLOCK_GetPllFreq(kCLOCK_SystemPll1Ctrl) / (CLOCK_GetRootPreDivider(kCLOCK_RootI2c4)) / \
		(CLOCK_GetRootPostDivider(kCLOCK_RootI2c4)) / 5 /* SYSTEM PLL1 DIV5 */

#define I2C3_CLK_FREQ                                                                \
		CLOCK_GetPllFreq(kCLOCK_SystemPll1Ctrl) / (CLOCK_GetRootPreDivider(kCLOCK_RootI2c3)) / \
		(CLOCK_GetRootPostDivider(kCLOCK_RootI2c3)) / 5 /* SYSTEM PLL1 DIV5 */

#define I2C2_CLK_FREQ                                                                \
		CLOCK_GetPllFreq(kCLOCK_SystemPll1Ctrl) / (CLOCK_GetRootPreDivider(kCLOCK_RootI2c2)) / \
		(CLOCK_GetRootPostDivider(kCLOCK_RootI2c2)) / 5 /* SYSTEM PLL1 DIV5 */


i2c_master_handle_t *g_m_handle;
i2c_rtos_handle_t master_rtos_handle_I2C2;
i2c_rtos_handle_t master_rtos_handle_I2C3;
i2c_rtos_handle_t master_rtos_handle_I2C4;

void i2c_Init(I2C_Type* chanelBase, uint32_t baudRate)
{

	i2c_master_config_t masterConfig;
	uint32_t sourceClock;
	status_t status;

	if(chanelBase == I2C2)
	{
		CLOCK_SetRootMux(kCLOCK_RootI2c2, kCLOCK_I2cRootmuxSysPll1Div5); /* Set I2C source to SysPLL1 Div5 160MHZ */
		CLOCK_SetRootDivider(kCLOCK_RootI2c2, 1U, 4U);                   /* Set root clock to 160MHZ / 4 = 40MHZ */
		NVIC_SetPriority(I2C2_IRQn, I2C2_IRQN_PRIOR);
		/*
		 * masterConfig->baudRate_Bps = 100000U;
		 * masterConfig->enableHighDrive = false;
		 * masterConfig->enableStopHold = false;
		 * masterConfig->glitchFilterWidth = 0U;
		 * masterConfig->enableMaster = true;
		 */
		I2C_MasterGetDefaultConfig(&masterConfig);

		masterConfig.baudRate_Bps = baudRate;
		sourceClock = I2C2_CLK_FREQ;

		status_t status = I2C_RTOS_Init(&master_rtos_handle_I2C2, I2C2, &masterConfig, sourceClock);
		if (status != kStatus_Success)
		{
			PRINTF("I2C master: error during init, %d", status);
		}
	}
	else if(chanelBase == I2C3)
	{
		CLOCK_SetRootMux(kCLOCK_RootI2c3, kCLOCK_I2cRootmuxSysPll1Div5); /* Set I2C source to SysPLL1 Div5 160MHZ */
		CLOCK_SetRootDivider(kCLOCK_RootI2c3, 1U, 4U);                /* Set root clock to 160MHZ / 4 = 40MHZ */

		NVIC_SetPriority(I2C3_IRQn, I2C3_IRQN_PRIOR);
		/*
		 * masterConfig->baudRate_Bps = 100000U;
		 * masterConfig->enableHighDrive = false;
		 * masterConfig->enableStopHold = false;
		 * masterConfig->glitchFilterWidth = 0U;
		 * masterConfig->enableMaster = true;
		 */

		I2C_MasterGetDefaultConfig(&masterConfig);

		masterConfig.baudRate_Bps = baudRate;
		sourceClock = I2C3_CLK_FREQ;

		status_t status = I2C_RTOS_Init(&master_rtos_handle_I2C3, I2C3, &masterConfig, sourceClock);
		if (status != kStatus_Success)
		{
			PRINTF("I2C master: error during init, %d", status);
		}

	}
	else if(chanelBase == I2C4)
	{
		CLOCK_SetRootMux(kCLOCK_RootI2c4, kCLOCK_I2cRootmuxSysPll1Div5); /* Set I2C source to SysPLL1 Div5 160MHZ */
		CLOCK_SetRootDivider(kCLOCK_RootI2c4, 1U, 4U);                /* Set root clock to 160MHZ / 4 = 40MHZ */

		NVIC_SetPriority(I2C4_IRQn, I2C4_IRQN_PRIOR);
		/*
		 * masterConfig->baudRate_Bps = 100000U;
		 * masterConfig->enableHighDrive = false;
		 * masterConfig->enableStopHold = false;
		 * masterConfig->glitchFilterWidth = 0U;
		 * masterConfig->enableMaster = true;
		 */

		I2C_MasterGetDefaultConfig(&masterConfig);

		masterConfig.baudRate_Bps = baudRate;
		sourceClock = I2C4_CLK_FREQ;

		status_t status = I2C_RTOS_Init(&master_rtos_handle_I2C4, I2C4, &masterConfig, sourceClock);
		if (status != kStatus_Success)
		{
			PRINTF("I2C master: error during init, %d", status);
		}
	}
	return;
}

status_t i2c_Write(I2C_Type* chanelBase, uint32_t addr7Bit, uint8_t* buf, uint32_t len)
{
	i2c_master_transfer_t masterXfer;
	status_t status = kStatus_Fail;

	memset(&masterXfer, 0, sizeof(masterXfer));

	masterXfer.slaveAddress = addr7Bit;
	masterXfer.direction = kI2C_Write;
	masterXfer.subaddress = (uint32_t)NULL;
	masterXfer.subaddressSize = 0;
	masterXfer.data = buf;
	masterXfer.dataSize = len;
	masterXfer.flags = kI2C_TransferDefaultFlag;
	if(chanelBase==I2C2)
	{
		status = I2C_RTOS_Transfer(&master_rtos_handle_I2C2, &masterXfer);
	}
	else
		if(chanelBase==I2C3)
		{
			status = I2C_RTOS_Transfer(&master_rtos_handle_I2C3, &masterXfer);
		}
		else
			if(chanelBase==I2C4)
			{
				status = I2C_RTOS_Transfer(&master_rtos_handle_I2C4, &masterXfer);
			}
	if (status != kStatus_Success)
	{
		PRINTF("I2C master: error during write transaction, %d\n", status);
	}
	return status;
}

status_t i2c_Read(I2C_Type* chanelBase, uint32_t addr7Bit, uint8_t* buf, uint32_t len)
{
	i2c_master_transfer_t masterXfer;
	status_t status = kStatus_Fail;

	memset(&masterXfer, 0, sizeof(masterXfer));

	masterXfer.slaveAddress = addr7Bit;
	masterXfer.direction = kI2C_Read;
	masterXfer.subaddress = (uint32_t)NULL;
	masterXfer.subaddressSize = 0;
	masterXfer.data = buf;
	masterXfer.dataSize = len;
	masterXfer.flags = kI2C_TransferDefaultFlag;
	if(chanelBase==I2C2)
	{
		status = I2C_RTOS_Transfer(&master_rtos_handle_I2C2, &masterXfer);
	}
	else
		if(chanelBase==I2C3)
		{
			status = I2C_RTOS_Transfer(&master_rtos_handle_I2C3, &masterXfer);
		}
		else
			if(chanelBase==I2C4)
			{
				status = I2C_RTOS_Transfer(&master_rtos_handle_I2C4, &masterXfer);
			}
	if (status != kStatus_Success)
	{
		PRINTF("I2C master: error during write transaction, %d\n", status);
	}
	return status;
}
