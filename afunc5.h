//-----------------------------------------------------------------
// Name:	Coulston/Dusty Weisner
// File:	lab5.c
// Date:	Fall 2014
// Purp:	Demo the decoding of an IR packet
// Weisner Documentation: none
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// Page 76 : MSP430 Optimizing C/C++ Compiler v 4.3 User's Guide
//-----------------------------------------------------------------
typedef		unsigned char		int8;
typedef		unsigned short		int16;
typedef		unsigned long		int32;
typedef		unsigned long long	int64;

#define		TRUE				1
#define		FALSE				0

//-----------------------------------------------------------------
// Function prototypes found in lab5.c
//-----------------------------------------------------------------
void initMSP430();
__interrupt void pinChange (void);
__interrupt void timerOverflow (void);


//-----------------------------------------------------------------
// Each PxIES bit selects the interrupt edge for the corresponding I/O pin.
//	Bit = 0: The PxIFGx flag is set with a low-to-high transition
//	Bit = 1: The PxIFGx flag is set with a high-to-low transition
//-----------------------------------------------------------------

#define		IR_PIN			(P2IN & BIT6)
#define		HIGH_2_LOW		P2IES |= BIT6
#define		LOW_2_HIGH		P2IES &= ~BIT6

// defined average pulse durations and min and max based on std dev.
#define		averageLogic0Pulse	590
#define		averageLogic1Pulse	1626
#define		averageStartPulse	4411
#define		minLogic0Pulse		averageLogic0Pulse - 200
#define		maxLogic0Pulse		averageLogic0Pulse + 200
#define		minLogic1Pulse		averageLogic1Pulse - 200
#define		maxLogic1Pulse		averageLogic1Pulse + 200
#define		minStartPulse		averageStartPulse - 200
#define		maxStartPulse		averageStartPulse + 200


// defined my tv remote's hex values for each button press that could be used
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


