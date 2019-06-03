#ifndef MAINSCREEN_H_
#define MAINSCREEN_H_

/********************************************************************/
/*                                                                  */
/* File Name    : MainScreen.h                       			    */
/*                                                                  */
/* General       : Manager handle event from all screen				*/
/*                 	 						                        */
/*                                                                  */
/* Product Name  : FH310                                            */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2016/08/22      Viet Le			  	new file        */
/* #0001       														*/
/*                                                                  */
/********************************************************************/

#include "WM.h"

#define TIMER_100_MS 											100
#define TIMER_500_MS 											(5*TIMER_100_MS)
#define TIMER_50_MS 											50
#define TIMER_1_SECONDS 										1000//1000ms
#define TIMER_1_MINUTES 										60000 //60000ms = 60 seconds
#define TIMER_3_SECONDS 										3000 //3000ms = 3 seconds
#define MAX_VALUE_EXT_FLOW_PATH_PRESSURE 						165*100

//define signal show home screen
#define WM_SHOW_HOMESCREEN 										(WM_USER + 11)
//define signal show alarm screen
#define WM_SHOW_ALARMSCREEN 									(WM_USER + 18)
//define signal show service tab screen
#define WM_SHOW_SERVICE_TAB_SCREEN 								(WM_USER + 19)
#define WM_SHOW_PASSWORD_DIALOG 								(WM_USER + 40)

#define PATIENT_BACKGROUND_COLOR 								0x563A00
#define SERVICE_BACKGROUND_COLOR 								0xFFDCD2
#define BUTTON_BACKGROUND_COLOR 								0xFFC85A
#define DIALOG_BACKGROUND_COLOR 								0xFFEADF
#define BUTTON_DIALOG_BACKGROUND_COLOR 							0xFFE3D4
#define BUTTON_NUMBER_BACKGROUND_COLOR 							0xF1D9C6
#define BORDER_COLOR 											0xC8A046
#define BORDER_DIALOG_COLOR 									0xCD9C79
#define BACKGROUND_SILENCE_BTN_COLOR							0xCBC0A9
#define BACKGROUND_SILENCE_ICON_BTN_COLOR						0xBABABA
#define BACKGROUND_SILENCE_LOW_ICON_BTN_COLOR					0x6B6B6B

#define IMAGE_ANIM_ONE 											1
#define IMAGE_ANIM_TWO 											2
#define IMAGE_ANIM_THREE 										3
#define IMAGE_ANIM_FOUR 										4
#define IMAGE_ANIM_FIVE 										5
#define IMAGE_ANIM_SIX 											6
#define MAX_IMAGE_ANIMATION 		 							6

#define MAIN_WINDOW_X											0
#define MAIN_WINDOW_Y											0
#define MAIN_WINDOW_SIZE_X										800
#define MAIN_WINDOW_SIZE_Y										480
#define WAKEUP_SHOW_TIME										3000

//Define position touch envet
typedef enum{
	eTouchScrOne = 0,
	eTouchScrTwo,
	eTouchScrThree,
	eTouchScrFour
}E_Touch;

//State machine
typedef enum {
    eStandby,
    eStartSequence,
    eOperating,
    eSleep,
    eEndSequence,
    eAlarmType
}E_StateMachine;

//main window
extern WM_HWIN g_mainWindow;

//Int main window
void mainScreen_Init(void);
//Show wake up screen
void mainScreen_ShowWakeup(void);

#endif /* MAINSCREEN_H_ */
/* end of file */
