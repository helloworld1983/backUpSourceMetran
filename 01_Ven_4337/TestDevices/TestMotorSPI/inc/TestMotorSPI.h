#ifndef INC_TESTMOTORSPI_H_
#define INC_TESTMOTORSPI_H_

#include "Standard.h"
#include "CommonTypes.h"
#include "DebugTrace.h"

class TestMotorSPI
{
public:
    // static instance of TestSolenoidValve
    static TestMotorSPI *S_GetInstance(void);

    // initialize all public access to domain objects
    void Init (void);

    //Create ProxyFlowTask
    void TestMotorSPICreateTask(void);

    //send data to address of motor
    void sendData2(uint8_t MotorId, uint8_t address, uint32_t datagram);

protected:

private:
    //$COMMON.ATTRIBUTE$
    //    Name: S_Instance
    //
    //    Description: Pointer to instance of TestMotorSPI
    //
    //    Units: None
    //
    //    Range: N/A
    //
    static TestMotorSPI *S_Instance;

    // Entry Point into HFO task
    static void Entry (void* pvParameters);

    //TestMotorSPI constructor
    TestMotorSPI(void);
};


#endif /* INC_TESTMOTORSPI_H_ */
