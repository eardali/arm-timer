#ifndef MYARM_TIMER_H_
#define MYARM_TIMER_H_

int bcm2835_arm_timer_init(void);
int bcm2835_arm_timer_enable(void);
int bcm2835_arm_timer_disable(void);
uint32_t bcm2835_arm_timer_read(void);
void bcm2835_arm_timer_delayNanosec(uint32_t nanos);

#endif