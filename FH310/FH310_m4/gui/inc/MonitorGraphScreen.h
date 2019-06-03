#ifndef INC_MONITORGRAPHSCREEN_H_
#define INC_MONITORGRAPHSCREEN_H_

/********************************************************************/
/*                                                                  */
/* File Name    : MonitorGraphScreen.h                              */
/*                                                                  */
/* General      : Display graph monitor				                */
/* 								                                    */
/*                 	 						                        */
/*                                                                  */
/* Product Name  : FH310                                            */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2016/08/14       Viet Le		  	    new file        */
/* #0001       														*/
/*                                                                  */
/********************************************************************/

#define Y_O2_GRAPH(y) 			(4*(y-70))
#define END_Y_GRAPH 			100
#define SIZE_GRAPH_BUFFER 		4
#define GRAPH_SCALE 			20
#define TIMER_UPDATE_GRAPH		250
#define DEFAULT_O2				90

extern WM_HWIN g_monitorGraphScreen;

//Init monitor graph screen
void monitorGraphScreen_Init(void);
//Show monitor graph screen
void monitorGraphScreen_Show(void);
//Hide monitor graph screen
void monitorGraphScreen_Hide(void);
//Update psa in graph screen
void monitorGraphScreen_UpdatePSA(IPC_MSG_DATA_t data);
//Change flow rate
void monitorGraphScreen_ChangeFlowrate(uint8_t FRPSAValue);

#endif /* INC_MONITORGRAPHSCREEN_H_ */
/* end of file */

