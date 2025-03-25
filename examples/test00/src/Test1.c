#include "../include/PSG_AY38910BF.h"
#include "../include/PSG_AY38910BF_Xfunctions.h"

#define  HALT	 __asm halt __endasm

void main(void)
{
	unsigned int period=0;
		
	InitAY();    			//Init library (set default AY and clear Buffer)
	AY_IOport=AY_EXTERNAL;	//Set extern AY
	
	SetNoisePeriod(20);
	SetVolume(AY_Channel_A,16); //channel A envelope on
	SetChannel(AY_Channel_A,ON,ON);
	SetEnvelopePeriod(1000);
	SetEnvelope(14);

	while(period<4096)
	{
		HALT;
		PlayAY();
		
		SetTonePeriod(AY_Channel_A, period);
		period += 8;		
	}
}
