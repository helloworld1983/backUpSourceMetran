#ifndef INC_TESTFLOWSENI2C1_H_
#define INC_TESTFLOWSENI2C1_H_

#include "Standard.h"
#include "CommonTypes.h"
#include "DebugTrace.h"

class TestFlowSenI2C1
{
    public:
        // static instance of TestFlowSensorTask
        static TestFlowSenI2C1 *S_GetInstance(void);

        // initialize all public access to domain objects
        void Init (void);

        //Create HFO Task
        void TestFlowSenI2C1CreateTask(void);

        //Read data from add
        bool I2C1_Read(uint32_t addr, uint8_t* buf, uint32_t len);

        //Write data from add
        bool I2C1_Write(uint32_t addr, uint8_t* buf, uint32_t len);

        //calculate CRC
        uint8_t GetCRC8( const void *buff, size_t size );

    protected:

    private:
        //$COMMON.ATTRIBUTE$
        //    Name: S_Instance
        //
        //    Description: Pointer to instance of TestFlowSensorTask
        //
        //    Units: None
        //
        //    Range: N/A
        //
        static TestFlowSenI2C1 *S_Instance;

        // Entry Point into HFO task
        static void Entry (void* pvParameters);

        //HFO constructor
        TestFlowSenI2C1(void);
};



#endif /* INC_TESTFLOWSENI2C1_H_ */
