/*******************************************************************************
*                                                                               
*    ̧�ٖ� �F   BZ.c                                                           
*                                                                               
*    �T�v �F                                                                    
*          �u�U�[�o�͏����̃v���O����                                           
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
#include "BZ.h"

/******************************************************************************
*** RAM�EROM
******************************************************************************/

unsigned char bz_cunt;

/******************************************************************************
*** �v���O�����R�[�h
******************************************************************************/


/******************************************************************************
##
## Name    :BZ_init
## Function:�u�U�[�o�͏���������
## Input   :�Ȃ�
## Output  :�Ȃ�
## Memo    :�Ȃ�
##
******************************************************************************/
void BZ_init( void )
{
	bz_cunt = 0;
}

/******************************************************************************
##
## Name    :BZ_cnt
## Function:�u�U�[�o�͏���
## Input   :�Ȃ�
## Output  :�Ȃ�
## Memo    :�Ȃ�
##
******************************************************************************/
void BZ_cnt( void )
{
	if( BZ == 0)
	{
		PWM8_2_Stop();
	}
	else if( BZ == 1)
	{
		PWM8_2_WritePeriod(99);
		/******* Modify S 111 *********/
		if (PSoC1.DATA.BZ_VOL == 10)
		PWM8_2_WritePulseWidth(99);
		else if (PSoC1.DATA.BZ_VOL == 9)
		PWM8_2_WritePulseWidth(89);
		else if (PSoC1.DATA.BZ_VOL == 8)
		PWM8_2_WritePulseWidth(79);
		else if (PSoC1.DATA.BZ_VOL == 7)
		PWM8_2_WritePulseWidth(69);
		else if (PSoC1.DATA.BZ_VOL == 6)
		PWM8_2_WritePulseWidth(59);
		else if (PSoC1.DATA.BZ_VOL == 5)
		PWM8_2_WritePulseWidth(49);
		else if (PSoC1.DATA.BZ_VOL == 4)
		PWM8_2_WritePulseWidth(39);
		else if (PSoC1.DATA.BZ_VOL == 3)
		PWM8_2_WritePulseWidth(29);
		else if (PSoC1.DATA.BZ_VOL == 2)
		PWM8_2_WritePulseWidth(19);
		else if (PSoC1.DATA.BZ_VOL == 1)
		PWM8_2_WritePulseWidth(9);
		
//		PWM8_2_WritePulseWidth(99);
		/******* Modify E 111 *********/
		PWM8_2_Start();							// �Ƃ肠�����炷
												// ��ŉ�����ς���Ƃ��H
	}
	else if( BZ == 2)
	{
		bz_cunt++;
		if(bz_cunt >= 8)						// 0.1�b
		{
			bz_cunt = 0;
		}
		if(bz_cunt < 1)							// 0.0125�b
		{
			PWM8_2_WritePeriod(99);
		/******* Modify S 111 *********/
		if (PSoC1.DATA.BZ_VOL == 10)
		PWM8_2_WritePulseWidth(79);
		else if (PSoC1.DATA.BZ_VOL == 9)
		PWM8_2_WritePulseWidth(71);
		else if (PSoC1.DATA.BZ_VOL == 8)
		PWM8_2_WritePulseWidth(63);
		else if (PSoC1.DATA.BZ_VOL == 7)
		PWM8_2_WritePulseWidth(55);
		else if (PSoC1.DATA.BZ_VOL == 6)
		PWM8_2_WritePulseWidth(47);
		else if (PSoC1.DATA.BZ_VOL == 5)
		PWM8_2_WritePulseWidth(39);
		else if (PSoC1.DATA.BZ_VOL == 4)
		PWM8_2_WritePulseWidth(31);
		else if (PSoC1.DATA.BZ_VOL == 3)
		PWM8_2_WritePulseWidth(23);
		else if (PSoC1.DATA.BZ_VOL == 2)
		PWM8_2_WritePulseWidth(15);
		else if (PSoC1.DATA.BZ_VOL == 1)
		PWM8_2_WritePulseWidth(7);
		
//		PWM8_2_WritePulseWidth(99);
		/******* Modify E 111 *********/
			PWM8_2_Start();						// �Ƃ肠�����炷
												// ��ŉ�����ς���Ƃ��H
		}
		else
		{
			PWM8_2_Stop();
		}
		
	}
}


