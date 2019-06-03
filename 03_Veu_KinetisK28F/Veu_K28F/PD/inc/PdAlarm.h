//$COMMON.H$
//    File Name: PdAlarms.h
//
//    Class: PdAlarms - current Patient data Alarms
//						checks for high and low conditions and 
//						hi and low O2 thresholds 
//
/******************************************************************************/
#ifndef  PDALARMS_H
#define  PDALARMS_H

//#include "AlarmStat.h"
#include "Standard.h"

// Patient Data Alarms 

class PdAlarms
{
    public:
        // return the static instance of patient data
        static PdAlarms* S_GetInstance (void);

        // check hi alarm condition
        void CheckHiCondition (LONG hilimit, LONG value, int alarmID, int SensitiveValue = 0);

        // check for low alarm condition
        void CheckLowCondition (LONG lowlimit, LONG value, int alarmID, int SensitiveValue = 0);

        // check hi O2
        void CheckHiO2 (LONG hilimit, LONG value, int alarmID);

        // check low o2
        void CheckLowO2 (LONG lowlimit, LONG value, int alarmID);

        // check hi Internal O2	value is raw counts
        void CheckHiInternalO2 (LONG value);

        //check low amptitude
        void CheckLowAmlpitude(LONG lowlimit, LONG value, int alarmID);
        void CheckHighAmlpitude(LONG hilimit, LONG value, int alarmID);

        // Check low base line
        void CheckLowBaseLine(LONG lowlimit, LONG value, int alarmID);
        void CheckLowBaseLine2Secs(LONG lowlimit, LONG value, int alarmID);

        // check hi base line
        void CheckHiBaseLine(LONG hilimit, LONG value, int alarmID);
        void CheckHiBaseLine2Secs(LONG lowlimit, LONG value, int alarmID);

        // check hi pip
        void CheckHiPip(LONG hilimit, LONG value, int alarmID);

        // check low pip
        void CheckLowPip(LONG lowlimit, LONG value, int alarmID);

        // check low map
        void CheckLowMap(LONG lowlimit, LONG value, int alarmID);
        void CheckHiMap(LONG hilimit, LONG value, int alarmID);

        // check hi/low minute volume
        void CheckLowMinuteVol(LONG lowlimit, LONG value, int alarmID);
        void CheckHiMinuteVol(LONG hilimit, LONG value, int alarmID);

        //check high breath rate
        void CheckHiBreathRate(LONG hilimit, LONG value, int alarmID);

        //check low/hi Plow
        void CheckLow_PLow(LONG lowlimit, LONG value,int alarmID);
        void CheckHigh_PLow(LONG hilimit, LONG value,int alarmID);

        //check low/high Plow 5time
        void CheckLowPLow5Time(LONG lowlimit, LONG value, int alarmID);
        void CheckHighPLow5Time(LONG hilimit, LONG value, int alarmID);

        //check low/high PHigh
        void CheckLow_PHigh(LONG lowlimit, LONG value,int alarmID);
        void CheckHigh_PHigh(LONG hilimit, LONG value,int alarmID);

        //check high/low Phigh 5 times
        void CheckLowPHigh5Time(LONG lowlimit, LONG value, int alarmID);
        void CheckHighPHigh5Time(LONG lowlimit, LONG value, int alarmID);

        //Reset 02's Timer
        void ResetO2Time(void);

        //Reset alarm active time
        void ResetAlarmActiveTime();

        //Reset low alarm acitve time
        void ResetPLowAlarmActiveTime();

        //Reset PEEP alarm active time
        void ResetPEEPAlarmActiveTime();

    protected:

        //$COMMON.ATTRIBUTE$
        //    Name: S_Instance
        //
        //    Description: Static instance pointer
        //
        //    Units: None
        //
        //    Range: N/A
        //
        static PdAlarms* S_Instance;

    private:
        // constructor
        PdAlarms (void);

        //$COMMON.ATTRIBUTE$
        //    Name: mHighO2AlarmDelayTimeAfterFlushO2
        //
        //    Description: High 02 alarm delay time after flush 02
        //
        //    Units: None
        //
        //    Range: N/A
        //
        LONG mHighO2AlarmDelayTimeAfterFlushO2;

        //$COMMON.ATTRIBUTE$
        //    Name: mLowO2AlarmDelayTimeAfterFlushO2
        //
        //    Description: low 02 alarm delay time after flush 02
        //
        //    Units: None
        //
        //    Range: N/A
        //
        LONG mLowO2AlarmDelayTimeAfterFlushO2;

        //$COMMON.ATTRIBUTE$
        //    Name: highPEEPActiveTime
        //
        //    Description: high PEEP Active Time
        //
        //    Units: None
        //
        //    Range: N/A
        //
        ULONG highPEEPActiveTime;

        //$COMMON.ATTRIBUTE$
        //    Name: lowPEEPActiveTime
        //
        //    Description: Low PEEP Active Time
        //
        //    Units: None
        //
        //    Range: N/A
        //
        ULONG lowPEEPActiveTime;

        //$COMMON.ATTRIBUTE$
        //    Name: highPEEP2SecActiveTime
        //
        //    Description: high PEEP2Sec Active Time
        //
        //    Units: None
        //
        //    Range: N/A
        //
        ULONG highPEEP2SecActiveTime;

        //$COMMON.ATTRIBUTE$
        //    Name: lowPEEP2SecActiveTime
        //
        //    Description: low PEEP 2Sec Active Time
        //
        //    Units: None
        //
        //    Range: N/A
        //
        ULONG lowPEEP2SecActiveTime;

        //$COMMON.ATTRIBUTE$
        //    Name: lowPEEP2SecClearTime
        //
        //    Description: low PEEP 2Sec Clear Time
        //
        //    Units: None
        //
        //    Range: N/A
        //
        ULONG lowPEEP2SecClearTime;

        //$COMMON.ATTRIBUTE$
        //    Name: highPEEP2SecClearTime
        //
        //    Description: high PEEP 2Sec Clear Time
        //
        //    Units: None
        //
        //    Range: N/A
        //
        ULONG highPEEP2SecClearTime;

        //$COMMON.ATTRIBUTE$
        //    Name: highPLow1SecActiveTime
        //
        //    Description: high PLow 1Sec Active Time
        //
        //    Units: None
        //
        //    Range: N/A
        //
        ULONG highPLow1SecActiveTime;

        //$COMMON.ATTRIBUTE$
        //    Name: lowPLow1SecActiveTime
        //
        //    Description: low PLow 1Sec Active Time
        //
        //    Units: None
        //
        //    Range: N/A
        //
        ULONG lowPLow1SecActiveTime;

        //$COMMON.ATTRIBUTE$
        //    Name: highPLow1SecClearTime
        //
        //    Description: high PLow 1Sec Clear Time
        //
        //    Units: None
        //
        //    Range: N/A
        //
        ULONG highPLow1SecClearTime;

        //$COMMON.ATTRIBUTE$
        //    Name: lowPLow1SecClearTime
        //
        //    Description: low PLow 1Sec ClearTime
        //
        //    Units: None
        //
        //    Range: N/A
        //
        ULONG lowPLow1SecClearTime;

        //$COMMON.ATTRIBUTE$
        //    Name: highPLowActive5Times
        //
        //    Description: high PLow Active 5 Times
        //
        //    Units: None
        //
        //    Range: N/A
        //
        ULONG highPLowActive5Times;

        //$COMMON.ATTRIBUTE$
        //    Name: lowPLowActive5Times
        //
        //    Description: low PLow Active 5 Times
        //
        //    Units: None
        //
        //    Range: N/A
        //
        ULONG lowPLowActive5Times;

        //$COMMON.ATTRIBUTE$
        //    Name: highPHigh2SecActiveTime
        //
        //    Description: high PHigh 2Sec Active Time
        //
        //    Units: None
        //
        //    Range: N/A
        //
        ULONG highPHigh2SecActiveTime;

        //$COMMON.ATTRIBUTE$
        //    Name: lowPHigh2SecActiveTime
        //
        //    Description: low PHigh 2Sec Active Time
        //
        //    Units: None
        //
        //    Range: N/A
        //
        ULONG lowPHigh2SecActiveTime;

        //$COMMON.ATTRIBUTE$
        //    Name: lowPHigh2SecClearTime
        //
        //    Description: low PHigh 2Sec Clear Time
        //
        //    Units: None
        //
        //    Range: N/A
        //
        ULONG lowPHigh2SecClearTime;

        //$COMMON.ATTRIBUTE$
        //    Name: highPHigh2SecClearTime
        //
        //    Description: high PHigh 2Sec Clear Time
        //
        //    Units: None
        //
        //    Range: N/A
        //
        ULONG highPHigh2SecClearTime;

        //$COMMON.ATTRIBUTE$
        //    Name: highPHighActive5Times
        //
        //    Description: high PHigh Active 5 Times
        //
        //    Units: None
        //
        //    Range: N/A
        //
        ULONG highPHighActive5Times;

        //$COMMON.ATTRIBUTE$
        //    Name: lowPHighActive5Times
        //
        //    Description: low PHigh Active 5 Times
        //
        //    Units: None
        //
        //    Range: N/A
        //
        ULONG lowPHighActive5Times;
};

#endif  //PDALARMS_H

