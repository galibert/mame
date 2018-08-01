// license:BSD-3-Clause
// copyright-holders:Olivier Galibert

// TI/Natsemi DP8753A Real Time Clock

#ifndef MAME_MACHINE_DP8573A_H
#define MAME_MACHINE_DP8573A_H

#pragma once

class dp8573a_device : public device_t, public device_nvram_interface
{
public:
	dp8573a_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock = 32768);

	void map(address_map &map);

protected:
	virtual void device_start() override;
	virtual void device_reset() override;
	virtual void device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr) override;

	virtual void nvram_default() override;
	virtual void nvram_read(emu_file &file) override;
	virtual void nvram_write(emu_file &file) override;

private:
	u8 m_status, m_pf, m_tsc, m_rtm, m_om, m_ic0, m_ic1;
	u8 m_tscc, m_hscc, m_scc, m_mcc, m_hcc, m_dmcc, m_mocc, m_ycc, m_lycc, m_dwcc;
	u8 m_scr, m_mcr, m_hcr, m_dmcr, m_mocr, m_dwcr;
	u8 m_ssr, m_msr, m_hsr, m_dmsr, m_mosr;
	u8 m_ram[2], m_ram2, m_ram3;

	DECLARE_READ8_MEMBER (status_r);
	DECLARE_WRITE8_MEMBER(status_w);
	DECLARE_READ8_MEMBER (rtm_r);
	DECLARE_WRITE8_MEMBER(rtm_w);
	DECLARE_READ8_MEMBER (om_r);
	DECLARE_WRITE8_MEMBER(om_w);
	DECLARE_READ8_MEMBER (pf_ic0_r);
	DECLARE_WRITE8_MEMBER(pf_ic0_w);
	DECLARE_READ8_MEMBER (tsc_ic1_r);
	DECLARE_WRITE8_MEMBER(tsc_ic1_w);
	DECLARE_READ8_MEMBER (hscc_r);
	DECLARE_WRITE8_MEMBER(hscc_w);
	DECLARE_READ8_MEMBER (scc_r);
	DECLARE_WRITE8_MEMBER(scc_w);
	DECLARE_READ8_MEMBER (mcc_r);
	DECLARE_WRITE8_MEMBER(mcc_w);
	DECLARE_READ8_MEMBER (hcc_r);
	DECLARE_WRITE8_MEMBER(hcc_w);
	DECLARE_READ8_MEMBER (dmcc_r);
	DECLARE_WRITE8_MEMBER(dmcc_w);
	DECLARE_READ8_MEMBER (mocc_r);
	DECLARE_WRITE8_MEMBER(mocc_w);
	DECLARE_READ8_MEMBER (ycc_r);
	DECLARE_WRITE8_MEMBER(ycc_w);
	DECLARE_READ8_MEMBER (ram2_r);
	DECLARE_WRITE8_MEMBER(ram2_w);
	DECLARE_READ8_MEMBER (ram3_r);
	DECLARE_WRITE8_MEMBER(ram3_w);
	DECLARE_READ8_MEMBER (dwcc_r);
	DECLARE_WRITE8_MEMBER(dwcc_w);
	DECLARE_READ8_MEMBER (scr_r);
	DECLARE_WRITE8_MEMBER(scr_w);
	DECLARE_READ8_MEMBER (mcr_r);
	DECLARE_WRITE8_MEMBER(mcr_w);
	DECLARE_READ8_MEMBER (hcr_r);
	DECLARE_WRITE8_MEMBER(hcr_w);
	DECLARE_READ8_MEMBER (dmcr_r);
	DECLARE_WRITE8_MEMBER(dmcr_w);
	DECLARE_READ8_MEMBER (mocr_r);
	DECLARE_WRITE8_MEMBER(mocr_w);
	DECLARE_READ8_MEMBER (dwcr_r);
	DECLARE_WRITE8_MEMBER(dwcr_w);
	DECLARE_READ8_MEMBER (ssr_r);
	DECLARE_WRITE8_MEMBER(ssr_w);
	DECLARE_READ8_MEMBER (msr_r);
	DECLARE_WRITE8_MEMBER(msr_w);
	DECLARE_READ8_MEMBER (hsr_r);
	DECLARE_WRITE8_MEMBER(hsr_w);
	DECLARE_READ8_MEMBER (dmsr_r);
	DECLARE_WRITE8_MEMBER(dmsr_w);
	DECLARE_READ8_MEMBER (mosr_r);
	DECLARE_WRITE8_MEMBER(mosr_w);
	DECLARE_READ8_MEMBER (ram_r);
	DECLARE_WRITE8_MEMBER(ram_w);

	void update_irq();
};

DECLARE_DEVICE_TYPE(DP8573A, dp8573a_device)

#endif
