/* ------------------------------------------
       ECS642/ECS714 Lab Work

   Configuration of PWM control of red, green, blue LEDs on 
   FRDM KL25Z development board
   ---------------------------------------- */
#include <MKL25Z4.h>
#include "../include/triColorLedPwm.h"

// Create a bit mask (32 bits) with only bit x set
#define MASK(x) (1UL << (x))

/*----------------------------------------------------------------------------
  Configuration
  The pin control registers (PCR) are set to enable PWM control of LEDs. 
*----------------------------------------------------------------------------*/

void configureLEDforPWM(void) {
    // Enable clock to ports B and D
    SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTD_MASK;
    
    // Set the Pin Control Register 
    // Set Pin on PWM_PORT as TPM o/p
    PWM2_PORT1->PCR[PWM2_PIN1] &= ~PORT_PCR_MUX_MASK;          
    PWM2_PORT1->PCR[PWM2_PIN1] |= PORT_PCR_MUX(PWM2_ALT1);

    PWM2_PORT2->PCR[PWM2_PIN2] &= ~PORT_PCR_MUX_MASK;          
    PWM2_PORT2->PCR[PWM2_PIN2] |= PORT_PCR_MUX(PWM2_ALT2);

    // Set the Pin Control Register 
    // Set Pin on PWM_PORT as TPM o/p
    PWM0_PORT1->PCR[PWM0_PIN1] &= ~PORT_PCR_MUX_MASK;          
    PWM0_PORT1->PCR[PWM0_PIN1] |= PORT_PCR_MUX(PWM0_ALT1);  
}
