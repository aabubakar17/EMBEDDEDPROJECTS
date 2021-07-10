#include "cmsis_os2.h"
#include <MKL25Z4.h>
#include <stdbool.h>
#include "../include/gpio.h"
#include "../include/serialPort.h"
#include "../include/TSI.h"
#include "../include/TPMPWM.h"
#include "../include/triColorLedPWM.h"

//define flag variables
#define leftout (1U)
#define Rightout (2U)
#define leftin (4U)
#define Rightin (8U)

osEventFlagsId_t touchEvtFlags; // event flags

/*--------------------------------------------------------------
 *   Thread t_touch
 *      detects key presses and creates events.
 *--------------------------------------------------------------*/
osThreadId_t t_touch; /* id of thread to read touch pad periodically */

// convert unsigned 8-bit to XXX
void uToString(uint8_t a, char * s) {

  // array of digits
  int digit[3]; // [0] least significant
  uint8_t a_dash;

  // digits
  int ix;
  for (ix = 0; ix < 3; ix++) {
    a_dash = a / 10; // 234 --> 23 
    digit[2 - ix] = a - (10 * a_dash); // 234 - 230 --> 4
    a = a_dash; // 23
  }

  // skip leading zero
  ix = 0;
  while ((ix < 2) && (digit[ix] == 0)) {
    s[ix] = ' ';
    ix++;
  }

  // characters
  while (ix < 3) {
    s[ix] = digit[ix] + '0';
    ix++;
  }
}

char distStr[] = "dist = XXX";
//                01234567890

void touchThread(void * arg) {
  #define NONE (0)
  #define LEFTIN (1)
  #define LEFTOUT (2)
  #define RIGHTIN (3)
  #define RIGHTOUT (4)
  int State = NONE;
  // data buffer
  uint8_t touchDist;

  // initialise touch sensor
  TSI_init();

  // loop polling touch sensor every 2s
  while (1) {
    osDelay(25);
    touchDist = readTSIDistance(); //returns distance on the touchpad

    // write distance
    uToString(touchDist, & distStr[7]);

    // interpret state machine 
    switch (State) {

      //case NONE for when no key press
    case NONE:

      if (touchDist > 3 && touchDist < 9) {
        osEventFlagsSet(touchEvtFlags, leftout); //Even flag set for when systems goes new state 
        State = LEFTOUT; //change to new state

      }
      if (touchDist > 13 && touchDist < 19) {
        osEventFlagsSet(touchEvtFlags, leftin);
        State = LEFTIN;
      }

      if (touchDist > 23 && touchDist < 29) {
        osEventFlagsSet(touchEvtFlags, Rightin);
        State = RIGHTIN;

      }

      if (touchDist > 33) {
        osEventFlagsSet(touchEvtFlags, Rightout);
        State = RIGHTOUT;

      }

      break;
      //case for outerleft key
    case LEFTOUT:
      if (touchDist < 3 || touchDist > 9) {
        State = NONE;

      }
      break;
      //case for inner left key
    case LEFTIN:
      if (touchDist < 13 || touchDist > 19) {
        State = NONE;
      }
      break;
      //case for inner right key
    case RIGHTIN:
      if (touchDist < 23 || touchDist > 29) {
        State = NONE;
      }
      break;
      //case for outer right key
    case RIGHTOUT:
      if (touchDist < 33) {
        State = NONE;

      }

      break;
    }

  }
}

/*--------------------------------------------------------------
 *   Thread t_respnd 
 *       when flag is set thread responds by selecting led 
           and changing brightness depending key presses
        
 *--------------------------------------------------------------*/

osThreadId_t t_respond; /* id of thread to flash red led */

void respondThread(void * arg) {
  //states
  #define first (0)
  #define none (1)
  #define inleft (2)
  #define outleft  (3)
  #define outright (4)
  #define inright (5)
  int state = first; //intitial state 

  unsigned int brightRED = 3; //intitial brightness for red led
  unsigned int brightBLUE = 3; // intitial brightness for blue led
  int current; //current led press 
  uint32_t flags; // returned by osEventFlagWait
  uint32_t allflags = leftin | Rightout | leftout | Rightin;
  while (1) {

    if (state != none) {
      flags = osEventFlagsWait(touchEvtFlags, allflags, osFlagsWaitAny, osWaitForever); // wait for the set of flags (allflags)
    }

    //interpret state machine 
    switch (state) {
    case first:
      state = none;

    case none:
      if (flags == leftin)
        state = inleft;
      if (flags == Rightout)
        state = outright;
      if (flags == leftout)
        state = outleft;
      if (flags == Rightin)
        state = inright;
      break;

      // case to select red led 
    case inleft:
      sendMsg("LEFTIN", CRLF);
      current = Red;
      state = none;
      break;

      //case to increase brightness
    case outleft:
      sendMsg("LEFTOUT", CRLF);

      //To ensure no further	changes in	the	same	direction
      if (brightRED == 7) {
        brightRED = 6;
      }
      if (brightBLUE == 7) {
        brightBLUE = 6;
      }

      // changing brightness for red led 
      if (current == Red) {
        brightRED = (brightRED + 1);

        setLEDBrightness(current, brightRED);
      }
      //changing brightness for blue led
      if (current == Blue) {
        brightBLUE = (brightBLUE + 1);
        setLEDBrightness(current, brightBLUE);
      }
      state = none;
      break;

      //case to select blue led 
    case inright:
      sendMsg("RIGHTIN", CRLF);
      current = Blue;
      state = none;
      break;

      //case to decrease the brightness
    case outright:
      sendMsg("RIGHTOUT", CRLF);
      //To esnure no further changes in the same direction
      if (brightBLUE == 0) {
        brightBLUE = 1;
      }
      if (brightRED == 0) {
        brightRED = 1;
      }

      //changing blue led brightness
      if (current == Blue) {
        brightBLUE = (brightBLUE - 1);
        setLEDBrightness(current, brightBLUE);

      }
      //changing red led brightness 
      if (current == Red) {
        brightRED = (brightRED - 1);
        setLEDBrightness(current, brightRED);

      }
      state = none;
      break;
    }

  }

}

/*----------------------------------------------------------------------------
 * Application main
 *   Initialise I/O
 *   Initialise kernel
 *   Create threads
 *   Start kernel
 *---------------------------------------------------------------------------*/

int main(void) {

  configureTPMClock();
  configureLEDforPWM(); // Configure LEDs for PWM control
  configureTPM0forPWM(); // configure PWM on TPM0 (blue LED)
  configureTPM2forPWM(); // configure PWM on TPM2 (red, green LEDs)

  setLEDBrightness(Red, 0);
  setLEDBrightness(Green, 0);
  setLEDBrightness(Blue, 0);

  // System Initialization
  SystemCoreClockUpdate();

  //configureGPIOinput();
  init_UART0(115200);

  // Initialize CMSIS-RTOS
  osKernelInitialize();

  // initialise serial port 
  initSerialPort();
  sendMsg("Reset", CRLF);
  // Initialise LEDs 
  //configureGPIOoutput();

  // Create event flags
  touchEvtFlags = osEventFlagsNew(NULL);

  // Create threads
  t_touch = osThreadNew(touchThread, NULL, NULL);
  t_respond = osThreadNew(respondThread, NULL, NULL);

  osKernelStart(); // Start thread execution - DOES NOT RETURN
  for (;;) {} // Only executed when an error occurs
}