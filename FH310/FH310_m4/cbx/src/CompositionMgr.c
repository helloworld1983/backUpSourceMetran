/********************************************************************/
/*                                                                  */
/* File Name    : CompositionMgr.c                        			*/
/*                                                                  */
/* General      : Manager of Composition							*/
/*                 	 						                        */
/*                                                                  */
/* Product Name  : FH310                                            */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2017/01/9       Linh Nguyen	  	    new file        */
/* #0001       2016/09/05      Linh Nguyen	  	    modify          */
/*  Improve composition manager design								*/
/* #0002       2016/09/15      Linh Nguyen	  	    modify          */
/*  Remove some static marco										*/
/* #0003       2016/09/28      Linh Nguyen	  	    modify          */
/*  Delete updating state when delete composition					*/
/*  Update device no1 and device number when there is update state	*/
/*  		   2017/10/07      Linh Nguyen(MV)  	    modify	    */
/*  #0004 change device quality update sequence for bug #1207		*/
/*  		   2017/10/07      Linh Nguyen(MV)  	    modify	    */
/*  #0005 change code for new spec #2060							*/
/*  		   2017/10/07      Linh Nguyen(MV)  	    modify	    */
/*  #0006 change code for new spec #2060							*/
/********************************************************************/
#include "CompositionMgr.h"
#include "board.h"
#include "setting.h"
#include "string.h"
#include "GuiInterface.h"

int8_t gs_deviceQuantity[COMPOSITION_LENG];

// Update status return
int8_t gs_updatingStatusReturn[COMPOSITION_LENG];

// Connnected device No1 ID
ExternalDeviceID gs_deviceNo1;

/** >>#005
 	Remove gs_updatingStatus
#005 >>**/

// allow change update status
static bool gs_isAllowChangeUpdateStatus;

//
/****************************************************************************************/
/*                                                                                      */
/* Function name: composition_Update			                                   		*/
/*                                                                                      */
/* Details:  Update composition when external device connect							*/
/*																						*/
/*                                                                                      */
/* Arguments : (O) NONE																	*/
/*                                                                                      */
/* Using Global Data : (I) gs_isAllowChangeUpdateStatus	                              	*/
/*                     (O) gs_updatingStatus                             			   	*/
/*                     (O) gs_updatingStatusReturn	                                   	*/
/*                     (O) gs_deviceQuantity		                                   	*/
/*                     (O) gs_deviceNo1				                                   	*/
/*                                                                                      */
/****************************************************************************************/
void composition_Update(void)
{
	if(gs_isAllowChangeUpdateStatus == true)
	{
		memcpy(gs_updatingStatusReturn, UPDATE_STATUS,COMPOSITION_LENG);
	}
	/** >>#005
 	Remove gs_updatingStatus
#005 >>**/
	gs_deviceNo1 = Anypale_ID;
	memcpy(gs_deviceQuantity, TWO_DEVICE,COMPOSITION_LENG);
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: composition_Delete			                                   		*/
/*                                                                                      */
/* Details:  Delete composition when delete external device								*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) 	NONE									  							*/
/*                                                                                      */
/* Using Global Data : (I) gs_updatingStatusReturn		                              	*/
/*                     (O) gs_deviceQuantity                               			   	*/
/*                     (O) gs_isAllowChangeUpdateStatus	                            	*/
/*                     (O) gs_deviceNo1				                                   	*/
/*                                                                                      */
/****************************************************************************************/
void composition_Delete(void)
{
	/** >>#005
 	add if branch
#005 >>**/
	if(gs_isAllowChangeUpdateStatus == true)
	{
		memcpy(gs_updatingStatusReturn, UPDATE_STATUS, COMPOSITION_LENG);
	}
	memcpy(gs_deviceQuantity, ONE_DEVICE,COMPOSITION_LENG);
	/** >>#006
	 	Remove gs_isAllowChangeUpdateStatus
	#006 >>**/
	gs_deviceNo1 = Null_ID;
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: Composition_Init			                                       		*/
/*                                                                                      */
/* Details:  init FH310 	Composition													*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE																	*/
/*                                                                                      */
/* ReturnValue : 				                                                        */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) gs_deviceQuantity			                              	*/
/*                     (O) gs_isAllowChangeUpdateStatus                   			   	*/
/*                     (O) gs_deviceNo1				                                   	*/
/* local valiables   : NONE			                                                  	*/
/*?Note?                                                                              	*/
/*                                                                                      */
/****************************************************************************************/
void composition_Init(void)
{
	gs_isAllowChangeUpdateStatus = false;
	memcpy(gs_updatingStatusReturn, NO_UPDATE_STATUS,COMPOSITION_LENG);
	gs_deviceNo1 = Null_ID;
	memcpy(gs_deviceQuantity, ONE_DEVICE,COMPOSITION_LENG);

	AnypalInfor ATP = guiInterface_GetAnypalInfo();

	//    Read device no1 ID
	if (!memcmp(ATP.equipmentName,ANYPAL_EQUIPMENT_NAME,COMPOSITION_LENG))
	{
		gs_deviceNo1 = Anypale_ID;
		memcpy(gs_deviceQuantity,TWO_DEVICE,strlen(TWO_DEVICE));
	}
	else
	{
		gs_deviceNo1 = Null_ID;
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: Get a Composition from an indicated id		                        */
/*                                                                                      */
/* Details:  init FH310 	Composition													*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) CompositionGetID ID						  							*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : 				                                                        */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) gs_deviceQuantity			                              	*/
/*                     (I) gs_updatingStatusReturn                        			   	*/
/*                     (I) gs_deviceNo1				                                   	*/
/* local valiables   : NONE			                                                  	*/
/*?Note?                                                                              	*/
/*                                                                                      */
/****************************************************************************************/
void* composition_Get(CompositionGetID ID)
{
	switch (ID)
	{
	case eDeviceQuantity:
		return gs_deviceQuantity;
	case eUpdatingStatus:
		return gs_updatingStatusReturn;
	case eDeviceNo1:
		return &gs_deviceNo1;
	default:
		break;
	}
	return NULL;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: composition_SetUpdatingStatus					                        */
/*                                                                                      */
/* Details:  Set updating status														*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE																	*/
/*                                                                                      */
/* Using Global Data : (O) gs_isAllowChangeUpdateStatus	                            	*/
/*                     (O) gs_updatingStatus                               			   	*/
/*                     (O) gs_updatingStatusReturn                                   	*/
/*                     (O) gs_deviceQuantity                                   			*/
/*                     (O) gs_deviceNo1		                                   			*/
/* local valiables   : NONE			                                                  	*/
/*?Note?                                                                              	*/
/*                                                                                      */
/****************************************************************************************/
void composition_SetUpdatingStatus(void)
{
	if(gs_isAllowChangeUpdateStatus == false)
	{
		/** >>#005
		Remove gs_updatingStatus
		#005 >>**/
		gs_isAllowChangeUpdateStatus = true;
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: composition_ClearUpdatingStatus				                        */
/*                                                                                      */
/* Details:  Clear Updating Status														*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE																	*/
/*                                                                                      */
/* Using Global Data : (O) gs_updatingStatusReturn                       			   	*/
/*                     (O) NONE						                                   	*/
/* local valiables   : NONE			                                                  	*/
/*?Note?                                                                              	*/
/*                                                                                      */
/****************************************************************************************/
void composition_ClearUpdatingStatus(void)
{
	memcpy(gs_updatingStatusReturn, NO_UPDATE_STATUS,COMPOSITION_LENG);
	return;
}
