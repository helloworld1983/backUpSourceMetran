/*******************************************************************************
*                                                                               
*    ﾌｧｲﾙ名 ：   PV.h                                                        
*                                                                               
*    概要 ：                                                                    
*          比例弁出力のプログラム                                               
*                                                                               
*    公開API :                                                                  
*                                                                               
*    備考 :                                                                     
*                                                                               
*                                                                               
*    変更履歴 :                                                                 
*            日  付    内    容                            Ver No.              
*            --------  ----------------------------------- -------              
*            10.04.14  初版作成                            0.00                 
*            11.12.08  1版作成                             1.00                 
*                                                                               
*******************************************************************************/

/******************************************************************************
  定数定義
******************************************************************************/

#define Trig_init1	0
#define Trig_init2	1
#define MACHINE		2
#define MANDATORY1	3
#define MANDATORY2	4
#define MANDATORY3	5
#define ASSIST		6


/*******************************************************************************
  構造体型宣言
*******************************************************************************/


/******************************************************************************
  マクロ定義
******************************************************************************/


/*******************************************************************************
  外部宣言
*******************************************************************************/

extern void PV_init( void );
extern void PV_cnt1( void );
extern void PV_cnt2( void );
	/******* Modify S 110 *********/
extern void E002_timeOut( void );
	/******* Modify E 110 *********/

extern signed short	INSTIM_Counter;

