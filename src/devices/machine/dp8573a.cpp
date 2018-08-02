// license:BSD-3-Clause
// copyright-holders:Olivier Galibert

// TI/Natsemi DP8753A Real Time Clock

#include "emu.h"
#include "dp8573a.h"

DEFINE_DEVICE_TYPE(DP8573A, dp8573a_device, "dp8573a", "DP8573A Real Time Clock")
	;

dp8573a_device::dp8573a_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock) :
	device_t(mconfig, DP8573A, tag, owner, clock),
	device_nvram_interface(mconfig, *this)
{
}

void dp8573a_device::device_start()
{
	save_item(NAME(m_status));
	save_item(NAME(m_pf));
	save_item(NAME(m_tsc));
	save_item(NAME(m_rtm));
	save_item(NAME(m_om));
	save_item(NAME(m_ic0));
	save_item(NAME(m_ic1));
	save_item(NAME(m_tscc));
	save_item(NAME(m_hscc));
	save_item(NAME(m_scc));
	save_item(NAME(m_mcc));
	save_item(NAME(m_hcc));
	save_item(NAME(m_dmcc));
	save_item(NAME(m_mocc));
	save_item(NAME(m_ycc));
	save_item(NAME(m_ram2));
	save_item(NAME(m_ram3));
	save_item(NAME(m_dwcc));
	save_item(NAME(m_scr));
	save_item(NAME(m_mcr));
	save_item(NAME(m_hcr));
	save_item(NAME(m_dmcr));
	save_item(NAME(m_mocr));
	save_item(NAME(m_dwcr));
	save_item(NAME(m_ssr));
	save_item(NAME(m_msr));
	save_item(NAME(m_hsr));
	save_item(NAME(m_dmsr));
	save_item(NAME(m_mosr));
	save_item(NAME(m_ram));

	auto timer = timer_alloc(0);
	auto cycle = attotime::from_double(32.768 / clock());
	timer->adjust(cycle, 0, cycle);
}

void dp8573a_device::device_reset()
{
}

void dp8573a_device::device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr)
{
	if(!(m_rtm & 0x08))
		return;

	u8 dmcc_limit;

	// Milliseconds
	m_pf |= 0x20;
	if(m_ic0 & 0x20)
		m_status |= 0x04;
	m_tscc ++;
	if(m_tscc != 10)
		goto check;
	m_tscc = 0;

	// Hundredths of second
		m_pf |= 0x10;
	if(m_ic0 & 0x10)
		m_status |= 0x04;
	m_hscc ++;
	if((m_hscc & 0x0f) != 10)
		goto check;

	// Tenths of second
		m_pf |= 0x08;
	if(m_ic0 & 0x08)
		m_status |= 0x04;
	m_hscc += 6;
	if(m_hscc != 0xa0)
		goto check;
	m_hscc = 0;

	// Seconds
	m_pf |= 0x04;
	if(m_ic0 & 0x04)
		m_status |= 0x04;
	m_scc ++;
	if((m_scc & 0x0f) != 10)
		goto check;

	// Ten seconds
	m_pf |= 0x02;
	if(m_ic0 & 0x02)
		m_status |= 0x04;
	m_scc += 6;
	if(m_scc != 0x60)
		goto check;
	m_scc = 0;

	// Minutes
	m_pf |= 0x01;
	if(m_ic0 & 0x01)
		m_status |= 0x04;
	m_mcc ++;
	if((m_mcc & 0x0f) != 10)
		goto check;
	m_mcc += 6;
	if(m_mcc != 0x60)
		goto check;

	// Hours
	m_mcc = 0;
	m_hcc ++;
	if((m_hcc & 0xf) == 10)
		m_hcc += 6;
	if(m_rtm & 0x04) {
		if(m_hcc != 13)
			goto check;
		m_hcc = 1;
	} else {
		if(m_hcc != 24)
			goto check;
		m_hcc = 0;
	}

	// Days
	m_dwcc ++;
	if(m_dwcc == 8)
		m_dwcc = 1;
	m_dmcc ++;
	if((m_dmcc & 0xf) == 10)
		m_dmcc += 6;
	dmcc_limit =
		m_mocc == 1 || m_mocc == 3 || m_mocc == 5 || m_mocc == 7 ||
		m_mocc == 8 || m_mocc == 10 || m_mocc == 12 ? 31 :
		m_mocc != 2 ? 30 :
		m_rtm & 3 ? 28 : 29;
	if(m_dmcc <= dmcc_limit)
		goto check;

	// Months
	m_mocc ++;
	if(m_mocc != 13)
		goto check;
	m_mocc = 1;

	// Years
	m_rtm = (m_rtm & 0xfc) | ((m_rtm + 1) & 0x03);
	m_ycc ++;
	if((m_ycc & 0xf) == 10)
		m_ycc += 6;
	if(m_ycc == 100)
		m_ycc = 0;

 check:
	if(m_ic1 & 0x40) {
		if((!(m_ic1 & 0x01) || m_scc  == m_scr)  &&
		   (!(m_ic1 & 0x02) || m_mcc  == m_mcr)  &&
		   (!(m_ic1 & 0x04) || m_mcc  == m_hcr)  &&
		   (!(m_ic1 & 0x08) || m_dmcc == m_dmcr) &&
		   (!(m_ic1 & 0x10) || m_mocc == m_mocr) &&
		   (!(m_ic1 & 0x20) || m_dwcc == m_dwcr))
			m_status |= 0x08;
	}

	update_irq();

	if(m_tsc & 0x80) {
		m_ssr = m_scc;
		m_msr = m_mcc;
		m_hsr = m_hcc;
		m_dmsr = m_dmcc;
		m_mosr = m_mocc;
	}
}

void dp8573a_device::nvram_default()
{
	m_status = 0x00;
	m_pf     = 0x40;
	m_tsc    = 0x00;
	m_rtm    = 0x00;
	m_om     = 0x00;
	m_ic0    = 0x00;
	m_ic1    = 0x00;
	m_tscc   = 0x00;
	m_hscc   = 0x00;
	m_scc    = 0x00;
	m_mcc    = 0x00;
	m_hcc    = 0x00;
	m_dmcc   = 0x01;
	m_mocc   = 0x01;
	m_ycc    = 0x00;
	m_ram2   = 0x00;
	m_ram3   = 0x00;
	m_dwcc   = 0x01;
	m_scr    = 0x00;
	m_mcr    = 0x00;
	m_hcr    = 0x00;
	m_dmcr   = 0x00;
	m_mocr   = 0x00;
	m_dwcr   = 0x00;
	m_ssr    = 0x00;
	m_msr    = 0x00;
	m_hsr    = 0x00;
	m_dmsr   = 0x00;
	m_mosr   = 0x00;
	m_ram[0] = 0x00;
	m_ram[1] = 0x00;
}

void dp8573a_device::nvram_read(emu_file &file)
{
	file.read(&m_status, 1);
	file.read(&m_pf, 1);
	file.read(&m_tsc, 1);
	file.read(&m_rtm, 1);
	file.read(&m_om, 1);
	file.read(&m_ic0, 1);
	file.read(&m_ic1, 1);
	file.read(&m_tscc, 1);
	file.read(&m_hscc, 1);
	file.read(&m_scc, 1);
	file.read(&m_mcc, 1);
	file.read(&m_hcc, 1);
	file.read(&m_dmcc, 1);
	file.read(&m_mocc, 1);
	file.read(&m_ycc, 1);
	file.read(&m_ram2, 1);
	file.read(&m_ram3, 1);
	file.read(&m_dwcc, 1);
	file.read(&m_scr, 1);
	file.read(&m_mcr, 1);
	file.read(&m_hcr, 1);
	file.read(&m_dmcr, 1);
	file.read(&m_mocr, 1);
	file.read(&m_dwcr, 1);
	file.read(&m_ssr, 1);
	file.read(&m_msr, 1);
	file.read(&m_hsr, 1);
	file.read(&m_dmsr, 1);
	file.read(&m_mosr, 1);
	file.read(m_ram, 2);
}

void dp8573a_device::nvram_write(emu_file &file)
{
	file.write(&m_status, 1);
	file.write(&m_pf, 1);
	file.write(&m_tsc, 1);
	file.write(&m_rtm, 1);
	file.write(&m_om, 1);
	file.write(&m_ic0, 1);
	file.write(&m_ic1, 1);
	file.write(&m_hscc, 1);
	file.write(&m_scc, 1);
	file.write(&m_mcc, 1);
	file.write(&m_hcc, 1);
	file.write(&m_dmcc, 1);
	file.write(&m_mocc, 1);
	file.write(&m_ycc, 1);
	file.write(&m_ram2, 1);
	file.write(&m_ram3, 1);
	file.write(&m_dwcc, 1);
	file.write(&m_scr, 1);
	file.write(&m_mcr, 1);
	file.write(&m_hcr, 1);
	file.write(&m_dmcr, 1);
	file.write(&m_mocr, 1);
	file.write(&m_dwcr, 1);
	file.write(&m_ssr, 1);
	file.write(&m_msr, 1);
	file.write(&m_hsr, 1);
	file.write(&m_dmsr, 1);
	file.write(&m_mosr, 1);
	file.write(m_ram, 2);
}

void dp8573a_device::map(address_map &map)
{
	map(0x00, 0x00).rw(FUNC(dp8573a_device::status_r),  FUNC(dp8573a_device::status_w));
	map(0x01, 0x01).rw(FUNC(dp8573a_device::rtm_r),     FUNC(dp8573a_device::rtm_w));
	map(0x02, 0x02).rw(FUNC(dp8573a_device::om_r),      FUNC(dp8573a_device::om_w));
	map(0x03, 0x03).rw(FUNC(dp8573a_device::pf_ic0_r),  FUNC(dp8573a_device::pf_ic0_w));
	map(0x04, 0x04).rw(FUNC(dp8573a_device::tsc_ic1_r), FUNC(dp8573a_device::tsc_ic1_w));
	map(0x05, 0x05).rw(FUNC(dp8573a_device::hscc_r),    FUNC(dp8573a_device::hscc_w));
	map(0x06, 0x06).rw(FUNC(dp8573a_device::scc_r),     FUNC(dp8573a_device::scc_w));
	map(0x07, 0x07).rw(FUNC(dp8573a_device::mcc_r),     FUNC(dp8573a_device::mcc_w));
	map(0x08, 0x08).rw(FUNC(dp8573a_device::hcc_r),     FUNC(dp8573a_device::hcc_w));
	map(0x09, 0x09).rw(FUNC(dp8573a_device::dmcc_r),    FUNC(dp8573a_device::dmcc_w));
	map(0x0a, 0x0a).rw(FUNC(dp8573a_device::mocc_r),    FUNC(dp8573a_device::mocc_w));
	map(0x0b, 0x0b).rw(FUNC(dp8573a_device::ycc_r),     FUNC(dp8573a_device::ycc_w));
	map(0x0c, 0x0c).rw(FUNC(dp8573a_device::ram2_r),    FUNC(dp8573a_device::ram2_w));
	map(0x0d, 0x0d).rw(FUNC(dp8573a_device::ram3_r),    FUNC(dp8573a_device::ram3_w));
	map(0x0e, 0x0e).rw(FUNC(dp8573a_device::dwcc_r),    FUNC(dp8573a_device::dwcc_w));
	map(0x13, 0x13).rw(FUNC(dp8573a_device::scr_r),     FUNC(dp8573a_device::scr_w));
	map(0x14, 0x14).rw(FUNC(dp8573a_device::mcr_r),     FUNC(dp8573a_device::mcr_w));
	map(0x15, 0x15).rw(FUNC(dp8573a_device::hcr_r),     FUNC(dp8573a_device::hcr_w));
	map(0x16, 0x16).rw(FUNC(dp8573a_device::dmcr_r),    FUNC(dp8573a_device::dmcr_w));
	map(0x17, 0x17).rw(FUNC(dp8573a_device::mocr_r),    FUNC(dp8573a_device::mocr_w));
	map(0x18, 0x18).rw(FUNC(dp8573a_device::dwcr_r),    FUNC(dp8573a_device::dwcr_w));
	map(0x19, 0x19).rw(FUNC(dp8573a_device::scr_r),     FUNC(dp8573a_device::ssr_w));
	map(0x1a, 0x1a).rw(FUNC(dp8573a_device::mcr_r),     FUNC(dp8573a_device::msr_w));
	map(0x1b, 0x1b).rw(FUNC(dp8573a_device::hcr_r),     FUNC(dp8573a_device::hsr_w));
	map(0x1c, 0x1c).rw(FUNC(dp8573a_device::dmcr_r),    FUNC(dp8573a_device::dmsr_w));
	map(0x1d, 0x1d).rw(FUNC(dp8573a_device::mocr_r),    FUNC(dp8573a_device::mosr_w));
	map(0x1e, 0x1f).rw(FUNC(dp8573a_device::ram_r),     FUNC(dp8573a_device::ram_w));
}

READ8_MEMBER (dp8573a_device::status_r)
{
	return m_status;
}

WRITE8_MEMBER(dp8573a_device::status_w)
{
	m_status = (m_status & 0x03) | (m_status & ~data & 0x0c) | (data & 0xf0);
	update_irq();
}

READ8_MEMBER (dp8573a_device::rtm_r)
{
	if(m_status & 0x40)
		return m_rtm;
	return 0x00;
}

WRITE8_MEMBER(dp8573a_device::rtm_w)
{
	if(m_status & 0x40) {
		m_rtm = data;
		if(m_rtm & 0x08) {
			m_pf &= 0xbf;
			logerror("[Transmit] Clock started\n");
			machine().debug_break();
		}
	}
}

READ8_MEMBER (dp8573a_device::om_r)
{
	if(m_status & 0x40)
		return m_om;
	return 0x00;
}

WRITE8_MEMBER(dp8573a_device::om_w)
{
	if(m_status & 0x40)
		m_om = data;
}

READ8_MEMBER (dp8573a_device::pf_ic0_r)
{
	if(m_status & 0x40)
		return m_ic0;
	else {
		u8 res = m_pf;
		if(!machine().side_effects_disabled())
			m_pf &= 0x80;
		return res;
	}
}

WRITE8_MEMBER(dp8573a_device::pf_ic0_w)
{
	if(m_status & 0x40)
		m_ic0 = data;
	else
		m_pf = (m_pf & 0x3f) | (data & 0x80);
}

READ8_MEMBER (dp8573a_device::tsc_ic1_r)
{
	if(m_status & 0x40)
		return m_ic1;
	else
		return m_tsc;
}

WRITE8_MEMBER(dp8573a_device::tsc_ic1_w)
{
	if(m_status & 0x40)
		m_ic1 = data;
	else {
		m_tsc = data;
		if(m_tsc & 0x80) {
			m_ssr = m_scc;
			m_msr = m_mcc;
			m_hsr = m_hcc;
			m_dmsr = m_dmcc;
			m_mosr = m_mocc;
		}
	}
}

READ8_MEMBER (dp8573a_device::hscc_r)
{
	return m_hscc;
}

WRITE8_MEMBER(dp8573a_device::hscc_w)
{
	m_hscc = data;
}

READ8_MEMBER (dp8573a_device::scc_r)
{
	return m_scc;
}

WRITE8_MEMBER(dp8573a_device::scc_w)
{
	m_scc = data;
}

READ8_MEMBER (dp8573a_device::mcc_r)
{
	return m_mcc;
}

WRITE8_MEMBER(dp8573a_device::mcc_w)
{
	m_mcc = data;
}

READ8_MEMBER (dp8573a_device::hcc_r)
{
	return m_hcc;
}

WRITE8_MEMBER(dp8573a_device::hcc_w)
{
	m_hcc = data;
}

READ8_MEMBER (dp8573a_device::dmcc_r)
{
	return m_dmcc;
}

WRITE8_MEMBER(dp8573a_device::dmcc_w)
{
	m_dmcc = data;
}

READ8_MEMBER (dp8573a_device::mocc_r)
{
	return m_mocc;
}

WRITE8_MEMBER(dp8573a_device::mocc_w)
{
	m_mocc = data;
}

READ8_MEMBER (dp8573a_device::ycc_r)
{
	return m_ycc;
}

WRITE8_MEMBER(dp8573a_device::ycc_w)
{
	m_ycc = data;
}

READ8_MEMBER (dp8573a_device::ram2_r)
{
	return m_ram2;
}

WRITE8_MEMBER(dp8573a_device::ram2_w)
{
	m_ram2 = data;
}

READ8_MEMBER (dp8573a_device::ram3_r)
{
	return m_ram3;
}

WRITE8_MEMBER(dp8573a_device::ram3_w)
{
	m_ram3 = data & 0x03;
}

READ8_MEMBER (dp8573a_device::dwcc_r)
{
	return m_dwcc;
}

WRITE8_MEMBER(dp8573a_device::dwcc_w)
{
	m_dwcc = data;
}

READ8_MEMBER (dp8573a_device::scr_r)
{
	return m_scr;
}

WRITE8_MEMBER(dp8573a_device::scr_w)
{
	m_scr = data;
}

READ8_MEMBER (dp8573a_device::mcr_r)
{
	return m_mcr;
}

WRITE8_MEMBER(dp8573a_device::mcr_w)
{
	m_mcr = data;
}

READ8_MEMBER (dp8573a_device::hcr_r)
{
	return m_hcr;
}

WRITE8_MEMBER(dp8573a_device::hcr_w)
{
	m_hcr = data;
}

READ8_MEMBER (dp8573a_device::dmcr_r)
{
	return m_dmcr;
}

WRITE8_MEMBER(dp8573a_device::dmcr_w)
{
	m_dmcr = data;
}

READ8_MEMBER (dp8573a_device::mocr_r)
{
	return m_mocr;
}

WRITE8_MEMBER(dp8573a_device::mocr_w)
{
	m_mocr = data;
}

READ8_MEMBER (dp8573a_device::dwcr_r)
{
	return m_dwcr;
}

WRITE8_MEMBER(dp8573a_device::dwcr_w)
{
	m_dwcr = data;
}

READ8_MEMBER (dp8573a_device::ssr_r)
{
	return m_ssr;
}

WRITE8_MEMBER(dp8573a_device::ssr_w)
{
	m_ssr = data;
}

READ8_MEMBER (dp8573a_device::msr_r)
{
	return m_msr;
}

WRITE8_MEMBER(dp8573a_device::msr_w)
{
	m_msr = data;
}

READ8_MEMBER (dp8573a_device::hsr_r)
{
	return m_hsr;
}

WRITE8_MEMBER(dp8573a_device::hsr_w)
{
	m_hsr = data;
}

READ8_MEMBER (dp8573a_device::dmsr_r)
{
	return m_dmsr;
}

WRITE8_MEMBER(dp8573a_device::dmsr_w)
{
	m_dmsr = data;
}

READ8_MEMBER (dp8573a_device::mosr_r)
{
	return m_mosr;
}

WRITE8_MEMBER(dp8573a_device::mosr_w)
{
	m_mosr = data;
}

READ8_MEMBER (dp8573a_device::ram_r)
{
	return (offset && (m_pf & 0x80)) ? 0x00 : m_ram[offset];
}

WRITE8_MEMBER(dp8573a_device::ram_w)
{
	if(!offset || !(m_pf & 0x80))
		m_ram[offset] = data;
}

void dp8573a_device::update_irq()
{
	//	u8 orig_status = m_status;
	if(m_status & 0x0e)
		m_status |= 0x01;
	else
		m_status &= 0xfe;
}
