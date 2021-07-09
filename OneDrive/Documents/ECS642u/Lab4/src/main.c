
 
#include "cmsis_os2.h"
#include "string.h"

#include <MKL25Z4.h>
#include <stdbool.h>
#include "gpio.h"
#include "serialPort.h"

#define RESET_EVT (1)
; 
osEventFlagsId_t errorFlags ;       // id of the event flags// id of the event flags

osMessageQueueId_t controlMsgQ;    // id for the message queue


osThreadId_t t_greenandredLED;      /* id of thread to toggle green  and red led */

// Green and Red LED states
#define GREENON (0)
#define REDON (1)
enum controlMsg_t {Faster, Slower, reset} ;  // type for the messages
int delaytime[] = {500, 1000, 1500, 2000, 2500, 3000, 3500, 4000}; // The 8 different times 

void greenandredLEDThread (void *arg) {
    uint32_t time ; //variable holding the updated time
    int i = 0; //  index of array
    uint32_t count; //variable holding the current count  
    int ledState = REDON ; // initial state
    enum controlMsg_t msg ; //msg from terminal 
    osStatus_t status ;   // returned by message queue get
  
    time = delaytime[i];// initial time 
  
    while (1) {
        // wait for message from queue
        
        status = osMessageQueueGet(controlMsgQ, &msg, 0, time);
      
        if (status == osOK){
         
          count = osKernelGetTickCount() - count; // time that has already passed 
         
          
          
          if (msg == Faster) {   
            i =i-1;     // decreasing on time the time
          if(i < 0){  // the time wrap around 
                 i= 7;
            
            
          }
            time = delaytime[i] - count; // new on time, for the wrap around case   

            // expected message
                
             
           if(count >= delaytime[i]){ //if time already that passed is more than new on time move new state.
             ledState = 1 - ledState; // change to a new state
             time = NULL;
           }
               
      
          }
          else if (msg == Slower) { 
                i = i+1; // increasing on time 
            
             if(i > 7)  {  // the time wrap around 
             
               i = 0;
             

              if(count < delaytime[i]){
               time = delaytime[i] - count; 
              }
              else 
                
                ledState = 1 - ledState; //wrap wound case            
     
            }
             if(count < delaytime[i]){ //if time already passed is less than new time, calculate new on time
               time = delaytime[i] - count; // new on time 
             }
             
               
          }
          
       }
       else if    (status == osErrorTimeout) {
           time = delaytime[i]; 
            switch (ledState) {
              //State for red led on for "time" seconds
                case REDON:
                  count = osKernelGetTickCount();
                   greenLEDOnOff(LED_OFF);
                   redLEDOnOff(LED_ON);
                   ledState = GREENON ;
    
                    break ;
                    
                
                
                case GREENON:     
                  //State for red led on for "time" seconds
                    count = osKernelGetTickCount();
                    greenLEDOnOff(LED_ON);
                    redLEDOnOff(LED_OFF);
                    ledState = REDON ;  
                 
                    break ;
                    
                
                }
        }
      }
}


/*------------------------------------------------------------
 *  Thread t_command
 *      Request user command
 *      
 *
 *------------------------------------------------------------*/
osThreadId_t t_command;        /* id of thread to receive command */

/* const */ char prompt[] = "Command: Faster / Slower >" ;
/* const */ char empty[] = "" ;

void commandThread (void *arg) {
    int i = 3;
    int time;
    char response[11] ;  // buffer for response string
    enum controlMsg_t msg ;
    bool valid ;
    while (1) {
        //sendMsg(empty, CRLF) ;
        sendMsg(prompt, NOLINE) ;
        readLine(response, 10) ;  
        valid = true ;
        if (strcmp(response, "Faster") == 0) {
            msg = Faster;
           
        } else if (strcmp(response, "Slower") == 0) {
            msg = Slower ;
        } 
            
        else valid = false ;
        
        if (valid) {
  
            osMessageQueuePut(controlMsgQ, &msg, 0, NULL);  // Send Message
        } 
        else {
            sendMsg(response, NOLINE) ;
            sendMsg(" not recognised", CRLF) ;
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

int main (void) { 
    
    // System Initialization
    SystemCoreClockUpdate();

    // Initialise peripherals
    configureGPIOoutput();
    //configureGPIOinput();
    init_UART0(115200) ;

    // Initialize CMSIS-RTOS
    osKernelInitialize();
    
    // Create event flags
    errorFlags = osEventFlagsNew(NULL);
    
    // create message queue
    controlMsgQ = osMessageQueueNew(2, sizeof(enum controlMsg_t), NULL) ;
   

    // initialise serial port 
    initSerialPort() ;

    // Create threads
    //t_redLED = osThreadNew(redLEDThread, NULL, NULL); 
    t_greenandredLED = osThreadNew(greenandredLEDThread, NULL, NULL);
    t_command = osThreadNew(commandThread, NULL, NULL); 
    
    osKernelStart();    // Start thread execution - DOES NOT RETURN
    for (;;) {}         // Only executed when an error occurs
}
