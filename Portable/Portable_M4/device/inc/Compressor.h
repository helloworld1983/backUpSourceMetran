#ifndef INC_COMPRESSOR_H_
#define INC_COMPRESSOR_H_
/********************************************************************/
/*                                                                  */
/* File Name    : Compressor.c	                                 	*/
/*                                                                  */
/* General      : This module contains function to control 			*/
/*                compressor				                        */
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
#include <DualCoreCommon.h>
#include <TaskCommon.h>

#define COMP_MAX_RPM							3000
#define DC_OVER_VOLT_ERR_BIT 					1<<0
#define DC_LOW_VOLT_ERR_BIT						1<<1
#define SOFT_OVER_CURRENT_ERR_BIT 				1<<2
#define HARD_OVER_CURRENT_ERR_BIT 				1<<3
#define BOARD_TEMP_ERR_BIT						1<<4
#define MOTOR_TEMP_ERR_BIT						1<<5
#define RPM_ERR_BIT								1<<6
#define MOTOR_STEP_OUT_ERR_BIT					1<<7
#define COMP_CMD_TIME_OUT_BIT					1<<8
#define COMP_RESET_NOTI_BIT						1<<9
#define COMP_COMMUNICATION_NO_RESPONSE_ERR_BIT	1<<15 // special bit, only set by control SW, not Compressor driver

#define COMPRESSOR_CHECKSUM_COMMUNICATION_ERR_BIT 			1<<0
#define COMPRESSOR_COMMAND_COMMUNICATION_ERR_BIT			1<<1
#define COMPRESSOR_PARAMETER_COMMUNICATION_ERR_BIT 			1<<2
typedef enum
{
	COMP_SEND_RPM,
	COMP_SEND_VERSION,
	COMP_SEND_TEMPERATURE,
	COMP_SEND_ACQ,
	COMP_SEND_CLR_ERR
}E_CompressorRequestCmd;

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	compressor_InitCommunication			             				*/
/*                                                                                      */
/* Details: Init UART configuration for compressor controller							*/
/*                                                                                      */
/* Arguments:	(I) None																*/
/*              (O) None						                                        */
/* ReturnValue : void				                                                    */
/*                                                                                      */
/****************************************************************************************/
void compressor_InitCommunication(void);

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	compressor_ReadFromDevice					        				*/
/*                                                                                      */
/* Details: Read and then handle Message returned from compressor						*/
/*                                                                                      */
/* Arguments:	(I) None																*/
/*              (O) None						                                        */
/* ReturnValue : void				                                                    */
/*                                                                                      */
/****************************************************************************************/
void compressor_ReadFromDevice();

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	compressor_SendStatusAcquisitionCmd			        				*/
/*                                                                                      */
/* Details: Send command to get current status of compressor							*/
/*                                                                                      */
/* Arguments:	(I) None																*/
/*              (O) None						                                        */
/* ReturnValue : void				                                                    */
/*                                                                                      */
/****************************************************************************************/
void compressor_SendStatusAcquisitionCmd();


/****************************************************************************************/
/*                                                                                      */
/* Function name: 	compressor_Handle			     			        				*/
/*                                                                                      */
/* Details: Handle communication with compressor controller								*/
/*                                                                                      */
/* Arguments:	(I) uint16_t rpm														*/
/*              (O) None						                                        */
/* ReturnValue : void				                                                    */
/*                                                                                      */
/****************************************************************************************/
void compressor_Handle(uint16_t rpm);

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	compressor_Reset							        				*/
/*                                                                                      */
/* Details: Reset compressor															*/
/*                                                                                      */
/* Arguments:	(I) None																*/
/*              (O) None						                                        */
/* ReturnValue : None			                                                   	*/
/*                                                                                      */
/****************************************************************************************/
void compressor_Reset();

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	compressor_Restart							        				*/
/*                                                                                      */
/* Details: Restart compressor															*/
/*                                                                                      */
/* Arguments:	(I) None																*/
/*              (O) None						                                        */
/* ReturnValue : None				                                                   	*/
/*                                                                                      */
/****************************************************************************************/
void compressor_Restart();

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	compressor_getErrStatus						        				*/
/*                                                                                      */
/* Details: Get current compressor error status											*/
/*                                                                                      */
/* Arguments:	(I) None																*/
/*              (O) None						                                        */
/* ReturnValue : uint16_t				                                                */
/*                                                                                      */
/****************************************************************************************/
uint16_t compressor_getErrStatus();// truong edit

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	compressor_getComunicateErrStatus			        				*/
/*                                                                                      */
/* Details: Get current compressor communication error status							*/
/*                                                                                      */
/* Arguments:	(I) None																*/
/*              (O) None						                                        */
/* ReturnValue : uint8_t			                                                    */
/*                                                                                      */
/****************************************************************************************/
uint8_t compressor_getCommunicateErrStatus();	// truong add

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	compressor_getResponseErrStatus				        				*/
/*                                                                                      */
/* Details: Get current compressor error response status								*/
/*                                                                                      */
/* Arguments:	(I) None																*/
/*              (O) None						                                        */
/* ReturnValue : E_CompressorStatus	                                                    */
/*                                                                                      */
/****************************************************************************************/
E_CompressorStatus compressor_getResponseErrStatus();	// truong add

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	compressor_GetCurrentSeq					        				*/
/*                                                                                      */
/* Details: Get current sequence of compressor											*/
/*                                                                                      */
/* Arguments:	(I) None																*/
/*              (O) None						                                        */
/* ReturnValue : E_CompressorSequence                                                   */
/*                                                                                      */
/****************************************************************************************/
E_CompressorSequence compressor_GetCurrentSeq();

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	compressor_getRpm							        				*/
/*                                                                                      */
/* Details: Get current RPM of compressor												*/
/*                                                                                      */
/* Arguments:	(I) None																*/
/*              (O) None						                                        */
/* ReturnValue : uint16_t			                                                   	*/
/*                                                                                      */
/****************************************************************************************/
uint16_t compressor_getRpm();

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	compressor_getBoardTemperature				        				*/
/*                                                                                      */
/* Details: Get compressor board temperature											*/
/*                                                                                      */
/* Arguments:	(I) None																*/
/*              (O) None						                                        */
/* ReturnValue : uint16_t			                                                   	*/
/*                                                                                      */
/****************************************************************************************/
uint16_t compressor_getBoardTemperature();

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	compressor_getMotorTemperature				        				*/
/*                                                                                      */
/* Details: Get compressor temperature													*/
/*                                                                                      */
/* Arguments:	(I) None																*/
/*              (O) None						                                        */
/* ReturnValue : uint16_t				                                                   	*/
/*                                                                                      */
/****************************************************************************************/
uint16_t compressor_getMotorTemperature();

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	compressor_ReqestSetRpm						        				*/
/*                                                                                      */
/* Details: request set rpm for compressor, compressor_Handle() function will process   */
/* 			this request																*/
/*                                                                                      */
/* Arguments:	(I) uint16_t rpm														*/
/*              (O) None						                                        */
/* ReturnValue : None				                                                   	*/
/*                                                                                      */
/****************************************************************************************/
void compressor_ReqestSetRpm(uint16_t rpm);

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	compressor_Stop								        				*/
/*                                                                                      */
/* Details: Stop compressor																*/
/*                                                                                      */
/* Arguments:	(I) None																*/
/*              (O) None						                                        */
/* ReturnValue : None				                                                   	*/
/*                                                                                      */
/****************************************************************************************/
void compressor_Stop();

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	compressor_GetVersion						        				*/
/*                                                                                      */
/* Details: Get version of compressor controller driver									*/
/*                                                                                      */
/* Arguments:	(I) char* buffer														*/
/*              (O) char* buffer				                                        */
/* ReturnValue : unsigned char*		                                                   	*/
/*                                                                                      */
/****************************************************************************************/
unsigned char* compressor_GetVersion(char*);

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	compressor_isGetVersionDone			              				    */
/*                                                                                      */
/* Details:        Check whether read version process is finished						*/
/*                                                                                      */
/* Arguments:	(I)	None																*/
/*              (O) None                                                                */
/* ReturnValue : bool			                                                        */
/*                                                                                      */
/*                                                                                      */
/****************************************************************************************/
bool compressor_isGetVersionDone();
#endif /* INC_COMPRESSOR_H_ */
/* end of file */
