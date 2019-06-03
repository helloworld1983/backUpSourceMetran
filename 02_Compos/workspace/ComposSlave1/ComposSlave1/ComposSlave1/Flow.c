/*******************************************************************************
*                                                                               
*    ﾌｧｲﾙ名 ：   Flow.c                                                      
*                                                                               
*    概要 ：                                                                    
*          加速度データ処理のプログラム                                            
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
*            11.12.19  モード切替時クリア追加              1.01                
*            11.12.20  センサーエラー追加                  1.02                
*            12.02.10  アプニア対策                        1.06                 
*                                                                               
*******************************************************************************/


#include <m8c.h>        // part specific constants and macros
//#include  <math.h>      // sqrt(  ), fabs(  ) で必要
#include "PSoCAPI.h"    // PSoC API definitions for all User Modules
#include "Flow.h"

#if analog_PSOC
#include "adc.h"
#include "dac.h"
#include "switch.h"

#else
#include "PV.h"			// 

#endif
#include "I2C_CNT.h"

/******************************************************************************
  関数プロトタイプ
******************************************************************************/

void Flow_init( void );
signed short Flow_nomal_cnt( void );
void Flow_cnt( void );


/******************************************************************************
*** RAM・ROM
******************************************************************************/

#if analog_PSOC
signed short Exh_Vol;						// 呼気換気量
signed short offset;
signed short	dac1_temp;
signed short	dac2_temp;
signed short	dac3_temp;
signed short	dac4_temp;
#else
unsigned int VF;
unsigned int VF1;
signed short offset;
signed short	CAL_offset;
signed short abs;
signed short abs_p;
signed short abs_m;
unsigned char	abs_detect;
unsigned short	F_00_CAL;
unsigned short	F_03_CAL;
unsigned short	F_06_CAL;
unsigned short	F_09_CAL;
unsigned short	F_12_CAL;
unsigned short	F_15_CAL;
unsigned short	F_20_CAL;
unsigned short	F_30_CAL;
unsigned short	F_40_CAL;
unsigned short	F_50_CAL;
unsigned char	D_Cnect_state;
#endif
unsigned char	Sensor_Dir;
unsigned char	Sensor_Dir_count;
/******* Modify S 111 *********/
// signed short	Sensor_Dir_Flow;
/******* Modify E 111 *********/
unsigned char	Ins_phase;
unsigned char	Ins_phase_old;

T_FLOW_PHASE FLOW_PHASE_Inter;					// 
T_FLOW_PHASE FLOW_PHASE_old;					// 吸気呼気エラー
unsigned char INS_cnt_OLD_Mode;					// モード

unsigned char EXH_log;
unsigned char INS_log;
unsigned char ERR_log;
unsigned char insensitive_time;
unsigned char Spont;
unsigned char Spont_old;
unsigned char Spont_log;

signed long Exh_Vol_temp1;
signed long Exh_Vol_temp2;
signed long Vol_temp1;
signed long Vol_temp2;
signed long Vol;								// 吸気換気量

signed long PV_base_temp1;
signed long PV_base_temp2;
signed long PV_base;
signed long PV_base_old;
unsigned short PV_base_tim1;
unsigned short PV_base_tim2;
/******* Modify S 111 *********/
unsigned short PV11_tim1;
unsigned short PV11_tim2;
signed long PV11_vol1;
signed long PV11_vol2;
/******* Modify E 111 *********/

unsigned char No_Exh_count;
unsigned char VF_Pri_count;
unsigned char VF_Sec_count;

signed short VF_Pri[8];
signed short VF_Sec[8];


unsigned short Non_count;
unsigned short IE_tim1;
unsigned short IE_tim2;
unsigned short E_tim1;
unsigned short E_tim2;

signed long int temp_long_flow2;
signed char temp_flow_count2;
signed short temp_flow1;
signed short temp_flow2;
signed short Real_flow;
signed long int base_flow;
signed long int base_flow_tmp2;
signed long int base_flow_old;
/******* Modify S 111 *********/
//signed long int temp_long_flow3;
//signed char temp_flow_count3;
//signed short temp_flow3;
//unsigned char base_flow_state;
//signed long int ins_base_flow;
signed long int TOut_base_flow1A[9];
signed long int TOut_base_flow1B[3];
signed long int TOut_base_flow1C[4];
unsigned char TOut_base_flow1A_count;
unsigned char TOut_base_Lost_count;
/******* Modify E 111 *********/
signed long int TOut_base_flow1;
signed long int TOut_base_flow1_old;
signed long int TOut_base_flow1_old1;
signed long int TOut_base_flow1_old2;
unsigned char TOut_base_flow1_count;
unsigned char TOut_base_flow1_flag;
signed long int TOut_base_flow2;
signed long int TOut_base_flow2_old;
signed long int TOut_base_flow2_old1;
signed long int TOut_base_flow2_old2;
unsigned char TOut_base_flow2_count;
unsigned char TOut_base_flow2_flag;
signed short temp_flow_ary2[temp_flow_num2];	// 1段目
/******* Modify S 111 *********/
unsigned char TOut_base_Conv_count;
//signed short temp_flow_ary3[8];					// 1段目
/******* Modify E 111 *********/
signed short DEF_Flow;
signed short Flow_ary[8];
signed short Max_Ins_Flow_ary[4];
signed short Max_Ins_Flow;
signed short Max_Exh_Flow_ary[4];
signed short Max_Exh_Flow;


unsigned char Resp_Phase;						// 呼吸フェーズ
unsigned char Sensor_Err;
unsigned char free_count;

signed char PTD_Flag1;
signed short Press_Local;
signed short Press_Local_pri;
signed short Press_Local_sec;
signed short Press_Local_PIP;
signed short Press_Local_PEEP;

unsigned char plateau_count;
unsigned char MODE_CHG_count;
unsigned short EXH_HLD_CUNT;
/******* Modify S 112 *********/
unsigned short Free_count_offset;
/******* Modify E 112 *********/


/******************************************************************************
##
## Name    :Flow_init
## Function:フロー一回換気量処理初期化
## Input   :なし
## Output  :なし
## Memo    :なし
##
******************************************************************************/
void Flow_init( void )

{

    char i;

#if analog_PSOC
    offset = 10000;
#else
    VF = 0;
    //		offset = 0;
    offset = 10000;
    abs = 1000;
    abs_detect = 0;
#endif

    Sensor_Dir = 0;								// 正常なら0 逆なら1
    Sensor_Dir_count = 0;
    /******* Modify E 111 *********/
    //	Sensor_Dir_Flow = 0;
    /******* Modify E 111 *********/
    FLOW_PHASE_Inter.BYTE = 0;
    FLOW_PHASE_old.BYTE = 0;					// 吸気呼気エラー
    // 0:wait
    // 1:insp_start
    // 2:insp
    // 3:plateau
    // 4:exh_start
    // 5:exh
    EXH_log = 0;
    INS_log = 0;
    ERR_log = 0;
    insensitive_time = 6;			// @12.5msec
    Spont = 0;
    Spont_old = 0;
    Spont_log = 0;

    INS_cnt_OLD_Mode = 0;

    Exh_Vol = 0;								// 呼気換気量
    Non_count = 0;
    Resp_Phase = 0;								// 呼吸フェーズ
    IE_tim1 = 0;
    IE_tim2 = 0;
    E_tim1 = 0;
    E_tim2 = 0;

    Exh_Vol_temp1 = 0;
    Exh_Vol_temp2 = 0;
    Vol_temp1 = 0;
    Vol_temp2 = 0;

    PV_base_temp1 = 0;
    PV_base = 0;
    PV_base_old = 0;
    PV_base_tim1 = 0;
    PV_base_tim2 = 0;

    /******* Modify S 111 *********/
    PV11_tim1 = 0;
    PV11_tim2 = 0;
    PV11_vol1 = 0;
    PV11_vol2 = 0;
    /******* Modify E 111 *********/

    /******* Modify S 111 *********/
    //	base_flow_state = 0;
    //	ins_base_flow = 0;
    for(i = 0; i < 9; i++)
    {
        TOut_base_flow1A[i] = 0;
    }
    TOut_base_flow1A_count = 0;
    TOut_base_Lost_count = 0;
    /******* Modify E 111 *********/
    TOut_base_flow1_count = 0;
    TOut_base_flow1_old1 = 0;
    TOut_base_flow1_old2 = 0;
    TOut_base_flow1_flag = 0;
    TOut_base_flow2_count = 0;
    TOut_base_flow2_old1 = 0;
    TOut_base_flow2_old2 = 0;
    TOut_base_flow2_flag = 0;

    No_Exh_count = 0;

    temp_flow_count2 = 0;
    /******* Modify S 111 *********/
    TOut_base_Conv_count = 24;
    //	temp_flow_count3 = 0;
    /******* Modify E 111 *********/

    Sensor_Err = 0;

    free_count = 0;

    for(i = 0; i < temp_flow_num2; i++)
    {
        temp_flow_ary2[i] = 0;
    }

    /******* Modify S 111 *********/
    /*	for(i = 0; i < 3; i++)
    {
        temp_flow_ary3[i] = 0;
    }*/
    /******* Modify E 111 *********/

    VF_Pri_count = 0;
    VF_Sec_count = 0;

    for(i = 0; i < 8; i++)
    {
        VF_Pri[i] = 0;
        VF_Sec[i] = 0;
    }
    DEF_Flow = 0;
    for(i = 0; i < 8; i++)
    {
        Flow_ary[i] = 0;
    }

    /******* Modify S 111 *********/
    Max_Ins_Flow = 30;
    for(i = 0; i < 4; i++)
    {
        Max_Ins_Flow_ary[i] = 30;
    }
    //	Max_Ins_Flow = 5;
    //	for(i = 0; i < 4; i++)
    //	{
    //		Max_Ins_Flow_ary[i] = 15;
    //	}
    /******* Modify E 111 *********/

    Max_Exh_Flow = -3;
    for(i = 0; i < 4; i++)
    {
        Max_Exh_Flow_ary[i] = -15;
    }

    PTD_Flag1 = 0;
    Press_Local = 0;
    Press_Local_pri = 0;
    Press_Local_sec = 0;
    Press_Local_PIP = 0;
    Press_Local_PEEP = 0;

    Ins_phase = 0;
    Ins_phase_old = 0;

    plateau_count = 240;	// 3sec
    EXH_HLD_CUNT = 0;
    /******* Modify S 112 *********/
    Free_count_offset = 0;
    /******* Modify E 112 *********/
}

/******************************************************************************
##
## Name    :Flow_nomal_cnt
## Function:フロー正規化
## Input   :なし
## Output  :なし
## Memo    :なし
##
******************************************************************************/
signed short Flow_nomal_cnt( void )
{
    char i;
    signed short	temp_int;
    signed long int temp_int1;
    signed long int temp_int2;
    signed long int temp_int3;

    // センサーエラー
#if analog_PSOC
    if(adc1 >= 3600)						// folwセンサエラー
    {	// 4.5V以上でセンサエラー
        FLOW_PHASE_Inter.A.ERR = 1;			// エラー
        Sensor_Err = 1;
        //		adc1 = adc1 -3600;
    }
#else
    if(VF >= 3600)							// folwセンサエラー
    {	// 4.5V以上でセンサエラー
        VF = 3600; //Q_add 11.04.16
        FLOW_PHASE_Inter.A.ERR = 1;			// エラー
        Sensor_Err = 1;
    }
#endif
    else
    {
    }

    // センサーエラー
    /**************************************************************************/
    // フローの補正計算

#if analog_PSOC
    if(offset > adc1)
    {
        offset = adc1;
    }


    if(adc2 >= 2000)						// folwフラグがHレベル
    {// 方向弁別
        temp_int = (adc1 - offset ) / 1.77;		// Flow1
    }
    else
    {
        temp_int = (adc1 - offset ) / -1.77;	// Flow1
    }	// 大体1digit = 0.1単位になる。

#else
    /******* Modify S 112 *********/
#if 1
    if( PRT0DR & 0x08 )					// P0-3 -方向フラグあり(フローセンサのフラグが逆)
    {// フローの方向判別
        abs_p = VF;
    }
    else
    {
        abs_m = VF;
    }
    if((abs_p + 1> abs_m)
            && (abs_m + 1> abs_p))
    {
        abs_detect = 1;
        if(abs_p > abs_m)
        {
            abs = abs_m;
        }
        else
        {
            abs = abs_m;
        }
    }
    /*		if(offset > abs)
        {
            offset = abs;
        }*/
    if((offset > VF) && (VF > 250))
    {
        offset = VF;
        abs_detect = 1;
    }
    if(abs_detect ==1)
    {
        //			CAL_offset = offset - F_00_CAL;
        if(offset > F_00_CAL)
        {
            CAL_offset = offset - F_00_CAL;
        }
        else
        {
            offset = F_00_CAL + 10;
            CAL_offset = 0;
        }
    }
    else
    {
        CAL_offset = 0;
    }
    Free_count_offset++;
    if(Free_count_offset > 4800)	// 1分
        //		if(Free_count_offset > 24000)	// 5分
    {
        Free_count_offset = 0;
        //			abs = 1000;
        //			abs_detect = 0;
        if(abs_detect ==1)
        {
            //				if(offset > F_00_CAL)
            //				{
            //					offset = offset-3;
            //				}
            if(offset < 1000)
            {
                offset = offset+3;
            }
        }
    }
    VF1 = VF - CAL_offset;
#else
    /*		if(offset > VF)			// 電源立ち上げ時に低くて暖まって高くなる可能性あり。
        {
            offset = VF;
            CAL_offset = offset - F_00_CAL;
        }
        if(VF > CAL_offset)
        {
            VF1 = VF - CAL_offset;
        }
        else
        {
            VF1 = 0;
//			offset = 10000;
        }*/
#endif
    /******* Modify E 112 *********/
    /******* Modify S 112 *********/
    if( InsCnt.A.Mode != 0 )				// イニシャル等何もしない。
    {
        if( VF1 < F_00_CAL)					// オフセットを下回ったか？
        {
            temp_int3 = 0;
        }
        else if( VF1 >= F_40_CAL)			// 40L/min以上か？
        {
            temp_int2 = VF1 - F_40_CAL;
            temp_int1 = (F_50_CAL - F_40_CAL);	// 0.1L単位
            temp_int3 = temp_int2 * 100 / temp_int1 + 400;
        }
        else if( VF1 >= F_30_CAL)			// 30L/min以上か？
        {
            temp_int2 = VF1 - F_30_CAL;
            temp_int1 = (F_40_CAL - F_30_CAL);	// 0.1L単位
            temp_int3 = temp_int2 * 100 / temp_int1 + 300;
        }
        else if( VF1 >= F_20_CAL)			// 20L/min以上か？
        {
            temp_int2 = VF1 - F_20_CAL;
            temp_int1 = (F_30_CAL - F_20_CAL);	// 0.1L単位
            temp_int3 = temp_int2 * 100 / temp_int1 + 200;
        }
        else if( VF1 >= F_15_CAL)			// 15L/min以上か？
        {
            temp_int2 = VF1 - F_15_CAL;
            temp_int1 = (F_20_CAL - F_15_CAL);	// 0.1L単位
            temp_int3 = temp_int2 * 50 / temp_int1 + 150;
        }
        else if( VF1 >= F_12_CAL)			// 12L/min以上か？
        {
            temp_int2 = VF1 - F_12_CAL;
            temp_int1 = (F_15_CAL - F_12_CAL);	// 0.1L単位
            temp_int3 = temp_int2 * 30 / temp_int1 + 120;
        }
        else if( VF1 >= F_09_CAL)			// 9L/min以上か？
        {
            temp_int2 = VF1 - F_09_CAL;
            temp_int1 = (F_12_CAL - F_09_CAL);	// 0.1L単位
            temp_int3 = temp_int2 * 30 / temp_int1 + 90;
        }
        else if( VF1 >= F_06_CAL)			// 6L/min以上か？
        {
            temp_int2 = VF1 - F_06_CAL;
            temp_int1 = (F_09_CAL - F_06_CAL);	// 0.1L単位
            temp_int3 = temp_int2 * 30 / temp_int1 + 60;
        }
        else if( VF1 >= F_03_CAL)			// 3L/min以上か？
        {
            temp_int2 = VF1 - F_03_CAL;
            temp_int1 = (F_06_CAL - F_03_CAL);	// 0.1L単位
            temp_int3 = temp_int2 * 30 / temp_int1 + 30;
        }
        else
        {
            temp_int2 = VF1 - F_00_CAL;
            temp_int1 = (F_03_CAL - F_00_CAL);	// 0.1L単位
            temp_int3 = temp_int2 * 30 / temp_int1;
        }
        /******* Modify E 112 *********/

        if( PRT0DR & 0x08 )					// P0-3 -方向フラグあり(フローセンサのフラグが逆)
        {// フローの方向判別
            temp_int = (temp_int3);			// Flow1
            //            Flow1 = temp_int; //Q_add 11-04-16
        }
        else
        {
            temp_int = (temp_int3) * -1;	// Flow1
            //            Flow1 = -8888;
        }
    }
    else
    {
        temp_int = 0;						// Flow1
    }
#endif

    // フローの補正計算
    /**************************************************************************/
    // センサの方向判別


    // センサの方向判別
    /**************************************************************************/
    // フロー生データ64点平均計算

#if analog_PSOC
#else
    VF_Pri[VF_Pri_count] = VF;

    VF_Pri_count++;
    if( VF_Pri_count >= 8)
    {
        VF_Pri_count = 0;
        VF_Sec[VF_Sec_count] = 0;
        for(i = 0; i < 8; i++)
        {
            VF_Sec[VF_Sec_count] += VF_Pri[i];
        }
        VF_Sec[VF_Sec_count] = VF_Sec[VF_Sec_count] / 8;
        VF_Sec_count++;

        if( VF_Sec_count >= 8)
        {
            VF_Sec_count = 0;
            Flow_RAW = (VF_Sec[0] + VF_Sec[1] + VF_Sec[2] + VF_Sec[3]
                    + VF_Sec[4] + VF_Sec[5] + VF_Sec[6] + VF_Sec[7]) / 8;
            if( PRT0DR & 0x08 )				// P0-3 -方向フラグあり(フローセンサのフラグが逆)
            {// フローの方向判別
                Flow_RAW = (Flow_RAW) * -1;
                // 吸気が本来は＋であるが、
                // キャリブレーション時に処理に困るので、
                // この場合だけ呼気を＋にした。
            }
        }
    }
#endif

    // フロー生データ64点平均
    return temp_int;
}

/******************************************************************************
##
## Name    :Flow_cnt
## Function:フロー一回換気量処理
## Input   :なし
## Output  :なし
## Memo    :なし
##
******************************************************************************/

void Flow_cnt( void )
{
    char i;
    signed short	temp_int;
    //	signed long int temp_int1;
    //	signed long int temp_int2;
    //	signed long int temp_int3;

    free_count++;
    if(free_count > 20)
        free_count = 0;

    /**************************************************************************/
    // モード切替時クリア
    if(INS_cnt_OLD_Mode != InsCnt.A.Mode)
    {
        MODE_CHG_count = 240;
    }
    if(MODE_CHG_count > 0)
    {
        MODE_CHG_count--;
        /******* Modify S 111 *********/
        //		base_flow = temp_flow2;
        //		base_flow_old = temp_flow2;
        base_flow_tmp2 = 0;
        //		base_flow_tmp2 = temp_flow2;
        PV_base = 0;
        PV_base_old = 0;
        /******* Modify E 111 *********/
    }
    // モード切替時クリア
    /**************************************************************************/
    // 圧トリガフェイズ

    /******* Modify S 111 *********/
    /*	Press_Local_pri =  Press_Local_sec;
    Press_Local_sec = Press_Local;

    #if analog_PSOC
        Press_Local = 0;
//		Press_Local = (adc3 -694) * 10 / 35.8;	// 0.1cmH2O
    #else
        Press_Local = PTm;
    #endif

    if(PTD_Flag1 == 0)
    {
        if(Press_Local_pri + 20 <= Press_Local_sec)		// 2cmH2O以上高くなる
        {
            PTD_Flag1 = 1;
        }
        else
        {
            Press_Local_PIP = Press_Local;
            Press_Local_PEEP = Press_Local;
        }
    }
    else if(PTD_Flag1 == 1)
    {
        if(Press_Local_pri >= Press_Local_sec + 20)		// 2cmH2O以上低くなる
        {
            PTD_Flag1 = 2;
        }
        if (Press_Local_PIP < Press_Local)		// 最大値
            Press_Local_PIP = Press_Local;
    }
    else if((PTD_Flag1 == 2)
     && (Press_Local < (Press_Local_PIP + Press_Local_PEEP)/ 2))	// PEEPとPIPの中央より下か？
    {
        PTD_Flag1 = 3;
    }
    else if(PTD_Flag1 == 3)
    {
        PTD_Flag1 = 4;
    }
    else if(PTD_Flag1 == 4)
    {
        PTD_Flag1 = 5;
    }
    else if(PTD_Flag1 == 5)
    {
        PTD_Flag1 = 0;
    }*/
    /******* Modify E 111 *********/

    // 圧トリガフェイズ
    /**************************************************************************/
    // 比例弁呼フェイズ取り込み

#if analog_PSOC

    if(SW_IN.Ins_phase == 1 )
    {
        Ins_phase = 1;
    }
    else if((adc3 > 2000)
            && (Ins_phase_old != 1))
    {
        Ins_phase = 0;
    }
    else if((SW_IN.Ins_phase != 1 )
            && (adc3 < 2000))
    {
        Ins_phase = 2;
    }
    if(SW_IN.FIX.BIT.SW_4 == 1)
    {
        InsCnt.A.Mode = 2;					// JET OFF、バックアップ中
    }
    else
    {
        InsCnt.A.Mode = 0;					// JET OFF、バックアップ中
    }
#else
    if(( InsCnt.A.Phase >= Ins_Strt )		// 0	吸気開始
            && ( InsCnt.A.Phase <= Ins_End ))		// 3	吸気終末
    {
        Ins_phase = 1;
    }
    else if (InsCnt.A.Phase == Trig_View2)
    {
        Ins_phase = 0;
    }
    else if (InsCnt.A.Phase == Trig_View)
    {
        Ins_phase = 0;
    }
    else
    {
        Ins_phase = 2;
    }
#endif

    // 比例弁呼フェイズ取り込み
    /**************************************************************************/
    // トリガマスク


    // トリガマスク
    /**************************************************************************/
    // フロー読み込み
    temp_int = Flow_nomal_cnt();
    // フロー読み込み
    /**************************************************************************/
    // 24点移動平均・ベースフロー

    temp_long_flow2 = 0;
    temp_flow_ary2[temp_flow_count2] = temp_int;
    for(i = 0; i < temp_flow_num2; i++)
    {
        temp_long_flow2 += temp_flow_ary2[i];
    }

    temp_flow2 = temp_long_flow2 / 24;			// 0.1L単位 & 正規化
    temp_flow_count2++;
    Real_flow = temp_flow2;						// PV回路外れで使っている。
    if( temp_flow_count2 >= temp_flow_num2)
    {
        temp_flow_count2 = 0;
    }

    // 24点移動平均・ベースフロー
    /**************************************************************************/
    // 8点移動平均・ベースフロー

    /******* Modify S 111 *********/
    /*	temp_long_flow3 = 0;
    temp_flow_ary3[temp_flow_count3] = temp_int;
    for(i = 0; i < 8; i++)
    {
        temp_long_flow3 += temp_flow_ary2[i];
    }

    temp_flow3 = temp_long_flow3 / 8;			// 0.1L単位 & 正規化
    temp_flow_count3++;
    Real_flow = temp_flow3;						// PV回路外れで使っている。
    if( temp_flow_count3 >= 8)
    {
        temp_flow_count3 = 0;
    }*/
    /******* Modify E 111 *********/

    // 8点移動平均・ベースフロー
    /**************************************************************************/
    // 波形差異

    Flow_ary[7] = Flow_ary[6];
    Flow_ary[6] = Flow_ary[5];
    Flow_ary[5] = Flow_ary[4];
    Flow_ary[4] = Flow_ary[3];
    Flow_ary[3] = Flow_ary[2];
    Flow_ary[2] = Flow_ary[1];
    Flow_ary[1] = Flow_ary[0];
    Flow_ary[0] = temp_int - base_flow;

    DEF_Flow = (Flow_ary[0] +Flow_ary[1] +Flow_ary[2]);
    /******* Modify S 111 *********/
    //	Sensor_Dir_Flow = (Flow_ary[0] +Flow_ary[1] +Flow_ary[2]  +Flow_ary[3]
    //		+Flow_ary[4] +Flow_ary[5]  +Flow_ary[6]  +Flow_ary[7] ) / 8;
    /******* Modify E 111 *********/

    // 波形差異
    /**************************************************************************/
    // 8点移動平均

    temp_flow1 = (Flow_ary[0] +Flow_ary[1] +Flow_ary[2]  +Flow_ary[3]
            +Flow_ary[4] +Flow_ary[5]  +Flow_ary[6]  +Flow_ary[7] ) / 8;

    // 8点移動平均
    /**************************************************************************/
    // 吸気呼気検出

    /******* Modify S 111 *********/
    ////	if(InsCnt.A.Mode == 2)
    //	if(((InsCnt.A.Mode == 2)
    //	 && (Trgt_PresB >= 1))
    //	 || (InsCnt.A.Mode == 5))
    if(((InsCnt.A.Mode == 2)					// JET OFF、バックアップ中
        && (Trgt_PresB >= 1))
            || ((InsCnt.A.Mode == 6)					// パーカッション
                && (Trgt_PresB >= 1))
            || (InsCnt.A.Mode == 5))
        /******* Modify E 111 *********/
    {	// 強制換気
//        Flow1 = 8888; //Q_add 11.4.16 In mandatory mode, jump to this case
        if(Resp_Phase == INS_START)
        {
            Resp_Phase = INS_MID;
        }
        if(Resp_Phase == EXH_START)
        {
            Resp_Phase = EXH_MID;
            EXH_HLD_CUNT = Tim_PIP2A;					//
        }

        if( InsCnt.A.Phase == Ins_Strt )		// 0	吸気開始
        {
            if(Resp_Phase != INS_MID)
            {
                Resp_Phase = INS_START;
            }
        }
        if( InsCnt.A.Phase == Exh_Strt )
        {
            Resp_Phase = EXH_START;
        }
        /******* Modify S 111 *********/
        if(( DEF_Flow > Max_Exh_Flow)
                && (Resp_Phase == EXH_MID)
                && (EXH_HLD_CUNT == 0))
        {
            Resp_Phase = NON;
        }
        /******* Modify E 111 *********/
    }
    else
    {	// モニタ
//        Flow1 = -1000; //Q_add 11.4.16 In monitor and backup mode, jump to this case
        if(Resp_Phase == INS_START)
        {
            Resp_Phase = INS_MID;
        }
        if(Resp_Phase == EXH_START)
        {
            Resp_Phase = EXH_MID;
        }

        if( DEF_Flow >= Max_Ins_Flow)
        {
            if((Resp_Phase == NON)
                    || (Resp_Phase == EXH_MID))
            {
                /******* Modify S 111 *********/
                //				Resp_Phase = INS_START;
                if(Resp_Phase != INS_START)
                {
                    Resp_Phase = INS_START;
                }
                /******* Modify E 111 *********/
            }
        }
        else
        {
            if(Resp_Phase == INS_MID)
            {
                Resp_Phase = PLATEAU;
                plateau_count = 240;
            }
        }

        if( DEF_Flow <= Max_Exh_Flow)
        {
            if(Resp_Phase == INS_MID)
            {
                Resp_Phase = EXH_START;
            }
            else if(Resp_Phase == PLATEAU)
            {
                Resp_Phase = EXH_START;
            }
            else if(Resp_Phase == NON)
            {
                Resp_Phase = EXH_START;
            }
            EXH_HLD_CUNT = 16;					// 200msec
        }
        else if(EXH_HLD_CUNT > 0)
        {
            Resp_Phase = EXH_MID;
        }
        else
        {
            if(Resp_Phase == EXH_MID)
            {
                Resp_Phase = NON;
            }
        }
        if(Resp_Phase == PLATEAU)
        {
            if(plateau_count > 0)
            {
                plateau_count--;
            }
            else
            {
                Resp_Phase = NON;
            }
        }
    }
    if(EXH_HLD_CUNT > 0)
    {
        EXH_HLD_CUNT--;
    }

    // 吸気呼気検出
    /**************************************************************************/
    // 吸気フラグ

    if(( Resp_Phase == INS_START ) || ( Resp_Phase == INS_MID ))
    {
        FLOW_PHASE_Inter.A.INS = 1;				// 吸気
    }
    else
    {
        FLOW_PHASE_Inter.A.INS = 0;				// 吸気
    }

    // 吸気フラグ
    /**************************************************************************/
    // 呼気フラグ

    if(( Resp_Phase == EXH_START ) || ( Resp_Phase == EXH_MID ))
    {
        FLOW_PHASE_Inter.A.EXH = 1;				// 呼気
    }
    else
    {
        FLOW_PHASE_Inter.A.EXH = 0;				// 呼気
    }

    // 呼気フラグ
    /**************************************************************************/
    // 最高吸気流量測定


    if(INS_cnt_OLD_Mode != InsCnt.A.Mode)		// モード切替時クリア
    {
        /******* Modify S 111 *********/
        //		Max_Ins_Flow = 15;
        //		Max_Ins_Flow = 30;
        Max_Ins_Flow = INS_Trig * 3;
        //		Max_Ins_Flow = 5;
        /******* Modify E 111 *********/
        for(i = 0; i < 4; i++)
        {
            /******* Modify S 111 *********/
            Max_Ins_Flow_ary[i] = INS_Trig * 3;
            //			Max_Ins_Flow_ary[i] = 30;
            //			Max_Ins_Flow_ary[i] = 15;
            /******* Modify E 111 *********/
        }
    }
    /******* Modify S 111 *********/
    if( 1 )							// トリガ感度オート
        //	if( INS_Trig == 0)							// トリガ感度オート
    {
        if( Resp_Phase == EXH_START)
        {
            Max_Ins_Flow_ary[3] = Max_Ins_Flow_ary[2];
            Max_Ins_Flow_ary[2] = Max_Ins_Flow_ary[1];
            Max_Ins_Flow_ary[1] = Max_Ins_Flow_ary[0];
            Max_Ins_Flow_ary[0] = temp_flow1;

            Max_Ins_Flow = Max_Ins_Flow_ary[1];
            if (Max_Ins_Flow < Max_Ins_Flow_ary[2])			// 最大値
                Max_Ins_Flow = Max_Ins_Flow_ary[2];
            if (Max_Ins_Flow < Max_Ins_Flow_ary[3])			// 最大値
                Max_Ins_Flow = Max_Ins_Flow_ary[3];

            /******* Modify S 111 *********/
            //			if (Max_Ins_Flow < 15)				// 最小値
            //				Max_Ins_Flow = 15;
            //			if (Max_Ins_Flow < 30)				// 最小値
            //				Max_Ins_Flow = 30;
            if (Max_Ins_Flow < INS_Trig * 3)				// 最小値
                Max_Ins_Flow = INS_Trig * 3;
            if (Max_Ins_Flow > 400)				// 最大値
                Max_Ins_Flow = 400;
            //			if (Max_Ins_Flow > 200)				// 最大値
            //				Max_Ins_Flow = 200;
            /******* Modify E 111 *********/

        }
        else
        {
            if (Max_Ins_Flow_ary[0] < temp_flow1)	// 最大値
                Max_Ins_Flow_ary[0] = temp_flow1;

            if(free_count == 0)
            {
                /******* Modify S 111 *********/
#if 1
                if( Resp_Phase == NON )
                {
                    Max_Ins_Flow = Max_Ins_Flow - 50;

                    if (Max_Ins_Flow < INS_Trig * 3)
                    {
                        Max_Ins_Flow = INS_Trig * 3;
                    }
                    //					if (Max_Ins_Flow < 30)
                    //					{
                    //						Max_Ins_Flow = 30;
                    //					}
                }
#else
                if (Max_Ins_Flow > 30)			// 最小値
                    Max_Ins_Flow--;
#endif
                /******* Modify E 111 *********/
            }
        }
    }
    else
    {
        Max_Ins_Flow = INS_Trig * 3;
    }
    /******* Modify E 111 *********/

    // 最高吸気流量測定
    /**************************************************************************/
    // 最小呼気流量測定

    if(INS_cnt_OLD_Mode != InsCnt.A.Mode)		// モード切替時クリア
    {
        Max_Exh_Flow = -3;
        for(i = 0; i < 4; i++)
        {
            Max_Exh_Flow_ary[i] = -15;
        }
    }
    if( Resp_Phase == INS_START)
    {
        Max_Exh_Flow_ary[3] = Max_Exh_Flow_ary[2];
        Max_Exh_Flow_ary[2] = Max_Exh_Flow_ary[1];
        Max_Exh_Flow_ary[1] = Max_Exh_Flow_ary[0];
        Max_Exh_Flow_ary[0] = temp_flow1;

        Max_Exh_Flow = Max_Exh_Flow_ary[1];
        if (Max_Exh_Flow > Max_Exh_Flow_ary[2])			// 最小値
            Max_Exh_Flow = Max_Exh_Flow_ary[2];
        if (Max_Exh_Flow > Max_Exh_Flow_ary[3])			// 最小値
            Max_Exh_Flow = Max_Exh_Flow_ary[3];

        if (Max_Exh_Flow > -15)					// 最大値
            Max_Exh_Flow = -15;
        if (Max_Exh_Flow < -30)					// 最小値
            Max_Exh_Flow = -30;

    }
    else
    {
        if (Max_Exh_Flow_ary[0] > temp_flow1)	// 最小値
            Max_Exh_Flow_ary[0] = temp_flow1;

        if(free_count == 0)
        {
            if (Max_Exh_Flow < -15)				// 最大値
                Max_Exh_Flow++;
        }
    }

    // 最小呼気流量測定
    /**************************************************************************/
    // 呼吸時間・呼気時間計測

    if( Resp_Phase == INS_START )
    {
        IE_tim2 = IE_tim1;
        E_tim2 = E_tim1;

        E_tim1 = 1;
        IE_tim1 = 1;
    }
    else
    {
        IE_tim1++;
    }
    if( Resp_Phase == EXH_START )
    {
        E_tim1 = 1;
    }
    else if(( Resp_Phase == EXH_MID )
            || ( Resp_Phase == NON ))
    {
        E_tim1++;
    }

    // 呼吸時間・呼気時間計測
    /**************************************************************************/
    // 一回換気量計測

    /******* Modify S 111 *********/
    ////	if(InsCnt.A.Mode == 2)
    //	if(((InsCnt.A.Mode == 2)
    //	 && (Trgt_PresB >= 1))
    //	 || (InsCnt.A.Mode == 5))
    if(((InsCnt.A.Mode == 2)					// JET OFF、バックアップ中
        && (Trgt_PresB >= 1))
            || ((InsCnt.A.Mode == 6)					// パーカッション
                && (Trgt_PresB >= 1))
            || (InsCnt.A.Mode == 5))
        /******* Modify E 111 *********/
    {
        if( Resp_Phase == INS_START )
        {

            if( Sensor_Err == 1)
            {
                Exh_Vol_temp2 = (Exh_Vol_temp1 + base_flow *  E_tim2 ) * (VOL_RATE + 100) /4560;// 5.7*8;
//                Flow1 = VOL_RATE; //Q_add 12.04.16 VOL_RATE = 18
            }
            else
            {
                Exh_Vol_temp2 = (Exh_Vol_temp1 + Vol_temp1 *  E_tim2 / IE_tim2) * (VOL_RATE + 100) /4560;// 5.7*8;
//                Flow1 = VOL_RATE; //Q_add 12.04.16 VOL_RATE = 18
            }
            Vol_temp1 = temp_int;
            Exh_Vol_temp1 = 0;
        }
        else
        {
            Vol_temp1 = Vol_temp1 + temp_int;
        }
        if( Resp_Phase == EXH_START )
        {
            Flow1 = temp_int; //Q_add 07.04.16
            if (temp_int <= 0) {
                Exh_Vol_temp1 =  - temp_int;
            } else {
                Exh_Vol_temp1 = temp_int;
            }
        }
        else if( Resp_Phase == EXH_MID )
        {
            Flow1 = temp_int; //Q_add 07.04.16
            if (temp_int <= 0) {
                Exh_Vol_temp1 = Exh_Vol_temp1 - temp_int;
            } else {
                if (temp_int < 700) { //Base on Experience ignore Flow > 70L
                    Exh_Vol_temp1 = Exh_Vol_temp1 + temp_int;
                }
            }
        }
        else if( Resp_Phase == NON )
        {
//            Flow1 = temp_int; //Q_add 07.04.16
            Exh_Vol_temp1 = Exh_Vol_temp1 - temp_int;
        }

        /******* Modify S 111 *********/
        //		if(InsCnt.A.Phase == Ins_Strt)		// 吸気開始
        //		if(Resp_Phase == INS_START)
        if((Ins_phase_old == 0)					// 吸気開始
                && (Ins_phase != 0))
            /******* Modify E 111 *********/
        {
            PV_base_tim2 = PV_base_tim1;
            PV_base_temp2 = PV_base_temp1;

            PV_base = PV_base_temp2 / PV_base_tim2;
            PV_base_temp1 = temp_int;
            PV_base_tim1 = 1;
            /******* Modify S 111 *********/
            //			ins_base_flow = PV_base;
            /******* Modify E 111 *********/

            //		LED_2_On();
        }
        else
        {
            PV_base_tim1++;

            PV_base_temp1 = PV_base_temp1 + temp_int;
            //		LED_2_Off();
        }
    }
    else
    {
        if( Resp_Phase == INS_START )
        {

            if( Sensor_Err == 1)
            {
                Exh_Vol_temp2 = (Exh_Vol_temp1 + base_flow *  E_tim2 ) * (VOL_RATE + 100) /4560;// 5.7*8;
            }
            else
            {
                Exh_Vol_temp2 = (Exh_Vol_temp1 + Vol_temp1 *  E_tim2 / IE_tim2) * (VOL_RATE + 100) /4560;// 5.7*8;
            }
            Vol_temp1 = temp_int;
            Exh_Vol_temp1 = 0;
        }
        else
        {
            Vol_temp1 = Vol_temp1 + temp_int;
        }


        if( Resp_Phase == EXH_START )
        {
            Exh_Vol_temp1 =  - temp_int;
        }
        else if( Resp_Phase == EXH_MID )
        {
            Exh_Vol_temp1 = Exh_Vol_temp1 - temp_int;
        }
        else if( Resp_Phase == NON )
        {
            Exh_Vol_temp1 = Exh_Vol_temp1 - temp_int;
        }
    }
    // 一回換気量計測
    /**************************************************************************/
    // 回路外れフロー検出
    /******* Modify S 111 *********/
    if((InsCnt.A.Mode == 1)
            || (InsCnt.A.Mode == 2)					// JET OFF、バックアップ中 //Q_comment backup
            || (InsCnt.A.Mode == 5)
            || (InsCnt.A.Mode == 6))					// パーカッション
    {
        if( Resp_Phase == INS_START )
            //		if( InsCnt.A.Phase == Ins_Strt)
        {
            PV11_vol2 = PV11_vol1 / Tim_PIP2B / 2;
            PV11_tim1 = 479;
            PV11_tim2 = 1;
            PV11_vol1 = temp_int;
            // Q_add 11.04.16 remove check alarm when PIP > 40
            if(Trgt_PresB <= 40) {
                if(PV11_vol2 > 60+6*Trgt_PresB)
                {
                    PSoC1.DATA.ERR.BIT.FLW_DCnct = 1;
                }
                else if(PV11_vol2 > 300)
                {
                    PSoC1.DATA.ERR.BIT.FLW_DCnct = 1;
                }
                else
                {
                    PSoC1.DATA.ERR.BIT.FLW_DCnct = 0;
                }
            }
        }
        else if(PV11_tim1 > 0)
        {
            PV11_vol1 = PV11_vol1 + temp_int;
            PV11_tim1--;
            PV11_tim2++;
        }
        else
        {
        }
    }
    else
    {
        PV11_tim1 = 0;
        PV11_tim2 = 0;
        PV11_vol1 = 0;
        PV11_vol2 = 0;
        PSoC1.DATA.ERR.BIT.FLW_DCnct = 0;
    }
    /******* Modify E 111 *********/


    // 回路外れフロー検出
    /**************************************************************************/
    // 一回換気量表示更新


    if( Resp_Phase == INS_START )
    {
        //	テスト肺を交換すると換気量が更新されなくなる。
        //Q_remove 11.04.16
        //        if( Exh_Vol_temp2  >= 8)
        //        {
        Exh_Vol = Exh_Vol_temp2 ;
        No_Exh_count = 0;
        //        }
    }

    if(INS_cnt_OLD_Mode != InsCnt.A.Mode)
    {
        Exh_Vol = 0;
    }


    // 一回換気量表示更新
    /**************************************************************************/
    // 無呼吸ベースフロー1
    /******* Modify S 111 *********/
    if(( Resp_Phase == EXH_START )
            || ( Resp_Phase == EXH_MID )
            || ( Resp_Phase == NON ))
        //	if( Resp_Phase == NON )
        /******* Modify E 111 *********/
    {
        if((TOut_base_flow1		 >= temp_flow2 - 1)
                && (TOut_base_flow1	 <= temp_flow2 + 1)
                && (TOut_base_flow1_old >= temp_flow2 - 1)
                && (TOut_base_flow1_old <= temp_flow2 + 1)
                && (temp_flow2 >= -100)
                && (temp_flow2 <= 0))
        {
            TOut_base_flow1_count++;
            /******* Modify S 112 *********/
            TOut_base_flow1_old2 = TOut_base_flow1;
            /******* Modify E 112 *********/
        }
        else
        {
            TOut_base_flow1_count = 0;
        }
        TOut_base_flow1_old = TOut_base_flow1;
        TOut_base_flow1 = temp_flow2;

        /******* Modify S 111 *********/
        if(TOut_base_flow1_count == TOut_base_Conv_count)	//
        {
            //		if(TOut_base_flow1_count >= 8*2-2)	//
            //		{
            //			TOut_base_flow1_count = 8*2;
            /******* Modify E 111 *********/
            /******* Modify S 112 *********/
            //			TOut_base_flow1_old2 = TOut_base_flow1;
            /******* Modify E 112 *********/
            TOut_base_flow1_flag = 1;
        }
        else if(TOut_base_flow1_count > TOut_base_Conv_count)	//
        {
            TOut_base_flow1_count = TOut_base_Conv_count;
        }
    }

    // 無呼吸ベースフロー1
    /**************************************************************************/
    // 無呼吸ベースフロー2
    /******* Modify S 111 *********/
    /*	if( Resp_Phase == NON )
    {
        if((TOut_base_flow2 >= temp_flow3 - 7)
         && (TOut_base_flow2 <= temp_flow3 + 7)
         && (TOut_base_flow2_old >= temp_flow3 - 7)
         && (TOut_base_flow2_old <= temp_flow3 + 7)
         && (temp_flow3 >= -100)
         && (temp_flow3 <= 0))
        {
            TOut_base_flow2_count++;
        }
        else
        {
            TOut_base_flow2_count = 0;
        }
        TOut_base_flow2_old = TOut_base_flow2;
        TOut_base_flow2 = temp_flow2;

        if(TOut_base_flow2_count >= 2)	// 0.2秒
        {
            TOut_base_flow2_count = 2;
            TOut_base_flow2_old2 = TOut_base_flow2;
            TOut_base_flow2_flag = 1;
        }
        else
        {
        }
    }*/
    /******* Modify E 111 *********/

    // 無呼吸ベースフロー2
    /**************************************************************************/
    // モニター用ベースフロー判定
    //　108で強制換気19cm設定以下でモニター用のアルゴリズムを使用
    /******* Modify S 111 *********/
    if(temp_flow_count2 == 0)		// 24
        //	if(temp_flow_count3 == 0)		// 8
        /******* Modify E 111 *********/
    {
        //		if((base_flow_tmp2 >= temp_flow2 - 5)
        //		 && (base_flow_tmp2 <= temp_flow2 + 5))
        if((base_flow_tmp2 >= temp_flow2 - 3)	// 5→3で調子が良くなった。
                && (base_flow_tmp2 <= temp_flow2 + 3))	// ただし反応が遅い。
        {
            base_flow_tmp2 = temp_flow2;
        }
        else if(base_flow_tmp2 > temp_flow2)
        {
            base_flow_tmp2--;
        }
        else
        {
            base_flow_tmp2++;
        }

    }
    // モニター用ベースフロー判定
    /**************************************************************************/
    // ベースフロー判定
    /******* Modify S 111 *********/
    ////	if(InsCnt.A.Mode == 2)
    //	if(((InsCnt.A.Mode == 2)
    //	 && (Trgt_PresB >= 1))
    //	 || (InsCnt.A.Mode == 5))					// 強制換気
    if(((InsCnt.A.Mode == 2)					// JET OFF、バックアップ中
        && (Trgt_PresB >= 1))
            || ((InsCnt.A.Mode == 6)					// パーカッション
                && (Trgt_PresB >= 1))
            || (InsCnt.A.Mode == 5))
        /******* Modify E 111 *********/
    {
        if( Resp_Phase == INS_START )
        {
            /******* Modify S 111 *********/
            if(TOut_base_flow1_flag == 1)
            {
                TOut_base_flow1A[TOut_base_flow1A_count] = TOut_base_flow1_old2 + 0;
                TOut_base_flow1A_count++;
                if (TOut_base_flow1A_count >= 9)
                {
                    TOut_base_flow1A_count = 0;
                }
            }
            for(i = 0; i < 3; i++)
            {
                if(TOut_base_flow1A[0+i*3] < TOut_base_flow1A[1+i*3])
                {
                    TOut_base_flow1B[0] = TOut_base_flow1A[0+i*3];
                    TOut_base_flow1B[1] = TOut_base_flow1A[1+i*3];
                }
                else
                {
                    TOut_base_flow1B[1] = TOut_base_flow1A[0+i*3];
                    TOut_base_flow1B[0] = TOut_base_flow1A[1+i*3];
                }

                if(TOut_base_flow1B[0] > TOut_base_flow1A[2])
                {
                    TOut_base_flow1B[0] = TOut_base_flow1A[2+i*3];
                    TOut_base_flow1B[1] = TOut_base_flow1A[0+i*3];
                    TOut_base_flow1B[2] = TOut_base_flow1A[1+i*3];
                }
                else
                {
                    TOut_base_flow1B[2] = TOut_base_flow1A[2+i*3];
                }
                if(TOut_base_flow1B[1] < TOut_base_flow1B[2])
                {
                    TOut_base_flow1C[i] = TOut_base_flow1B[1];
                }
                else
                {
                    TOut_base_flow1C[i] = TOut_base_flow1B[2];
                }
            }
            if(TOut_base_flow1C[0] < TOut_base_flow1C[1])
            {
                TOut_base_flow1B[0] = TOut_base_flow1C[0];
                TOut_base_flow1B[1] = TOut_base_flow1C[1];
            }
            else
            {
                TOut_base_flow1B[1] = TOut_base_flow1C[0];
                TOut_base_flow1B[0] = TOut_base_flow1C[1];
            }

            if(TOut_base_flow1B[0] > TOut_base_flow1C[2])
            {
                TOut_base_flow1B[0] = TOut_base_flow1C[2];
                TOut_base_flow1B[1] = TOut_base_flow1C[0];
                TOut_base_flow1B[2] = TOut_base_flow1C[1];
            }
            else
            {
                TOut_base_flow1B[2] = TOut_base_flow1C[2];
            }
            if(TOut_base_flow1B[1] < TOut_base_flow1B[2])
            {
                TOut_base_flow1C[3] = TOut_base_flow1B[1];
            }
            else
            {
                TOut_base_flow1C[3] = TOut_base_flow1B[2];
            }

            if(TOut_base_flow1_flag == 1)
            {
                //				base_flow_state = 1;
                base_flow = TOut_base_flow1C[3];
                //				base_flow = TOut_base_flow1_old2;
                //				base_flow_old = base_flow;

                TOut_base_Conv_count++;
                if(TOut_base_Conv_count > 24)
                    TOut_base_Conv_count = 24;
                TOut_base_Lost_count = 0;
            }

            else if(Sensor_Err == 0)
            {
                TOut_base_Lost_count++;
                if(TOut_base_Lost_count > 6)
                {
                    base_flow = PV_base;
                    //					base_flow_old = base_flow;
                }
                TOut_base_Conv_count--;
                if(TOut_base_Conv_count < 8)
                    TOut_base_Conv_count = 8;
            }
            else
            {
                //				base_flow_state = 7;
                //				base_flow = base_flow_old;
            }
            if(base_flow > 0)
            {
                //				base_flow = 0;
            }

            /******* Modify E 111 *********/
        }
    }
    else		// モニター用
    {
        if( Resp_Phase == INS_START )
        {
            base_flow = base_flow_tmp2;
            /******* Modify S 111 *********/
            TOut_base_flow1A[TOut_base_flow1A_count] = base_flow_tmp2;
            TOut_base_flow1A_count++;
            if (TOut_base_flow1A_count >= 9)
            {
                TOut_base_flow1A_count = 0;
            }
            /******* Modify E 111 *********/
        }
    }
    // ベースフロー判定
    /**************************************************************************/
    // ベースフロー更新用タイムアウト

    if( Resp_Phase == INS_START )
    {
        Non_count = 0;
    }
    else
    {
        Non_count++;
    }
    if(Non_count >= 480)						// 6秒以上吸気がないとき //Q_comment no breath >6s
    {
        base_flow = temp_flow2;
        Non_count = 0;
        /******* Modify S 111 *********/
        TOut_base_flow1A[TOut_base_flow1A_count] = temp_flow2;
        TOut_base_flow1A_count++;
        if (TOut_base_flow1A_count >= 9)
        {
            TOut_base_flow1A_count = 0;
        }
        /******* Modify E 111 *********/
    }

    // ベースフロー更新用タイムアウト
    /**************************************************************************/
    // エラー解除
    if(Resp_Phase == INS_START)
    {
        Sensor_Err = 0;
    }
    /******* Modify S 111 *********/
    else if(Resp_Phase == INS_MID)
        //	else if(Resp_Phase == PLATEAU)
        /******* Modify E 111 *********/
    {
        TOut_base_flow1_flag = 0;
        TOut_base_flow2_flag = 0;
    }
    if(INS_cnt_OLD_Mode != InsCnt.A.Mode)
    {
        Sensor_Err = 0;
    }
    // エラー解除
    /**************************************************************************/
    // 出力
#if analog_PSOC
    //		dac1_temp = Max_Ins_Flow;//  / 2;
    dac1_temp = temp_int;//  / 2;
    dac2_temp = DEF_Flow / 3;//  / 6;
    dac3_temp = base_flow;//  / 2;
    //		dac3_temp = Exh_Vol_temp1 / 45.6 / 2;

    if(SW_IN.FIX.BIT.SW_1 == 1)
    {
        dac1 = dac1_temp / 125;
        dac2 = dac2_temp / 125;// - 31;
        dac3 = dac3_temp / 125;				// 15.0L / 30bit
    }
    else if(SW_IN.FIX.BIT.SW_2 == 1)
    {
        dac1 = dac1_temp / 25;
        dac2 = dac2_temp / 25;// - 31;
        dac3 = dac3_temp / 25;				// 15.0L / 30bit
    }
    else if(SW_IN.FIX.BIT.SW_3 == 1)
    {
        dac1 = dac1_temp / 5;
        dac2 = dac2_temp / 5;// - 31;
        dac3 = dac3_temp / 5;				// 15.0L / 30bit
    }
    else
    {
        dac1 = dac1_temp;//  / 2;
        dac2 = dac2_temp;//  / 2;// - 31;
        dac3 = dac3_temp;//  / 2;			// 15.0L / 30bit
    }

    dac4 = Resp_Phase * 8 -15;				// 通常
    //		dac4 = Ins_phase * 8 -15;

    if(TOut_base_flow2_flag == 1)
    {
        LED_3_On();
    }
    else
    {
        LED_3_Off();
    }

    if(TOut_base_flow1_flag == 1)
    {
        LED_4_On();
    }
    else
    {
        LED_4_Off();
    }

    if( Sensor_Err == 1)
    {
        LED_5_On();
    }
    else
    {
        LED_5_Off();
    }

#else

    if( FLOW_PHASE_Inter.A.EXH )			// P1-6 スイッチ入力あり
    {
        EXH_log = EXH_log << 1 | 1;			// 左１ビットシフトで最下位ビット１
    }
    else									// スイッチ入力なし
    {
        EXH_log = EXH_log << 1;				// 左１ビットシフトで最下位ビット０
    }

    if( FLOW_PHASE_Inter.A.INS )			// P1-6 スイッチ入力あり
    {
        INS_log = INS_log << 1 | 1;			// 左１ビットシフトで最下位ビット１
    }
    else									// スイッチ入力なし
    {
        INS_log = INS_log << 1;				// 左１ビットシフトで最下位ビット０
    }

    if( FLOW_PHASE_Inter.A.ERR )			// P1-6 スイッチ入力あり
    {
        ERR_log = ERR_log << 1 | 1;			// 左１ビットシフトで最下位ビット１
    }
    else									// スイッチ入力なし
    {
        ERR_log = ERR_log << 1;				// 左１ビットシフトで最下位ビット０
    }

    if(EXH_log != 0)
    {
        FLOW_PHASE.A.EXH = 1;
    }
    else
    {
        FLOW_PHASE.A.EXH = 0;
    }

    if(INS_log != 0)
    {
        FLOW_PHASE.A.INS = 1;
    }
    else
    {
        FLOW_PHASE.A.INS = 0;
    }

    if(ERR_log != 0)
    {
        FLOW_PHASE.A.ERR = 1;
    }
    else
    {
        FLOW_PHASE.A.ERR = 0;
    }

    if( DEF_Flow <= Max_Exh_Flow)		// 呼気終了後の揺らぎ
    {
        insensitive_time = 8;			// @12.5msec
    }
    else if( FLOW_PHASE_Inter.A.INS == 1)		// 吸気が終わって少しタイムラグを持たせる
    {
        insensitive_time = 8;			// @12.5msec
    }
    else
    {
        if(insensitive_time > 0)
        {
            insensitive_time--;
        }
    }
    /******* Modify S 111 *********/
    if(( InsCnt.A.Mode == 2 )					// JET OFF、バックアップ中
            || ( InsCnt.A.Mode == 6 ))					// パーカッション
        //		if( InsCnt.A.Mode == 2 )					// JET OFF、バックアップ中
        /******* Modify E 111 *********/
    {
        if(( DEF_Flow >= Max_Ins_Flow)
                && (FLOW_PHASE_Inter.A.INS == 0)
                /******* Modify S 112 *********/
                //			 && (INSTIM_Counter > 15)
                && (InsCnt.A.Phase != Trig_View)
                /******* Modify E 112 *********/
                && (insensitive_time == 0))
        {
            Spont = 1;
        }
        else
        {
            Spont = 0;
        }

        if(DEF_Flow <= Max_Exh_Flow)
        {
            Spont = 0;
        }

    }
    else if(InsCnt.A.Mode == 3)						// 自発呼吸モニター中
    {
        if( DEF_Flow >= Max_Ins_Flow)
        {
            Spont = 1;
        }
        else
        {
            Spont = 0;
        }
    }
    else
    {
        Spont = 0;
    }

    if(( Spont == 1 )			//
            && (Spont_old == 0))
    {
        Spont_log = Spont_log << 1 | 1;	// 左１ビットシフトで最下位ビット１
    }
    else									// スイッチ入力なし
    {
        Spont_log = Spont_log << 1;				// 左１ビットシフトで最下位ビット０
    }
    /******* Modify S 112 *********/
    //		if((INSTIM_Counter <= 15) && ( InsCnt.A.Mode == 2 ))			//
    //		{
    //			Spont_log = 0;
    //		}
    /******* Modify E 112 *********/

    if(Spont_log != 0)
    {
        Status = 1;
    }
    else
    {
        Status = 0;
    }
    Spont_old = Spont;
    //		Flow1 = DEF_Flow / 3;
    //Q_remove 11.4.16    Flow1 = base_flow;						// 通常
    //		Flow1 = TOut_base_flow1_old2;

    //		Flow1 = offset;
    //		Flow1 = VF1;
    //		Flow1 = CAL_offset;
    //		Flow1 = TOut_base_flow2_old2;

    //		Flow1 = PV11_vol2;
    //		Flow1 = Max_Ins_Flow;

    //		Flow1 = PV_base;
    //		Flow1 = PV_base_temp2;
    //		Flow1 = temp_int;
    //		Flow1 = VF;
    //		Flow1 = DEF_Flow;


#endif

    // 出力
    /**************************************************************************/
    INS_cnt_OLD_Mode = InsCnt.A.Mode;
    FLOW_PHASE_old.BYTE = FLOW_PHASE_Inter.BYTE;		// 吸気呼気エラー
    Ins_phase_old = Ins_phase;

    /**************************************************************************/
    // フラグ
    //	if(INS_log != 0)

    //	if(TOut_base_flow1_flag)
    //	if(( Resp_Phase == INS_START )
    //	 && (TOut_base_flow1_flag == 1))
    //	if(Resp_Phase == EXH_MID)
    //	if(Resp_Phase == INS_START)
    //	if( InsCnt.A.Phase == Ins_Strt)
    //	if( InsCnt.A.Phase == Exh_Strt )
    //	if( FLOW_PHASE_Inter.A.INS == 1)
    //	if( DEF_Flow >= Max_Ins_Flow)
    //	if( InsCnt.A.Phase == Trig_View)
    if( DOUCHOU.A.SIMV == 1)
    {
        LED_1_On();
    }
    else
    {
        LED_1_Off();
    }

    ////	if(EXH_log != 0)
    //	if( InsCnt.A.Phase == Trig_View2)
    //	if( FLOW_PHASE_Inter.A.EXH == 1)
    //	if(Non_count >= 480)						// 6秒以上吸気がないとき
    if(TOut_base_flow1_flag)
        //	if(Resp_Phase == INS_START)
        //	if(Resp_Phase == NON)
        //	if( DEF_Flow <= Max_Exh_Flow)
        //	if( DEF_Flow > Max_Exh_Flow)
        //	if(Spont)
        //	if(PSoC1.DATA.ERR.BIT.FLW_DCnct == 1)
        if( EXH_HLD_CUNT == 0 )
        {
            LED_2_On();
        }
        else
        {
            LED_2_Off();
        }
    // フラグ
    /**************************************************************************/


}



