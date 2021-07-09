/* ------------------------------------------
       ECS642/ECS714 Lab Work

   Configuration and control of red, green, blue LEDs on 
   FRDM KL25Z development board
     ---------------------------------------- */
#include <MKL25Z4.h>
#include "triColorLed.h"

// Create a bit mask (32 bits) with only bit x set
#define MASK(x) (1UL << (x))

/*----------------------------------------------------------------------------
  Configuration
  The GPIO ports for the LEDs are configured. 
*----------------------------------------------------------------------------*/
void configureLEDs() {
  // Configuration steps
  //   1. Enable clock to GPIO ports
  //   2. Enable GPIO ports
  //   3. Set GPIO direction to output
  //   4. Ensure LEDs are off

  // Enable clock to ports B and D
  SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTD_MASK;

  // Make 3 pins GPIO
  PORTB->PCR[RED_LED_POS] &= ~PORT_PCR_MUX_MASK;
  PORTB->PCR[RED_LED_POS] |= PORT_PCR_MUX(1);
  PORTB->PCR[GREEN_LED_POS] &= ~PORT_PCR_MUX_MASK;
  PORTB->PCR[GREEN_LED_POS] |= PORT_PCR_MUX(1);
  PORTD->PCR[BLUE_LED_POS] &= ~PORT_PCR_MUX_MASK;
  PORTD->PCR[BLUE_LED_POS] |= PORT_PCR_MUX(1);

  // Set ports to outputs
  PTB->PDDR |= MASK(RED_LED_POS) | MASK(GREEN_LED_POS);
  PTD->PDDR |= MASK(BLUE_LED_POS);

  // Turn off LEDs
  PTB->PSOR = MASK(RED_LED_POS) | MASK(GREEN_LED_POS);
  PTD->PSOR = MASK(BLUE_LED_POS);
  // end of configuration code
}

/*----------------------------------------------------------------------------
  Turn LEDs on or off
    onOff can be ON or OFF
*----------------------------------------------------------------------------*/
void setRedLED(int onOff) {
  if (onOff == ON) {
    PTB->PCOR = MASK(RED_LED_POS) ;
  }
  if (onOff == OFF) {
    PTB->PSOR =  MASK(RED_LED_POS) ;
  }
  // no change otherwise
}

void setGreenLED(int onOff) {
  if (onOff == ON) {
    PTB->PCOR = MASK(GREEN_LED_POS) ;
  }
  if (onOff == OFF) {
    PTB->PSOR = MASK(GREEN_LED_POS) ;
  }
  // no change otherwise
}

void setBlueLED(int onOff) {
  if (onOff == ON) {
    PTD->PCOR = MASK(BLUE_LED_POS) ;
  }
  if (onOff == OFF) {
    PTD->PSOR = MASK(BLUE_LED_POS) ;
  }
  // no change otherwise
}