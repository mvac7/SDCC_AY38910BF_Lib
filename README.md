# PSG AY-3-8910 BF MSX SDCC Libraries (fR3eL Project)

<table>
<tr><td rowspan=2>Name</td><td>PSG_AY38910BF</td></tr>
<tr><td>PSG_AY38910BF_eXtended</td></tr>
<tr><td>Architecture</td><td>MSX</td></tr>
<tr><td>Environment</td><td>ROM, MSX-DOS, MSX BASIC</td></tr>
<tr><td>Programming language</td><td>C</td></tr>
<tr><td>Format</td><td>SDCC Relocatable object file (.rel)</td></tr>
<tr><td>Compiler</td><td>SDCC v4.4</td></tr>
</table>


---

## Description

C function libraries with functions to play sounds and/or music with the PSG AY-3-8910 or compatibles.

This project consists of two libraries that complement each other:
- **PSG_AY38910BF** Basic functions for initializing, editing and playing sound. It is the basis of the fR3eL Project audio system to be able to reproduce music and effects in game development (requires third-party libraries).
- **PSG_AY38910BF_eXtended** (optional) Adds specific functions to make it easier to write AY parameters. Requires the PSG_AY38910BF library.

PSG_AY38910BF includes the SOUND function with the same behavior as the command included in MSX BASIC, 
while PSG_AY38910BF_eXtended contains specific functions to modify the different sound parameters of the AY.

Security control of the I/O port enable bits in the Mixer register.
On some MSX computers that incorporate an AY-3-8910, they may be damaged if incorrect activation values ​​are written.
This library reads the port trigger values ​​and persists them, every time the PlayAY function is executed.

You can access the documentation here with [`How to use the library`](docs/HOWTO.md).

These libraries are part of the [MSX fR3eL Project](https://github.com/mvac7/SDCC_MSX_fR3eL).

You can use this library to develop applications for ROM, MSXBASIC or MSX-DOS environments, 
using the Small Device C Compiler [(SDCC)](http://sdcc.sourceforge.net/) cross compiler.

This project is open source under the [MIT license](LICENSE).<br/>
You can add part or all of this code in your application development or include it in other libraries/engines.

Enjoy it!

<br/>

---

## History of versions

### PSG_AY38910BF Library

- v1.0 (08/02/2025) 
	- update to SDCC (4.1.12) Z80 calling conventions
	- Added a function to initialize audio system with different port and buffer (InitAYbuffer)
	- Added function to clear the AY registers buffer (ClearAY and ClearAYbuffer)
	- Added two functions to play a given buffer and port (Dump2AY).
	- Added two functions to mute PSG (SilenceAY and SilenceAYbuffer).
	- Add a function to select default AY (SelectAY).
- v0.9b (16/07/2021) First version (Based in AY-3-8910 RT Library)

<br/>

### PSG_AY38910BF_eXtended Library

- v1.1 (14/08/2025) 
	- Added EnableEnvelope, EnableTone and EnableNoise functions
	- Removed SetChannel function
- v1.0 (03/03/2025) First version

<br/>

---

## Requirements

- [Small Device C Compiler (SDCC) v4.4](http://sdcc.sourceforge.net/)
- [Hex2bin v2.5](http://hex2bin.sourceforge.net/)

<br/>

---

## fR3eL Sound System

This library is designed to work with other libraries that use a buffer of AY registers (such as [PT3player](https://github.com/mvac7/SDCC_PT3player) and/or [ayFXplayer](https://github.com/mvac7/SDCC_ayFXplayer)). 
Includes a function that safely dumps buffer values ​​to an AY-3-8910 PSG.
It allows to use the internal PSG of the MSX or an external one (like the one incorporated in the MEGAFLASHROM SCC+, Flashjacks, Yamanooto, Carnivore2 or others).

![AY Sound System](docs/AYlibs.png) 

<br/>

---

## Functions

### PSG_AY38910BF Library

| Namne | Function Prototype | Description |
| :--- | :--- | :--- |
| SelectAY | `SelectAY(char port)` | Select the default AY using the index port value. |
| InitAY   | `InitInternalAY()` | Initialize the library.<br/>Select as default the internal AY and the library AY buffer.<br/>Also initialize the buffer.  |
| InitAY buffer    | `InitAY(char port, unsigned int bufferADDR)` | Initialize the library.<br/>Sets the default AY and default AY buffer.<br/>Also initialize the buffer. |
| ClearDefAYbuffer | `ClearDefAYbuffer()` | Initializes default buffer of AY registers |
| ClearAYbuffer    | `ClearAYbuffer(unsigned int bufferADDR)` | Initializes a buffer of AY registers |
| SOUND     | `SOUND(char reg, char value)` | Writes a value to the AY buffer |
| GetSound  | `char GetSound(char reg)` | Read a register value from the AY buffer |
| PlayAY    | `PlayAY()` | Dump default AY buffer to default AY. |
| Dump2AY   | `Dump2AY(char AY_port, unsigned int bufferADDR)` | Dump an AY buffer to the indicated AY |
| SilenceAY | `SilenceAY()` | Silences default AY sound processor. |
| SilenceAYbuffer | `SilenceAYbuffer(char AY_port, unsigned int bufferADDR)` | Silences an AY sound processor. |

<br/>

### PSG_AY38910BF_eXtended Library

| Namne | Function Prototype | Description |
| :--- | :--- | :--- |
| SetTonePeriod     | `SetTonePeriod(char channel, unsigned int period)` | Set Tone Period for any channel |
| SetNoisePeriod    | `SetNoisePeriod(char period)` | Set Noise Period |
| SetVolume         | `SetVolume(char channel, char volume)` | Set volume channel |
| EnableTone        | `EnableTone(char channel, switcher state)` | Mixer. Enables or disables Tone channel  |
| EnableTone        | `EnableTone(char channel, switcher state)` | Mixer. Enables or disables noise on a channel |
| EnableEnvelope    | `EnableEnvelope(char channel, switcher state)` | Enables or disables sound envelope on a channel |
| SetEnvelopePeriod | `SetEnvelopePeriod(unsigned int period)` | Set Envelope Period |
| SetEnvelope       | `SetEnvelope(char shape)` | Set envelope shape |

<br/>

---

## Code Examples

The project includes several examples that I have used to test the library and that can help you learn how to use this library.

<br/>

### TestPSGAYBFLib

Performs a test of the functions of the PSG_AY38910BF library.

Format: MSX 8K ROM

[`Sourcecode`](PSG_AY38910BF/test)

![Test screenshot 1](docs/pics/TESTAYBF.1.png) 
![Test screenshot 2](docs/pics/TESTAYBF.2.png)<br/> 
![Test screenshot 3](docs/pics/TESTAYBF.3.png) 
![Test screenshot 4](docs/pics/TESTAYBF.4.png) 
![Test OpenMSX screenshot 5](docs/pics/openMSX_MFRPSG.png) 

<br/>

### TestAY

Performs a test of the functions of the PSG_AY38910BF and PSG_AY38910BF_eXtended libraries.

Allows testing of an external AY.

Format: MSX 16K ROM

[`Sourcecode`](PSG_AY38910BF_eXtended/test)

![TestAY screenshot 1](docs/pics/TESTAY.1.png) 
![TestAY screenshot 2](docs/pics/TESTAY.2.png) 

<br/>

---

## References

* [General Instrument AY-3-8910 (wikipedia)](https://en.wikipedia.org/wiki/General_Instrument_AY-3-8910)
* [GI AY-3-8910 Datasheet (PDF)](http://map.grauw.nl/resources/sound/generalinstrument_ay-3-8910.pdf)

<br/>

### Hardware for a second AY

* [MEGAFLASHROM SCC+](https://www.msxcartridgeshop.com/) by MSX Cartridge Shop
* [Yamanooto Specs](https://www.msx.org/wiki/The_SCC_Alliance_Yamanooto) (MRC Wiki) by The SCC Alliance
* [FlashJacks](https://www.retromsx.com/todo-flashjacks/) by Aquijacks
* [Carnivore2](https://sysadminmosaic.ru/en/msx/carnivore2/carnivore2?redirect=1#sound_settings) by RBSC
