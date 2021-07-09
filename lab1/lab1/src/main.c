#include <MKL25Z4.h>
#include "SysTick.h"
#include "gpio.h"


/* ------------------------------------------
       ECS642/ECS714 Lab1

   Demonstration of simple digital output
   Use RGB LED on Freedom board
   Introduction to cyclic systems
  -------------------------------------------- */

/* --------------------------------------
     Documentation
     =============
     WARNING: SOME PARTS OF THIS CODE ARE NOT EXPLAINED IN FULL IN WEEK 1

     The code has three principal functions
     1. main: this is where the program starts
        DO NOT CHANGE THIS FUNCTION
     2. configure: this setup the peripherals so the LEDs can be used
        DO NOT CHANGE THIS FUNCTION
     3. every10ms: this function runs every 10 ms
        *****EDIT THIS FUNCTION******

     There are also functions setRedLED, setGreenLED, setBlueLED
     Call these but do not change them

     FILE gpio.h
     - - - - - -
     This file contains some (macro) constants that are used here
     You may need to add or change constants
 -------------------------------------- */

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

/*----------------------------------------------------------------------------
  every10ms - this function runs every 10ms

This function evaluates whether the system should change state (only occassionally)

The system stays in each state for a number of cycles, counted by the 'count'
variable. Each cycle is 10ms long, so 100 cycles gives 100 x 10ms = 1 sec
*----------------------------------------------------------------------------*/
int state = REDOFF ;  // this variable holds the current state
int count = OFF1PERIOD ;// this counter variable to decremented to zero
int phase1count = PHASE1PERIOD;
int phase2count = PHASE2PERIOD;
void every10ms() {
	
	
  if (count > 0) count -- ; // decrement the counter
	
  switch (state) {

    // there is one case for each state
    // each case has the same structure
     // phase 1
  
	if (phase1count > 0) {phase1count--;
    case BLUEOFF:			// the state names are defined in the gpio.h file
      setRedLED(OFF) ;  // set the LEDs for the current state
      setGreenLED(OFF) ;
		  setBlueLED(OFF) ;
      if (count == 0) {    // now time to change state
        state = BLUEON ;    // ... the new state
        count = ON1PERIOD ; // reset the counter
      }
		
      break ;

			case BLUEON:
      setRedLED(OFF) ;
      setGreenLED(OFF) ;
			setBlueLED(ON) ;
      if (count == 0) {
        state = BLUEOFF ;
        count = OFF1PERIOD ;
      }
      break ;
		}
		else { state = REDON;
		count = ON2PERIOD;}
		
	
		//phase 2 
	if(phase2count > 0) {
		
		phase2count--;
    case REDON:
      setRedLED(ON) ;
      setGreenLED(OFF) ;
		  setBlueLED(OFF) ;
      if (count == 0) {
        state = REDOFF;
        count = OFF2PERIOD ;
      }
      break ;
			
		case REDOFF:
      setRedLED(OFF) ;
      setGreenLED(OFF) ;
		  setBlueLED(OFF) ;
      if (count == 0) {
        state = BLUEON ;
        count = ONB2PERIOD ;
      }
      break ;
			
			case BLUEON2:
      setRedLED(OFF) ;
      setGreenLED(OFF) ;
			setBlueLED(ON) ;
      if (count == 0) {
        state = BLUEOFF2 ;
        count = OFFB2PERIOD ;
      }
      break ;
			
		
		case BLUEOFF2:			// the state names are defined in the gpio.h file
      setRedLED(OFF) ;  // set the LEDs for the current state
      setGreenLED(OFF) ;
		  setBlueLED(OFF) ;
      if (count == 0) {    // now time to change state
        state = REDON ;    // ... the new state
        count = ON1PERIOD ; // reset the counter
      }
      break ;

		

    case GREENON:
      setRedLED(OFF) ;
      setGreenLED(ON) ;
		  setBlueLED(OFF) ;
      if (count == 0) {
        state = BLUEON ;
        count = OFF2PERIOD ;
      }
      break ;

			
		}	
			
			
			
			
			
			
			
			
			
  }
}

/*----------------------------------------------------------------------------
  Configuration
  The GPIO ports for the LEDs are configured. This is not explained in week 1
*----------------------------------------------------------------------------*/
void configure() {
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
  MAIN function
 *----------------------------------------------------------------------------*/
int main (void) {
  configure() ;     // configure the GPIO outputs for the LED
  setRedLED(OFF) ;  // ensure all the LEDs are off
  setGreenLED(OFF) ;
  setBlueLED(OFF) ;
  Init_SysTick(1000) ; // initialse SysTick every 1ms
  waitSysTickCounter(10) ;
  while (1) {      // this runs for ever
    every10ms() ;  // call this every 10ms
    // delay
    waitSysTickCounter(10) ;  // cycle every 10 ms - not explained in week 1
  }
}
