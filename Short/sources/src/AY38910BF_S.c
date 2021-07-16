/* =============================================================================
   AY38910BF
   PSG AY-3-8910 Buffer Short MSX SDCC Library (fR3eL Project)
   Version: 0.9b (16 July 2021)
   Author: mvac7
   Architecture: MSX
   Format: C Object (SDCC .rel)
   Programming language: C and Z80 assembler

   Description:                                                              
     Library for accessing the AY38910 through a buffer of registers.
     
     It does not use the BIOS so it can be used to program ROMs or 
     MSX-DOS executables.
     
   History of versions:
     v0.9b (16 July 2021) First version (Based in AY-3-8910 RT Library)
============================================================================= */
#include "../include/AY38910BF_S.h"



//intern AY ports
#define AY0index 0xA0
#define AY0write 0xA1
#define AY0read  0xA2

//external AY ports (MEGAFLASHROM-SCC+,)
#define AY1index 0x10
#define AY1write 0x11
#define AY1read  0x12



char AY_TYPE;      // 1 = External AY  Else Internal AY

char AYREGS[14];   // buffer of AY registers





/* =============================================================================
 AY_Init

 Function : Initialize the buffer
 Input    : -
 Output   : -
============================================================================= */
void AY_Init() __naked
{
__asm

    XOR	A
    ld   (#_AY_TYPE),A    ;set Internal AY

    ; Clear PSG buffer 
    LD   HL,#_AYREGS
    LD   DE,#_AYREGS+1
    LD   BC,#13
    LD   (HL),A
    LDIR
 
    dec  A        ;A = #0xFF
    LD   (HL),A   ;reg 13 envelpe shape
    
    ret
__endasm;
}



/* =============================================================================
 PlayAY() 

 Function : Copy buffer to AY (internal or external).
 Input    : -
 Output   : -
============================================================================= */
//void PlayAY(unsigned int bufferADDR, char AY_TYPE) __naked
void PlayAY() __naked
{
__asm
  push IX

;  get the address of the buffer as a parameter --------------------------------
;    ld    IX,#0
;    add   IX,SP    
;    ld   L,4(IX)
;    ld   H,5(IX)
      
  ld   HL,#_AYREGS

;    ld   A,6(IX)  ;get the internal/external AY indicator by parameter    
  ld   A,(#_AY_TYPE)
  CP   #1
  jr   Z,EXTERN_AY

;internal AY  
;control of I/O bits of register 7 in intern AY
  ld   A,(#_AYREGS + AY_Mixer)
  AND  #0b00111111
  ld   B,A
     
  ld   A,#AY_Mixer
  out  (#AY0index),A
  in   A,(#AY0read)
  and  #0b11000000	; Mask to catch two bits of joys 
  or   B		    ; I add the new mixer state collected from the buffer
  
  ld   (#_AYREGS + AY_Mixer),A
  
  xor  A	
  ld   B,#13
  ld   C,#AY0write  ;port
AY0_LOOP:
  out  (#AY0index),A
  inc  A
  outi          ; out(C),(HL) / inc HL / dec B
  JR   NZ,AY0_LOOP
  
  out  (#AY0index),A
  jr   SETSHAPE

  
EXTERN_AY:
  xor  A	
  ld   B,#13
  ld   C,#AY1write  ;port
AY1_LOOP:
  out  (#AY1index),A
  inc  A
  outi          ; out(C),(HL) / inc HL / dec B
  JR   NZ,AY1_LOOP
  
  out  (#AY1index),A

SETSHAPE:  
;Envelope shape (reg 13)    
  ld   A,(HL)
  and  A          
  jp   M,END_playAY   ;A=255
  
  out  (C),A
  ld   (HL),#255     ; change the value so that it is not triggered every frame

END_playAY:  
  pop  IX
  ret

__endasm;
}  
