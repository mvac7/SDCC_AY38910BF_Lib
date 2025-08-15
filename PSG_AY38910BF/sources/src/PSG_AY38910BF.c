/* =============================================================================
# PSG_AY38910BF

- Version: 1.0 (08/02/2025)
- Author: mvac7/303bcn
- Architecture: MSX
- Format: SDCC Relocatable object file (.rel)
- language: C and Z80 assembler
- Compiler: SDCC v4.4
- Library: fR3eL Project

## Description:                        
PSG AY-3-8910 Buffer MSX SDCC Library (fR3eL Project)
Library for accessing the AY38910 through a buffer of registers values.

This version is a Short version, which only incorporates functions 
for music playback (Vortex or WYZplayer) and sound effects (ayFX).

Requires executing the PlayAY function on each frame (VBLANK).

It does not use the BIOS so it can be used to program for ROMs,
MSX BASIC or MSX-DOS environments. 
 
## History of versions (dd/mm/yyyy):
- v1.0  (08/02/2025) update to SDCC (4.1.12) Z80 calling conventions
- v0.9b (16/07/2021) First version (Based in AY-3-8910 RT Library)
============================================================================= */
#include "../include/PSG_AY38910BF.h"


/*
## Internal AY
index 0xA0
write 0xA1
read  0xA2

## External AY (MEGAFLASHROM-SCC+, Flashjacks, Yamanooto, Carnivore2 or others)
index 0x10
write 0x11
read  0x12
*/
char AY_IOport = AY_INTERNAL;		//new (v1.0) set default AY (first port of 3)

char AYREGS[14];	// buffer of AY registers



/* =============================================================================
InitAY
Function:		InitAY()
Description:	Initialize the library. 
				Set default AY (internal) and clear buffer.
Input:			-
Output:			-
============================================================================= */
void InitAY(void) __naked
{
__asm
	
//set Internal AY for default
	ld   A,#AY_INTERNAL
	ld   (#_AY_IOport),A

/* =============================================================================
ClearDefAYbuffer
Function:		ClearDefAYbuffer()
Description:	Clear default AY buffer (AYREGS).
============================================================================= */
_ClearDefAYbuffer::
	LD   HL,#_AYREGS
	
/* =============================================================================
ClearAYbuffer
Function:		ClearAYbuffer(bufferADDR)
Description:	Clear indicated AY buffer.
Input:			[unsigned int] [HL] buffer address of AY registers
============================================================================= */
_ClearAYbuffer::
	ld   D,H
	ld   E,L
	inc  DE
    LD   BC,#13
    LD   (HL),#0
    LDIR

//disable envelope
    LD   (HL),#0b10000000		//reg 13 disable envelpe shape
    
    ret
__endasm;
}



/* =============================================================================
SOUND
Function:		SOUND(reg, value)
Description:	Writes a value to the PSG register buffer
Input:			[char] register number (0 to 13)
				[char] value
Output:			-
============================================================================= */
void SOUND(char reg, char value)
{
    AYREGS[reg]=value; 
}  



/* =============================================================================
GetSound
Function:		GetSound(reg)
Description:	Read PSG register value (from buffer)
Input:			[char] register number (0 to 13)
Output:			[char] value 
============================================================================= */
char GetSound(char reg)
{
    return AYREGS[reg];
}



/* =============================================================================
SilenceAY
Function:		SilenceAY()
Description:	Silences selected AY sound processor.
Input:			[char] AY index port
Output:			-
============================================================================= */
void SilenceAY(void)
{
	SilenceAYbyPort(AY_IOport);
}



/* =============================================================================
SilenceAYbyPort
Function:		SilenceAYbyPort(port)
Description:	Silences the indicated AY sound processor.
				Set to zero the amplitude value by writing directly to the AY 
				registers.
				This is indicated for the case of playing sound dynamically 
				between AYs (Internal/External), so that the last written 
				values ​​do not sound infinitely.
Input:			[char] AY index port
Output:			-
============================================================================= */
void SilenceAYbyPort(char port) __naked
{
port;		//A
__asm
	ld   E,A	//AY index port (Internal or External AY)
	ld   D,#AY_AmpA
	xor  A
	ld   B,#3

AY_ampLOOP:
	ld   C,E	//set AY index port
	out  (C),D	//AY register
	inc  D
	inc  C		//go to AY write port
	out  (C),A	//write to AY reg
	djNZ AY_ampLOOP
	ret
__endasm;	
}



/* =============================================================================
PlayAY
Function:		PlayAY()
Description:	Copy buffer to selected AY (AY_IOport)
Input:			-
Output:			-
============================================================================= */
void PlayAY(void)
{
	Dump2AY(AY_IOport,(unsigned int) AYREGS);
	
	AYREGS[AY_EnvShape]=0b10000000;	//disable enevelope shape (reg 13). 
									//This is necessary so that it doesn't hit every frame.
}



/* =============================================================================
Dump2AY
Function:		Dump2AY(port, bufferADDR)
Description:	Dump a buffer to the indicated AY
Input:			[char] AY index port
				[unsigned int] buffer address of AY registers
Output:			-
============================================================================= */
void Dump2AY(char port, unsigned int bufferADDR) __naked
{
port;	  		//A
bufferADDR;		//DE	
__asm

	ex   DE,HL
	ld   E,A
	cp   #AY_INTERNAL
	jr   NZ,AY_WRITEREGS


/* #########################################################
control of I/O bits of register 7 in intern AY
############################################################ MIXER IO bits
Register 7 must always contain 10xxxxxx or possible damage 
could result to the PSG, there are active devices connected 
to its I/O pins. 
############################################################ */
	ld   A,(#_AYREGS + AY_Mixer)
	and  #0b00111111
	ld   B,A
	ld   C,E
	 
	ld   A,#AY_Mixer
	out  (C),A			//C=AY index port
	inc  C
	inc  C
	in   A,(C)			//C=AY read port
	and  #0b11000000	//Mask to catch two bits of joys 
	or   B		    	//I add the new mixer state collected from the buffer

//	res	6,A
//	set	7,A

	ld   (#_AYREGS + AY_Mixer),A
//########################################################## END MIXER IO bits


	
AY_WRITEREGS:
	xor  A	
	ld   B,#13

AY_WriteLOOP:
	ld   C,E		//set index port
	out  (C),A		//AY register
	inc  A
	inc  C			//go to write port
	outi			//set value --> out(C),(HL) / inc HL / dec B	
	jr   NZ,AY_WriteLOOP 

//Set Envelope shape (reg 13)
	ld   A,(HL)		//read shape value
	bit  7,A          
	ret  NZ			//IF b7=1 do not trigger envelope shape
  
	ld   B,#AY_EnvShape
	ld   C,E		//set index port
	out  (C),B		//AY register 13
	inc  C			//go to write port
	out  (C),A		//set value


//	set  7,(HL)     //change the value so that it is not triggered every frame

	ret

__endasm;
}
