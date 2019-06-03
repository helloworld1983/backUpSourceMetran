#ifndef INC_TESTIPCTASK_H_
#define INC_TESTIPCTASK_H_

#include "Standard.h"
#include "CommonTypes.h"
#include "DebugTrace.h"

//#include "queue.h"
//
//#define MAX_SIZE_OF_DATA_MESSAGE            (30)
//#define QUEUE_LENGTH_MESSAGE                (10)

//struct Message
//{
//        uint32_t MessageID;
//        bool IsResponse;
//
//        uint8_t CommandID;
//        uint8_t Length;
//        uint8_t Data[MAX_SIZE_OF_DATA_MESSAGE];
//};

class TestIPCTask
{
    public:
        // static instance of TestIPCTask
        static TestIPCTask *S_GetInstance(void);

        // initialize all public access to domain objects
        void Init (void);

        //Create HFO Task
        void TestIPCCreateTask(void);

//        //Send Data to Send Queue
//        bool SendData(Message *eMessage);
//
//        //Receive data from Receive queue
//        bool GetData(Message *eMessage);

    protected:

    private:
        //$COMMON.ATTRIBUTE$
        //    Name: S_Instance
        //
        //    Description: Pointer to instance of TestIPCTask
        //
        //    Units: None
        //
        //    Range: N/A
        //
        static TestIPCTask *S_Instance;

        // Entry Point into TestIPCTask task
        static void Entry (void* pvParameters);

        //TestIPCTask constructor
        TestIPCTask(void);

//        //$COMMON.ATTRIBUTE$
//        //    Name: qTrasferDataToIMX
//        //
//        //    Description: Queue to transfer data to IMX
//        //
//        //    Units: None
//        //
//        //    Range: N/A
//        //
//        QueueHandle_t qTrasferDataToIMX;
//
//        //$COMMON.ATTRIBUTE$
//        //    Name: qReceiveFromIMX
//        //
//        //    Description: Queue to receive data from IMX
//        //
//        //    Units: None
//        //
//        //    Range: N/A
//        //
//        QueueHandle_t qReceiveFromIMX;


};




#endif /* INC_TESTIPCTASK_H_ */
