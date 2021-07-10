/* ------------------------------------------
       ECS642/ECS714 Lab Work

   Configuration of TPM modules
   ---------------------------------------- */

#include <MKL25Z4.H>
#include "../include/TPMPWM.h"
#include "../include/triColorLedPWM.h"


/*----------------------------------------------------------------------------
  Configure TPM0 and TPM2 for PWM

Three channels used to control LEDs:
  TPM0 - channel 1 for blue LED
  TPM2 - channels 0, 1 for red, green LEDs
  
  
  Controlled by the following macro definitions:
       TPM2_CHAN1, TPM2_CHAN2, TPM0_CHAN1 the channels, determined by the pins
       PWM2_PORT1, PWM2_PORT2, PWM0_PORT1 the ports, such as PORTA, PORTB etc
       PWM2_PIN1, PWM2_PIN2, PWM0_PIN1    the pins within the ports
       PWM0_ALT1, PWM0_ALT2, PWM0_ALT1    the alternative number for muxiplexing the pin
    These values mst be consistent with the KL25Z pinout
    
    The following values are fixed (i.e. no macros)
       Clock frequency - CPU clock 20,971,520 Hz
       Pre-scaling - divide by 128
       Count modulo - 255
         
    Note that there are many other useful configurations of the TPM
    not covered here.
 *----------------------------------------------------------------------------*/ 
void configureTPMClock(){
    // Enable clock to TPM0 and TPM2
    SIM->SCGC6 |= SIM_SCGC6_TPM0_MASK | SIM_SCGC6_TPM2_MASK ;
    
    // Choose TPM clock – FLLCLK (i.e.20.9 MHz) (section 12.2.3, p195)
    SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1) ; // | SIM_SOPT2_PLLFLLSEL_MASK ;
    // Default value NOT ok for PLLFLLSEL

}

void configureTPM0forPWM() {

    // -----------------------
    // Module configuration
    // ------------------------
    
    // Disable the TPM0
    TPM0->SC = TPM_SC_CMOD(0) ;
    while ((TPM0->SC & TPM_SC_CMOD(3))) ;
    
    // Prescale by 64 with 110
    TPM0->SC |= TPM_SC_PS(0x6) ;

    // Count modulo 2^10 - 1 = 1023. Note: 1024 is used for 100% duty cycle
    TPM0->MOD = TPM_MOD_MOD(1023) ;

    // Set counter to continue when debugging
    TPM0->CONF |= TPM_CONF_DBGMODE(0x3) ;

    // -----------------------
    // Channel configuration
    // ------------------------

    // Configure channel TPM_CHAN
    //   Edge pulse: MSB:A = 1:0
    //   True pulse high: ELSB:A = 1:0
    TPM0->CONTROLS[TPM0_CHAN1].CnSC = 0x28 ; 
    
    // Set the channel variable - off
    TPM0->CONTROLS[TPM0_CHAN1].CnV = TPM_CnV_VAL(0) ;
    
    
    // -----------------------
    // Enable using internal clock 
    // -----------------------

    TPM0->SC |= TPM_SC_CMOD(0x01) ;
    while (!(TPM0->SC & TPM_SC_CMOD(3))) ;
}


void configureTPM2forPWM() {

    // -----------------------
    // Module configuration
    // ------------------------
    
    // Disable the TPM2
    TPM2->SC = TPM_SC_CMOD(0) ;
    while ((TPM2->SC & TPM_SC_CMOD(3))) ;
    
    // Prescale by 64 with 110
    TPM2->SC |= TPM_SC_PS(0x6) ;

    // Count modulo 2^10 - 1 = 1023. Note: 1024 is used for 100% duty cycle
    TPM2->MOD = TPM_MOD_MOD(1023) ;

    // Set counter to continue when debugging
    TPM2->CONF |= TPM_CONF_DBGMODE(0x3) ;

    // -----------------------
    // Channel configuration
    // ------------------------

    // Configure channels TPM2_CHAN1,2
    //   Edge pulse: MSB:A = 1:0
    //   True pulse high: ELSB:A = 1:0
    TPM2->CONTROLS[TPM2_CHAN1].CnSC = 0x28 ; 
    TPM2->CONTROLS[TPM2_CHAN2].CnSC = 0x28 ; 
    
    // Set the channel variable - off
    TPM2->CONTROLS[TPM2_CHAN1].CnV = TPM_CnV_VAL(0) ;
    TPM2->CONTROLS[TPM2_CHAN2].CnV = TPM_CnV_VAL(0) ;
    
    
    // -----------------------
    // Enable using internal clock 
    // -----------------------

    TPM2->SC |= TPM_SC_CMOD(0x01) ;
    while (!(TPM2->SC & TPM_SC_CMOD(3))) ;
}


/*----------------------------------------------------------------------------
  Set PWM duty cycle to change the LED brightness. There are two complications
    1. The LEDs are wired active low so we need a low duty cycle for a bright LED
    2. The response is not at all linear: we use a look-up table for duty cycle values
    
 *----------------------------------------------------------------------------*/ 


const uint16_t duty[] = {
    1024, 1021, 1016, 1010, 1002, 991, 977, 958,
    935, 904, 866, 819, 764, 699, 628, 551,
    473, 397, 325, 261, 205, 159, 121, 90,
    66, 48, 33, 23, 15, 8, 4, 0} ;

const unsigned int MAXBRIGHTNESS = 31 ; // range is 0 to 31
                         
void setLEDBrightness(enum LED led, unsigned int brightness) {
    if (brightness > MAXBRIGHTNESS) brightness = MAXBRIGHTNESS ;
    uint16_t d = duty[brightness] ; 
    
    // Set the channel variable
    switch (led) {
        case Red:
            TPM2->CONTROLS[TPM2_CHAN1].CnV = TPM_CnV_VAL(d) ;
            break ;
        case Green:
            TPM2->CONTROLS[TPM2_CHAN2].CnV = TPM_CnV_VAL(d) ;
            break ;
        case Blue:
            TPM0->CONTROLS[TPM0_CHAN1].CnV = TPM_CnV_VAL(d) ;
            break ;
    }
}
