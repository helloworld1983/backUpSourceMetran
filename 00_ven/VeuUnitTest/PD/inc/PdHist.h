//$COMMON.H$
//    File Name: PdHist.h
//    Copyright 1996 InVentive Technologies, Inc.  All Rights Reserved.
//
//    Class:PdHistory Patient Data History - 
//		Keeps a history of last breath period, breath type, exhaled volume, and
//		mean airway pressure for the last MAX_BREATHS.  Primary use is in the 
//		computations of the Minute Volume calculations for Patient Data and 
//		average mean airway pressure
/******************************************************************************/
#ifndef PDHIST_H
#define	PDHIST_H

#include "CommonTypes.h"    // Common stuff
// Patient Data History

class PdHistory
{
    public:

        //$COMMON.ATTRIBUTE$
        //    Name: InhTime
        //
        //    Description: Inhaltion Time -
        //				   Time of breath in inhalation phase
        //
        //    Units: Milliseconds (ms)
        //
        //    Range: N/A
        //
        LONG InhTime;

        //$COMMON.ATTRIBUTE$
        //    Name: ExhTime
        //
        //    Description: Exhalation time -
        //				   Time of breath in exhalation phase
        //
        //    Units: Milliseconds (ms)
        //
        //    Range: N/A
        //
        LONG ExhTime;

        //$COMMON.ATTRIBUTE$
        //    Name: Indicator
        //
        //    Description:  Indicator of Inhalation type -
        //					for patient data eMandatory and eSpontaneous supported;
        //					If eAssisted then eMandatory is used; if eSupported
        //					then eSpontaneous is used.
        //
        //    Units: None
        //
        //    Range: eMandatory and eSpontaneous
        //
        E_BreathPhaseIndicator Indicator;

        //$COMMON.ATTRIBUTE$
        //    Name: ExhTidalVol
        //
        //    Description: Exhaled Tidal Volume
        //
        //    Units: milli-liters
        //
        //    Range: N/A
        //
        LONG ExhTidalVol;

        //$COMMON.ATTRIBUTE$
        //    Name: ExhTidalVol
        //
        //    Description: Inhaled Tidal Volume
        //
        //    Units: milli-liters
        //
        //    Range: N/A
        //
        LONG InhTidalVol;

        //$COMMON.ATTRIBUTE$
        //    Name: MeanAirwayPress
        //
        //    Description:  The mean airway pressure
        //
        //    Units: CMH2O scaled 100
        //
        //    Range: N/A
        //
        LONG MeanAirwayPress;

    protected:

    private:

};

#endif  //PDHIST_H
