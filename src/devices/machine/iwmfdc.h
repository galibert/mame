// license:BSD-3-Clause
// copyright-holders:Olivier Galibert
/*********************************************************************

    iwmfdc.h

    Apple IWM floppy disk controller

*********************************************************************/

#pragma once

#ifndef __IWMFDC_H__
#define __IWMFDC_H__

#include "emu.h"
#include "imagedev/floppy.h"
#include "formats/flopimg.h"

#include <set>

//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************


class iwmfdc_device:
	public device_t
{
public:
	// construction/destruction
	iwmfdc_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock, uint32_t q3_clock = 0);

	u8 read(offs_t offset);
	void write(offs_t offset, u8 data);

	void set_floppies(floppy_connector *f0, floppy_connector *f1);

protected:
	virtual void device_start() override;
	virtual void device_reset() override;
	virtual void device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr) override;

private:
	enum {
		MODE_IDLE,
		MODE_ACTIVE, MODE_DELAY, // m_active modes
		MODE_READ, MODE_WRITE    // m_rw modes
	};

	// state machine states
	enum {
		S_IDLE,
		SR_WINDOW_EDGE_0,
		SR_WINDOW_EDGE_1,
		SW_WINDOW_MIDDLE,
		SW_WINDOW_END
	};

	floppy_connector *m_floppy0, *m_floppy1;
	floppy_image_device *m_floppy;
	emu_timer *m_timer;
	double m_q3_fclk_ratio, m_fclk_q3_ratio;
	u64 m_last_sync, m_next_state_change, m_sync_update, m_async_update;
	u64 m_flux_write_start;
	std::array<u64, 16> m_flux_write;
	u32 m_flux_write_count;
	u32 m_q3_clock;
	int m_active, m_rw, m_rw_state;
	u8 m_data, m_whd, m_mode, m_status, m_control;
	u8 m_rsh, m_wsh;

	void sync();
	u8 control(int offset, u8 data);
	u64 time_to_cycles(const attotime &tm) const;
	attotime cycles_to_time(u64 cycles) const;
	u64 fclk_to_q3(u64 cycles) const;
	u64 q3_to_fclk(u64 cycles) const;

	void mode_w(u8 data);
	void data_w(u8 data);

	u64 window_size() const;
	u64 half_window_size() const;
	u64 read_register_update_delay() const;
	u64 write_sync_half_window_size() const;
	inline bool is_sync() const;
	void flush_write();
};

DECLARE_DEVICE_TYPE(IWM_FDC, iwmfdc_device)

#endif  /* __IWMFDC_H__ */
