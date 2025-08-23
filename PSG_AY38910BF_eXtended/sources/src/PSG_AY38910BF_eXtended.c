/* =============================================================================
# PSG_AY38910BF_eXtended

- Version: 1.1 (14/08/2025)
- Author: mvac7/303bcn
- Architecture: MSX
- Format: SDCC Relocatable object file (.rel)
- Programming language: C and Z80 assembler
- Compiler: SDCC 4.4 
- Library: fR3eL Project

##Description:  
PSG AY-3-8910 Buffer eXtended functions MSX SDCC Library (fR3eL Project)
Extension of the PSG_AY38910BF library.
 
Adds specific functions to make it easier to write AY parameters.
 
## History of versions (dd/mm/yyyy):
- v1.1 (14/08/2025) 
	- Added EnableEnvelope, EnableTone and EnableNoise functions
	- Removed SetChannel function.
- v1.0 (03/03/2025) First version
============================================================================= */

#include "../include/PSG_AY38910BF.h"
#include "../include/PSG_AY38910BF_eXtended.h"




/* =============================================================================
SetTonePeriod
Function:		SetTonePeriod(channel, period) 
Description:	Set Tone Period for any channel
Input:			[char] channel (0, 1 or 2)
				[unsigned int] period (0 - 4095)
Output:			-
============================================================================= */
void SetTonePeriod(char channel, unsigned int period)
{
	if (channel>2) return;
	channel=channel*2;
	AYREGS[channel++]=period & 0xFF;
	AYREGS[channel]=period>>8;	//AYREGS[reg+1]=(period & 0xFF00)/0xFF;
}



/* =============================================================================
SetNoisePeriod
Function:		SetNoisePeriod(period) 
Description:	Set Noise Period
Input:			[char] period (0 - 31) 
Output:			-
============================================================================= */
void SetNoisePeriod(char period){
	AYREGS[AY_Noise]=period;
}



/* =============================================================================
SetVolume
Function:		SetVolume(channel, volume) 
Description:	Set volume channel
Input:			[char] channel (0, 1 or 2)
				[char] volume, 0 to 15 or 16 for activate envelope
Output:			-
============================================================================= */
void SetVolume(char channel, char volume)
{
	if (channel>2) return;
	AYREGS[AY_AmpA+channel]=volume;
}



/* =============================================================================
EnableTone
Function:		EnableTone(channel, state)
Description:	Mixer. 
				Enables or disables Tone channel.
Input:			[char] channel (0, 1 or 2)
				[switcher] tone state (ON=enable;OFF=disable)
Output:			-
============================================================================= */
void EnableTone(char channel, switcher state) __naked
{
channel;	//A
state;		//L
__asm

//if (channel>2) return;
	cp   #3
	ret  NC		//if A>=3 then ret

	ld	 C,#0x01

AYx_SETbitMIX:
//	bit<<=channel;
	ld   B,A			//channel value		
	inc	 B
	jr	 AYx_firstIter$
AYx_gotoChannBit$:
	sla	 C
AYx_firstIter$:
	djnz AYx_gotoChannBit$
	
//	newValue = AYREGS[AY_Mixer];
	ld	 DE,#_AYREGS+7
	ld	 A,(DE)

//	if(state==ON) newValue&=~bit; 
	bit	 0,L
	jr	 Z,AYx_TONEOFF$
//Tone channel ON	
	ld	 B,A
	ld	 A,C
	cpl
	and	 A,B

	jr   AYx_TONEMIX$
AYx_TONEOFF$:
//Tone channel OFF
//	else newValue|=bit;
	or   A,C

AYx_TONEMIX$:
//	AYREGS[AY_Mixer] = newValue;
	ld   (DE),A
	
	ret
__endasm;
}



/* =============================================================================
EnableNoise
Function:		EnableNoise(channel, state)
Description:	Mixer. 
				Enables or disables noise on a channel.
Input:			[char] channel (0, 1 or 2)
				[switcher] noise state (ON=enable;OFF=disable)
Output:			-
============================================================================= */
void EnableNoise(char channel, switcher state) __naked
{
channel;	//A
state;		//L
__asm

//if (channel>2) return;
	cp   #3
	ret  NC		//>=3

	ld	 C,#0b00001000
	jr   AYx_SETbitMIX
	
__endasm;
}




/* =============================================================================
EnableEnvelope
Function:		EnableEnvelope(channel, isEnvelope) 
Description:	Enables or disables sound envelope on a channel.
Input:			[char] channel (0, 1 or 2)
				[switcher] Envelope state (ON=enable;OFF=disable)
Output:			-
============================================================================= */
void EnableEnvelope(char channel, switcher state)
{
	char value;
	if (channel>2) return;
	channel+=AY_AmpA;
	value = AYREGS[channel];
	if(state) value|=0b00010000;
	else value&=0B00001111;
	AYREGS[channel]=value;
}



/* =============================================================================
SetEnvelopePeriod
Function:		SetEnvelopePeriod(period) 
Description:	Set Envelope Period
Input:			[unsigned int] period (0 - 65535) 
Output:			-
============================================================================= */
void SetEnvelopePeriod(unsigned int period){
	AYREGS[AY_EnvPeriod]=period & 0xFF;
	AYREGS[AY_EnvPeriod+1]=period>>8;	//AYREGS[12]=(period & 0xFF00)/0xFF;
}



/* =============================================================================
SetEnvelope
Function:		SetEnvelope(shape) 
Description:	Set envelope shape.
				It will affect all channels that have the sound envelope active.
				Use SetVolume function to activate the sound envelope.
Input:			[char] Envelope shape (0-15) (use envelope shapes definitions)
Output:			-
============================================================================= */
void SetEnvelope(char shape)
{
	AYREGS[AY_EnvShape]=shape;
}

