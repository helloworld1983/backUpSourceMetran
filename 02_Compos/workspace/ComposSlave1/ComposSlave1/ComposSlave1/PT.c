/*******************************************************************************
*                                                                               
*    ̧�ٖ� �F   PT.c                                                         
*                                                                               
*    �T�v �F                                                                    
*          ���������͏����̃v���O����                                           
*                                                                               
*    ���JAPI :                                                                  
*                                                                               
*    ���l :                                                                     
*                                                                               
*                                                                               
*    �ύX���� :                                                                 
*            ��  �t    ��    �e                            Ver No.              
*            --------  ----------------------------------- -------              
*            10.04.15  ���ō쐬                            0.00                 
*            11.12.08  1�ō쐬                             1.00                 
*                                                                               
*******************************************************************************/

#include "PSoCAPI.h"    // PSoC API definitions for all User Modules
#include "I2C_CNT.h"
#include "PT.h"
#include "PV.h"

/******************************************************************************
  �֐��v���g�^�C�v
******************************************************************************/

void PT_init( void );
void PT_cnt( void );

/******************************************************************************
*** RAM�EROM
******************************************************************************/

unsigned int VT;

char 			temp_RAW_count;
signed short 	temp_RAW;
signed short 	temp_RAW_ary[temp_pt_num];
char 			temp_pt_count;
signed short 	temp_pt;
signed short 	temp_pt_ary[temp_pt_num];
signed short 	temp_pt_max;
unsigned char 	temp_pt_max_count;

unsigned short	P_00_CAL;
unsigned short	P_30_CAL;
unsigned short	P_60_CAL;

unsigned char 	Hi_Alarm;
unsigned char 	Hi_Alarm_count;
signed short 	Press;

/******************************************************************************
*** �v���O�����R�[�h
******************************************************************************/


/******************************************************************************
##
## Name    :PT_init
## Function:���������͏���������
## Input   :�Ȃ�
## Output  :�Ȃ�
## Memo    :�Ȃ�
##
******************************************************************************/
void PT_init( void )
{
	char i;
	
	Hi_Alarm = 0;
	Hi_Alarm_count = 0;
	VT = 0;
	temp_pt_count = 0;
	temp_RAW_count = 0;
	temp_pt_max = 0;
	Press = 0;
	for(i = 0; i < temp_pt_num; i++)
	{
		temp_pt_ary[i] = 0;
		temp_RAW_ary[i] = 0;
	} 
}

/******************************************************************************
##
## Name    :PT_cnt
## Function:���������͏���������
## Input   :�Ȃ�
## Output  :�Ȃ�
## Memo    :�Ȃ�
##
******************************************************************************/
void PT_cnt( void )
{
	char i;
	signed short temp_int1;
	signed short temp_int2;
	
	
	
	temp_RAW = 0;
	temp_RAW_ary[temp_RAW_count] = VT;
	for(i = 0; i < temp_pt_num; i++)
	{
		temp_RAW += temp_RAW_ary[i];
	} 
	PT_RAW = temp_RAW / temp_pt_num;
	temp_RAW_count++;
	if( temp_RAW_count >= temp_pt_num)
	{
		temp_RAW_count = 0;
	}
	
	
	/*********************/
	
	if( VT >= P_30_CAL)			// 30cm/H2O�ȏ�
	{
		temp_int2 = VT - P_30_CAL;
		temp_int1 = (P_60_CAL - P_30_CAL) / 30;
		Press = temp_int2 * 10 / temp_int1 + 300;
	}
	else if( VT >= P_00_CAL)	// 0cm/H2O�ȏ�
	{
		temp_int2 = VT - P_00_CAL;
		temp_int1 = (P_30_CAL - P_00_CAL) / 30;
		Press = temp_int2 * 10 / temp_int1;
	}
	else						// 0cm/H2O����
	{
		temp_int2 = P_00_CAL - VT;
		temp_int1 = (P_30_CAL - P_00_CAL) / 30;
		Press = temp_int2 * -10 / temp_int1;
//		Press = 0;
	}
	/*********************/
	
	temp_pt = 0;
	temp_pt_ary[temp_pt_count] = Press;			// RAM[10,11] ������
	for(i = 0; i < temp_pt_num; i++)
	{
		temp_pt += temp_pt_ary[i] / temp_pt_num;
	} 
	PTm = temp_pt;
	temp_pt_count++;
	if( temp_pt_count >= temp_pt_num)
	{
		temp_pt_count = 0;
	}
	if( InsCnt.A.Mode != 0 )					// �C�j�V������������Ȃ��B
	{
		if(Hi_PrsA < Press / 10)
		{
			Hi_Alarm = 1;
		}
		
		// �ǂ�������Ă��Ȃ�
		if (INSTIM_Counter >= Tim_TotalA)
		{
			Hi_Alarm = 0;
		}
		
		// �ǂ�������Ă��Ȃ�
		if((InsCnt.A.Phase == Trig_View)		// �g���K�r���[
		 && ( FLOW_PHASE.A.INS == 1 ))
		{
			Hi_Alarm = 0;
		}
		
		if( Hi_Alarm == 1)
		{
			Hi_Alarm_count++;
		}
		else
		{
			Hi_Alarm_count = 0;
		}
		
		if( Hi_Alarm_count > 16)
		{
			Hi_Alarm_count = 0;
			Hi_Alarm = 0;
		}
	}
	
	
	
	/******************************************/
	// 2�Ԗڈ����o�͂���
	PTh = 0;
	temp_pt_max = 0;
	temp_pt_max_count = 0;
	for(i = 0; i < temp_pt_num; i++)
	{
		if(temp_pt_max < temp_pt_ary[i])
		{
			temp_pt_max = temp_pt_ary[i];
			temp_pt_max_count = i;
		}
	} 
	for(i = 0; i < temp_pt_num; i++)
	{
		if((PTh < temp_pt_ary[i])
		 && (temp_pt_max_count != i))
			PTh = temp_pt_ary[i];
	} 
	// 2�Ԗڈ����o�����
	/******************************************/
}


