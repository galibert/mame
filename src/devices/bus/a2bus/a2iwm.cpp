// license:BSD-3-Clause
// copyright-holders:R. Belmont
/*********************************************************************

    a2iwm.c

    Implementation of the Apple II IWM controller card

*********************************************************************/

#include "emu.h"
#include "formats/ap2_dsk.h"
#include "a2iwm.h"

//**************************************************************************
//  GLOBAL VARIABLES
//**************************************************************************

DEFINE_DEVICE_TYPE(A2BUS_IWM, a2bus_iwm_device, "a2iwm", "Apple IWM controller card")

//-------------------------------------------------
//  device_add_mconfig - add device configuration
//-------------------------------------------------

void a2bus_iwm_device::device_add_mconfig(machine_config &config)
{
	IWM(config, m_iwm, clock(), 1021800*2);
	m_iwm->phases_cb().set(FUNC(a2bus_iwm_device::phases_w));
	m_iwm->devsel_cb().set(FUNC(a2bus_iwm_device::devsel_w));
	applefdc_device::add_525(config, m_floppy[0]);
	applefdc_device::add_525(config, m_floppy[1]);
}

//**************************************************************************
//  LIVE DEVICE
//**************************************************************************

a2bus_iwm_device::a2bus_iwm_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock) :
	device_t(mconfig, A2BUS_IWM, tag, owner, clock),
	device_a2bus_card_interface(mconfig, *this),
	m_iwm(*this, "iwm"),
	m_floppy(*this, "%u", 0U)
{
}

//-------------------------------------------------
//  device_start - device-specific startup
//-------------------------------------------------

void a2bus_iwm_device::device_start()
{
}

void a2bus_iwm_device::device_reset()
{
}

/*-------------------------------------------------
    read_c0nx - called for reads from this card's c0nx space
-------------------------------------------------*/

uint8_t a2bus_iwm_device::read_c0nx(uint8_t offset)
{
	return m_iwm->read(offset);
}


/*-------------------------------------------------
    write_c0nx - called for writes to this card's c0nx space
-------------------------------------------------*/

void a2bus_iwm_device::write_c0nx(uint8_t offset, uint8_t data)
{
	m_iwm->write(offset, data);
}

void a2bus_iwm_device::devsel_w(u8 data)
{
	if(data & 1)
		m_iwm->set_floppy(m_floppy[0]->get_device());
	else if(data & 2)
		m_iwm->set_floppy(m_floppy[1]->get_device());
	else
		m_iwm->set_floppy(nullptr);
}

void a2bus_iwm_device::phases_w(u8 data)
{
	auto flp = m_iwm->get_floppy();
	if(flp)
		flp->seek_phase_w(data);
}
