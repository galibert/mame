/***************************************************************************

    esqmr.c - Ensoniq MR-61, MR-76, and MR-Rack

    Skeleton driver by R. Belmont

    Hardware:
        CPU: 68340 MCU
        Sound: 2xES5506
        Effects: ES5511

    Memory map:

    0x000000-0x0FFFFF   OS ROM
 
    MR Rack
	Ensoniq, 1995

	This is a 64-voice expandable synth module made by Ensoniq in 1995.

	PCB Layout
	----------

	|--------------------------------------------------------|
	| J6  J7          J12  J13               J21   J23    J22|
	|                                                      J5|
	|       4565  4565                          HP_6N138     |
	|                                                        |
	|                                      BATTERY   7407    |
	|     4565                                               |
	|                                                        |
	|          OTTOR2  OTTOR2                                |
	|AD1861                                                  |
	|AD1861                                                  |
	|AD1861                                                  |
	|AD1861                                    |--J3-----|   |
	| HC04                                     |EXPANSION|   |
	|                                          |BOARD #3 |   |
	|                                          |         |   |
	|            1370001501                    |         |   |
	|                                          |         |   |
	|                                          |--J8-----|   |
	|     ESP2                         ROM2                  |
	|                                          |--J2-----|   |
	|                                          |EXPANSION|   |
	|                                  ROM0    |BOARD #2 |   |
	|                                          |         |   |
	| D43256               IDT7130             |         |   |
	| D43256  35MHz  16MHz             ROM1    |         |   |
	| D43256               J14                 |--J9-----|   |
	| D43256     22.5792MHz                            ADM691|
	| D43256             MC68340               |--J1-----|   |
	| D43256                                   |EXPANSION|   |
	|                    6MHz                  |BOARD #1 |   |
	|                          EPROM_UP        |         |   |
	|         MC68HC705C4A                KM681000       |   |
	|J4                        EPROM_LO   KM681000       |J11|
	|       J19        J18                     |--J10----|   |
	|--------------------------------------------------------|
	Notes:
		  J4/J18/J19   - Connectors to front panel buttons, LCD etc
		  J1/J10       - Connectors for expansion board #1
		  J2/J9        - Connectors for expansion board #2
		  J3/J8        - Connectors for expansion board #3
		  J11          - Memory card connector
		  J14          - JTAG connector
		  J6           - Main left/mono jack
		  J7           - Main right jack
		  J12          - Aux left/mono jack
		  J13          - Aux right jack
		  J21          - MIDI in connector
		  J23          - MIDI out connector
		  J22          - MIDI thru connector
		  J5           - Power input connector
		  1370001501   - Unknown TQFP144 IC stamped with the Ensoniq logo. CPLD or gate array.
		  ESP2         - Ensoniq ESP2 (ES5511) sound chip
		  OTTOR2       - Ensoniq OTTOR2 (ES5506) sound chip
		  ROM*         - 4M x8-bit SOP44 mask ROM
		  D43256       - NEC D43256 32k x8-bit SRAM
		  HP_6N138     - HP/Agilent HP 6N138 Low Input Current High Gain Optocoupler
		  4565         - JRC4565 Dual Operational Amplifier
		  AD1861       - Analog Devices AD1861 16-bit/18-bit PCM Audio DAC
		  MC68HC705C4A - Motorola MC68HC705C4A Microcontroller. Clock input is tied to the TQFP CPLD
		  MC68340      - Motorola MC68340PV16E 68000-compatible 32-bit processor with on-board peripherals. Clock input 6.000MHz
		  EPROM*       - 27C4001 EPROM
		  FDT7130      - IDT7130 High Speed 1k x8-bit Dual Port Static RAM
		  KM681000     - Samsung KM681000 128k x8-bit Static RAM
 
***************************************************************************/

#include "emu.h"
#include "machine/68340.h"
#include "sound/es5506.h"

#include "machine/esqvfd.h"

class esqmr_state : public driver_device
{
public:
	esqmr_state(const machine_config &mconfig, device_type type, const char *tag)
	: driver_device(mconfig, type, tag),
		m_maincpu(*this, "maincpu"),
		m_sq1vfd(*this, "sq1vfd")
	{ }

	required_device<m68340cpu_device> m_maincpu;
	required_device<esq2x40_sq1_t> m_sq1vfd;

	virtual void machine_reset();

public:
	DECLARE_DRIVER_INIT(mr);
	DECLARE_WRITE_LINE_MEMBER(esq5506_otto_irq);
	DECLARE_READ16_MEMBER(esq5506_read_adc);	
};

void esqmr_state::machine_reset()
{
}

static ADDRESS_MAP_START( mr_map, AS_PROGRAM, 32, esqmr_state )
	AM_RANGE(0x00000000, 0x000fffff) AM_ROM AM_REGION("maincpu", 0)
//  AM_RANGE(0x200000, 0x20003f) AM_DEVREADWRITE8("ensoniq", es5506_device, read, write, 0xffffffff)
//  AM_RANGE(0x240000, 0x24003f) AM_DEVREADWRITE8("ensoniq2", es5506_device, read, write, 0xffffffff)
//    AM_RANGE(0xff0000, 0xffffff) AM_RAM AM_SHARE("osram")
ADDRESS_MAP_END

WRITE_LINE_MEMBER(esqmr_state::esq5506_otto_irq)
{
}

READ16_MEMBER(esqmr_state::esq5506_read_adc)
{
	return 0;
}

static const es5506_interface es5506_config =
{
	"waverom",  /* Bank 0 */
	"waverom2", /* Bank 1 */
	"waverom3", /* Bank 0 */
	"waverom4", /* Bank 1 */
	1,          /* channels */
	DEVCB_DRIVER_LINE_MEMBER(esqmr_state,esq5506_otto_irq), /* irq */
	DEVCB_DRIVER_MEMBER16(esqmr_state, esq5506_read_adc)
};

static const es5506_interface es5506_2_config =
{
	"waverom",  /* Bank 0 */
	"waverom2", /* Bank 1 */
	"waverom3", /* Bank 0 */
	"waverom4", /* Bank 1 */
	1,          /* channels */
	DEVCB_NULL,
	DEVCB_NULL
};

static MACHINE_CONFIG_START( mr, esqmr_state )
	MCFG_CPU_ADD("maincpu", M68340, XTAL_16MHz)
	MCFG_CPU_PROGRAM_MAP(mr_map)

	MCFG_ESQ2x40_SQ1_ADD("sq1vfd")

	MCFG_SPEAKER_STANDARD_STEREO("lspeaker", "rspeaker")
	MCFG_SOUND_ADD("ensoniq", ES5506, XTAL_16MHz)
	MCFG_SOUND_CONFIG(es5506_config)
	MCFG_SOUND_ROUTE(0, "lspeaker", 0.5)
	MCFG_SOUND_ROUTE(1, "rspeaker", 0.5)
	MCFG_SOUND_ADD("ensoniq2", ES5506, XTAL_16MHz)
	MCFG_SOUND_CONFIG(es5506_2_config)
	MCFG_SOUND_ROUTE(0, "lspeaker", 0.5)
	MCFG_SOUND_ROUTE(1, "rspeaker", 0.5)
MACHINE_CONFIG_END

static INPUT_PORTS_START( mr )
INPUT_PORTS_END

ROM_START( mr61 )
	ROM_REGION(0x100000, "maincpu", 0)
	ROM_LOAD16_WORD_SWAP( "mrw-osf-11af-2.10.bin",  0x000000, 0x080000, CRC(5854314e) SHA1(8fb2e2ee2f5fb12eae8ea33cb18f757efaec6780) )
	ROM_LOAD16_WORD_SWAP( "mrw-romc-32ef-1.20.bin", 0x080000, 0x080000, CRC(68321347) SHA1(56cb96943ba42c35ba2787a49b5f4adf7c8dffb8) )

	ROM_REGION(0x400000, "waverom", ROMREGION_ERASE00)

	ROM_REGION(0x400000, "waverom2", ROMREGION_ERASE00)

	ROM_REGION(0x400000, "waverom3", ROMREGION_ERASE00)

	ROM_REGION(0x200000, "waverom4", ROMREGION_ERASE00)
ROM_END

ROM_START( mrrack )
	// 68340 main MCU
	ROM_REGION(0x100000, "maincpu", 0)
	ROM_LOAD16_BYTE( "mr_r_ec51_lo_1.50.u36", 0x000001, 0x080000, CRC(b29988a1) SHA1(986c2def11de27fa2b9be55ac32f7fec0c414bca) ) 
	ROM_LOAD16_BYTE( "mr_r_9dac_up_1.50.u35", 0x000000, 0x080000, CRC(71511692) SHA1(54744f16f1db1ac5abb2f70b6e04aebf1e0e029d) ) 

	// 68705 display/front panel MCU
	ROM_REGION(0x2000, "mcu", 0)
	ROM_LOAD( "68hc705.u40",  0x000000, 0x002000, CRC(7b0291a7) SHA1(c92c19ce9289b7b21dbc915475cdff8930e3c677) ) 

	ROM_REGION(0x400000, "waverom", ROMREGION_ERASE00)
	ROM_LOAD( "1351000901_h-rom0.u5", 0x000000, 0x400000, CRC(89654b42) SHA1(4bdffd8060eb20cdb01f6178222aeb32fdbfd703) ) 

	ROM_REGION(0x400000, "waverom2", ROMREGION_ERASE00)
	ROM_LOAD( "1351000902_h-rom1.u23", 0x000000, 0x400000, CRC(4a19e517) SHA1(e819f1e0b50c4911c4855ad95ed505998a2bbe86) ) 

	ROM_REGION(0x400000, "waverom3", ROMREGION_ERASE00)
	ROM_LOAD( "1351000903_h-rom2.u24", 0x000000, 0x400000, CRC(c9ab1214) SHA1(92f48b068bbe49eacbffd03e428599e3ab21b8ec) ) 

	ROM_REGION(0x200000, "waverom4", ROMREGION_ERASE00)
ROM_END

DRIVER_INIT_MEMBER(esqmr_state, mr)
{
}

CONS( 1996, mr61,   0, 0, mr, mr, esqmr_state, mr, "Ensoniq", "MR-61 Workstation", GAME_NOT_WORKING )
CONS( 1996, mrrack, 0, 0, mr, mr, esqmr_state, mr, "Ensoniq", "MR-Rack", GAME_NOT_WORKING )
