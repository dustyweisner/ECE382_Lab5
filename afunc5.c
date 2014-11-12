//-----------------------------------------------------------------
// Name:	Coulston/Dusty Weisner
// File:	lab5.c
// Date:	Fall 2014
// Purp:	Demo the decoding of an IR packet
// Weisner Documentation: I had some help from C2C Clayton Jaksha
// 	in understanding how to use my if statemnents inside the
// 	interrupt. I recieved help from Dr. York in a little bug 
//	that requires me to initialize everything every time the
//	program draws to the LCD screen***. 
//-----------------------------------------------------------------
#include <msp430g2553.h>
#include "afunc5.h"

// external methods
extern void init();
extern void initNokia();
extern void clearDisplay();
extern void drawBlock(unsigned char row, unsigned char col, unsigned char chgpress);

#define		TRUE			1
#define		FALSE			0


// intialize variables
int8	newIrPacket = FALSE;
int16	packetData[48];
int8	packetIndex = 0;
int32	irPacket = 0x00000000;
int8 packetCount =0;
int8 sum=0;
unsigned char flagged=0;

// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
void main(void) {
		unsigned char	x, y, button_press;
		volatile unsigned char	change_press;

		// === Initialize system ================================================
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

	while(1)  {
		// flag for when the interrupt has fulfilled its purpose
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
			
			// The select button on the remote caused the 
			// programs etch-a-sketch to change to a delete
			// mode
			if (irPacket == SEL) {
				if (change_press == TRUE){
					change_press = FALSE;
				} else change_press = TRUE;
				
				//***The bug as explained in Weisner 
				// Documentation above
				init();
				initNokia();
				drawBlock(y,x,change_press);
				initMSP430();
			}
			// Button press if statements - move block
			if (irPacket == CH_UP) {
				if (y>=1) y=y-1;
			} else if (irPacket == CH_DW) {
				if (y<=6) y=y+1;
			} else if (irPacket == CH_L) {
				if (x>=1) x=x-1;
			} else if (irPacket == CH_R) {
				if (x<=10) x=x+1;
			}
			
			//***The bug as explained in Weisner 
			// Documentation above
			init();
			initNokia();
			drawBlock(y,x,change_press);
			initMSP430();
			
			// Delay for button presses
			unsigned int i;
			for(i=0; i<0xFFFF;i++);
			for(i=0; i<0xFFFF;i++);
			
			// reset Index, ir Packet, and interrupt flag;
			// and reenable the interrupt
			packetIndex=0;
			irPacket=0x00000000;
			flagged =0;
			_enable_interrupt();
			
		// if flag isn't set, then nothing should happen to irPacket
		} else {
			irPacket = 0x00000000;
		}
	} // end if new IR packet arrived
} // end infinite loop
// end main

// -----------------------------------------------------------------------
// In order to decode IR packets, the MSP430 needs to be configured to
// tell time and generate interrupts on positive going edges.  The
// edge sensitivity is used to detect the first incoming IR packet.
// The P2.6 pin change ISR will then toggle the edge sensitivity of
// the interrupt in order to measure the times of the high and low
// pulses arriving from the IR decoder.
//
// The timer must be enabled so that we can tell how long the pulses
// last.  In some degenerate cases, we will need to generate a interrupt
// when the timer rolls over.  This will indicate the end of a packet
// and will be used to alert main that we have a new packet.
// -----------------------------------------------------------------------
void initMSP430() {

	IFG1=0; 					// clear interrupt flag1
	WDTCTL=WDTPW+WDTHOLD; 		// stop WD

	BCSCTL1 = CALBC1_8MHZ;
	DCOCTL = CALDCO_8MHZ;

	P2SEL  &= ~BIT6;						// Setup P2.6 as GPIO not XIN
	P2SEL2 &= ~BIT6;
	P2DIR &= ~BIT6;
	P2IFG &= ~BIT6;						// Clear any interrupt flag
	P2IE  |= BIT6;						// Enable PORT 2 interrupt on pin change

	HIGH_2_LOW;
	P1DIR |= BIT0 | BIT6;				// Enable updates to the LED
	P1OUT &= ~(BIT0 | BIT6);			// An turn the LED off

	TA0CCR0 = 0x8000;					// create a 16mS roll-over period
	TACTL &= ~TAIFG;					// clear flag before enabling interrupts = good practice
	TACTL = ID_3 | TASSEL_2 | MC_1;		// Use 1:1 presclar off MCLK and enable interrupts

	_enable_interrupt();
}

// -----------------------------------------------------------------------
// Since the IR decoder is connected to P2.6, we want an interrupt
// to occur every time that the pin changes - this will occur on
// a positive edge and a negative edge.
//
// Negative Edge:
// The negative edge is associated with end of the logic 1 half-bit and
// the start of the logic 0 half of the bit.  The timer contains the
// duration of the logic 1 pulse, so we'll pull that out, process it
// and store the bit in the global irPacket variable. Going forward there
// is really nothing interesting that happens in this period, because all
// the logic 0 half-bits have the same period.  So we will turn off
// the timer interrupts and wait for the next (positive) edge on P2.6
//
// Positive Edge:
// The positive edge is associated with the end of the logic 0 half-bit
// and the start of the logic 1 half-bit.  There is nothing to do in
// terms of the logic 0 half bit because it does not encode any useful
// information.  On the other hand, we going into the logic 1 half of the bit
// and the portion which determines the bit value, the start of the
// packet, or if the timer rolls over, the end of the ir packet.
// Since the duration of this half-bit determines the outcome
// we will turn on the timer and its associated interrupt.
// -----------------------------------------------------------------------
#pragma vector = PORT2_VECTOR			// This is from the MSP430G2553.h file

__interrupt void pinChange (void) {

	int8	pin;
	int16	pulseDuration;			// The timer is 16-bits

	if (IR_PIN)		pin=1;	else pin=0;

	switch (pin) {					// read the current pin level
		case 0:						// !!!!!!!!!NEGATIVE EDGE!!!!!!!!!!
			pulseDuration = TAR;
			
			// if the pulse duration is within range of a possible 0 or 1 data value,
			// the pulse duration is changed to a 0 or a 1. If the pulse duration is within the
			// range of a possible start bit, the pulse duration is changed to a 2 so that
			// in the main loop, the irPacket doesn't include start bits.
			if (pulseDuration >= minLogic1Pulse && pulseDuration <= maxLogic1Pulse) {
				pulseDuration = 1;
			} else if (pulseDuration >= minLogic0Pulse && pulseDuration <= maxLogic0Pulse ) {
				pulseDuration = 0;
			}else {
				pulseDuration = 2;
			}
			
			// Set all of the new pulse durations to the array of packetData
			packetData[packetIndex++] = pulseDuration;
			LOW_2_HIGH; // Setup pin interrupr on positive edge
			break;

		case 1:							// !!!!!!!!POSITIVE EDGE!!!!!!!!!!!
			TAR = 0x0000;						// time measurements are based at time 0
			HIGH_2_LOW; 						// Setup pin interrupr on falling edge
			break;
	} // end switch

	P2IFG &= ~BIT6;			// Clear the interrupt flag to prevent immediate ISR re-entry

	// set an interrupt flag for if the program has experienced an interrupt (a remote button press)
	if (packetIndex > 33) {
		flagged=1;
	}
} // end pinChange ISR


// Timer interrupt below was not used:

// -----------------------------------------------------------------------
//			0 half-bit	1 half-bit		TIMER A COUNTS		TIMER A COUNTS
//	Logic 0	xxx
//	Logic 1
//	Start
//	End
//
// -----------------------------------------------------------------------
//#pragma vector = TIMER0_A1_VECTOR			// This is from the MSP430G2553.h file
//__interrupt void timerOverflow (void) {
//	TACTL &= ~TAIFG;
//}
