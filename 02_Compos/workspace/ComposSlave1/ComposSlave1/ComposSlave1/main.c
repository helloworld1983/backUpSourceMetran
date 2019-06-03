/*******************************************************************************
*                                                                               
*    ﾌｧｲﾙ名 ：  main.c                                                          
*                                                                               
*    概要 ：                                                                    
*          メインルーチン                                                       
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

#include <m8c.h>
#include "PSoCAPI.h"    // PSoC API definitions for all User Modules
#include "clock_1_25ms.h"		// 10,100msecインターバルフラグ
#include "I2C_CNT.h"
#include "PV.h"
#include "PT.h"
#include "Flow.h"
#include "BZ.h"
#include "R_Enc.h"

/******************************************************************************
*** RAM・ROM
******************************************************************************/


/******************************************************************************
*** プログラムコード
******************************************************************************/


/******************************************************************************
##
## Name    :main
## Function:メインルーチン
## Input   :なし
## Output  :なし
## Memo    :なし
##
******************************************************************************/
void main(void)
{
	signed short flow;
	
	clock_init();
	PWM8_1_EnableInt();
	LED_1_Start();
	LED_2_Start();
		
	PGA_1_Start(PGA_1_HIGHPOWER);
	PGA_2_Start(PGA_2_HIGHPOWER);
	
	PWM8_1_Start();
	BZ_init();
	PV_init();
	Flow_init();
	PT_init();
	R_Enc_init( );
	I2C_CNT_init();
	
	
	M8C_EnableGInt;
	
	DUALADC_1_Start(DUALADC_1_HIGHPOWER);	// Turn on Analog section 
	DUALADC_1_SetResolution(12);			// Set resolution to 10 Bits 
	DUALADC_1_GetSamples(1);				// Start ADC to read 1time
	
	LED_2_Off();
	
	while(1)
	{
		//////////////////////////////////////////
		// 100msec単位で実行する処理            //
		//////////////////////////////////////////
		if(flag100ms != 0)						// シリアル送受信低レベル処理
		{
			flag100ms = 0;                      // 100msec周期フラグクリア
			
			R_Enc();
	/******* Modify S 110 *********/
			E002_timeOut();
	/******* Modify E 110 *********/
		}
		
		//////////////////////////////////////////
		// 10msec単位で実行する処理             //
		//////////////////////////////////////////
		if(flag12_5ms != 0)						//
		{
			flag12_5ms = 0;						// 10msec周期フラグクリア
			
			
			while(DUALADC_1_fIsDataAvailable() == 0);	// Wait for data to be ready 
			VF = DUALADC_1_iGetData1();					// Get Data from ADC Input1 
			VT = DUALADC_1_iGetData2ClearFlag();		// Get Data from ADC Input2 
														// and clear data ready flag 
			I2C_CNT();
			
			BZ_cnt();
			
			PT_cnt();
			Flow_cnt();
			
			PV_cnt1();
			
			DUALADC_1_Start(DUALADC_1_HIGHPOWER);	// Turn on Analog section 
			DUALADC_1_SetResolution(12);			// Set resolution to 10 Bits 
			DUALADC_1_GetSamples(1);				// Start ADC to read 1time
			
		}
		
		//////////////////////////////////////////
		// 1msec単位で実行する処理             //
		//////////////////////////////////////////
		else if(flag1_25ms != 0)						//
		{
			
			flag1_25ms = 0;						// 1msec周期フラグクリア
			
			PV_cnt2();
		}
	}
}
