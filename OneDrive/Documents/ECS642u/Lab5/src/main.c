#include "cmsis_os2.h"
#include <MKL25Z4.h>
#include <stdbool.h>
#include "gpio.h"
#include "serialPort.h"
#include "i2c.h"
#include "accel.h"

osThreadId_t t_change; // id for change thread
osMessageQueueId_t MsgQ; // id for message queue


enum msg_t{FLAT, RIGHT, UP, OVER, DOWN, LEFT}; // types of messages

  
  
 
/*--------------------------------------------------------------
 *   Thread t_accel
 *      Read accelarations periodically  
 *      Write results to terminal
 *      Toggle green LED on each poll
 *--------------------------------------------------------------*/
osThreadId_t t_accel;      /* id of thread to poll accelerometer */

// convert signed integer +/- 999 to +/-X.XX
//   SX.XX
//   01234
void aToString(int16_t a, char* s) {  
    bool negative = false ;
    if (a < 0) {
        a = -a ;
        negative = true ; 
    }

    // get digits
    s[4] = '0' + (a % 10) ;
    a = a / 10 ;
    s[3] = '0' + (a % 10) ;
    a = a / 10 ;
    s[1] = '0' + (a % 10) ;
        
    // set sign
    s[0] = '+' ;
    if (negative) {
        s[0] = '-' ;
    }
}

// buffer for message
char xyzStr[] = "X=SX.XX Y=SX.XX Z=SX.XX" ;
//               01234567890123456789012

void accelThread(void *arg) {
    int16_t xyz[3] ; // array of values from accelerometer
        // signed integers in range +8191 (+2g) to -8192 (-2g)
   
    enum msg_t message; // variable for messsage sent to queue
   

    
  
    //Orientation states
    #define intermediate (0)
    #define flat (1)
    #define right (2)
    #define left (3)
    #define up (4)
    #define down (5)
    #define over (6)
      
   //initial state
    int state = intermediate;
  
  
    // initialise accelerometer
    int aOk = initAccel() ;
    if (aOk) {
        sendMsg("Accel init ok", CRLF) ;
    } else {
        sendMsg("Accel init failed", CRLF) ;
    }
    while(1) {
        osDelay(200) ;
        readXYZ(xyz) ; // read X, Y Z values
        int16_t xyznew[3];
        xyznew[0] = (xyz[0] * 100) / 4096;
        xyznew[1] = (xyz[1] * 100) / 4096;
        xyznew[2] = (xyz[2] * 100) / 4096;
      
         

        // interpret state machine 
        switch (state) {
            
            case intermediate :
                if(xyznew[2] > 90) {
                  message = FLAT; 
                  osMessageQueuePut(MsgQ, &message, 0, NULL); // put message in the queue 
                  sendMsg("flat",CRLF); // send message to terminal 
                  state =  flat; 
                  
                } 
                else if(xyznew[2] < -90) {
                  message = OVER;
                  osMessageQueuePut(MsgQ, &message, 0, NULL); // put message in the queue 
                  sendMsg("over",CRLF); // send message to terminal 
                  state =  over;
                }
                if(xyznew[1] < -90) {
                  message = RIGHT;
                  osMessageQueuePut(MsgQ, &message, 0, NULL); // put message in the queue 
                  sendMsg("right",CRLF); // send message to terminal 
                  state =  right;
                }
                else if(xyznew[1] > 90) {
                  message = LEFT;
                  osMessageQueuePut(MsgQ, &message, 0, NULL); // put message in the queue 
                  sendMsg("left",CRLF); // send message to terminal 
                  state =  left;
                }
                if(xyznew[0] < -90) {
                  message = UP;
                  osMessageQueuePut(MsgQ, &message, 0, NULL); // put message in the queue 
                  sendMsg("up",CRLF); // send message to terminal 
                  state =  up;
                }
                else if(xyznew[0]  > 90) {
                  message = DOWN;
                  osMessageQueuePut(MsgQ, &message, 0, NULL); // put message in the queue 
                  sendMsg("down",CRLF); // send message to terminal 
                  state =  down;
                }
                break ;
                
            // cases for when orientaion has changed
            case flat:
               if(xyznew[2] < 80) {
                  state =  intermediate;
                }
               break;
                 
            case over:
              
               if(xyznew[2] > -80) {
                  state =  intermediate;
                }
             
                break ;  
                
             case right:
               if(xyznew[1] > -80) {
                  state =  intermediate;
                }
               break;
                
            case left:
               if(xyznew[1] < 80) {
                  state =  intermediate;
                }
               break;
            
            case up:
               if(xyznew[0] > -80) {
                  state =  intermediate;
                }
               break;
                
             
             case down:
               if(xyznew[0] < 80) {
                  state =  intermediate;
                }
              
               
                 
        }            
    }
}
    


 /*--------------------------------------------------------------
 *   Thread t_chnageThread
 *    detects errors in orientation
 *    detects errors in time
 *    green LED lights when system is triggered
 *--------------------------------------------------------------*/

void changeThread(void *arg) {
  //variables to record time before message sent and after
  uint32_t endcount;
  uint32_t startcount;
  uint32_t count;
  
  osStatus_t status; // returned by message queue get
  enum msg_t message; 
  #define Fstate (7)
  #define Rstate (8)
  #define Ustate (9)
  #define Estate  (10)
  #define inter  (11)
  #define Finalstate (12)
  
  uint32_t time = osWaitForever;
  int Changestate = inter; //initial state 
  
  while(1){
   if(Changestate!= Estate & Changestate != Finalstate){ // if statement only true for when trigger or error state has not occured
   startcount = osKernelGetTickCount(); //get time at the start
   status = osMessageQueueGet(MsgQ, &message, NULL, time); // wait for message from queue
   endcount = osKernelGetTickCount(); // get time at the end
   }
   
   else{ // if true skip message an go straigh to error or trigger state. 
    status = osOK; 
   }
    
   if(status == osErrorTimeout) { // timeout for when the state has remained in the orientation for too long 
      Changestate = Estate; 
   }
   
   if(status == osOK){
     count = endcount - startcount; // get the overall time that has passed
     switch (Changestate) {
    
     case inter: //intermediate state checking first state/ orientation is flat
      if(message == FLAT){
       Changestate = Fstate;
       time = osWaitForever;        
      }         
      else{
        Changestate = Estate;
      }
       break;
  
     
     case Fstate:
       if(count > 10000){ // check if the state has stayed in flat for suffcient time
         if(message == RIGHT) { // check if new message is right 
         Changestate = Rstate; 
         time = 6000; // change timeout time 
         }
         
         else {
         Changestate = Estate; // error state for when new message is not right
         }
       }
       else{
         Changestate = Estate; // error state for when new message is not stayed for suffecient time 
       }
     break;
       
     case Rstate:
       if(count > 2000){// check if the state has stayed in flat for suffcient time
         if(message == UP){ // check if new message is up 
         Changestate = Ustate;
         time = 8000;  // change timeout time 
         }
       else 
         Changestate = Estate;
     }
     else 
       Changestate = Estate;
     break;
     
     
     case Ustate:
       if(count > 4000){ //check if the state has stayed in flat for suffcient time
         if(message == FLAT){ //check if new message is flat
          Changestate = Finalstate;
          time = osWaitForever;  // change timeout time 
         }
         else 
           
           Changestate = Estate;
       }
       else 
         Changestate = Estate;
     
     break;
       
       
     case Finalstate: // state for when trigger has occured 
       greenLEDOnOff(LED_ON);
     break;
       
     case Estate: // state for when error has occured
     
     redLEDOnOff(LED_ON);
     
     break;
       
     }
     
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

    //configureGPIOinput();
    init_UART0(115200) ;

    // Initialize CMSIS-RTOS
    osKernelInitialize();
  

  
    // initialise serial port 
    initSerialPort() ;

    // Initialise I2C0 for accelerometer 
    i2c_init() ;
    
    // Initialise GPIo for on-board LEDs
    configureGPIOoutput() ;
  
       
    // create message queue
   
    MsgQ = osMessageQueueNew(2, sizeof(enum msg_t), NULL) ;
    
    // Create threads
    t_change = osThreadNew(changeThread, NULL, NULL);
    t_accel = osThreadNew(accelThread, NULL, NULL); 
    
    osKernelStart();    // Start thread execution - DOES NOT RETURN
    for (;;) {}         // Only executed when an error occurs
}
