#include <MKL25Z4.h>

/* ------------------------------------------
       ECS642/ECS714 Lab Work

   SysTick timer functions
     * Init_SysTick  - set interrupt frequency
     * waitSysTickCounter - the delay function
  -------------------------------------------- */

void Init_SysTick(uint32_t ticksPerSec) ;
void waitSysTickCounter(int ticks) ;
