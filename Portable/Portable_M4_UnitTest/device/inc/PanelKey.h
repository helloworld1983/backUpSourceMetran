#ifndef DEVICE_INC_PANELKEY_H_
#define DEVICE_INC_PANELKEY_H_
/********************************************************************/
/*                                                                  */
/* File Name    : Panelkey.h	                                    */
/*                                                                  */
/* General       : This module contain function for Initialize and	*/
/* 					detect key push						            */
/*                 	 						                        */
/*                                                                  */
/* Product Name  : Portable                                         */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*          April 18, 2017      Thao Ha(MV)  		new file        */
/*                                                                  */
/********************************************************************/

#include "board.h"

typedef enum
{
  TOGGLE,
  TOGGLESHORT,
  TOGGLE2S,
  IDLE,
} KEY_EVENT;
enum
{
	ePowerButtonStatus,
	eIncreaseButtonStatus,
	eDecreaseButtonStatus,
	eMuteButtonStatus,
	eModeChangeButtonStatus
};

/****************************************************************************************/
/*                                                                                      */
/* Function name:   keyPanel_IncreaseButtonToggle                                            */
/*                                                                                      */
/* Details:        Handle when increase button is toggled                               */
/*                                                                                      */
/* Arguments:   (I) None                                                                */
/*                                                                                      */
/* ReturnValue : void                                                                   */
/*                                                                                      */
/****************************************************************************************/
void keyPanel_IncreaseButtonToggle(void);

/****************************************************************************************/
/*                                                                                      */
/* Function name:   keyPanel_IncreaseButtonToggleShort                                       */
/*                                                                                      */
/* Details:        Handle when increase button is toggled short                         */
/*                                                                                      */
/* Arguments:   (I) None                                                                */
/*                                                                                      */
/* ReturnValue : void                                                                   */
/*                                                                                      */
/****************************************************************************************/
void keyPanel_IncreaseButtonToggleShort(void);

/****************************************************************************************/
/*                                                                                      */
/* Function name:   keyPanel_IncreaseButtonToggle2S                                          */
/*                                                                                      */
/* Details:        Handle when increase button is toggled for 2 second                  */
/*                                                                                      */
/* Arguments:   (I) None                                                                */
/*                                                                                      */
/* ReturnValue : void                                                                   */
/*                                                                                      */
/****************************************************************************************/
void keyPanel_IncreaseButtonToggle2S(void);


/****************************************************************************************/
/*                                                                                      */
/* Function name:   keyPanel_DecreaseButtonToggle                                            */
/*                                                                                      */
/* Details:        Handle when decrease button is toggled                               */
/*                                                                                      */
/* Arguments:   (I) None                                                                */
/*                                                                                      */
/* ReturnValue : void                                                                   */
/*                                                                                      */
/****************************************************************************************/
void keyPanel_DecreaseButtonToggle(void);

/****************************************************************************************/
/*                                                                                      */
/* Function name:   keyPanel_DecreaseButtonToggleShort                                       */
/*                                                                                      */
/* Details:        Handle when decrease button is toggled short                         */
/*                                                                                      */
/* Arguments:   (I) None                                                                */
/*                                                                                      */
/* ReturnValue : void                                                                   */
/*                                                                                      */
/****************************************************************************************/
void keyPanel_DecreaseButtonToggleShort(void);

/****************************************************************************************/
/*                                                                                      */
/* Function name:   keyPanel_DecreaseButtonToggle2S                                          */
/*                                                                                      */
/* Details:        Handle when decrease button is toggled for 2 second                  */
/*                                                                                      */
/* Arguments:   (I) None                                                                */
/*                                                                                      */
/* ReturnValue : void                                                                   */
/*                                                                                      */
/****************************************************************************************/
void keyPanel_DecreaseButtonToggle2S(void);


/****************************************************************************************/
/*                                                                                      */
/* Function name:   keyPanel_MuteButtonToggle                                                */
/*                                                                                      */
/* Details:        Handle when mute button is toggled                                   */
/*                                                                                      */
/* Arguments:   (I) None                                                                */
/*                                                                                      */
/* ReturnValue : void                                                                   */
/*                                                                                      */
/****************************************************************************************/
void keyPanel_MuteButtonToggle(void);

/****************************************************************************************/
/*                                                                                      */
/* Function name:   keyPanel_MuteButtonToggleShort                                           */
/*                                                                                      */
/* Details:        Handle when mute button is toggled short                             */
/*                                                                                      */
/* Arguments:   (I) None                                                                */
/*                                                                                      */
/* ReturnValue : void                                                                   */
/*                                                                                      */
/****************************************************************************************/
void keyPanel_MuteButtonToggleShort(void);

/****************************************************************************************/
/*                                                                                      */
/* Function name:   keyPanel_MuteButtonToggle2S                                              */
/*                                                                                      */
/* Details:        Handle when mute button is toggled for 2 second                      */
/*                                                                                      */
/* Arguments:   (I) None                                                                */
/*                                                                                      */
/* ReturnValue : void                                                                   */
/*                                                                                      */
/****************************************************************************************/
void keyPanel_MuteButtonToggle2S(void);

/****************************************************************************************/
/*                                                                                      */
/* Function name:   keyPanel_ModeChangeButtonToggle                                          */
/*                                                                                      */
/* Details:        Handle when mode change button is toggled                            */
/*                                                                                      */
/* Arguments:   (I) None                                                                */
/*                                                                                      */
/* ReturnValue : void                                                                   */
/*                                                                                      */
/****************************************************************************************/
void keyPanel_ModeChangeButtonToggle(void);

/****************************************************************************************/
/*                                                                                      */
/* Function name:   keyPanel_ModeChangeButtonToggleShort                                     */
/*                                                                                      */
/* Details:        Handle when mode change button is toggled short                      */
/*                                                                                      */
/* Arguments:   (I) None                                                                */
/*                                                                                      */
/* ReturnValue : void                                                                   */
/*                                                                                      */
/****************************************************************************************/
void keyPanel_ModeChangeButtonToggleShort(void);

/****************************************************************************************/
/*                                                                                      */
/* Function name:   keyPanel_ModeChangeButtonToggle2S                                        */
/*                                                                                      */
/* Details:        Handle when mode change button is toggled for 2 second               */
/*                                                                                      */
/* Arguments:   (I) None                                                                */
/*                                                                                      */
/* ReturnValue : void                                                                   */
/*                                                                                      */
/****************************************************************************************/
void keyPanel_ModeChangeButtonToggle2S(void);


/****************************************************************************************/
/*                                                                                      */
/* Function name:   keyPanel_PowerButtonToggle                                               */
/*                                                                                      */
/* Details:        Handle when power button is toggled                                  */
/*                                                                                      */
/* Arguments:   (I) None                                                                */
/*                                                                                      */
/* ReturnValue : void                                                                   */
/*                                                                                      */
/****************************************************************************************/
void keyPanel_PowerButtonToggle(void);


/****************************************************************************************/
/*                                                                                      */
/* Function name:   keyPanel_PowerButtonToggleShort                                          */
/*                                                                                      */
/* Details:        Handle when power button is toggled short                            */
/*                                                                                      */
/* Arguments:   (I) None                                                                */
/*                                                                                      */
/* ReturnValue : void                                                                   */
/*                                                                                      */
/****************************************************************************************/
void keyPanel_PowerButtonToggleShort(void);


/****************************************************************************************/
/*                                                                                      */
/* Function name:   keyPanel_PowerButtonToggle2S                                             */
/*                                                                                      */
/* Details:        Handle when power button is toggled for 2 second                     */
/*                                                                                      */
/* Arguments:   (I) None                                                                */
/*                                                                                      */
/* ReturnValue : void                                                                   */
/*                                                                                      */
/****************************************************************************************/
void keyPanel_PowerButtonToggle2S(void);

/****************************************************************************************/
/*                                                                                      */
/* Function name:   keyPanel_Handle                                                          */
/*                                                                                      */
/* Details:     Key handle function is call for update Key status                       */
/*                                                                                      */
/* Arguments:   (I) None                                                                */
/*                                                                                      */
/* ReturnValue : None                                                                   */
/*                                                                                      */
/****************************************************************************************/
void keyPanel_Handle();

/****************************************************************************************/
/*                                                                                      */
/* Function name:   keyPanel_GetButtonStatus                                                 */
/*                                                                                      */
/* Details:     Get current status of each Key on the panel                             */
/*                                                                                      */
/* Arguments:   (I) KEY_EVENT* status                                                   */
/*              (O) KEY_EVENT* status                                                   */
/* ReturnValue : None                                                                   */
/*                                                                                      */
/****************************************************************************************/
void keyPanel_GetButtonStatus(KEY_EVENT* status);

/****************************************************************************************/
/*                                                                                      */
/* Function name:   keyPanel_Init                                                     */
/*                                                                                      */
/* Details:         Initialize by create Timer for Key module                           */
/*                                                                                      */
/* Arguments:   (I) None                                                                */
/*                                                                                      */
/* ReturnValue : void                                                                   */
/*                                                                                      */
/****************************************************************************************/
void keyPanel_Init();

#endif /* DEVICE_INC_PANELKEY_H_ */
