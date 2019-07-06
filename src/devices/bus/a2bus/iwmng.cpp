// license:BSD-3-Clause
// copyright-holders:R. Belmont
/*********************************************************************

    a2iwm.c

    Implementation of the Apple II Disk II controller card

*********************************************************************/

#include "emu.h"
#include "imagedev/floppy.h"
#include "formats/ap2_dsk.h"
#include "iwmng.h"

/***************************************************************************
    PARAMETERS
***************************************************************************/

//**************************************************************************
//  GLOBAL VARIABLES
//**************************************************************************

DEFINE_DEVICE_TYPE(A2BUS_IWMNG, a2bus_iwmng_device, "a2iwmng", "Apple Disk II NG controller")

#define IWMFDC_TAG      "iwmfdc"

static void a2_floppies(device_slot_interface &device)
{
	device.option_add("525", FLOPPY_525_SD);
}

void a2bus_iwmng_device::device_add_mconfig(machine_config &config)
{
	IWM_FDC(config, m_iwmfdc, clock(), m_q3_clock);
	for(auto &floppy : m_floppy)
		FLOPPY_CONNECTOR(config, floppy, a2_floppies, "525", a2bus_iwmng_device::floppy_formats);
}

FLOPPY_FORMATS_MEMBER( a2bus_iwmng_device::floppy_formats )
	FLOPPY_A216S_FORMAT, FLOPPY_RWTS18_FORMAT, FLOPPY_EDD_FORMAT, FLOPPY_WOZ_FORMAT
FLOPPY_FORMATS_END


//**************************************************************************
//  LIVE DEVICE
//**************************************************************************

	a2bus_iwmng_device::a2bus_iwmng_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock, uint32_t q3_clock) :
	device_t(mconfig, A2BUS_IWMNG, tag, owner, clock),
	device_a2bus_card_interface(mconfig, *this),
	m_iwmfdc(*this, IWMFDC_TAG),
	m_floppy(*this, "%u", 0U),
	m_q3_clock(q3_clock)
{
}

//-------------------------------------------------
//  device_start - device-specific startup
//-------------------------------------------------

void a2bus_iwmng_device::device_start()
{
}

void a2bus_iwmng_device::device_reset()
{
	m_iwmfdc->set_floppies(m_floppy[0], m_floppy[1]);
}

/*-------------------------------------------------
    read_c0nx - called for reads from this card's c0nx space
-------------------------------------------------*/

uint8_t a2bus_iwmng_device::read_c0nx(uint8_t offset)
{
	return m_iwmfdc->read(offset);
}


/*-------------------------------------------------
    write_c0nx - called for writes to this card's c0nx space
-------------------------------------------------*/

void a2bus_iwmng_device::write_c0nx(uint8_t offset, uint8_t data)
{
	m_iwmfdc->write(offset, data);
}
