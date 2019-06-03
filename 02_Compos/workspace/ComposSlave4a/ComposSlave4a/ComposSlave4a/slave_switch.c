/*******************************************************************************
*                                                                               
*    ﾌｧｲﾙ名 ：   slave_switch.h                                                 
*                                                                               
*    概要 ：                                                                    
*          スレーブ側SW入力処理のプログラム                                     
*                                                                               
*    公開API :                                                                  
*                                                                               
*    備考 :                                                                     
*                                                                               
*                                                                               
*    変更履歴 :                                                                 
*            日  付    内    容                            Ver No.              
*            --------  ----------------------------------- -------              
*            10.01.26  初版作成                            0.00                 
*            11.12.08  1版作成                             1.00                 
*                                                                               
*******************************************************************************/
#include <m8c.h>        // part specific constants and macros
#include "slave_switch.h"
#include "I2C_CNT.h"


/******************************************************************************
*** RAM・ROM
******************************************************************************/

SW_INPUT SW_IN;



/******************************************************************************
*** プログラムコード
******************************************************************************/

/******************************************************************************
##
## Name    :slave_sw_init
## Function:スレーブ側SW入力処理初期化
## Input   :なし
## Output  :なし
## Memo    :なし
##
******************************************************************************/

void slave_sw_init( void )
{
	SW_IN.SW_UP = 0;
	SW_IN.SW_DOWN = 0;
	SW_IN.SW_LEFT = 0;
	SW_IN.SW_RIGHT = 0;
	
	SW_IN.SW_UP_10 = 0;
	SW_IN.SW_DOWN_10 = 0;
	SW_IN.SW_RIGHT_10 = 0;
	SW_IN.SW_LEFT_10 = 0;
	
	SW_IN.FIX.BYTE = 0;
	
	PRT1DR = 0x5C;		// プルアップで、使用するポートをHiにする。
}

/******************************************************************************
##
## Name    :scan_slave_switch
## Function:スレーブ側SW入力処理
## Input   :なし
## Output  :なし
## Memo    :なし
##
******************************************************************************/

void scan_slave_switch( void )
{

	//////////////////////////////////////////////////
	//  UP JET
	//////////////////////////////////////////////////
	if( PRT1DR & 0x40 )							// P1-6 スイッチ入力あり
		SW_IN.SW_UP = SW_IN.SW_UP << 1 | 1;		// 左１ビットシフトで最下位ビット１
	else										// スイッチ入力なし
		SW_IN.SW_UP = SW_IN.SW_UP << 1;			// 左１ビットシフトで最下位ビット０
	
	if( ( SW_IN.SW_UP & SW_UP_FIX )
	 == SW_UP_FIX )								// 押下状態が確定した場合
		SW_IN.SW_UP_10 = SW_IN.SW_UP_10 << 1 | 1;
	else
		SW_IN.SW_UP_10 = SW_IN.SW_UP_10 << 1;
	
	if( SW_IN.SW_UP_10 & SW_10count)			// 過去1秒以内に押下状態がある
		SW_IN.FIX.BIT.UP = 1;					// ＳＷ状態フラグをセットする
	else
		SW_IN.FIX.BIT.UP = 0;					// ＳＷ状態フラグをクリアする
	
	//////////////////////////////////////////////////
	// DOWN MUTE
	//////////////////////////////////////////////////
	if( PRT1DR & 0x10 )							// P1-4 スイッチ入力あり
		SW_IN.SW_DOWN = SW_IN.SW_DOWN << 1 | 1;	// 左１ビットシフトで最下位ビット１
	else										// スイッチ入力なし
		SW_IN.SW_DOWN = SW_IN.SW_DOWN << 1;		// 左１ビットシフトで最下位ビット０
	
	if( ( SW_IN.SW_DOWN & SW_DOWN_FIX )
	 == SW_DOWN_FIX )							// 押下状態が確定した場合
		SW_IN.SW_DOWN_10 = SW_IN.SW_DOWN_10 << 1 | 1;
	else
		SW_IN.SW_DOWN_10 = SW_IN.SW_DOWN_10 << 1;
	
	if( SW_IN.SW_DOWN_10 & SW_10count)			// 過去1秒以内に押下状態がある
		SW_IN.FIX.BIT.DOWN = 1;					// ＳＷ状態フラグをセットする
	else
		SW_IN.FIX.BIT.DOWN = 0;					// ＳＷ状態フラグをクリアする
	
	//////////////////////////////////////////////////
	//  RIGHT AUTO
	//////////////////////////////////////////////////
	if( PRT1DR & 0x04 )							// P1-2 スイッチ入力あり
		SW_IN.SW_RIGHT = SW_IN.SW_RIGHT << 1 | 1;		// 左１ビットシフトで最下位ビット１
	else										// スイッチ入力なし
		SW_IN.SW_RIGHT = SW_IN.SW_RIGHT << 1;	// 左１ビットシフトで最下位ビット０
	
	if( ( SW_IN.SW_RIGHT & SW_RIGHT_FIX )
	 == SW_RIGHT_FIX )							// 押下状態が確定した場合
		SW_IN.SW_RIGHT_10 = SW_IN.SW_RIGHT_10 << 1 | 1;
	else
		SW_IN.SW_RIGHT_10 = SW_IN.SW_RIGHT_10 << 1;
	
	if( SW_IN.SW_RIGHT_10 & SW_10count)			// 過去1秒以内に押下状態がある
		SW_IN.FIX.BIT.RIGHT = 1;				// ＳＷ状態フラグをセットする
	else
		SW_IN.FIX.BIT.RIGHT = 0;				// ＳＷ状態フラグをクリアする
	
	//////////////////////////////////////////////////
	// LEFT 
	//////////////////////////////////////////////////
	if( PRT1DR & 0x08 )							// P1-3 スイッチ入力あり
		SW_IN.SW_LEFT = SW_IN.SW_LEFT << 1 | 1;		// 左１ビットシフトで最下位ビット１
	else										// スイッチ入力なし
		SW_IN.SW_LEFT = SW_IN.SW_LEFT << 1;		// 左１ビットシフトで最下位ビット０
	
	if( ( SW_IN.SW_LEFT & SW_LEFT_FIX )
	 == SW_LEFT_FIX )							// 押下状態が確定した場合
		SW_IN.SW_LEFT_10 = SW_IN.SW_LEFT_10 << 1 | 1;
	else
		SW_IN.SW_LEFT_10 = SW_IN.SW_LEFT_10 << 1;
	
	if( SW_IN.SW_LEFT_10 & SW_10count)			// 過去1秒以内に押下状態がある
		SW_IN.FIX.BIT.LEFT = 1;					// ＳＷ状態フラグをセットする
	else
		SW_IN.FIX.BIT.LEFT = 0;					// ＳＷ状態フラグをクリアする
	
	//////////////////////////////////////////////////
	// データコピー
	//////////////////////////////////////////////////
	PSoC4.DATA.SW.BIT.MUTE  = SW_IN.FIX.BIT.DOWN;		// 消音ボタン
	PSoC4.DATA.SW.BIT.AUTO  = SW_IN.FIX.BIT.RIGHT;		//  AUTOボタン
	PSoC4.DATA.SW.BIT.SELCT = SW_IN.FIX.BIT.LEFT;		// 設定選択ボタン
	PSoC4.DATA.SW.BIT.JET   = SW_IN.FIX.BIT.UP;			// ジェットボタン
}



