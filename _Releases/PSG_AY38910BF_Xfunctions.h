/* =============================================================================
PSG_AY38910BF_Xfunctions
PSG AY-3-8910 Buffer eXtended functions MSX SDCC Library (fR3eL Project)
============================================================================= */
#ifndef  __AY38910BF_Xfunc_H__
#define  __AY38910BF_Xfunc_H__





#ifndef _SWITCHER
#define _SWITCHER
  typedef enum {OFF = 0, ON = 1} switcher;
#endif




/* =============================================================================
SetTonePeriod(channel, period) 

Function:	Set Tone Period for any channel
Input   :	[char] channel (0, 1 or 2)
			[unsigned int] period (0 - 4095)
Output  :	-
============================================================================= */
void SetTonePeriod(char channel, unsigned int period);



/* =============================================================================
SetNoisePeriod(period) 

Function:	Set Noise Period
Input   :	[char] period (0 - 31) 
Output  :	- 
============================================================================= */
void SetNoisePeriod(char period);



/* =============================================================================
SetEnvelopePeriod(period) 

Function:	Set Envelope Period
Input   :	[unsigned int] period (0 - 65535) 
Output  :	- 
============================================================================= */
void SetEnvelopePeriod(unsigned int period);



/* =============================================================================
SetVolume(channel, volume) 

Function:	Set volume channel
Input   :	[char] channel (0, 1 or 2)
			[char] volume, 0 to 15 or 16 for activate envelope
Output  :	-
============================================================================= */
void SetVolume(char channel, char volume);



/* =============================================================================
SetChannel(channel, isTone, isNoise)

Function:	Mixer. Enable/disable Tone and Noise channels.
Input   :	[char] channel (0, 1 or 2)
			[switcher] tone state (ON=1;OFF=0)
			[switcher] noise state (ON=1;OFF=0)
Output  :	-
============================================================================= */
void SetChannel(char channel, switcher isTone, switcher isNoise);



/* =============================================================================
SetEnvelope(shape) 

Function:	Set envelope shape.
			It will only affect channels that have the envelope active (see SetVolume)
Input   :	[char] Envelope shape (0-15) (see envelope shapes definitions)
Output  :	-
============================================================================= */
void SetEnvelope(char shape);




#endif