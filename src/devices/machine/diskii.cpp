// license:BSD-3-Clause
// copyright-holders:Olivier Galibert
/*********************************************************************

    Implementation of the Apple Disk II floppy disk controller

*********************************************************************/

#include "emu.h"
#include "diskii.h"
#include "formats/ap2_dsk.h"

/***************************************************************************
    PARAMETERS
***************************************************************************/

//**************************************************************************
//  GLOBAL VARIABLES
//**************************************************************************

DEFINE_DEVICE_TYPE(DISKII, diskii_device, "diskii", "Apple Disk II floppy controller")

#define DISKII_P6_REGION  "diskii_rom_p6"

ROM_START( diskii )
	ROM_REGION(0x100, DISKII_P6_REGION, 0)
	ROM_LOAD( "341-0028-a.rom", 0x0000, 0x0100, CRC(b72a2c70) SHA1(bc39fbd5b9a8d2287ac5d0a42e639fc4d3c2f9d4)) /* 341-0028: 16-sector disk drive (older version), PROM P6 */
ROM_END

//-------------------------------------------------
//  rom_region - device-specific ROM region
//-------------------------------------------------

const tiny_rom_entry *diskii_device::device_rom_region() const
{
	return ROM_NAME( diskii );
}

//-------------------------------------------------
//  device_add_mconfig - add device configuration
//-------------------------------------------------

void diskii_device::device_add_mconfig(machine_config &config)
{
	F9334(config, m_phaselatch); // 9334 on circuit diagram but 74LS259 in parts list; actual chip may vary
	m_phaselatch->parallel_out_cb().set(FUNC(diskii_device::set_phase));
}

//**************************************************************************
//  LIVE DEVICE
//**************************************************************************

diskii_device::diskii_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock) :
	applefdc_device(mconfig, DISKII, tag, owner, clock),
	m_phaselatch(*this, "phaselatch")
{
}

//-------------------------------------------------
//  device_start - device-specific startup
//-------------------------------------------------

void diskii_device::device_start()
{
	applefdc_device::device_start();

	m_rom_p6 = machine().root_device().memregion(this->subtag(DISKII_P6_REGION).c_str())->base();

	m_timer = timer_alloc(0);
	m_delay_timer = timer_alloc(1);

	save_item(NAME(m_last_6502_write));
	save_item(NAME(m_mode_write));
	save_item(NAME(m_mode_load));
	save_item(NAME(m_active));
	save_item(NAME(m_external_io_select));
	save_item(NAME(m_cycles));
	save_item(NAME(m_data_reg));
	save_item(NAME(m_address));
	save_item(NAME(m_write_start_time));
	save_item(NAME(m_write_position));
	save_item(NAME(m_write_line_active));
}

void diskii_device::device_reset()
{
	applefdc_device::device_reset();

	m_floppy = nullptr;
	m_active = MODE_IDLE;
	m_mode_write = false;
	m_mode_load = false;
	m_last_6502_write = 0x00;
	m_cycles = time_to_cycles(machine().time());
	m_data_reg = 0x00;
	m_address = 0x00;
	m_write_start_time = attotime::never;
	m_write_position = 0;
	m_write_line_active = false;

	// Just a timer to be sure that the lss is updated from time to
	// time, so that there's no hiccup when it's talked to again.
	m_timer->adjust(attotime::from_msec(10), 0, attotime::from_msec(10));

	m_devsel_cb(1);
}

void diskii_device::device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr)
{
	if(m_active)
		lss_sync();

	if(id == 1 && m_active == MODE_DELAY) {
		if(m_floppy)
			m_floppy->mon_w(true);
		m_active = MODE_IDLE;
	}
}

void diskii_device::set_floppy(floppy_image_device *floppy)
{
	if(m_floppy == floppy)
		return;

	lss_sync();

	if(m_floppy)
		m_floppy->mon_w(true);
	m_floppy = floppy;
	if(m_active)
		m_floppy->mon_w(false);
	update_phases();
}

floppy_image_device *diskii_device::get_floppy() const
{
	return m_floppy;
}

/*-------------------------------------------------
    read - called to read the FDC's registers
-------------------------------------------------*/

uint8_t diskii_device::read(offs_t offset)
{
	lss_sync();
	control(offset);

	if(!(offset & 1))
		return m_data_reg;

	return 0xff;
}


/*-------------------------------------------------
    write - called to write the FDC's registers
-------------------------------------------------*/

void diskii_device::write(offs_t offset, uint8_t data)
{
	lss_sync();
	control(offset);
	m_last_6502_write = data;
}

WRITE8_MEMBER(diskii_device::set_phase)
{
	m_phases = 0xf0 | data;
	update_phases();
}

void diskii_device::control(int offset)
{
	if(offset < 8)
		m_phaselatch->write_bit(offset >> 1, offset & 1);

	else
		switch(offset) {
		case 0x8:
			if(m_active == MODE_ACTIVE) {
				m_delay_timer->adjust(attotime::from_seconds(1));
				m_active = MODE_DELAY;
			}
			break;
		case 0x9:
			switch(m_active) {
			case MODE_IDLE:
				if(m_floppy)
					m_floppy->mon_w(false);
				m_active = MODE_ACTIVE;
				if(m_floppy)
					lss_start();
				break;
			case MODE_DELAY:
				m_active = MODE_ACTIVE;
				m_delay_timer->adjust(attotime::never);
				break;
			}
			break;
		case 0xa:
			m_devsel_cb(1);
			break;
		case 0xb:
			m_devsel_cb(2);
			break;
		case 0xc:
			if(m_mode_load) {
				if(m_active)
					m_address &= ~0x04;
				m_mode_load = false;
			}
			break;
		case 0xd:
			if(!m_mode_load) {
				if(m_active)
					m_address |= 0x04;
				m_mode_load = true;
			}
			break;
		case 0xe:
			if(m_mode_write) {
				if(m_active)
					m_address &= ~0x08;
				m_mode_write = false;
				attotime now = machine().time();
				if(m_floppy)
					m_floppy->write_flux(m_write_start_time, now, m_write_position, m_write_buffer);
			}
			break;
		case 0xf:
			if(!m_mode_write) {
				if(m_active) {
					m_address |= 0x08;
					m_write_start_time = machine().time();
					m_write_position = 0;
					if(m_floppy)
						m_floppy->set_write_splice(m_write_start_time);
				}
				m_mode_write = true;
			}
			break;
		}
}

uint64_t diskii_device::time_to_cycles(const attotime &tm)
{
	// Clock is falling edges of the ~2Mhz clock
	// The 1021800 must be the controlling 6502's speed

	uint64_t cycles = tm.as_ticks(clock()*2);
	cycles = (cycles+1) >> 1;
	return cycles;
}

attotime diskii_device::cycles_to_time(uint64_t cycles)
{
	return attotime::from_ticks(cycles*2+1, clock()*2);
}

void diskii_device::lss_start()
{
	m_cycles = time_to_cycles(machine().time());
	m_data_reg = 0x00;
	m_address &= ~0x0e;
	m_write_position = 0;
	m_write_start_time = m_mode_write ? machine().time() : attotime::never;
	m_write_line_active = false;
	if(m_mode_write && m_floppy)
		m_floppy->set_write_splice(m_write_start_time);
}

void diskii_device::lss_sync()
{
	if(!m_active)
		return;

	attotime next_flux = m_floppy ? m_floppy->get_next_transition(cycles_to_time(m_cycles-1)) : attotime::never;

	uint64_t cycles_limit = time_to_cycles(machine().time());
	uint64_t cycles_next_flux = next_flux != attotime::never ? time_to_cycles(next_flux) : uint64_t(-1);
	uint64_t cycles_next_flux_down = cycles_next_flux != uint64_t(-1) ? cycles_next_flux+1 : uint64_t(-1);

	if(m_cycles >= cycles_next_flux && m_cycles < cycles_next_flux_down)
		m_address &= ~0x10;
	else
		m_address |= 0x10;

	while(m_cycles < cycles_limit) {
		uint64_t cycles_next_trans = cycles_limit;
		if(cycles_next_trans > cycles_next_flux && m_cycles < cycles_next_flux)
			cycles_next_trans = cycles_next_flux;
		if(cycles_next_trans > cycles_next_flux_down && m_cycles < cycles_next_flux_down)
			cycles_next_trans = cycles_next_flux_down;

		while(m_cycles < cycles_next_trans) {
			uint8_t opcode = m_rom_p6[m_address];

			if(m_mode_write) {
				if((m_write_line_active && !(m_address & 0x80)) ||
					(!m_write_line_active && (m_address & 0x80))) {
					m_write_line_active = !m_write_line_active;
					assert(m_write_position != 32);
					m_write_buffer[m_write_position++] = cycles_to_time(m_cycles);
				} else if(m_write_position >= 30) {
					attotime now = cycles_to_time(m_cycles);
					if(m_floppy)
						m_floppy->write_flux(m_write_start_time, now, m_write_position, m_write_buffer);
					m_write_start_time = now;
					m_write_position = 0;
				}
			}

			m_address = (m_address & 0x1e) | (opcode & 0xc0) | ((opcode & 0x20) >> 5) | ((opcode & 0x10) << 1);
			switch(opcode & 0x0f) {
			case 0: case 1: case 2: case 3: case 4: case 5: case 6: case 7:
				m_data_reg = 0x00;
				break;
			case 0x8: case 0xc:
				break;
			case 0x9:
				m_data_reg <<= 1;
				break;
			case 0xa: case 0xe:
				m_data_reg = (m_data_reg >> 1) | (m_floppy && m_floppy->wpt_r() ? 0x80 : 0x00);
				break;
			case 0xb: case 0xf:
				m_data_reg = m_last_6502_write;
				break;
			case 0xd:
				m_data_reg = (m_data_reg << 1) | 0x01;
				break;
			}
			if(m_data_reg & 0x80)
				m_address |= 0x02;
			else
				m_address &= ~0x02;
			m_cycles++;
		}

		if(m_cycles == cycles_next_flux)
			m_address &= ~0x10;
		else if(m_cycles == cycles_next_flux_down) {
			m_address |= 0x10;
			next_flux = m_floppy ? m_floppy->get_next_transition(cycles_to_time(m_cycles)) : attotime::never;
			cycles_next_flux = next_flux != attotime::never ? time_to_cycles(next_flux) : uint64_t(-1);
			cycles_next_flux_down = cycles_next_flux != uint64_t(-1) ? cycles_next_flux+1 : uint64_t(-1);
		}
	}
}
