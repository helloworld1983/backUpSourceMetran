#ifndef INC_GUIINTERFACE_H_
#define INC_GUIINTERFACE_H_

#include "CommonTypes.h"
#include "CommonConstants.h"

struct EndExhalationDataGUIEvent
{
    LONG MeanAirwayPressure;
    LONG ExhaledMinuteVolume;
    LONG SpontaneousMinuteVolume;

    LONG TotalRespiratoryRate;
    LONG SpontaneousRespiratoryRate;
    LONG IERatio;
    LONG RapidShallowBreathingIndex;
    LONG TidalVolume;
    LONG TimeInExhalation;
    LONG PEEP;
    LONG ProxymalLeak;
    LONG Compliance;
    LONG APRVPLow;

//    EndExhalationDataGUIEvent& operator = (EndExhalationDataGUIEvent tmpEvent)
//    {
//        MeanAirwayPressure = tmpEvent.MeanAirwayPressure;
//        ExhaledMinuteVolume = tmpEvent.ExhaledMinuteVolume;
//        SpontaneousMinuteVolume = tmpEvent.SpontaneousMinuteVolume;
//        TotalRespiratoryRate = tmpEvent.TotalRespiratoryRate;
//        SpontaneousRespiratoryRate = tmpEvent.SpontaneousRespiratoryRate;
//        IERatio = tmpEvent.IERatio;
//        RapidShallowBreathingIndex = tmpEvent.RapidShallowBreathingIndex;
//        TidalVolume = tmpEvent.TidalVolume;
//        TimeInExhalation = tmpEvent.TimeInExhalation;
//        PEEP = tmpEvent.PEEP;
//        ProxymalLeak = tmpEvent.ProxymalLeak;
//        Compliance = tmpEvent.Compliance;
//        APRVPLow = tmpEvent.APRVPLow;
//        return *this;
//    }
};

struct EndInhalationDataGUIEvent
{
    LONG PeakInspiratoryPressure;
    LONG EndInhalationPressure;
    LONG TimeInInhalation;
    E_BreathPhaseIndicator InhalationType;
    LONG InhaledTidalVolume;
    LONG InhaledMinuteVolume;
    LONG PlateausPress;

    EndInhalationDataGUIEvent& operator = (EndInhalationDataGUIEvent tmpEvent)
    {
        PeakInspiratoryPressure = tmpEvent.PeakInspiratoryPressure;
        EndInhalationPressure = tmpEvent.EndInhalationPressure;
        TimeInInhalation = tmpEvent.TimeInInhalation;
        InhalationType = tmpEvent.InhalationType;
        InhaledTidalVolume = tmpEvent.InhaledTidalVolume;
        InhaledMinuteVolume = tmpEvent.InhaledMinuteVolume;
        PlateausPress = tmpEvent.PlateausPress;

        return *this;
    }
};




#endif /* INC_GUIINTERFACE_H_ */
