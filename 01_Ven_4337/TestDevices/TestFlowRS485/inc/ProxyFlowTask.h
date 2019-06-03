#ifndef INC_PROXYFLOWTASK_H_
#define INC_PROXYFLOWTASK_H_

#include "Standard.h"
#include "CommonTypes.h"
#include "DebugTrace.h"

class ProxyFlowTask
{
    public:
        // static instance of ProxyFlowTask
        static ProxyFlowTask *S_GetInstance(void);

        // initialize all public access to domain objects
        void Init (void);

        //Create ProxyFlowTask
        void ProxyFlowCreateTask(void);

        void ProxyFlowReadTask(void);

    protected:

    private:
        //$COMMON.ATTRIBUTE$
        //    Name: S_Instance
        //
        //    Description: Pointer to instance of ProxyFlowTask
        //
        //    Units: None
        //
        //    Range: N/A
        //
        static ProxyFlowTask *S_Instance;

        // Entry Point into HFO task
        static void Entry (void* pvParameters);

        static void Read(void* P);

        //HFO constructor
        ProxyFlowTask(void);

};

#endif /* INC_PROXYFLOWTASK_H_ */
