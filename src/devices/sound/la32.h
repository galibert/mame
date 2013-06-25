#ifndef __LA32_H__
#define __LA32_H__

#define MCFG_LA32_ADD(_tag) \
	MCFG_DEVICE_ADD(_tag, LA32, 0)

#define MCFG_LA32_IRQ_CALLBACK(_cb) \
	downcast<la32_device *>(device)->set_irq(DEVCB_##_cb);

#define MCFG_LA32_SAMPLE_CALLBACK(_cb) \
	downcast<la32_device *>(device)->set_sample(DEVCB_##_cb);

class la32_device : public device_t,
					public device_sound_interface
{
public:
	la32_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	DECLARE_READ8_MEMBER(read);
	DECLARE_WRITE8_MEMBER(write);

	template<class _cb> void set_irq(_cb cb) {
		irq_cb.set_callback(cb);
	}

	template<class _cb> void set_sample(_cb cb) {
		sample_cb.set_callback(cb);
	}

protected:
	virtual void device_start() override;
	virtual void device_reset() override;
	virtual void device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr) override;
	virtual void sound_stream_update(sound_stream &stream, stream_sample_t **inputs, stream_sample_t **outputs, int samples) override;

private:
	const static uint16_t exp2_table[0x200];
	const static uint8_t exp2_delta_table[0x200];
	const static uint16_t logsin_table[0x200];

	struct ramp_t {
		uint32_t cur_value;
		uint8_t increment;
		uint8_t target;
		bool done;

		bool check_done() const;
		bool step();
		void reset();
		void set_increment(uint8_t i);
		void set_target(uint8_t t);
	};

	struct partial_t {
		ramp_t tva, tvf;

		uint16_t pitch;
		uint8_t base_adr_cutoff;
		uint8_t flags, len_flags, pulse_width;

		bool irq_ramp, irq_sample;

		bool pcm() const { return flags & 0x80; }
		bool interpolate() const { return flags & 0x40; }
		bool sawtooth() const { return flags & 0x40; }
		bool ring_modulation() { return flags & 0x20; }
		bool active() const { return flags & 0x10; }
		bool channel() const { return flags & 0x08; }
		uint8_t pan() const { return flags & 0x07; }

		uint32_t base_adr() const { return ((len_flags & 1) << 19) | (base_adr_cutoff << 11); }
		uint8_t base_cutoff() const { return base_adr_cutoff; }
		uint32_t len() const { return 0x800 << ((len_flags >> 4) & 7); }
		bool loop() const { return len_flags & 0x80; }
		uint8_t resonance() const { return len_flags & 0x1f; }
		void reset();
	};

	bool timer_phase;
	sound_stream *stream;
	emu_timer *timer;
	partial_t partials[32];
	int irq_count;

	devcb_write_line irq_cb, sample_cb;

	std::string tts(attotime t);
	std::string ttsn();

	void step_ramp(partial_t &p, ramp_t &r);
	uint16_t compute_output_pcm(partial_t &p);
	uint16_t compute_output_synthetic(partial_t &p);
	uint16_t compute_output(partial_t &p);
};

DECLARE_DEVICE_TYPE(LA32, la32_device);

#endif
