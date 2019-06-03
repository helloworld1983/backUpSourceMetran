/*******************************************************************************
*                                                                               
*    ﾌｧｲﾙ名 ：   Flow.h                                                      
*                                                                               
*    概要 ：                                                                    
*          adc処理のプログラム                                            
*                                                                               
*    公開API :                                                                  
*                                                                               
*    備考 :                                                                     
*                                                                               
*                                                                               
*    変更履歴 :                                                                 
*            日  付    内    容                            Ver No.              
*            --------  ----------------------------------- -------              
*            10.06.19  初版作成                            0.00                 
*            11.12.08  1版作成                             1.00                 
*            11.12.19                                      1.01                 
*            11.12.20                                      1.02                 
*                                                                               
*******************************************************************************/

/******************************************************************************
  定数定義
******************************************************************************/

#define temp_flow_num2	24
#define base_flow_num	128
#define flow_trg_ary_FIX1	0x03				// トリガの確定条件
#define flow_trg_ary_FIX2	0x03				// トリガの確定条件
#define flow_trg_ary_FIX5	0x03				// トリガの確定条件
#define flow_trg_ary_FIX6	0x03				// トリガの確定条件

#define NON			0
#define INS_START	1
#define INS_MID		2
#define PLATEAU		3
#define EXH_START	4
#define EXH_MID		5

#define analog_PSOC 0

/*******************************************************************************
  構造体型宣言
*******************************************************************************/


/******************************************************************************
  マクロ定義
******************************************************************************/


/*******************************************************************************
  外部宣言
*******************************************************************************/

extern void Flow_init( void );
extern void Flow_cnt( void );


#if analog_PSOC
#else
extern unsigned int VF;
extern unsigned short	F_00_CAL;
extern unsigned short	F_03_CAL;
extern unsigned short	F_06_CAL;
extern unsigned short	F_09_CAL;
extern unsigned short	F_12_CAL;
extern unsigned short	F_15_CAL;
extern unsigned short	F_20_CAL;
extern unsigned short	F_30_CAL;
extern unsigned short	F_40_CAL;
extern unsigned short	F_50_CAL;
extern signed short 	Real_flow;
extern unsigned char 	Resp_Phase;				// 呼吸フェーズ
extern signed short 	DEF_Flow;
extern signed short 	Max_Ins_Flow;
extern signed short		Max_Exh_Flow;
extern unsigned char	D_Cnect_state;
extern unsigned char 	TOut_base_flow1_flag;
/******* Modify S 101 *********/
extern unsigned char INS_cnt_OLD_Mode;					// モード
/******* Modify E 101 *********/
/******* Modify S 102 *********/
extern unsigned char Sensor_Err;
/******* Modify E 102 *********/
#endif

