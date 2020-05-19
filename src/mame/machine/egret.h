// license:BSD-3-Clause
// copyright-holders:R. Belmont
#ifndef MAME_MACHINE_EGRET_H
#define MAME_MACHINE_EGRET_H

#pragma once

#include "bus/adb/adb.h"
#include "cpu/m6805/m68hc05.h"


//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> egret_device

class egret_device :  public device_t, public device_nvram_interface
{
public:
	// interface routines
	uint8_t get_xcvr_session() { return xcvr_session; }
	void set_via_full(uint8_t val) { via_full = val; }
	void set_sys_session(uint8_t val) { sys_session = val; }
	uint8_t get_via_data() { return via_data; }
	void set_via_data(uint8_t dat) { via_data = dat; }
	uint8_t get_via_clock() { return via_clock; }

	auto reset_callback() { return write_reset.bind(); }
	auto via_clock_callback() { return write_via_clock.bind(); }
	auto via_data_callback() { return write_via_data.bind(); }

protected:
	egret_device(const machine_config &mconfig, device_type type, const char *tag, device_t *owner, uint32_t clock);

	// device-level overrides
	virtual void device_start() override;
	virtual void device_reset() override;
	virtual void device_add_mconfig(machine_config &config) override;

	// device_config_nvram_interface overrides
	virtual void nvram_default() override;
	virtual void nvram_read(emu_file &file) override;
	virtual void nvram_write(emu_file &file) override;

	virtual void device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr) override;

private:
	devcb_write_line write_reset, write_linechange, write_via_clock, write_via_data;
	required_device<m68hc05eg_device> m_maincpu;
	optional_device<adb_connector> m_adb_connector[2];
	adb_device *m_adb_device[2];
	bool m_adb_device_out[2];
	bool m_adb_device_poweron[2];
	bool m_adb_out;

	uint8_t ddrs[3];
	uint8_t ports[3];
	uint8_t pll_ctrl;
	uint8_t timer_ctrl;
	uint8_t timer_counter;
	uint8_t onesec;
	uint8_t xcvr_session, via_full, sys_session, via_data, via_clock;
	bool egret_controls_power;
	int reset_line;
	emu_timer *m_timer;
	uint8_t pram[0x100], disk_pram[0x100];
	bool pram_loaded;

	void send_port(uint8_t offset);

	uint8_t ddr_r(offs_t offset);
	void ddr_w(offs_t offset, uint8_t data);
	uint8_t ports_r(offs_t offset);
	void ports_w(offs_t offset, uint8_t data);
	uint8_t pll_r();
	void pll_w(uint8_t data);
	uint8_t timer_ctrl_r();
	void timer_ctrl_w(uint8_t data);
	uint8_t timer_counter_r();
	void timer_counter_w(uint8_t data);
	uint8_t onesec_r();
	void onesec_w(uint8_t data);
	uint8_t pram_r(offs_t offset);
	void pram_w(offs_t offset, uint8_t data);

	void adb_w(int id, int state);
	void adb_poweron_w(int id, int state);
	void adb_change();

	void egret_map(address_map &map);
};

class egret_341s0851_device : public egret_device
{
public:
	egret_341s0851_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock = 0);
	virtual ~egret_341s0851_device() = default;

protected:
	virtual const tiny_rom_entry *device_rom_region() const override;
};

class egret_341s0850_device : public egret_device
{
public:
	egret_341s0850_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock = 0);
	virtual ~egret_341s0850_device() = default;

protected:
	virtual const tiny_rom_entry *device_rom_region() const override;
};

class egret_344s0100_device : public egret_device
{
public:
	egret_344s0100_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock = 0);
	virtual ~egret_344s0100_device() = default;

protected:
	virtual const tiny_rom_entry *device_rom_region() const override;
};



// device type definition
DECLARE_DEVICE_TYPE(EGRET_341S0851, egret_341s0851_device)
DECLARE_DEVICE_TYPE(EGRET_341S0850, egret_341s0850_device)
DECLARE_DEVICE_TYPE(EGRET_344S0100, egret_344s0100_device)

#endif // MAME_MACHINE_EGRET_H
