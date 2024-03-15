/* Reference work: https://github.com/brianddk/reddit/blob/fd6d7ad/c/bcmticks.c
 * Copyright 2023 brianddk at github https://github.com/brianddk
 * Apache 2.0 License https://www.apache.org/licenses/LICENSE-2.0
 *
 * Standard function in bcm2835 (https://www.airspayce.com/mikem/bcm2835/index.html)
 * bcm2835_st_read(), reads from system timer which based on 1MHz clock
 * ARM side timer functions are
 * This timer/counter runs at higher speeds and gives finer timing resolution
 * However, its clock source can be dynamically change (ie low power mode etc),
 * hence timing accuracy may change based on this behavior of the system.
 * Refer to pg#196 of bcm2835 arm peripheral document, chapter #14 Timer (ARM side) further details
 * https://www.raspberrypi.org/app/uploads/2012/02/BCM2835-ARM-Peripherals.pdf
 *
 * Distributed under the terms of the MIT license.
 * Copyright 2024 github.com/eardali
 * Initial version, March 2024
 */
#include <sys/mman.h>
#include <stdio.h>
#include <bcm2835.h>

//#define DEBUG 1   //uncomment to print out some debug logs
#ifdef DEBUG
#define DEBUG_PRINT(...) printf(__VA_ARGS__)
#else
#define DEBUG_PRINT(...)
#endif

#define ARM_TIMER_BASE       0x0000B000
#define ARM_TIMER_SIZE       0x424
#define ARM_TIMER_COUNTER    0x420 //free running counter, based on sys_clk (250Mhz for RP1) and pre-scaler
#define ARM_TIMER_VALUE      0x404 //std timer (SP804), based on APB clk and pre-divider and pre-scaler
#define ARM_TIMER_LOAD       0x400
#define ARM_TIMER_CONTROL    0x408
#define ARM_TIMER_COUNTER_EN  0x00000200 //this sets all pre-scales divide by 1, enable free run counter only, not SP804 timer
#define ARM_TIMER_CONTROL_RST 0x003E0020 //value after reset


volatile uint32_t *bcm2835_arm_timer = (uint32_t *)MAP_FAILED;

int bcm2835_arm_timer_init(void)
{
    bcm2835_arm_timer = bcm2835_peripherals + ARM_TIMER_BASE/4; //division by 4 is necessary since we are adding it to a pointer, 32bit, 4byte, otherwise jumps further
    
    return 1; //success in bcm2835 lib
}

int bcm2835_arm_timer_enable(void)
{
    volatile uint32_t* paddr = bcm2835_arm_timer + ARM_TIMER_CONTROL/4;
    uint32_t control = bcm2835_peri_read(paddr);
    DEBUG_PRINT("Value of arm timer control register: %08X\n", control);
    uint32_t enable = ARM_TIMER_COUNTER_EN;
    
    if (ARM_TIMER_COUNTER_EN != (control & ARM_TIMER_COUNTER_EN))
        bcm2835_peri_write(paddr, enable);
    
    return 1;
}

int bcm2835_arm_timer_disable(void)
{
    volatile uint32_t* paddr = bcm2835_arm_timer + ARM_TIMER_CONTROL/4;
    uint32_t disable = ARM_TIMER_CONTROL_RST;
    
    bcm2835_peri_write(paddr, disable);
    
    return 1;
}

//read free running counter
uint32_t bcm2835_arm_timer_read(void)
{
    volatile uint32_t* paddr = bcm2835_arm_timer + ARM_TIMER_COUNTER/4;
    uint32_t counter = bcm2835_peri_read(paddr);
    DEBUG_PRINT("Value of arm free running counter: %08X\n", counter);
    return counter;
}

//supposed to be each tick at 4 nsec resolution (since arm timer enabled to run at sys_clk which is 250 MHz for RP1)
//for longer waits (ie 5us and above), use bcm2835_delayMicroseconds()
//based on experiments, ~180-200 nsec delays seem achievable, shorter delays seem not possible
void bcm2835_arm_timer_delayNanosec(uint32_t nanos)
{
    if (nanos > 5000) //force to use bcm2835_delayMicroseconds()
    {
        bcm2835_delayMicroseconds(nanos/1000);  //coarse conversion to nano to micro
        return;
    }
    
    uint32_t start =  bcm2835_arm_timer_read();
    uint32_t compare = start + nanos;

    while(bcm2835_arm_timer_read() < compare)
	;
}


/*below are some experimental functions to understand SP804 timer*/

//read SP804 timer value
uint32_t bcm2835_arm_timer_SP804_read(void)
{
    volatile uint32_t* paddr = bcm2835_arm_timer + ARM_TIMER_VALUE/4;
    uint32_t counter = bcm2835_peri_read(paddr);
    DEBUG_PRINT("Value of SP804 timer: %08X\n", counter);
    return counter;
}

//load SP804 timer value
//seems load register is 0x00 at reset, so that even start SP804 timer, it doesn't count, need to load first
int bcm2835_arm_timer_SP804_load(void)
{
    volatile uint32_t* paddr = bcm2835_arm_timer + ARM_TIMER_LOAD/4;
    uint32_t load = 0xFFFF; //on chip reset its configured to be 16bit timer
    DEBUG_PRINT("Loading of SP804 timer: %08X\n", load);
    bcm2835_peri_write(paddr, load);
    return 1;
}
