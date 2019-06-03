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
unsigned char	Dark_Mode;	// 暗表示
unsigned char	MaxPrs_Set;	// 最大気道内圧
unsigned char	PEEP_Set;	// PEEP
unsigned short	InsCnt_Set;	// 呼吸回数
unsigned char	InsTim_Set;	// 吸気時間
unsigned char	Apnea_Set;	// Apnea
T_PSoC3 PSoC3;				// I2C構造体
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
## 30/09/15 Change version number from 100 to 113
******************************************************************************/
void I2C_CNT_init( void )
{
	EzI2Cs_1_SetRamBuffer(sizeof(PSoC3), 9, (BYTE *) &PSoC3);
	EzI2Cs_1_Start(); 
	
	Status = 0;									// スレーブのステータス状況
	
	Cmnd = 0;									// スレーブへのコマンド
	Dark_Mode = 0;								// 暗表示
	MaxPrs_Set = 88;							// 最大気道内圧
	PEEP_Set = 88;								// PEEP
	InsCnt_Set = 888;							// 呼吸回数
	InsTim_Set = 88;							// 吸気時間
	Apnea_Set = 88;								// Apnea
	Brink_Set.BYTE = 0x00;
	
	PSoC3.DATA.Cmnd = 0;						// スレーブへのコマンド
	PSoC3.DATA.Dark_Mode = 0;					// 暗表示
	PSoC3.DATA.MaxPrs_Set = 88;					// 最大気道内圧
	PSoC3.DATA.PEEP_Set = 88;					// PEEP
	PSoC3.DATA.InsCnt_Set = 888;				// 呼吸回数
	PSoC3.DATA.InsTim_Set = 88;					// 吸気時間
	PSoC3.DATA.Apnea_Set = 88;					// Apnea
	PSoC3.DATA.Brink_Set.BYTE = 0x00;
	
	PSoC3.DATA.VER = 113;						// バージョン
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
	Cmnd = PSoC3.DATA.Cmnd;						// スレーブへのコマンド
	Dark_Mode = PSoC3.DATA.Dark_Mode;			// 暗表示
	MaxPrs_Set = PSoC3.DATA.MaxPrs_Set;			// 最大気道内圧
	PEEP_Set = PSoC3.DATA.PEEP_Set;				// PEEP
	InsCnt_Set = PSoC3.DATA.InsCnt_Set;			// 呼吸回数
	InsTim_Set = PSoC3.DATA.InsTim_Set;			// 吸気時間
	Apnea_Set = PSoC3.DATA.Apnea_Set;			// Apnea
	
	Brink_Set.BYTE = PSoC3.DATA.Brink_Set.BYTE;
	PSoC3.DATA.Status = Status;					// スレーブのステータス状況
	
}
