/* =============================================================================
# TestAY

- Version: 1.2 (20/08/2025)
- Author: mvac7/303bcn
- Architecture: MSX
- Format: 16K ROM (BIOS+ROM+RAM+RAM)
- Programming language: C and Z80 assembler
- Compiler: SDCC 4.4

## Description:
Test PSG_AY38910BF and PSG_AY38910BF_eXtended MSX Library (fR3eL Project)

This library is based on writing in a buffer the values of the AY 
registers and copying them in each VBLANK interrupt.
   
## History of versions (dd/mm/yyyy):
- v1.2 (20/08/2025) Added PSG_AY38910BF_eXtended functions 
- v1.1 (07/02/2025) update to SDCC (4.1.12) Z80 calling conventions
- v1.0 (07/07/2021) first version
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
#include "../include/PSG_AY38910BF_eXtended.h"

#include "../include/Test_GFX.h"


// ---------------------------------------------------------------------------- Labels
#define PAUSE_TIME 30


#define C_Octave1 0x06AE


// ---------------------------------------------------------------------------- Declaration of functions
void my_TIMI(void);

void WAIT(unsigned int cicles);

void LOCATE(char x, char y) ;

void VPrintNumberO(unsigned int vaddr, unsigned int value, char length, signed char offset);

void Set_tone(char channel, uint period);
void Set_Amp(char channel, char value);
void Set_noise(char period);
void Set_mixer(char value);
void Set_EnvPeriod(uint period);
void Set_EnvShape(char shape);

void ShowMixer(void);
void PrintSwitcher(char col, char lin, boolean state);
void ShowAYregs(void);
void ShowTone(char channel);
void ShowAmp(char channel, char value);
void ShowEnvelopeShape(char shape);

void test1(void); // test SOUND
void test2(void); // test SetTonePeriod
void test3(void); // test SetNoisePeriod
void test4(void); // test SetVolume
void test5(void); // test SetEnvelopePeriod
void test6(void); // test PlayEnvelope and SetChannel

void test_Tone(char channel, uint vaddr);
void test_Noise(void);
void test_Volume(char channel, uint vaddr);



// ---------------------------------------------------------------------------- Constants
const char text01[] = "Test PSG_AY38910BF and ";
const char text02[] = "     PSG_AY38910BF_eXtended";

const char text03[] = "                             ";

//const char EnvelopeValue[8]={1,4,8,10,11,12,13,14};
const char EnvelopeIndex[16]={0,0,0,0,1,1,1,1,2,0,3,4,5,6,7,1};



// ---------------------------------------------------------------------------- Global Variables



// ---------------------------------------------------------------------------- Definition of functions

//
void main(void)
{
	char result;
	char AYtype = 0;

	InitAY();    //Clear AY Buffer

	POKE(LINL32,32); //Width(32)
	COLOR(BLACK,GRAY,DARK_BLUE);      
	SCREEN(1);

	unWRLE2toVRAM16((uint) Frame_MAP,BASE5);
	unWRLE2toVRAM16((uint) GUI_G1_TSET_COL,BASE6);
	unWRLE2toVRAM16((uint) GUI_G1_TSET_PAT,BASE7);

	VLOCATE(1,1);
	VPRINT(text01);
	VLOCATE(1,2);
	VPRINT(text02);

	VLOCATE(1,8);
	VPRINT("Do you want to use the");
	VLOCATE(1,9);
	VPRINT("external AY? ([Y] for Yes)");

	LOCATE(1,10);
	result = INKEY();

	if (result==89 || result==121){
		AY_IOport=AY_EXTERNAL;
		AYtype++;
	}else	AY_IOport=AY_INTERNAL;

	unWRLE2toVRAM16((uint) GUI_MAP,BASE5+96); //show screen map
	
	//PRINTON(0x1A6D,AYtype);
	PrintSwitcher(13,19,AYtype);
  
	Install_TIMI(my_TIMI);  //install interruption function

	test1(); // test SOUND
	test2(); // test SetTonePeriod
	test3(); // test SetNoisePeriod
	test4(); // test SetVolume
	test5(); // test SetEnvelopePeriod
	test6(); // test PlayEnvelope and SetChannel

	Disable_TIMI();

	VLOCATE(1,21);
	VPRINT(">End Test                     ");
	WAIT(250);

	unWRLE2toVRAM16((uint) Frame_MAP,BASE5);

	WAIT(100);

}



// Routine for Hook TIMI (FD9Fh)
void my_TIMI(void) 
{
	PUSH_AF;

	PlayAY(); //Dump buffer to AY

	POP_AF;
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



/* =============================================================================
LOCATE
Function:		LOCATE(x, y)
Description:	Moves the cursor to the specified location.
Input:			[char] Position X of the cursor. (0 to 31 or 79)
				[char] Position Y of the cursor. (0 to 23)         
Output:   		-
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



/* =============================================================================
VPrintNumberO
Function:		VPrintNumberO(vaddr, value, length, offset)
Description:	Prints a number at the specified position on the screen.

				Apply an offset to the tile number to be able to use a font  
				hosted in another area of the tileset.

Inputs:			[unsigned int] VRAM address in Pattern Name Table.
				[unsigned int] number
				[char] length
============================================================================= */
void VPrintNumberO(unsigned int vaddr, unsigned int value, char length, signed char offset)
{
	char pos=0; //5-length;
	//char text[]="     ";

	num2Dec16(value, 0x2000+length); 

	while (length-->0){ VPOKE(vaddr++,VPRINT_ntext[pos++]+offset);}
}



// test SOUND
void test1(void)
{
	char value;
	uint period;
	
	VLOCATE(1,21);
	VPRINT(">Test SOUND function");  
	VLOCATE(1,22);
	VPRINT(" Press a key to stop sound");

	period=0x0D5D;
	SOUND(AY_ToneA_fine,period&0xFF);
	SOUND(AY_ToneA_coarse,(period&0xFF00)>>8);
	
	period=C_Octave1;
	SOUND(AY_ToneB_fine,period&0xFF);
	SOUND(AY_ToneB_coarse,(period&0xFF00)>>8);
	
	period=0x0357;
	SOUND(AY_ToneC_fine,period&0xFF);
	SOUND(AY_ToneC_coarse,(period&0xFF00)>>8);
	
/*	Set_tone(AY_Channel_A,0x0D5D);
	Set_tone(AY_Channel_B,C_Octave1);
	Set_tone(AY_Channel_C,0x0357);*/

	//Set_noise(20);
	SOUND(AY_Noise,20);

/*	Set_Amp(AY_Channel_A,16);
	Set_Amp(AY_Channel_B,16);
	Set_Amp(AY_Channel_C,16);*/
	
	SOUND(AY_AmpA,16);	//enable envelope in A channel
	SOUND(AY_AmpB,16);
	SOUND(AY_AmpC,16);

/*	SetChannel(AY_Channel_A,ON,ON);
	SetChannel(AY_Channel_B,ON,OFF);
	SetChannel(AY_Channel_C,ON,OFF);*/
	SOUND(AY_Mixer,0b00110000); //Mixer

//	Set_EnvPeriod(1000);
	period=1000;
	SOUND(AY_EnvPeriod_fine,period&0xFF);
	SOUND(AY_EnvPeriod_coarse,(period&0xFF00)>>8);

//	Set_EnvShape(14);
	ShowEnvelopeShape(14);
	SOUND(AY_EnvShape,14);
	
	ShowAYregs();

	LOCATE(27,22);
	INKEY();

	ClearDefAYbuffer();
	ShowAYregs();
	value=GetSound(AY_EnvShape);
	ShowEnvelopeShape(value);

	WAIT(PAUSE_TIME);
}



// test SetTonePeriod
void test2(void)
{
	SOUND(AY_Mixer,0b00111111); //Mixer · All channels OFF

	VLOCATE(1,21);  
	VPRINT(">Test SetTonePeriod function  ");  
	VLOCATE(1,22);
	VPRINT(" Press [SPACE] to next        ");

	SetChannel(AY_Channel_A,ON,OFF);
	ShowMixer();
	test_Tone(AY_Channel_A,0x18E6);

	WAIT(PAUSE_TIME);

	SetChannel(AY_Channel_A,OFF,OFF);
	SetChannel(AY_Channel_B,ON ,OFF);
	ShowMixer();
	test_Tone(AY_Channel_B,0x18F0);
	
	WAIT(PAUSE_TIME);

	SetChannel(AY_Channel_B,OFF,OFF);
	SetChannel(AY_Channel_C,ON ,OFF);
	ShowMixer();
	test_Tone(AY_Channel_C,0x18FB);

	WAIT(PAUSE_TIME);  
}



void test_Tone(char channel, uint vaddr)
{
	uint period=0;
	
	Set_Amp(channel,15);
	while(period<4096)
	{
		HALT;

		if (GetKeyMatrix(8)!=255) break;

		VPrintNumberO(vaddr,period,4,144); //27,7

		SetTonePeriod(channel, period++);
		//if (period>4094) period=0;
	}
	Set_Amp(channel,0);	
}



// Test SetNoisePeriod
void test3(void)
{
	char noise=0;
	
	SOUND(AY_Mixer,0b00111111); //Mixer channels OFF

	VLOCATE(1,21);
	VPRINT(">Test SetNoisePeriod function ");
	VLOCATE(1,22);
	VPRINT(text03);

	//Set_mixerSet_mixer(0B00110111); //Mixer
	SetChannel(AY_Channel_A,OFF,ON);
	ShowMixer();
	Set_Amp(AY_Channel_A,15);
	test_Noise();

	//Set_mixer(0B00100111); //Mixer
	SetChannel(AY_Channel_B,OFF,ON);
	ShowMixer();
	Set_Amp(AY_Channel_B,15);
	test_Noise();

	//Set_mixer(0B00000111); //Mixer
	SetChannel(AY_Channel_C,OFF,ON);
	ShowMixer();
	Set_Amp(AY_Channel_C,15);
	test_Noise();

	WAIT(PAUSE_TIME);

	Set_Amp(AY_Channel_A,0);
	Set_Amp(AY_Channel_B,0);
	Set_Amp(AY_Channel_C,0);
}



void test_Noise(void)
{
    char i;
    
    for (i=0;i<32;i++)
    {
        //if (GetKeyMatrix(8)!=255) break;
    
        VPrintNumberO(0x19CF,i,2,144); // 15,14
    
        SetNoisePeriod(i);
        
        WAIT(10);    
    }
}



// Test SetVolume
void test4(void)
{
	SOUND(AY_Mixer,0b00111111); //Mixer channels OFF

	VLOCATE(1,21);
	VPRINT(">Test SetVolume function     ");
	VLOCATE(1,22);
	VPRINT(text03);

	Set_noise(10);

	test_Volume(AY_Channel_A,0x1925);

	WAIT(PAUSE_TIME);

	SetChannel(AY_Channel_A,OFF,OFF);
	test_Volume(AY_Channel_B,0x192F);

	WAIT(PAUSE_TIME);

	SetChannel(AY_Channel_B,OFF,OFF);
	test_Volume(AY_Channel_C,0x1939);
	
	WAIT(PAUSE_TIME);    
}



void test_Volume(char channel, uint vaddr)
{
	char i;
	
	Set_tone(channel,C_Octave1);
	SetChannel(channel,ON,ON);
	ShowMixer();
	
	for(i=0;i<16;i++)
	{  
		VPrintNumberO(vaddr,i,2,144); //25,9
		SetVolume(channel,i);
		WAIT(10);
	}  
	SetVolume(channel,0);
}



// Test SetEnvelopePeriod
void test5(void)
{
	uint period=0;
	
	SOUND(AY_Mixer,0b00111111);		//Mixer channels OFF

	VLOCATE(1,21);
	VPRINT(">Test SetEnvelopePeriod      ");
	VLOCATE(1,22);
	VPRINT(" Press [SPACE] to end        ");

	//LOCATE(0,8);
	//PRINT(" Env Period:");

	Set_tone(AY_Channel_A,C_Octave1);
	//Set_mixer(0B00111110); //Mixer
	SetChannel(AY_Channel_A,ON,OFF);
	ShowMixer();
	Set_Amp(AY_Channel_A,16);
	Set_EnvShape(14);

	while(1)
	{
		HALT;
		VPrintNumberO(0x1A10,period,5,144); //16,16

		SetEnvelopePeriod(period++);		
		
		if(period==0) break;
		if (!(GetKeyMatrix(8)&Bit0)) break;
	}

	Set_Amp(AY_Channel_A,0);

	WAIT(PAUSE_TIME);  
}



// Test PlayEnvelope and SetChannel
void test6(void)
{  
	char i;
	
	SOUND(AY_Mixer,0b00111111);		//Mixer channels OFF

	VLOCATE(1,21);
	VPRINT(">Test SetEnvelope           ");
	VLOCATE(1,22);
	VPRINT(text03);

	Set_tone(AY_Channel_A,C_Octave1);
	SetChannel(AY_Channel_A,ON,OFF);
	ShowMixer();
	//Set_mixer(0B00111110); //Mixer
	Set_Amp(AY_Channel_A,16); //Enable sound envelope
	Set_EnvPeriod(1024);

	LOCATE(26,5);

	for(i=0;i<16;i++)
	{
		Set_EnvShape(i);    
		WAIT(100);
	}

	Set_Amp(AY_Channel_A,0);
	
	SetChannel(AY_Channel_A,OFF,OFF);
	ShowMixer();

	WAIT(PAUSE_TIME);
}



/*void PLAY_EnvShape(char shape)
{
	char index;

	//shape = EnvelopeValue[index];
	index = EnvelopeIndex[shape]; 

	SetEnvelope(shape);

	VPrintNumberO(0x1A30,shape,2,+144); //16,17  
	CopyToVRAM((uint) EnvelopeShapes + (index*3), 0x1A33, 3);  // 19,17
}*/



void Set_tone(char channel, uint period)
{
	uint vaddr=BASE5 + 6+(channel*10) + (7*32); //calculates the position of the numeric field
	if (channel==2) vaddr++;  // correct the position of the third column

	VPrintNumberO(vaddr,period,4,144);  //6+(channel*10),10

	SetTonePeriod(channel,period);
/*	channel=channel*2;
	SOUND(channel++,period & 0xFF);
	SOUND(channel,(period & 0xFF00)/0xFF);*/
}



void Set_Amp(char channel, char value)
{
	SetVolume(channel,value);
	ShowAmp(channel,value);
}



void Set_noise(char period)
{
    SetNoisePeriod(period);
    //SOUND(AY_Noise,period);
	
	VPrintNumberO(0x19CF,period,2,144); //15,14
}



void Set_EnvPeriod(uint period)
{
	SetEnvelopePeriod(period);	
	//SOUND(11,period & 0xFF);
	//SOUND(12,(period & 0xFF00)/0xFF);

	VPrintNumberO(0x1A10,period,5,144); //16,16
}



void Set_EnvShape(char shape)
{
	SetEnvelope(shape);			//envelope shape type
	ShowEnvelopeShape(shape);
}



void Set_mixer(char value)
{
    SOUND(AY_Mixer,value);
	ShowMixer();
}



void ShowMixer(void)
{
    char value=GetSound(AY_Mixer);
    
	PrintSwitcher(7,11,!(value&Bit0)); //tone  A
    PrintSwitcher(17,11,!(value&Bit1)); //tone  B
    PrintSwitcher(27,11,!(value&Bit2)); //tone  C
    
    PrintSwitcher(7,12,!(value&Bit3)); //noise A
    PrintSwitcher(17,12,!(value&Bit4)); //noise B
    PrintSwitcher(27,12,!(value&Bit5)); //noise C
}



void PrintSwitcher(char col, char lin, boolean state)
{
    VLOCATE(col,lin);
	VPRINT(SwitcherTiles[state]);
}



void ShowAYregs(void)
{
	char value;
	char valueCoarse;
	uint period=0;
	
	ShowTone(AY_Channel_A);
	ShowTone(AY_Channel_B);
	ShowTone(AY_Channel_C);
	
	value=GetSound(AY_Noise);
	VPrintNumberO(0x19CF,value,2,144); //15,14
	
	value=GetSound(AY_AmpA);
	ShowAmp(AY_Channel_A,value);
	value=GetSound(AY_AmpB);
	ShowAmp(AY_Channel_B,value);
	value=GetSound(AY_AmpC);
	ShowAmp(AY_Channel_C,value);
	
	ShowMixer();
	
	value=GetSound(AY_EnvPeriod_fine);
	valueCoarse=GetSound(AY_EnvPeriod_coarse);
	period=(valueCoarse<<8)+value;
	VPrintNumberO(0x1A10,period,5,144); //16,16	
}


void ShowTone(char channel)
{
	char valueFine;
	char valueCoarse;
	uint period=0;
	
	uint vaddr=BASE5 + 6+(channel*10) + (7*32); //calculates the position of the numeric field
	if (channel==2) vaddr++;  // correct the position of the third column
	
	channel=channel*2;
	valueFine=GetSound(channel++);
	valueCoarse=GetSound(channel);
	period=(valueCoarse<<8)+valueFine;

	VPrintNumberO(vaddr,period,4,144);  //6+(channel*10),10
}



void ShowAmp(char channel, char value)
{
	boolean envState;
	uint vaddr=BASE5 + 5+(channel*10) + (9*32); //calculates the position of the numeric field
	
	VPrintNumberO(vaddr,value,2,144); //5+(channel*10),12

	if (value&Bit4) envState=true;
	else envState=false;

	PrintSwitcher(7+(channel*10),9,envState);
}



void ShowEnvelopeShape(char shape)
{
	char index;
	
	shape=shape&0x0F;
	index = EnvelopeIndex[shape];

	VPrintNumberO(0x1A30,shape,2,+144); //16,17
	CopyToVRAM((uint) EnvelopeShapes + (index*3), 0x1A33, 3); //19,17
}