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

// PSoC2データ構造体
typedef union 									// 
{
	unsigned char RAM[14];						// 配列読み込み
	struct					
	{
		unsigned char	Count_W1;				// RAM[0] 書き込みカウンタ１
		unsigned char	Cmnd;					// RAM[1] スレーブへのコマンド
		unsigned short	Volume;					// RAM[2,3] 1回換気
		unsigned short	MaxPrs;					// RAM[4,5] 最大吸気圧
		unsigned char	PEEP;					// RAM[6] ＰＥＥＰ
		union
		{
			unsigned char	BYTE;				// RAM[7] 点滅
			struct
			{
				unsigned char   Fast    :2;		// 
				unsigned char           :2;		// 
				unsigned char   VolumeH :1;		// 
				unsigned char   VolumeL :1;		// 
				unsigned char   MaxPrs  :1;		// 
				unsigned char   PEEP    :1;		// 
			} BIT;
		} Brink_Set;
		union
		{
			unsigned char	BYTE;				// RAM[8] 点滅
			struct
			{
				unsigned char   VolumeE  :1;	// 
/******* Modify S 111 *********/
				unsigned char   JET      :1;	// ();
//				unsigned char   RSV2     :1;	// ();
/******* Modify E 111 *********/
				unsigned char   RSV1     :1;	// ();
				unsigned char   GAS_DCnct :1;	// ガス回路外れ
				unsigned char   PT_DCnct :1;	// 圧ライン外れ
				unsigned char   PT_KINK  :1;	// 圧ラインキンク
				unsigned char   ER_PRES  :1;	// 圧力センサ異常
				unsigned char   ER_I2C   :1;	// 通信異常
			} BIT;
		} Disp_Mode;
		unsigned char	Count_W2;				// RAM[9] 書き込みカウンタ２
		
		unsigned char	Status;					// RAM[10] スレーブのステータス状況
		unsigned char	Reserv_1;				// RAM[11] 
		unsigned short	VER;					// RAM[12,13] 
	} DATA;
} T_PSoC2;

typedef union
{
	unsigned char	BYTE;	// SW
	struct
	{
		unsigned char   Fast    :2;				// 
		unsigned char           :2;				// 
		unsigned char   VolumeH :1;				// 
		unsigned char   VolumeL :1;				// 
		unsigned char   MaxPrs  :1;				// 
		unsigned char   PEEP    :1;				// 
	} BIT;
} T_Brink_Set;

typedef union
{
	unsigned char	BYTE;						// RAM[7] 点滅
	struct
	{
		unsigned char           :2;				// 
		unsigned char           :2;				// 
		unsigned char           :1;				// 
		unsigned char   ER_PRES :1;				// 圧力センサ異常
		unsigned char   ER_I2C  :1;				// 通信異常
		unsigned char   VolumeE :1;				// 
	} BIT;
} T_Disp_Mode;


typedef union
{
	unsigned short	WORD;						// SW
	struct
	{
		unsigned char	HI;						// 書き込みカウンタ１
		unsigned char	LO;						// スレーブへのコマンド
	} BYTE;
} T_ENDIAN;


/******************************************************************************
  マクロ定義
******************************************************************************/


/*******************************************************************************
  外部宣言
*******************************************************************************/

extern unsigned short	Volume;					// 1回換気
extern unsigned short	MaxPrs;					// 最大吸気圧
extern unsigned char	PEEP;					// ＰＥＥＰ

extern T_PSoC2 PSoC2;							// I2C構造体
extern T_Brink_Set Brink_Set;					// 点滅


extern void I2C_CNT_init( void );
extern void I2C_CNT( void );


/******************************************************************************
  関数プロトタイプ
******************************************************************************/


void I2C_CNT_init( void );
void I2C_CNT( void );


