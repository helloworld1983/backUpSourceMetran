#ifndef INC_OXYCYLINDER_H_
	#define INC_OXYCYLINDER_H_

/********************************************************************/
/*                                                                  */
/* File Name     : OxyCylinder.h	                               	*/
/*                                                                  */
/* General       : handle mode oxygen cylinder module	            */
/*                 	 						                        */
/*                                                                  */
/* Product Name  : FH310                                            */
/* 						                                            */
/* Company		 : Metran                                           */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2016/10/16      Quyen Ngo  	new file       		    */
/*																	*/
/* #0001	   2017/09/08	   Quyen Ngo	modify					*/
/*		-remmove set status function not use						*/
/********************************************************************/
#include "DualCoreCommon.h"

void oxyCylinder_Handle(float extFlowPress);

E_O2CylinStatus oxyCylinder_GetStatus(void);

#endif /* end of file */
