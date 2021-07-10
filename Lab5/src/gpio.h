// Header file for GPIO in Lab 4
//   Definitions for pin usage
//   Function prototypes

#ifndef GPIO_DEFS_H
#define GPIO_DEFS_H

#include <stdbool.h>

#define MASK(x) (1UL << (x))

// Freedom KL25Z LEDs
#define RED_LED_POS (18)    // on port B
#define GREEN_LED_POS (19)	// on port B
#define BLUE_LED_POS (1)	// on port D

// LED states
#define LED_ON  (1)
#define LED_OFF (0)
#define intermediate (0)
#define flat (1)
#define right (2)
#define left (3)
#define up (4)
#define down (5)
#define over (6)
// Function prototypes
void configureGPIOoutput(void) ;
void redLEDOnOff (int onOff) ;
void greenLEDOnOff (int onOff) ;
void blueLEDOnOff (int onOff) ;

#endif


