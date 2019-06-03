
#ifndef DEVICE_INC_MODEHANDLE_H_
#define DEVICE_INC_MODEHANDLE_H_
/********************************************************************/
/*                                                                  */
/* File Name    : ModeHandle.h	                                    */
/*                                                                  */
/* General       : This module contain function for manage Mode		*/
/*                 	 						                        */
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
#include <DualCoreCommon.h>


/****************************************************************************************/
/*                                                                                      */
/* Function name:   mode_ChangeMode                                                     */
/*                                                                                      */
/* Details:        Change current mode including play audio, change UI and record log   */
/*                                                                                      */
/* Arguments:   None                                                                    */
/*                                                                                      */
/* ReturnValue : void                                                                   */
/*                                                                                      */
/*                                                                                      */
/****************************************************************************************/
void mode_ChangeMode();


/****************************************************************************************/
/*                                                                                      */
/* Function name:   mode_IncreaseFlowRate                                               */
/*                                                                                      */
/* Details:        Increase flow rate including play audio, change UI and record log     */
/*                                                                                      */
/* Arguments:   None                                                                    */
/*                                                                                      */
/* ReturnValue : void                                                                   */
/*                                                                                      */
/*                                                                                      */
/****************************************************************************************/
void mode_IncreaseFlowRate();


/****************************************************************************************/
/*                                                                                      */
/* Function name:   mode_DecreaseFlowRate                                               */
/*                                                                                      */
/* Details:        Decrease flow rate including play audio, change UI and record log     */
/*                                                                                      */
/* Arguments:   None                                                                    */
/*                                                                                      */
/* ReturnValue : void                                                                   */
/*                                                                                      */
/*                                                                                      */
/****************************************************************************************/
void mode_DecreaseFlowRate();


/****************************************************************************************/
/*                                                                                      */
/* Function name:   mode_StartSequence                                                  */
/*                                                                                      */
/* Details:        Mode function handle when portable start up                          */
/*                                                                                      */
/* Arguments:   (I) void			                                                    */
/*                                                                                      */
/* ReturnValue : void                                                                   */
/*                                                                                      */
/*                                                                                      */
/****************************************************************************************/
void mode_StartSequence(void);

#endif /* DEVICE_INC_MODEHANDLE_H_ */
