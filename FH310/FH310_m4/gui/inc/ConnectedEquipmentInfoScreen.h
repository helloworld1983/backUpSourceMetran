#ifndef INC_CONNECTEDEQUIPMENTINFOSCREEN_H_
#define INC_CONNECTEDEQUIPMENTINFOSCREEN_H_

/********************************************************************/
/*                                                                  */
/* File Name    : ConnectedEquipmentInfoScreen.h                    */
/*                                                                  */
/* General       : Displaying data in connected 					*/
/*                 equipment information 							*/
/*                 	 						                        */
/*                                                                  */
/* Product Name  : FH310                                            */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2008/08/22      Viet Le  			new file        */
/* #0001       2017/09/19      Viet Le				modify			*/
/*     Edit int8_t-> uint8_t in 									*/
/*		connectedEquipmentInforScreen_DisplayBTAddress() function 	*/
/*		(bug 1860)													*/
/********************************************************************/

#include "WM.h"
#include "AnypalHandler.h"

#define SIZE_BUFFER_VERSION_ANYPAL 				22
#define SIZE_BUFFER_BT_ADDR						26
#define DIV_3									3
#define SIZE_BUFFER_INSERT_STR					100

extern WM_HWIN g_connectedEquipmentInforScreen;

//Init connected equipment information external screen
void connectedEquipmentInforScreen_Init(void);
//Show connected equipment Information external screen
void connectedEquipmentInforScreen_Show(void);
//Hide connected equipment Information external screen
void connectedEquipmentInforScreen_Hide(void);
//Update gui when anypal connect
void connectedEquipmentInforScreen_UpdateAnypal(void);
//Show item in connected infor
void connectedEquipmentInforScreen_ShowItemInConnectInfor(void);
//Show item in connected setting
void connectedEquipmentInforScreen_ShowItemInConnectSetting(void);
//Hide all item
void connectedEquipmentInforScreen_HideAllItem(void);
//Show all item
void connectedEquipmentInforScreen_ShowAllItem(void);
//Display bluetooth address
void connectedEquipmentInforScreen_DisplayBTAddress(uint8_t* btAddr);

#endif /* INC_CONNECTEDEQUIPMENTINFOSCREEN_H_ */
/* end of file */
