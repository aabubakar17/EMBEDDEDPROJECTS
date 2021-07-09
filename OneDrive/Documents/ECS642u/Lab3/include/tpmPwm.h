#ifndef TPM_PWM_H
#define TPM_PWM_H
#include <MKL25Z4.h>

#define PWM_DUTY_MAX (255)

// prototypes
void configureTPMClock(void) ;
void configureTPM0forPWM(void) ;
void configureTPM2forPWM(void) ;

extern const unsigned int MAXBRIGHTNESS ;
enum LED {Red, Green, Blue} ;
void setLEDBrightness(enum LED led, unsigned int brightness) ;

#endif
