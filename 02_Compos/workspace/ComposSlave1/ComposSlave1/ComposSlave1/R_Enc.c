/*******************************************************************************
*                                                                               
*    ﾌｧｲﾙ名 ：   R_Enc.c                                                        
*                                                                               
*    概要 ：                                                                    
*          ロータリーエンコーダ処理のプログラム                                 
*                                                                               
*    公開API :                                                                  
*                                                                               
*    備考 :                                                                     
*                                                                               
*                                                                               
*    変更履歴 :                                                                 
*            日  付    内    容                            Ver No.              
*            --------  ----------------------------------- -------              
*            10.04.15  初版作成                            0.00                 
*            11.12.08  1版作成                             1.00                 
*                                                                               
*******************************************************************************/

#include "PSoCAPI.h"    // PSoC API definitions for all User Modules
#include "I2C_CNT.h"	// 
#include "R_Enc.h"		// 

/******************************************************************************
*** RAM・ROM
******************************************************************************/

unsigned char countA;
unsigned char countA_old;
unsigned char countB;
unsigned char countB_old;
int R_Enc_count;

/******************************************************************************
*** プログラムコード
******************************************************************************/


/******************************************************************************
##
## Name    :R_Enc_init
## Function:ロータリーエンコーダ処理初期化
## Input   :なし
## Output  :なし
## Memo    :なし
##
******************************************************************************/
void R_Enc_init( void )
{
	countA = 0;
	countB = 0;
	countA_old = 0;
	countB_old = 0;
	R_Enc_count = 0;
	Counter8_1_Start();
	Counter8_2_Start();
}

/******************************************************************************
##
## Name    :R_Enc
## Function:ロータリーエンコーダ計数処理
## Input   :なし
## Output  :なし
## Memo    :なし
##
******************************************************************************/
void  R_Enc( void )
{
	BYTE tempA;
	BYTE tempB;
	
	tempA = Counter8_1_bReadCounter();
	tempB = Counter8_2_bReadCounter();
	
	countA = countA_old - tempA;
	countB = countB_old - tempB;
	R_Enc_count = R_Enc_count + countA - countB; 
	
	countA_old = tempA;
	countB_old = tempB;
	
	Enc = R_Enc_count;
}


