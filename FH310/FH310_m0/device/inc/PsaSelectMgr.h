#ifndef INC_PSASELECTMGR_H_
	#define INC_PSASELECTMGR_H_
/******************************************************************************/
//
//  File Name		: PsaSelectMgr.h
//  
//	General       	: This file contains function to handle the psa select paramter based on temperature and O2 condition
//
//  Product Name	: FH310_m0
//
//
//  Revision History:
//          Rev:      Date:       Editor:         
//          01        Oct 4, 2016     Quyen Ngo(MV)         
//          Description:  InitialRevision
//
/******************************************************************************/
#include "DualCoreCommon.h"

typedef enum
{
	HIGH_TEMP,
	NORM_TEMP,
	LOW_TEMP
}E_TempertureType;

typedef enum
{
	NORM_O2,
	LOW_O2
}E_O2Type;

E_PsaSelectName psaSelect_GetConditionAtStart(void);

void psaSelect_StartCheckO2Condition(void);

void psaSelect_StopCheckCondition(void);

void psaSelect_ChangeO2Threshold(uint16_t O2);

void psaSelect_ChangeLowTempThreshold(uint16_t temp);

void psaSelect_ChangeHighTempThreshold(uint16_t temp);

void psaSelect_Handling(float temperature, uint16_t o2Percent);

E_PsaSelectName psaSelect_GetConditionType(void);

E_PsaPortSelect psaSelect_CheckPort(void);

#endif /* INC_PSASELECTMGR_H_ */
/* end of file */
