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
#include "cpu/mips/mips3.h"
#include "cpu/mips/r3000.h"
#include "machine/z80scc.h"
#include "machine/eepromser.h"
#include "machine/sgi.h"
#include "machine/wd33c93.h"
#include "machine/dp8573a.h"
#include "machine/clock.h"
#include "machine/pit8253.h"
#include "machine/nscsi_bus.h"
#include "machine/nscsi_hd.h"
#include "machine/nscsi_cd.h"

#include "emupal.h"
#include "screen.h"
#include "speaker.h"

class indigo_state : public driver_device
{
public:
	indigo_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag)
		, m_maincpu(*this, "maincpu")
		, m_wd33c93(*this, "scsibus:0:wd33c93")
		, m_scc0(*this, "scc0")
		, m_scc1(*this, "scc1")
		, m_rtc(*this, "dp8573a")
		, m_pit(*this, "pit")
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
	DECLARE_READ8_MEMBER  (isr0_r);
	DECLARE_WRITE8_MEMBER (isr0_w);
	DECLARE_READ8_MEMBER  (isr1_r);
	DECLARE_WRITE8_MEMBER (isr1_w);
	DECLARE_READ8_MEMBER  (imr0_r);
	DECLARE_WRITE8_MEMBER (imr0_w);
	DECLARE_READ8_MEMBER  (imr1_r);
	DECLARE_WRITE8_MEMBER (imr1_w);
	DECLARE_READ8_MEMBER  (vmeisr_r);
	DECLARE_WRITE8_MEMBER (vmeisr_w);
	DECLARE_READ8_MEMBER  (vmeimr0_r);
	DECLARE_WRITE8_MEMBER (vmeimr0_w);
	DECLARE_READ8_MEMBER  (vmeimr1_r);
	DECLARE_WRITE8_MEMBER (vmeimr1_w);

	DECLARE_READ32_MEMBER (scsi0_bc_r);
	DECLARE_WRITE32_MEMBER(scsi0_bc_w);
	DECLARE_READ32_MEMBER (scsi0_cbp_r);
	DECLARE_WRITE32_MEMBER(scsi0_cbp_w);
	DECLARE_READ32_MEMBER (scsi0_nbdp_r);
	DECLARE_WRITE32_MEMBER(scsi0_ndbp_w);
	DECLARE_READ32_MEMBER (scsi0_ctrl_r);
	DECLARE_WRITE32_MEMBER(scsi0_ctrl_w);
	DECLARE_READ32_MEMBER (scsi0_pntr_r);
	DECLARE_WRITE32_MEMBER(scsi0_pntr_w);
	DECLARE_READ32_MEMBER (scsi0_fifo_r);
	DECLARE_WRITE32_MEMBER(scsi0_fifo_w);

	virtual void machine_start() override;
	virtual void video_start() override;

	uint32_t screen_update(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);

	void wd33c93(device_t *device);
	static void cdrom_config(device_t *device);
	device_slot_interface &scsi_devices(device_slot_interface &device);

	void indigo3k_map(address_map &map);
	void indigo4k_map(address_map &map);
	void indigo_map(address_map &map);


	required_device<cpu_device> m_maincpu;
	required_device<wd33c93_device> m_wd33c93;
	required_device<scc8530_device> m_scc0, m_scc1;
	required_device<dp8573a_device> m_rtc;
	required_device<pit8254_device> m_pit;
	required_device<eeprom_serial_93cxx_device> m_eeprom;
	required_device<screen_device> m_screen;
	required_device<palette_device> m_palette;

	memory_access_cache<2, 0, ENDIANNESS_BIG> *m_scsi0_cache;

	u32 m_misc_status;
	u32 m_scsi0_bc, m_scsi0_cbp, m_scsi0_nbdp, m_scsi0_ctrl;
	u32 m_scsi0_pntr;

	u16 m_isr, m_imr, m_vmeimr;
	u8 m_vmeisr;
	u8 m_eeprom_reg;
	u8 m_scsi0_fifo[64];

	void scsi0_dma_run();
	u8 scsi0_dma_fifo_r();
	void scsi0_dma_fifo_w(u8 data);
	void scsi0_irq(int state);
	void scsi0_drq(int state);
};

void indigo_state::video_start()
{
}

uint32_t indigo_state::screen_update(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect)
{
	return 0;
}

READ8_MEMBER(indigo_state::eeprom_r)
{
#if 0
	attotime t = machine().time();
	int secs = t.m_seconds;
	t.m_seconds = 0;
	int nsecs = t.as_ticks(1000000000);

	logerror("cs %d clk %d di %d do %d time %6d.%03d-%03d-%03d\n", m_eeprom_reg & 2 ? ASSERT_LINE : CLEAR_LINE, m_eeprom_reg & 4 ? CLEAR_LINE : ASSERT_LINE, m_eeprom_reg & 8 ? 1 : 0, m_eeprom->do_read(),
			 secs, nsecs/1000000, (nsecs/1000) % 1000, nsecs % 1000);
#endif

	return (m_eeprom_reg & 0xef) | (m_eeprom->do_read() << 4);
}

WRITE8_MEMBER(indigo_state::eeprom_w)
{
	if(false && ((data ^ m_eeprom_reg) & 1))
		logerror("CPU led %s\n", data & 1 ? "on" : "off");
	m_eeprom_reg = data;
	//	logerror("cs %d clk %d di %d\n", m_eeprom_reg & 2 ? ASSERT_LINE : CLEAR_LINE, m_eeprom_reg & 4 ? CLEAR_LINE : ASSERT_LINE, m_eeprom_reg & 8 ? 1 : 0);
	m_eeprom->di_write(m_eeprom_reg & 8 ? 1 : 0 );
	m_eeprom->cs_write(m_eeprom_reg & 2 ? ASSERT_LINE : CLEAR_LINE );
	m_eeprom->clk_write(m_eeprom_reg & 4 ? CLEAR_LINE : ASSERT_LINE );	
}

READ32_MEMBER(indigo_state::misc_status_r)
{
	return m_misc_status;
}

WRITE32_MEMBER(indigo_state::misc_status_w)
{
	COMBINE_DATA(&m_misc_status);
	logerror("Misc status: %ssram %dK irqa %s%s dsp %s\n",
			 (m_misc_status & 0x70) == 0x70 ? "pbus-gio 8->32 " : (m_misc_status & 0x70) == 0x60 ? "pbus-gio 16->32 " : (m_misc_status & 0x70) == 0x40 ? "pbus-gio 24->32 " : "",
			 m_misc_status & 8 ? 32 : 8,
			 m_misc_status & 4 ? "high" : "low",
			 m_misc_status & 2 ? " forced" : "",
			 m_misc_status & 1 ? "reset" : "normal");
}

READ8_MEMBER(indigo_state::isr0_r)
{
	return m_isr;
}

WRITE8_MEMBER(indigo_state::isr0_w)
{
	logerror("isr0_w %02x\n", data);
}

READ8_MEMBER(indigo_state::isr1_r)
{
	return m_isr >> 8;
}

WRITE8_MEMBER(indigo_state::isr1_w)
{
	logerror("isr1_w %02x\n", data);
}

READ8_MEMBER(indigo_state::imr0_r)
{
	return m_imr;
}

WRITE8_MEMBER(indigo_state::imr0_w)
{
	m_imr = (m_imr & 0xff00) | data;
	logerror("imr_w %04x\n", m_imr);
}

READ8_MEMBER(indigo_state::imr1_r)
{
	return m_imr >> 8;
}

WRITE8_MEMBER(indigo_state::imr1_w)
{
	m_imr = (m_imr & 0x00ff) | (data << 8);
	logerror("imr_w %04x\n", m_imr);
}

READ8_MEMBER(indigo_state::vmeisr_r)
{
	return m_vmeisr;
}

WRITE8_MEMBER(indigo_state::vmeisr_w)
{
	logerror("vmeisr_w %02x\n", data);
}

READ8_MEMBER(indigo_state::vmeimr0_r)
{
	return m_vmeimr;
}

WRITE8_MEMBER(indigo_state::vmeimr0_w)
{
	m_vmeimr = (m_vmeimr & 0xff00) | data;
	logerror("vmeimr_w %04x\n", m_vmeimr);
}

READ8_MEMBER(indigo_state::vmeimr1_r)
{
	return m_vmeimr >> 8;
}

WRITE8_MEMBER(indigo_state::vmeimr1_w)
{
	m_vmeimr = (m_vmeimr & 0x00ff) | (data << 8);
	logerror("vmeimr_w %04x\n", m_vmeimr);
}


READ32_MEMBER (indigo_state::scsi0_bc_r)
{
	return m_scsi0_bc;
}

WRITE32_MEMBER(indigo_state::scsi0_bc_w)
{
	COMBINE_DATA(&m_scsi0_bc);
	scsi0_dma_run();
}

READ32_MEMBER (indigo_state::scsi0_cbp_r)
{
	return m_scsi0_cbp;
}

WRITE32_MEMBER(indigo_state::scsi0_cbp_w)
{
	COMBINE_DATA(&m_scsi0_cbp);
	scsi0_dma_run();
}

READ32_MEMBER (indigo_state::scsi0_nbdp_r)
{
	return m_scsi0_nbdp;
}

WRITE32_MEMBER(indigo_state::scsi0_ndbp_w)
{
	COMBINE_DATA(&m_scsi0_nbdp);
}

READ32_MEMBER (indigo_state::scsi0_ctrl_r)
{
	return m_scsi0_ctrl;
}

WRITE32_MEMBER(indigo_state::scsi0_ctrl_w)
{
	u32 ctrl = m_scsi0_ctrl;
	COMBINE_DATA(&m_scsi0_ctrl);
	if(!(ctrl & 0x80) && (m_scsi0_ctrl & 0x80))
		m_scsi0_cbp &= 0x7fffffff;
	scsi0_dma_run();
}

READ32_MEMBER (indigo_state::scsi0_pntr_r)
{
	return m_scsi0_pntr;
}

WRITE32_MEMBER(indigo_state::scsi0_pntr_w)
{
	COMBINE_DATA(&m_scsi0_pntr);
	scsi0_dma_run();
}

READ32_MEMBER (indigo_state::scsi0_fifo_r)
{
	return scsi0_dma_fifo_r();
}

WRITE32_MEMBER(indigo_state::scsi0_fifo_w)
{
	scsi0_dma_fifo_w(data);
}

u8 indigo_state::scsi0_dma_fifo_r()
{
	if(m_scsi0_pntr) {
		u8 res = m_scsi0_fifo[0];
		std::copy(&m_scsi0_fifo[1], std::end(m_scsi0_fifo), &m_scsi0_fifo[0]);
		m_scsi0_pntr --;
		return res;
	}
	return 0;
}

void indigo_state::scsi0_dma_fifo_w(u8 data)
{
	if(m_scsi0_pntr < 64)
		m_scsi0_fifo[m_scsi0_pntr++] = data;
}

void indigo_state::scsi0_dma_run()
{
	if(m_scsi0_ctrl & 0x01) {
		m_wd33c93->reset();
		m_scsi0_ctrl &= 0x01;
		m_scsi0_pntr = 0;
		m_scsi0_bc = 0;
		m_scsi0_cbp = 0x80000000;
		m_scsi0_nbdp = 0;
		return;
	}

	if(m_scsi0_ctrl & 0x02) {
		m_scsi0_pntr = 0;
		m_scsi0_ctrl &= 0x7f;
		return;
	}

	if(!(m_scsi0_ctrl & 0x80))
		return;

}

void indigo_state::indigo_map(address_map &map)
{
	map(0x00000000, 0x05ffffff).ram();

	map(0x1fb80088, 0x1fb8008b).rw(FUNC(indigo_state::scsi0_bc_r), FUNC(indigo_state::scsi0_bc_w));
	map(0x1fb8008c, 0x1fb8008f).rw(FUNC(indigo_state::scsi0_cbp_r), FUNC(indigo_state::scsi0_cbp_w));
	map(0x1fb80090, 0x1fb80093).rw(FUNC(indigo_state::scsi0_nbdp_r), FUNC(indigo_state::scsi0_ndbp_w));
	map(0x1fb80094, 0x1fb80097).rw(FUNC(indigo_state::scsi0_ctrl_r), FUNC(indigo_state::scsi0_ctrl_w));
	map(0x1fb80098, 0x1fb8009b).rw(FUNC(indigo_state::scsi0_pntr_r), FUNC(indigo_state::scsi0_pntr_w));
	map(0x1fb8009c, 0x1fb8009f).rw(FUNC(indigo_state::scsi0_fifo_r), FUNC(indigo_state::scsi0_fifo_w));

	map(0x1fb800c0, 0x1fb800c3).lr32("endianness", [](address_space &, offs_t, u32) -> u32 { return 0x40; }).nopw(); // Rev 1, all big-endian (dsp, par, scsi, enet, cpu)
	map(0x1fb80120, 0x1fb80127).m(m_wd33c93, FUNC(wd33c93_device::map)).umask32(0x0000ff00);
	map(0x1fb801b0, 0x1fb801b3).rw(FUNC(indigo_state::misc_status_r), FUNC(indigo_state::misc_status_w));
	map(0x1fb801bf, 0x1fb801bf).rw(FUNC(indigo_state::eeprom_r), FUNC(indigo_state::eeprom_w));
	map(0x1fb801c3, 0x1fb801c3).rw(FUNC(indigo_state::isr0_r), FUNC(indigo_state::isr0_w));
	map(0x1fb801c7, 0x1fb801c7).rw(FUNC(indigo_state::imr0_r), FUNC(indigo_state::imr0_w));
	map(0x1fb801cb, 0x1fb801cb).rw(FUNC(indigo_state::isr1_r), FUNC(indigo_state::isr1_w));
	map(0x1fb801cf, 0x1fb801cf).rw(FUNC(indigo_state::imr0_r), FUNC(indigo_state::imr0_w));
	map(0x1fb801d3, 0x1fb801d3).rw(FUNC(indigo_state::vmeisr_r), FUNC(indigo_state::vmeisr_w));
	map(0x1fb801d7, 0x1fb801d7).rw(FUNC(indigo_state::vmeimr0_r), FUNC(indigo_state::vmeimr0_w));
	map(0x1fb801db, 0x1fb801db).rw(FUNC(indigo_state::vmeimr1_r), FUNC(indigo_state::vmeimr1_w));
	map(0x1fb801f0, 0x1fb801ff).rw(m_pit, FUNC(pit8254_device::read), FUNC(pit8254_device::write)).umask32(0x000000ff);

	map(0x1fb80d03, 0x1fb80d03).rw(m_scc0, FUNC(scc8530_device::cb_r), FUNC(scc8530_device::cb_w));
	map(0x1fb80d07, 0x1fb80d07).rw(m_scc0, FUNC(scc8530_device::db_r), FUNC(scc8530_device::db_w));
	map(0x1fb80d0b, 0x1fb80d0b).rw(m_scc0, FUNC(scc8530_device::ca_r), FUNC(scc8530_device::ca_w));
	map(0x1fb80d0f, 0x1fb80d0f).rw(m_scc0, FUNC(scc8530_device::da_r), FUNC(scc8530_device::da_w));
	map(0x1fb80d13, 0x1fb80d13).rw(m_scc1, FUNC(scc8530_device::cb_r), FUNC(scc8530_device::cb_w));
	map(0x1fb80d17, 0x1fb80d17).rw(m_scc1, FUNC(scc8530_device::db_r), FUNC(scc8530_device::db_w));
	map(0x1fb80d1b, 0x1fb80d1b).rw(m_scc1, FUNC(scc8530_device::ca_r), FUNC(scc8530_device::ca_w));
	map(0x1fb80d1f, 0x1fb80d1f).rw(m_scc1, FUNC(scc8530_device::da_r), FUNC(scc8530_device::da_w));

	map(0x1fb80e00, 0x1fb80e7f).m(m_rtc, FUNC(dp8573a_device::map)).umask32(0x000000ff);

	map(0x1fbd0000, 0x1fbd0003).lr32("revision", [](address_space &, offs_t, u32) -> u32 { return 0x2000; });

	map(0x1fbe0000, 0x1fbfffff).ram(); // DSP ram, 32k*24 bits

	//	map(0x1fbd9000, 0x1fbd903f).rw(FUNC(indigo_state::int_r), FUNC(indigo_state::int_w));
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
	m_misc_status = 0x00000000;
	m_eeprom_reg = 0x00;
	m_isr = 0;
	m_imr = 0;
	m_vmeisr = 0;
	m_vmeimr = 0;

	m_scsi0_bc = 0;
	m_scsi0_cbp = 0x80000000;
	m_scsi0_nbdp = 0;
	m_scsi0_ctrl = 0;
	m_scsi0_pntr = 0;
	std::fill(std::begin(m_scsi0_fifo), std::end(m_scsi0_fifo), 0);

	m_scsi0_cache = m_maincpu->space(AS_PROGRAM).cache<2, 0, ENDIANNESS_BIG>();

	save_item(NAME(m_misc_status));
	save_item(NAME(m_eeprom_reg));
	save_item(NAME(m_isr));
	save_item(NAME(m_imr));
	save_item(NAME(m_vmeisr));
	save_item(NAME(m_vmeimr));
	save_item(NAME(m_scsi0_bc));
	save_item(NAME(m_scsi0_cbp));
	save_item(NAME(m_scsi0_nbdp));
	save_item(NAME(m_scsi0_ctrl));
	save_item(NAME(m_scsi0_pntr));
	save_item(NAME(m_scsi0_fifo));
}

static INPUT_PORTS_START( indigo )
	PORT_START("unused")
	PORT_BIT ( 0xff, IP_ACTIVE_HIGH, IPT_UNUSED )
INPUT_PORTS_END

device_slot_interface &indigo_state::scsi_devices(device_slot_interface &device)
{
	device.option_add("cdrom", NSCSI_CDROM);
	device.option_add("harddisk", NSCSI_HARDDISK);
	device.set_option_machine_config("cdrom", cdrom_config);
	return device;
}

void indigo_state::cdrom_config(device_t *device)
{
#if 0 // cdda not in nscsi_cd yet
	device->subdevice<cdda_device>("cdda")->add_route(ALL_OUTPUTS, "^^mono", 1.0);
#endif
}

void indigo_state::wd33c93(device_t *device)
{
	auto wd = downcast<wd33c93_device *>(device);
	wd->irq_cb().set(":", FUNC(indigo_state::scsi0_irq));
	wd->drq_cb().set(":", FUNC(indigo_state::scsi0_drq));
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

	NSCSI_BUS(config, "scsibus");

	auto &id0(NSCSI_CONNECTOR(config, "scsibus:0"));
	id0.option_add_internal("wd33c93", WD33C93);
	id0.set_default_option("wd33c93");
	id0.set_fixed(true);

	scsi_devices(NSCSI_CONNECTOR(config, "scsibus:1")).set_default_option("harddisk");
	scsi_devices(NSCSI_CONNECTOR(config, "scsibus:2"));
	scsi_devices(NSCSI_CONNECTOR(config, "scsibus:3"));
	scsi_devices(NSCSI_CONNECTOR(config, "scsibus:4")).set_default_option("cdrom");
	scsi_devices(NSCSI_CONNECTOR(config, "scsibus:5"));
	scsi_devices(NSCSI_CONNECTOR(config, "scsibus:6"));
	scsi_devices(NSCSI_CONNECTOR(config, "scsibus:7"));

	EEPROM_SERIAL_93C56_16BIT(config, m_eeprom);

	PIT8254(config, m_pit, 0);
	m_pit->set_clk<2>(1000000);
	m_pit->out_handler<2>().set(m_pit, FUNC(pit8253_device::write_clk0));
	m_pit->out_handler<2>().append(m_pit, FUNC(pit8253_device::write_clk1));
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
