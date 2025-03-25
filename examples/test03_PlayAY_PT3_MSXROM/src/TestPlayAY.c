/* =============================================================================
TestPlayAY
Version:	1.0 (01/03/2025)
Author:		mvac7/303bcn
Architecture: MSX
Format:		ROM 16K
language:	C and Z80 assembler
Compiler:	SDCC 4.4 or newer

PT3 songs:
- "Mathematical brain" by Makinavaja (XI MICROCOMPO AY)

Description:
 Application for test AY38910BF MSX Library with PT3player Library.
    
History of versions: (dd/mm/yyyy)
- v1.0 (01/03/2025)
============================================================================= */

#include "../include/newTypes.h"
#include "../include/msxSystemVariables.h"
#include "../include/msxBIOS.h"

#include "../include/interruptM1_Hooks.h"
#include "../include/memory_Z80.h"
#include "../include/keyboard_MSX.h"
#include "../include/VDP_TMS9918A_MSXBIOS.h"
#include "../include/VDP_PRINT.h"
#include "../include/unWRLE2toVRAM.h"

#include "../include/PSG_AY38910BF.h"
#include "../include/PT3player.h"
#include "../include/PT3player_NoteTable2.h"

//song-data .PT3
#include "../include/maki_CompoAY19v2_PT3.h"

#include "../include/Test_GFX.h"






#define VUMETERSPRBUFF 12*4



#ifndef  __BITVALUES__
#define  __BITVALUES__
#define Bit0 1
#define Bit1 2
#define Bit2 4
#define Bit3 8
#define Bit4 16
#define Bit5 32
#define Bit6 64
#define Bit7 128
#endif




// Function Declarations -------------------------------------------------------
void my_TIMI(void);


//void SilenceAY(char AYport);

//void Copy2secondAY(void);

void DumpOAM(void);

void SelectAY(char type);

void WAIT(uint cicles);
void LOCATE(char x, char y);

void ShowState(uint vaddr, boolean state);

void ShowVumeter(char channel, char value);

void InitVumeter(void);

void Pause(void);

void PlaySong(void);
//void PlaySong(char songNumber);

void SwapLoop(void);
void ShowLoop(void);
void ShowPlayback(void);
void ShowENDsong(void);


char INKEY(void);
char GetKeyMatrix(char row);



// constants  ------------------------------------------------------------------
const char text01[] = "Test AY38910BF + PT3player";
//const char text02[] = "v1.0 (1/3/2025)";

const char presskey[] = "Press a key to Play";



// global variable definition --------------------------------------------------

char VALUE;

char SPRBUFFER[VUMETERSPRBUFF];

//boolean Row6pressed;
boolean Row7pressed;

uint firstPATaddr;

//uint songNames[2];
//uint songAuthors[2];
//uint songPT3Data[2];

char _PSGtype;

switcher _loop;

char GUI_Playback;
char GUI_Loop;
char GUI_End;

//char _currentSong;

char AYREGS_plus[14];	// buffer for second AY
     

// Functions -------------------------------------------------------------------



void main(void)
{
	char keyPressed;  
//	char result;

	uint conta=0;
	uint songStep;

//	_currentSong=0;

//	Row6pressed=false;
	Row7pressed=false;
	
	GUI_Playback = 128;
	GUI_Loop = 128;
	GUI_End = 128;
	
	_loop = OFF;
	

/*	songNames[0] = (unsigned int) SONG00_name;
	songNames[1] = (unsigned int) SONG01_name;
	songAuthors[0] = (unsigned int) SONG00_author;
	songAuthors[1] = (unsigned int) SONG01_author;  
	songPT3Data[0] = (unsigned int) SONG00;
	songPT3Data[1] = (unsigned int) SONG01;
*/

	// Initialize the Player
	Player_Init();
	//AY_Init();
	//
		
	POKE(LINL32,32); //Width(32)
	COLOR(WHITE,DARK_BLUE,LIGHT_BLUE);     
	SCREEN(1);    
	SetSpritesSize(1);

	unWRLE2toVRAM16((uint) GUI_MAP,BASE5);			//show screen map
	unWRLE2toVRAM16((uint) GUI_G1_TSET_COL,BASE6);
	unWRLE2toVRAM16((uint) GUI_G1_TSET_PAT,BASE7);  

	VLOCATE(1,1);
	VPRINT(text01);
//	VLOCATE(1,2);
//	VPRINT(text02);

	SelectAY(1);
				  
	InitVumeter();

	VLOCATE(8,11);
	VPRINTN(SONG01_name,23);

	VLOCATE(8,12);
	VPRINTN(SONG01_author,23); 

	Install_TIMI(my_TIMI);

	while(1)
	{
		HALT;

		if (PT3_state & PT3_PLAY)
		{
			songStep=PT3_CrPsPtr - firstPATaddr;
			VLOCATE(26,14);
			VPrintFNumber(songStep,0x20,3);	//3
			// -------------------------------------- test vars
			//VPrintNumber(14,13,PEEK(77,25),5); //num of pattern?        
			// -------------------------------------- END test vars
		}


		/*    else{        
			if (Player_IsEnd() && _loop==OFF)
			{
				_currentSong++;
				if(_currentSong>1) _currentSong=0; 
				PlaySong(_currentSong);            
			}   
		}*/

		ShowVumeter(0,AYREGS[AY_AmpA]);
		ShowVumeter(1,AYREGS[AY_AmpB]);
		ShowVumeter(2,AYREGS[AY_AmpC]);    


		// Keyboard row 6
/*		keyPressed = GetKeyMatrix(6);  
		if (keyPressed!=0xFF)  //pressure control of the keys
		{
			if(Row6pressed==false)
			{
				if (!(keyPressed&Bit5)){Row6pressed=true;PlaySong(0);} //F1 Key
				if (!(keyPressed&Bit6)){Row6pressed=true;PlaySong(1);} //F2 Key
				//if (!(keyPressed&Bit7)){keyB6pressStatus=true;PlaySong(3);} //F3 Key        
			}      
		}else Row6pressed=false;
*/   


		// Keyboard row 7
		keyPressed = GetKeyMatrix(7);
		if (keyPressed!=0xFF)
		{
			if(Row7pressed==false)
			{
				//if (!(keyPressed&Bit0)) {Row7pressed=true;}			//[F4]
				//if (!(keyPressed&Bit1)) {Row7pressed=true;}			//[F5]
				//if (!(keyPressed&Bit2)) {Row7pressed=true;}			//[ESC]
				if (!(keyPressed&Bit3)) {Row7pressed=true;SwapLoop();}	//[TAB]
				if (!(keyPressed&Bit4)) {Row7pressed=true;Pause();}		//[STOP]
				//if (!(keyPressed&Bit5)) {Row7pressed=true;}			//[BS]
				if (!(keyPressed&Bit6)) {Row7pressed=true;SelectAY(++_PSGtype);}			//[SELECT]
				if (!(keyPressed&Bit7)) {Row7pressed=true;PlaySong();}	//[RETURN]
			}      
		}else Row7pressed=false;


		Player_Decode();  //Process the next step in the song sequence

		ShowPlayback();
		ShowLoop();
		ShowENDsong();

	}

/*  
  uninstall_isr();
  
  CLS();
  PRINT("END");
  WAIT(30*5);
*/  
}



// Routine for Hook TIMI (FD9Fh)
void my_TIMI(void) 
{
	PUSH_AF;

	if (_PSGtype==3) Dump2AY(AY_EXTERNAL,(unsigned int) AYREGS);
	PlayAY();

	DumpOAM();	//Vumeter - dump to VRAM sprite attributes

	POP_AF;
}



void DumpOAM(void)
{
__asm
	ld   HL,#_SPRBUFFER
	ld   DE,#BASE13  
	ld   BC,#VUMETERSPRBUFF
	call BIOS_LDIRVM	;Copy a block from memory to VRAM
__endasm;
}



void SelectAY(char type)
{
	if(type==0 || type>3) type=1;
	_PSGtype = type;
	
	if(type&1)	AY_IOport = AY_INTERNAL;
	if(type==2) AY_IOport = AY_EXTERNAL;
	
	if(type==1)	SilenceAYbyPort(AY_EXTERNAL);
	if(type==2)	SilenceAYbyPort(AY_INTERNAL);
	
	VLOCATE(4,23);
	//VPrintFNumber(type,0x20,3);
	if(type&1) VPRINT(PSGtypeTiles[0]);
	if(type&2) VPRINT(PSGtypeTiles[1]);	
	if(type!=3) VPRINT("   ");
}





/*void SilenceAY(char AY_port) __naked
{
AY_port;		//A
__asm
	ld   E,A
	ld   D,#AY_AmpA
	xor  A
	ld   B,#3

AY_ampLOOP:
	ld   C,E	;set AY index port
	out  (C),D	;AY register
	inc  D
	inc  C		;go to AY write port
	out  (C),A	;write to AY reg
	djNZ AY_ampLOOP
	ret
__endasm;	
}*/


/*void Copy2secondAY(void)
{
__asm
    LD   HL,#_AYREGS
    LD   DE,#_AYREGS_plus
    LD   BC,#14
    LDIR
__endasm;
}*/




void Pause(void)
{
	if (PT3_state & PT3_END) return;	//(Player_IsEnd()==true)
	if (PT3_state & PT3_PLAY) Player_Pause();
	else Player_Resume();
}



void SwapLoop(void)
{
	_loop = !_loop;
	Player_Loop(_loop);
}



//void PlaySong(char songNumber)
void PlaySong(void)
{
//	Player_Pause();
	
	Player_InitSong((unsigned int) SONG01, (unsigned int) NT2, OFF);
	firstPATaddr = PT3_CrPsPtr;
}



void ShowPlayback(void)
{
	boolean SwitherStatus = false;
	uint vaddr;
	SwitherStatus = (PT3_state & PT3_PLAY);
	
	if (SwitherStatus==GUI_Playback) return;
	GUI_Playback = SwitherStatus;
	vaddr = BASE5 + (23*32) + 19;
	
	ShowState(vaddr,SwitherStatus);
	//else ShowState(vaddr,false);
}



void ShowLoop(void)
{   
//	uint vaddr = BASE5 + (23*32) + 25;
//	if (PT3_state & Bit4) ShowState(vaddr,true);
//	else ShowState(vaddr,false);
	
	boolean SwitherStatus = false;
	uint vaddr;
	SwitherStatus = (PT3_state & PT3_LOOP);
	
	if (SwitherStatus==GUI_Loop) return;
	GUI_Loop = SwitherStatus;
	
	vaddr = BASE5 + (23*32) + 25;	
	ShowState(vaddr,SwitherStatus);
}



void ShowENDsong(void)
{   
//	uint vaddr = BASE5 + (23*32) + 30;
//	if (Player_IsEnd()==true) ShowState(vaddr,true);
//	else ShowState(vaddr,false);
	
	boolean SwitherStatus = false;
	uint vaddr;
	
	SwitherStatus = (PT3_state & PT3_END);	//Player_IsEnd();
	
	if (SwitherStatus==GUI_End) return;
	GUI_End = SwitherStatus;
	
	vaddr = BASE5 + (23*32) + 30;	
	ShowState(vaddr,SwitherStatus);
}




/* =============================================================================
   WAIT
   Generates a pause in the execution of n interruptions.
   PAL: 50=1second. ; NTSC: 60=1second.
   Input    : [unsigned int] cicles 
   Output   : -
============================================================================= */ 
void WAIT(uint cicles)
{
  uint i;
  for(i=0;i<cicles;i++) HALT;
  return;
}



/* =============================================================================
  LOCATE
 
  Description: 
            Moves the cursor to the specified location.
  Input:    (char) Position X of the cursor. (0 to 31 or 79)
            (char) Position Y of the cursor. (0 to 23)         
  Output:   -
============================================================================= */
void LOCATE(char x, char y) __naked
{
x;	//A
y;	//L
__asm
	push IX

	inc  A       ;incrementa las posiciones para que se situen correctamente en la pantalla
	ld   H,A
	inc  L
	call BIOS_POSIT

	pop  IX
	ret
__endasm;
}




void ShowState(uint vaddr, boolean state)
{
    char tile;
/*    if (state==true) tile=157;
    else tile=189;
    VPOKE(vaddr++,tile++);
    VPOKE(vaddr++,tile++);
    VPOKE(vaddr,tile);*/
	
	if (state==true) tile=156;
    else tile=188;
	VPOKE(vaddr,tile);
}



void ShowVumeter(char channel, char value) __naked
{
channel;	//A
value;		//L
__asm

    
  ld   C,A
  ld   A,L


;C = num channel
;A = value  
;showVumeter:

	ld   (_VALUE),A

	SLA  C
	SLA  C

	ld	 B,#0
L00107:
	ld	 a,c
	ld	 l,a
	rla
	sbc	 a,a
	ld	 h,a
	add	 hl,hl
	add	 hl,hl

    ld   DE,#_SPRBUFFER
    ADD  HL,DE
    ex   DE,HL
  
	inc	 DE
	inc	 DE

	ld   A,(_VALUE)
	cp	 #0
	jr	 NZ,L00102
	xor  A
	ld	 (DE),A
	jr   L00105
  
L00102:                       
	ld   A,(_VALUE)
	cp   #4
	jr	 C,L00104
	ld	 A,#16
	ld	 (DE),A

	ld    A,(_VALUE)
	sub   #4
	ld   (_VALUE),A
	jr	 L00105
L00104:
	ld   A,(_VALUE)
	add	 a,a
	add	 a,a
	ld	(DE),A
	xor  A
	ld   (_VALUE),A
L00105:
	inc	 C
	inc	 B
	ld	 A,B
    cp   #4
	jr	 C,L00107

	ret
  
__endasm;
}



void InitVumeter(void) __naked
{
__asm

  push IX
  
  ld   HL,#SPRITE_DATA
  ld   DE,#BASE14  
  ld   BC,#32*5
  call 0x005C

  ld   DE,#_SPRBUFFER
  ld   HL,#VUMETER
  ld   BC,#VUMETERSPRBUFF
  ldir
  
  pop  IX
  ret
  

SPRITE_DATA:
; 0-vum0
.db 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
.db 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
; 1-vum1
.db 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7F,0x7F,0x7F,0x00
.db 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFE,0xFE,0xFE,0x00
; 2-vum2
.db 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7F,0x7F,0x7F,0x00,0x7F,0x7F,0x7F,0x00
.db 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFE,0xFE,0xFE,0x00,0xFE,0xFE,0xFE,0x00
; 3-vum3
.db 0x00,0x00,0x00,0x00,0x7F,0x7F,0x7F,0x00,0x7F,0x7F,0x7F,0x00,0x7F,0x7F,0x7F,0x00
.db 0x00,0x00,0x00,0x00,0xFE,0xFE,0xFE,0x00,0xFE,0xFE,0xFE,0x00,0xFE,0xFE,0xFE,0x00
; 4-vum4
.db 0x7F,0x7F,0x7F,0x00,0x7F,0x7F,0x7F,0x00,0x7F,0x7F,0x7F,0x00,0x7F,0x7F,0x7F,0x00
.db 0xFE,0xFE,0xFE,0x00,0xFE,0xFE,0xFE,0x00,0xFE,0xFE,0xFE,0x00,0xFE,0xFE,0xFE,0x00



;SPRITE ATRIBUTE DATAS #########################################################
; for adjust colors, changue the last valor in line


;Y,X,SPR,COLOR
VUMETER:
.db 159,8,0,2
.db 143,8,0,2
.db 127,8,0,10
.db 111,8,0,8

.db 159,24,0,2
.db 143,24,0,2
.db 127,24,0,10             
.db 111,24,0,8 

.db 159,40,0,2
.db 143,40,0,2
.db 127,40,0,10
.db 111,40,0,8
;END SPRITE ATRIBUTE DATAS #####################################################
__endasm;
}
