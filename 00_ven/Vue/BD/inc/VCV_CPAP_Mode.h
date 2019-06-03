#ifndef VCV_CPAP_MODE_H
#define VCV_CPAP_MODE_H

#include "NormBreathingMode.h"
#include "SpontMode.h"

class VCV_CPAP_Mode : public SpontMode
{
public:
    static VCV_CPAP_Mode* S_GetInstance(void);
    void Enter(E_VentilationMode fromModeId);

protected:

private:
    VCV_CPAP_Mode(const VCV_CPAP_Mode&);
    VCV_CPAP_Mode(void);


    //$COMMON.ATTRIBUTE$
    //  Name: S_Instance
    //  Description:  Pointer to the single instance of this class.
    //  Units: None
    //  Range: n/a
    static VCV_CPAP_Mode* S_Instance;
};

#endif // VCV_CPAP_MODE_H
