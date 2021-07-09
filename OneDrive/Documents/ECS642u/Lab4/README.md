# Abubakar's Lab 4 

The behaviour is: 
 1. The	red	and	green	LED	alternate,	with	exactly	one	LED	lit	at	any	moment. The	
two	LEDs	have	equal	on	times.
2. The	on	time	can	be	varied.	Eight	different	on	times	are	available:	0.5	s,	1	s,	1.5	s,	
2	s,	2.5	s,	3s,	3.5	s	and	4	s.			
3. Two	commands	entered	in	the serial	terminal	are	used	to	change	the	time.	The	
command	‘faster’	decreases	the	on	time	by	one	step	and	‘slower’	increases	the	on	
time	by	one	step.	The	command	would	always	produce	a	change:	when	‘faster’	is	
entered	on	the	shortest	time	(0.5	s)	the	list would also	wrap	around	so	that	the	new	
time	is	4	s.
a. Faster:	4s	→ 3.5s	→ …	→ 0.5s	→ 4	s	→ 3.5	s	→ …
b. Slower:	0.5	s	→ 1s	→ 1.5s	→ …	→ 4s	→ 0.5	s

In addition: 
 * If	the	new	on-time	has	already	expired	when	the	change	occurs,	then	led
immediately	change	to	the	new	state.	

* The	new	on-time	has	not	yet	been	completed.	In	this	case,	the	additional	time	 is 	calculated	and	this	delay	used	for	the	first	transition	for	the	new	ontime.	
 
In addition, the project includes the code for the serial interface. 
   1. `sendMsg` which write a message to the terminal
   2. `readLine` which reads a line from the terminal
 

