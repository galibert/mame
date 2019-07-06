// license:BSD-3-Clause
// copyright-holders:Olivier Galibert
/*********************************************************************

    Apple Disk II floppy disk controller

*********************************************************************/

#ifndef MAME_MACHINE_DISKII_H
#define MAME_MACHINE_DISKII_H

#pragma once

#include "applefdc.h"
#include "formats/flopimg.h"
#include "machine/74259.h"

//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************


class diskii_device: public applefdc_device
{
public:
	// construction/destruction
	diskii_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	// optional information overrides
	virtual const tiny_rom_entry *device_rom_region() const override;
	virtual void device_add_mconfig(machine_config &config) override;

	virtual u8 read(offs_t offset) override;
	virtual void write(offs_t offset, uint8_t data) override;

	virtual void set_floppy(floppy_image_device *floppy) override;
	virtual floppy_image_device *get_floppy() const override;

protected:
	virtual void device_start() override;
	virtual void device_reset() override;
	virtual void device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr) override;

	void control(int offset);
	DECLARE_WRITE8_MEMBER(set_phase);
	uint64_t time_to_cycles(const attotime &tm);
	attotime cycles_to_time(uint64_t cycles);

	void lss_start();
	void lss_sync();

	enum {
		MODE_IDLE, MODE_ACTIVE, MODE_DELAY
	};

	floppy_image_device *m_floppy;

	required_device<addressable_latch_device> m_phaselatch;

	uint64_t m_cycles;
	uint8_t m_data_reg, m_address;
	attotime m_write_start_time;
	attotime m_write_buffer[32];
	int m_write_position;
	bool m_write_line_active;

	const uint8_t *m_rom_p6;
	uint8_t m_last_6502_write;
	bool m_mode_write, m_mode_load;
	int m_active;
	emu_timer *m_timer, *m_delay_timer;
	bool m_external_drive_select;
	bool m_external_io_select;

	int m_drvsel;
	int m_enable1;
};

// device type definition
DECLARE_DEVICE_TYPE(DISKII, diskii_device)

#endif // MAME_MACHINE_DISKII_H
