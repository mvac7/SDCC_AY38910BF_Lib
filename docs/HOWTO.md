# How to use the PSG AY38910 BF MSX SDCC Libraries

---

## Index

- [1 Description](#1-Description)
- [2 Requirements](#2-Requirements)
- [3 AY Sound System](#3-AY-Sound-System)
- [4 PSG_AY38910BF Library](#4-PSG_AY38910BF-Library)
	- [4.1 Definitions](#41-Definitions)
		- [4.1.1 switcher Type](#411-SWITCHER-Type)
		- [4.1.2 AY Type](#412-AY-Type)
		- [4.1.3 AY Registers](#413-AY-Registers)
		- [4.1.4 Envelope shapes](#414-Envelope-shapes)
		- [4.1.5 AY channels](#415-AY-channels)
		- [4.1.6 AY port](#416-AY-port)
	- [4.2 Functions](#42-Functions)
		- [4.2.1 SelectAY](#421-SelectAY)
		- [4.2.2 InitAY](#422-InitAY)
		- [4.2.3 InitInternalAY](#423-InitInternalAY)		
		- [4.2.4 ClearDefAYbuffer](#424-ClearDefAYbuffer)		
		- [4.2.5 ClearAYbuffer](#425-ClearAYbuffer)		
		- [4.2.5 SOUND](#425-SOUND)
		- [4.2.6 GetSound](#426-GetSound)
		- [4.2.7 PlayAY](#427-PlayAY)
		- [4.2.8 Dump2AY](#428-Dump2AY)		
		- [4.2.9 SilenceAY](#429-SilenceAY)
		- [4.2.10 SilenceAYbuffer](#4210-SilenceAYbuffer)		
- [5 PSG_AY38910BF_eXtended Library](#5-PSG_AY38910BF_eXtended-Library)
	- [5.1 Functions](#51-Functions)   
	   - [5.1.1 SetTonePeriod](#511-SetTonePeriod)
	   - [5.1.2 SetNoisePeriod](#512-SetNoisePeriod)
	   - [5.1.3 SetVolume](#513-SetVolume)
	   - [5.1.4 EnableTone](#514-EnableTone)
	   - [5.1.5 EnableNoise](#515-EnableNoise)
	   - [5.1.6 EnableEnvelope](#516-EnableEnvelope)   
	   - [5.1.7 SetEnvelopePeriod](#517-SetEnvelopePeriod)
	   - [5.1.8 SetEnvelope](#518-SetEnvelope)
- [6 Set Internal or External AY](#6-Set-Internal-or-External-AY)
- [7 How to use](#7-How-to-use)
- [8 Appendices](#8-Appendices)
   - [8.1 AY-3-8910 Register Table](#81-AY-3-8910-Register-Table)  
- [9 References](#9-References)


<br/>

---

## 1 Description

C function libraries with functions to play sounds and/or music with the PSG AY-3-8910 or compatibles.

This project consists of two libraries that complement each other:
- **PSG_AY38910BF** Basic functions for initializing, editing and playing sound. It is the basis of the fR3eL Project audio system to be able to reproduce music and effects in game development (requires third-party libraries).
- **PSG_AY38910BF_eXtended** (optional) Adds specific functions to make it easier to write AY parameters. Requires the PSG_AY38910BF library.

PSG_AY38910BF includes the SOUND function with the same behavior as the command included in MSX BASIC, 
while PSG_AY38910BF_eXtended contains specific functions to modify the different sound parameters of the AY.

Security control of the I/O port enable bits in the Mixer register.
On some MSX computers that incorporate an AY-3-8910, they may be damaged if incorrect activation values ​​are written.
This library reads the port trigger values ​​and persists them, every time the PlayAY function is executed.

These libraries are part of the [MSX fR3eL Project](https://github.com/mvac7/SDCC_MSX_fR3eL).

You can use this library to develop applications for ROM, MSXBASIC or MSX-DOS environments, 
using the Small Device C Compiler [(SDCC)](http://sdcc.sourceforge.net/) cross compiler.

This project is open source under the [MIT license](LICENSE).<br/>
You can add part or all of this code in your application development or include it in other libraries/engines.

Enjoy it!                         

<br/>

---

## 2 Requirements

- [Small Device C Compiler (SDCC) v4.4](http://sdcc.sourceforge.net/)
- [Hex2bin v2.5](http://hex2bin.sourceforge.net/)

<br/>

---

## 3 AY Sound System

This library is designed to work with other libraries that use a buffer of AY registers (such as [PT3player](https://github.com/mvac7/SDCC_PT3player) and/or [ayFXplayer](https://github.com/mvac7/SDCC_ayFXplayer)). 
Includes a function that safely dumps buffer values ​​to an AY-3-8910 PSG.
It allows to use the internal PSG of the MSX or an external one (like the one incorporated in the MEGAFLASHROM SCC+, Flashjacks or others).

![AY Sound System](https://raw.githubusercontent.com/mvac7/SDCC_AY38910BF_Lib/master/docs/AYlibs.png)

<br/>

---

## 4 PSG_AY38910BF Library

This library includes only the functions necessary to play songs or effects (requires third-party libraries).

<br/>

### 4.1 Definitions

#### 4.1.1 switcher Type 

Data type definition to be used in SetChannel function (PSG_AY38910BF_Xfunctions Library).

Label | Value
:---  | ---:  
OFF   | 0
ON    | 1

<br/>

#### 4.1.2 AY Type 

Label | Value | Description
:---  | :---  | :--- 
AY_INTERNAL | 0xA0 | Internal MSX PSG
AY_EXTERNAL | 0x10 | MEGAFLASHROM SCC+, Flashjacks, Yamanooto, Carnivore2 or others

<br/>

#### 4.1.3 AY Registers

Label | Value | Description
:---  | ---:  | :--- 
AY_ToneA        | 0 | Channel A Tone Period (12 bits)
AY_ToneA_fine	| 0 | Channel A Fine Tune   (8 bits)
AY_ToneA_coarse	| 1 | Channel A Coarse Tune (4 bits)
AY_ToneB        | 2 | Channel B Tone Period (12 bits)
AY_ToneB_fine   | 2 | Channel B Fine Tune   (8 bits)
AY_ToneB_coarse | 3 | Channel B Coarse Tune (4 bits)
AY_ToneC        | 4 | Channel C Tone Period (12 bits)
AY_ToneC_fine   | 4 | Channel C Fine Tune   (8 bits)
AY_ToneC_coarse | 5 | Channel C Coarse Tune (4 bits)
AY_Noise     |  6 | Noise Period (5 bits)
AY_Mixer     |  7 | Mixer
AY_AmpA      |  8 | Channel Volume A (4 bits + B5 active Envelope)
AY_AmpB      |  9 | Channel Volume B (4 bits + B5 active Envelope)
AY_AmpC      | 10 | Channel Volume C (4 bits + B5 active Envelope)
AY_EnvPeriod        | 11 | Envelope Period (16 bits)
AY_EnvPeriod_fine   | 11 | Envelope Fine Tune   (8 bits)
AY_EnvPeriod_coarse | 12 | Envelope Coarse Tune (8 bits)
AY_EnvShape         | 13 | Envelope Shape

<br/>

#### 4.1.4 Envelope shapes

The header file defines envelope shapes in case you prefer to use it instead of the numerical form:

Label | Value
:---  | ---:  
AY_ENV_LowerBeat     | 1 
AY_ENV_Upper         | 4 
AY_ENV_LeftSaw       | 8
AY_ENV_LowerTriangle | 10 
AY_ENV_LowerHold     | 11 
AY_ENV_RightSaw      | 12
AY_ENV_UpperHold     | 13
AY_ENV_UpperTriangle | 14

**Attention!** The LowerBeat shape may be produced with the values: 0, 1, 2, 3 and 9. 
The value 1 has been assigned to *LowerBeat* as 0 can be useful in case you need to control when the envelope is triggered. In this case 0 can act as a "don't
write the envelop" event. Remember that, every time the register 13 is written, the sound with the assigned shape is started.

The Upper shape may be produced with the values: 4, 5, 6, 7 and 15.

<br/>

#### 4.1.5 AY channels

You can use it in the functions: SetTonePeriod, SetVolume and SetChannel

Label | Value
:---  | ---: 
AY_Channel_A | 0
AY_Channel_B | 1
AY_Channel_C | 2

<br/> 

#### 4.1.6 AY port

Label       | Value | Description
:---        | :---  | :---
AY_INTERNAL | 0xA0  | Internal PSG
AY_EXTERNAL | 0x10  | External PSG (MEGAFLASHROM SCC+, Flashjacks, Yamanooto, Carnivore2 or others)

<br/> 

---
 
### 4.2 Functions

#### 4.2.1 SelectAY

<table>
<tr><th colspan=2 align="left">SelectAY</th></tr>
<tr><td colspan="2">Select the default AY using the index port value.</td></tr>
<tr><th>Function</th><td>SelectAY()</td></tr>
<tr><th>Input</th><td>char</td><td>AY index port<br/>(0xA0 for internal or 0x10 for external)</td></tr>
<tr><th>Output</th><td> - </td></tr>
</table>

##### Example:

```c
	SelectAY(AY_EXTERNAL);
```

<br/> 

#### 4.2.2 InitInternalAY

<table>
<tr><th colspan=2 align="left">InitInternalAY</th></tr>
<tr><td colspan="2">Initialize the library.<br/>Set default AY (internal) and clear buffer.</td></tr>
<tr><th>Function</th><td>InitInternalAY()</td></tr>
<tr><th>Input</th><td> - </td></tr>
<tr><th>Output</th><td> - </td></tr>
</table>

##### Example:

```c
	InitAY();
```

<br/> 

#### 4.2.3 InitAY

<table>
<tr><th colspan=3 align="left">InitAY</th></tr>
<tr><td colspan=3>Initialize the library.<br/>Sets the default AY and default AY buffer.<br/>Also initialize the buffer.</td></tr>
<tr><th>Function</th><td colspan=2>InitAY(AY_port, bufferADDR)</td></tr>
<tr><th rowspan=2>Input</th><td>char</td><td>AY index port<br/>(0xA0 for internal or 0x10 for external)</td></tr>
<tr><td>unsigned int</td><td>buffer address of AY registers</td></tr>
<tr><th>Output</th><td colspan=2> - </td></tr>
</table>

##### Example:

```c
	char AYREGS_plus[14];	// buffer for second AY
	InitAY(AY_EXTERNAL,(unsigned int) AYREGS_plus);
```


<br/> 

#### 4.2.4 ClearDefAYbuffer

<table>
<tr><th colspan=2 align="left">ClearDefAYbuffer</th></tr>
<tr><td colspan="2">Initializes default buffer of AY registers.</td></tr>
<tr><th>Function</th><td>ClearDefAYbuffer()</td></tr>
<tr><th>Input</th><td> - </td></tr>
<tr><th>Output</th><td> - </td></tr>
</table>

##### Example:

```c
	ClearDefAYbuffer();
```

<br/> 

#### 4.2.5 ClearAYbuffer

<table>
<tr><th colspan=3 align="left">ClearAYbuffer</th></tr>
<tr><td colspan=3>Initializes a buffer of AY registers</td></tr>
<tr><th>Function</th><td colspan=2>ClearAYbuffer(bufferADDR)</td></tr>
<tr><th>Input</th><td>unsigned int</td><td>memory address of AY buffer</td></tr>
<tr><th>Output</th><td colspan=2> - </td></tr>
</table>

##### Example:

```c
	char AYREGS_plus[14];	// buffer for second AY
	ClearAYbuffer((unsigned int) AYREGS_plus);
```

<br/> 

#### 4.2.6 SOUND

<table>
<tr><th colspan=3 align="left">SOUND</th></tr>
<tr><td colspan=3>Writes a value to the AY buffer.</td></tr>
<tr><th>Function</th><td colspan=2>SOUND(register, value)</td></tr>
<tr><th rowspan=2>Input</th><td>char</td><td>register number (0 to 13)</td></tr>
<tr><td>char</td><td>value</td></tr>
<tr><th>Output</th><td colspan=2> - </td></tr>
</table>

##### Example:

```c
	SOUND(8,16);		//channel A envelope on
	SOUND(AY_Noise,31);	//Set noise period
```

<br/> 

#### 4.2.7 GetSound

<table>
<tr><th colspan=3 align="left">GetSound</th></tr>
<tr><td colspan=3>Read a register value from the AY buffer.</td></tr>
<tr><th>Function</th><td colspan=2>GetSound(register)</td></tr>
<tr><th>Input</th><td>char</td><td>register number (0 to 13)</td></tr>
<tr><th>Output</th><td>char</td><td>value</td></tr>
</table>

##### Example:

```c
	char value;            
	value = GetSound(8);	//read register 8 (Channel Volume A)
```

<br/> 

#### 4.2.7 PlayAY

<table>
<tr><th colspan=2 align="left">PlayAY</th></tr>
<tr><td colspan="2">Dump default AY buffer to default AY.<br/>Execute on each interruption of VBLANK or when you want to throw in a change in sound.</td></tr>
<tr><th>Function</th><td>PlayAY()</td></tr>
<tr><th>Input</th><td> - </td></tr>
<tr><th>Output</th><td> - </td></tr>
</table>

##### Attention!

PlayAY uses the Dump2AY function with the AY I/O port number and buffer configured in the library.

It includes a control that Dump2AY doesn't have: it adds a flag to register 13 (envelope shape) after playing.

This is necessary because this function is designed to be executed on every frame, preventing the envelope from being triggered continuously, thus avoiding the generation of an unwanted sound.

It is likely that the music or sound effects player control this problem.

Remember that once you write to register 13, it is the PSG that applies the envelope according to the waveform and period parameters.

It has been included to ensure that whether we use it with a Player or independently this problem does not occur.

<br/> 

##### Example:

```c
	PlayAY();
```

<br/> 

#### 4.2.8 Dump2AY

<table>
<tr><th colspan=3 align="left">Dump2AY</th></tr>
<tr><td colspan=3>Dump a buffer to the indicated AY.<br/>(Execute on each interruption of VBLANK or when you want to throw in a change in sound.).</td></tr>
<tr><th>Function</th><td colspan=2>Dump2AY(AY_port, bufferADDR)</td></tr>
<tr><th rowspan=2>Input</th><td>char</td><td>AY index port</td></tr>
<tr><td>unsigned int</td><td>buffer address of AY registers</td></tr>
<tr><th>Output</th><td colspan=2> --- </td></tr>
</table>

##### Attention!

While PlayAY is designed for when we use a single AY, Dump2AY allows us to have more control of the output and take advantage of a second AY.

This function does not disable the envelope trigger (register 13).
It is intended for when you use two AYs simultaneously playing a song and FX. You must first transfer the registers to the second AY with this function and then to the main AY with PlayAY.

<br/> 

##### Example:

```c
	char AYREGS_plus[14];	// buffer for second AY
	unsigned int freqval = 1024;
	AYREGS_plus[AY_ToneA]=freqval & 0xFF;
	AYREGS_plus[AY_ToneA+1]=freqval>>8;
	AYREGS_plus[AY_Noise]=22;
	AYREGS_plus[AY_AmpA]=15;
	AYREGS_plus[AY_Mixer]=0b10110110;	//0-->ON;1-->OFF
	Dump2AY(AY_EXTERNAL,(unsigned int) AYREGS_plus);
```

<br/>

#### 4.2.9 SilenceAY

<table>
<tr><th colspan=3 align="left">SilenceAY</th></tr>
<tr><td colspan=3>Silences default AY sound processor.<br/>Run SilenceAYbuffer with the default port and buffer parameters.</td></tr>
<tr><th>Function</th><td colspan=2>SilenceAY()</td></tr>
<tr><th>Input</th><td colspan=2> - </td></tr>
<tr><th>Output</th><td colspan=2> - </td></tr>
</table>

##### Example:

```c
	SilenceAY();
```

<br/> 

#### 4.2.10 SilenceAYbuffer

<table>
<tr><th colspan=3 align="left">SilenceAYbuffer</th></tr>
<tr><td colspan=3>Silences an AY sound processor.<br/>
It resets the amplitude value of the three channels to zero and dumps it to the AY.<br/>
This is indicated for the case of playing sound dynamically between AYs (Internal/External), so that the last written values ​​do not sound infinitely.</td></tr>
<tr><th>Function</th><td colspan=2>SilenceAYbuffer(port,bufferADDR)</td></tr>
<tr><th rowspan=2>Input</th><td>char</td><td>AY index port</td></tr>
<tr><td>unsigned int</td><td>buffer address of AY registers</td></tr>
<tr><th>Output</th><td colspan=2> - </td></tr>
</table>

##### Example:

```c
	SilenceAYbuffer(AY_INTERNAL,AY_defAYREGs_addr);			//silence internal AY
```

```c
	char AYREGS_plus[14];	// buffer for second AY
	SilenceAYbuffer(0x10, (unsigned int) AYREGS_plus);		//silence external AY (MEGAFLASHROM SCC+, Flashjacks or others)
```

<br/> 

---

## 5 PSG_AY38910BF_eXtended Library

This library adds specific functions to make it easier to write AY parameters. 

Requires the PSG_AY38910BF library.

<br/> 

### 5.1 Functions

#### 5.1.1 SetTonePeriod

<table>
<tr><th colspan=3 align="left">SetTonePeriod</th></tr>
<tr><td colspan=3>Set Tone Period for any channel</td></tr>
<tr><th>Function</th><td colspan=2>SetTonePeriod(channel, period)</td></tr>
<tr><th rowspan=2>Input</th><td>[char]</td><td>channel (0, 1 or 2)</td></tr>
<tr><td>[unsigned int]</td><td>period (0 - 4095)</td></tr>
<tr><th>Output</th><td colspan=2> --- </td></tr>
</table>


#### Example:

```c
	SetTonePeriod(AY_Channel_B,1100);  //set tone period for channel A
```

<br/> 

#### 5.1.2 SetNoisePeriod

<table>
<tr><th colspan=3 align="left">SetNoisePeriod</th></tr>
<tr><td colspan=3>Set Noise Period</td></tr>
<tr><th>Function</th><td colspan=2>SetNoisePeriod(period)</td></tr>
<tr><th>Input</th><td>[char]</td><td>period (0 - 31)</td></tr>
<tr><th>Output</th><td colspan=2> --- </td></tr>
</table>

#### Example:

```c
	SetNoisePeriod(10);
```

<br/> 

#### 5.1.3 SetVolume

<table>
<tr><th colspan=3 align="left">SetVolume</th></tr>
<tr><td colspan=3>Set volume for any channel</td></tr>
<tr><th>Function</th><td colspan=2>SetVolume(channel, volume)</td></tr>
<tr><th rowspan=2>Input</th><td>[char]</td><td>channel (0, 1 or 2)</td></tr>
<tr><td>[char]</td><td>volume, 0 to 15 or 16 for activate envelope</td></tr>
<tr><th>Output</th><td colspan=2> --- </td></tr>
</table>

#### Example:
```c
	SetVolume(0,14);	// set 14 volume level for channel A
	SetVolume(AY_Channel_B,16);	// activate envelope for channel B
```

<br/> 

### 5.1.4 EnableTone

<table>
<tr><th colspan=3 align="left">EnableTone</th></tr>
<tr><td colspan=3>Mixer. Enables or disables Tone channel.</td></tr>
<tr><th>Function</th><td colspan=2>EnableTone(channel, state)</td></tr>
<tr><th rowspan=2>Input</th><td>char</td><td>channel (0, 1 or 2)</td></tr>
<tr><td>switcher</td><td>state (ON=enable;OFF=disable)</td></tr>
<tr><th>Output</th><td colspan=2> - </td></tr>
</table>

#### Example:

```c
	EnableTone(0,ON);
	EnableTone(1,ON);
	EnableTone(AY_Channel_C,OFF);
```

<br/> 

### 5.1.5 EnableNoise

<table>
<tr><th colspan=3 align="left">EnableNoise</th></tr>
<tr><td colspan=3>Mixer. Enables or disables noise on a channel.</td></tr>
<tr><th>Function</th><td colspan=2>EnableNoise(channel, state)</td></tr>
<tr><th rowspan=2>Input</th><td>char</td><td>channel (0, 1 or 2)</td></tr>
<tr><td>switcher</td><td>state (ON=enable;OFF=disable)</td></tr>
<tr><th>Output</th><td colspan=2> - </td></tr>
</table>

#### Example:

```c
	EnableNoise(AY_Channel_A,ON);
	EnableNoise(AY_Channel_B,OFF);
	EnableNoise(AY_Channel_C,OFF);
```

<br/> 

### 5.1.6 EnableEnvelope

<table>
<tr><th colspan=3 align="left">EnableEnvelope</th></tr>
<tr><td colspan=3>Enables or disables sound envelope on a channel.</td></tr>
<tr><th>Function</th><td colspan=2>EnableEnvelope(channel, state)</td></tr>
<tr><th rowspan=2>Input</th><td>char</td><td>channel (0, 1 or 2)</td></tr>
<tr><td>switcher</td><td>state (ON=enable;OFF=disable)</td></tr>
<tr><th>Output</th><td colspan=2> - </td></tr>
</table>

#### Example:

```c
	EnableEnvelope(AY_Channel_A,OFF);
	EnableEnvelope(AY_Channel_B,ON);
	EnableEnvelope(AY_Channel_C,OFF);
```

<br/> 

#### 5.1.7 SetEnvelopePeriod

<table>
<tr><th colspan=3 align="left">SetEnvelopePeriod</th></tr>
<tr><td colspan=3>Set Envelope Period</td></tr>
<tr><th>Function</th><td colspan=2>SetEnvelopePeriod(period)</td></tr>
<tr><th>Input</th><td>[unsigned int]</td><td>period (0 - 65535)</td></tr>
<tr><th>Output</th><td colspan=2> --- </td></tr>
</table>

#### Example:

```c
	SetEnvelopePeriod(1000);
```

<br/> 


#### 5.1.6 PlayEnvelope

<table>
<tr><th colspan=3 align="left">PlayEnvelope</th></tr>
<tr><td colspan=3>Set envelope shape.<br/>It will only affect channels that have the envelope active (see SetVolume).</td></tr>
<tr><th>Function</th><td colspan=2>PlayEnvelope(shape)</td></tr>
<tr><th>Input</th><td>[char]</td><td>shape (0-15)</td></tr>
<tr><th>Output</th><td colspan=2> --- </td></tr>
</table>


#### Examples:

```c
	PlayEnvelope(0);	//Play LowerBeat envelope shape
```
	
```c
	PlayEnvelope(ENV_LowerTriangle);	//Play LowerTriangle envelope shape
```

<br/> 

---

## 6 Set Internal or External AY


This library provides you with several ways to work with one or two PSGs.

You can play the sound to the internal AY, to an external one, or to both at the same time.

If you have an external AY, you can control what is played in each of them. You can modify the buffer values ​​or create a time offset between them to create effects.

You can indicate a default AY to use simple functions or you can use parameterized functions where you can indicate what you want to do at any time.




To indicate in which PSG the sounds are to be played, you have the **AY_IOport** variable. 
To select an external AY (ports 10h to 12h), like the one included in the MegaFlashROM SCC+, Flashjacks or other, you have to set the variable to AY_EXTERNAL.

```c
  AY_IOport = AY_EXTERNAL;
```

**Attention!** When you execute the InitAY() function, it will be updated to the default value corresponding to the internal AY.

<br/> 

---


## 7 How to use

This library is based on a buffer containing the values ​​of the AY registers. 
We can write to it at any time but it does not generate any sound until the buffer is dumped to the sound processor using the PlayAY or Dump2AY functions.

It can work in different ways:

### Write and Play
You can write the information that defines a sound effect using the generic function SOUND (example Test0.c) or the specific ones contained in the PSG_AY38910BF_Xfunctions library (example Test1.c) and then trigger the sound at any time with PlayAY or Dump2AY.

<br/>

### Play in frame
This method consists of using the frame interrupt to dump the buffer. Any changes made to the buffer will take effect directly.

This is the way to go when using song players like PT3player.

<br/>

### Using two AYs

This library provides functionality to use a second external AY sound processor.
This may come included in hardware such as the MEGAFLASHROM SCC+, Flashjacks, Yamanooto, Carnivore2 or others.
It allows dumping data from the main buffer or a second buffer to either of the two PSGs.






<br/>

### Examples

#### Example01.c 

Perform a simple test of the library, where a sound effect is played using the SOUND function.

```c
/* =============================================================================
# Example01

- Architecture: MSX
- Format: 8K ROM
- Compiler: SDCC 4.4

## Description:
Perform a simple test of the library, where a sound effect is played using 
the SOUND function.
============================================================================= */
#include "PSG_AY38910BF.h"

#define  HALT	 __asm halt __endasm

void main(void)
{
	unsigned int time = 10*50;					//10 seconds in PAL
	unsigned int tone = 0x06AE;					//C Octave 1
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
}
```

To compile you will need to run these two statements from the command line (for Windows OS):

```bat
sdcc -mz80 -o build\ --code-loc 0x4020 --data-loc 0xC000 --use-stdout --no-std-crt0 crt_MSX816kROM4000.rel PSG_AY38910BF.rel Example01.c
hex2bin -e bin -l 4000 Example01.ihx
```

<br/>

#### Example02.c

Example where a rhythm and a melody are generated using the libraries PSG_AY38910BF and PSG_AY38910BF_eXtended

```c
/* =============================================================================
# Example02

- Architecture: MSX
- Format: 8K ROM
- Compiler: SDCC 4.4

## Description:
Example where a rhythm and a melody are generated using the libraries 
PSG_AY38910BF and PSG_AY38910BF_eXtended
============================================================================= */
#include "PSG_AY38910BF.h"
#include "PSG_AY38910BF_eXtended.h"

#define  HALT	 __asm halt __endasm


// WF: Cosine Unsigned Length=32 Min=4 Max=12 Phase=0 Freq=1
const char InstrEnvelop[]={
0x0C,0x0C,0x0C,0x0B,0x0B,0x0A,0x09,0x09,0x08,0x07,0x06,0x06,0x05,0x05,0x04,0x04};

//0=Silent;1=C;2=C#;3=D;4=D#;5=E;6=F;7=F#;8=G;9=G#;10=A;11=A#;12=B
const char pattern00[]={
	1,1,0,1,1,0,3,4,
	1,1,0,1,1,0,5,4};

//tone period by note (0-11) octave 1
const unsigned int toneFreqz[]={
0,
0x06AE,0x064E,0x05F4,0x059E,0x054D,0x0501,0x04B9,0x0475,0x0435,0x03F9,0x03C0,0x038A};	//octave 1
//0x0357,0x0327,0x02FA,0x02CF,0x02AF,0x0281,0x025D,0x023B,0x021B,0x01FC,0x01E0,0x01C5};	//Octave 2


void SetKick(void);
void SetHihat(void);


void main(void)
{
	char frame=0;
	char step=0;
	char track=0;
	char note=0;
		
	InitAY();  							/* Init library. Set default AY (internal) 
										   and clear Buffer */
	//InitAYbuffer(AY_EXTERNAL, (unsigned int) AYREGS);			//option for test extern AY
	
	SetNoisePeriod(4);					//Set noise period

/* ------------------------------------------------------------------
This code is not necessary since the initialization by default leaves 
the tone enabled on all three channels and disables the noise.
	EnableTone(AY_Channel_A,ON);	//Enable Tone in channel A
	EnableNoise(AY_Channel_A,OFF);	//Enable Noise in channel A
------------------------------------------------------------------ */

	EnableEnvelope(AY_Channel_C,ON);
	SetTonePeriod(AY_Channel_C,0x0D5D);	//Set channel A tone period	(C note · Octave 0)


	//8 tracks; 16 steps
	while(track<8)
	{
		HALT;
		PlayAY();						//Dump AY buffer to PSG (the sound is played)
		
		if(frame==0)
		{
			//Play Drums in chanel C
			if((step&0b00000011)==0) SetKick();		//0,4,8,12
			if((step&0b00000011)==2) SetHihat();	//2,6,10,14

			//Play tone in channel A
			note=pattern00[step];
			if(note==0) SetVolume(AY_Channel_A,0);
			else SetTonePeriod(AY_Channel_A, toneFreqz[note]);	//Set channel A tone period
				
			step++;
			if(step>15){step=0;track++;}
		}
		
		if(note>0) SetVolume(AY_Channel_A,InstrEnvelop[frame]);	//Change the volume to generate an envelope
		
		frame++;
		if(frame>15) frame=0;		
	}
	//SilenceAY();						//Stop any sound!

}


//This function will trigger a Drum-like sound the next time PlayAY is run.
void SetKick(void)
{
	SetEnvelopePeriod(1024);
	EnableTone(AY_Channel_C,ON);		//Enable Tone in channel C
	EnableNoise(AY_Channel_C,OFF);		//Enable Noise in channel C
	SetEnvelope(AY_ENV_LowerBeat);
}


//This function will trigger a Hihat-like sound the next time PlayAY is run.
void SetHihat(void)
{
	SetEnvelopePeriod(808);
	EnableTone(AY_Channel_C,OFF);		//Enable Tone in channel C
	EnableNoise(AY_Channel_C,ON);		//Enable Noise in channel C
	SetEnvelope(AY_ENV_LowerBeat);
}
```

To compile you will need to run these two statements from the command line (for Windows OS):

```bat
sdcc -mz80 -o build\ --code-loc 0x4020 --data-loc 0xC000 --use-stdout --no-std-crt0 crt_MSX816kROM4000.rel PSG_AY38910BF.rel PSG_AY38910BF_Xfunctions.rel Example02.c
hex2bin -e bin -l 4000 Example02.ihx
```

<br/>

#### Example03.c

Example of playing a sound on two AYs simultaneously (internal and external).

```c
/* =============================================================================
# Example03

- Architecture: MSX
- Format: 8K ROM
- Compiler: SDCC 4.4
- Library: fR3eL Project

## Description:
Example of playing a sound on two AYs simultaneously (internal and external).
============================================================================= */
#include "PSG_AY38910BF.h"
#include "PSG_AY38910BF_eXtended.h"

#define  HALT	 __asm halt __endasm

void main(void)
{
	unsigned int period=0;
		
	InitAY();    						/* Init library. Set default AY (internal) 
										   and clear Buffer */
	
	SetNoisePeriod(20);					//Set noise period
	EnableEnvelope(AY_Channel_A,ON);	//channel A envelope on
	EnableTone(AY_Channel_A,ON);		//Enable Tone on channel A
	EnableNoise(AY_Channel_A,ON);		//Enable Noise on channel A
	SetEnvelopePeriod(1000);			//Set envelope period
	SetEnvelope(AY_ENV_UpperTriangle);	//Set envelope shape

	while(period<4096)
	{
		HALT;
		Dump2AY(AY_EXTERNAL,(unsigned int) AYREGS);	//Dump AY buffer to second AY
		PlayAY();									//Dump AY buffer to internal PSG
		
		SetTonePeriod(AY_Channel_A, period);		//Set channel A period
		period += 8;		
	}
	
	SilenceAY();						//Stop any sound!
}
```

To compile you will need to run these two statements from the command line (for Windows OS):

```bat
sdcc -mz80 -o build\ --code-loc 0x4020 --data-loc 0xC000 --use-stdout --no-std-crt0 crt_MSX816kROM4000.rel PSG_AY38910BF.rel PSG_AY38910BF_Xfunctions.rel Example03.c
hex2bin -e bin -l 4000 Example03.ihx
```

<br/>

---

## 8 Appendices

### 8.1 AY-3-8910 Register Table

<table>
<tr>
<th colspan=2>Register\bit</th><th width=50>B7</th><th width=50>B6</th><th width=50>B5</th><th width=50>B4</th><th width=50>B3</th><th width=50>B2</th><th width=50>B1</th><th width=50>B0</th>
</tr>
<tr>
<td>R0</td><td rowspan=2>Channel A Tone Period (12 bit)</td><td colspan=8 align="center">8 bit Fine Tune</td>
</tr>
<tr>
<td>R1</td><td colspan=4></td><td colspan=4 align="center">4 bit Coarse Tune</td>
</tr>
<tr>
<td>R2</td><td rowspan=2>Channel B Tone Period (12 bit)</td><td colspan=8 align=center>8 bit Fine Tune</td>
</tr>
<tr>
<td>R3</td><td colspan=4></td><td colspan=4 align="center">4 bit Coarse Tune</td>
</tr>
<tr>
<td>R4</td><td rowspan=2>Channel C Tone Period (12 bit)</td><td colspan=8 align=center>8 bit Fine Tune</td>
</tr>
<tr>
<td>R5</td><td colspan=4></td><td colspan=4 align="center">4 bit Coarse Tune</td>
</tr>
<tr>
<td>R6</td><td>Noise period</td><td colspan=3></td><td colspan=5 align=center>5 bit Period control</td>
</tr>
<tr>
<td rowspan=2>R7</td><td rowspan=2>Mixer Control<br/>Enable (0=on, 1=off)</td><td colspan=2 align="center">IN/OUT</td><td colspan=3 align="center">Noise</td><td colspan=3 align="center">Tone</td></tr>
<tr>
<td align=center> IOB</td><td align="center">IOA</td><td align="center">C</td><td align=center>B</td><td align="center">A</td><td align="center">C</td><td align=center>B</td><td align="center">A</td>
</tr>
<tr>
<td>R8</td><td>Channel A Volume<br/>Envelope (1=on, 0=off)</td><td colspan=3></td><td align="center">Env</td><td colspan=4 align="center">Amplitude Level</td>
</tr>
<tr>
<td>R9</td><td>Channel B Volume<br/>Envelope (1=on, 0=off)</td><td colspan=3></td><td align="center">Env</td><td colspan=4 align="center">Amplitude Level</td>
</tr>
<tr>
<td>R10</td><td>Channel C Volume<br/>Envelope (1=on, 0=off)</td><td colspan=3></td><td align="center">Env</td><td colspan=4 align="center">Amplitude Level</td>
</tr>
<tr>
<td>R11</td><td rowspan=2>Envelope Period (16 bit)</td><td colspan=8 align="center">8 bit Fine Tune Envelope</td>
</tr>
<tr>
<td>R12</td><td colspan=8 align="center">8 bit Coarse Tune Envelope</td>
</tr>
<tr>
<td>R13</td><td>Envelope Shape</td><td colspan=4></td><td>Cont</td><td>Att</td><td>Alt</td><td>Hold</td>
</tr>
<tr>
<td>R14</td><td>I/O Port A Data Store</td><td colspan=8 align="center">8 bit Parallel I/O on Port A</td>
</tr>
<tr>
<td>R15</td><td>I/O Port B Data Store</td><td colspan=8 align="center">8 bit Parallel I/O on Port B</td>
</tr>
</table>

<br/>

---

## 9 References

* [General Instrument AY-3-8910 (wikipedia)](https://en.wikipedia.org/wiki/General_Instrument_AY-3-8910)
* [GI AY-3-8910 Datasheet (PDF)](http://map.grauw.nl/resources/sound/generalinstrument_ay-3-8910.pdf)

<br/>

### External Hardware

* [MEGAFLASHROM SCC+](https://www.msxcartridgeshop.com/) by MSX Cartridge Shop
* [Yamanooto Specs](https://www.msx.org/wiki/The_SCC_Alliance_Yamanooto) (MRC Wiki) by The SCC Alliance
* [FlashJacks](https://www.retromsx.com/todo-flashjacks/) by Aquijacks
* [Carnivore2](https://sysadminmosaic.ru/en/msx/carnivore2/carnivore2?redirect=1#sound_settings) by RBSC

<br/>

---

![Creative Commons License](https://i.creativecommons.org/l/by-nc/4.0/88x31.png) 
<br/>This document is licensed under a [Creative Commons Attribution-NonCommercial 4.0 International License](http://creativecommons.org/licenses/by-nc/4.0/).