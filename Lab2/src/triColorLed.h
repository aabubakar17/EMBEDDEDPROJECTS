/* ------------------------------------------
       ECS642/ECS714 Lab Work

   Header for configuration and control of red, green, blue LEDs
   on the FRDM KL25Z development board
     ---------------------------------------- */

#ifndef TRICOLORLED_H
#define TRICOLORLED_H

// Freedom KL25Z LEDs pin numbers
#define RED_LED_POS (18)		// on port B
#define GREEN_LED_POS (19)	    // on port B
#define BLUE_LED_POS (1)		// on port D

// Symbols for constants
#define OFF 0
#define ON 1

// functions
void configureLEDs() ; // configure GPIO for LEDs
void setRedLED(int onOff) ;
void setGreenLED(int onOff) ;
void setBlueLED(int onOff) ;

#endif
