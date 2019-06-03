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
#include "slave_switch.h"	// temp

/******************************************************************************
*** RAM・ROM
******************************************************************************/

unsigned char count;					// ダイナミック桁指定カウンタ
unsigned char brink_count;				// 点滅用
unsigned char brink_flag;				// 点滅有効フラグ
unsigned char bit;						// ダイナミック桁指定ビット
unsigned char disp_data[ MAX_KETA ];	// ７セグ表示データ

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

const unsigned char OUT_PTN[] = 
{

	0x00,       // 0
	0x01,       // 1
	0x03,       // 2
	0x07,       // 3
	0x0F,       // 4
	0x1F,       // 5
	0x3F,       // 6
	0x7F,       // 7
	0xFF        // 8
};

const unsigned char OUT_BIT[] = 
{
	0x00,       // 0
	0x01,       // 1
	0x02,       // 2
	0x04,       // 3
	0x08,       // 4
	0x10,       // 5
	0x20,       // 6
	0x40,       // 7
	0x80        // 8
};

/******************************************************************************
*** プログラムコード
******************************************************************************/


/******************************************************************************
##
## Name    :LED_CNT_init
## Function:ダイナミック点灯初期化
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
	bit = 1;
	for( i = 0; i < 9; i++)
	{
		disp_data[i] = 0;
	}
	
	count_10ms = 0;
	
	disp_data[0] = OUT_PTN[8];
	disp_data[1] = OUT_PTN[8];
	disp_data[2] = OUT_PTN[8];
	disp_data[3] = OUT_PTN[8];
	disp_data[4] = OUT_PTN[8];
	disp_data[5] = OUT_PTN[8];
	disp_data[6] = OUT_PTN[8];
	disp_data[7] = OUT_PTN[8];
	
	LED_10_Start();
	LED_11_Start();
	LED_12_Start();
	LED_13_Start();
	LED_14_Start();
	LED_15_Start();


}




/******************************************************************************
##
## Name    :LED_CNT_update
## Function:LED点灯I2Cデータ更新（100msec）
## Input   :unsigned char count_10ms 時分割用
## Output  :なし
## Memo    :なし
##
******************************************************************************/
void LED_CNT_update( unsigned char count_10ms)
{
	switch (count_10ms)
	{
	case (1):
		if(Bar > MAX_Bar)
			Bar = MAX_Bar;
		LED_CNT_BAR_set(Bar);					// バーグラフ
		break;
	
	case (2):
		disp_data[5] = AlarmLED;				// アラームLED
		break;
	
	case (3):
		brink_count++;
		if (Brink == 1)
		{
			if(brink_count >= 10)				// 0.6秒
			{
				brink_count = 0;
			}
			if(brink_count < 5)					// 0.3秒
				brink_flag = 1;					// 消灯
			else
				brink_flag = 0;					// 点灯
		}
		else
		{
			brink_flag = 0;						// 点灯
		}
		break;
	
	default	:
		break;
	}
}


/******************************************************************************
##
## Name    :LED_CNT_set
## Function:LED点灯I2Cデータ更新
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
## Name    :LED_CNT_BAR_set
## Function:バーLED点灯I2Cデータ更新
## Input   :桁指定
##			表示データ
## Output  :なし
## Memo    :なし
##
******************************************************************************/
void LED_CNT_BAR_set( unsigned char Bar)
{
	int i;
	int syou;
	int amari;
	
	if(Bar >= MAX_Bar)
	{
		Bar = 40;
		
		for( i = 0; i < 5; i++)
		{
			disp_data[i] = OUT_PTN[8];
		}
	}
	else
	{
		syou = Bar / 8;
		amari = Bar % 8;
		
		for( i = 0; i < syou; i++)
		{
			disp_data[i] = OUT_PTN[8];
		}
		
		disp_data[syou] = OUT_PTN[amari];
		
		for( i = syou+1; i < 5; i++)
		{
			disp_data[i] = OUT_PTN[0];
		}
	}
	
	if(HI >= MAX_Bar)
	{
		HI = 40;
		if(brink_flag == 1)						// 消灯
		{
			disp_data[4] = disp_data[4] & ~OUT_BIT[8];
		}
		else
		{
			disp_data[4] = disp_data[4] | OUT_BIT[8];
		}
	}
	else
	{
		syou = HI / 8;
		amari = HI % 8;
		if(brink_flag == 1)						// 消灯
		{
			if( amari == 0)
				disp_data[syou-1] = disp_data[syou-1] & ~OUT_BIT[8];
			else
				disp_data[syou] = disp_data[syou] & ~OUT_BIT[amari];
		}
		else
		{
			if( amari == 0)
				disp_data[syou-1] = disp_data[syou-1] | OUT_BIT[8];
			else
				disp_data[syou] = disp_data[syou] | OUT_BIT[amari];
		}
	}
	
	if(LO >= MAX_Bar)
	{
		LO = 40;
		if(brink_flag == 1)						// 消灯
		{
			disp_data[4] = disp_data[4] & ~OUT_BIT[8];
		}
		else
		{
			disp_data[4] = disp_data[4] | OUT_BIT[8];
		}
	}
	else
	{
		syou = LO / 8;
		amari = LO % 8;
		if(brink_flag == 1)						// 消灯
		{
			if( amari == 0)
				disp_data[syou-1] = disp_data[syou-1] & ~OUT_BIT[8];
			else
				disp_data[syou] = disp_data[syou] & ~OUT_BIT[amari];
		}
		else
		{
			if( amari == 0)
				disp_data[syou-1] = disp_data[syou-1] | OUT_BIT[8];
			else
				disp_data[syou] = disp_data[syou] | OUT_BIT[amari];
		}
	}
	
	if(MEAN >= MAX_Bar)
	{
		MEAN = 40;
		disp_data[4] = OUT_BIT[8];
	}
	else
	{
		syou = MEAN / 8;
		amari = MEAN % 8;
		if( amari == 0)
			disp_data[syou-1] = disp_data[syou-1] | OUT_BIT[8];
		else
			disp_data[syou] = disp_data[syou] | OUT_BIT[amari];
	}
}


/******************************************************************************
##
## Name    :LED_CNT
## Function:ダイナミック点灯（0.5msec）
## Input   :なし
## Output  :なし
## Memo    :なし
##
******************************************************************************/
void LED_CNT( void )
{
	if( count == 0)
	{
		LED_15_Off(); 
		PRT0DR = ~(disp_data[count]);			// 負論理のためチルダが付いている
		LED_10_On();
	}
	else if( count == 1)
	{
		LED_10_Off(); 
		PRT0DR = ~(disp_data[count]);			// 負論理のためチルダが付いている
		LED_11_On();
	}
	else if( count == 2)
	{
		LED_11_Off(); 
		PRT0DR = ~(disp_data[count]);			// 負論理のためチルダが付いている
		LED_12_On();
	}
	else if( count == 3)
	{
		LED_12_Off(); 
		PRT0DR = ~(disp_data[count]);			// 負論理のためチルダが付いている
		LED_13_On();
	}
	else if( count == 4)
	{
		LED_13_Off(); 
		PRT0DR = ~(disp_data[count]);			// 負論理のためチルダが付いている
		LED_14_On();
	}
	else if( count == 5)
	{
		LED_14_Off(); 
		PRT0DR = ~(disp_data[count]);			// 負論理のためチルダが付いている
		LED_15_On();
	}
	else if( count == 6)
	{
		LED_15_Off(); 
			count_10ms++;
			
			LED_CNT_update(count_10ms);
			if(count_10ms >= 10)				// 1秒
			{
				count_10ms = 0;
			}
	}
	count++;
	bit = bit * 2;
	if(count >= MAX_KETA+1)
	{
		count = 0;
		bit = 1;
	}
}

