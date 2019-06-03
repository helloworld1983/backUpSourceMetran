/******************************************************************************/
//$COMMON.H$
//   File Name:  Btps.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: 
//      This static class declares static attributes and operations for
//      determining BTPS compensation factors for delivered air and oxygen
//      flows and for exhaled flow.
/******************************************************************************/
#ifndef BTPS_H
#define BTPS_H

#include "CommonConstants.h"

/******************************************************************************/
//      CLASS DECLARATIONS
/******************************************************************************/
class Btps
{
    public:
        static inline LONG S_GetBtpsO2Factor (void)
        {
            return S_BtpsO2Factor;
        }
        static inline LONG S_GetBtpsAirFactor (void)
        {
            return S_BtpsAirFactor;
        }
        static inline LONG S_GetBtpsExhFactor (void)
        {
            return S_BtpsExhFactor;
        }
        static void S_ComputeBtpsFactors (void);
        static void S_ComputeBtpsExhFactor (LONG pctO2);

    protected:

    private:
        Btps (void);
        Btps (const Btps&);

        //$COMMON.ATTRIBUTE$
        //  Name: S_BtpsO2Factor
        //  Description:  BTPS factor for delivered O2; delivered flows
        //      are divided by this value.
        //  Units: None
        //  Range: n/a
        static LONG S_BtpsO2Factor;

        //$COMMON.ATTRIBUTE$
        //  Name: S_BtpsAirFactor
        //  Description:  BTPS factor for delivered Air; delivered flows
        //      are divided by this value.
        //  Units: None
        //  Range: n/a
        static LONG S_BtpsAirFactor;

        //$COMMON.ATTRIBUTE$
        //  Name: S_BtpsExhFactor
        //  Description:  BTPS factor for exhaled flow; exhaled flows are
        //      multiplied by this value.
        //  Units: None
        //  Range: n/a
        static LONG S_BtpsExhFactor;

        //$COMMON.ATTRIBUTE$
        //  Name: S_BtpsExhAirFactor
        //  Description:  BTPS factor for exhaled Air; this value is used
        //      with S_BtpsExhO2Factor to determine the S_BtpsExhFactor based
        //      upon oxygen mix.
        //  Units: None
        //  Range: n/a
        static LONG S_BtpsExhAirFactor;

        //$COMMON.ATTRIBUTE$
        //  Name: S_BtpsExhO2Factor
        //  Description:  BTPS factor for exhaled O2; this value is used
        //      with S_BtpsExhAirFactor to determine the S_BtpsExhFactor based
        //      upon oxygen mix.
        //  Units: None
        //  Range: n/a
        static LONG S_BtpsExhO2Factor;

};

#endif // BTPS_H
