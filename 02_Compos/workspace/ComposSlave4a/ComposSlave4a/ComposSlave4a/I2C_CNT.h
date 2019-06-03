/*******************************************************************************
*                                                                               
*    ﾌｧｲﾙ名 ：   I2C_CNT.h                                                      
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

/******************************************************************************
  定数定義
******************************************************************************/



/*******************************************************************************
  構造体型宣言
*******************************************************************************/

// PSoC4データ構造体
typedef union 									// 
{
	unsigned char RAM[15];						// 配列読み込み
	struct					
	{
		unsigned char	Count_W1;				// RAM[0] 書き込みカウンタ１
		unsigned char	Cmnd;					// RAM[1] スレーブへのコマンド
		unsigned char	Bar;					// RAM[2] バーグラフ
		unsigned char	MEAN;					// RAM[3] バーグラフ
		union
		{
			unsigned char	BYTE;				// RAM[4] アラームLED
			struct
			{
				unsigned char   High_Prs :1;	// 高圧アラーム
				unsigned char   Low_Prs  :1;	// 低圧アラーム
				unsigned char   Pow_Fail :1;	// 電源断アラーム
				unsigned char   Disconec :1;	// 回路はずれアラーム
				unsigned char   Work_Prs :1;	// 供給圧
				unsigned char   Prs      :1;	// 口元圧
				unsigned char   Apnea    :1;	// Apneaアラーム
				unsigned char   Jet_BTN  :1;	// ジェットボタン
			} BIT;
		} AlarmLED;
		unsigned char	Count_W2;				// RAM[5] 書き込みカウンタ２
		
		unsigned char	HI;						// RAM[6] バーグラフ
		unsigned char	LO;						// RAM[7] バーグラフ
		
		unsigned char	Count_R1;				// RAM[8] 読み出しカウンタ１
		unsigned char	Status;					// RAM[9] スレーブのステータス状況
		union
		{
			unsigned char	BYTE;				// RAM[10] SW
			struct
			{
				unsigned char        :4;		// 
				unsigned char   MUTE :1;		// 消音ボタン
				unsigned char   AUTO :1;		// AUTOボタン
				unsigned char   SELCT:1;		// 設定選択ボタン
				unsigned char   JET  :1;		// ジェットボタン
			} BIT;
		} SW;
		unsigned char	Count_R2;				// RAM[11] 読み出しカウンタ２
		unsigned short	VER;					// RAM[12,13] 
		unsigned char	Reserv_1;				// RAM[14] 
	} DATA;
} T_PSoC4;



/******************************************************************************
  マクロ定義
******************************************************************************/


/*******************************************************************************
  外部宣言
*******************************************************************************/


extern unsigned char	Status;					// スレーブのステータス状況

extern unsigned char	Cmnd;					// スレーブへのコマンド
extern unsigned char	Bar;					// RAM[2] バーグラフ
extern unsigned char	AlarmLED;				// RAM[3] アラームLED
extern unsigned char	MEAN;					// RAM[4] バーグラフ
extern unsigned char	HI;						// RAM[6] バーグラフ
extern unsigned char	LO;						// RAM[7] バーグラフ
extern unsigned char	Brink;					// RAM[7] バーグラフ

extern T_PSoC4 PSoC4;							// I2C構造体

extern void I2C_CNT_init( void );
extern void I2C_CNT( void );


/******************************************************************************
  関数プロトタイプ
******************************************************************************/


void I2C_CNT_init( void );
void I2C_CNT( void );


