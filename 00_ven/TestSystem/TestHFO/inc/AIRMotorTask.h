#ifndef INC_AIRMOTORTASK_H_
#define INC_AIRMOTORTASK_H_

#include "Standard.h"
#include "CommonTypes.h"
#include "DebugTrace.h"

class AirTask
{
    public:
        // static instance of HFOTask
        static AirTask *S_GetInstance(void);

        // initialize all public access to domain objects
        void Init (void);

        //Create HFO Task
        void AirCreateTask(void);

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
        static AirTask *S_Instance;

        // Entry Point into HFO task
        static void Entry (void* pvParameters);

        //HFO constructor
        AirTask(void);

        uint8_t CalCheckSum(uint8_t Cmd[8]);

};




#endif /* INC_AIRMOTORTASK_H_ */
