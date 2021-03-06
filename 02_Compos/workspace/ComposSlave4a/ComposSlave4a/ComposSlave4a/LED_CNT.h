/*******************************************************************************
*                                                                               
*    ﾌｧｲﾙ名 ：   LED_CNT.h                                                      
*                                                                               
*    概要 ：                                                                    
*          ダイナミック点灯（11桁）のプログラム                           
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

#define MAX_KETA 6		// 最大表示桁数
#define MAX_Bar 40		// BAR最大値

/*******************************************************************************
  構造体型宣言
*******************************************************************************/


/*******************************************************************************
  外部宣言
*******************************************************************************/


extern unsigned char	Status;		// スレーブのステータス状況

extern unsigned char	Cmnd;		// スレーブへのコマンド
extern unsigned short	Volume;		// 1回換気
extern unsigned short	MaxPrs;		// 最大吸気圧
extern unsigned char	PEEP;		// ＰＥＥＰ


extern void LED_CNT_init( void );
extern void LED_CNT_update( unsigned char count_10ms );
extern void LED_CNT_set( unsigned char keta, unsigned char value);
extern void LED_CNT_BAR_set( unsigned char Bar);
extern void LED_CNT( void );


/******************************************************************************
  マクロ定義
******************************************************************************/


/******************************************************************************
  関数プロトタイプ
******************************************************************************/


void LED_CNT_init( void );
void LED_CNT_update( unsigned char count_10ms );
void LED_CNT_set( unsigned char keta, unsigned char value);
void LED_CNT_BAR_set( unsigned char Bar);
void LED_CNT( void );


