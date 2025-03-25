/* =============================================================================
PSG_AY38910BF_Xfunc
PSG AY-3-8910 Buffer eXtended functions MSX SDCC Library (fR3eL Project)
Version: 1.0 (03/03/2025)
Author: mvac7/303bcn
Architecture: MSX
Format: C Object (SDCC .rel)
Programming language: C and Z80 assembler
Compiler: SDCC v4.4 or newer

Description:                                                              
 Extension of the PSG_AY38910BF library.
 
 Adds specific functions to make it easier to write AY parameters.
 
History of versions: (dd/mm/yyyy)
- v1.0  (03/03/2025) First version
============================================================================= */

#include "../include/PSG_AY38910BF.h"
#include "../include/PSG_AY38910BF_Xfunctions.h"




/* =============================================================================
SetTonePeriod(channel, period) 

Function:	Set Tone Period for any channel
Input   :	[char] channel (0, 1 or 2)
			[unsigned int] period (0 - 4095)
Output  :	-
============================================================================= */
void SetTonePeriod(char channel, unsigned int period)
{
	char reg;
	if (channel>2) return;
	reg=channel*2;
	AYREGS[reg]=period & 0xFF;
	AYREGS[reg+1]=period>>8;	//AYREGS[reg+1]=(period & 0xFF00)/0xFF;
}



/* =============================================================================
SetNoisePeriod(period) 

Function:	Set Noise Period
Input   :	[char] period (0 - 31) 
Output  :	- 
============================================================================= */
void SetNoisePeriod(char period){
	AYREGS[AY_Noise]=period;
}



/* =============================================================================
SetEnvelopePeriod(period) 

Function:	Set Envelope Period
Input   :	[unsigned int] period (0 - 65535) 
Output  :	- 
============================================================================= */
void SetEnvelopePeriod(unsigned int period){
	AYREGS[AY_EnvPeriod]=period & 0xFF;
	AYREGS[AY_EnvPeriod+1]=period>>8;	//AYREGS[12]=(period & 0xFF00)/0xFF;
}



/* =============================================================================
SetVolume(channel, volume) 

Function:	Set volume channel
Input   :	[char] channel (0, 1 or 2)
			[char] volume, 0 to 15 or 16 for activate envelope
Output  :	-
============================================================================= */
void SetVolume(char channel, char volume)
{
	if (channel>2) return;
	AYREGS[8+channel]=volume;
}



/* =============================================================================
SetChannel(channel, isTone, isNoise)

Function:	Mixer. Enable/disable Tone and Noise channels.
Input   :	[char] channel (0, 1 or 2)
			[switcher] tone state (ON=1;OFF=0)
			[switcher] noise state (ON=1;OFF=0)
Output  :	-
============================================================================= */
void SetChannel(char channel, switcher isTone, switcher isNoise)
{
	char newValue;

	if (channel>2) return;

	newValue = AYREGS[AY_Mixer];


	//control of the two I/O bits of register 7 is done in the SOUND function
	if(channel==0) 
	{
		if(isTone==ON){newValue&=254;}else{newValue|=1;}
		if(isNoise==ON){newValue&=247;}else{newValue|=8;}
	}
	if(channel==1)    
	{
		if(isTone==ON){newValue&=253;}else{newValue|=2;}
		if(isNoise==ON){newValue&=239;}else{newValue|=16;}
	}
	if(channel==2)
	{ 
		if(isTone==ON){newValue&=251;}else{newValue|=4;}
		if(isNoise==ON){newValue&=223;}else{newValue|=32;}
	}
	AYREGS[AY_Mixer] = newValue;
}



/* =============================================================================
SetEnvelope(shape) 

Function:	Set envelope shape.
			It will only affect channels that have the envelope active (see SetVolume)
Input   :	[char] Envelope shape (0-15) (see envelope shapes definitions)
Output  :	-
============================================================================= */
void SetEnvelope(char shape)
{
	AYREGS[AY_EnvShape]=shape;
}

