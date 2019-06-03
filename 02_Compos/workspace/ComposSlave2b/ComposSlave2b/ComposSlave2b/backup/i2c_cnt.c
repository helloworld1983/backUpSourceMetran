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
*                                                                               
*******************************************************************************/

#include <m8c.h>			// part specific constants and macros
#include "I2C_CNT.h"		// 
#include "PSoCAPI.h"		// PSoC API definitions for all User Modules

/******************************************************************************
*** RAM・ROM
******************************************************************************/

unsigned char	Status;		// スレーブのステータス状況

unsigned char	Cmnd;		// スレーブへのコマンド
unsigned short	Volume;		// 1回換気
unsigned short	MaxPrs;		// 最大吸気圧
unsigned char	PEEP;		// ＰＥＥＰ
T_PSoC2 PSoC2;				// I2C構造体
T_Brink_Set Brink_Set;		// 点滅



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
//	EzI2Cs_1_EnableInt();                                     // Proxy Class 1
	EzI2Cs_1_SetRamBuffer(sizeof(PSoC2), 9, (BYTE *) &PSoC2);
	EzI2Cs_1_Start(); // Turn on I2C
	
	Status = 0;		// スレーブのステータス状況
	
	Cmnd = 0;		// スレーブへのコマンド
	Volume = 0;		// 1回換気 7seg(4)
	MaxPrs = 0;		// 最大吸気圧 7seg(3)
	PEEP = 0;		// ＰＥＥＰ 7seg(2)
	Brink_Set.BYTE = 0x00;
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
//	if(PSoC2.DATA.Count_W1 == PSoC2.DATA.Count_W2)
//	{
		Cmnd = PSoC2.DATA.Cmnd;		// スレーブへのコマンド
		Volume = PSoC2.DATA.Volume;		// 1回換気
		MaxPrs = PSoC2.DATA.MaxPrs;		// 最大吸気圧
		PEEP = PSoC2.DATA.PEEP;		// ＰＥＥＰ
		Brink_Set.BYTE = PSoC2.DATA.Brink_Set.BYTE;
//	}
	PSoC2.DATA.Status = Status;		// スレーブのステータス状況
}
