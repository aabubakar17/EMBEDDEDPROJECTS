# Abubakar's lab 3 

Clone this project with the starter code for lab 3. First read it and then run it, before making any changes.

The code varies the brightness of the on-board green LED on the KL25Z development board. There are 32 brightness levels.
Initially, the change from dimmest to brightest takes 10 sec but, at random intervals, the system changes between this rate and
a faster one where all the brightness levels are covered in 2 sec. 

There are two tasks:
  1. The 'randomPress' tasks simulates a button press at random intervals.
  1. The 'toogleRate' task toggles the update rate on each button press.

and also a ISR (interrupt service routine) for the PIT (programmanble interrupt timer). This is where the brightness
changes are made.
