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

// PSoC3データ構造体
typedef union 									// 
{
	unsigned char RAM[13];						// 配列読み込み
	struct					
	{
		unsigned char	Cmnd;					// RAM[0] スレーブへのコマンド
		unsigned char	Dark_Mode;				// RAM[1] 暗表示
		unsigned char	MaxPrs_Set;				// RAM[2] 最大気道内圧
		unsigned char	PEEP_Set;				// RAM[3] PEEP
		unsigned short	InsCnt_Set;				// RAM[4,5] 呼吸回数
		unsigned char	InsTim_Set;				// RAM[6] 吸気時間
		unsigned char	Apnea_Set;				// RAM[7] Apnea
		union
		{
			unsigned char	BYTE;				// RAM[8] 点滅
			struct
			{
				unsigned char   Fast    :2;		// 
				unsigned char           :1;		// 
				unsigned char   MaxPrs  :1;		// 
				unsigned char   PEEP    :1;		// 
				unsigned char   InsCnt  :1;		// 
				unsigned char   InsTim  :1;		// 
				unsigned char   Apnea   :1;		// 
			} BIT;
		} Brink_Set;
		
		unsigned char	Status;					// RAM[9] スレーブのステータス状況
		unsigned short	VER;					// RAM[62,63] 
		unsigned char	Reserv_1;				// RAM[10] 
	} DATA;
} T_PSoC3;


typedef union
{
	unsigned char	BYTE;				// SW
	struct
	{
		unsigned char   Fast    :2;		// 
		unsigned char           :1;		// 
		unsigned char   MaxPrs  :1;		// 
		unsigned char   PEEP    :1;		// 
		unsigned char   InsCnt :1;		// 
		unsigned char   InsTim  :1;		// 
		unsigned char   Apnea   :1;		// 
	} BIT;
} T_Brink_Set;


/*******************************************************************************
  外部宣言
*******************************************************************************/


extern unsigned char	Status;			// スレーブのステータス状況

extern unsigned char	Cmnd;			// スレーブへのコマンド
extern unsigned char	Dark_Mode;		// 暗表示
extern unsigned char	MaxPrs_Set;		// 最大気道内圧
extern unsigned char	PEEP_Set;		// PEEP
extern unsigned short	InsCnt_Set;		// 呼吸回数
extern unsigned char	InsTim_Set;		// 吸気時間
extern unsigned char	Apnea_Set;		// Apnea
extern T_PSoC3 PSoC3;					// I2C構造体
extern T_Brink_Set Brink_Set;			// 点滅
extern unsigned char	I2C_ERR;

extern void I2C_CNT_init( void );
extern void I2C_CNT( void );


/******************************************************************************
  マクロ定義
******************************************************************************/


/******************************************************************************
  関数プロトタイプ
******************************************************************************/


void I2C_CNT_init( void );
void I2C_CNT( void );


