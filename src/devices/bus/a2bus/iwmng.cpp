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

static SLOT_INTERFACE_START( a2_floppies )
	SLOT_INTERFACE( "525", FLOPPY_525_SD )
SLOT_INTERFACE_END

MACHINE_CONFIG_START(a2bus_iwmng_device::device_add_mconfig)
	MCFG_DEVICE_ADD(IWMFDC_TAG, IWM_FDC, 7000000) // To be verified
	MCFG_FLOPPY_DRIVE_ADD("0", a2_floppies, "525", a2bus_iwmng_device::floppy_formats)
	MCFG_FLOPPY_DRIVE_ADD("1", a2_floppies, "525", a2bus_iwmng_device::floppy_formats)
MACHINE_CONFIG_END

FLOPPY_FORMATS_MEMBER( a2bus_iwmng_device::floppy_formats )
	FLOPPY_A216S_FORMAT, FLOPPY_RWTS18_FORMAT, FLOPPY_EDD_FORMAT, FLOPPY_WOZ_FORMAT
FLOPPY_FORMATS_END


//**************************************************************************
//  LIVE DEVICE
//**************************************************************************

a2bus_iwmng_device::a2bus_iwmng_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock) :
	device_t(mconfig, A2BUS_IWMNG, tag, owner, clock),
	device_a2bus_card_interface(mconfig, *this),
	m_iwmfdc(*this, IWMFDC_TAG),
	floppy0(*this, "0"),
	floppy1(*this, "1"), m_rom(nullptr)
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
	m_iwmfdc->set_floppies(floppy0, floppy1);
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
