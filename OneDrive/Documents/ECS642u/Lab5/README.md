# Abubakar's lab 5

The project uses a serial data link (over USB) to a terminal emulator running on the
connected PC / laptop.
  * The accelerometer is polled every 0.2 seconds
  * The orientation are printed on the terminal emulator 
  * The green light is lit when the trigger is occured and after all orientation are correclty done.
  * Red light is lit when state erros occur and the time errors.

The project uses a two threads

The project includes the code for the serial interface. This API has two functions:
   1. `sendMsg` which write a message to the terminal
   2. `readLine` which reads a line from the terminal (not used here)

The project includes code for the accelerometer. This API has two functions:
   1. `initAccel` initialises the accelerometer
   2. `readXYZ` reads the three accelerations

state diagram for lab 5:

<img width="658" alt="Lab 5 state diagram" src="https://media.github.research.its.qmul.ac.uk/user/2288/files/399ab880-34b8-11eb-9806-e2b6b2197e42">
