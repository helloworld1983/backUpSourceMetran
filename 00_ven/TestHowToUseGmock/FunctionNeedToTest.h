#ifndef FUNCTIONNEEDTOTEST_H_
#define FUNCTIONNEEDTOTEST_H_

//Declare function that used in testing
//Not need to declare body function
int GpioRead(int portNumber, int pinNumber);

void GpioWrite(int portNumber, int pinNumber , int value);

int GetTime(void);

void myFunction(int timeout);


//test My function
void myFunction(int timeout)
{
    int startTime = GetTime();
    while(GetTime() - startTime < timeout)
    {
        if (GpioRead(0, 1) == 0)
            GpioWrite(1, 1, 0);
        else
            GpioWrite(1, 1, 1);
    }
}


#endif /* FUNCTIONNEEDTOTEST_H_ */
