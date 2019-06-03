/******************************************************************************/
//$COMMON.H$
//   File Name:  ValveStuck.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: This file contains the class declaration for the ValveStuck.  
/******************************************************************************/
#ifndef VALVESTUCK_H
#define VALVESTUCK_H

#include "PhaseMgr.h"

class ValveStuck
{
    public:

	void CheckAllValves(E_PhaseId phaseId);
	static ValveStuck* S_GetInstance(void);

	protected:

	private:

	void CheckExhValveStuckOpen(void);

	void CheckExhValveStuckClosed(void);

	void CheckAirValveStuckOpen(void);

	void CheckAirValveStuckClosed(void);

	void CheckO2ValveStuckOpen(void);

	void CheckO2ValveStuckClosed(void);

    //$COMMON.ATTRIBUTE$
    //    Name: PhaseId  
    //
    //    Description:
    //             Breath pahse identifier
    //
    //    Units: N/A
    //
    //    Range: N/A
    //                
	E_PhaseId PhaseId;

    //$COMMON.ATTRIBUTE$
    //    Name: S_Instance  
    //
    //    Description:
    //            static instance of this object pointer
    //
    //    Units: N/A
    //
    //    Range: N/A
    //                
	static ValveStuck* S_Instance;

	};

#endif  // VALVESTUCK_H

