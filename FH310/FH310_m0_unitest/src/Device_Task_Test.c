/*
 * Device_Task_Test.c
 *
 *  Created on: Jun 17, 2016
 *      Author: WINDOWS
 */

#include "unity.h"
#include "unity_fixture.h"
#include "device_task.h"

TEST_GROUP(Device_Task);

//sometimes you may want to get at local data in a module.
//for example: If you plan to pass by reference, this could be useful
//however, it should often be avoided
extern int Counter;

TEST_SETUP(Device_Task)
{

}

TEST_TEAR_DOWN(Device_Task)
{
}

TEST(Device_Task, add)
{
	TEST_ASSERT_EQUAL(0, add(1,2));
}



