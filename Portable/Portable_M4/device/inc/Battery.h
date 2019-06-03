#ifndef BATTERY_INC_BATTERY_H_
#define BATTERY_INC_BATTERY_H_

/********************************************************************/
/*                                                                  */
/* File Name    : battery.h		                                 	*/
/*                                                                  */
/* General      : battery command to get current status of smart	*/
/* 				battery												*/
/*                 	 						                        */
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

#include "board.h"



typedef struct {
	uint16_t remainingCapacity;
	uint16_t remainingPercentage;
	uint16_t status;
	bool 	 isBatteryConnected;
	bool	 isBatteryCharging;
	bool 	 isInvalidBattery;
	bool isBatteryChargingError;
}E_BatteryStatus;

typedef enum
{
	eCarChargeType,
	eACChargeType,
	eNoBatteryChargeType

} E_ChargeType;


typedef enum
{
	eCarSource,
	eACSource,
	eBatterySource
} E_ChargeSource;


typedef enum
{
	eSendAuthenCommand,
	eReadAuthenData
} E_AuthenState;

typedef enum
{
	eOK,
	eReadAuthenDataError,
	eCheckCRCError,
	eAuthenticationCodeError,
} E_ResultOfAuthen;



/****************************************************************************************/
/*                                                                                      */
/* Function name:   battery_Handle                                                      */
/*                                                                                      */
/* Details:        battery handle function is call for update battery status every 3s   */
/* Arguments:   (I)E_ChargingState currentChargingState                                 */
/*                                                                                      */
/* ReturnValue : None                                                                   */
/*                                                                                      */
/****************************************************************************************/
void battery_Handle();

/****************************************************************************************/
/*                                                                                      */
/* Function name:   battery_Init                                                        */
/*                                                                                      */
/* Details:     Initialize Battery function by create timer and initialize interrupt    */
/*              pin                                                                     */
/*                                                                                      */
/* Arguments:   None                                                                    */
/*                                                                                      */
/* ReturnValue : None                                                                   */
/*                                                                                      */
/****************************************************************************************/
void battery_Init();

/****************************************************************************************/
/*                                                                                      */
/* Function name:   battery_GetChargeState                                              */
/*                                                                                      */
/* Details:    Refer to specification                                                   */
/*                                                                                      */
/* Arguments:   E_ChargingState                                                         */
/*                                                                                      */
/* ReturnValue : None                                                             		*/
/*                                                                                      */
/****************************************************************************************/
E_ChargeSource battery_GetChargeSource(void);

/****************************************************************************************/
/*                                                                                      */
/* Function name:   battery_GetRemainingPercentage                                      */
/*                                                                                      */
/* Details:    Get remaining capacity percentage                                        */
/*                                                                                      */
/* Arguments:   None                                                                    */
/*                                                                                      */
/* ReturnValue : uint16_t                                                               */
/*                                                                                      */
/****************************************************************************************/
uint16_t battery_GetRemainingPercentage();

/****************************************************************************************/
/*                                                                                      */
/* Function name:   battery_GetRemainingCapacity                                        */
/*                                                                                      */
/* Details:    Get remaining capacity (mAh)                                             */
/*                                                                                      */
/* Arguments:   None                                                                    */
/*                                                                                      */
/* ReturnValue : uint16_t                                                               */
/*                                                                                      */
/****************************************************************************************/
uint16_t battery_GetRemainingCapacity();

/****************************************************************************************/
/*                                                                                      */
/* Function name:   battery_getStatus                                                   */
/*                                                                                      */
/* Details:    get status of battery                                                    */
/*                                                                                      */
/* Arguments:   None                                                                    */
/*                                                                                      */
/* ReturnValue : uint16_t                                                               */
/*                                                                                      */
/****************************************************************************************/
uint16_t battery_GetStatus();

/****************************************************************************************/
/*                                                                                      */
/* Function name:   battery_isBatteryCharging                                           */
/*                                                                                      */
/* Details:    Check whether battery is charging                                        */
/*                                                                                      */
/* Arguments:   None                                                                    */
/*                                                                                      */
/* ReturnValue : bool                                                                   */
/*                                                                                      */
/****************************************************************************************/
bool battery_isBatteryCharging();

/****************************************************************************************/
/*                                                                                      */
/* Function name:   battery_isBatteryConnected                                          */
/*                                                                                      */
/* Details:    Check whether battery is connected                                       */
/*                                                                                      */
/* Arguments:   None                                                                    */
/*                                                                                      */
/* ReturnValue : bool                                                                   */
/*                                                                                      */
/****************************************************************************************/
bool battery_isBatteryConnected();

/****************************************************************************************/
/*                                                                                      */
/* Function name:   battery_isInvalidBatteryInserted                                    */
/*                                                                                      */
/* Details: check whether invalid battery type is inserted                              */
/* Arguments:   (I) None                                                                */
/*              (O) None                                                                */
/* ReturnValue : bool                                                                   */
/*                                                                                      */
/****************************************************************************************/
bool battery_isInvalidBatteryInserted();

/****************************************************************************************/
/*                                                                                      */
/* Function name:   batter_isBatteryChargingError                                       */
/*                                                                                      */
/* Details:    check whether battery charging is error                                  */
/*                                                                                      */
/* Arguments:   None                                                                    */
/*                                                                                      */
/* ReturnValue : None                                                                   */
/*                                                                                      */
/****************************************************************************************/
bool battery_isBatteryChargingError();

#endif /* BATTERY_INC_BATTERY_H_ */
