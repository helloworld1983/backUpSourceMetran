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

	/******* Modify S 110 *********/
//		unsigned char	InsCnt_Phase;			// RAM[4] 
#define Ins_Strt	0							// 吸気開始
#define Ins_Sec		1							// 吸気中2
#define Ins_End		2							// 吸気終末
#define Exh_Strt	3							// 呼気開始
#define Exh			4							// 呼気中
#define Exh_End		5							// 呼気終末
#define Trig_View	6							// トリガビュー
#define Trig_View2	7							// トリガビュー

/*
#define Ins_Strt	0							// 吸気開始
#define Ins_Pri		1							// 吸気中1 使っていない
#define Ins_Sec		2							// 吸気中2
#define Ins_End		3							// 吸気終末
#define Exh_Strt	4							// 呼気開始
#define Exh			5							// 呼気中
#define Exh_End		6							// 呼気終末
#define Trig_View	7							// トリガビュー
*/
	/******* Modify S 110 *********/

/*******************************************************************************
  構造体型宣言
*******************************************************************************/


typedef union 									// 
{
	unsigned char	BYTE;						// 
	struct
	{
		unsigned char Mode		:4;				// モード
		unsigned char Phase		:4;				// 呼吸相
	} A;
} T_InsCnt;

typedef union 									// 
{
	unsigned char	BYTE;						// 
	struct
	{
		unsigned char Phase		:4;				// 呼吸相
		unsigned char No_Exh	:1;				// 一回換気量未測定
		unsigned char EXH		:1;				// 呼気
		unsigned char INS		:1;				// 吸気
		unsigned char ERR		:1;				// エラー
	} A;
} T_FLOW_PHASE;

typedef union
{
	unsigned char	BYTE;
	struct
	{
		unsigned char Reserv		:6;			// 呼気
		unsigned char PT_Flow		:1;			// 圧・フローデータ選択
												// 1:圧力
												// 0:フロー
		unsigned char Ins_OP		:1;			// 呼吸作動中
	} A;
} T_Cmnd;

	/******* Modify S 112 *********/
typedef union 									// 
{
	unsigned char	BYTE;						// 
	struct
	{
		unsigned char Reserv		:7;			// 
		unsigned char SIMV		:1;			// SIMV動作
	} A;
} T_DOUCHOU;
	/******* Modify E 112 *********/

// PSoC1データ構造体
typedef union 
{
	unsigned char RAM[74];						// 配列読み込み
	struct					
	{
		unsigned char	Count_W1;				// RAM[0] 書き込みカウンタ１
		T_Cmnd			Cmnd;					// RAM[1] スレーブへのコマンド
		T_InsCnt		InsCnt;					// RAM[2] 呼吸相
		unsigned char	BZ;						// RAM[3] BZ
		unsigned short	PV_PIP1;				// RAM[4,5] 
		unsigned short	PV_PIP2;				// RAM[6,7] 
		unsigned short	PV_PEEP;				// RAM[8,9] 
		unsigned short	Tim_Total;				// RAM[10,11] 
		unsigned short	Tim_PIP2;				// RAM[12,13] 
		unsigned short	Tim_PIP1;				// RAM[14,15] 使っていない
		unsigned short	Tim_Trig;				// RAM[16,17] 使っていない
		unsigned short	Trgt_Pres;				// RAM[18,19] 
		unsigned char	Hi_Prs;					// RAM[20] 高圧アラーム
		unsigned char	KINK_DIF;				// RAM[21] キンク差圧
		unsigned char	Count_W2;				// RAM[22] 書き込みカウンタ２
		
		unsigned char	Count_A1;				// RAM[23] 
		unsigned short	P_00_CAL;				// RAM[24,25] 
		unsigned short	P_30_CAL;				// RAM[26,27] 
		unsigned short	P_60_CAL;				// RAM[28,29] 
		unsigned short	F_00_CAL;				// RAM[30,31] 
		unsigned short	F_03_CAL;				// RAM[32,33] 
		unsigned short	F_06_CAL;				// RAM[34,35] 
		unsigned short	F_09_CAL;				// RAM[36,] 
		unsigned short	F_12_CAL;				// RAM[38,] 
		unsigned short	F_15_CAL;				// RAM[40,] 
		unsigned short	F_20_CAL;				// RAM[42,] 
		unsigned short	F_30_CAL;				// RAM[44,] 
		unsigned short	F_40_CAL;				// RAM[46,] 
		unsigned short	F_50_CAL;				// RAM[48,] 
		unsigned char	Count_A2;				// RAM[50] 
		
		unsigned char	INS_Trig;				// RAM[51] 	
		signed char		VOL_RATE;				// RAM[52] 	// I
		/******* Modify S 111 *********/
//		unsigned char	Reserv_3;				// RAM[53] 	// D
		unsigned char	BZ_VOL;					// RAM[53] 	// D
		/******* Modify E 111 *********/
		/******* Modify S 112 *********/
		T_DOUCHOU		DOUCHOU;
//		unsigned char	Reserv_4;				// RAM[54] 
		/******* Modify E 112 *********/
		
		unsigned char	Count_R1;				// RAM[55] 読み出しカウンタ１
		signed short	Exh_Vol;				// RAM[56,57] 
		signed short	Flow1;					// RAM[58,59] フロー１
		signed short	PTm;					// RAM[60,61] 口元圧(平均)
		signed short	PTh;					// RAM[62,63] 口元圧(ピーク)
		signed short	RAW_DATA;				// RAM[64,65] 生データ
		unsigned short	Enc;					// RAM[66,67] エンコーダ
		unsigned char	Status;					// RAM[68] スレーブのステータス状況
		T_FLOW_PHASE	FLOW_PHASE;				// RAM[69] 
		union
		{
			unsigned char	BYTE;				// RAM[70] 
			struct
			{
				unsigned char   Hi_Alm   :1;	// 高圧警報
				unsigned char   ER_EPV   :1;	// EPバルブ破損
/******* Modify S 111 *********/
//				unsigned char   RSV1     :1;	// ();
				unsigned char   FLW_DCnct :1;	// ();
/******* Modify E 111 *********/
				unsigned char   GAS_DCnct :1;	// ガス回路外れ
				unsigned char   PT_DCnct :1;	// 圧ライン外れ
				unsigned char   PT_KINK  :1;	// 圧ラインキンク
				unsigned char   ER_PRES  :1;	// 圧力センサ異常
				unsigned char   ER_I2C   :1;	// 通信異常
			} BIT;
		} ERR;									// 書き込み中フラグ
		unsigned char	Count_R2;				// RAM[71] 読み出しカウンタ２
		unsigned short	VER;					// RAM[72,73] 
	} DATA;
} T_PSoC1;





/******************************************************************************
  マクロ定義
******************************************************************************/


/*******************************************************************************
  外部宣言
*******************************************************************************/

extern T_Cmnd	Cmnd;							// RAM[1] スレーブへのコマンド
extern T_InsCnt			InsCnt;					// RAM[2] 呼吸相
extern unsigned char	BZ;						// RAM[3] BZ

extern unsigned short	Flow_Offset;			// RAM[6,7] Flow_Offset
extern unsigned short	PV_PIP1A;				// RAM[10,11] 
extern unsigned short	PV_PIP2A;				// RAM[14,15] 
extern unsigned short	PV_PEEPA;				// RAM[18,19] 
extern unsigned short	Tim_TotalA;				// RAM[10,11] 
extern unsigned short	Tim_PIP2A;				// RAM[12,13] 
extern unsigned short	Tim_PIP1A;				// RAM[14,15] 
extern unsigned short	Tim_TrigA;				// RAM[16,17] 
extern unsigned short	Trgt_PresA;				// RAM[18,19] 
extern unsigned char	Hi_PrsA;				// RAM[20] 高圧アラーム
extern unsigned char	KINK_DIFA;				// RAM[21] キンク差圧

extern unsigned short	PV_PIP1B;				// RAM[10,11] 
extern unsigned short	PV_PIP2B;				// RAM[14,15] 
extern unsigned short	PV_PEEPB;				// RAM[18,19] 
extern unsigned short	Tim_TotalB;				// RAM[10,11] 
extern unsigned short	Tim_PIP2B;				// RAM[12,13] 
extern unsigned short	Tim_PIP1B;				// RAM[14,15] 
extern unsigned short	Tim_TrigB;				// RAM[16,17] 
extern unsigned short	Trgt_PresB;				// RAM[18,19] 
extern unsigned char	Hi_PrsB;				// RAM[20] 高圧アラーム
extern unsigned char	KINK_DIFB;				// RAM[21] キンク差圧
extern unsigned char	Tim_PIP2_Dif_flag;		// 吸気時間変更フラグ

extern unsigned char	Status;					// RAM[33] スレーブのステータス状況
extern signed short		Exh_Vol;				// RAM[34,35] 
extern signed short		Flow1;					// RAM[36,37] フロー１
extern signed short		Flow_RAW;				// RAM[36,37] フロー１
extern signed short		PT_RAW;					// RAM[36,37] フロー１
extern signed short		PTm;					// RAM[38,39] 口元圧(平均)
extern signed short		PTh;					// RAM[40,41] 口元圧(ピーク)
extern unsigned short	Enc;					// RAM[42,43] エンコーダ
extern unsigned char No_Exh_count;
extern unsigned char	INS_Trig;
extern signed char		VOL_RATE;				// RAM[52] 	// I
/******* Modify S 112 *********/
extern T_DOUCHOU		DOUCHOU;				// RAM[54]
/******* Modify E 112 *********/

extern T_PSoC1 PSoC1;							// I2C構造体
extern T_FLOW_PHASE FLOW_PHASE;					// 吸気呼気エラー

extern unsigned short	PV_soft;

extern void I2C_CNT_init( void );
extern void I2C_CNT( void );


/******************************************************************************
  関数プロトタイプ
******************************************************************************/


void I2C_CNT_init( void );
void I2C_CNT( void );


