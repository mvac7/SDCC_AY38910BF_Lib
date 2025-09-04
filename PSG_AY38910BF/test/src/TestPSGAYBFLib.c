/*  =============================================================================
# TestAY38910BF

- Version: 1.0 (18/08/2025)
- Author: mvac7/303bcn
- Architecture: MSX
- Format: 8K ROM
- Programming language: C and Z80 assembler
- Compiler: SDCC 4.4

## Description:
	Test fR3eL's PSG_AY38910BF library 

## History of versions: (dd/mm/yyyy)
- v1.0 (18/08/2025) first version
============================================================================= */
#include "../include/newTypes.h"
#include "../include/msxSystemVariables.h"
#include "../include/msxBIOS.h"

#include "../include/textmode_MSX.h"
#include "../include/PSG_AY38910BF.h"



// ---------------------------------------------------------------------------- Labels
#define  HALT __asm halt __endasm   //wait for the next interrupt

#define T1_MAP	0x0000 // Name Table Text1
#define G1_MAP	0x1800 // Name Table GRAPHIC1

#ifndef  __BITVALUES__
#define  __BITVALUES__
#define Bit0 0b00000001
#define Bit1 2
#define Bit2 4
#define Bit3 8
#define Bit4 16
#define Bit5 32
#define Bit6 64
#define Bit7 128
#endif




// ---------------------------------------------------------------------------- Function Declaration
char PEEK(unsigned int address);

char VPEEK(unsigned int vaddr);

char INKEY(void);

void WAIT(unsigned int cicles);

void test_AYLIB_page1(void);
void test_AYLIB_page2(void);
void test_AYLIB_page3(void);
void test_AYLIB_page4(void);
void test_AYLIB_page5(void);
void test_AYLIB_page6(void);

void SetFullSound(void);
void TestGetSound(void);

void ShowDefAYregs(void);

unsigned int GetPeriod(char fine, char coarse);

void PrintHeader(void);

void PrintBinary(char value);
void PrintBit(char value);

void PressAnyKey(void);

void PrintLine(char size);

void SetG1colors(char octet, char INKcolor,char BGcolor);

boolean isTxtMode(void);




// ---------------------------------------------------------------------------- Constants
const char text01[] = "Test PSG_AY38910BF Lib    Page:";

const char msg_presskey[] = "Press any key to continue";

const char CheckResult[2][8] = {"=ERROR!","=Ok    "};



// ---------------------------------------------------------------------------- Global Variables
char scr_page=0;



// ---------------------------------------------------------------------------- Definition of functions


void main(void)
{
	COLOR(WHITE,DARK_BLUE,BLACK);
	WIDTH(32);
	SCREEN1();
	
	SetG1colors(2,GRAY,DARK_BLUE);
	SetG1colors(3,GRAY,DARK_BLUE);
	
	scr_page=1;
  
	test_AYLIB_page1();
	test_AYLIB_page2();
	test_AYLIB_page3();
	test_AYLIB_page4();
	test_AYLIB_page5();
	test_AYLIB_page6();

	CLS();
	PRINT("END");
	WAIT(200);
}



/* =============================================================================
PEEK
Function:		PEEK(address)
Description:	Read a 8 bit value from the memory.
Input:			[unsigned int] memory address
Output:			[char] value
============================================================================= */
char PEEK(unsigned int address) __naked
{
address;
__asm
	ld   A,(HL)
	ret
__endasm;
}



/* =============================================================================
VPEEK
Function:		VPEEK(vaddr)
Description:	Reads a value from video RAM. 
Input:			[unsigned int] VRAM address
Output:			[char] value
============================================================================= */ 
char VPEEK(unsigned int vaddr) __naked
{
vaddr;	//HL
__asm
	jp BIOS_RDVRM
__endasm;
}



/* =============================================================================
INKEY
Function:		INKEY()
Description:	Waits for a key press and returns its value
Input:			-
Output:			[char] key code
============================================================================= */
char INKEY(void) __naked
{
__asm   
	jp  BIOS_CHGET
__endasm;
}



/* =============================================================================
WAIT
Function:		WAIT(cicles)
Description:	Generates a pause in the execution of n interruptions.
Input:			[unsigned int]  cicles number (VBLANKs)
				(Note: PAL: 50=1second. ; NTSC: 60=1second.)
============================================================================= */
void WAIT(unsigned int cicles)
{
	unsigned int i;
	for(i=0;i<cicles;i++) HALT;
}



void test_AYLIB_page1(void)
{
	PrintHeader();
	
	PrintLN(">InitInternalAY()");
	PrintLN(" Set default AY (internal) and\n AYREGS buffer");
	InitInternalAY();    								//Init library. Set default AY (internal) 
	
	WAIT(100);
	
	PrintLN(">Write buffer with SOUND()");
	SetFullSound();
	WAIT(100);
	
	PrintLN(">Test GetSound()");
	WAIT(50);
	TestGetSound();
	
	WAIT(100);
	
	PrintLN(">PlayAY()");
	PlayAY();
	
	PressAnyKey();
}



void test_AYLIB_page2(void)
{
	PrintHeader();
	
	PrintLN(">ClearAY()");
	ClearAY();
	WAIT(50);
	ShowDefAYregs();
	WAIT(100);
	PrintLN(">PlayAY()");
	PlayAY();

	PressAnyKey();
}



void test_AYLIB_page3(void)
{	
	PrintHeader();
	
	PrintLN("Test on an external AY.\n");
	PrintLN("The next part of the test\nrequires a second AY-3-8910 or\ncompatible.\n"); 
	PrintLN("You can find an external AY in\nthe cartridges:\nMEGAFLASHROM SCC+, Flashjacks,\nYamanooto and Carnivore2.\n");
	
	PRINT(msg_presskey);
	INKEY();
	
	PRINT("\r");
	PrintLine(32);
	
	PrintLN(">Select external AY");
	PrintLN(" SelectAY(AY_EXTERNAL)");
	SelectAY(AY_EXTERNAL);
	//PrintLN(" AY_defIOport=AY_EXTERNAL");
	//AY_defIOport = AY_EXTERNAL;
	WAIT(100);
	
	PrintLN(">Write buffer with SOUND()");
	SetFullSound();
	WAIT(100);
		
	PrintLN(">PlayAY()");
	PlayAY();
	
	PressAnyKey();
	
	ClearAY();
	PlayAY();
}



void test_AYLIB_page4(void)
{
	PrintHeader();
	
	PrintLN("Test 2 AY at a time."); 
	
	PrintLN("\n>Select internal AY");
	PrintLN(" SelectAY(AY_INTERNAL)");
	SelectAY(AY_INTERNAL);
//	PrintLN(" AY_defIOport=AY_INTERNAL");
//	AY_defIOport = AY_INTERNAL;	
	WAIT(100);
	
	PrintLN(">Write buffer with SOUND()");
	SetFullSound();
	WAIT(100);
	
	PRINT(">Play external AY with Dump2AY()");
	WAIT(50);
	Dump2AY(AY_EXTERNAL, AY_defAYREGs_addr);
	WAIT(200);
		
	PrintLN(">Play internal AY with PlayAY()");
	PlayAY();
	
	WAIT(200);
	
	PressAnyKey();
}



void test_AYLIB_page5(void)
{
	PrintHeader();
	
	PrintLN("Test Silence AYs"); 
	
	PrintLN("\n>Silence external AY");
	WAIT(50);
	PrintLN("\n SilenceAYbuffer(AY_EXTERNAL,\n AY_defAYREGs_addr)\n");
	SilenceAYbuffer(AY_EXTERNAL,AY_defAYREGs_addr);
	ShowDefAYregs();	
	
	PressAnyKey();
}



void test_AYLIB_page6(void)
{
	PrintHeader();
	
	PrintLN("Test Silence AYs"); 
	
	PrintLN("\n>Silence Internal");
	WAIT(50);
	PrintLN("\n SilenceAY()\n");
	SilenceAY();
	ShowDefAYregs();
//	WAIT(100);
	
	PressAnyKey();
}





void SetFullSound(void)
{
	unsigned int time = 10*50;					//10 seconds in PAL
	unsigned int toneC1 = 0x06AE;				//C note/octave 1
	unsigned int toneD2 = 0x02FA;				//D note/octave 2
	unsigned int toneE3 = 0x0153;				//E note/octave 3
	unsigned int envPeriod = 700;
	
//write to the buffer
	SOUND(AY_ToneA_fine,toneC1&0xFF);			//Set channel A fine tune period (8b)
	SOUND(AY_ToneA_coarse,toneC1>>8);			//Set channel A coarse tune period (4b)
	SOUND(AY_ToneB_fine,toneD2&0xFF);			//Set channel B fine tune period (8b)
	SOUND(AY_ToneB_coarse,toneD2>>8);			//Set channel B coarse tune period (4b)
	SOUND(AY_ToneC_fine,toneE3&0xFF);			//Set channel C fine tune period (8b)
	SOUND(AY_ToneC_coarse,toneE3>>8);			//Set channel C coarse tune period (4b)
	SOUND(AY_Noise,20);							//Set noise period
	SOUND(AY_AmpA,12);							//Set volume on channel A
	SOUND(AY_AmpB,14);							//Set volume on channel B
	SOUND(AY_AmpC,16);							//Enable envelope on channel C
	SOUND(AY_Mixer,0B00110000);					//Enable Tone and Noise on channel A and Tone on channel B and C
	SOUND(AY_EnvPeriod_fine,envPeriod&0xFF);	//Set fine tune envelope period (8b)
	SOUND(AY_EnvPeriod_coarse,envPeriod>>8);	//Set coarse tune envelope period (8b)
	SOUND(AY_EnvShape,AY_ENV_UpperTriangle);	//Set envelope shape	
}



void TestGetSound(void)
{
	ShowDefAYregs();
}



void ShowDefAYregs(void)
{
	char fine;
	char coarse;
	uint value16;
	
	fine=GetSound(AY_ToneA_fine);
	coarse=GetSound(AY_ToneA_coarse);
	value16=GetPeriod(fine,coarse);
	PRINT(" Tone channel A:   ");
	PrintNumber(value16);
	
	fine=GetSound(AY_ToneB_fine);
	coarse=GetSound(AY_ToneB_coarse);
	value16=GetPeriod(fine,coarse);
	PRINT("\n Tone channel B:   ");
	PrintNumber(value16);
	
	fine=GetSound(AY_ToneC_fine);
	coarse=GetSound(AY_ToneC_coarse);
	value16=GetPeriod(fine,coarse);
	PRINT("\n Tone channel C:   ");
	PrintNumber(value16);
	
	fine=GetSound(AY_Noise);
	PRINT("\n Noise period:     ");
	PrintNumber(fine);
	
	fine=GetSound(AY_AmpA);
	PRINT("\n Channel A Volume: ");
	PrintNumber(fine);
	
	fine=GetSound(AY_AmpB);
	PRINT("\n Channel B Volume: ");
	PrintNumber(fine);
	
	fine=GetSound(AY_AmpC);
	PRINT("\n Channel C Volume: ");
	PrintNumber(fine);
	
	//Mixer
	fine=GetSound(AY_Mixer);
	PRINT("\n Mixer:            ");
	PrintBinary(fine);
	
	fine=GetSound(AY_EnvPeriod_fine);
	coarse=GetSound(AY_EnvPeriod_coarse);
	value16=GetPeriod(fine,coarse);
	PRINT("\n Envelope period:  ");
	PrintNumber(value16);
	
	fine=GetSound(AY_EnvShape);
	PRINT("\n Envelope shape:   ");
	PrintNumber(fine);
	
	PrintLN("");
}



unsigned int GetPeriod(char fine, char coarse)
{
	unsigned int result;
	result = (coarse<<8) + fine;
	return result;	
}



void PrintHeader(void)
{
	CLS();
	PRINT(text01);
	PrintFNumber(scr_page++,0,1);
	PrintLine(32);
}



void PrintBinary(char value)
{
	PrintBit(value&Bit7);
	PrintBit(value&Bit6);
	PrintBit(value&Bit5);
	PrintBit(value&Bit4);
	PrintBit(value&Bit3);
	PrintBit(value&Bit2);
	PrintBit(value&Bit1);
	PrintBit(value&Bit0);
}



void PrintBit(char value)
{
	if(value) PRINT("1");
	else PRINT("0");
}





/* =============================================================================
   Print a message on line 24 and wait for a key press.
============================================================================= */
void PressAnyKey(void)
{	
	LOCATE(0,23);
	PRINT(msg_presskey);
	INKEY();	
}





/* =============================================================================
   Print a line.
============================================================================= */
void PrintLine(char size)
{
	char i;	
	for(i=0;i<size;i++) bchput('-');	
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