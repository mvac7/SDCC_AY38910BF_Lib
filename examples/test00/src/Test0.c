#include "../include/PSG_AY38910BF.h"

#define  HALT	 __asm halt __endasm

void main(void)
{
	unsigned int time = 10*50;	//10 seg in PAL
	
	InitAY();    //Init library (set default AY and clear Buffer)
	
	SOUND(AY_ToneA_fine,1000&0xFF);
	SOUND(AY_ToneA_coarse,1000>>8);
	SOUND(AY_Noise,20);
	SOUND(AY_AmpA,16);
	SOUND(AY_Mixer,0B00110110);
	SOUND(AY_EnvPeriod_fine,700&0xFF);
	SOUND(AY_EnvPeriod_coarse,700>>8);
	SOUND(AY_EnvShape,AY_ENV_UpperTriangle);
	
	PlayAY();
	
	while(time-->0) HALT;
}
