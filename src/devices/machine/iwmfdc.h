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
	iwmfdc_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	u8 read(offs_t offset);
	void write(offs_t offset, u8 data);

	void set_floppies(floppy_connector *f0, floppy_connector *f1);

protected:
	virtual void device_start() override;
	virtual void device_reset() override;
	virtual void device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr) override;

	floppy_connector *m_floppy0, *m_floppy1;
	floppy_image_device *m_floppy;

private:
	uint8_t m_main_reg_latch;

	static const int max_logic_input_count = 15;
	static const int max_choice_input_count = 4;

	struct logic_entry {
		bool buffered;
		const char *name;
		int ninp;
		int inputs[max_logic_input_count];
		std::function<bool (const bool *)> eval;
	};

	struct drive_entry {
		const char *name;
		int in_0g, in_1g;
	};

	struct drive2_entry {
		const char *name;
		int in_0ga, in_0gb, in_1g;
	};

	struct choice_entry {
		const char *name;
		int ninp;
		struct {
			int source;
			int gate;
		} choices[max_choice_input_count];
	};

	static const int new_ids[];
	static const logic_entry logic_entries[];
	static const drive_entry drive_entries[];
	static const drive2_entry drive2_entries[];
	static const choice_entry choice_entries[];
	static const char *const input_names[];

	bool *m_net_vals;
	std::list<int> *m_targets;
	std::set<int> m_changed;

	uint64_t m_cycles, m_next_read_transition;
	bool m_current_input_state;

	void set_net(int net, bool value);

	void propagate_change(int net, std::set<int> &to_recompute_normal, std::set<int> &to_recompute_buffered);
	void set_dual(int id, bool in_0, bool in_1, std::set<int> &to_recompute_normal_next, std::set<int> &to_recompute_buffered);
	void update();
	void run_q_cycles(int count);
	void sync();

	void set_address(int offset);
	void set_data(uint8_t data);
	uint8_t get_data();
	const char *net_name(int net);

	uint8_t access(offs_t offset, uint8_t data);
	void setup_next_read_transition(uint64_t base_cycles);
	void trace_frame();
};

DECLARE_DEVICE_TYPE(IWM_FDC, iwmfdc_device)

#endif  /* __IWMFDC_H__ */
