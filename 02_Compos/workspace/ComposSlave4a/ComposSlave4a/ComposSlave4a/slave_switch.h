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

/******************************************************************************
  定数定義
******************************************************************************/

#define SW_UP_FIX		0x07				// 上キーの確定条件
#define SW_DOWN_FIX		0x07				// 下キーの確定条件
#define SW_LEFT_FIX		0x07				// 左キーの確定条件
#define SW_RIGHT_FIX	0x07				// 右キーの確定条件
#define SW_10count		0x03FF				// 1が10bit


/*******************************************************************************
  構造体型宣言
*******************************************************************************/

typedef struct							// スイッチ入力
{
	unsigned char		SW_UP;			// 上キー
	unsigned char		SW_DOWN;		// 下キー
	unsigned char		SW_LEFT;		// 左キー
	unsigned char		SW_RIGHT;		// 右キー
	
	unsigned short		SW_UP_10;			// 上キー
	unsigned short		SW_DOWN_10;		// 下キー
	unsigned short		SW_LEFT_10;		// 左キー
	unsigned short		SW_RIGHT_10;		// 右キー
	union 
	{
		unsigned char	BYTE;			// スイッチ状態フラグ
		struct
		{
			unsigned char UP    :1;		// 上キー
			unsigned char DOWN  :1;		// 下キー
			unsigned char LEFT  :1;		// 左キー
			unsigned char RIGHT :1;		// 右キー
			unsigned char       :4;		//
		} BIT;
	} FIX;
} SW_INPUT;

/******************************************************************************
  マクロ定義
******************************************************************************/


/*******************************************************************************
 外部宣言
*******************************************************************************/

extern void slave_sw_init( void );
extern void scan_slave_switch(void);

/*******************************************************************************
 プロトタイプ宣言
*******************************************************************************/
void slave_sw_init( void );
void scan_slave_switch(void);

