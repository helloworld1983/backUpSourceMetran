/*******************************************************************************
*                                                                               
*    ̧�ٖ� �F   PV.c                                                           
*                                                                               
*    �T�v �F                                                                    
*          ���ُo�͂̃v���O����                                               
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
*            11.12.19  SIMV�o�O                            1.01                 
*            11.12.20  �L���N�A���[����Ɍċz�����������Ȃ�o�O�C��             
*                      SIMV�d�l�ύX                        1.02                                                     
*            12.01.06  �t���[�f�[�^�ύX                    1.03                
*                                                                               
*******************************************************************************/

#include "PSoCAPI.h"    // PSoC API definitions for all User Modules
#include "I2C_CNT.h"	// 
#include "PV.h"			// 
#include "PT.h"			// 
#include "Flow.h"		// 

/******************************************************************************
*** RAM�EROM
******************************************************************************/

signed short	INSTIM_Counter;
unsigned short	PV_PEEP_old;
unsigned char	Hi_Alarm_old;
unsigned char	ERR_EPV_count;
unsigned char	PT_ERR_count;
unsigned char	PT_DCnct_count;
unsigned char	D_Cnect_count;
signed short	pt_PIP1;
signed short	pt_PEEP1;
signed short	SIMV_Counter;
unsigned char	Trig_mode;
unsigned char	Insp_Log;
	/******* Modify S 102 *********/
unsigned short	CarryA;
	/******* Modify E 102 *********/
	/******* Modify S 110 *********/
unsigned short	E002_Counter;
	/******* Modify E 110 *********/
	/******* Modify S 112 *********/
unsigned char	SIMV;
	/******* Modify E 112 *********/


/******************************************************************************
  �֐��v���g�^�C�v
******************************************************************************/

void PV_init( void );
void PV_cnt1( void );
void PV_cnt2( void );
void E002_timeOut( void );

/******************************************************************************
*** �v���O�����R�[�h
******************************************************************************/


/******************************************************************************
##
## Name    :PV_init
## Function:���ُo�͏���������
## Input   :�Ȃ�
## Output  :�Ȃ�
## Memo    :�Ȃ�
##
******************************************************************************/
void PV_init( void )
{
	DAC9_1_Start(DAC9_1_FULLPOWER);
	DAC9_1_WriteStall(0);
	INSTIM_Counter = Tim_PIP2A * 2;
	PV_PEEP_old = 0;
	Hi_Alarm_old = 0;
	ERR_EPV_count = 0;
	PT_ERR_count = 0;
	PT_DCnct_count = 0;
	pt_PIP1 = 0;
	pt_PEEP1 = 200;
	D_Cnect_count = 0;
	D_Cnect_state = 0;
	SIMV_Counter = 0;
	Trig_mode = Trig_init1;
	/******* Modify S 110 *********/
	E002_Counter = 1800;
	/******* Modify E 110 *********/
	/******* Modify S 112 *********/
	SIMV = 0;
	/******* Modify E 112 *********/

}
	/******* Modify S 110 *********/
/******************************************************************************
##
## Name    :E002_timeOut
## Function:E002�s���ю��Ԑ���
## Input   :�Ȃ�
## Output  :�Ȃ�
## Memo    :E002�s���ю��Ԑ���
##
******************************************************************************/
void E002_timeOut( void )
{
	if(E002_Counter > 0)
	{
		E002_Counter--;
	}
}

	/******* Modify E 110 *********/

/******************************************************************************
##
## Name    :PV_cnt
## Function:���ُo�͏���1
## Input   :�Ȃ�
## Output  :�Ȃ�
## Memo    :1.25msec����
##
******************************************************************************/
void PV_cnt1( void )
{
	signed short error;
	
	// SuperViserMode
	//	STATE.DATA.InsCnt.A.Mode = 0;�C�j�V������������Ȃ��B
	//	STATE.DATA.InsCnt.A.Mode = 1;JET ON
	//	STATE.DATA.InsCnt.A.Mode = 2;JET OFF�A�o�b�N�A�b�v��
	//	STATE.DATA.InsCnt.A.Mode = 3;�����ċz���j�^�[��
	//	STATE.DATA.InsCnt.A.Mode = 4;�f�o�b�O�A����
	//	STATE.DATA.InsCnt.A.Mode = 5;�V�X�e���R���v���C�A���X
	//	STATE.DATA.InsCnt.A.Mode = 6;�p�[�J�b�V����
	/**************************************************************************/
	/******* Modify S 101 *********/
	// ���[�h�ؑ֎��N���A
	if(INS_cnt_OLD_Mode != InsCnt.A.Mode)
	{
		Trig_mode = Trig_init2;
	}
	// ���[�h�ؑ֎��N���A
	/******* Modify E 101 *********/
	/**************************************************************************/
	// �������Z���T�j�����o�͂���
	if( InsCnt.A.Mode != 0 )					// �C�j�V������������Ȃ��B
	{
	/******* Modify S 110 *********/
		if((PTm < -30)
		 && (E002_Counter == 0))
//		if(PTm < -30)
	/******* Modify E 110 *********/
		{
			PT_ERR_count++;
			if(PT_ERR_count > 240)
			{
				PT_ERR_count = 240;
				PSoC1.DATA.ERR.BIT.ER_PRES = 1;
			}
		}
		else
		{
			PT_ERR_count = 0;
		}
	}
	// �������Z���T�j�����o�����
	/**************************************************************************/
	// EP�o���u�j������͂���
	/******* Modify S 111 *********/
	if(( InsCnt.A.Mode == 2 )					// JET OFF�A�o�b�N�A�b�v��
	 || ( InsCnt.A.Mode == 6 ))					// �p�[�J�b�V����
//	if( InsCnt.A.Mode == 2 )					// JET OFF�A�o�b�N�A�b�v��
	/******* Modify E 111 *********/
	{
		if(Hi_PrsA < Press / 10)
		{
			ERR_EPV_count++;
			if((ERR_EPV_count > 80)
			 || (ERR_EPV_count > Tim_PIP2B * 0.8))
			{
				ERR_EPV_count = 80;
				PSoC1.DATA.ERR.BIT.ER_EPV = 1;
			}
		}
		else
		{
			ERR_EPV_count = 0;
			PSoC1.DATA.ERR.BIT.ER_EPV = 0;
		}
	}
	
	// EP�o���u�j�����肨���
	/**************************************************************************/
	// ��H�O��͂���
	/******* Modify S 111 *********/
	if(( InsCnt.A.Mode == 2 )					// JET OFF�A�o�b�N�A�b�v��
	 || ( InsCnt.A.Mode == 6 ))					// �p�[�J�b�V����
//	if( InsCnt.A.Mode == 2 )					// JET OFF�A�o�b�N�A�b�v��
	/******* Modify E 111 *********/
	{
        if( INSTIM_Counter >= Tim_PIP2A)		// �ċC�� Q_comment exhalation
		{
			if(PV_PEEPA >= 1)					// PEEP��1�ȏ�
			{
				if(Real_flow > 100)				// 10L�ȏ�͂������Ă��邩�H
				{
					D_Cnect_count++;
					if(D_Cnect_count > 40)
					{
						PSoC1.DATA.ERR.BIT.GAS_DCnct = 1;
						D_Cnect_count = 40;
					}
				}
				else
				{
					PSoC1.DATA.ERR.BIT.GAS_DCnct = 0;
					D_Cnect_count = 0;
				}
			}
			else
			{
				PSoC1.DATA.ERR.BIT.GAS_DCnct = 0;
				D_Cnect_count = 0;
			}
		}
		else
		{
			PSoC1.DATA.ERR.BIT.GAS_DCnct = 0;
			D_Cnect_count = 0;
		}
	}
	else
	{
		PSoC1.DATA.ERR.BIT.GAS_DCnct = 0;
		D_Cnect_count = 0;
	}
	// ��H�O�ꂨ���
	/**************************************************************************/
	// ���������C���O��͂���
	/******* Modify S 111 *********/
	if(( InsCnt.A.Mode == 2 )					// JET OFF�A�o�b�N�A�b�v��
	 || ( InsCnt.A.Mode == 6 ))					// �p�[�J�b�V����
//	if( InsCnt.A.Mode == 2 )					// JET OFF�A�o�b�N�A�b�v��
	/******* Modify E 111 *********/
	{
		if( INSTIM_Counter >= Tim_PIP2A)		// �ċC��
		{
			if(PV_PEEPA >= 1)					// PEEP��1�ȏ�
			{
				if((Real_flow <= 100)			// 10L�ȏ�͂������Ă��邩�H
				 && (Real_flow >= 5)
				 && (PTm <= 5))					// ����0.5cmH2�ȉ���
				{
					PT_DCnct_count++;
					if(PT_DCnct_count > 240)
					{
						PSoC1.DATA.ERR.BIT.PT_DCnct = 1;
						PT_DCnct_count = 240;
					}
				}
				else
				{
					PSoC1.DATA.ERR.BIT.PT_DCnct = 0;
					PT_DCnct_count = 0;
				}
			}
			else
			{
				PSoC1.DATA.ERR.BIT.PT_DCnct = 0;
				PT_DCnct_count = 0;
			}
		}
		else									// �z�C��
		{
			PSoC1.DATA.ERR.BIT.PT_DCnct = 0;
			PT_DCnct_count = 0;
		}
	}
	else										// �z�C��
	{
		PSoC1.DATA.ERR.BIT.PT_DCnct = 0;
		PT_DCnct_count = 0;
	}
	
	// ���������C���O�ꂨ���
	/**************************************************************************/
	// ���������C���L���N�͂���
	
		/******* Modify S 110 *********/
	if (pt_PIP1 < PTh)							// �ő�l
		pt_PIP1 = PTh;
//	if (pt_PIP1 < PTm)							// �ő�l
//		pt_PIP1 = PTm;
		/******* Modify E 110 *********/
	if (pt_PEEP1 > PTm)							// �ŏ��l
		pt_PEEP1 = PTm;
	
	/******* Modify S 111 *********/
	if(( InsCnt.A.Mode == 2 )					// JET OFF�A�o�b�N�A�b�v��
	 || ( InsCnt.A.Mode == 6 ))					// �p�[�J�b�V����
//	if( InsCnt.A.Mode == 2 )					// JET OFF�A�o�b�N�A�b�v��
	/******* Modify E 111 *********/
	{
		if( INSTIM_Counter == 1 )
		{
			if ( KINK_DIFB >= 6)
			{
		/******* Modify S 110 *********/
		/******* Modify S 111 *********/
				if ((pt_PIP1 - pt_PEEP1) >= KINK_DIFB * 1)		// 10%
		/******* Modify E 111 *********/
//				if ((pt_PIP1 - pt_PEEP1) >= KINK_DIFB * 5)		// 60%
//				if ((pt_PIP1 - pt_PEEP1) >= KINK_DIFB * 6)		// 60%
		/******* Modify E 110 *********/
				{
					PSoC1.DATA.ERR.BIT.PT_KINK = 0;	// �����C���L���N
				}
				else
				{
					PSoC1.DATA.ERR.BIT.PT_KINK = 1;	// �����C���L���N
				}
			}
			else if ( KINK_DIFB >= 2)
			{
				if ((pt_PIP1 - pt_PEEP1)   >= KINK_DIFB * 3)	// 30%
				{
					PSoC1.DATA.ERR.BIT.PT_KINK = 0;	// �����C���L���N
				}
				else
				{
					PSoC1.DATA.ERR.BIT.PT_KINK = 1;	// �����C���L���N
				}
			}
			else
			{
				PSoC1.DATA.ERR.BIT.PT_KINK = 0;	// �����C���L���N
			}
			pt_PIP1 = PTm;
		}
		else if( INSTIM_Counter == Tim_PIP2A )
		{
			if ( KINK_DIFB >= 6)
			{
		/******* Modify S 110 *********/
		/******* Modify S 111 *********/
				if ((pt_PIP1 - pt_PEEP1)   >= KINK_DIFB * 1)	// 10%
		/******* Modify E 111 *********/
//				if ((pt_PIP1 - pt_PEEP1)   >= KINK_DIFB * 5)	// 60%
//				if ((pt_PIP1 - pt_PEEP1)   >= KINK_DIFB * 6)	// 60%
		/******* Modify E 110 *********/
				{
					PSoC1.DATA.ERR.BIT.PT_KINK = 0;	// �����C���L���N
				}
				else
				{
					PSoC1.DATA.ERR.BIT.PT_KINK = 1;	// �����C���L���N
				}
			}
			else if ( KINK_DIFB >= 2)
			{
				if ((pt_PIP1 - pt_PEEP1)   >= KINK_DIFB * 3)	// 30%
				{
					PSoC1.DATA.ERR.BIT.PT_KINK = 0;	// �����C���L���N
				}
				else
				{
					PSoC1.DATA.ERR.BIT.PT_KINK = 1;	// �����C���L���N
				}
			}
			else
			{
				PSoC1.DATA.ERR.BIT.PT_KINK = 0;	// �����C���L���N
			}
			pt_PEEP1 = PTm;
		}
		else
		{
		}
	}
	else
	{
		PSoC1.DATA.ERR.BIT.PT_KINK = 0;			// �����C���L���N
	}
	if(PSoC1.DATA.ERR.BIT.PT_KINK == 1)
	{
		D_Cnect_state = 1;
	}
	else if((PSoC1.DATA.ERR.BIT.PT_KINK == 0)
	 && (D_Cnect_state == 1))
	{
		D_Cnect_state = 2;
	}
	/******* Modify S 102 *********/
//	else if((TOut_base_flow1_flag == 1)
//	 && (D_Cnect_state == 2))
	else if((TOut_base_flow1_flag == 1)
	 && (PSoC1.DATA.ERR.BIT.PT_KINK == 0))
	{
		D_Cnect_state = 0;
	}
	else if((Sensor_Err == 0)
	 && (PSoC1.DATA.ERR.BIT.PT_KINK == 0))
	/******* Modify E 102 *********/
	{
		D_Cnect_state = 0;
	}
	
	// ���������C���L���N�����
	/**************************************************************************/
	// PV�̊e���[�h�ɂ�铮��͂���
	if( InsCnt.A.Mode == 1 )					// JET ON
	{
		/******* Modify S 112 *********/
		DOUCHOU.A.SIMV = 0;
		SIMV = 0;
		/******* Modify E 112 *********/
		PV_PEEPA = PV_PEEPB;
		Tim_TotalA = Tim_TotalB;
		// �ċz������͂���
		if( INSTIM_Counter == 0)
		{
			PV_PIP1A = PV_PIP1B;
			PV_PIP2A = PV_PIP2B;
			Tim_PIP2A = Tim_PIP2B;
			Trgt_PresA = Trgt_PresB;
			Hi_PrsA = Hi_PrsB;
			
			InsCnt.A.Phase = Ins_Strt;			// �z�C�J�n
		}
		else if( INSTIM_Counter == Tim_PIP2A)
		{
			InsCnt.A.Phase = Exh_Strt;			// �ċC�J�n
		}
		else if( INSTIM_Counter == Tim_PIP2A - 1 )
		{
			InsCnt.A.Phase = Ins_End;			// �z�C�I��
		}
		else if( INSTIM_Counter == Tim_TotalA -1)
		{
			InsCnt.A.Phase = Exh_End;			// �ċC�I��
		}
		else if( INSTIM_Counter < Tim_PIP2A - 1)
		{
			InsCnt.A.Phase = Ins_Sec;			// �z�C��2
		}
		else if( INSTIM_Counter < Tim_TotalA -1)
		{
			InsCnt.A.Phase = Exh;				// �ċC��
		}
		else
		{
			InsCnt.A.Phase = Trig_View;			// �g���K�r���[
		}
		
		INSTIM_Counter++;
		if (INSTIM_Counter >= Tim_TotalA )
		{
			INSTIM_Counter = 0;
		}
		
        if (Cmnd.A.Ins_OP == 0)					// �ċz�쓮���łȂ� Q_comment Apnea when breath
		{
			INSTIM_Counter =  Tim_TotalA  -1;
		}
		
		// �ċz�����肨���
		SIMV_Counter = 0;
		Trig_mode = Trig_init1;
	}
    else if( InsCnt.A.Mode == 2 )				// JET OFF���A�o�b�N�A�b�v�� Q_comment backup?
	{
		if((Hi_Alarm_old == 0)
		 && (Hi_Alarm == 1))
		{
			INSTIM_Counter = Tim_PIP2A ;		// �ċC�ɓ]��
		}
		PV_PEEPA = PV_PEEPB;
		
		// ��H�O�ꌟ�o
		if(D_Cnect_state == 2)
		{
			Tim_TotalA = Tim_TotalB + 8*20;		// 2�b�ǉ�
		}
		else
		{
			Tim_TotalA = Tim_TotalB;
		}
		
	/**************************************************************************/
	/******* Modify S 102 *********/
		// �ċz������͂���
		// �g���K�[�r���[��1�b �Ƃ肠�������ߑł�
		if( INSTIM_Counter == 0)
		{
			PV_PIP1A = PV_PIP1B;		// �X���[�u�ւ̃R�}���h
			PV_PIP2A = PV_PIP2B;		// �X���[�u�ւ̃R�}���h
			Tim_PIP2A = Tim_PIP2B;		// RAM[12,13] 
			Tim_TrigA = Tim_TrigB;		// RAM[16,17] 
			Trgt_PresA = Trgt_PresB;	// RAM[18,19] 
			Hi_PrsA = Hi_PrsB;			// RAM[20] �����A���[��
			
			InsCnt.A.Phase = Ins_Strt;		// �z�C�J�n
			if(No_Exh_count == 0)
			{
					FLOW_PHASE.A.No_Exh = 0;
			}
			else if(No_Exh_count >= 3)
			{
				FLOW_PHASE.A.No_Exh = 1;
				No_Exh_count = 3;
			}
			No_Exh_count ++;
		}
		else if( INSTIM_Counter < Tim_PIP2A - 1)
		{
			InsCnt.A.Phase = Ins_Sec;		// �z�C��2
		}
		else if( INSTIM_Counter == Tim_PIP2A - 1 )
		{
			InsCnt.A.Phase = Ins_End;		// �z�C�I��
		}
		else if( INSTIM_Counter == Tim_PIP2A)
		{
			InsCnt.A.Phase = Exh_Strt;		// �ċC�J�n
		}
		else if( INSTIM_Counter < Tim_TotalA - Tim_TrigA -1 + CarryA)
		{
			InsCnt.A.Phase = Exh;			// �ċC��
		}
		else if( INSTIM_Counter == Tim_TotalA - Tim_TrigA -1 + CarryA)
		{
			InsCnt.A.Phase = Exh_End;		// �ċC�I��
			/******* Modify S 112 *********/
			DOUCHOU.A.SIMV = SIMV;
			SIMV = 0;
			/******* Modify E 112 *********/
		}
//		else if( 0)
		/******* Modify S 110 *********/
		else if( INSTIM_Counter >= Tim_TotalA - Tim_TrigA/2 -1 + CarryA)
		{
			InsCnt.A.Phase = Trig_View2;		// �g���K�r���[
		}
		/******* Modify E 110 *********/
		else
		{
			InsCnt.A.Phase = Trig_View;	// �g���K�r���[
		}
		// �ċz�����肨���
		
		if(( InsCnt.A.Phase >= Ins_Strt )			// 0	�z�C�J�n
		 && ( InsCnt.A.Phase <= Ins_End ))		// 3	�z�C�I��
		{
			
		}
		else
		{
			PV_PIP1A = PV_PIP2B;
		}
		
/******* Modify S 111 *********/
		INSTIM_Counter++;			// 
		if (INSTIM_Counter >= Tim_TotalA + CarryA)
		{
			INSTIM_Counter = 0;			// 
			CarryA = 0;
		}
/******* Modify E 111 *********/
		
		PV_PEEPA = PV_PEEPB;		// �X���[�u�ւ̃R�}���h
		Tim_TotalA = Tim_TotalB;	// RAM[10,11] 
		
        if (Cmnd.A.Ins_OP == 0)				// �ċz�쓮���łȂ� Q_comment Apnea when breath
		{
			INSTIM_Counter = Tim_TotalA - Tim_TrigA -1;			// 
			
			PV_PIP1A = PV_PIP1B;		// �X���[�u�ւ̃R�}���h
			PV_PIP2A = PV_PIP2B;		// �X���[�u�ւ̃R�}���h
			Tim_PIP2A = Tim_PIP2B;		// RAM[12,13] 
			Tim_TrigA = Tim_TrigB;		// RAM[16,17] 
			Trgt_PresA = Trgt_PresB;	// RAM[18,19] 
			Hi_PrsA = Hi_PrsB;			// RAM[20] �����A���[��
		}
		
		if((InsCnt.A.Phase == Trig_View)		// �g���K�r���[
		 && ( DEF_Flow >= Max_Ins_Flow))
		{
			CarryA = Tim_TotalA + CarryA - INSTIM_Counter;
/******* Modify S 111 *********/
			InsCnt.A.Phase = Ins_Strt;		// �z�C�J�n
/******* Modify E 111 *********/
			/******* Modify S 112 *********/
			SIMV = 0;
//			SIMV = 1;
			/******* Modify E 112 *********/
			INSTIM_Counter = 0;			// 
		}
		/******* Modify S 110 *********/
		else if((InsCnt.A.Phase == Trig_View2)		// �g���K�r���[
		 && ( DEF_Flow > Max_Exh_Flow))
		{
			CarryA = Tim_TotalA + CarryA - INSTIM_Counter;
/******* Modify S 111 *********/
			InsCnt.A.Phase = Ins_Strt;		// �z�C�J�n
/******* Modify E 111 *********/
			INSTIM_Counter = 0;			// 
		}
		/******* Modify E 110 *********/
		
		SIMV_Counter = Tim_TotalA * 0.7 -1;		// �g���Ă��Ȃ�
		
		if(InsCnt.A.Phase == Trig_View)
//		if( INSTIM_Counter == 0)
		{
//			LED_2_On();
		}
		else
		{
//			LED_2_Off();
		}
		
	/******* Modify E 102 *********/
	/**************************************************************************/
	}
    else if( InsCnt.A.Mode == 3 )				// �����ċz���j�^�[�� Q_comment Spontaneous breathing monitoring in progress
	{
		PV_PIP1A = 0;
		PV_PIP2A = 0;
		PV_PEEPA = 0;
		INSTIM_Counter = Tim_PIP2A ;			// �ċC�ɓ]��
		/******* Modify S 112 *********/
		DOUCHOU.A.SIMV = 0;
		SIMV = 0;
		/******* Modify E 112 *********/
		
		// �ċz��
		if( FLOW_PHASE.A.INS == 1 )
		{
			if(InsCnt.A.Phase == Trig_View)		// �g���K�r���[
			{
				InsCnt.A.Phase = Ins_Strt;		// �z�C�J�n
			}
			else if(InsCnt.A.Phase == Ins_Strt)	// �z�C�J�n
			{
				InsCnt.A.Phase = Ins_Sec;		// �z�C��2
			}
		}
		else									// �ċz����
		{
			if(InsCnt.A.Phase == Ins_Sec)		// �z�C��2
			{
				InsCnt.A.Phase = Exh;			// �ċC��
			}
			else if( InsCnt.A.Phase == Exh)		// �ċC��
			{
				InsCnt.A.Phase = Trig_View;		// �g���K�r���[
			}
			else
			{
				InsCnt.A.Phase = Trig_View;		// �g���K�r���[
			}
		}
		
		Trig_mode = Trig_init1;
	}
	else if( InsCnt.A.Mode == 4 )
	{
		INSTIM_Counter = Tim_PIP2A * 2;
		PV_PEEPA = PV_PEEPB;
		SIMV_Counter = Tim_TotalA * 0.7 -1;
		Trig_mode = Trig_init1;
		/******* Modify S 112 *********/
		DOUCHOU.A.SIMV = 0;
		SIMV = 0;
		/******* Modify E 112 *********/
	}
	else if( InsCnt.A.Mode == 5 )				// �V�X�e���R���v���C�A���X
	{
		/******* Modify S 112 *********/
		DOUCHOU.A.SIMV = 0;
		SIMV = 0;
		/******* Modify E 112 *********/
		if((Hi_Alarm_old == 0)
		 && (Hi_Alarm == 1))
		{
			INSTIM_Counter = Tim_PIP2A;
		}
			PV_PEEPA = PV_PEEPB;
			if(D_Cnect_state == 2)
			{
				Tim_TotalA = Tim_TotalB + 8*20;
			}
			else
			{
				Tim_TotalA = Tim_TotalB;
			}
		// �ċz������͂���
		// �g���K�[�r���[��1�b �Ƃ肠�������ߑł�
		if( INSTIM_Counter == 0)
		{
			PV_PIP1A = PV_PIP1B;
			PV_PIP2A = PV_PIP2B;
			Tim_PIP2A = Tim_PIP2B;
			Tim_TrigA = Tim_TrigB;
			Trgt_PresA = Trgt_PresB;
			Hi_PrsA = Hi_PrsB;
			
			InsCnt.A.Phase = Ins_Strt;			// �z�C�J�n
			if(No_Exh_count == 0)
			{
					FLOW_PHASE.A.No_Exh = 0;
			}
			else if(No_Exh_count >= 3)
			{
				FLOW_PHASE.A.No_Exh = 1;
				No_Exh_count = 3;
			}
			No_Exh_count ++;
		}
		else if( INSTIM_Counter < Tim_PIP2A - 1)
		{
			InsCnt.A.Phase = Ins_Sec;			// �z�C��2
		}
		else if( INSTIM_Counter == Tim_PIP2A - 1 )
		{
			InsCnt.A.Phase = Ins_End;			// �z�C�I��
		}
		else if( INSTIM_Counter == Tim_PIP2A)
		{
			InsCnt.A.Phase = Exh_Strt;			// �ċC�J�n
		}
		else if( INSTIM_Counter == Tim_TotalA -1 )
		{
			InsCnt.A.Phase = Exh_End;			// �ċC�I��
		}
		else if( INSTIM_Counter < Tim_TotalA -1)
		{
			InsCnt.A.Phase = Exh;				// �ċC��
		}
		else
		{
			InsCnt.A.Phase = Trig_View;			// �g���K�r���[
		}
		// �ċz�����肨���
		
		INSTIM_Counter++;
		if (INSTIM_Counter >= Tim_TotalA )
		{
			INSTIM_Counter = 0;
		}
		
		PV_PEEPA = PV_PEEPB;
		Tim_TotalA = Tim_TotalB;
		
        if (Cmnd.A.Ins_OP == 0)					// �ċz�쓮���łȂ� Q_comment Apnea when breath
		{
			INSTIM_Counter = Tim_TotalA -1;			// 
			
			PV_PIP1A = PV_PIP1B;
			PV_PIP2A = PV_PIP2B;
			Tim_PIP2A = Tim_PIP2B;
			Tim_TrigA = Tim_TrigB;
			Trgt_PresA = Trgt_PresB;
			Hi_PrsA = Hi_PrsB;
		}
		
		if((InsCnt.A.Phase == Trig_View)		// �g���K�r���[
		 && ( DEF_Flow >= Max_Ins_Flow))
		{
			InsCnt.A.Phase = Ins_Strt;			// �z�C�J�n
			INSTIM_Counter = 0;
		}
		
	}
		/******* Modify S 111 *********/
	else if( InsCnt.A.Mode == 6 )				// �p�[�J�b�V����
	{
		/******* Modify S 112 *********/
		DOUCHOU.A.SIMV = 0;
		SIMV = 0;
		/******* Modify E 112 *********/
		if((Hi_Alarm_old == 0)
		 && (Hi_Alarm == 1))
		{
//			INSTIM_Counter = Tim_PIP2A ;		// �ċC�ɓ]��
		}
		PV_PEEPA = PV_PEEPB;
		
		// ��H�O�ꌟ�o
		if(D_Cnect_state == 2)
		{
			Tim_TotalA = Tim_TotalB + 8*20;		// 2�b�ǉ�
		}
		else
		{
			Tim_TotalA = Tim_TotalB;
		}
		
	/**************************************************************************/
	/******* Modify S 102 *********/
		// �ċz������͂���
		// �g���K�[�r���[��1�b �Ƃ肠�������ߑł�
		if( INSTIM_Counter == 0)
		{
			PV_PIP1A = PV_PIP1B;		// �X���[�u�ւ̃R�}���h
			PV_PIP2A = PV_PIP2B;		// �X���[�u�ւ̃R�}���h
			Tim_PIP2A = Tim_PIP2B;		// RAM[12,13] 
			Tim_TrigA = Tim_TrigB;		// RAM[16,17] 
			Trgt_PresA = Trgt_PresB;	// RAM[18,19] 
			Hi_PrsA = Hi_PrsB;			// RAM[20] �����A���[��
			
			InsCnt.A.Phase = Ins_Strt;		// �z�C�J�n
			if(No_Exh_count == 0)
			{
					FLOW_PHASE.A.No_Exh = 0;
			}
			else if(No_Exh_count >= 3)
			{
				FLOW_PHASE.A.No_Exh = 1;
				No_Exh_count = 3;
			}
			No_Exh_count ++;
		}
		else if( INSTIM_Counter < Tim_PIP2A - 1)
		{
			InsCnt.A.Phase = Ins_Sec;		// �z�C��2
		}
		else if( INSTIM_Counter == Tim_PIP2A - 1 )
		{
			InsCnt.A.Phase = Ins_End;		// �z�C�I��
		}
		else if( INSTIM_Counter == Tim_PIP2A)
		{
			InsCnt.A.Phase = Exh_Strt;		// �ċC�J�n
		}
		else if( INSTIM_Counter < Tim_TotalA - Tim_TrigA -1 + CarryA)
		{
			InsCnt.A.Phase = Exh;			// �ċC��
		}
		else if( INSTIM_Counter == Tim_TotalA - Tim_TrigA -1 + CarryA)
		{
			InsCnt.A.Phase = Exh_End;		// �ċC�I��
		}
//		else if( 0)
		/******* Modify S 110 *********/
		else if( INSTIM_Counter >= Tim_TotalA - Tim_TrigA/2 -1 + CarryA)
		{
			InsCnt.A.Phase = Trig_View2;		// �g���K�r���[
		}
		/******* Modify E 110 *********/
		else
		{
			InsCnt.A.Phase = Trig_View;	// �g���K�r���[
		}
		// �ċz�����肨���
		
		if(( InsCnt.A.Phase >= Ins_Strt )			// 0	�z�C�J�n
		 && ( InsCnt.A.Phase <= Ins_End ))		// 3	�z�C�I��
		{
			
		}
		else
		{
			PV_PIP1A = PV_PIP2B;
		}
		
/******* Modify S 111 *********/
		INSTIM_Counter++;			// 
		if (INSTIM_Counter >= Tim_TotalA + CarryA)
		{
			INSTIM_Counter = 0;			// 
			CarryA = 0;
		}
/******* Modify E 111 *********/
		
		PV_PEEPA = PV_PEEPB;		// �X���[�u�ւ̃R�}���h
		Tim_TotalA = Tim_TotalB;	// RAM[10,11] 
		
        if (Cmnd.A.Ins_OP == 0)				// �ċz�쓮���łȂ� Apnea when breath
		{
			INSTIM_Counter = Tim_TotalA - Tim_TrigA -1;			// 
			
			PV_PIP1A = PV_PIP1B;		// �X���[�u�ւ̃R�}���h
			PV_PIP2A = PV_PIP2B;		// �X���[�u�ւ̃R�}���h
			Tim_PIP2A = Tim_PIP2B;		// RAM[12,13] 
			Tim_TrigA = Tim_TrigB;		// RAM[16,17] 
			Trgt_PresA = Trgt_PresB;	// RAM[18,19] 
			Hi_PrsA = Hi_PrsB;			// RAM[20] �����A���[��
		}
		
		if((InsCnt.A.Phase == Trig_View)		// �g���K�r���[
		 && ( DEF_Flow >= Max_Ins_Flow))
		{
			CarryA = Tim_TotalA + CarryA - INSTIM_Counter;
/******* Modify S 111 *********/
			InsCnt.A.Phase = Ins_Strt;		// �z�C�J�n
/******* Modify E 111 *********/
			INSTIM_Counter = 0;			// 
		}
		/******* Modify S 110 *********/
		else if((InsCnt.A.Phase == Trig_View2)		// �g���K�r���[
		 && ( DEF_Flow > Max_Exh_Flow))
		{
			CarryA = Tim_TotalA + CarryA - INSTIM_Counter;
/******* Modify S 111 *********/
			InsCnt.A.Phase = Ins_Strt;		// �z�C�J�n
/******* Modify E 111 *********/
			INSTIM_Counter = 0;			// 
		}
		/******* Modify E 110 *********/
		
		SIMV_Counter = Tim_TotalA * 0.7 -1;		// �g���Ă��Ȃ�
		
		if(InsCnt.A.Phase == Trig_View)
//		if( INSTIM_Counter == 0)
		{
//			LED_2_On();
		}
		else
		{
//			LED_2_Off();
		}
		
	/******* Modify E 102 *********/
	/**************************************************************************/
	}
		/******* Modify E 111 *********/
	else
	{
		/******* Modify S 112 *********/
		DOUCHOU.A.SIMV = 0;
		SIMV = 0;
		/******* Modify E 112 *********/
		PV_PIP1A = 0;
		PV_PIP2A = 0;
		PV_PEEPA = 0;
		InsCnt.A.Phase = Exh;					// �ċC��
		SIMV_Counter = Tim_TotalA * 0.7 -1;
		Trig_mode = Trig_init1;
	}
	FLOW_PHASE.A.Phase = InsCnt.A.Phase;
	
		/******* Modify S 110 *********/
//	Status = Trig_mode;
		/******* Modify E 110 *********/
	
	// PV�̊e���[�h�ɂ�铮�삨���
	/**************************************************************************/
	
	Hi_Alarm_old = Hi_Alarm;
	PSoC1.DATA.ERR.BIT.Hi_Alm = Hi_Alarm;
	
	/******* Modify S 103 *********/
//	Flow1 = CarryA;
	/******* Modify E 103 *********/
}


/******************************************************************************
##
## Name    :PV_cnt
## Function:���ُo�͏���2
## Input   :�Ȃ�
## Output  :�Ȃ�
## Memo    :1.25msec����
##
******************************************************************************/
void PV_cnt2( void )
{
	unsigned short	PV;
	
	if( PV_PIP1A >= 511)						// DAC��9bit�̂���(�ő�l��510)
	{
		PV_PIP1A = 510;
	}
	
	if( PV_PIP2A >= 511)
	{
		PV_PIP2A = 510;
	}
	
	if( PV_PEEPA >= 511)
	{
		PV_PEEPA = 510;
	}
	
	
	
	// �z�C���⏞
	if(( InsCnt.A.Phase >= Ins_Strt )			// 0	�z�C�J�n
	 && ( InsCnt.A.Phase <= Ins_End ))			// 3	�z�C�I��
	{
		if(PSoC1.DATA.ERR.BYTE == 0)
		{
			PV = PV_PIP1A;
		}
		else
		{
			PV = PV_PIP2A;
		}
		
	}
	else
	{
		PV = PV_PEEPA;
	}
	
	
	
	// �\�t�g����
	if (PV_soft + 20 < PV)
	{
		PV_soft = PV_soft + 20;
	}
	else if (PV_soft > PV + 20 )
	{
		PV_soft = PV_soft - 20;
	}
	else
	{
		PV_soft = PV;
	}
	
	// Hi�A���[��������
	/******* Modify S 111 *********/
//	if(Hi_Alarm == 1)
	if((Hi_Alarm == 1)
	 && (InsCnt.A.Mode != 6))
	/******* Modify E 111 *********/
	{
		PV_soft = PV_PEEPA;
	}
	PV_PEEP_old = PV_soft;
	
	DAC9_1_WriteStall(PV_soft);
	
}


