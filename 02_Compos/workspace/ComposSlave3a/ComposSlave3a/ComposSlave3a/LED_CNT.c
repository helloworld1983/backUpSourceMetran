/*******************************************************************************
*                                                                               
*    ﾌｧｲﾙ名 ：   LED_CNT.c                                                      
*                                                                               
*    概要 ：                                                                    
*          ７セグダイナミック点灯（11桁）のプログラム                           
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
#include "LED_CNT.h"		// 
#include "PSoCAPI.h"		// PSoC API definitions for all User Modules
#include "I2C_CNT.h"		// 

/******************************************************************************
*** RAM・ROM
******************************************************************************/

unsigned char count;					// ダイナミック桁指定カウンタ
unsigned char brink_count;				// 点滅用
unsigned char brink_flag;				// 点滅有効フラグ
unsigned char bit;						// ダイナミック桁指定ビット
unsigned char dark_flag;				// 暗点灯有効フラグ
unsigned char disp_data[ MAX_KETA ];	// ７セグ表示データ
T_DOT Dot;								// デシマルドットフラグ
unsigned char Brink_Set_old;

unsigned char count_10ms;

const unsigned char OUT_SEG[] = 		//7セグデコードデータ（正論理）
{
	0x3F,		// [ 0] = '0' 表示
	0x06,		// [ 1] = '1' 表示
	0x5B,		// [ 2] = '2' 表示
	0x4F,		// [ 3] = '3' 表示
	0x66,		// [ 4] = '4' 表示
	0x6D,		// [ 5] = '5' 表示
	0x7D,		// [ 6] = '6' 表示
	0x07,		// [ 7] = '7' 表示
	0x7F,		// [ 8] = '8' 表示
	0x6F,		// [ 9] = '9' 表示
	0x77,		// [10] = 'A' 表示
	0x7C,		// [11] = 'b' 表示
	0x39,		// [12] = 'C' 表示
	0x5E,		// [13] = 'd' 表示
	0x79,		// [14] = 'E' 表示
	0x71,		// [15] = 'F' 表示
	0x40,		// [16] = '-' 表示
	0x00,		// [17] = 非表示
};


/******************************************************************************
*** プログラムコード
******************************************************************************/


/******************************************************************************
##
## Name    :LED_CNT_init
## Function:７セグダイナミック点灯初期化
## Input   :なし
## Output  :なし
## Memo    :なし
##
******************************************************************************/
void LED_CNT_init( void )
{
	int i;
	count = 0;
	brink_count = 0;
	brink_flag = 1;
	dark_flag = 1;
	bit = 1;
	for( i = 0; i < 9; i++)
	{
		disp_data[i] = 0;
	}
	Dot.WORD = 0;
	Dot.BIT.D07 = 1;	// 吸気時間
	
	count_10ms = 0;
	Brink_Set_old = 0;
	
/*	LED_CNT_set( 0, 8);			// 8で点灯 17で消灯
	LED_CNT_set( 1, 8);
	LED_CNT_set( 2, 8);
	LED_CNT_set( 3, 8);
	LED_CNT_set( 4, 8);
	LED_CNT_set( 5, 8);
	LED_CNT_set( 6, 8);
	LED_CNT_set( 7, 8);
	LED_CNT_set( 8, 8);
	LED_CNT_set( 9, 8);
	LED_CNT_set( 10, 8);
	*/
	LED_CNT_set( 0, 3);			// 8で点灯 17で消灯		// mod101
	LED_CNT_set( 1, 3);
	LED_CNT_set( 2, 3);
	LED_CNT_set( 3, 3);
	LED_CNT_set( 4, 3);
	LED_CNT_set( 5, 3);
	LED_CNT_set( 6, 3);
	LED_CNT_set( 7, 3);
	LED_CNT_set( 8, 3);
	LED_CNT_set( 9, 3);
	LED_CNT_set( 10, 3);

	LED_8_Start();
	LED_9_Start();
	LED_10_Start();

}


/******************************************************************************
##
## Name    :LED_CNT_update
## Function:７セグダイナミック点灯I2Cデータ更新（100msec）
## Input   :unsigned char count_10ms 時分割用
## Output  :なし
## Memo    :なし
##
******************************************************************************/
void LED_CNT_update( unsigned char count_10ms)
{
	int temp;
	
	switch (count_10ms)
	{
	case (1):
		if(brink_flag && Brink_Set.BIT.MaxPrs)
		{
			LED_CNT_set( 1, 17);
			LED_CNT_set( 0, 17);
		}
		else
		{
			// 最大気道内圧 7seg(2)
			temp = MaxPrs_Set;					// 最大気道内圧
			LED_CNT_set( 1, (temp % 10 ));
			temp = temp /10;
			LED_CNT_set( 0, (temp % 10 ));
		}
		break;
	
	case (2):
		if(brink_flag && Brink_Set.BIT.PEEP)
		{
			LED_CNT_set( 3, 17);
			LED_CNT_set( 2, 17);
		}
		else
		{
			// PEEP 7seg(2)
			temp = PEEP_Set;		// ＰＥＥＰ
			LED_CNT_set( 3, (temp % 10 ));
			temp = temp /10;
			LED_CNT_set( 2, (temp % 10 ));
		}
		break;
	
	case (3):
		if(brink_flag && Brink_Set.BIT.InsCnt)
		{
			LED_CNT_set( 6, 17);
			LED_CNT_set( 5, 17);
			LED_CNT_set( 4, 17);
		}
		else
		{
			// 呼吸回数 7seg(3)
			temp = InsCnt_Set;					// 呼吸回数
			LED_CNT_set( 6, (temp % 10 ));
			temp = temp /10;
			LED_CNT_set( 5, (temp % 10 ));
			temp = temp /10;
			LED_CNT_set( 4, (temp % 10 ));
		}
		break;
	
	case (4):
		if(brink_flag && Brink_Set.BIT.InsTim)
		{
			LED_CNT_set( 8, 17);
			LED_CNT_set( 7, 17);
		}
		else
		{
			// 吸気時間 7seg(2)
			temp = InsTim_Set;					// 吸気時間
			LED_CNT_set( 8, (temp % 10 ));
			temp = temp /10;
			LED_CNT_set( 7, (temp % 10 ));
		}
		break;
	
	case (5):
		if(brink_flag && Brink_Set.BIT.Apnea)
		{
			LED_CNT_set( 10, 17);
			LED_CNT_set( 9, 17);
		}
		else
		{
			// Apnea 7seg(2)
			temp = Apnea_Set;					// Apnea
			LED_CNT_set( 10, (temp % 10 ));
			temp = temp /10;
			LED_CNT_set( 9, (temp % 10 ));
		}
		break;
	
	case (6):
		break;
	
	case (7):
		break;
	
	case (8):
		break;
	
	case (9):
		brink_count++;
		if( Brink_Set_old != (Brink_Set.BYTE & 0x1f))
		{
			brink_count = 0;
		}
		Brink_Set_old = (Brink_Set.BYTE & 0x1f); 
		if (Brink_Set.BIT.Fast == 0)			// SLOW_BRINK
		{
			if(brink_count >= 12)				// 1.2秒
			{
				brink_count = 0;
			}
		
			if(brink_count < 4)					// 0.4秒
				brink_flag = 1;					// 消灯
			else
				brink_flag = 0;					// 点灯
		}
		else if (Brink_Set.BIT.Fast == 1)		// MID_BRINK
		{
			if(brink_count >= 9)				// 0.9秒
			{
				brink_count = 0;
			}
			if(brink_count < 3)					// 0.3秒
				brink_flag = 1;					// 消灯
			else
				brink_flag = 0;					// 点灯
		}
		else if (Brink_Set.BIT.Fast == 2)		// FAST_BRINK
		{
			if(brink_count >= 6)				// 0.6秒
			{
				brink_count = 0;
			}
			if(brink_count < 2)					// 0.2秒
				brink_flag = 1;					// 消灯
			else
				brink_flag = 0;					// 点灯
		}
		else if (Brink_Set.BIT.Fast == 3)
		{
			if(brink_count >= 3)				// 0.3秒
			{
				brink_count = 0;
			}
			if(brink_count < 2)					// 0.2秒
				brink_flag = 1;					// 消灯
			else
				brink_flag = 0;					// 点灯
		}
		break;
	
	default	:
		break;
	}
}


/******************************************************************************
##
## Name    :LED_CNT_set
## Function:７セグダイナミック点灯I2Cデータ更新
## Input   :桁指定
##			表示データ
## Output  :なし
## Memo    :なし
##
******************************************************************************/
void LED_CNT_set( unsigned char keta, unsigned char value)
{
	if( keta < MAX_KETA)
	{
		if( value > 17)
		{
			value = 17;
		}
		disp_data[keta] = OUT_SEG[value];
		
	}
}


/******************************************************************************
##
## Name    :LED_CNT
## Function:７セグダイナミック点灯（0.5msec）
## Input   :なし
## Output  :なし
## Memo    :なし
##
******************************************************************************/
void LED_CNT( void )
{
	int temp;							// mod101
	if(count < 8)
	{
		LED_8_Off(); 
		LED_9_Off(); 
		LED_10_Off(); 
		PRT2DR = 0x00;
		if((dark_flag != 0)						// 暗点灯フラグ
		 && (Dark_Mode == 2)
		 && ((Brink_Set.BYTE & 0x1f) == 0))
		{
			PRT0DR = 0xFF;						// 非表示 負論理
		}
		else
		{
			if( (Dot.WORD << (15-count)) && 0x8000)		// ドットの制御
			{
				PRT0DR = ~(disp_data[count] | 0x80);	// 負論理のためチルダが付いている
			}
			else
			{
				PRT0DR = ~(disp_data[count] | 0x00);	// 負論理のためチルダが付いている
			}
		}
		PRT2DR = bit;
	}
	else if( count == 8)
	{
		PRT2DR = 0x00;
		if((dark_flag != 0)						// 暗点灯フラグ
		 && (Dark_Mode == 2)
		 && ((Brink_Set.BYTE & 0x1f) == 0))
		{
			PRT0DR = 0xFF;						// 非表示 負論理
		}
		else
		{
			if( Dot.BIT.D08 == 1)				// ドットの制御
			{
				PRT0DR = ~(disp_data[count] | 0x80);	// 負論理のためチルダが付いている
			}
			else
			{
				PRT0DR = ~(disp_data[count] | 0x00);	// 負論理のためチルダが付いている
			}
		}
		LED_8_On();
	}
	else if( count == 9)
	{
		PRT2DR = 0x00;
		LED_8_Off(); 
		if((dark_flag != 0)						// 暗点灯フラグ
		 && ((Dark_Mode == 2) || (Dark_Mode == 4))
		 && ((Brink_Set.BYTE & 0x1f) == 0))
		{
			PRT0DR = 0xFF;						// 非表示 負論理
		}
		else
		{
			if( Dot.BIT.D09 == 1)				// ドットの制御
			{
				PRT0DR = ~(disp_data[count] | 0x80);	// 負論理のためチルダが付いている
			}
			else
			{
				PRT0DR = ~(disp_data[count] | 0x00);	// 負論理のためチルダが付いている
			}
		}
		LED_9_On();
	}
	else if( count == 10)
	{
		PRT2DR = 0x00;
		LED_9_Off(); 
		if((dark_flag != 0)						// 暗点灯フラグ
		 && ((Dark_Mode == 2) || (Dark_Mode == 4))
		 && ((Brink_Set.BYTE & 0x1f) == 0))
		{
			PRT0DR = 0xFF;						// 非表示 負論理
		}
		else
		{
			if( Dot.BIT.D10 == 1)				// ドットの制御
			{
				PRT0DR = ~(disp_data[count] | 0x80);	// 負論理のためチルダが付いている
			}
			else
			{
				PRT0DR = ~(disp_data[count] | 0x00);	// 負論理のためチルダが付いている
			}
		}
		LED_10_On();
	}
	else if( count == 11)
	if (1)
	{
		LED_10_Off(); 
		count_10ms++;
		
		LED_CNT_update(count_10ms);
		if(count_10ms >= 10)		// 1秒
		{
			count_10ms = 0;
		}
	}
	count++;
	bit = bit * 2;
//	if(count >= MAX_KETA+1)						// mod101
			temp = MaxPrs_Set;					// mod101
				temp = temp % 10;				// mod101
//				if(temp < 2)					// mod101
//				{								// mod101
//					temp = 2;					// mod101
//				}								// mod101
	if(count > MAX_KETA+temp )							// mod101
	{
		count = 0;
		bit = 1;
		if(dark_flag >= 3)						// 暗点灯フラグ
		{
			dark_flag = 0;
		}
		else
		{
			dark_flag ++;
		}
	}
}
