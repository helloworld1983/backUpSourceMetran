#include <HfoServo.h>
#include "board.h"

HfoServo *HfoServo::S_Instance = NULL;

#define ASSERTION(...)  //delete later

#define MAX_STROKE_VOL_ADJUSTMENT  200
#define MIN_STROKE_VOL_ADJUSTMENT  0
#define NUM_STROKE_INDEX (MAX_STROKE_VOL_ADJUSTMENT - MIN_STROKE_VOL_ADJUSTMENT + 1)

#define NUM_FREQUENCY_INDEX 14

static const int32_t steps_for_big_ml[NUM_FREQUENCY_INDEX][NUM_STROKE_INDEX] = {
        {0, 628, 1253, 1873, 2486, 3089, 3679, 4256, 4815, 5356,
                5875, 6372, 6843, 7287, 7702, 8087, 8441, 8760, 9046, 9296,
                9509, 9684, 9822, 9920, 9980, 10000, 9981, 9922, 9825, 9688,
                9514, 9301, 9053, 8768, 8449, 8097, 7712, 7298, 6854, 6384,
                5888, 5369, 4829, 4270, 3694, 3104, 2501, 1889, 1268, 643,
                16, -612, -1237, -1857, -2470, -3074, -3665, -4241, -4801, -5342,
                -5862, -6359, -6831, -7276, -7692, -8078, -8432, -8753, -9039, -9290,
                -9504, -9680, -9819, -9918, -9979, -10000, -9982, -9924, -9827, -9692,
                -9518, -9307, -9059, -8776, -8458, -8106, -7723, -7309, -6866, -6396,
                -5901, -5383, -4843, -4285, -3709, -3119, -2517, -1904, -1284, -659, -32},
        {0, 753, 1502, 2242, 2969, 3679, 4369, 5034, 5670, 6274,
                6843, 7372, 7860, 8303, 8699, 9046, 9341, 9583, 9771, 9904,
                9980, 9999, 9962, 9868, 9718, 9514, 9255, 8943, 8581, 8170,
                7712, 7211, 6669, 6089, 5475, 4829, 4156, 3460, 2743, 2012,
                1268, 518, -235, -987, -1734, -2470, -3193, -3897, -4579,
                -5236, -5862, -6456, -7012, -7529, -8003, -8432, -8813, -9144, -9423,
                -9648, -9819, -9933, -9992, -9994, -9939, -9827, -9660, -9438, -9163,
                -8835, -8458, -8032, -7561, -7046, -6492, -5901, -5276, -4622, -3941,
                -3238, -2517, -1781, -1035, -283},
        {0, 878, 1749, 2607, 3445, 4256, 5034, 5773, 6468, 7113,
                7702, 8233, 8699, 9099, 9428, 9684, 9866, 9971, 9999, 9950,
                9825, 9623, 9347, 8999, 8581, 8097, 7550, 6945, 6287, 5579,
                4829, 4042, 3223, 2379, 1517, 643, -235, -1112, -1980, -2834,
                -3665, -4467, -5236, -5964, -6645, -7276, -7850, -8364, -8813,
                -9194, -9504, -9740, -9901, -9986, -9994, -9924, -9778, -9556,
                -9261, -8893, -8458, -7956, -7394, -6774, -6102, -5383, -4622,
                -3825, -2999, -2150, -1284, -409},
        {0, 1003, 1996, 2969, 3912, 4815, 5670, 6468, 7200, 7860,
                8441, 8936, 9341, 9652, 9866, 9980, 9993, 9906, 9718,
                9433, 9053, 8581, 8022, 7383, 6669, 5888, 5048, 4156,
                3223, 2257, 1268, 267, -737, -1734, -2713, -3665, -4579,
                -5448, -6262, -7012, -7692, -8294, -8813, -9242, -9579,
                -9819, -9959, -9999, -9939, -9778, -9518, -9163, -8715,
                -8179, -7561, -6866, -6102, -5276, -4398, -3475, -2517,
                -1533, -534},
        {0, 1128, 2242, 3327, 4369, 5356, 6274, 7113, 7860, 8507,
                9046, 9469, 9771, 9949, 9999, 9922, 9718, 9391, 8943,
                8381, 7712, 6945, 6089, 5156, 4156, 3104, 2012, 894,
                -235, -1361, -2470, -3548, -4579, -5553, -6456, -7276,
                -8003, -8629, -9144, -9542, -9819, -9970, -9994, -9890,
                -9660, -9307, -8835, -8251, -7561, -6774, -5901, -4953,
                -3941, -2879, -1781, -659},
        {0, 1253, 2486, 3679, 4815, 5875, 6843, 7702, 8441, 9046,
                9509, 9822, 9980, 9981, 9825, 9514, 9053, 8449, 7712,
                6854, 5888, 4829, 3694, 2501, 1268, 16, -1237, -2470,
                -3665, -4801, -5862, -6831, -7692, -8432, -9039, -9504,
                -9819, -9979, -9982, -9827, -9518, -9059, -8458, -7723,
                -6866, -5901, -4843, -3709, -2517, -1284, -32},
        {0, 1377, 2728, 4027, 5249, 6372, 7372, 8233, 8936, 9469, 9822,
                9987, 9962, 9747, 9347, 8768, 8022, 7124, 6089, 4939, 3694,
                2379, 1019, -361, -1734, -3074, -4355, -5553, -6645, -7611,
                -8432, -9092, -9579, -9883, -9999, -9924, -9660, -9212, -8589,
                -7802, -6866, -5799, -4622, -3357, -2027, -659},
        {0, 1502, 2969, 4369, 5670, 6843, 7860, 8699, 9341, 9771, 9980,
                9962, 9718, 9255, 8581, 7712, 6669, 5475, 4156, 2743,
                1268, -235, -1734, -3193, -4579, -5862, -7012, -8003,
                -8813, -9423, -9819, -9992, -9939, -9660, -9163, -8458,
                -7561, -6492, -5276, -3941, -2517, -1035},
        {0, 1626, 3208, 4705, 6077, 7287, 8303, 9099, 9652, 9949, 9981,
                9747, 9255, 8516, 7550, 6384, 5048, 3577, 2012, 393,
                -1237, -2834, -4355, -5760, -7012, -8078, -8929, -9542,
                -9901, -9997, -9827, -9396, -8715, -7802, -6681, -5383,
                -3941, -2395, -785},
        {0, 1749, 3445, 5034, 6468, 7702, 8699, 9428, 9866, 9999, 9825,
                9347, 8581, 7550, 6287, 4829, 3223, 1517, -235, -1980,
                -3665, -5236, -6645, -7850, -8813, -9504, -9901, -9994,
                -9778, -9261, -8458, -7394, -6102, -4622, -2999, -1284},
        {0, 1873, 3679, 5356, 6843, 8087, 9046, 9684, 9980, 9922, 9514,
                8768, 7712, 6384, 4829, 3104, 1268, -612, -2470, -4241,
                -5862, -7276, -8432, -9290, -9819, -10000, -9827, -9307,
                -8458, -7309, -5901, -4285, -2517, -659},
        {0, 1996, 3912, 5670, 7200, 8441, 9341, 9866, 9993, 9718, 9053,
                8022, 6669, 5048, 3223, 1268, -737, -2713, -4579, -6262,
                -7692, -8813, -9579, -9959, -9939, -9518, -8715, -7561,
                -6102, -4398, -2517, -534},
        {0, 2119, 4142, 5976, 7540, 8760, 9583, 9971, 9906, 9391, 8449,
                7124, 5475, 3577, 1517, -612, -2713, -4691, -6456, -7927,
                -9039, -9740, -9999, -9803, -9163, -8106, -6681, -4953, -2999, -910},

                //test
                {0, 2486, 4815, 6843, 8441, 9509, 9980, 9825, 9053, 7712, 5888, 3694, 1268,
                        -1237, -3665, -5862, -7692, -9039, -9819, -9982, -9518, -8458, -6866,
                        -4843, -2517, -32
                }

};

int32_t HfoServo::CreateSinPosition (int iFrequence)
{
//    Board_UARTDebug("Fre = %d\n",iFrequence);
    int returnValue = 0;
    static int index = 0;

    int iMaxIndex = 1000 / (iFrequence);
    if(iMaxIndex % 2 == 0)
    {
        iMaxIndex = iMaxIndex / 2;
    }
    else
    {
        iMaxIndex -= 1;
        iMaxIndex = iMaxIndex / 2;
    }

//    Board_UARTDebug("iMaxIndex = %d\n",iMaxIndex);
//    Board_UARTDebug("1(%d) ",index);


    returnValue = steps_for_big_ml[iFrequence - 5][index];
    ++index;


    if(index == iMaxIndex)
    {
        index = 0;
    }
//    Board_UARTDebug(" %d \n", returnValue);

    return returnValue;
}
int32_t HfoServo::CreateSinPositionForSmallMotor (int iFrequence)
{
    int returnValue = 0;
    static int index = 0;

    int iMaxIndex = 1000 / (iFrequence);
    if(iMaxIndex % 2 == 0)
    {
        iMaxIndex = iMaxIndex / 2;
    }
    else
    {
        iMaxIndex -= 1;
        iMaxIndex = iMaxIndex / 2;
    }

//    Board_UARTDebug("2(%d) ",index);

    returnValue = steps_for_big_ml[iFrequence - 5][index];
    ++index;


    if(index == iMaxIndex)
    {
        index = 0;
    }

//    Board_UARTDebug(" %d \n", returnValue);

    return returnValue;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: HfoServo()
//
//    Processing:
//      This operation is the Console Task constructor.
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      None
//
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//      None
//
//    Requirements:
//
/******************************************************************************/
HfoServo::HfoServo() : Controller()
{
    servoIsAskedForStop = eTrue;
    motorIsStop = eTrue;

}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: S_GetInstance
//
//    Processing: returns the static instance of the HfoServo class
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values: pointer to HfoServo class
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
HfoServo* HfoServo::S_GetInstance (void)
{
    //if objects do not exit, create it
    if(NULL == S_Instance)
    {
        S_Instance = new HfoServo();
    }

    //check object is null or not
    ASSERTION(S_Instance != NULL);

    return (S_Instance);
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: PistonInstalled
//
//    Processing: Check status of HFO piston ON or OFF
//
//    Input Parameters: None
//
//    Output Parameters:
//
//    Return Values: False if piston switch is ON
//                  True if piston switch is OFF
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
E_Bool HfoServo::PistonInstalled (void)
{
    E_Bool status = eFalse;

    //if hfo piston switch is ON, return FALSE and  vice versa
    if("ON")    //if(HFOPistonSwitch->GetStateDirect() == eOn)
    {
        status = eFalse;
    }
    else
    {
        status = eTrue;
    }

    return status;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: MotorIsStopped
//
//    Processing: returns status of motor(stop or not)
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values: E_Bool MotorIsStop
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
E_Bool HfoServo::MotorIsStopped (void)
{
    return motorIsStop;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: StopMotor
//
//    Processing: Stop motor
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
void HfoServo::StopMotor (void)
{
    servoIsAskedForStop = eTrue;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: StartMotor
//
//    Processing: Start motor
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
void HfoServo::StartMotor (void)
{
    servoIsAskedForStop = eFalse;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Init
//
//    Processing: init for motor and servo
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
void HfoServo::Init (void)
{
    motorIsStop = eFalse;

    servoIsAskedForStop = eTrue;

    freqIntegrator = 0;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Init
//
//    Processing: servo control motor
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
void HfoServo::Run (void)
{
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ResetShiftPosition
//
//    Processing: Reset shift position
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
void HfoServo::ResetShiftPosition (void)
{
    shiftPosition = 0;
    shiftCounter = 0;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GetCurrentFreq
//
//    Processing: get current frequency of motor
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
long HfoServo::GetCurrentFreq (void)
{
    return freqIntegrator;
}
