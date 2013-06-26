/*************************************************************************

    Centuri Aztarac hardware

*************************************************************************/

class aztarac_state : public driver_device
{
public:
	aztarac_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag),
			m_nvram(*this, "nvram") ,
		m_vectorram(*this, "vectorram"),
		m_maincpu(*this, "maincpu"),
		m_audiocpu(*this, "audiocpu") { }

	required_shared_ptr<UINT16> m_nvram;
	int m_sound_status;
	required_shared_ptr<UINT16> m_vectorram;
	int m_xcenter;
	int m_ycenter;
	DECLARE_READ16_MEMBER(nvram_r);
	DECLARE_READ16_MEMBER(joystick_r);
	DECLARE_WRITE16_MEMBER(aztarac_ubr_w);
	DECLARE_READ16_MEMBER(aztarac_sound_r);
	DECLARE_WRITE16_MEMBER(aztarac_sound_w);
	DECLARE_READ8_MEMBER(aztarac_snd_command_r);
	DECLARE_READ8_MEMBER(aztarac_snd_status_r);
	DECLARE_WRITE8_MEMBER(aztarac_snd_status_w);
	virtual void machine_reset();
	virtual void video_start();
	INTERRUPT_GEN_MEMBER(aztarac_snd_timed_irq);
	IRQ_CALLBACK_MEMBER(aztarac_irq_callback);
	inline void read_vectorram(UINT16 *vectorram, int addr, int *x, int *y, int *c);
	required_device<cpu_device> m_maincpu;
	required_device<cpu_device> m_audiocpu;
};
