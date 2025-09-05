/* =============================================================================
PSG_AY38910BF
PSG AY-3-8910 Buffer MSX SDCC Library (fR3eL Project)
============================================================================= */
#ifndef  __AY38910BF_H__
#define  __AY38910BF_H__



#ifndef AY_REGISTERS
#define AY_REGISTERS
#define AY_ToneA			0 //Channel A Tone Period (12 bits)
#define AY_ToneA_fine		0 //Channel A Fine Tune   (8 bits)
#define AY_ToneA_coarse		1 //Channel A Coarse Tune (4 bits)
#define AY_ToneB			2 //Channel B Tone Period (12 bits)
#define AY_ToneB_fine		2 //Channel B Fine Tune   (8 bits)
#define AY_ToneB_coarse		3 //Channel B Coarse Tune (4 bits)
#define AY_ToneC			4 //Channel C Tone Period (12 bits)
#define AY_ToneC_fine		4 //Channel C Fine Tune   (8 bits)
#define AY_ToneC_coarse		5 //Channel C Coarse Tune (4 bits)
#define AY_Noise			6 //Noise Period (5 bits)
#define AY_Mixer			7 //Mixer
#define AY_AmpA				8 //Channel Volume A (4 bits + B5 active Envelope)
#define AY_AmpB				9 //Channel Volume B (4 bits + B5 active Envelope)
#define AY_AmpC				10 //Channel Volume C (4 bits + B5 active Envelope)
#define AY_EnvPeriod		11 //Envelope Period (16 bits)
#define AY_EnvPeriod_fine	11 //Envelope Fine Tune   (8 bits)
#define AY_EnvPeriod_coarse	12 //Envelope Coarse Tune (8 bits)
#define AY_EnvShape			13 //Envelope Shape
#endif



//AY envelope shapes
#define AY_ENV_LowerBeat      1	// (0,1,2,3 and 9)<-- 0 can be useful in case you need to control when the envelope is triggered
#define AY_ENV_Upper          4	// (4,5,6,7 and 15)
#define AY_ENV_LeftSaw        8	//
#define AY_ENV_LowerTriangle 10	//
#define AY_ENV_LowerHold     11	//
#define AY_ENV_RightSaw      12	//
#define AY_ENV_UpperHold     13	//
#define AY_ENV_UpperTriangle 14	//



//AY channels
#define AY_Channel_A 0
#define AY_Channel_B 1
#define AY_Channel_C 2



//AY port 
#define AY_INTERNAL 0xA0	//MSX internal PSG
#define AY_EXTERNAL 0x10	//External PSG (MEGAFLASHROM SCC+, Flashjacks, Yamanooto, Carnivore2 or others)




extern char AY_defIOport;	//new (v1.7) AY first port

extern unsigned int AY_defAYREGs_addr;

extern char AYREGS[14];		// buffer of AY registers




/* =============================================================================
SelectAY
Function:		SelectAY(port)
Description:	Select the default AY using the index port value.
Input:			[char] AY index port (0xA0 for internal or 0x10 for external)
Output:			-
============================================================================= */
void SelectAY(char port);



/* =============================================================================
InitAY
Function:		InitAY()
Description:	Initialize the library. 
				Select as default the internal AY and the library AY buffer. 
				Also initialize the buffer.
Input:			-
Output:			-
============================================================================= */
void InitAY(void);



/* =============================================================================
InitAYbuffer
Function:		InitAYbuffer(port, bufferADDR)
Description:	Initialize the library. 
				Sets the default AY and default AY buffer.
				Also initialize the buffer.
Input:			[char] AY index port (0xA0 for internal or 0x10 for external)
				[unsigned int] memory address of AY buffer
Output:			-
============================================================================= */
void InitAYbuffer(char port, unsigned int bufferADDR);



/* =============================================================================
ClearAY
Function:		ClearAY()
Description:	Initializes default buffer of AY registers.
Input:			-
Output:			-
============================================================================= */
void ClearAY(void);



/* =============================================================================
ClearAYbuffer
Function:		ClearAYbuffer(bufferADDR)
Description:	Initializes a buffer of AY registers.
Input:			[unsigned int] [HL] memory address of AY buffer
Output:			-
============================================================================= */
extern void ClearAYbuffer(unsigned int bufferADDR);



/* =============================================================================
SOUND
Function:		SOUND(reg, value)
Description:	Writes a value to the AY buffer.
Input:			[char] register number (0 to 13)
				[char] value
Output:			-
============================================================================= */
void SOUND(char reg, char value);



/* =============================================================================
GetSound
Function:		GetSound(reg)
Description:	Read a register value from the AY buffer.
Input:			[char] register number (0 to 13)
Output:			[char] value 
============================================================================= */
char GetSound(char reg);



/* =============================================================================
PlayAY
Function:		PlayAY()
Description:	Dump default AY buffer to default AY.

				Execute on each interruption of VBLANK or when you want to throw 
				in a change in sound.
				
				Attention! #####################################################
				PlayAY uses the Dump2AY function with the AY I/O port number and 
				buffer configured in the library. 
				It includes a control that Dump2AY doesn't have: it adds a flag 
				to register 13 (envelope shape) after playing.
				This is necessary because this function is designed to be 
				executed on every frame, preventing the envelope from being 
				triggered continuously, thus avoiding the generation of an 
				unwanted sound. 
				
				It is likely that the music or sound effects player control this 
				problem.
				
				It has been included to ensure that whether we use it with a 
				Player or independently this problem does not occur.
				
				Remember that once you write to register 13, it is the PSG that 
				applies the envelope according to the waveform and period 
				parameters.
				
				For more information, you can see the technical documentation.
				################################################################
Input:			-
Output:			-
============================================================================= */
void PlayAY(void);



/* =============================================================================
Dump2AY
Function:		Dump2AY(port, bufferADDR)
Description:	Dump an AY buffer to the indicated AY

				Attention! #####################################################
				This function does not disable the envelope trigger (register 13).
				It is intended for when you use two AYs simultaneously playing a 
				song and FX. You must first transfer the registers to the second 
				AY with this function and then to the main AY with PlayAY.
				
				For more information, you can see the technical documentation.
				################################################################

Input:			[char] AY index port (0xA0 for internal or 0x10 for external)
				[unsigned int] memory address of AY buffer
Output:			-
============================================================================= */
void Dump2AY(char port, unsigned int bufferADDR);



/* =============================================================================
SilenceAY
Function:		SilenceAY()
Description:	Silences default AY sound processor.
				Run SilenceAYbuffer with the default port and buffer parameters.
Input:			[char] AY index port
Output:			-
============================================================================= */
void SilenceAY(void);



/* =============================================================================
SilenceAYbuffer
Function:		SilenceAYbuffer(port,bufferADDR)
Description:	Silences an AY sound processor.
				It resets the amplitude value of the three channels to zero and 
				dumps it to the AY.
				This is indicated for the case of playing sound dynamically 
				between AYs (Internal/External), so that the last written 
				values ​​do not sound infinitely.
Input:			[char][A] AY index port (0xA0 for internal or 0x10 for external)
				[unsigned int][DE] memory address of AY buffer
Output:			-
============================================================================= */
extern void SilenceAYbuffer(char port, unsigned int bufferADDR);



#endif