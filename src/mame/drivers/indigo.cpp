// license:BSD-3-Clause
// copyright-holders:Ryan Holtz
/*********************************************************************\
*
*   SGI Indigo workstation
*
*  Skeleton Driver
*
*  Todo: Everything
*
*  Note: Machine uses R4400, not R4600
*
*  Memory map:
*
*  1fa00000 - 1fa02047      Memory Controller
*  1fb80000 - 1fb9a7ff      HPC1 CHIP0
*  1fc00000 - 1fc7ffff      BIOS
*
\*********************************************************************/

#include "emu.h"
#include "bus/scsi/scsi.h"
#include "bus/scsi/scsicd.h"
#include "cpu/mips/mips3.h"
#include "cpu/mips/r3000.h"
#include "machine/z80scc.h"
#include "machine/eepromser.h"
#include "machine/sgi.h"
#include "machine/wd33c93.h"
#include "machine/dp8573a.h"
#include "machine/clock.h"

#include "emupal.h"
#include "screen.h"
#include "speaker.h"

class indigo_state : public driver_device
{
public:
	indigo_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag)
		, m_maincpu(*this, "maincpu")
		, m_wd33c93(*this, "wd33c93")
		, m_scc0(*this, "scc0")
		, m_scc1(*this, "scc1")
		, m_scc2(*this, "scc2")
		, m_rtc(*this, "dp8573a")
		, m_eeprom(*this, "eeprom")
		, m_screen(*this, "screen")
		, m_palette(*this, "palette")
	{
	}

	void indigo4k(machine_config &config);
	void indigo3k(machine_config &config);

private:
	DECLARE_WRITE_LINE_MEMBER(scsi_irq);
	DECLARE_READ8_MEMBER  (eeprom_r);
	DECLARE_WRITE8_MEMBER (eeprom_w);
	DECLARE_READ32_MEMBER (misc_status_r);
	DECLARE_WRITE32_MEMBER(misc_status_w);

	virtual void machine_start() override;
	virtual void video_start() override;

	uint32_t screen_update(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);

	static void cdrom_config(device_t *device);
	void indigo3k_map(address_map &map);
	void indigo4k_map(address_map &map);
	void indigo_map(address_map &map);

	struct hpc_t
	{
		uint8_t m_misc_status;
		uint32_t m_parbuf_ptr;
		uint32_t m_local_ioreg0_mask;
		uint32_t m_local_ioreg1_mask;
		uint32_t m_vme_intmask0;
		uint32_t m_vme_intmask1;
		uint32_t m_scsi0_descriptor;
		uint32_t m_scsi0_dma_ctrl;
	};

	required_device<cpu_device> m_maincpu;
	required_device<wd33c93_device> m_wd33c93;
	required_device<scc8530_device> m_scc0, m_scc1, m_scc2;
	required_device<dp8573a_device> m_rtc;
	required_device<eeprom_serial_93cxx_device> m_eeprom;
	required_device<screen_device> m_screen;
	required_device<palette_device> m_palette;

	u32 m_misc_status;
	u8 m_eeprom_reg;

	hpc_t m_hpc;

	inline void ATTR_PRINTF(3,4) verboselog(int n_level, const char *s_fmt, ... );
};


#define VERBOSE_LEVEL (0)

inline void ATTR_PRINTF(3,4) indigo_state::verboselog(int n_level, const char *s_fmt, ... )
{
	if( VERBOSE_LEVEL >= n_level )
	{
		va_list v;
		char buf[ 32768 ];
		va_start( v, s_fmt );
		vsprintf( buf, s_fmt, v );
		va_end( v );
		logerror( "%08x: %s", m_maincpu->pc(), buf );
	}
}

void indigo_state::video_start()
{
}

uint32_t indigo_state::screen_update(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect)
{
	return 0;
}



#if 0
READ32_MEMBER(indigo_state::hpc_r)
{
	offset <<= 2;
	switch( offset )
	{
	case 0x05c:
		verboselog(2, "HPC Unknown Read: %08x (%08x) (returning 0x000000a5 as kludge)\n", 0x1fb80000 + offset, mem_mask );
		return 0x0000a500;
	case 0x00ac:
		verboselog(2, "HPC Parallel Buffer Pointer Read: %08x (%08x)\n", m_hpc.m_parbuf_ptr, mem_mask );
		return m_hpc.m_parbuf_ptr;
	case 0x01c4:
		verboselog(2, "HPC Local IO Register 0 Mask Read: %08x (%08x)\n", m_hpc.m_local_ioreg0_mask, mem_mask );
		return m_hpc.m_local_ioreg0_mask;
	case 0x01cc:
		verboselog(2, "HPC Local IO Register 1 Mask Read: %08x (%08x)\n", m_hpc.m_local_ioreg1_mask, mem_mask );
		return m_hpc.m_local_ioreg1_mask;
	case 0x01d4:
		verboselog(2, "HPC VME Interrupt Mask 0 Read: %08x (%08x)\n", m_hpc.m_vme_intmask0, mem_mask );
		return m_hpc.m_vme_intmask0;
	case 0x01d8:
		verboselog(2, "HPC VME Interrupt Mask 1 Read: %08x (%08x)\n", m_hpc.m_vme_intmask1, mem_mask );
		return m_hpc.m_vme_intmask1;
	case 0x0d20:
		verboselog(2, "HPC DUART2 Channel B Control Read\n" );
		return 0x00000004;
	case 0x0d24:
		verboselog(2, "HPC DUART2 Channel B Data Read\n" );
		return 0;
	case 0x0d28:
		verboselog(2, "HPC DUART2 Channel A Control Read\n" );
		return 0;
	case 0x0d2c:
		verboselog(2, "HPC DUART2 Channel A Data Read\n" );
		return 0;
	case 0x0d30:
		verboselog(2, "HPC DUART3 Channel B Control Read\n" );
		return 0x00000004;
	case 0x0d34:
		verboselog(2, "HPC DUART3 Channel B Data Read\n" );
		return 0;
	case 0x0d38:
		verboselog(2, "HPC DUART3 Channel A Control Read\n" );
		return 0;
	case 0x0d3c:
		verboselog(2, "HPC DUART3 Channel A Data Read\n" );
		return 0;
	}
	verboselog(0, "Unmapped HPC read: 0x%08x (%08x)\n", 0x1fb80000 + offset, mem_mask );
	return 0;
}

WRITE32_MEMBER(indigo_state::hpc_w)
{
	offset <<= 2;
	switch( offset )
	{
	case 0x0090:    // SCSI0 next descriptor pointer
		m_hpc.m_scsi0_descriptor = data;
		break;

	case 0x0094:    // SCSI0 control flags
		m_hpc.m_scsi0_dma_ctrl = data;
		#if 0
		if (data & 0x80)
		{
			uint32_t next;

			osd_printf_info("DMA activated for SCSI0\n");
			osd_printf_info("Descriptor block:\n");
			osd_printf_info("CTL: %08x BUFPTR: %08x DESCPTR %08x\n",
				program_read_dword(m_hpc.m_scsi0_descriptor), program_read_dword(m_hpc.m_scsi0_descriptor+4),
				program_read_dword(m_hpc.m_scsi0_descriptor+8));

			next = program_read_dword(m_hpc.m_scsi0_descriptor+8);
			osd_printf_info("CTL: %08x BUFPTR: %08x DESCPTR %08x\n",
				program_read_dword(next), program_read_dword(next+4),
				program_read_dword(next+8));
		}
		#endif
		break;

	case 0x00ac:
		verboselog(2, "HPC Parallel Buffer Pointer Write: %08x (%08x)\n", data, mem_mask );
		m_hpc.m_parbuf_ptr = data;
		break;
	case 0x0120:
		if (ACCESSING_BITS_8_15)
		{
			verboselog(2, "HPC SCSI Controller Register Write: %08x\n", ( data >> 8 ) & 0x000000ff );
			m_wd33c93->write( space, 0, ( data >> 8 ) & 0x000000ff );
		}
		else
		{
			return;
		}
		break;
	case 0x0124:
		if (ACCESSING_BITS_8_15)
		{
			verboselog(2, "HPC SCSI Controller Data Write: %08x\n", ( data >> 8 ) & 0x000000ff );
			m_wd33c93->write( space, 1, ( data >> 8 ) & 0x000000ff );
		}
		else
		{
			return;
		}
		break;
	case 0x01c4:
		verboselog(2, "HPC Local IO Register 0 Mask Write: %08x (%08x)\n", data, mem_mask );
		m_hpc.m_local_ioreg0_mask = data;
		break;
	case 0x01cc:
		verboselog(2, "HPC Local IO Register 1 Mask Write: %08x (%08x)\n", data, mem_mask );
		m_hpc.m_local_ioreg1_mask = data;
		break;
	case 0x01d4:
		verboselog(2, "HPC VME Interrupt Mask 0 Write: %08x (%08x)\n", data, mem_mask );
		m_hpc.m_vme_intmask0 = data;
		break;
	case 0x01d8:
		verboselog(2, "HPC VME Interrupt Mask 1 Write: %08x (%08x)\n", data, mem_mask );
		m_hpc.m_vme_intmask1 = data;
		break;
	case 0x0d30:
		osd_printf_info("HPC DUART3 Channel B Control Write: %08x (%08x)\n", data, mem_mask );
		break;
	case 0x0d34:
		verboselog(2, "HPC DUART3 Channel B Data Write: %08x (%08x)\n", data, mem_mask );
		break;
	case 0x0d38:
		osd_printf_info("HPC DUART3 Channel A Control Write: %08x (%08x)\n", data, mem_mask );
		break;
	case 0x0d3c:
		verboselog(2, "HPC DUART3 Channel A Data Write: %08x (%08x)\n", data, mem_mask );
		break;
	default:
		osd_printf_info("Unmapped HPC write: 0x%08x (%08x): %08x\n", 0x1fb80000 + offset, mem_mask, data);
		break;
	}
}

// INT/INT2/INT3 interrupt controllers
READ32_MEMBER(indigo_state::int_r)
{
	osd_printf_info("INT: read @ ofs %x (mask %x) (PC=%x)\n", offset, mem_mask, m_maincpu->pc());
	return 0;
}

WRITE32_MEMBER(indigo_state::int_w)
{
	osd_printf_info("INT: write %x to ofs %x (mask %x) (PC=%x)\n", data, offset, mem_mask, m_maincpu->pc());
}
#endif

READ8_MEMBER(indigo_state::eeprom_r)
{
	return (m_eeprom_reg & 0xef) | (m_eeprom->do_read() << 4);
}

WRITE8_MEMBER(indigo_state::eeprom_w)
{
	if((data ^ m_eeprom_reg) & 1)
		logerror("CPU led %s\n", data & 1 ? "on" : "off");
	m_eeprom_reg = data;
	m_eeprom->di_write(m_eeprom_reg & 8 ? 1 : 0 );
	m_eeprom->cs_write(m_eeprom_reg & 2 ? CLEAR_LINE : ASSERT_LINE );
	m_eeprom->clk_write(m_eeprom_reg & 4 ? CLEAR_LINE : ASSERT_LINE );	
}

READ32_MEMBER(indigo_state::misc_status_r)
{
	return m_misc_status;
}

WRITE32_MEMBER(indigo_state::misc_status_w)
{
	COMBINE_DATA(&m_misc_status);
	logerror("Misc status: sram %dK irqa %s%s dsp %s\n",
			 m_misc_status & 8 ? 32 : 8,
			 m_misc_status & 4 ? "high" : "low",
			 m_misc_status & 2 ? " forced" : "",
			 m_misc_status & 1 ? "reset" : "normal");
}

/*
  bfc02c60:
  read   5f, drop it
  write  5b, 0x5a
  read  e57 to r16
  write e57, 0xa5
  read  e5b to r4
  write e5b, 0x5a
  read  e57, check it's a5 (RTC ram) (r15 |= 4)
  read  e5b, check it's 5a (RTC ram)
  write e57, r16
  write e5b, r4

  write 1c7, 0xa5
  write 1db, 0x5a
  read  1c7, check it's a5 (r15 |= 8)
  read  1db, check it's 5a (r15 |= 8)
  write 1c7, 0x00
  write 1db, 0x00

  if r15 != 0 "Data path test failed"

  bfc00730: Ram test, including size

*/


void indigo_state::indigo_map(address_map &map)
{
	map(0x00000000, 0x00ffffff).ram();
	map(0x1fb800c0, 0x1fb800c3).lr32("endianness", [](address_space &, offs_t, u32) -> u32 { return 0x40; }); // Rev 1, all big-endian (dsp, par, scsi, enet, cpu)
	map(0x1fb80120, 0x1fb80127).rw(m_wd33c93, FUNC(wd33c93_device::read), FUNC(wd33c93_device::write)).umask32(0x0000ff00);
	map(0x1fb801b0, 0x1fb801b3).rw(FUNC(indigo_state::misc_status_r), FUNC(indigo_state::misc_status_w));
	map(0x1fb801bf, 0x1fb801bf).rw(FUNC(indigo_state::eeprom_r), FUNC(indigo_state::eeprom_w));

	map(0x1fb80d03, 0x1fb80d03).rw(m_scc0, FUNC(scc8530_device::cb_r), FUNC(scc8530_device::cb_w));
	map(0x1fb80d07, 0x1fb80d07).rw(m_scc0, FUNC(scc8530_device::db_r), FUNC(scc8530_device::db_w));
	map(0x1fb80d0b, 0x1fb80d0b).rw(m_scc0, FUNC(scc8530_device::ca_r), FUNC(scc8530_device::ca_w));
	map(0x1fb80d0f, 0x1fb80d0f).rw(m_scc0, FUNC(scc8530_device::da_r), FUNC(scc8530_device::da_w));
	map(0x1fb80d13, 0x1fb80d13).rw(m_scc1, FUNC(scc8530_device::cb_r), FUNC(scc8530_device::cb_w));
	map(0x1fb80d17, 0x1fb80d17).rw(m_scc1, FUNC(scc8530_device::db_r), FUNC(scc8530_device::db_w));
	map(0x1fb80d1b, 0x1fb80d1b).rw(m_scc1, FUNC(scc8530_device::ca_r), FUNC(scc8530_device::ca_w));
	map(0x1fb80d1f, 0x1fb80d1f).rw(m_scc1, FUNC(scc8530_device::da_r), FUNC(scc8530_device::da_w));
	map(0x1fb80d23, 0x1fb80d23).rw(m_scc2, FUNC(scc8530_device::cb_r), FUNC(scc8530_device::cb_w));
	map(0x1fb80d27, 0x1fb80d27).rw(m_scc2, FUNC(scc8530_device::db_r), FUNC(scc8530_device::db_w));
	map(0x1fb80d2b, 0x1fb80d2b).rw(m_scc2, FUNC(scc8530_device::ca_r), FUNC(scc8530_device::ca_w));
	map(0x1fb80d2f, 0x1fb80d2f).rw(m_scc2, FUNC(scc8530_device::da_r), FUNC(scc8530_device::da_w));

	map(0x1fb80e00, 0x1fb80e7f).m(m_rtc, FUNC(dp8573a_device::map)).umask32(0x000000ff);

	//	map(0x1fb80000, 0x1fb8ffff).rw(FUNC(indigo_state::hpc_r), FUNC(indigo_state::hpc_w));
	//	map(0x1fbd9000, 0x1fbd903f).rw(FUNC(indigo_state::int_r), FUNC(indigo_state::int_w));


	map(0x1fb801c7, 0x1fb801c7).lr8("hack3", [](address_space &, offs_t, u8) -> u8 { return 0xa5; });
	map(0x1fb801db, 0x1fb801db).lr8("hack4", [](address_space &, offs_t, u8) -> u8 { return 0x5a; });
}

void indigo_state::indigo3k_map(address_map &map)
{
	indigo_map(map);
	map(0x1fc00000, 0x1fc3ffff).rom().share("share2").region("user1", 0);
}

void indigo_state::indigo4k_map(address_map &map)
{
	indigo_map(map);
	map(0x1fa00000, 0x1fa1ffff).rw("sgi_mc", FUNC(sgi_mc_device::read), FUNC(sgi_mc_device::write));
	map(0x1fc00000, 0x1fc7ffff).rom().share("share2").region("user1", 0);
}

WRITE_LINE_MEMBER(indigo_state::scsi_irq)
{
}

void indigo_state::machine_start()
{
	m_hpc.m_misc_status = 0;
	m_hpc.m_parbuf_ptr = 0;
	m_hpc.m_local_ioreg0_mask = 0;
	m_hpc.m_local_ioreg1_mask = 0;
	m_hpc.m_vme_intmask0 = 0;
	m_hpc.m_vme_intmask1 = 0;

	m_misc_status = 0x00000000;
	m_eeprom_reg = 0x00;
	save_item(NAME(m_misc_status));
	save_item(NAME(m_eeprom_reg));
}

static INPUT_PORTS_START( indigo )
	PORT_START("unused")
	PORT_BIT ( 0xff, IP_ACTIVE_HIGH, IPT_UNUSED )
INPUT_PORTS_END

void indigo_state::cdrom_config(device_t *device)
{
	device->subdevice<cdda_device>("cdda")->add_route(ALL_OUTPUTS, "^^mono", 1.0);
}

void indigo_state::indigo3k(machine_config &config)
{
	auto &cpu(R3041(config, m_maincpu, 33000000));
	cpu.set_endianness(ENDIANNESS_BIG);
	cpu.set_addrmap(AS_PROGRAM, &indigo_state::indigo3k_map);

	SCREEN(config, m_screen, SCREEN_TYPE_RASTER);
	m_screen->set_refresh(HZ_TO_ATTOSECONDS(60));
	m_screen->set_vblank_time(ATTOSECONDS_IN_USEC(2500));
	m_screen->set_size(800, 600);
	m_screen->set_visarea(0, 799, 0, 599);
	m_screen->set_screen_update(FUNC(indigo_state::screen_update));
	m_screen->set_palette(m_palette);

	PALETTE(config, m_palette, 65536);

	SPEAKER(config, "mono").front_center();

	DP8573A(config, m_rtc);

	SCC8530N(config, m_scc0, 3672000); // Actual clock unknown
	SCC8530N(config, m_scc1, 3672000);

	auto &brg(CLOCK(config, "brg", 3672000));
	brg.signal_handler().set   (m_scc0, FUNC(scc8530_device::rxca_w));
	brg.signal_handler().append(m_scc0, FUNC(scc8530_device::txca_w));
	brg.signal_handler().append(m_scc0, FUNC(scc8530_device::rxcb_w));
	brg.signal_handler().append(m_scc0, FUNC(scc8530_device::txcb_w));
	brg.signal_handler().append(m_scc1, FUNC(scc8530_device::rxca_w));
	brg.signal_handler().append(m_scc1, FUNC(scc8530_device::txca_w));
	brg.signal_handler().append(m_scc1, FUNC(scc8530_device::rxcb_w));
	brg.signal_handler().append(m_scc1, FUNC(scc8530_device::txcb_w));

	auto &scsi_port(SCSI_PORT(config, "scsi"));
	auto &cdrom(*scsi_port.subdevice<scsi_port_slot_device>("1"));
	cdrom.option_add("cdrom", SCSICD);
	cdrom.set_default_option("cdrom");
	cdrom.set_option_device_input_defaults("cdrom", DEVICE_INPUT_DEFAULTS_NAME(SCSI_ID_4));
	cdrom.set_option_machine_config("cdrom", cdrom_config);

	WD33C93(config, m_wd33c93, 10000000);
	m_wd33c93->set_scsi_port(scsi_port);
	m_wd33c93->set_irq_callback(DEVCB_WRITELINE(*this, indigo_state, scsi_irq));      /* command completion IRQ */

	EEPROM_SERIAL_93C56_16BIT(config, m_eeprom);
}

void indigo_state::indigo4k(machine_config &config)
{
	indigo3k(config);
	auto &cpu(R4600BE(config.replace(), m_maincpu, 150000000)); // Should be R4400
	cpu.set_icache_size(32768);
	cpu.set_dcache_size(32768);
	cpu.set_addrmap(AS_PROGRAM, &indigo_state::indigo4k_map);

	SGI_MC(config, "sgi_mc", 0);
}


ROM_START( indigo3k )
	ROM_REGION32_BE( 0x40000, "user1", 0 )
	ROM_SYSTEM_BIOS( 0, "401-rev-c", "SGI Version 4.0.1 Rev C LG1/GR2, Jul 9, 1992" ) // dumped over serial connection from boot monitor and swapped
	ROMX_LOAD( "ip12prom.070-8088-xxx.u56", 0x000000, 0x040000, CRC(25ca912f) SHA1(94b3753d659bfe50b914445cef41290122f43880), ROM_GROUPWORD | ROM_REVERSE | ROM_BIOS(0) )
	ROM_SYSTEM_BIOS( 1, "401-rev-d", "SGI Version 4.0.1 Rev D LG1/GR2, Mar 24, 1992" ) // dumped with EPROM programmer
	ROMX_LOAD( "ip12prom.070-8088-002.u56", 0x000000, 0x040000, CRC(ea4329ef) SHA1(b7d67d0e30ae8836892f7170dd4757732a0a3fd6), ROM_GROUPWORD | ROM_REVERSE | ROM_BIOS(1) )
ROM_END

ROM_START( indigo4k )
	ROM_REGION32_BE( 0x80000, "user1", 0 )
	ROMX_LOAD( "ip20prom.070-8116-004.bin", 0x000000, 0x080000, CRC(940d960e) SHA1(596aba530b53a147985ff3f6f853471ce48c866c), ROM_GROUPDWORD | ROM_REVERSE )
ROM_END

//    YEAR  NAME      PARENT  COMPAT  MACHINE   INPUT   CLASS         INIT        COMPANY                 FULLNAME                                          FLAGS
COMP( 1991, indigo3k, 0,      0,      indigo3k, indigo, indigo_state, empty_init, "Silicon Graphics Inc", "IRIS Indigo (R3000, 33MHz)",                     MACHINE_NOT_WORKING | MACHINE_NO_SOUND )
COMP( 1993, indigo4k, 0,      0,      indigo4k, indigo, indigo_state, empty_init, "Silicon Graphics Inc", "IRIS Indigo (R4400, 150MHz, Ver. 4.0.5D Rev A)", MACHINE_NOT_WORKING | MACHINE_NO_SOUND )
