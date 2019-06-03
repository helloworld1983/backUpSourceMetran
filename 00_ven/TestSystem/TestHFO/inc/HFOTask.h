#ifndef INC_HFOTASK_H_
#define INC_HFOTASK_H_

#include "Standard.h"
#include "CommonTypes.h"
#include "DebugTrace.h"

class HFOTask
{
    public:
        // static instance of HFOTask
        static HFOTask *S_GetInstance(void);

        // initialize all public access to domain objects
        void Init (void);

        //Create HFO Task
        void HFOCreateTask(void);

    protected:

    private:
        //$COMMON.ATTRIBUTE$
        //    Name: S_Instance
        //
        //    Description: Pointer to instance of Alarm task
        //
        //    Units: None
        //
        //    Range: N/A
        //
        static HFOTask *S_Instance;

        // Entry Point into HFO task
        static void Entry (void* pvParameters);

        //HFO constructor
        HFOTask(void);

        uint8_t CalCheckSum(uint8_t Cmd[8]);

};
#endif /* INC_HFOTASK_H_ */
