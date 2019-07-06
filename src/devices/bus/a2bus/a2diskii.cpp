// license:BSD-3-Clause
// copyright-holders:R. Belmont
/*********************************************************************

    a2diskii.c

    Implementation of the Apple II Disk II controller card

*********************************************************************/

#include "emu.h"
#include "a2diskii.h"

/***************************************************************************
    PARAMETERS
***************************************************************************/

//**************************************************************************
//  GLOBAL VARIABLES
//**************************************************************************

DEFINE_DEVICE_TYPE(A2BUS_DISKII, a2bus_diskii_device, "a2diskii", "Apple Disk II controller (16-sector)")
DEFINE_DEVICE_TYPE(A2BUS_DISKII13, a2bus_diskii13_device, "diskii13", "Apple Disk II controller (13-sector)")

#define WOZFDC_TAG         "wozfdc"
#define DISKII_ROM_REGION  "diskii_rom"

ROM_START( diskii )
	ROM_REGION(0x100, DISKII_ROM_REGION, 0)
	ROM_LOAD( "341-0027-a.p5",  0x0000, 0x0100, CRC(ce7144f6) SHA1(d4181c9f046aafc3fb326b381baac809d9e38d16) ) /* 341-0027-a: 16-sector disk drive (older version), PROM P5 */
ROM_END

ROM_START( diskii13 )
	ROM_REGION(0x100, DISKII_ROM_REGION, 0)
	ROM_LOAD( "341-0009.bin", 0x000000, 0x000100, CRC(d34eb2ff) SHA1(afd060e6f35faf3bb0146fa889fc787adf56330a) )
ROM_END

//-------------------------------------------------
//  device_add_mconfig - add device configuration
//-------------------------------------------------

void a2bus_diskii_base_device::device_add_mconfig(machine_config &config)
{	
	DISKII(config, m_wozfdc, 1021800*2);
	m_wozfdc->phases_cb().set(FUNC(a2bus_diskii_base_device::phases_w));
	m_wozfdc->devsel_cb().set(FUNC(a2bus_diskii_base_device::devsel_w));
}

void a2bus_diskii_device::device_add_mconfig(machine_config &config)
{
	a2bus_diskii_base_device::device_add_mconfig(config);
	applefdc_device::add_525(config, m_floppy[0]);
	applefdc_device::add_525(config, m_floppy[1]);
}

void a2bus_diskii13_device::device_add_mconfig(machine_config &config)
{
	a2bus_diskii_base_device::device_add_mconfig(config);
	applefdc_device::add_525_13(config, m_floppy[0]);
	applefdc_device::add_525_13(config, m_floppy[1]);
}

//-------------------------------------------------
//  rom_region - device-specific ROM region
//-------------------------------------------------

const tiny_rom_entry *a2bus_diskii_device::device_rom_region() const
{
	return ROM_NAME( diskii );
}

const tiny_rom_entry *a2bus_diskii13_device::device_rom_region() const
{
	return ROM_NAME( diskii13 );
}

//**************************************************************************
//  LIVE DEVICE
//**************************************************************************

a2bus_diskii_base_device::a2bus_diskii_base_device(const machine_config &mconfig, device_type type, const char *tag, device_t *owner, uint32_t clock) :
	device_t(mconfig, type, tag, owner, clock),
	device_a2bus_card_interface(mconfig, *this),
	m_wozfdc(*this, WOZFDC_TAG),
	m_floppy(*this, "%u", 0U),
	m_rom(nullptr)
{
}

a2bus_diskii_device::a2bus_diskii_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock) :
	a2bus_diskii_base_device(mconfig, A2BUS_DISKII, tag, owner, clock)
{
}

a2bus_diskii13_device::a2bus_diskii13_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock) :
	a2bus_diskii_base_device(mconfig, A2BUS_DISKII13, tag, owner, clock)
{
}

//-------------------------------------------------
//  device_start - device-specific startup
//-------------------------------------------------

void a2bus_diskii_base_device::device_start()
{
	m_rom = device().machine().root_device().memregion(this->subtag(DISKII_ROM_REGION).c_str())->base();
}

void a2bus_diskii_base_device::device_reset()
{
}

/*-------------------------------------------------
    read_c0nx - called for reads from this card's c0nx space
-------------------------------------------------*/

uint8_t a2bus_diskii_base_device::read_c0nx(uint8_t offset)
{
	return m_wozfdc->read(offset);
}


/*-------------------------------------------------
    write_c0nx - called for writes to this card's c0nx space
-------------------------------------------------*/

void a2bus_diskii_base_device::write_c0nx(uint8_t offset, uint8_t data)
{
	m_wozfdc->write(offset, data);
}

/*-------------------------------------------------
    read_cnxx - called for reads from this card's cnxx space
-------------------------------------------------*/

uint8_t a2bus_diskii_base_device::read_cnxx(uint8_t offset)
{
	return m_rom[offset];
}

void a2bus_diskii_base_device::devsel_w(u8 data)
{
	if(data & 1)
		m_wozfdc->set_floppy(m_floppy[0]->get_device());
	else if(data & 2)
		m_wozfdc->set_floppy(m_floppy[1]->get_device());
	else
		m_wozfdc->set_floppy(nullptr);
}

void a2bus_diskii_base_device::phases_w(u8 data)
{
	auto flp = m_wozfdc->get_floppy();
	if(flp)
		flp->seek_phase_w(data);
}
