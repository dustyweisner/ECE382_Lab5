ECE382_Lab5
===========

IR remote control

__REMOTE USED: iLIVE model: ITP280B (DFEC #1)__

__*Day 1*__

The time that it takes the timer to roll over is 16ms, and each counter lasts 1 us (8MHz/8).

|Pulse|Duration-logic analayzer (ms)|Timer A counts avg|Timer A counts st dev|
|:--|:--|:--|:--|
|Start logic 0 half-pulse|9.01|8937|0|
|Start logic 1 half-pulse|4.45|4411|0|
|Data 1 logic 0 half-pulse|0.601|595.6875|22.32104762|
|Data 1 logic 1 half-pulse|1.67|1626.125|22.51184873|
|Data 0 logic 0 half-pulse|0.599|588.5|20.33715811|
|Data 0 logic 1 half-pulse|0.597|521.8125|14.41396892|
|Stop logic 0 half-pulse|0.599|595|0|
|Stop logic 1 half-pulse|39.4|39445|0|

I collected and tabulated in Excel 8 samples of timer A counts for each of the following pulse types (in decimal): Data 1, logic 1 half-pulse; Data 0, logic 0 half-pulse; Data 0, logic 1 half-pulse. Then I computed the average and standard deviation of each pulse type. I would suggest just grabbing it from the CCS variables tab.

Then I wrote the codes (in hex) for several remote control buttons.

|Button|code (not including start and stop bits)|
|:--|:--|
|0|0x00FF50AF|	
|1|0x00FF00FF|
|2|0x00FF807F|
|3|0x00FF40BF|
|Power|0x00FF30CF|	
|VOL +|0x00FF42BD|
|VOL -|0x00FFC23C|
|CH +|0x00FF08F7|
|CH -|0x00FF8876|	


__*Day 2*__

I had to demonstrate that my code can receive and decode button presses from the remote control.

**Demoed through Required Functionality to Dr. York.**

__*LAB*__

*REQUIRED FUNCTIONALITY*

For the required functionality, I was tasked to turn an LED on and off with one button on the remote and to toggle the other one using a different button. First, because I combined Day 2 work with the required functionality, I had to make my program take the packet of IR data, and put it into one variable to use. To do so, I knew that the program would keep asking to see if I had pushed a button, unless I used the interrupt! So I decided to use the interrupt, and I used the "start5.c" and "start5.h" files that were already given as shell code by Dr. Coulston. I scrolled to where he gave an interrupt shell, and noticed that all I had to do was write my if statements for whether the logic one half-pulses were data 1 or 0 in case 0 inside the  switch statements. If the pulse duration is within range of my remote's 0 or 1 data value (given in the define statements below), the pulse duration is changed to a 0 or a 1. If the pulse duration is within the range of the remote's average start bit, the pulse duration is changed to a 2 so that in the main loop, the irPacket doesn't include start bits.

start5.h:

      #define		averageLogic0Pulse	590
      #define		averageLogic1Pulse	1626
      #define		averageStartPulse	4411
      #define		minLogic0Pulse		averageLogic0Pulse - 200
      #define		maxLogic0Pulse		averageLogic0Pulse + 200
      #define		minLogic1Pulse		averageLogic1Pulse - 200
      #define		maxLogic1Pulse		averageLogic1Pulse + 200
      #define		minStartPulse		averageStartPulse - 200
      #define		maxStartPulse		averageStartPulse + 200

start5.c (inside case 0 inside interrupt):

      if (pulseDuration >= minLogic1Pulse && pulseDuration <= maxLogic1Pulse) {
      	pulseDuration = 1;
      } else if (pulseDuration >= minLogic0Pulse && pulseDuration <= maxLogic0Pulse ) {
      	pulseDuration = 0;
      }else {
      	pulseDuration = 2;
      }

Then, because after an interrupt I want to disable the interrupt when I actually do stuff in the main, I wanted to create a flag saying that I had interrupted and gathered the IR `packetData` into an array already.

      if (packetIndex > 33) {
      		flagged=1;
      }

After that, I moved on to work in the main. I wanted to compile all the 1's and 0's created from pulseDuration into one single variable to be used for comparison. I wanted to do things only if `flagged` was a 1, or if the program had been interrupted. If `flagged` was 1, I wanted to first disable another interrupt while my data was being used. Next, I wanted the start bits not to be counted in the single variable, `irPacket`, so I would use a while loop until the `pulseDuration` is not a 2 anymore. Then I wanted to use another while loop for if the irPacket is not filled yet. Inside that while loop is where, I would add the bit of `packetData[i]` and shift left, and repeat until the index is 33. After that I had my Day 2 functionality, because `irPacket` was filled with the value of my remote's button press. The following is the code used for that section:

      if(flagged==1) {
      
      // Disabling the interrupt before doing this seemed to help the
      // program from screwing up
      _disable_interrupt();
      packetCount = 0;
      
      // Count past the start bits
      while(packetData[packetCount]==2) {
      	packetCount++;
      }
      
      // Start the concatenation of the ir Packet
      while(packetCount<33) {
      	irPacket+=packetData[packetCount];
      	irPacket<<=1;
      	packetCount++;
      }
      // One bit was being left out
      irPacket+=packetData[packetCount];

The only thing left to do was to toggle the lights, create a time delay, reenable the interrupt, and reset the index, flag, and ir packet. The following are the define statements for the individual hex values of the button pushes and the toggle statements with the rest of the code if `flagged` is 1 (the `PWR` button toggling the red LED and the `ONE` button toggling the green LED):

start5.h:

    #define		PWR	  	0x00FF30CF
    #define		ONE	  	0x00FF00FF
    #define		TWO	  	0x00FF807F
    #define		THR	  	0x00FF40BF
    #define 	ZERO  	0x00FF50AF
    #define		VOL_UP	0x00FF42BD
    #define		VOL_DW	0x00FFC23C
    #define		CH_UP	 0x00FF08F7
    #define		CH_DW	 0x00FF8877
    #define		CH_R	 0x00FFC837
    #define		CH_L  	0x00FF48B7
    #define		SEL	 0x00FF708F

start5.c:

    if(irPacket == PWR) {
      P1OUT ^= BIT0;
    }
    if(irPacket == ONE) {
    	P1OUT ^= BIT6;
    }
    unsigned int i;
    for(i=0; i<0xFFFF;i++);
    for(i=0; i<0xFFFF;i++);
    packetIndex=0;
    irPacket=0x00000000;
    _enable_interrupt();
    flagged =0;

Afterwards, for a little debugging, I added an else statement to when `flagged` is 0.

    else {
      irPacket = 0x00000000;
    }

This concludes the required functionality.

*A FUNCTIONALITY*

A functionality was to implement the lab 4 etch-a-sketch using the required functionality. First I used the same idea from Lab 4 with the button presses. Instead of using the MSP430 buttons, I just used if statements with my remote values compared to my `irPacket` value. I copied the same format with the inplementation of vertical and horizontal moveBlock features, as shown in the code below:

      if (irPacket == CH_UP) {
    		if (y>=1) y=y-1;
    	} else if (irPacket == CH_DW) {
    		if (y<=6) y=y+1;
    	} else if (irPacket == CH_L) {
    		if (x>=1) x=x-1;
    	} else if (irPacket == CH_R) {
    		if (x<=10) x=x+1;
    	}

I also needed to initalize the system and to initialize my variables up at the top of the program. Above the while(1) loop but in the main, I initialized the system:

      IFG1=0; /* clear interrupt flag1 */
      	WDTCTL=WDTPW+WDTHOLD; /* stop WD */
      	button_press = FALSE;
      	change_press = FALSE;
      
      	init();
      	initNokia();
      	clearDisplay();
      	x=4;		y=4;
      	drawBlock(y,x,change_press);
      	initMSP430();				// Setup MSP to process IR and buttons

I also remembered to include external void methods from nokia.asm. After doing so I tested the program. It did not work and I had no clue why not. After talking with others about the same problem, I reinitialized everything ever time I had to drawBlock. I used the following code: 

      init();
      initNokia();
      drawBlock(y,x,change_press);
      initMSP430();

This code worked! Because of the two labs being seperate, the files may not have been completely compatible. Maybe somewhere in nokia, something was being reset that was messing up the LCD display.

This concludes the A Functionality write up.
