//$COMMON.CPP$
//   File Name:  Result.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: To store the results of a particular Diagnostic self test
//      parameter.
//
//   Interfaces:
//      An iterator is available to cycle through all Result instances, 
//      called ResultIterator.
//
//      ResultIterator iterator;
//      Result::Info info;
//      do
//      {
//          info = iterator->GetInfo();
//      } while (iterator++);
//
//   Restrictions:
//      Read the header comments for ResetAll() for more information 
//      regarding usage of this class. 
/******************************************************************************/

#include "Result.h"

Result* Result::S_Head = NULL;
Result* Result::S_Tail = NULL;
USHORT Result::UsedCtr = 0;

//*****************************************************************************/
//    Operation Name: Result()
//
//    Processing: Class constructor.
//
//    Input Parameters:
//      file - name of the file, which the file should be name what the
//          test is.
//      criterionName - name of the criterion.
//      constraint - the type of range checking to perform. When passing in the
//          constraint, we need it to indicate when a test passes. e.g. if 100 
//          or greater means a test passed, the lower argument will be 100 and 
//          constraint eGreaterThan.
//      lower - a string representation of the lower boundary for the test 
//          parameter.
//      upper - a string representation of the upper boundary for the test 
//          parameter.
//      units - the string ID for the units text.
//      code - a failure code if the test criterion fails. 
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
Result::Result
    (
        CHAR* file,
        CHAR* criterionName,
        E_ConstraintType constraint,
        USHORT scale,
        CHAR* lower, 
        CHAR* upper,
        E_StringID units,
        E_FailureCodes code
    ) :
    File(file),
    Units(units),
    CriterionName(criterionName),
    Constraint(constraint),
    Scale(scale),
    FailureCode(code)
{
    Insert();
    Passes = eFalse;
    Used = 0;
    LowerLimit = lower;
    UpperLimit = upper;
}

//*****************************************************************************/
//    Operation Name: ResetAll()
//
//    Processing: The method is utilized to reset all Result instances to a 
//      known starting point before a self test is performed. Once the test is 
//      complete, the caller can determine which Result's have been used by
//      examining the 'used' attribute. Any instance with used != 0 was used 
//      during the test. Therefore, it is critical to call this method before
//      starting each self test, otherwise there is no way to determine which
//      Result objects belong to a certain self test. 
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
void Result::ResetAll(void)
{
    ResultIterator iterator;

    if (iterator.Valid())
    {
        do
        {
            iterator->SetResult((CHAR*)"0");
            iterator->SetPasses(eFalse);
            iterator->ResetUsed();
        } while(iterator++);
        iterator->ResetUsedCtr();
    }
}

//*****************************************************************************/
//    Operation Name: GetInfo()
//
//    Processing: Return a pointer to a structure with information regarding an 
//      instance of Result. 
//
//    Input Parameters:
//
//    Output Parameters:
//      Result::Info* - a pointer to a structure containing the information.
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
void Result::GetInfo(Result::Info* info)
{
    info->file = File;
    info->criterionName = CriterionName;
    info->constraint = Constraint;
    info->scale = Scale;
    info->lower = LowerLimit;
    info->upper = UpperLimit;
    info->result = TestResult;
    info->passes = Passes;
    info->units = Units;
    info->code = FailureCode;
    info->used = Used;
}

//*****************************************************************************/
//    Operation Name: Insert()
//
//    Processing: Inserts a Result this pointer into a linked list.
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
void Result::Insert(void)
{
    // IF this is the first Criterion to be created
    //      assign the head of the linked list
    //      assign the tail of the linked list
    // ELSE 
    //      assign the old tail's Next pointer to this Criterion
    //      assign a new tail
    // set the Next pointer to NULL
    if (!S_Head)
    {
        S_Head = this;
        S_Tail = this;
    }
    else
    {
        S_Tail->Next = this;
        S_Tail = this;
    }
    this->Next = NULL;
}



