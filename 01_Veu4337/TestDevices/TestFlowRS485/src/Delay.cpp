#include "Delay.h"

#define SYSTEM_FRE_KHZ          (204)
#define CYCLE_PER_INSTRUCTION   (4)
#define INSTRUCTION_FRE_KHZ     (SYSTEM_FRE_KHZ/CYCLE_PER_INSTRUCTION)

void delay_ms(unsigned long ms)
{
    for(unsigned long i = 0; i < ms; i++)
    {
        for(long j = 0; j < INSTRUCTION_FRE_KHZ; j++)
            asm("nop"); //do nothing
    }

    return;
}

void delay_us(unsigned long us)
{

    for(unsigned long i = 0; i < us; i++)
    {
        for(long j = 0; j < INSTRUCTION_FRE_KHZ; j++)
            asm("nop"); //do nothing
    }

    return;
}
