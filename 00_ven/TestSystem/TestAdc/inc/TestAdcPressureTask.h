#ifndef INC_TESTADCPRESSURETASK_H_
#define INC_TESTADCPRESSURETASK_H_

#include "Standard.h"
#include "CommonTypes.h"
#include "DebugTrace.h"

class TestAdcPressureTask
{
    public:
        // static instance of TestAdcPressureTask
        static TestAdcPressureTask *S_GetInstance(void);

        // initialize all public access to domain objects
        void Init (void);

        //Create TestAdcPressureTask
        void TestAdcPressureCreateTask(void);

        //Read value from adc channel
        float ReadADC(ADC_CHANNEL_T channel);

    protected:

    private:
        //$COMMON.ATTRIBUTE$
        //    Name: S_Instance
        //
        //    Description: Pointer to instance of TestAdcPressureTask
        //
        //    Units: None
        //
        //    Range: N/A
        //
        static TestAdcPressureTask *S_Instance;

        // Entry Point into HFO task
        static void Entry (void* pvParameters);

        //TestAdcPressureTask constructor
        TestAdcPressureTask(void);

};

#endif /* INC_TESTADCPRESSURETASK_H_ */
