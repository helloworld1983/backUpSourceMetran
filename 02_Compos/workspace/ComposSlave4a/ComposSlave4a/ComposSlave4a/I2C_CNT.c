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
unsigned char	Bar;		// RAM[2] バーグラフ
unsigned char	AlarmLED;	// RAM[3] アラームLED
unsigned char	MEAN;		// RAM[4] バーグラフ
unsigned char	HI;			// RAM[6] バーグラフ
unsigned char	LO;			// RAM[7] バーグラフ
unsigned char	Brink;		// RAM[7] バーグラフ
T_PSoC4 PSoC4;				// I2C構造体

unsigned char	I2C_wach_cunt;		// I2C読み込み監視カウンタ
unsigned char	Count_W1_old;		// 前回の読み込みカウンタ


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
## 30/09/15 change the version number from 100 to 113
******************************************************************************/
void I2C_CNT_init( void )
{
	EzI2Cs_1_EnableInt();
	EzI2Cs_1_SetRamBuffer(sizeof(PSoC4), 8, (BYTE *) &PSoC4);
	EzI2Cs_1_Start(); 							// Turn on I2C
	
	I2C_wach_cunt = 255;						// I2C読み込み監視カウンタ
	Count_W1_old = 255;							// 前回の読み込みカウンタ
	
	Status = 0;									// スレーブのステータス状況
	
	Cmnd = 0;									// スレーブへのコマンド
	Bar = 40;									// RAM[2] バーグラフ
	AlarmLED = 0xff;							// RAM[3] アラームLED
	MEAN = 0;									// RAM[4] バーグラフ
	HI = 0;										// RAM[6] バーグラフ
	LO = 0;										// RAM[7] バーグラフ
	Brink = 0;
	PSoC4.DATA.Bar = 40;						// RAM[2] バーグラフ
	PSoC4.DATA.AlarmLED.BYTE = 0xff;			// RAM[3] アラームLED
	
	PSoC4.DATA.VER = 113;						// バージョン
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
	Cmnd = PSoC4.DATA.Cmnd;						// スレーブへのコマンド
	Bar = PSoC4.DATA.Bar;						// RAM[2] バーグラフ
	AlarmLED = PSoC4.DATA.AlarmLED.BYTE;		// RAM[3] アラームLED
	MEAN = PSoC4.DATA.MEAN;						// RAM[4] バーグラフ
	HI = PSoC4.DATA.HI;							// RAM[6] バーグラフ
	LO = PSoC4.DATA.LO;							// RAM[7] バーグラフ
	
	PSoC4.DATA.Count_R1 = PSoC4.DATA.Count_W1;
	PSoC4.DATA.Count_R2 = PSoC4.DATA.Count_W1;
	PSoC4.DATA.Status = Status;					// スレーブのステータス状況
	
	if(Count_W1_old == PSoC4.DATA.Count_W1)
	{
		if(I2C_wach_cunt != 255)
		{
			I2C_wach_cunt++;
			if(I2C_wach_cunt > 20)
			{
				I2C_wach_cunt = 20;
				AlarmLED = PSoC4.DATA.AlarmLED.BYTE | 0xF0;	// RAM[3] アラームLED
			}
		}
	}
	else
	{
		I2C_wach_cunt = 0;
	}
	
	if((PSoC4.DATA.Count_W1 != 0) || (I2C_wach_cunt != 255))
	{
		Count_W1_old = PSoC4.DATA.Count_W1;		// 前回の読み込みカウンタ
	}
	
	if(Cmnd == 1)
	{
		Brink = 1;
	}
	else
	{
		Brink = 0;
	}
}

