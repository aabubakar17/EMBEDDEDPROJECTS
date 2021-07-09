/* ------------------------------------------
       ECS642/ECS714 Lab Work

   Header for configuration and control of red, green, blue LEDs
   on the FRDM KL25Z development board
     ---------------------------------------- */

#ifndef TRICOLORLEDPWM_H
#define TRICOLORLEDPWM_H

// red: pin 18, port B. FTM2_CH0
#define TPM2_CHAN1 (0)
#define PWM2_PORT1 (PORTB)
#define PWM2_PIN1 (18)
#define PWM2_ALT1 (3)

// green: pin 19, port B. FTM2_CH1
#define TPM2_CHAN2 (1)
#define PWM2_PORT2 (PORTB)
#define PWM2_PIN2 (19)
#define PWM2_ALT2 (3)

// blue: pin 1, port D. FTM0_CH1
#define TPM0_CHAN1 (1)
#define PWM0_PORT1 (PORTD)
#define PWM0_PIN1 (1)
#define PWM0_ALT1 (4)

// functions
void configureLEDforPWM() ; // configure PWM alternatives for LEDs

#endif
