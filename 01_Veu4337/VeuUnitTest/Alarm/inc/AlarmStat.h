#ifndef INC_ALARMSTAT_H_
#define INC_ALARMSTAT_H_

#include "CommonConstants.h"
#include "CommonTypes.h"
#include "AlarmInterface.h"

class ElevationTimer
{
    public:
        //constructor
        ElevationTimer (UNSIGNED tim);

        // Virtual routine to call upon expiration.
        void ExpirationRoutine (void);

        //$COMMON.ATTRIBUTE$
        //    Name: ElevationFlag
        //
        //    Description: flag used to indicate a an alarm is to be elevated
        //                 in priority
        //
        //    Units: None
        //
        //    Range: N/A
        //
        E_Bool ElevationFlag;

    protected:
    private:
};

class AlarmStat
{

    public:

        // constructor
        AlarmStat (E_GUIEventID id, E_AlarmLevel initialPriority,
                   E_AlarmLevel finalPriority, UNSIGNED tim, UNSIGNED occurent);

        // Set alarm to be active or not active
        E_ReturnStatus SetIsActive (E_AlarmStatus stat);

        // return the alarm status
        E_AlarmStatus GetIsActive (void);

        //return level of alarm
        E_AlarmLevel GetLevel(void);

        //$COMMON.ATTRIBUTE$
        //    Name: Level
        //
        //    Description: Current alarm priority level
        //
        //    Units: None
        //
        //    Range: N/A
        //
        E_AlarmLevel  Level;

        //$COMMON.ATTRIBUTE$
        //    Name: Level
        //
        //    Description: previous alarm priority level
        //
        //    Units: None
        //
        //    Range: N/A
        //
        E_AlarmLevel PreLevel;

        //$COMMON.ATTRIBUTE$
        //    Name: Processed
        //
        //    Description: Flag to indicate Alarm's task has processed
        //                 alarm
        //    Units: None
        //
        //    Range: N/A
        //
        E_Bool Processed;

        //$COMMON.ATTRIBUTE$
        //    Name: AlarmTimer
        //
        //    Description: Pointer to alarm elevation class
        //
        //    Units: None
        //
        //    Range: N/A
        //
        ElevationTimer * AlarmTimer;

        //$COMMON.ATTRIBUTE$
        //    Name: IsActive
        //
        //    Description: Alarm Status
        //
        //    Units: None
        //
        //    Range: N/A
        //
        E_AlarmStatus IsActive;

        //$COMMON.ATTRIBUTE$
        //    Name: OldIsActive
        //
        //    Description: Previous Alarm status
        //
        //    Units: None
        //
        //    Range: N/A
        //
        E_AlarmStatus OldIsActive;

        //$COMMON.ATTRIBUTE$
        //    Name: InitialPriority
        //
        //    Description: inital alarm level priority
        //
        //    Units: None
        //
        //    Range: N/A
        //
        E_AlarmLevel InitialPriority;

        //$COMMON.ATTRIBUTE$
        //    Name: FinalPriority
        //
        //    Description: End Alarm level priority
        //
        //    Units: None
        //
        //    Range: N/A
        //
        E_AlarmLevel FinalPriority;

        //$COMMON.ATTRIBUTE$
        //    Name: Id
        //
        //    Description: Current alarm identifier
        //
        //    Units: None
        //
        //    Range: N/A
        //
        E_GUIEventID Id;

    protected:

    private:

};

#endif /* INC_ALARMSTAT_H_ */
