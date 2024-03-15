# ARM Timer for BCM2835 (Raspberry Pi)

### Description
- Adds functions to start and read ARM timer which runs at higher speeds (250 MHz) and gives finer timing resolution compared to system timer (runs at 1 MHz). However, its clock source can be dynamically change (ie low power mode etc), hence timing accuracy may change based on this behavior of the system.  
- Suggested use for short busy waits (ie less than 5000 nsec). Based on experiments, ~180-200 nsec delays seem achievable, shorter delays seem not possible ([see output of test program](test-out.txt))  
- Tested on, Raspberry Pi 1 Model B, Raspbian Buster, kernel 5.10.63.  

### Usage
- `#include "arm_timer.h"` in your code along with `#include <bcm2835.h>`.  
- First `bcm2835_init()` then call `bcm2835_arm_timer_init()` and `bcm2835_arm_timer_enable()`.  
- See [`test.c`](test.c) for further detail.  
- After build, run with sudo.  

### Resources
- [Reference work](https://github.com/brianddk/reddit/blob/fd6d7ad/c/bcmticks.c)  
- Based on [bcm2835 library](http://www.airspayce.com/mikem/bcm2835/)  
- [ARM peripheral details document](https://www.raspberrypi.org/app/uploads/2012/02/BCM2835-ARM-Peripherals.pdf)  
- Origin repo at github.com/eardali  

### License
Distributed under the terms of the MIT license.  
