/* =============================================================================
# PSG_AY38910BF

- Version: 1.0 (08/02/2025)
- Author: mvac7/303bcn
- Architecture: MSX
- Format: SDCC Relocatable object file (.rel)
- Programming language: C and Z80 assembler
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
- v1.0 (08/02/2025) 
	- update to SDCC (4.1.12) Z80 calling conventions
	- Added a function to initialize audio system with different port and 
	  buffer (InitAYbuffer)
	- Added function to clear the AY registers buffer (ClearAY and ClearAYbuffer)
	- Added two functions to play a given buffer and port (Dump2AY).
	- Added two functions to mute PSG (SilenceAY and SilenceAYbuffer).
	- Add a function to select default AY (SelectAY).
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
char AY_defIOport = AY_INTERNAL;		//new (v1.0) set default AY (first port of 3)

unsigned int AY_defAYREGs_addr;

char AYREGS[14];	// buffer of AY registers





/* =============================================================================
SelectAY
Function:		SelectAY(port)
Description:	Select the default AY using the index port value.
Input:			[char] AY index port (0xA0 for internal or 0x10 for external)
Output:			-
============================================================================= */
void SelectAY(char port)
{
	AY_defIOport=port;
}



/* =============================================================================
InitAY
Function:		InitAY()
Description:	Initialize the library. 
				Select as default the internal AY and the library AY buffer. 
				Also initialize the buffer.
Input:			-
Output:			-
============================================================================= */
void InitAY(void)
{
	InitAYbuffer(AY_INTERNAL,(unsigned int) AYREGS);
}



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
void InitAYbuffer(char port, unsigned int bufferADDR) __naked
{
port;	  		//A
bufferADDR;		//DE
__asm
//set Internal AY for default
	ld   (#_AY_defIOport),A

//set default AY buffer	
	ld   (#_AY_defAYREGs_addr),DE

	ex   DE,HL
	jr   _ClearAYbuffer
__endasm;
}



/* =============================================================================
ClearAY
Function:		ClearAY()
Description:	Initializes default buffer of AY registers.
Input:			-
Output:			-
============================================================================= */
void ClearAY(void) __naked
{
__asm
	ld   HL,(#_AY_defAYREGs_addr)
//	jr   _ClearAYbuffer


/* =============================================================================
ClearAYbuffer
Function:		ClearAYbuffer(bufferADDR)
Description:	Initializes a buffer of AY registers.
Input:			[unsigned int] [HL] memory address of AY buffer
Output:			-
============================================================================= */
_ClearAYbuffer::
	xor  A
	LD   B,#13
AY_CLAYloop:
	cp   #7
	jr   Z,AY_setdef7		//IF reg=7 (mixer)
	LD   (HL),#0
AY_CLAnext:
	inc  HL
	inc  A
    djnz AY_CLAYloop

//disable envelope
    LD   (HL),#0b10000000	//reg 13 disable envelpe shape (for library use only)
    ret
	
AY_setdef7:
	LD   (HL),#0b00111000	//default mixer (in MSX system) = 0x38 
	jr   AY_CLAnext

__endasm;
}




/* =============================================================================
SOUND
Function:		SOUND(reg, value)
Description:	Writes a value to the AY buffer.
Input:			[char] register number (0 to 13)
				[char] value
Output:			-
============================================================================= */
void SOUND(char reg, char value) __naked
{
reg;		//A
value;		//L
__asm	
//    AYREGS[reg]=value; 
	ld   E,L
	call AY_getAYBUFaddrByReg	
	ld   (HL),E

	ret	

/* ------------------------------
AY_getAYBUFaddrByReg
Input:	A <--- num AY register
Output:	HL --> mem addr
regs:	BC
------------------------------ */
AY_getAYBUFaddrByReg::
	ld   C,A
	ld   B,#0
	ld   HL,(#_AY_defAYREGs_addr)	
	add  HL,BC
	ret
__endasm;
}  



/* =============================================================================
GetSound
Function:		GetSound(reg)
Description:	Read a register value from the AY buffer.
Input:			[char] register number (0 to 13)
Output:			[char] value 
============================================================================= */
char GetSound(char reg) __naked
{
reg;		//A
__asm	
//    return AYREGS[reg];
	call AY_getAYBUFaddrByReg		//A-->reg;-->HL=addr;regs:BC
	
	ld   A,(HL)
	ret	

__endasm;
}



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
void PlayAY(void) __naked
{
__asm
	ld   A,(#_AY_defIOport)
	ld   HL,(#_AY_defAYREGs_addr)
	
	call AY_DumpHL
	
	set  7,(HL)     /*	mark the sound enevelope value so that it is not 
						triggered every frame */
	
	ret	
__endasm;
/*	Dump2AY(AY_defIOport,AY_defAYREGs_addr);
	
	SOUND(AY_EnvShape,0b10000000);
	//AYREGS[AY_EnvShape]=0b10000000;	//disable enevelope shape (reg 13). 
									//This is necessary so that it doesn't hit every frame.
*/
}



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
void Dump2AY(char port, unsigned int bufferADDR) __naked
{
port;	  		//A
bufferADDR;		//DE	
__asm

	ex   DE,HL
AY_DumpHL:
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
//	ld   A,(#_AYREGS + AY_Mixer)
	push HL
	ld   C,#AY_Mixer
	ld   B,#0
	add  HL,BC
	ld   A,(HL)
	and  #0b00111111
	ld   B,A
	ld   C,E			//AY index port
	 
	ld   A,#AY_Mixer
	out  (C),A
	inc  C
	inc  C
	in   A,(C)			//C=AY read port
	and  #0b11000000	//Mask to catch two bits of joys 
	or   B		    	//I add the new mixer state collected from the buffer

//	res	6,A
//	set	7,A

//	ld   (#_AYREGS + AY_Mixer),A
	ld   (HL),A
	pop  HL
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

	ret

__endasm;
}



/* =============================================================================
SilenceAY
Function:		SilenceAY()
Description:	Silences default AY sound processor.
				Run SilenceAYbuffer with the default port and buffer parameters.
Input:			-
Output:			-
============================================================================= */
void SilenceAY(void) __naked
{
//	SilenceAY(AY_defIOport,AY_defAYREGs_addr);
__asm
	ld	A,(_AY_defIOport)
	ld	DE,(_AY_defAYREGs_addr)



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
_SilenceAYbuffer::
	ex   DE,HL
	push HL
	
	ld   E,#AY_AmpA
	ld   D,#0
	add  HL,DE
	
//clear volume registers
	ld   (HL),D
	inc  HL
	ld   (HL),D
	inc  HL
	ld   (HL),D
	
	pop  HL
	jp   AY_DumpHL		//dump to selected AY (A)
__endasm;
}