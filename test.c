//github.com/eardali, March 2024
#include <stdio.h>
#include <unistd.h>
#include <bcm2835.h>
#include "arm_timer.h"
 
#define N 10

int main()
{
	uint64_t t1, t2;
    uint32_t ta1, ta2, buf[N];
    int i, delay=100;
    
    if (!bcm2835_init())
        return 1;
    
    //init free run counter and enable
    bcm2835_arm_timer_init();
    bcm2835_arm_timer_enable();

    printf("Expected delay for each round: %d us\n", delay);
    for(i=0; i<N; i++)
    {
        t1 = bcm2835_st_read(); //get system timer value
        ta1 = bcm2835_arm_timer_read(); //get ARM free running counter value
        bcm2835_delayMicroseconds(delay);
        ta2 = bcm2835_arm_timer_read();
        t2 = bcm2835_st_read();
        printf("Round#: %d, Number of passed cycles, system_timer: %llu, arm_timer: %u, ratio: %.2f\n", i, t2-t1, ta2-ta1, (ta2-ta1)*1.0/(t2-t1));
    }
    
    //experiment on consecutive read delays
    for(i=0; i<N; i++)
    {
        buf[i] = bcm2835_arm_timer_read();
    }
    
    printf("Arm timer value for consecutive reads:\n");
    for(i=0; i<N; i++)
    {
        printf("%u, ", buf[i]);
    }
    printf("\n");
    
    printf("Arm timer value difference for consecutive reads:\n");
    for(i=0; i<N-1; i++)
    {
        printf("%u, ", buf[i+1]-buf[i]);
    }
    printf("\n");
    
    //make a short delay at nanosec resolution, not suggest to use long waits
    bcm2835_arm_timer_delayNanosec(300);
    
    //disable, restore control register to reset state
    bcm2835_arm_timer_disable();
    
    return 0;
}
