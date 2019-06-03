/*******************************************************************************
*                                                                               
*    ﾌｧｲﾙ名 ：   I2C_CNT.c                                                      
*                                                                               
*    概要 ：                                                                    
*          I2Cデータ処理のプログラム                                            
*                                                                               
*    公開API :                                                                  
*                                                                               
*    備考 :                                                                     
*                                                                               
*                                                                               
*    変更履歴 :                                                                 
*            日  付    内    容                            Ver No.              
*            --------  ----------------------------------- -------              
*            10.01.20  初版作成                            0.00                 
*            11.12.08  1版作成                             1.00                 
*            11.12.19  SIMVバグ                            1.01                 
*            11.12.20  SIMV仕様変更                        1.02                 
*            12.01.06  フローデータ変更
*                      生データに方向を持たせる。          1.03                
*            12.02.10  アプニア対策                        1.06                 
*            12.03.19  アプニア対策吸気トリガ戻す          1.07                 
*            12.05.22  低換気量でベースフロー更新しない    1.08                 
*            13.01.09  PT標準化バグ修正                    1.09                 
*			 13.02.07	自発呼吸検知追加                   1.10                 
*						E002_timeOut追加                             
*						回路外れ用キンク差圧の値変更                  
*						トリガウィンドウ二分割アクティブ・パッシブ追加
*						呼吸回数121回以上で一回換気量表示を"----"追加
*						E002_timeOut追加                          
*						フローレートがマイナス表示のときのバグ修正
*            13.05.08  フロートリガ検知の最初を沈静化      1.11                 
*                                                                               
*******************************************************************************/

#include <m8c.h>			// part specific constants and macros
#include "I2C_CNT.h"		// 
#include "PSoCAPI.h"		// PSoC API definitions for all User Modules
#include "Flow.h"		// 
#include "PT.h"			// 

/******************************************************************************
*** RAM・ROM
******************************************************************************/

T_Cmnd			Cmnd;							// RAM[1] スレーブへのコマンド
T_InsCnt		InsCnt;							// RAM[2] 呼吸相
unsigned char	BZ;								// RAM[3] BZ

unsigned short	PV_PIP1A;						// RAM[10,11] 
unsigned short	PV_PIP2A;						// RAM[14,15] 
unsigned short	PV_PEEPA;						// RAM[18,19] 
unsigned short	Tim_TotalA;						// RAM[10,11] 
unsigned short	Tim_PIP2A;						// RAM[12,13] 
unsigned short	Tim_PIP1A;						// RAM[14,15] 
unsigned short	Tim_TrigA;						// RAM[16,17] 
unsigned short	Trgt_PresA;						// RAM[18,19] 
unsigned char	Hi_PrsA;							// RAM[20] 高圧アラーム
unsigned char	KINK_DIFA;						// RAM[21] キンク差圧

unsigned short	PV_PIP1B;						// RAM[10,11] 
unsigned short	PV_PIP2B;						// RAM[14,15] 
unsigned short	PV_PEEPB;						// RAM[18,19] 
unsigned short	Tim_TotalB;						// RAM[10,11] 
unsigned short	Tim_PIP2B;						// RAM[12,13] 
unsigned short	Tim_PIP1B;						// RAM[14,15] 
unsigned short	Tim_TrigB;						// RAM[16,17] 
unsigned short	Trgt_PresB;						// RAM[18,19] 
unsigned char	Hi_PrsB;							// RAM[20] 高圧アラーム
unsigned char	KINK_DIFB;						// RAM[21] キンク差圧

unsigned char	Status;							// RAM[33] スレーブのステータス状況
signed short	Exh_Vol;						// RAM[34,35] 
signed short	Flow1;							// RAM[36,37] フロー１
signed short	Flow_RAW;						// 
signed short	PT_RAW;							// 
signed short	PTm;							// RAM[38,39] 口元圧(平均)
signed short	PTh;							// RAM[40,41] 口元圧(ピーク)
unsigned short	Enc;							// RAM[42,43] エンコーダ
T_FLOW_PHASE 	FLOW_PHASE;						// 吸気呼気エラー
/******* Modify S 112 *********/
T_DOUCHOU		DOUCHOU;
/******* Modify E 112 *********/

unsigned char	INS_Trig;
signed char		VOL_RATE;				// RAM[52] 	// I
unsigned short	VER;							// RAM[43,44] 

T_PSoC1 		PSoC1;							// I2C構造体


unsigned short	PV_soft;						// 

unsigned char	I2C_wach_cunt;					// I2C読み込み監視カウンタ
unsigned char	Count_W1_old;					// 前回の読み込みカウンタ


/******************************************************************************
*** プログラムコード
******************************************************************************/


/******************************************************************************
##
## Name    :I2C_CNT_init
## Function:I2Cデータ処理初期化
## Input   :なし
## Output  :なし
## Memo    :なし
##
******************************************************************************/
void I2C_CNT_init( void )
{
    PSoC1.DATA.VER = 122;						// バージョン
	
	EzI2Cs_1_EnableInt();
	EzI2Cs_1_SetRamBuffer(sizeof(PSoC1), sizeof(PSoC1), (BYTE *) &PSoC1);
//	EzI2Cs_1_SetRamBuffer(sizeof(PSoC1), 54, (BYTE *) &PSoC1);
	EzI2Cs_1_Start();
	
	I2C_wach_cunt = 255;						// I2C読み込み監視カウンタ
	Count_W1_old = 255;							// 前回の読み込みカウンタ
	
	PSoC1.DATA.Cmnd.BYTE = 0;					// RAM[1] スレーブへのコマンド
	PSoC1.DATA.InsCnt.BYTE = 0;					// RAM[2] 呼吸相
	PSoC1.DATA.BZ = 1;							// RAM[3] BZ

	PSoC1.DATA.F_00_CAL = 0;					// RAM[6,7] F_00_CAL
	PSoC1.DATA.PV_PIP1 = 0;						// RAM[10,11] 
	PSoC1.DATA.PV_PIP2 = 0;						// RAM[14,15] 
	PSoC1.DATA.PV_PEEP = 0;						// RAM[18,19] 
	PSoC1.DATA.Tim_Total = 0;					// RAM[10,11] 
	PSoC1.DATA.Tim_PIP2 = 0;					// RAM[12,13] 
	PSoC1.DATA.Tim_PIP1 = 0;					// RAM[14,15] 
	PSoC1.DATA.Tim_Trig = 0;					// RAM[16,17] 
	PSoC1.DATA.Trgt_Pres = 0;					// RAM[18,19] 
	PSoC1.DATA.Hi_Prs = 0;
	PSoC1.DATA.KINK_DIF = 0;

	PSoC1.DATA.Status = 0;						// RAM[33] スレーブのステータス状況
	PSoC1.DATA.Exh_Vol = 0;						// RAM[34,35] 
	PSoC1.DATA.Flow1 = 0;						// RAM[36,37] フロー１
	PSoC1.DATA.PTm = 0;							// RAM[38,39] 口元圧(平均)
	PSoC1.DATA.PTh = 0;							// RAM[40,41] 口元圧(ピーク)
	PSoC1.DATA.Enc = 0;							// RAM[42,43] エンコーダ
	PSoC1.DATA.FLOW_PHASE.BYTE = 0;				// RAM[42] 
	PSoC1.DATA.INS_Trig = 0;					// RAM[51] 	
	PSoC1.DATA.VOL_RATE = 0;					// RAM[52] 	
	
	Cmnd.BYTE = 0;								// RAM[1] スレーブへのコマンド
	InsCnt.BYTE = 0;							// RAM[2] 呼吸相
	BZ = 1;										// RAM[3] BZ
	Flow_RAW = 0;								// 
	PT_RAW = 0;									// 
	INS_Trig = 0;								// RAM[51] 	
	VOL_RATE = 0;								// RAM[52] 	

	F_00_CAL = 492;
	F_03_CAL = 549;
	F_06_CAL = 609;
	F_09_CAL = 664;
	F_12_CAL = 731;
	F_15_CAL = 778;
	F_20_CAL = 847;
	F_30_CAL = 1017;
	F_40_CAL = 1198;
	F_50_CAL = 1378;
	PV_PIP1B = 0;
	PV_PIP2B = 0;
	PV_PEEPB = 0;
	Tim_TotalB = 0;
	Tim_PIP2B = 0;
	Tim_PIP1B = 0;
	Tim_TrigB = 0;
	Trgt_PresB = 0;
	KINK_DIFB = 0;

	Status = 0;
	Exh_Vol = 0;
	Flow1 = 0;
	PTm = 0;
	PTh = 0;
	Enc = 0;
	FLOW_PHASE.BYTE = 0;
	Status = 0;	
	
	PV_soft = 0;	// ＰＶ
/******* Modify S 111 *********/
	PSoC1.DATA.BZ_VOL = 99;	// D
//	PSoC1.DATA.Reserv_3 = 1;	// D
/******* Modify E 111 *********/
	/******* Modify S 112 *********/
//	PSoC1.DATA.Reserv_4 = 0;	// 
	PSoC1.DATA.DOUCHOU.BYTE = 0;				// RAM[54] 	
	DOUCHOU.BYTE = 0;
	/******* Modify E 112 *********/
	/******* Modify S 112 *********/
	/******* Modify E 112 *********/
}



/******************************************************************************
##
## Name    :I2C_CNT
## Function:I2Cデータ処理
## Input   :なし
## Output  :なし
## Memo    :なし
##
******************************************************************************/
void I2C_CNT( void )
{
	unsigned char i;
	
	if(PSoC1.DATA.Count_W1 == PSoC1.DATA.Count_W2)
	{
		Cmnd.BYTE = PSoC1.DATA.Cmnd.BYTE;		// スレーブへのコマンド
		InsCnt.A.Mode = PSoC1.DATA.InsCnt.A.Mode;	// 呼吸相
		BZ = PSoC1.DATA.BZ;						// BZ
		PV_PIP1B = PSoC1.DATA.PV_PIP1;			// スレーブへのコマンド
		PV_PIP2B = PSoC1.DATA.PV_PIP2;			// スレーブへのコマンド
		PV_PEEPB = PSoC1.DATA.PV_PEEP;			// スレーブへのコマンド
		Tim_TotalB = PSoC1.DATA.Tim_Total;		// RAM[10,11] 
		Tim_PIP2B = PSoC1.DATA.Tim_PIP2;		// RAM[12,13] 
		Tim_PIP1B = PSoC1.DATA.Tim_PIP1;		// RAM[14,15] 
		Tim_TrigB = PSoC1.DATA.Tim_Trig;		// RAM[16,17] 
		Trgt_PresB = PSoC1.DATA.Trgt_Pres;		// RAM[18,19] 
		Hi_PrsB = 	PSoC1.DATA.Hi_Prs;			// RAM[20] 高圧アラーム
		KINK_DIFB = PSoC1.DATA.KINK_DIF;
	
	}
	if(PSoC1.DATA.Count_A1 == PSoC1.DATA.Count_A2)
	{
		P_00_CAL = PSoC1.DATA.P_00_CAL;
		P_30_CAL = PSoC1.DATA.P_30_CAL;
		P_60_CAL = PSoC1.DATA.P_60_CAL;
		F_00_CAL = PSoC1.DATA.F_00_CAL;
		F_03_CAL = PSoC1.DATA.F_03_CAL;
		F_06_CAL = PSoC1.DATA.F_06_CAL;
		F_09_CAL = PSoC1.DATA.F_09_CAL;
		F_12_CAL = PSoC1.DATA.F_12_CAL;
		F_15_CAL = PSoC1.DATA.F_15_CAL;
		F_20_CAL = PSoC1.DATA.F_20_CAL;
		F_30_CAL = PSoC1.DATA.F_30_CAL;
		F_40_CAL = PSoC1.DATA.F_40_CAL;
		F_50_CAL = PSoC1.DATA.F_50_CAL;
	}
	
	INS_Trig = PSoC1.DATA.INS_Trig;					// RAM[51] 	
	VOL_RATE = PSoC1.DATA.VOL_RATE;					// RAM[51] 	
	
	PSoC1.DATA.Count_R2 = PSoC1.DATA.Count_W1;
	PSoC1.DATA.Status = Status;					// スレーブのステータス状況
	PSoC1.DATA.Flow1 = Flow1;
	PSoC1.DATA.PTm = PTm;
	PSoC1.DATA.PTh = PTh / 10;
	PSoC1.DATA.Enc = Enc;
	PSoC1.DATA.Exh_Vol = Exh_Vol;
	PSoC1.DATA.Enc = Enc;
	PSoC1.DATA.FLOW_PHASE.BYTE = FLOW_PHASE.BYTE;
	PSoC1.DATA.Count_R1 = PSoC1.DATA.Count_W1;
	/******* Modify S 112 *********/
	PSoC1.DATA.DOUCHOU.BYTE = DOUCHOU.BYTE;				// RAM[54] 	
	/******* Modify E 112 *********/
	
	if(Cmnd.A.PT_Flow == 1)
	{
		PSoC1.DATA.RAW_DATA = PT_RAW;			// 
	}
	else
	{
		PSoC1.DATA.RAW_DATA = Flow_RAW;			// 
	}
	
	if(Count_W1_old == PSoC1.DATA.Count_W1)
	{
		if(I2C_wach_cunt != 255)
		{
			I2C_wach_cunt++;
			if(I2C_wach_cunt > 160)
			{
				I2C_wach_cunt = 160;
				BZ = 1;
			}
		}
	}
	else
	{
		I2C_wach_cunt = 0;
	}
	if((PSoC1.DATA.Count_W1 != 0) || (I2C_wach_cunt != 255))
	{
		Count_W1_old = PSoC1.DATA.Count_W1;		// 前回の読み込みカウンタ
	}
}
