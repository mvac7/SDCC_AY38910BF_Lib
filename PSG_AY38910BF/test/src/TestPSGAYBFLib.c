/*  =============================================================================
# TestAY38910BF

- Version: 0.9 (18/08/2025)
- Author: mvac7/303bcn
- Architecture: MSX
- Format: 8K ROM
- Programming language: C and Z80 assembler
- Compiler: SDCC 4.4

## Description:
	Test fR3eL's PSG_AY38910BF library 


## History of versions: (dd/mm/yyyy)
- v1.0 (???)
============================================================================= */
#include "../include/newTypes.h"
#include "../include/msxSystemVariables.h"
#include "../include/msxBIOS.h"

#include "../include/textmode_MSX.h"



// ---------------------------------------------------------------------------- Labels
#define  HALT __asm halt __endasm   //wait for the next interrupt

#define T1_MAP	0x0000 // Name Table Text1
#define G1_MAP	0x1800 // Name Table GRAPHIC1



// ---------------------------------------------------------------------------- Function Declaration
char PEEK(uint address);

char VPEEK(uint address);

char INKEY(void);

void WAIT(unsigned int cicles);

void test_SC0(void);
void test_SC1(void);

void testWIDTH(void);
void testPRINT(void);
void testPrintNumber(void);
void testCLS(void);

void PressAnyKey(void);

void PrintExtendedGFXchar(char A);
void PrintLine(char size);
void DrawBox(char width, char height);

void SetG1colors(char octet, char INKcolor,char BGcolor);

boolean isTxtMode(void);




// ---------------------------------------------------------------------------- Constants
const char text01[] = "Test textmode_MSXBIOS Lib";
const char text10[] = ">Test CLS()";

const char text_32col[] = "----5----1----1----2----2----3--         0    5    0    5    0  ";
const char text_40col[] = "----5----1----1----2----2----3----3----4         0    5    0    5    0    5    0";

const char text_LF[] = "\n"; // LF line Feed
const char text_CR[] = "\r"; // CR Carriage Return

const char testString[] = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.";

const char presskey[] = "Press any key to continue";

const char CheckResult[2][8] = {"=ERROR!","=Ok    "};



// ---------------------------------------------------------------------------- Global Variables




// ---------------------------------------------------------------------------- Definition of functions


void main(void)
{
	COLOR(WHITE,DARK_BLUE,BLACK);
	WIDTH(32);
	SCREEN1();
	
	SetG1colors(2,GRAY,DARK_BLUE);
	SetG1colors(3,GRAY,DARK_BLUE);

	LOCATE(2,10);
	DrawBox(28, 3);
	LOCATE(3,11);
	PRINT(text01);
	
	//LOCATE(7,12);
	//PRINT(text02);

	PressAnyKey(); 
	  
	test_AYLIB();

	CLS();
	PRINT("END");
	PressAnyKey();  
}



/* =============================================================================
PEEK
Description: 
		Read a 8 bit value from the memory.
Input:	[unsigned int] memory address
Output:	[char] value
============================================================================= */
char PEEK(uint address) __naked
{
address;
__asm
	ld   A,(HL)
	ret
__endasm;
}



/* =============================================================================
VPEEK
Description:	Reads a value from video RAM. 
Input:			[unsigned int] VRAM address
Output:			[char] value
============================================================================= */ 
char VPEEK(uint address) __naked
{
address;
__asm
	jp BIOS_RDVRM
__endasm;
}



/* =============================================================================
INKEY
Description: 
		Waits for a key press and returns its value
Input:	-
Output:	[char] key code
============================================================================= */
char INKEY(void) __naked
{
__asm   
	jp  BIOS_CHGET
__endasm;
}



/* =============================================================================
WAIT
Description:	Generates a pause in the execution of n interruptions.
Input:			[unsigned int]  cicles number (VBLANKs)
				(Note: PAL: 50=1second. ; NTSC: 60=1second.)
============================================================================= */
void WAIT(unsigned int cicles)
{
	unsigned int i;
	for(i=0;i<cicles;i++) HALT;
}






void test_AYLIB(void)
{
	
	unsigned int time = 10*50;					//10 seconds in PAL
	unsigned int tone = 1000;
	unsigned int envPeriod = 700;
	
	InitAY();    								/* Init library. Set default AY (internal) 
												   and clear Buffer */
	
	SOUND(AY_ToneA_fine,tone&0xFF);				//Set channel A fine tune period (8b)
	SOUND(AY_ToneA_coarse,tone>>8);				//Set channel A coarse tune period (4b)
	SOUND(AY_Noise,20);							//Set noise period
	SOUND(AY_AmpA,16);							//Enable envelope
	SOUND(AY_Mixer,0B00110110);					//Enable Tone and Noise in channel A
	SOUND(AY_EnvPeriod_fine,envPeriod&0xFF);	//Set fine tune envelope period (8b)
	SOUND(AY_EnvPeriod_coarse,envPeriod>>8);	//Set coarse tune envelope period (8b)
	SOUND(AY_EnvShape,AY_ENV_UpperTriangle);	//Set envelope shape
	
	PlayAY();									//Dump AY buffer to PSG (the sound is played)
	
	while(time-->0) HALT;						//wait 10secs.
	
	

	PressAnyKey();
}





/* =============================================================================
   Print a message on line 24 and wait for a key press.
============================================================================= */
void PressAnyKey(void)
{	
	LOCATE(0,23);
	PRINT(presskey);
	INKEY();	
}



/* =============================================================================
   Print an Extended Graphic char
============================================================================= */
void PrintExtendedGFXchar(char A)
{
	bchput(1);
	bchput(A);
}



/* =============================================================================
   Print a line.
============================================================================= */
void PrintLine(char size)
{
	char i;
	
	for(i=0;i<size;i++) PrintExtendedGFXchar(0x57);	
}



/* =============================================================================
   Prints a box from the cursor position.
============================================================================= */
void DrawBox(char width, char height)
{
	char i;
	char box_winside = width-2;
	
	char x = GetCursorColumn();
	char y = GetCursorRow();
	
/*  PrintLN("\1\x58\1\x57\1\x57\1\x59");
	PrintLN("\1\x56  \1\x56");
	PrintLN("\1\x5A\1\x57\1\x57\1\x5B");*/
	
	PrintExtendedGFXchar(0x58);
	PrintLine(box_winside);
	PrintExtendedGFXchar(0x59);
	
	width--;
	
	for(i=y+1;i<y+(height-1);i++)
	{
		LOCATE(x,i);
		PrintExtendedGFXchar(0x56);
		LOCATE(x+width,i);
		PrintExtendedGFXchar(0x56);
	}
	
	LOCATE(x,y+height-1);
	PrintExtendedGFXchar(0x5A);
	PrintLine(box_winside);
	PrintExtendedGFXchar(0x5B);	
}



/* =============================================================================
SetG1colors
Description: 
		Assigns colors to a group of GRAPHIC1 tiles.
		ROM/MSX-BASIC environment
	   
Input:	(char) Octet. Group of 8 tiles.
		(char) Ink color (0-15)
		(char) Background color (0-15)      
Output:	-
============================================================================= */
void SetG1colors(char octet, char INKcolor, char BGcolor)
{
octet;		//A
INKcolor;	//L
BGcolor;	//Stack	
__asm
	push IX
	ld   IX,#0
	add  IX,SP

	ld   B,L	

	ld   HL,#0x2000
	ld   D,#0
	ld   E,A
	add  HL,DE

	ld   A,B
	add  A
	add  A
	add  A
	add  A	
	or   4(IX)	

	call  0x004D	//MSX BIOS WRTVRM - Writes data in VRAM

	pop  IX
__endasm;	
}



/* =============================================================================
isTxtMode
Description:	Indicates whether Text 1 mode is active.
Input:			-
Output:			[char] or deftype [boolean] --> ]1=Yes/True ; 0=No/False
============================================================================= */
boolean isTxtMode(void)
{
	char VDP1 = *(unsigned int *) 0xF3E0;	//RG1SAV=0xF3E0 (System var)
	return VDP1 & 0b00010000;		
}