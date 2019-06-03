//*****************************************************************************/
//$COMMON.H$
//   File Name:  Criterion.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: Criterion
//
//   Purpose: To provide a common class which provides test criterion checking
//      and test result storage. Each Criterion should only be used for a 
//      single testable attribute. This way, each test result can be stored
//      and retrieved later.
//
//   Interfaces:
//      If we need to test that the air flow sensor is within 5lpm +/-1, 
//      we can use this class to store the limits and the measured result. A 
//      simple macro helps to use the class more effectively. In this case, 
//      the macro definition would be:
//
//      CRITERION(AirFlowSensor, int, 10, eRange, 40, 60, eFC_AirFlowSensor)
//
//      AirFlowSensor - the name of the test criteria
//      int - the data type for the criteria data
//      10 - the scale factor for the result (e.g 123 means 12.3 with a scale
//          factor of 10)
//      eRange - specifies the criteria test must be between two values
//      40 - is the lower value limit. Taking into account the scale factor 40
//          means 4.0.
//      60 - is the upper value limit. Taking into account the scale factor 60
//          means 6.0.
//      eFC_AirFlowSensor - is the failure code generated if the test fails
//
//      For test criteria other than eRange, the upper value is ignored. For
//      instance, a test of the same flow sensor being less than 10lpm would 
//      only require a lower parameter, the upper can be set to 0.
//
//      CRITERION(AirFlowSensor, int, 0 eLessThan, 10, 0, eFC_AirFlowSensor)
//
//      To access the criterion object, use the name supplied in the macro:
//
//      E_Bool pass = AirFlowSensor->Passes(9);
//
//  Restrictions:
//      Only use the macro inside a function (function scope) or block (local
//      scope), never as a global (file scope) or class member (class scope). 
//      This is due to a Nucleus Plus OS limitation where static constructors 
//      are not called. Using inside a function or block also limits the 
//      possibility of using a Criterion for more than one testable attribute, 
//      which would overwrite the previous test results.
//
//      A Criterion can be used in a class that is used by multiple self tests. 
//      The way to determine which Criterion is used in which self test is 
//      to use the Result class (see Result.cpp for more info).
//*****************************************************************************/

#ifndef CRITERION_H
#define CRITERION_H

#include "CommonTypes.h"
#include "FailureCodes.h"
#include "Result.h"
#include "DebugTrace.h"

// Macro used to make using Criterion easier to use. Don't use this macro in 
// file or class scope (see above).
#define CRITERION(name, type, scale, constraint, lower, upper, units, code) \
    static Criterion<type>* name = new Criterion<type> (__FILE__, #name, scale,\
    	constraint, lower, upper, units, code); 

template <class T>
class Criterion 
{
public:
    Criterion(CHAR*, CHAR*, USHORT, E_ConstraintType, T, T, E_StringID, 
        E_FailureCodes);
    ~Criterion() { }

    E_Bool Passes(T);
    T GetResult(void) { return Value; }
    void SetLimits(T lower, T upper);
    void GetInfo(Result::Info* info) { TestResult.GetInfo(info); }
private:
    // prevent copying
    Criterion(const Criterion&);
    void operator=(Criterion&);

    CHAR* ToString(T value, CHAR* string, USHORT scale);
    CHAR* ToString(T value, USHORT scale)
    {
        CHAR* string = new CHAR[STRING_SIZE];
        return ToString(value, string, scale);
    }
	inline T Abs(T j) { return j<0 ? -j: j; }
    
    //$COMMON.ATTRIBUTE$
    //  Name: TestResult
    //  Description: Where the criterion results and other info is stored.
    //  Units: 
    //  Range: 
    Result TestResult;

    //$COMMON.ATTRIBUTE$
    //  Name: LowerLimit
    //  Description: The lower limit of of a test.
    //  Units: 
    //  Range: 
    T LowerLimit;
    
    //$COMMON.ATTRIBUTE$
    //  Name: UpperLimit
    //  Description: The upper limit of of a test.
    //  Units: 
    //  Range: 
    T UpperLimit;

    //$COMMON.ATTRIBUTE$
    //  Name: Value
    //  Description: The last tested Value.
    //  Units: 
    //  Range: 
    T Value;
};

//*****************************************************************************/
//    Operation Name: Criterion()
//
//    Processing: Class constructor. The only constraint that uses both 
//      upper and lower arguments is eRange. All others use just the lower to 
//      determine if the criterion passes.
//
//    Input Parameters:
//      file - name of the file, which the file should be name what the
//          test is.
//      criterionName - name of the criterion.
//      constraint - the type of range checking to perform. When passing in the
//          constraint, we need it to indicate when a test passes. e.g. if 100 
//          or greater means a test passed, the lower argument will be 100 and 
//          constraint eGreaterThan.
//      lower - the lower boundary for the test parameter.
//      upper - the upper boundary for the test parameter.
//      code - a failure code if the test criterion fails. 
//
//    Output Parameters:
//
//    Return Values:
//
//    Pre-Conditions: T must be an integral type. 
//
//    Miscellaneous:
//
//    Requirements:  
//
//*****************************************************************************/
template <class T> Criterion<T>::Criterion
    (
        CHAR* file,
        CHAR* criterionName,
        USHORT scale, 
        E_ConstraintType constraint,
        T lower, 
        T upper,
        E_StringID units,
        E_FailureCodes code
    ) :
    LowerLimit(lower),
    UpperLimit(upper),
    TestResult
        (
            file,
            criterionName,
            constraint,
            scale,
            ToString(lower, scale), 
            ToString(upper, scale),
            units,
            code
        )
{
}

//*****************************************************************************/
//    Operation Name: Passes()
//
//    Processing: Checks to see if the value is within the pass/fail limit
//      boundaries. 
//
//    Input Parameters:
//      v - a value of the correct type to check against the limits.
//
//    Output Parameters:
//
//    Return Values: Returns eTrue if value passes limit boundaries, eFalse 
//      otherwise.
//
//    Pre-Conditions:
//
//    Miscellaneous:
//
//    Requirements:  
//
//*****************************************************************************/
template <class T> E_Bool Criterion<T>::Passes(T v)
{
    E_Bool passes = eFalse;
    CHAR string[STRING_SIZE];

    Value = v; 
    TestResult.SetResult(ToString(Value, string, TestResult.Scale));

    // check pass/fail depending on test method
    switch (TestResult.Constraint)
    {
        case eGreaterThan:
            passes = (Value > LowerLimit) ? eTrue : eFalse;
            break;
        case eGreaterThanOrEqual:
            passes = (Value >= LowerLimit) ? eTrue : eFalse;
            break;
        case eLessThan:
            passes = (Value < LowerLimit) ? eTrue : eFalse;
            break;
        case eLessThanOrEqual:
            passes = (Value <= LowerLimit) ? eTrue : eFalse;
            break;
        case eRange:
            passes = 
                (LowerLimit <= Value && Value <= UpperLimit) ? eTrue : eFalse;
            break;
        case eEqual:
            passes = (Value == LowerLimit) ? eTrue : eFalse;
            break;
        default:
            ASSERTION(false);
    }

    TestResult.SetPasses(passes);
    return passes;
}

//*****************************************************************************/
//    Operation Name: ChangeLimits()
//
//    Processing: Change the upper and lower limits. 
//
//    Input Parameters:
//      lower - the new lower limit. 
//      upper - the new upper limit.
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
template <class T> void Criterion<T>::SetLimits(T lower, T upper)
{
    CHAR lowerStr[STRING_SIZE];
    CHAR upperStr[STRING_SIZE];

    LowerLimit = lower;
    UpperLimit = upper;
    TestResult.SetLimits
        (
            ToString(lower, lowerStr, TestResult.Scale), 
            ToString(upper, upperStr, TestResult.Scale)
        );
}

//*****************************************************************************/
//    Operation Name: ToString()
//
//    Processing: Converts a value to a string. The caller must supply the 
//      storage for the string.
//
//    Input Parameters:
//      value - the value to convert to a string.
//      string - the converted string.
//      scale - the scale used on the value (e.g. 123 means 1.23 with a 
//          scale of 100).
//
//    Output Parameters:
//      string - will point to the converted string. 
//
//    Return Values:
//      CHAR* - returns a pointer to the converted string.
//
//    Pre-Conditions:
//
//    Miscellaneous:
//
//    Requirements:  
//
//*****************************************************************************/
template <class T> CHAR* Criterion<T>::ToString(T value, CHAR* string, USHORT scale)
{
//    CoreStream os(string, STRING_SIZE);
//
//    // IF a scale factor is used
//    //      build up a string with the decimal point
//    // ELSE
//    //      no decimal point required
//    if (scale)
//    {
//        if (value < 0)
//            os << "-";
//        os << Abs(value / scale) << ".";
//
//        ULONG precision = 0;
//        USHORT tempScale = scale;
//        while (tempScale / 10)
//        {
//            tempScale /= 10;
//            precision++;
//        }
//
//        os.width(precision);
//        os << Abs(value % scale) << ends;
//    }
//    else
//        os << value << ends;

    return string;
}

#endif //CRITERION_H



