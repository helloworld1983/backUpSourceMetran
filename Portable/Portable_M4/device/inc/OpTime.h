#ifndef DEVICE_INC_OPTIME_H_
#define DEVICE_INC_OPTIME_H_
/********************************************************************/
/*                                                                  */
/* File Name    : OpTime.h         			                        */
/*                                                                  */
/* General       : This module contain function for manage Operation*/
/*                 Time						                        */
/*                                                                  */
/* Product Name  : Portable                                         */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*          April 18, 2017      Thao Ha(MV)  	new file        	*/
/*                                                                  */
/********************************************************************/
#include "board.h"

/****************************************************************************************/
/*                                                                                      */
/* Function name:   OperationTime_Init                                                  */
/*                                                                                      */
/* Details:        Initialize OperationTime function by reading the previous tick time */
/*                  from EEPROM                                                         */
/*                                                                                      */
/* Arguments:   None                                                                    */
/*                                                                                      */
/* ReturnValue : void                                                                   */
/*                                                                                      */
/*                                                                                      */
/****************************************************************************************/
void OperationTime_Init();

/****************************************************************************************/
/*                                                                                      */
/* Function name:   OperationTime_Handle                                                */
/*                                                                                      */
/* Details:     OperationTime handle function is call for update current Operation      */
/*              tick time every 1 hour                                                  */
/*                                                                                      */
/* Arguments:   None                                                                    */
/*                                                                                      */
/* ReturnValue : void                                                                   */
/*                                                                                      */
/*                                                                                      */
/****************************************************************************************/
void OperationTime_Handle();


/****************************************************************************************/
/*                                                                                      */
/* Function name:   OperationTime_Display                                               */
/*                                                                                      */
/* Details:     Display current Operation time (continuous and synchronize)             */
/*                                                                                      */
/* Arguments:   None                                                                    */
/*                                                                                      */
/* ReturnValue : void                                                                   */
/*                                                                                      */
/*                                                                                      */
/****************************************************************************************/
void OperationTime_Display();


/****************************************************************************************/
/*                                                                                      */
/* Function name:   OperationTime_Save                                                  */
/*                                                                                      */
/* Details:     Save total operation tick time to EEPROM                                */
/*                                                                                      */
/* Arguments:   None                                                                    */
/*                                                                                      */
/* ReturnValue : void                                                                   */
/*                                                                                      */
/*                                                                                      */
/****************************************************************************************/
void OperationTime_Save();


/****************************************************************************************/
/*                                                                                      */
/* Function name:   OperationTime_SetOperationTimeAfterMaintenanceContinuous            */
/*                                                                                      */
/* Details:     Set total continuous operation time after maintenance                   */
/*                                                                                      */
/* Arguments:   (I) uint32_t hour                                                       */
/*                                                                                      */
/* ReturnValue : void                                                                   */
/*                                                                                      */
/*                                                                                      */
/****************************************************************************************/
void OperationTime_SetOperationTimeAfterMaintenanceContinuous(uint32_t hour);


/****************************************************************************************/
/*                                                                                      */
/* Function name:   OperationTime_SetOperationTimeAfterMaintenanceSynchronize           */
/*                                                                                      */
/* Details:     Set total Synchronize operation time after maintenance                  */
/*                                                                                      */
/* Arguments:   (I) uint32_t hour                                                       */
/*                                                                                      */
/* ReturnValue : void                                                                   */
/*                                                                                      */
/*                                                                                      */
/****************************************************************************************/
void OperationTime_SetOperationTimeAfterMaintenanceSynchronize(uint32_t hour);


/****************************************************************************************/
/*                                                                                      */
/* Function name:   OperationTime_SetTotalOperationTimeContinuous                       */
/*                                                                                      */
/* Details:     Set total continuous operation time                                     */
/*                                                                                      */
/* Arguments:   (I) uint32_t hour                                                       */
/*                                                                                      */
/* ReturnValue : void                                                                   */
/*                                                                                      */
/*                                                                                      */
/****************************************************************************************/
void OperationTime_SetTotalOperationTimeContinuous(uint32_t hour);


/****************************************************************************************/
/*                                                                                      */
/* Function name:   OperationTime_SetTotalOperationTimeSynchronize                      */
/*                                                                                      */
/* Details:     Set total Synchronize operation time                                    */
/*                                                                                      */
/* Arguments:   (I) uint32_t hour                                                       */
/*                                                                                      */
/* ReturnValue : void                                                                   */
/*                                                                                      */
/*                                                                                      */
/****************************************************************************************/
void OperationTime_SetTotalOperationTimeSynchronize(uint32_t hour);


/****************************************************************************************/
/*                                                                                      */
/* Function name:   OperationTime_GetTotalOperationTimeContinuous                       */
/*                                                                                      */
/* Details:     get total continuous operation time                                     */
/*                                                                                      */
/* Arguments:   None                                                                    */
/*                                                                                      */
/* ReturnValue : uint32_t                                                               */
/*                                                                                      */
/*                                                                                      */
/****************************************************************************************/
uint32_t OperationTime_GetTotalOperationTimeContinuous(void);


/****************************************************************************************/
/*                                                                                      */
/* Function name:   OperationTime_GetTotalOperationTimeContinuous                       */
/*                                                                                      */
/* Details:     get total continuous operation time                                     */
/*                                                                                      */
/* Arguments:   None                                                                    */
/*                                                                                      */
/* ReturnValue : uint32_t                                                               */
/*                                                                                      */
/*                                                                                      */
/****************************************************************************************/
uint32_t OperationTime_GetTotalOperationTimeSynchronize(void);


/****************************************************************************************/
/*                                                                                      */
/* Function name:   OperationTime_GetOperationTimeAfterMaintenanceContinuous            */
/*                                                                                      */
/* Details:     get total continuous operation time after maintenance                   */
/*                                                                                      */
/* Arguments:   None                                                                    */
/*                                                                                      */
/* ReturnValue : uint32_t                                                               */
/*                                                                                      */
/*                                                                                      */
/****************************************************************************************/
uint32_t OperationTime_GetOperationTimeAfterMaintenanceContinuous(void);



/****************************************************************************************/
/*                                                                                      */
/* Function name:   OperationTime_GetOperationTimeAfterMaintenanceSynchronize           */
/*                                                                                      */
/* Details:     get total synchronize operation time after maintenance                  */
/*                                                                                      */
/* Arguments:   None                                                                    */
/*                                                                                      */
/* ReturnValue : uint32_t                                                               */
/*                                                                                      */
/*                                                                                      */
/****************************************************************************************/
uint32_t OperationTime_GetOperationTimeAfterMaintenanceSynchronize(void);

#endif /* DEVICE_INC_OPTIME_H_ */
