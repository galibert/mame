// license:BSD-3-Clause
// copyright-holders:R. Belmont
/*********************************************************************

    a2diskii.h

    Apple II Disk II Controller Card, new floppy

*********************************************************************/

#ifndef MAME_BUS_A2BUS_A2DISKII_H
#define MAME_BUS_A2BUS_A2DISKII_H

#pragma once

#include "a2bus.h"
#include "imagedev/floppy.h"
#include "formats/flopimg.h"
#include "machine/diskii.h"

//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************


class a2bus_diskii_base_device:
	public device_t,
	public device_a2bus_card_interface
{
public:
	// construction/destruction
	a2bus_diskii_base_device(const machine_config &mconfig, device_type type, const char *tag, device_t *owner, uint32_t clock);

protected:
	virtual void device_start() override;
	virtual void device_reset() override;
	virtual void device_add_mconfig(machine_config &config) override;

	// overrides of standard a2bus slot functions
	virtual uint8_t read_c0nx(uint8_t offset) override;
	virtual void write_c0nx(uint8_t offset, uint8_t data) override;
	virtual uint8_t read_cnxx(uint8_t offset) override;

	required_device<diskii_device> m_wozfdc;
	required_device_array<floppy_connector, 2> m_floppy;

protected:
	const uint8_t *m_rom;

	void devsel_w(u8 data);
	void phases_w(u8 data);
};

class a2bus_diskii_device: public a2bus_diskii_base_device
{
public:
	a2bus_diskii_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

protected:
	virtual void device_add_mconfig(machine_config &config) override;
	virtual const tiny_rom_entry *device_rom_region() const override;
};

class a2bus_diskii13_device: public a2bus_diskii_base_device
{
public:
	a2bus_diskii13_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

protected:
	virtual void device_add_mconfig(machine_config &config) override;
	virtual const tiny_rom_entry *device_rom_region() const override;
};

// device type definition
DECLARE_DEVICE_TYPE(A2BUS_DISKII, a2bus_diskii_device)
DECLARE_DEVICE_TYPE(A2BUS_DISKII13, a2bus_diskii13_device)

#endif  // MAME_BUS_A2BUS_A2DISKII_H
