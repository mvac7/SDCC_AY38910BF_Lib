/* =============================================================================
# Example03

- Architecture: MSX
- Format: 8K ROM
- Compiler: SDCC 4.4
- Library: fR3eL Project

## Description:
	Simple example of the PSG_AY38910BF + PSG_AY38910BF_eXtended Libraries 
	Plays a sound effect on two AYs (internal and external)
============================================================================= */
#include "PSG_AY38910BF.h"
#include "PSG_AY38910BF_eXtended.h"

#define  HALT	 __asm halt __endasm

void main(void)
{
	unsigned int period=0;
		
	InitAY();    						//Init library (set default AY and clear Buffer)
	AY_IOport=AY_EXTERNAL;				//Set extern AY
	
	SetNoisePeriod(20);					//Set noise period
	//SetVolume(AY_Channel_A,16);		//channel A envelope on
	EnableEnvelope(AY_Channel_A,ON);	//channel A envelope on
	SetChannel(AY_Channel_A,ON,ON);		//Enable Tone and Noise in channel A
	SetEnvelopePeriod(1000);			//Set envelope period
	SetEnvelope(14);					//Set envelope shape

	while(period<4096)
	{
		HALT;
		Dump2AY(AY_EXTERNAL,(unsigned int) AYREGS);	//Dump AY buffer to second AY
		PlayAY();									//Dump AY buffer to internal PSG
		
		SetTonePeriod(AY_Channel_A, period);		//Set channel A period
		period += 8;		
	}
	
	SilenceAY();						//Stop any sound!
}
