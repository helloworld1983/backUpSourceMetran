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
*            13.05.08  JETモードのときの表示を----         1.11                 
*                                                                               
*******************************************************************************/

#include <m8c.h>			// part specific constants and macros
#include "I2C_CNT.h"		// 
#include "PSoCAPI.h"		// PSoC API definitions for all User Modules

/******************************************************************************
*** RAM・ROM
******************************************************************************/

unsigned char	Status;							// スレーブのステータス状況

unsigned char	Cmnd;							// スレーブへのコマンド
unsigned short	Volume;							// 1回換気
unsigned short	MaxPrs;							// 最大吸気圧
unsigned char	PEEP;							// ＰＥＥＰ
T_PSoC2 PSoC2;									// I2C構造体
T_Brink_Set Brink_Set;							// 点滅
T_Disp_Mode Disp_Mode;							// 

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
## 30/09/15 change the version number to 113
##
******************************************************************************/
void I2C_CNT_init( void )
{
	EzI2Cs_1_SetRamBuffer(sizeof(PSoC2), 10, (BYTE *) &PSoC2);
	EzI2Cs_1_Start();
	
	I2C_wach_cunt = 255;						// I2C読み込み監視カウンタ
	Count_W1_old = 255;							// 前回の読み込みカウンタ
	
	Status = 0;									// スレーブのステータス状況
	
	Cmnd = 0;									// スレーブへのコマンド
	Volume = 8888;								// 1回換気 7seg(4)
	MaxPrs = 888;								// 最大吸気圧 7seg(3)
	PEEP = 88;									// ＰＥＥＰ 7seg(2)
	Brink_Set.BYTE = 0x00;
	Disp_Mode.BYTE = 0x00;
	PSoC2.DATA.Volume = 8888;					// 1回換気 7seg(4)
	PSoC2.DATA.MaxPrs = 888;					// 最大吸気圧 7seg(3)
	PSoC2.DATA.PEEP = 88;						// ＰＥＥＰ 7seg(2)
	
	PSoC2.DATA.VER = 113;						// バージョン
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
	if(PSoC2.DATA.Count_W1 == PSoC2.DATA.Count_W2)
	{
		Cmnd = PSoC2.DATA.Cmnd;					// スレーブへのコマンド
		Volume = PSoC2.DATA.Volume;				// 1回換気
		MaxPrs = PSoC2.DATA.MaxPrs;				// 最大吸気圧
		PEEP = PSoC2.DATA.PEEP;					// ＰＥＥＰ
		Brink_Set.BYTE = PSoC2.DATA.Brink_Set.BYTE;
		Disp_Mode.BYTE = PSoC2.DATA.Disp_Mode.BYTE;
	}
	PSoC2.DATA.Status = Status;					// スレーブのステータス状況
	PSoC2.DATA.Reserv_1 = 0xaa;
	
	if(Count_W1_old == PSoC2.DATA.Count_W1)
	{
		if(I2C_wach_cunt != 255)
		{
			I2C_wach_cunt++;
			if(I2C_wach_cunt > 20)
			{
				I2C_wach_cunt = 20;
				PSoC2.DATA.Disp_Mode.BIT.ER_I2C = 1;
			}
		}
	}
	else
	{
		I2C_wach_cunt = 0;
	}
	
	if((PSoC2.DATA.Count_W1 != 0) || (I2C_wach_cunt != 255))
	{
		Count_W1_old = PSoC2.DATA.Count_W1;		// 前回の読み込みカウンタ
	}

}
