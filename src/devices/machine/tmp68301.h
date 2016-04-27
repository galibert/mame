// license:BSD-3-Clause
// copyright-holders:Luca Elia
#ifndef MAME_MACHINE_TMP68301_H
#define MAME_MACHINE_TMP68301_H

#pragma once

#include "cpu/m68000/m68000.h"

#include "cpu/m68000/m68000.h"

//**************************************************************************
//  INTERFACE CONFIGURATION MACROS
//**************************************************************************

#define MCFG_TMP68301_ADD(_tag, _cpu) \
	MCFG_DEVICE_ADD( _tag, TMP68301, 0 ) \
	downcast<tmp68301_device *>(device)->set_cpu_tag(_cpu);

/* TODO: serial ports, frequency & hook it up with m68k */

// FIXME: M68000 ought to be a parent class, not an external object
#define MCFG_TMP68301_CPU(_tag) \
	downcast<tmp68301_device &>(*device).set_cpu_tag("^" _tag);

#define MCFG_TMP68301_IN_PARALLEL_CB(cb) \
	devcb = &downcast<tmp68301_device &>(*device).set_in_parallel_callback((DEVCB_##cb));

#define MCFG_TMP68301_OUT_PARALLEL_CB(cb) \
	devcb = &downcast<tmp68301_device &>(*device).set_out_parallel_callback((DEVCB_##cb));

#define MCFG_TMP68301_SERIAL_ADD( _tag ) \
	MCFG_DEVICE_ADD( _tag, TMP68301_SERIAL, 0 )

#define MCFG_TMP68301_RS232_ADD( _tag ) \
	MCFG_DEVICE_ADD( _tag, TMP68301_RS232, 0 )

#define MCFG_TMP68301_SERIAL_TX_CALLBACK(_devcb) \
	devcb = &tmp68301_serial_device::set_tx_cb(*device, DEVCB_##_devcb);

#define MCFG_TMP68301_SERIAL_RTS_CALLBACK(_devcb) \
	devcb = &tmp68301_rs232_device::set_rts_cb(*device, DEVCB_##_devcb);

#define MCFG_TMP68301_SERIAL_DTR_CALLBACK(_devcb) \
	devcb = &tmp68301_rs232_device::set_dtr_cb(*device, DEVCB_##_devcb);

//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

class tmp68301_device;

class tmp68301_serial_device : public device_t
{
public:
	tmp68301_serial_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);
	tmp68301_serial_device(const machine_config &mconfig, device_type &type, const char *tag, device_t *owner, uint32_t clock);

	DECLARE_READ8_MEMBER(smr_r);
	DECLARE_WRITE8_MEMBER(smr_w);
	DECLARE_READ8_MEMBER(scmr_r);
	DECLARE_WRITE8_MEMBER(scmr_w);
	DECLARE_READ8_MEMBER(sbrr_r);
	DECLARE_WRITE8_MEMBER(sbrr_w);
	DECLARE_READ8_MEMBER(ssr_r);
	DECLARE_WRITE8_MEMBER(ssr_w);
	DECLARE_READ8_MEMBER(sdr_r);
	DECLARE_WRITE8_MEMBER(sdr_w);

	DECLARE_WRITE_LINE_MEMBER(rx_w);

	template<class _Object> static devcb_base &set_tx_cb(device_t &device, _Object object) { return downcast<tmp68301_serial_device &>(device).tx_cb.set_callback(object); }

	void set_prescaled_clock(double clock);

protected:
	required_device<tmp68301_device> m_cpu;
	devcb_write_line tx_cb;
	attotime clock_interval;
	double prescaled_clock;

	uint8_t smr, scmr, ssr, sbrr;

	virtual void device_start() override;
	virtual void device_reset() override;

	void clock_update();
};

class tmp68301_rs232_device : public tmp68301_serial_device {
public:
	tmp68301_rs232_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	DECLARE_WRITE_LINE_MEMBER(cts_w);
	DECLARE_WRITE_LINE_MEMBER(dsr_w);

	template<class _Object> static devcb_base &set_rts_cb(device_t &device, _Object object) { return downcast<tmp68301_rs232_device &>(device).rts_cb.set_callback(object); }
	template<class _Object> static devcb_base &set_dtr_cb(device_t &device, _Object object) { return downcast<tmp68301_rs232_device &>(device).dtr_cb.set_callback(object); }

protected:
	devcb_write_line rts_cb, dtr_cb;

	virtual void device_start() override;
	virtual void device_reset() override;
};

class tmp68301_device : public device_t,
						public device_memory_interface
{
public:
	tmp68301_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	void set_cpu_tag(const char *tag) { m_cpu.set_tag(tag); }
	template <class Object> devcb_base &set_in_parallel_callback(Object &&cb) { return m_in_parallel_cb.set_callback(std::forward<Object>(cb)); }
	template <class Object> devcb_base &set_out_parallel_callback(Object &&cb) { return m_out_parallel_cb.set_callback(std::forward<Object>(cb)); }

	// Hardware Registers
	DECLARE_READ16_MEMBER( regs_r );
	DECLARE_WRITE16_MEMBER( regs_w );

	// Interrupts
	void external_interrupt_0();
	void external_interrupt_1();
	void external_interrupt_2();

	IRQ_CALLBACK_MEMBER(irq_callback);

	offs_t pc() { return m_cpu->pc(); }

private:
	DECLARE_READ16_MEMBER(imr_r);
	DECLARE_WRITE16_MEMBER(imr_w);
	DECLARE_READ16_MEMBER(iisr_r);
	DECLARE_WRITE16_MEMBER(iisr_w);
	DECLARE_READ16_MEMBER(pdr_r);
	DECLARE_WRITE16_MEMBER(pdr_w);
	DECLARE_READ16_MEMBER(pdir_r);
	DECLARE_WRITE16_MEMBER(pdir_w);
	DECLARE_READ8_MEMBER(icr_r);
	DECLARE_WRITE8_MEMBER(icr_w);

	void tmp68301_regs(address_map &map);

	DECLARE_READ8_MEMBER(spr_r);
	DECLARE_WRITE8_MEMBER(spr_w);
	DECLARE_READ8_MEMBER(scr_r);
	DECLARE_WRITE8_MEMBER(scr_w);

protected:
	// device-level overrides
	virtual void device_start() override;
	virtual void device_reset() override;
	virtual space_config_vector memory_space_config() const override;
	virtual void device_add_mconfig(machine_config &config) override;

private:
	TIMER_CALLBACK_MEMBER( timer_callback );
	void update_timer( int i );
	void update_irq_state(uint16_t cause);
	void update_irq_serial(uint16_t cause, uint8_t type);

	static constexpr uint16_t EXT_IRQ0 = 1 << 0;
	static constexpr uint16_t EXT_IRQ1 = 1 << 1;
	static constexpr uint16_t EXT_IRQ2 = 1 << 2;
	static constexpr uint16_t SERIAL_IRQ_CH0 = 1 << 4;
	static constexpr uint16_t SERIAL_IRQ_CH1 = 1 << 5;
	static constexpr uint16_t SERIAL_IRQ_CH2 = 1 << 6;
	static constexpr uint16_t PARALLEL_IRQ = 1 << 7;
	static constexpr uint16_t TIMER0_IRQ = 1 << 8;
	static constexpr uint16_t TIMER1_IRQ = 1 << 9;
	static constexpr uint16_t TIMER2_IRQ = 1 << 10;

	inline uint16_t read_word(offs_t address);
	inline void write_word(offs_t address, uint16_t data);

	required_device<m68000_device> m_cpu;

	devcb_read16         m_in_parallel_cb;
	devcb_write16        m_out_parallel_cb;

	required_device<tmp68301_rs232_device> m_ser0;
	required_device<tmp68301_serial_device> m_ser1;
	required_device<tmp68301_serial_device> m_ser2;

	const char *m_cpu_tag;

	// internal state
	uint16_t m_regs[0x400];

	emu_timer *m_tmp68301_timer[3];        // 3 Timers

	uint16_t m_irq_vector[8];

	uint16_t m_imr;
	uint16_t m_iisr;
	uint16_t m_pdir;
	uint16_t m_pdr;
	uint8_t m_icr[10];
	uint8_t m_scr, m_spr;

	const address_space_config      m_space_config;

	void recalc_serial_clock();
};

DECLARE_DEVICE_TYPE(TMP68301, tmp68301_device)
DECLARE_DEVICE_TYPE(TMP68301_SERIAL, tmp68301_serial_device)
DECLARE_DEVICE_TYPE(TMP68301_RS232, tmp68301_rs232_device)

#endif // MAME_MACHINE_TMP68301_H
