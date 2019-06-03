/*******************************************************************************
*                                                                               
*    ﾌｧｲﾙ名 ：   clock_05ms.h                                                   
*                                                                               
*    概要 ：                                                                    
*          ソフトウェアタイマ割り込みのプログラム                               
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


/******************************************************************************
  定数定義
******************************************************************************/


/******************************************************************************
  構造体
******************************************************************************/


/*******************************************************************************
  外部宣言
*******************************************************************************/


extern unsigned char   flag100ms;
			// 100msec周期フラグ(セット:割込処理 , クリア:ユーザー処理)
extern unsigned char   flag10ms;
			// 10msec周期フラグ(セット:割込処理 , クリア:ユーザー処理)
extern unsigned char   flag05ms;
			// 0.5msec周期フラグ(セット:割込処理 , クリア:ユーザー処理)

extern void clock_init( void );
extern void PWM8_1_ISR2( void );


/******************************************************************************
  マクロ定義
******************************************************************************/


/******************************************************************************
  関数プロトタイプ
******************************************************************************/

void clock_init( void );
void PWM8_1_ISR2( void );

