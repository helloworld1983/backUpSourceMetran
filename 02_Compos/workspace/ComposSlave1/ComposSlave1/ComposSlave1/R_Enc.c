/*******************************************************************************
*                                                                               
*    ̧�ٖ� �F   R_Enc.c                                                        
*                                                                               
*    �T�v �F                                                                    
*          ���[�^���[�G���R�[�_�����̃v���O����                                 
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
#include "I2C_CNT.h"	// 
#include "R_Enc.h"		// 

/******************************************************************************
*** RAM�EROM
******************************************************************************/

unsigned char countA;
unsigned char countA_old;
unsigned char countB;
unsigned char countB_old;
int R_Enc_count;

/******************************************************************************
*** �v���O�����R�[�h
******************************************************************************/


/******************************************************************************
##
## Name    :R_Enc_init
## Function:���[�^���[�G���R�[�_����������
## Input   :�Ȃ�
## Output  :�Ȃ�
## Memo    :�Ȃ�
##
******************************************************************************/
void R_Enc_init( void )
{
	countA = 0;
	countB = 0;
	countA_old = 0;
	countB_old = 0;
	R_Enc_count = 0;
	Counter8_1_Start();
	Counter8_2_Start();
}

/******************************************************************************
##
## Name    :R_Enc
## Function:���[�^���[�G���R�[�_�v������
## Input   :�Ȃ�
## Output  :�Ȃ�
## Memo    :�Ȃ�
##
******************************************************************************/
void  R_Enc( void )
{
	BYTE tempA;
	BYTE tempB;
	
	tempA = Counter8_1_bReadCounter();
	tempB = Counter8_2_bReadCounter();
	
	countA = countA_old - tempA;
	countB = countB_old - tempB;
	R_Enc_count = R_Enc_count + countA - countB; 
	
	countA_old = tempA;
	countB_old = tempB;
	
	Enc = R_Enc_count;
}


