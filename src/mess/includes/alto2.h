/*****************************************************************************
 *
 * includes/alto2.h
 *
 ****************************************************************************/

#ifndef _INCLUDES_ALTO2_H_
#define _INCLUDES_ALTO2_H_

#include "emu.h"
#include "cpu/alto2/alto2cpu.h"
#include "machine/diablo_hd.h"

class alto2_state : public driver_device
{
public:
	alto2_state(const machine_config &mconfig, device_type type, const char *tag) :
		driver_device(mconfig, type, tag),
		m_maincpu(*this, "maincpu"),
		m_io_row0(*this, "ROW0"),
		m_io_row1(*this, "ROW1"),
		m_io_row2(*this, "ROW2"),
		m_io_row3(*this, "ROW3"),
		m_io_row4(*this, "ROW4"),
		m_io_row5(*this, "ROW5"),
		m_io_row6(*this, "ROW6"),
		m_io_row7(*this, "ROW7"),
		m_io_config(*this, "CONFIG")
	{ }

	UINT32 screen_update(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);

	DECLARE_DRIVER_INIT(alto2);

	virtual void palette_init();
	DECLARE_MACHINE_RESET(alto2);

protected:
	required_device<cpu_device> m_maincpu;
	required_ioport m_io_row0;
	required_ioport m_io_row1;
	required_ioport m_io_row2;
	required_ioport m_io_row3;
	required_ioport m_io_row4;
	required_ioport m_io_row5;
	required_ioport m_io_row6;
	required_ioport m_io_row7;
	optional_ioport m_io_config;
};

#endif /* _INCLUDES_ALTO2_H_ */
