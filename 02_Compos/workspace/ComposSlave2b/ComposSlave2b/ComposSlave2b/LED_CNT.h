/*******************************************************************************
*                                                                               
*    ﾌｧｲﾙ名 ：   LED_CNT.h                                                      
*                                                                               
*    概要 ：                                                                    
*          ７セグダイナミック点灯（9桁）のプログラム                           
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

#define MAX_KETA 9		// 最大表示桁数

/*******************************************************************************
  構造体型宣言
*******************************************************************************/

typedef union 
{
    unsigned short      WORD;		//
    struct 
	{
        unsigned char   D15:1;      // 
        unsigned char   D14:1;      // 
        unsigned char   D13:1;      // 
        unsigned char   D12:1;      // 
        unsigned char   D11:1;      // 
        unsigned char   D10:1;      // 
        unsigned char   D09:1;      // 
        unsigned char   D08:1;      // 
        unsigned char   D07:1;      // 
        unsigned char   D06:1;      // 
        unsigned char   D05:1;      // 
        unsigned char   D04:1;      // 
        unsigned char   D03:1;      // 
        unsigned char   D02:1;      // 
        unsigned char   D01:1;      // 
        unsigned char   D00:1;      // 
    } BIT;
} T_DOT;


/*******************************************************************************
  外部宣言
*******************************************************************************/


extern unsigned char	Status;		// スレーブのステータス状況
extern unsigned char	Cmnd;		// スレーブへのコマンド

extern void LED_CNT_init( void );
extern void LED_CNT_update( unsigned char count_10ms );
extern void LED_CNT_set( unsigned char keta, unsigned char value);
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
void LED_CNT( void );


