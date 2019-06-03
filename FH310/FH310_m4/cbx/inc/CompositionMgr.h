/********************************************************************/
/*                                                                  */
/* File Name    : CompositionMgr.h                        			*/
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
/* #0001       														*/
/*                                                                  */
/********************************************************************/

#ifndef INC_COMPOSITIONMGR_H_
#define INC_COMPOSITIONMGR_H_

typedef enum
{
    Null_ID,
    Anypale_ID
}ExternalDeviceID;

typedef enum
{
	eDeviceQuantity,
	eUpdatingStatus,
	eDeviceNo1

}CompositionGetID;

#define UPDATE_STATUS "01\0"
#define NO_UPDATE_STATUS "00\0"

#define ONE_DEVICE "01\0"
#define TWO_DEVICE "02\0"
#define ANYPAL_EQUIPMENT_NAME "ATP"

#define COMPOSITION_LENG 3

// Update composition when external device connect
void composition_Update(void);

// Delete composition when delete external device
void composition_Delete(void);


void composition_Init(void);

void* composition_Get(CompositionGetID ID);

void composition_SetUpdatingStatus(void);
void composition_ClearUpdatingStatus(void);

#endif /* INC_COMPOSITIONMGR_H_ */
