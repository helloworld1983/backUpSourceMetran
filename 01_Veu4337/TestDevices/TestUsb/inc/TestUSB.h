#ifndef INC_TESTUSB_H_
#define INC_TESTUSB_H_

#include "Standard.h"
#include "CommonTypes.h"
#include "DebugTrace.h"

class TestUSB
{
public:
    // static instance of TestSolenoidValve
    static TestUSB *S_GetInstance(void);

    // initialize all public access to domain objects
    void Init (void);

    //Create USBTask
    void TestUSBCreateTask(void);

protected:

private:
    //$COMMON.ATTRIBUTE$
    //    Name: S_Instance
    //
    //    Description: Pointer to instance of TestUSB
    //
    //    Units: None
    //
    //    Range: N/A
    //
    static TestUSB *S_Instance;

    // Entry Point into HFO task
    static void Entry (void* pvParameters);

    //TestUSB constructor
    TestUSB(void);
};





#endif /* INC_TESTUSB_H_ */
