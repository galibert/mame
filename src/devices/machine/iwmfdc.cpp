// license:BSD-3-Clause
// copyright-holders:Olivier Galibert
/*********************************************************************

    iwmfdc.c

    Implementation of the Apple IWM floppy disk controller

*********************************************************************/

#include "emu.h"
#include "imagedev/floppy.h"
#include "formats/ap2_dsk.h"
#include "iwmfdc.h"

/***************************************************************************
    PARAMETERS
***************************************************************************/

#include "iwmfdc.inc"

bool v;
int zc;
FILE *trace_fd;
uint8_t *trace_buffer;
int trace_buffer_size;

//**************************************************************************
//  GLOBAL VARIABLES
//**************************************************************************

DEFINE_DEVICE_TYPE(IWM_FDC, iwmfdc_device, "iwmfdc", "Apple IWM floppy controller")

//**************************************************************************
//  LIVE DEVICE
//**************************************************************************

iwmfdc_device::iwmfdc_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock) :
	device_t(mconfig, IWM_FDC, tag, owner, clock)
{
}

void iwmfdc_device::set_floppies(floppy_connector *f0, floppy_connector *f1)
{
	m_floppy0 = f0;
	m_floppy1 = f1;
}

//-------------------------------------------------
//  device_start - device-specific startup
//-------------------------------------------------

void iwmfdc_device::device_start()
{
	m_net_vals = new bool[limit_input];
	m_targets = new std::list<int>[limit_input];
	for(int i=0; i<limit_pullup; i++)
		for(int j=0; j<logic_entries[i].ninp; j++)
			m_targets[logic_entries[i].inputs[j]].push_back(i);
	for(int i=limit_pullup; i<limit_drive; i++) {
		int idx = i - limit_pullup;
		m_targets[drive_entries[idx].in_0g].push_back(i);
		m_targets[drive_entries[idx].in_1g].push_back(i);
	}
	for(int i=limit_drive; i<limit_drive2; i++) {
		int idx = i - limit_drive;
		m_targets[drive2_entries[idx].in_0ga].push_back(i);
		m_targets[drive2_entries[idx].in_0gb].push_back(i);
		m_targets[drive2_entries[idx].in_1g].push_back(i);
	}
	for(int i=limit_drive2; i<limit_choice; i++) {
		int idx = i - limit_drive2;
		for(int j=0; j<choice_entries[idx].ninp; j++) {
			m_targets[choice_entries[idx].choices[j].gate].push_back(i);
			if(choice_entries[idx].choices[j].source >= 0)
				m_targets[choice_entries[idx].choices[j].source].push_back(i);
		}
	}
}

void iwmfdc_device::trace_frame()
{
	memset(trace_buffer, 0, trace_buffer_size);
	for(int i=0; i<number_of_schem_nets; i++)
		if(new_ids[i] != -1 && m_net_vals[new_ids[i]])
			trace_buffer[i >> 3] |= 1 << (i & 7);
	fwrite(trace_buffer, trace_buffer_size, 1, trace_fd);
	fflush(trace_fd);
}

void iwmfdc_device::device_reset()
{
	trace_fd = fopen("trace.bin", "wb");
	trace_buffer_size = (number_of_schem_nets+7)/8;
	trace_buffer = new uint8_t[trace_buffer_size];
	v = false;
	zc = 50;
	if(v) logerror("reset start\n");
	m_floppy = nullptr;

	m_changed.clear();
	for(int i=0; i<limit_input; i++) {
		m_net_vals[i] = false;
		m_changed.insert(i);
	}
	m_net_vals[net_p_dev] = true;
	m_net_vals[net_p_rddata] = true;
	m_net_vals[net_p_reset] = true;
	m_net_vals[net_p_sense] = true;

	update();
	run_q_cycles(100);
	m_net_vals[net_p_reset] = false;
	run_q_cycles(100);

	m_cycles = 0;
	m_next_read_transition = 0;
	m_current_input_state = true;

	if(v) logerror("reset done\n");

	m_main_reg_latch = 0x00;
}

void iwmfdc_device::device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr)
{
}


void iwmfdc_device::setup_next_read_transition(uint64_t base_cycles)
{
	if(m_next_read_transition && base_cycles < m_next_read_transition)
		return;
	if(m_next_read_transition && m_current_input_state == false) {
		m_next_read_transition += clock() / 1000000;
		if(v) logerror("%lld: Next transition at %lld (%lld)\n", base_cycles, m_next_read_transition, m_next_read_transition - base_cycles);
		return;
	}
	attotime next_flux = m_floppy->get_next_transition(attotime::from_ticks(base_cycles+1, clock()));
	if(next_flux.is_never()) {
		m_next_read_transition = 0;
		return;
	}
	m_next_read_transition = next_flux.as_ticks(clock());
	if(v) logerror("%lld: Next transition at %lld (%lld)\n", base_cycles, m_next_read_transition, m_next_read_transition - base_cycles);
}

void iwmfdc_device::run_q_cycles(int count)
{
	if(v) logerror("%lld: run %d cycles, trans at %lld floppy %p\n", m_cycles, count, m_next_read_transition, m_floppy);
	uint64_t base_cycles = m_cycles;
	m_cycles += count;
	if(count > 1000)
		count = 1000;

	for(uint64_t i = base_cycles; i<base_cycles + count; i++) {
		if(i && i == m_next_read_transition) {
			m_current_input_state = !m_current_input_state;
			set_net(net_p_rddata, m_current_input_state);
			if(v) logerror("%lld: input = %d\n", i, m_current_input_state);
			setup_next_read_transition(i);
		}
		set_net(net_p_fclk, true);
		update();
		if(v) trace_frame();
		set_net(net_p_fclk, false);
		update();
		if(v) trace_frame();
	}
	if(m_next_read_transition && m_cycles != base_cycles + count) {
		if(m_current_input_state == false) {
			m_current_input_state = !m_current_input_state;
			set_net(net_p_rddata, m_current_input_state);
		}
		m_next_read_transition = 0;
		setup_next_read_transition(m_cycles);
	}
}


void iwmfdc_device::propagate_change(int net, std::set<int> &to_recompute_normal, std::set<int> &to_recompute_buffered)
{
	if(v) {
		if(false)
			if(net != net_fclk && net != net_ifclk && net != net_fclk2 && net != net_ifclk2 && net != net_cclk && net != net_icclk && net != net_p_fclk && net != net_i_fclk) {
				const char *name = net_name(net);
				if(name)
					logerror("%30s: %d\n", name, m_net_vals[net]);
			}
		
		if(net == net_dbus_enable_handshake)
			logerror("dbus_enable_handshake: %d\n", m_net_vals[net]);
		if(net == net_dbus_enable_all_ones)
			logerror("dbus_enable_all_ones: %d\n", m_net_vals[net]);
		if(net == net_dbus_enable_read_data)
			logerror("dbus_enable_read_data: %d\n", m_net_vals[net]);
		if(net == net_dbus_enable_status)
			logerror("dbus_enable_status: %d\n", m_net_vals[net]);
		if(net == net_dbus_disable_output)
			logerror("dbus_disable_output: %d\n", m_net_vals[net]);
		
		if(false)
			if(net == net_p_enbl1)
				logerror("floppy enable 1: %d\n", m_net_vals[net]);
		if(false)
			if(net == net_p_enbl2)
				logerror("floppy enable 2: %d\n", m_net_vals[net]);
		if(false)
			if(net == net_drive_active)
				logerror("drive active: %d\n", m_net_vals[net]);
		if(false)
			if(net == net_drive_sel)
				logerror("drive sel: %d\n", m_net_vals[net]);
		if(false)
			if(net == net_p_phase0 || net == net_p_phase1 || net == net_p_phase2 || net == net_p_phase3)
				logerror("phases: %d%d%d%d\n",
						 m_net_vals[net_p_phase0],
						 m_net_vals[net_p_phase1],
						 m_net_vals[net_p_phase2],
						 m_net_vals[net_p_phase3]);
		if(false)
			if(net == net_o_phase0 || net == net_o_phase1 || net == net_o_phase2 || net == net_o_phase3 || net == net_lmotor_on || net == net_drive_sel || net == net_l6 || net == net_l7)
				logerror("decup: %d%d%d%d%d%d%d%d\n",
						 !m_net_vals[net_o_phase0],
						 !m_net_vals[net_o_phase1],
						 !m_net_vals[net_o_phase2],
						 !m_net_vals[net_o_phase3],
						 !m_net_vals[net_lmotor_on],
						 !m_net_vals[net_drive_sel],
						 !m_net_vals[net_l6],
						 !m_net_vals[net_l7]);
	}

	for(int n : m_targets[net])
		if(n >= limit_pullup || !logic_entries[n].buffered)
			to_recompute_normal.insert(n);
		else
			to_recompute_buffered.insert(n);
}

void iwmfdc_device::set_net(int net, bool value)
{
	if(m_net_vals[net] != value) {
		m_changed.insert(net);
		m_net_vals[net] = value;
	}
}

void iwmfdc_device::set_dual(int net, bool in_0, bool in_1, std::set<int> &to_recompute_normal_next, std::set<int> &to_recompute_buffered)
{
	bool old_val = m_net_vals[net];
	if(in_0)
		if(in_1) {
			// Contradicting drives, don't change, try again later
			to_recompute_normal_next.insert(net);
		} else {
			// Driven to 0
			if(old_val) {
				m_net_vals[net] = false;
				propagate_change(net, to_recompute_normal_next, to_recompute_buffered);
			}
		}
	else
		if(in_1) {
			// Driven to 1
			if(!old_val) {
				m_net_vals[net] = true;
				propagate_change(net, to_recompute_normal_next, to_recompute_buffered);
			}
		} else {
			// Undriven, leave as-is
		}
}

const char *iwmfdc_device::net_name(int net)
{
	if(net < limit_pullup)
		return logic_entries[net].name;
	if(net < limit_drive)
		return drive_entries[net - limit_pullup].name;
	if(net < limit_drive2)
		return drive2_entries[net - limit_drive].name;
	if(net < limit_choice)
		return choice_entries[net - limit_drive2].name;
	return input_names[net - limit_choice];
}

void iwmfdc_device::update()
{
	std::set<int> to_recompute_normal, to_recompute_buffered;
	for(int net : m_changed)
		propagate_change(net, to_recompute_normal, to_recompute_buffered);
	m_changed.clear();

	// Don't see how that could happen in real use, but heh
	if(to_recompute_normal.empty()) {
		to_recompute_normal = to_recompute_buffered;
		to_recompute_buffered.clear();
	}

	while(!to_recompute_normal.empty()) {
		std::set<int> to_recompute_normal_next;
		int first = *to_recompute_normal.begin();
		if(first < limit_pullup) {
			// Recompute simple logical expressions
			for(int net : to_recompute_normal)
				if(net >= limit_pullup)
					to_recompute_normal_next.insert(net);
				else {
					bool newval = logic_entries[net].eval(m_net_vals);
					if(newval != m_net_vals[net]) {
						m_net_vals[net] = newval;
						propagate_change(net, to_recompute_normal_next, to_recompute_buffered);
					}
				}

		} else if(first < limit_drive) {
			// Recompute nets with one line setting 0 and one line setting 1
			for(int net : to_recompute_normal)
				if(net >= limit_drive)
					to_recompute_normal_next.insert(net);
				else {
					int idx = net - limit_pullup;
					set_dual(net, m_net_vals[drive_entries[idx].in_0g], m_net_vals[drive_entries[idx].in_1g], to_recompute_normal_next, to_recompute_buffered);
				}

		} else if(first < limit_drive2) {
			// Recompute nets with two lines setting 0 and one line setting 1
			for(int net : to_recompute_normal)
				if(net >= limit_drive2)
					to_recompute_normal_next.insert(net);
				else {
					int idx = net - limit_drive;
					set_dual(net, m_net_vals[drive2_entries[idx].in_0ga] || m_net_vals[drive2_entries[idx].in_0gb],
							 m_net_vals[drive2_entries[idx].in_1g], to_recompute_normal_next, to_recompute_buffered);
				}
		} else {
			// Recompute nets with a number of gates choosing between a number of inputs
			for(int net : to_recompute_normal) {
				int idx = net - limit_drive2;
				bool in_0 = false;
				bool in_1 = false;

				for(int i=0; i < choice_entries[idx].ninp; i++)
					if(m_net_vals[choice_entries[idx].choices[i].gate]) {
						int src = choice_entries[idx].choices[i].source;
						if(src == -3 || (src != -2 && m_net_vals[src]))
							in_1 = true;
						else
							in_0 = true;
					}

				set_dual(net, in_0, in_1, to_recompute_normal_next, to_recompute_buffered);
			}
		}

		// If something in next, then it's the next step.  Otherwise, go to the buffered nets
		if(!to_recompute_normal_next.empty())
			to_recompute_normal = to_recompute_normal_next;
		else {
			to_recompute_normal = to_recompute_buffered;
			to_recompute_buffered.clear();
		}
	}
}

void iwmfdc_device::set_address(int offset)
{
	set_net(net_p_a0, offset & 1);
	set_net(net_p_a1, offset & 2);
	set_net(net_p_a2, offset & 4);
	set_net(net_p_a3, offset & 8);
}

void iwmfdc_device::set_data(uint8_t data)
{
	set_net(net_p_d0, data & 0x01);
	set_net(net_p_d1, data & 0x02);
	set_net(net_p_d2, data & 0x04);
	set_net(net_p_d3, data & 0x08);
	set_net(net_p_d4, data & 0x10);
	set_net(net_p_d5, data & 0x20);
	set_net(net_p_d6, data & 0x40);
	set_net(net_p_d7, data & 0x80);
}

uint8_t iwmfdc_device::get_data()
{
	return
		(m_net_vals[net_p_d0] ? 0x01 : 0x00) |
		(m_net_vals[net_p_d1] ? 0x02 : 0x00) |
		(m_net_vals[net_p_d2] ? 0x04 : 0x00) |
		(m_net_vals[net_p_d3] ? 0x08 : 0x00) |
		(m_net_vals[net_p_d4] ? 0x10 : 0x00) |
		(m_net_vals[net_p_d5] ? 0x20 : 0x00) |
		(m_net_vals[net_p_d6] ? 0x40 : 0x00) |
		(m_net_vals[net_p_d7] ? 0x80 : 0x00);
}

void iwmfdc_device::sync()
{
	uint64_t next_cycles = machine().time().as_ticks(clock());
	if(next_cycles > m_cycles)
		run_q_cycles(next_cycles - m_cycles);
}

uint8_t iwmfdc_device::access(offs_t offset, uint8_t data)
{
	uint8_t old_main_reg_latch = m_main_reg_latch;
	if(offset & 1)
		m_main_reg_latch |= 1 << (offset >> 1);
	else
		m_main_reg_latch &= ~(1 << (offset >> 1));

	old_main_reg_latch ^= m_main_reg_latch;

	// Drive select, motor on
	if(old_main_reg_latch & 0x30) {
		if(m_floppy)
			m_floppy->mon_w(true);

		if(m_next_read_transition) {
			m_next_read_transition = 0;
			m_current_input_state = true;
			set_net(net_p_rddata, true);
		}

		if(m_main_reg_latch & 0x10) {
			if(m_main_reg_latch & 0x20)
				m_floppy = m_floppy1->get_device();
			else
				m_floppy = m_floppy0->get_device();
		} else
			m_floppy = nullptr;
		if(m_floppy) {
			m_floppy->mon_w(false);
			m_floppy->seek_phase_w(m_main_reg_latch & 0x0f);
			setup_next_read_transition(m_cycles);
		}
	} else if(old_main_reg_latch & 0x0f) {
		if(m_floppy) {
			m_floppy->seek_phase_w(m_main_reg_latch & 0x0f);
			setup_next_read_transition(m_cycles);
		}
	}

	if(false)
	if(old_main_reg_latch != m_main_reg_latch)
	logerror("decupx %c%c%c%c%c%c%c%c\n",
			 m_main_reg_latch & 0x01 ? '1' : '0',
			 m_main_reg_latch & 0x02 ? '1' : '0',
			 m_main_reg_latch & 0x04 ? '1' : '0',
			 m_main_reg_latch & 0x08 ? '1' : '0',
			 m_main_reg_latch & 0x10 ? '1' : '0',
			 m_main_reg_latch & 0x20 ? '1' : '0',
			 m_main_reg_latch & 0x40 ? '1' : '0',
			 m_main_reg_latch & 0x80 ? '1' : '0');
	return 0xff;
}

/*-------------------------------------------------
    read - called to read the FDC's registers
-------------------------------------------------*/

u8 iwmfdc_device::read(offs_t offset)
{
	if(offset == 0xc && zc) {
		zc--;
		v = !zc;
	}
	sync();
	set_address(offset);
	set_data(0xff);
	update();
	set_net(net_p_dev, false);
	set_net(net_p_q3, false);
	update();
	run_q_cycles(1);
	uint8_t data = get_data();
	if(v)
	logerror("--read l67=%d handshake=%d all_ones=%d read_data=%d status=%d\n",
			 2*(!m_net_vals[net_l7]) + !m_net_vals[net_l6],
			 m_net_vals[net_dbus_enable_handshake],
			 m_net_vals[net_dbus_enable_all_ones],
			 m_net_vals[net_dbus_enable_read_data],
			 m_net_vals[net_dbus_enable_status]);
	set_net(net_p_dev, true);
	set_net(net_p_q3, true);
	update();

	uint8_t data2 = access(offset, 0xff);
	if(v)
	logerror("Read %x, %02x (%02x)\n", offset, data, data2);
	return data;
}


/*-------------------------------------------------
    write - called to write the FDC's registers
-------------------------------------------------*/

void iwmfdc_device::write(offs_t offset, u8 data)
{
	if(v)
	logerror("Write %x, %02x\n", offset, data);

	sync();
	set_address(offset);
	set_data(data);
	update();
	set_net(net_p_dev, false);
	set_net(net_p_q3, false);
	update();
	run_q_cycles(1);
	set_net(net_p_dev, true);
	set_net(net_p_q3, true);
	update();

	access(offset, data);
}
