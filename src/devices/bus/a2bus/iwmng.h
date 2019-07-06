// license:BSD-3-Clause
// copyright-holders:R. Belmont
/*********************************************************************

    iwmng.h

    Apple II IWM Controller Card, new floppy

*********************************************************************/

#ifndef MAME_BUS_A2BUS_IWMNG_H
#define MAME_BUS_A2BUS_IWMNG_H

#pragma once

#include "emu.h"
#include "a2bus.h"
#include "imagedev/floppy.h"
#include "formats/flopimg.h"
#include "machine/iwmfdc.h"

//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************


class a2bus_iwmng_device:
	public device_t,
	public device_a2bus_card_interface
{
public:
	// construction/destruction
	a2bus_iwmng_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock, uint32_t q3_clock = 0);

	// optional information overrides
	virtual void device_add_mconfig(machine_config &config) override;

protected:
	virtual void device_start() override;
	virtual void device_reset() override;

	// overrides of standard a2bus slot functions
	virtual uint8_t read_c0nx(uint8_t offset) override;
	virtual void write_c0nx(uint8_t offset, uint8_t data) override;

private:
	required_device<iwmfdc_device> m_iwmfdc;
	required_device_array<floppy_connector, 2> m_floppy;

	uint32_t m_q3_clock;

	DECLARE_FLOPPY_FORMATS( floppy_formats );
};

// device type definition
DECLARE_DEVICE_TYPE(A2BUS_IWMNG, a2bus_iwmng_device)

#endif  // MAME_BUS_A2BUS_IWMNG_H
