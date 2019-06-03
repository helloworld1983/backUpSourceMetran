//*****************************************************************************/
//$COMMON.H$
//   File Name:  Result.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: Result
//      To store the results of a particular Diagnostic self test parameter.
//*****************************************************************************/

#ifndef RESULT_H
#define RESULT_H

#include <string.h>
#include "CommonTypes.h"
#include "FailureCodes.h"
#include "StateMachine.h"
#include "StringTable.h"

#include "DebugTrace.h"

enum E_ConstraintType 
{
    eGreaterThan,
    eGreaterThanOrEqual,
    eLessThan,
    eLessThanOrEqual,
    eRange,
    eEqual
};

static const UCHAR STRING_SIZE = 16;  

class Result 
{
public:
    Result
        (
            CHAR* file,
            CHAR* criterionName,
            E_ConstraintType constraint,
            USHORT scale,
            CHAR* lower, 
            CHAR* upper,
            E_StringID units,
            E_FailureCodes code
        );
    ~Result() { } 

    // inheriting from EventData allows new and delete
    struct Info : public EventData
    {
        const CHAR* file;
        const CHAR* criterionName;
        E_ConstraintType constraint;
        USHORT scale;
        CHAR* lower;
        CHAR* upper;
        CHAR* result;
        E_Bool passes;
        E_StringID units;
        E_FailureCodes code;
        USHORT used;
    };

    const CHAR* const File;
    const CHAR* const CriterionName;
    const USHORT Scale;
    const E_ConstraintType Constraint;
    const E_StringID Units;
    const E_FailureCodes FailureCode;

    void ResetUsed(void) { Used = 0; }
    void ResetUsedCtr(void) { UsedCtr = 0; }
    USHORT GetUsed(void) { return Used; }
    E_Bool WasUsed(void) { return Used ? eTrue : eFalse; }
    void SetPasses(E_Bool p) { Passes = p; }
    void SetResult(CHAR* r) 
    { 
        strcpy(TestResult, r);
        if (!Used)
            Used = ++UsedCtr;
    }
    void SetLimits(CHAR* lower, CHAR* upper = NULL) 
    { 
        strcpy(LowerLimit, lower);
        strcpy(UpperLimit, upper);
    }

    void ResetAll(void);
    void GetInfo(Info*);

    friend class ResultIterator;
protected:
private:
    // prevent copying
    Result(const Result&);
    void operator=(Result&);

    void Insert(void);
    
    //$COMMON.ATTRIBUTE$
    //  Name: TestResult
    //  Description: The result of a test in string format.
    //  Units: 
    //  Range: 
    CHAR TestResult[STRING_SIZE];
    
    //$COMMON.ATTRIBUTE$
    //  Name: LowerLimit
    //  Description: The lower limit of of a test in string format.
    //  Units: 
    //  Range: 
    CHAR* LowerLimit;

    //$COMMON.ATTRIBUTE$
    //  Name: UpperLimit
    //  Description: The upper limit of of a test in string format.
    //  Units: 
    //  Range: 
    CHAR* UpperLimit;
    
    //$COMMON.ATTRIBUTE$
    //  Name: Used
    //  Description: A number that deterines if a result has been stored 
    //      into an instance of this class. Will be 0 if not used, otherwise
    //      contains the sequence number (order of result used).
    //  Units: 
    //  Range: 
    USHORT Used;
    
    //$COMMON.ATTRIBUTE$
    //  Name: UsedCtr
    //  Description: Tracks the number of results stored for all instances of
    //      Result.
    //  Units: 
    //  Range: 
    static USHORT UsedCtr;

    //$COMMON.ATTRIBUTE$
    //  Name: Passes
    //  Description: Indicates whether the result passed (eTrue) or failed
    //      (eFalse). 
    //  Units: 
    //  Range: 
    E_Bool Passes;
    
    //$COMMON.ATTRIBUTE$
    //  Name: S_Head
    //  Description: A pointer to the first Result in the linked list.
    //  Units: 
    //  Range: 
    static Result* S_Head;
    
    //$COMMON.ATTRIBUTE$
    //  Name: S_Tail
    //  Description: A pointer to the last Result in the linked list.
    //  Units: 
    //  Range: 
    static Result* S_Tail;

    //$COMMON.ATTRIBUTE$
    //  Name: Next
    //  Description: A pointer to the next Result in the linked list.
    //  Units: 
    //  Range: 
    Result* Next;
};

// Iterator class that eases access to all the Result types.
class ResultIterator
{
public:
    ResultIterator() { Reset(); }
    ~ResultIterator() {}

    // Increment the iterator to the next Result. Returns FALSE when
    // end of list has been found.        
    
    E_Bool operator++(int) 
    { 
        if (Current->Next)
        {
            Current = Current->Next;
            return eTrue;
        }
        return eFalse;
    }
    // Resets the iterator to start of list when assigned 0.
    void operator=(const CHAR num) 
    {
        ASSERTION(!num);
        Reset();
    }
    Result* operator->() const { return Current; }
    E_Bool Valid(void) { return Current ? eTrue : eFalse; }
    
private:
    // prevent copying
    ResultIterator(const ResultIterator&);
    void operator=(ResultIterator&);

    void Reset(void) { Current = Result::S_Head; }

    //$COMMON.ATTRIBUTE$
    //  Name: Current
    //  Description: Allows user to move though all instances of Result.
    //  Units: 
    //  Range: 
    Result* Current;
};

#endif //RESULT_H

