#ifndef INC_TESTSOLENOIDVALVE_H_
#define INC_TESTSOLENOIDVALVE_H_

#include "Standard.h"
#include "CommonTypes.h"
#include "DebugTrace.h"

class TestSolenoidValve
{
    public:
        // static instance of TestSolenoidValve
        static TestSolenoidValve *S_GetInstance(void);

        // initialize all public access to domain objects
        void Init (void);

        //Create ProxyFlowTask
        void SolenoidValveCreateTask(void);

    protected:

    private:
        //$COMMON.ATTRIBUTE$
        //    Name: S_Instance
        //
        //    Description: Pointer to instance of TestSolenoidValve
        //
        //    Units: None
        //
        //    Range: N/A
        //
        static TestSolenoidValve *S_Instance;

        // Entry Point into HFO task
        static void Entry (void* pvParameters);

        //TestSolenoidValve constructor
        TestSolenoidValve(void);
};

#endif /* INC_TESTSOLENOIDVALVE_H_ */
