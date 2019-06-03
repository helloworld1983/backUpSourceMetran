//$COMMON.CPP$
//   File Name:  TestServices.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: Provides common services required by tests.
//
//   Interfaces:
//
//   Restrictions:
/******************************************************************************/

#include "TestServices.h"
#include "Criterion.h"
//#include "mathutil.h"
//
//#ifndef NO_HARDWARE
//    #include "RS232Utilities.h"
//    #include "Clock.h"
//    #include "Blower.h"
//    #include "Aztest.h"
//    #include "ExhContr.h"
//    #include "FiO2.h"
//    #include "FlowSens.h"
//    #include "PresSens.h"
//    #include "Flow_Cnt.h"
//    #include "Parser.h"
//	#include "D24VOutput.h"
//#endif

#include "DiagnosticTask.h"

const UNSIGNED TestServices::MAXUNSIGNED = ~0L;

//*****************************************************************************/
//    Operation Name: TheTestServices()
//
//    Processing: Return a reference to a local static instance of this class. 
//      All access to this class should be made through this function. 
//
//    Input Parameters:
//
//    Output Parameters:
//
//    Return Values:
//      TestServices& - a ref to the only instance of this class. 
//
//    Pre-Conditions:
//
//    Miscellaneous:
//
//    Requirements:  
//
//*****************************************************************************/
TestServices& TheTestServices(void)
{
    static TestServices testServices;
    return testServices;
}

//*****************************************************************************/
//    Operation Name: Initiate()
//
//    Processing: Initiates the self test by initializing the instrument 
//      before testing begins.
//
//    Input Parameters:
//
//    Output Parameters:
//
//    Return Values:
//
//    Pre-Conditions:
//
//    Miscellaneous:
//
//    Requirements:  
//
//*****************************************************************************/
void TestServices::Initiate(void)
{
//    TheBlower->SwitchOn();
//    TheBlower->RampToDefaultSpeed();

    // reset all previous results from any self test
    ResultIterator results;
    results->ResetAll();

//	AirFlowController->SetDesired(0);
//    O2FlowController->SetDesired(0);

//	AirFlowController->Enable();
//    O2FlowController->Enable();

    // rezero inh pressure transducer 
    // rezero exh pressure transducer
//	PInhAzTestPtr->Run();
//	PExhAzTestPtr->Run();

    KnownState();
}

//*****************************************************************************/
//    Operation Name: Conclude()
//
//    Processing: Concludes the self test.
//
//    Input Parameters:
//
//    Output Parameters:
//
//    Return Values:
//
//    Pre-Conditions:
//                     
//    Miscellaneous:
//
//    Requirements:  
//
//*****************************************************************************/
void TestServices::Conclude(void)
{
//    KnownState();

//    HeatedFilter->SetState(eOff);
//    SafetyValve->SetState(eOpen);
}

//*****************************************************************************/
//    Operation Name: KnownState()
//
//    Processing: Puts the hardware into a known state. This allows each test
//      to start off in a known state and makes each test more autonomous
//      because a test is not dependent on the state of a previous test. Test
//      order can then change with little impact.
//
//    Input Parameters:
//
//    Output Parameters:
//
//    Return Values:
//
//    Pre-Conditions:
//
//    Miscellaneous:
//
//    Requirements:  
//
//*****************************************************************************/
void TestServices::KnownState(void)
{
//    ExhController->Open();

//	AirFlowController->Enable();
//	AirFlowController->SetDesired(0);
//	AirFlowController->Reset(0);
//
//    O2FlowController->Enable();
//    O2FlowController->SetDesired(0);
//    O2FlowController->Reset(0);
//
//    HeatedFilter->SetState(eOn);
//    Enable24V->SetState(eOn);
//
//    CrossoverPilot->SetState(eOpen);
//    SafetyValve->SetState(eClose);
//    InhPressureZero->SetState(eClose);
//    ExhPressureZero->SetState(eClose);
//
//    TheBlower->SwitchOn();
//    TheBlower->RampToDefaultSpeed();
}

//*****************************************************************************/
//    Operation Name: GetFirstFailure()
//
//    Processing: Cycle through all the test results to find the first failure
//      that occurred during a self test and return a pointer to that info 
//      struct. If no failure is found, the info struct code will be eFC_None.
//
//    Input Parameters:
//
//    Output Parameters:
//      info - a pointer to the failure structure. 
//
//    Return Values:
//
//    Pre-Conditions:
//
//    Miscellaneous:
//
//    Requirements:  
//
//*****************************************************************************/
void TestServices::GetFirstFailure(Result::Info& infoReturn)
{
    ResultIterator iterator;
    Result::Info info; 
    USHORT lowestUsedNum = 0xFFFF;

    infoReturn.code = eFC_None;

    // if there are results to cycle through
    if (iterator.Valid())
    {
        // cycle through all test results
        do
        {
            // get result information structure
            iterator->GetInfo(&info);

            // if the test result was used and it failed and its used is lowest
            if (info.used && !info.passes && info.used < lowestUsedNum)
            {
                // save a new lowest index
                lowestUsedNum = info.used;

                // copy failed struct info over to users struct  
                infoReturn = info;
            }
        } while (iterator++);
    }
}

//*****************************************************************************/
//    Operation Name: DoTestsPass()
//
//    Processing: Checks all the test results and determines whether a self
//      test passes or fails. If any individual result fails then the entire
//      self test fails. 
//
//    Input Parameters:
//
//    Output Parameters:
//
//    Return Values:
//      E_Bool - true if all results passed; otherwise false.
//
//    Pre-Conditions:
//
//    Miscellaneous:
//
//    Requirements:  
//
//*****************************************************************************/
E_Bool TestServices::DoTestsPass(void)
{
    E_Bool passes = eTrue;
    E_Bool thereWereResults = eFalse;
    ResultIterator iterator;
    Result::Info info;

    // if there are results to cycle through
    if (iterator.Valid())
    {
        // cycle through all test results
        do
        {
            // get result information structure
            iterator->GetInfo(&info);

            // if this test result was used during the test sequence
            if (info.used)
            {
                // set flag to indicate if there were any results used
                thereWereResults = eTrue;

                // if the result was used during test and it failed
                if (!info.passes)
                {
                    passes = eFalse;
                    break;
                }
            }
        } while (iterator++);
    }

    // return whether test was successful or unsuccessful
    if (passes && thereWereResults)
        return eTrue;
    else
        return eFalse;
}

//*****************************************************************************/
//    Operation Name: GetFlow()
//
//    Processing: Averages some flow samples before returning the measured flow.
//
//    Input Parameters:
//      sensor - the sensor to read the flow.
//      readingsToAvg - the number of samples to take from the sensor and 
//          average.
//      *min - a pointer to output the minimum reading taken.
//      *max - a pointer to output the maximum reading taken.
//
//    Output Parameters:
//      *min - minimum measurement.
//      *min - maximum measurement.
//
//    Return Values:
//      LONG - the averaged flow measured in lpm.
//
//    Pre-Conditions:
//
//    Miscellaneous:
//
//    Requirements:  
//
//*****************************************************************************/
LONG TestServices::GetFlow
    (
        FlowSensor* sensor, 
        USHORT readingsToAvg,
        SHORT* min, 
        SHORT* max
    )
{
    ASSERTION(readingsToAvg);

    LONG averageFlow = 0;

    for (UCHAR i=1; i<=readingsToAvg; i++)
    {
        // read the flow sensor
//    	LONG flow = sensor->GetCurrentReading() / LPM_FACTOR;

        // compute the average flow
//        averageFlow = avg(averageFlow, flow, i);

        // track the minimum and maximum readings
//        if (i == 1 && min && max)
//            *min = *max = flow;
//        else if (min && max)
//        {
//            *min = flow < *min ? flow : *min;
//            *max = flow > *max ? flow : *max;
//        }
    }

    return averageFlow;
}

//*****************************************************************************/
//    Operation Name: GetPressure()
//
//    Processing: Averages some pressure samples before returning the measured 
//      flow.
//
//    Input Parameters:
//      sensor - the sensor to read the pressure.
//      readingsToAvg - the number of samples to take from the sensor and 
//          average.
//      *min - a pointer to output the minimum reading taken.
//      *max - a pointer to output the maximum reading taken.
//
//    Output Parameters:
//      *min - minimum measurement.
//      *min - maximum measurement.
//
//    Return Values:
//      SHORT - the averaged pressure measured.
//
//    Pre-Conditions:
//
//    Miscellaneous: 
//
//    Requirements:  
//
//*****************************************************************************/
SHORT TestServices::GetPressure
    (
        PressureSensor* sensor, 
        USHORT readingsToAvg,
        SHORT* min, 
        SHORT* max
    )
{
    static const SHORT MAX_PRESSURE_SPREAD = 500;  // 5.00 cmH20

    ASSERTION(readingsToAvg);
  	SHORT averagePressure;
    SHORT minimum;
    SHORT maximum;
    E_Bool stable;

    // continue reading pressure samples until stable readings are made
    do
    {
        for (USHORT i=1; i<=readingsToAvg; i++)
        {
            // read the pressure sensor
//        	SHORT pressure = sensor->GetCurrentReading();

            // compute the average pressure
//            averagePressure = avg(averagePressure, pressure, i);

            // track the minimum and maximum readings
//            if (i == 1)
//                minimum = maximum = pressure;
//            else
//            {
//                minimum = pressure < minimum ? pressure : minimum;
//                maximum = pressure > maximum ? pressure : maximum;
//            }
        }

        // filter out any noise spikes from the average
        stable = (maximum - minimum) < MAX_PRESSURE_SPREAD ? eTrue : eFalse;
    }
    while(!stable);

    // return via pointers the min and max values
    if (min && max)
    {
        *max = maximum;
        *min = minimum;
    }

    return averagePressure;
}

//*****************************************************************************/
//    Operation Name: GetO2V()
//
//    Processing: Average the O2 concentration sensor voltage readings and 
//      return the result. 
//
//    Input Parameters:
//      readingsToAvg - the number of samples to take from the sensor and 
//          average.
//      *min - a pointer to output the minimum reading taken.
//      *max - a pointer to output the maximum reading taken.
//
//    Output Parameters:
//      *min - minimum measurement in A/D counts.
//      *min - maximum measurement in A/D counts.
//
//    Return Values:
//      USHORT - average of all measurements in A/D counts.
//
//    Pre-Conditions:
//
//    Miscellaneous:
//
//    Requirements:  
//
//*****************************************************************************/
USHORT TestServices::GetO2V
    (
        USHORT readingsToAvg,
        USHORT* min, 
        USHORT* max
    )
{
    ASSERTION(readingsToAvg);

  	SHORT averageO2 = 0;

    for (USHORT i=1; i<=readingsToAvg; i++)
    {
        // read the oxygen sensor voltage
//    	SHORT O2 = FiO2Sensor->GetRawCounts();

        // compute the average voltage
        //averageO2 = avg(averageO2, O2, i);

        // track the minimum and maximum readings
//        if (i == 1 && min && max)
//            *min = *max = O2;
//        else if (min && max)
//        {
//            *min = O2 < *min ? O2 : *min;
//            *max = O2 > *max ? O2 : *max;
//        }
    }

//    averageO2 = (SHORT) FiO2Sensor->AvgCounts();

    return averageO2;
}

//*****************************************************************************/
//    Operation Name: ChargeCircuit()
//
//    Processing: Pressurize the patient circuit to the specified pressure. 
//      While pressurizing, track the volume required to attain that pressure.
//
//      Volume is integrated using this formula as a basis. Each time through
//      the loop, accumulate volume based upon the flow and the elapsed time
//      since the last flow sample was taken.
//
//      volume = volume + (flow * (current time - last time))
//
//    Input Parameters:
//      *controller - a pointer to a flow controller (O2 or air).
//      flow - the flow rate used to pressurize.
//      timeLimit - the maximum time to wait for pressure to be attained. 
//      targetPressure - the pressure to charge up to in cmH20 * SCALE.
//
//    Output Parameters:
//      *actualPressure - returns a pressure that was the highest it attained 
//          during the pressurization. To error check the success of the 
//          pressurization the returned pressure will be >= targetPressure.
//      *accVolume - the accumulated volume of air it took to pressurize the 
//          tubing in ml * SCALE.
//
//    Return Values:
//
//    Pre-Conditions: User must have patient wye plugged or pressure will not
//      build up.
//
//    Miscellaneous: 
//
//    Requirements:  
//
//*****************************************************************************/
void TestServices::ChargeCircuit
    (
        FlowController* controller,
        LONG flow,
        UNSIGNED timeLimit,
        SHORT targetPressure,
        SHORT* actualPressure,
        LONG* accVolume
    )
{
    static const int FILTER = 3;    
    int consecutive = 0;

    SHORT pressure = 0;
    LONG inhFlow = 0;
    *actualPressure = 0;
    if (accVolume)
        *accVolume = 0;

    // close the exh valve to trap air within patient circuit
//    ExhController->Close();

    // wait for the valve to fully close off the exh path
//    NU_Sleep(TWO_HUNDRED_MS);

    // time stamp the starting time
//	UNSIGNED startTime = SystemClock::Retrieve();
//    UNSIGNED lastTime = startTime;

    // start flow going to build up pressure
//    controller->Reset(flow);
//    controller->SetDesired(flow);

    // WHILE time limit has not expired AND target pressure not attained
//    while (ElapsedTime(startTime) < timeLimit &&
//           *actualPressure < targetPressure)
//    {
//        // read the inh pressure
////        pressure = InhPressSensor->GetCurrentReading();
//
//        // if the pressure exceeds the maximum a number of consecutive times
//        // then assign a new maximum pressure value
//        if (pressure > *actualPressure)
//        {
//            if (++consecutive >= FILTER)
//            {
//                *actualPressure = pressure;
//                consecutive = 0;
//            }
//        }
//        else
//            consecutive = 0;
//
//        // IF the caller requested a volume
//        if (accVolume)
//        {
//            // read the inh flow sensor
////        	inhFlow = AirFlowSensor->GetCurrentReading();
//
//            // calc elapsed time since the last measurement was taken
//        	UNSIGNED elapsedTime = ElapsedTime(lastTime);
//
//            // integrate the flow to calculate a volume
//            *accVolume += inhFlow * (elapsedTime * MS_PER_TICK);
//
//            // increment the lastTime taking into account clock rollover
//            if (MAXUNSIGNED - lastTime >= elapsedTime)
//                lastTime += elapsedTime;
//            else
//                lastTime = elapsedTime - (MAXUNSIGNED - lastTime);
//        }
//    }

    // shut off air flow
//    AirFlowController->Reset(0);
//    AirFlowController->SetDesired(0);

    // IF the caller requested a volume
    //      divide by 1000 to account for time units being mS instead of S
    if (accVolume)
        *accVolume /= 1000;
}

//*****************************************************************************/
//    Operation Name: BlowerSpinDown()
//
//    Processing: Waits for the blower to spin down to a specific flow level. 
//      By commanding the air flow valve open and monitoring the flow rate as
//      the blower slows down we can determine when it has spun down.
//      This routine will not monitor indefinitely, a maximum time limit
//      for the spin down is set within here. 
//
//      This routine does not turn the blower off; the caller is responsible 
//      for turning blower off. 
//
//      To make diagnostics more responsive to a user pressing cancel, this
//      routine monitors the cancel event. If while waiting for the blower 
//      to spin down the event gets set, the routines does an early return 
//      of eTrue. 
//
//    Input Parameters:
//      targetFlow - the flow at which this blower is deemed spun down in 
//          ml/sec * SCALE.
//
//    Output Parameters:
//      actualFlow - the minimum flow attained in LPM * SCALE. To error check 
//          the success of the spin down the returned flow will be <= targetFlow
//
//    Return Values:
//      E_Bool - true if the routine detected a cancel, false otherwise. 
//
//    Pre-Conditions:
//
//    Miscellaneous:
//
//    Requirements:  
//
//*****************************************************************************/
E_Bool TestServices::BlowerSpinDown(LONG targetFlow, LONG* actualFlow)
{
    static const UNSIGNED MAX_TIME = ONE_SECOND * 20;
    static const int FILTER = 3;    
    int consecutive = 0;

    // time stamp the starting time
//	UNSIGNED startTime = SystemClock::Retrieve();

    // disable the air controller
    // completely open air valve
//    AirFlowController->Disable();
//    AirValve->SetStepPosition(2000);

    // wait for the air valve to open
//    NU_Sleep(ONE_HUNDRED_MS * 2);

//    LONG minFlow = AirFlowSensor->GetCurrentReading();

    // while the max time has not expired and min flow not reached 
//    while (ElapsedTime(startTime) < MAX_TIME && minFlow > targetFlow)
//    {
//        // read the inh flow sensor
//        LONG inhFlow = AirFlowSensor->GetCurrentReading();
//
//        // if the flow under the minimum a number of consecutive times
//        // then assign a new minimum pressure value
//        if (inhFlow < minFlow)
//        {
//            if (++consecutive >= FILTER)
//            {
//                minFlow = inhFlow;
//                consecutive = 0;
//            }
//        }
//        else
//            consecutive = 0;
//
//        // check for a cancel event being set
//#ifdef USE_NUCLEUS
//        STATUS status = TheDiagnosticTask().RetrieveCancelEvent();
//        if (status == NU_NOT_PRESENT) {}
//        else if (status == NU_SUCCESS)
//            return eTrue;
//        else
//            ASSERTION(0);
//#else
//        return eTrue;
//#endif
//    }

    // re-enable the air flow controller
//    AirFlowController->Enable();

//    *actualFlow = (minFlow / LPM_FACTOR);
    return eFalse;
}

//*****************************************************************************/
//    Operation Name: ElapsedTime()
//
//    Processing: Calculates the time elapsed given a starting time.
//
//    Input Parameters:
//      startTime - the start time to calculate the elapsed time from.
//
//    Output Parameters:
//
//    Return Values:
//      UNSIGNED - the elapsed time.
//
//    Pre-Conditions:
//
//    Miscellaneous:
//
//    Requirements:  
//
//*****************************************************************************/
UNSIGNED TestServices::ElapsedTime(UNSIGNED startTime)
{
    UNSIGNED elapsedTime;

//	UNSIGNED currentTime = SystemClock::Retrieve();

//	if (currentTime >= startTime)
//		elapsedTime = currentTime - startTime;
//	else
//		elapsedTime = MAXUNSIGNED - startTime + currentTime;

	return(elapsedTime);
}

//*****************************************************************************/
//    Operation Name: DelayNoSleep()
//
//    Processing: Delays a number of ticks. 
//
//      WARNING: This delay does not release task control and can starve lower
//      priority tasks of CPU time.
//
//    Input Parameters:
//      delay - the amount to delay in ticks.
//      startTime - the starting time to base our delay on. If one is not 
//          provided, the current time is used. 
//
//    Output Parameters:
//
//    Return Values:
//
//    Pre-Conditions:
//
//    Miscellaneous:
//
//    Requirements:  
//
//*****************************************************************************/
void TestServices::DelayNoSleep(UNSIGNED delay, UNSIGNED startTime)
{
//    if (!startTime)
//	    startTime = SystemClock::Retrieve();

    while (ElapsedTime(startTime) < delay);
}

//*****************************************************************************/
//    Operation Name: DelayWithCancel()
//
//    Processing: Delay a certain number of ticks while periodically checking 
//      to see if the cancel event is set. The task will sleep for a short 
//      duration and then wake up and check the cancel event. If the event is 
//      set, the delay routine will exit. Otherwise, will continue to delay 
//      until the time limit expires. 
//
//    Input Parameters:
//      delay - the amount to delay in ticks.
//
//    Output Parameters:
//
//    Return Values:
//      E_Bool - true is cancel event occurred while delaying, false otherwise.
//
//    Pre-Conditions:
//
//    Miscellaneous:
//
//    Requirements:  
//
//*****************************************************************************/
E_Bool TestServices::DelayWithCancel(UNSIGNED delay)
{
//    UNSIGNED startTime = SystemClock::Retrieve();

    // while time limit has not expired
//    while (ElapsedTime(startTime) < delay)
//    {
//        // check the cancel event. If set return true, else sleep a while
//#ifdef USE_NUCLEUS
//        STATUS status = TheDiagnosticTask().RetrieveCancelEvent();
//        if (status == NU_NOT_PRESENT)
//            NU_Sleep(1);
//        else if (status == NU_SUCCESS)
//            return eTrue;
//        else
//            ASSERTION(0);
//#else
//        return eTrue;
//#endif
//    }
    
    return eFalse;
}

//*****************************************************************************/
//    Operation Name: SendResultsOutRS232()
//
//    Processing: Send all the test results out the RS-232 port.
//
//    Input Parameters:
//
//    Output Parameters:
//
//    Return Values:
//
//    Pre-Conditions:
//
//    Miscellaneous:
//
//    Requirements:  
//
//*****************************************************************************/
void TestServices::SendResultsOutRS232(void)
{
#ifndef NO_HARDWARE 

    static const int LENGTH = 255;

    // if we are being controlled by a remote device
//    if (Parser::S_GetInstance()->RemoteEnableStateIs())
//    {
        CHAR str[LENGTH];
//        CoreStream os(str, LENGTH);
        ResultIterator iterator;
        Result::Info info;

        // build date and time string
//        os << Clock::S_GetInstance()->AscDate() << ",";
//        os << Clock::S_GetInstance()->AscTime();
//        os << endl << ends;
//
//        // send string out RS-232 port
//        SendString(str);
        
        // if there are results to cycle through
//        if (iterator.Valid())
//        {
//            // cycle through all test results
//            do
//            {
//                // get result information structure
//                iterator->GetInfo(&info);
//
//                // if this test result was used during the test sequence
//                if (info.used)
//                {
//                    // reset stream to the beginning
//                    os.seekp(0);
//
//                    os << '"' << info.file << '"' << ",";
//                    os << '"' << info.criterionName << '"' << ",";
//                    os << info.result << ",";
//                    switch (info.passes)
//                    {
//                        case 0: os << "F,"; break;
//                        case 1: os << "P,"; break;
//                        default:
//                            os << "UNKNOWN,";
//                    }
//                    os << info.lower << ",";
//                    os << info.upper << ",";
//                    switch (info.constraint)
//                    {
//                        case eGreaterThan: os << ">,"; break;
//                        case eGreaterThanOrEqual: os << ">=,"; break;
//                        case eLessThan: os << "<,"; break;
//                        case eLessThanOrEqual: os << "<=,"; break;
//                        case eRange: os << "<=x<=,"; break;
//                        case eEqual: os << "=,"; break;
//                        default:
//                            os << "UNKNOWN,";
//                    }
//                    switch (info.units)
//                    {
//                        case 0: os << "none,"; break;
//                        case emL: os << "mL,"; break;
//                        case eL: os << "L,"; break;
//                        case eLPM: os << "LPM,"; break;
//                        case ecmH2O: os << "cmH2O,"; break;
//                        case ePerCent: os << "%,"; break;
//                        case eSec: os << "S,"; break;
//                        case eSteps: os << "steps,"; break;
//                        case eV: os << "V,"; break;
//                        case emV: os << "mV,"; break;
//                        case euV: os << "uV,"; break;
//                        case eA: os << "A,"; break;
//                        case eA2DCnts: os << "A/D Cnts,"; break;
//                        case eDACCnts: os << "DAC Cnts,"; break;
//                        case emlcmH2O: os << "ml/cmH2O,"; break;
//                        case eDegrees: os << "degrees,"; break;
//                        default:
//                            os << "UNKNOWN,";
//                    }
//                    os << info.code << ",";
//                    os << info.used;
//                    os << endl << ends;
//
//                    // send string out RS-232 port
//                    SendString(str);
//
//                    // don't overrun the RS-232 port with too much information
//                    NU_Sleep(FIFTY_MS);
//                }
//            } while (iterator++);
//        }
//    }
#endif  // NO_HARDWARE
}



