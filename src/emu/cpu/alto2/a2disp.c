/*****************************************************************************
 *
 *   Xerox AltoII display interface
 *
 *   Copyright © Jürgen Buchmüller <pullmoll@t-online.de>
 *
 *   Licenses: MAME, GPLv2
 *
 *****************************************************************************/
#include "alto2cpu.h"

/**
 * @brief PROM a38 contains the STOPWAKE' and MBEMBPTY' signals for the FIFO
 * <PRE>
 * The inputs to a38 are the UNLOAD counter RA[0-3] and the DDR← counter
 * WA[0-3], and the designer decided to reverse the address lines :-)
 *
 *	a38  counter
 *	-------------
 *	 A0  RA[0]
 *	 A1  RA[1]
 *	 A2  RA[2]
 *	 A3  RA[3]
 *	 A4  WA[0]
 *	 A5  WA[1]
 *	 A6  WA[2]
 *	 A7  WA[3]
 *
 * Only two bits of a38 are used:
 * 	O1 (002) = STOPWAKE'
 * 	O3 (010) = MBEMPTY'
 * </PRE>
 */

//! PROM a38 bit O1 is STOPWAKE' (stop DWT if bit is zero)
#define FIFO_STOPWAKE(a38) (0 == (a38 & disp_a38_STOPWAKE) ? true : false)

//! PROM a38 bit O3 is MBEMPTY' (FIFO is empty if bit is zero)
#define	FIFO_MBEMPTY(a38) (0 == (a38 & disp_a38_MBEMPTY) ? true : false)

/**
 * @brief emulation of PROM a63 in the display schematics page 8
 * <PRE>
 * The PROM's address lines are driven by a clock CLK, which is
 * pixel clock / 24, and an inverted half-scanline signal H[1]'.
 *
 * It is 32x8 bits and its output bits (B) are connected to the
 * signals, as well as its own address lines (A) through a latch
 * of the type SN74774 like this:
 *
 *	PROM  174   A   others
 *	------------------------
 *	B0    D5    -   HBLANK
 *	B1    D0    -   HSYNC
 *	B2    D4    A0  -
 *	B3    D1    A1  -
 *	B4    D3    A2  -
 *	B5    D2    A3  -
 *	B6    -     -   SCANEND
 *	B7    -     -   HLCGATE
 *	------------------------
 *	H[1]' -     A4  -
 *
 * The display_state_machine() is called at a rate of pixelclock/24.
 *
 * Decoded states of this PROM:
 *
 *  STATE  PROM   binary   HBLANK  HSYNC NEXT SCANEND HLCGATE
 *	----------------------------------------------------------
 *	  000  0007  00000111     1      1    001    0       0
 *	  001  0013  00001011     1      1    002    0       0
 *	  002  0015  00001101     1      0    003    0       0
 *	  003  0021  00010001     1      0    004    0       0
 *	  004  0024  00010100     0      0    005    0       0
 *	  005  0030  00011000     0      0    006    0       0
 *	  006  0034  00011100     0      0    007    0       0
 *	  007  0040  00100000     0      0    010    0       0
 *	  010  0044  00100100     0      0    011    0       0
 *	  011  0050  00101000     0      0    012    0       0
 *	  012  0054  00101100     0      0    013    0       0
 *	  013  0060  00110000     0      0    014    0       0
 *	  014  0064  00110100     0      0    015    0       0
 *	  015  0070  00111000     0      0    016    0       0
 *	  016  0074  00111100     0      0    017    0       0
 *	  017  0200  10000000     0      0    000    0       1
 *	  020  0004  00000100     0      0    001    0       0
 *	  021  0010  00001000     0      0    002    0       0
 *	  022  0014  00001100     0      0    003    0       0
 *	  023  0020  00010000     0      0    004    0       0
 *	  024  0024  00010100     0      0    005    0       0
 *	  025  0030  00011000     0      0    006    0       0
 *	  026  0034  00011100     0      0    007    0       0
 *	  027  0040  00100000     0      0    010    0       0
 *	  030  0044  00100100     0      0    011    0       0
 *	  031  0050  00101000     0      0    012    0       0
 *	  032  0054  00101100     0      0    013    0       0
 *	  033  0060  00110000     0      0    014    0       0
 *	  034  0064  00110100     0      0    015    0       0
 *	  035  0070  00111000     0      0    016    0       0
 *	  036  0175  01111101     1      0    017    1       0
 *	  037  0203  10000011     1      1    000    0       1
 * </PRE>
 */

//!< test the HBLANK (horizontal blanking) signal in PROM a63 being high
#define A63_HBLANK(a) ((a & disp_a63_HBLANK) ? true : false)

//!< test the HSYNC (horizontal synchonisation) signal in PROM a63 being high
#define A63_HSYNC(a) ((a & disp_a63_HSYNC) ? true : false)

//!< test the SCANEND (scanline end) signal in PROM a63 being high
#define A63_SCANEND(a) ((a & disp_a63_SCANEND) ? true : false)

//!< test the HLCGATE (horz. line counter gate) signal in PROM a63 being high
#define A63_HLCGATE(a) ((a & disp_a63_HLCGATE) ? true : false)

/**
 * @brief PROM a66 is a 256x4 bit (type 3601)
 * <PRE>
 * Address lines are driven by H[1] to H[128] of the the horz. line counters.
 * PROM is enabled when H[256] and H[512] are both 0.
 *
 * Q1 is VSYNC for the odd field (with H1024=0)
 * Q2 is VSYNC for the even field (with H1024=1)
 * Q3 is VBLANK for the odd field (with H1024=0)
 * Q4 is VBLANK for the even field (with H1024=1)
 * </PRE>
 */

//! test the VSYNC (vertical synchronisation) signal in PROM a66 being high
#define A66_VSYNC(a) (a & (HLC1024 ? disp_a66_VSYNC_ODD : disp_a66_VSYNC_EVEN) ? false : true)

//! test the VBLANK (vertical blanking) signal in PROM a66 being high
#define A66_VBLANK(a) (a & (HLC1024 ? disp_a66_VBLANK_ODD : disp_a66_VBLANK_EVEN) ? false : true)

/**
 * @brief double the bits for a byte (left and right of display word) to a word
 */
static const UINT16 double_bits[256] = {
	0x0000,0x0003,0x000c,0x000f,0x0030,0x0033,0x003c,0x003f,
	0x00c0,0x00c3,0x00cc,0x00cf,0x00f0,0x00f3,0x00fc,0x00ff,
	0x0300,0x0303,0x030c,0x030f,0x0330,0x0333,0x033c,0x033f,
	0x03c0,0x03c3,0x03cc,0x03cf,0x03f0,0x03f3,0x03fc,0x03ff,
	0x0c00,0x0c03,0x0c0c,0x0c0f,0x0c30,0x0c33,0x0c3c,0x0c3f,
	0x0cc0,0x0cc3,0x0ccc,0x0ccf,0x0cf0,0x0cf3,0x0cfc,0x0cff,
	0x0f00,0x0f03,0x0f0c,0x0f0f,0x0f30,0x0f33,0x0f3c,0x0f3f,
	0x0fc0,0x0fc3,0x0fcc,0x0fcf,0x0ff0,0x0ff3,0x0ffc,0x0fff,
	0x3000,0x3003,0x300c,0x300f,0x3030,0x3033,0x303c,0x303f,
	0x30c0,0x30c3,0x30cc,0x30cf,0x30f0,0x30f3,0x30fc,0x30ff,
	0x3300,0x3303,0x330c,0x330f,0x3330,0x3333,0x333c,0x333f,
	0x33c0,0x33c3,0x33cc,0x33cf,0x33f0,0x33f3,0x33fc,0x33ff,
	0x3c00,0x3c03,0x3c0c,0x3c0f,0x3c30,0x3c33,0x3c3c,0x3c3f,
	0x3cc0,0x3cc3,0x3ccc,0x3ccf,0x3cf0,0x3cf3,0x3cfc,0x3cff,
	0x3f00,0x3f03,0x3f0c,0x3f0f,0x3f30,0x3f33,0x3f3c,0x3f3f,
	0x3fc0,0x3fc3,0x3fcc,0x3fcf,0x3ff0,0x3ff3,0x3ffc,0x3fff,
	0xc000,0xc003,0xc00c,0xc00f,0xc030,0xc033,0xc03c,0xc03f,
	0xc0c0,0xc0c3,0xc0cc,0xc0cf,0xc0f0,0xc0f3,0xc0fc,0xc0ff,
	0xc300,0xc303,0xc30c,0xc30f,0xc330,0xc333,0xc33c,0xc33f,
	0xc3c0,0xc3c3,0xc3cc,0xc3cf,0xc3f0,0xc3f3,0xc3fc,0xc3ff,
	0xcc00,0xcc03,0xcc0c,0xcc0f,0xcc30,0xcc33,0xcc3c,0xcc3f,
	0xccc0,0xccc3,0xcccc,0xcccf,0xccf0,0xccf3,0xccfc,0xccff,
	0xcf00,0xcf03,0xcf0c,0xcf0f,0xcf30,0xcf33,0xcf3c,0xcf3f,
	0xcfc0,0xcfc3,0xcfcc,0xcfcf,0xcff0,0xcff3,0xcffc,0xcfff,
	0xf000,0xf003,0xf00c,0xf00f,0xf030,0xf033,0xf03c,0xf03f,
	0xf0c0,0xf0c3,0xf0cc,0xf0cf,0xf0f0,0xf0f3,0xf0fc,0xf0ff,
	0xf300,0xf303,0xf30c,0xf30f,0xf330,0xf333,0xf33c,0xf33f,
	0xf3c0,0xf3c3,0xf3cc,0xf3cf,0xf3f0,0xf3f3,0xf3fc,0xf3ff,
	0xfc00,0xfc03,0xfc0c,0xfc0f,0xfc30,0xfc33,0xfc3c,0xfc3f,
	0xfcc0,0xfcc3,0xfccc,0xfccf,0xfcf0,0xfcf3,0xfcfc,0xfcff,
	0xff00,0xff03,0xff0c,0xff0f,0xff30,0xff33,0xff3c,0xff3f,
	0xffc0,0xffc3,0xffcc,0xffcf,0xfff0,0xfff3,0xfffc,0xffff
};

//! update the internal bitmap to a byte array
void alto2_cpu_device::update_bitmap_word(int x, int y, UINT16 word)
{
	UINT8* pix = m_dsp.scanline[y] + x;
	*pix++ = (word >> 15) & 1;
	*pix++ = (word >> 14) & 1;
	*pix++ = (word >> 13) & 1;
	*pix++ = (word >> 12) & 1;
	*pix++ = (word >> 11) & 1;
	*pix++ = (word >> 10) & 1;
	*pix++ = (word >>  9) & 1;
	*pix++ = (word >>  8) & 1;
	*pix++ = (word >>  7) & 1;
	*pix++ = (word >>  6) & 1;
	*pix++ = (word >>  5) & 1;
	*pix++ = (word >>  4) & 1;
	*pix++ = (word >>  3) & 1;
	*pix++ = (word >>  2) & 1;
	*pix++ = (word >>  1) & 1;
	*pix++ = (word >>  0) & 1;
}

#define	HLC1	((m_dsp.hlc >>  0) & 1)		//!< horizontal line counter bit 0
#define	HLC2	((m_dsp.hlc >>  1) & 1)		//!< horizontal line counter bit 1
#define	HLC4	((m_dsp.hlc >>  2) & 1)		//!< horizontal line counter bit 2
#define	HLC8	((m_dsp.hlc >>  3) & 1)		//!< horizontal line counter bit 3
#define	HLC16	((m_dsp.hlc >>  4) & 1)		//!< horizontal line counter bit 4
#define	HLC32	((m_dsp.hlc >>  5) & 1)		//!< horizontal line counter bit 5
#define	HLC64	((m_dsp.hlc >>  6) & 1)		//!< horizontal line counter bit 6
#define	HLC128	((m_dsp.hlc >>  7) & 1)		//!< horizontal line counter bit 7
#define	HLC256	((m_dsp.hlc >>  8) & 1)		//!< horizontal line counter bit 8
#define	HLC512	((m_dsp.hlc >>  9) & 1)		//!< horizontal line counter bit 9
#define	HLC1024 ((m_dsp.hlc >> 10) & 1)		//!< horizontal line counter bit 10

#define GET_SETMODE_SPEEDY(mode) X_RDBITS(mode,16,0,0)  //!< get the pixel clock speed from a SETMODE<- bus value
#define GET_SETMODE_INVERSE(mode) X_RDBITS(mode,16,1,1) //!< get the inverse video flag from a SETMODE<- bus value

//!< helper to extract A3-A0 from a PROM a63 value
#define A63_NEXT(n) ((n >> 2) & 017)

/**
 * @brief unload the next word from the display FIFO and shift it to the screen
 */
void alto2_cpu_device::unload_word()
{
	int x = m_unload_word;
	int y = ((m_dsp.hlc - m_dsp.vblank) & ~(1024|1)) | HLC1024;
	UINT16* scanline = m_dsp.raw_bitmap  + y * ALTO2_DISPLAY_SCANLINE_WORDS;

	UINT32 word = m_dsp.inverse;
	UINT8 a38 = m_disp_a38[m_dsp.fifo_rd * 16 + m_dsp.fifo_wr];
	if (FIFO_MBEMPTY(a38)) {
		LOG((LOG_DISPL,1, "	DSP FIFO underrun y:%d x:%d\n", y, x));
	} else {
		word ^= m_dsp.fifo[m_dsp.fifo_rd];
		m_dsp.fifo_rd = (m_dsp.fifo_rd + 1) % ALTO2_DISPLAY_FIFO;
		LOG((LOG_DISPL,3, "	DSP pull %04x from FIFO[%02o] y:%d x:%d\n",
			word, (m_dsp.fifo_rd - 1) & (ALTO2_DISPLAY_FIFO - 1), y, x));
	}

	if (y >= 0 && y < ALTO2_DISPLAY_HEIGHT && x < ALTO2_DISPLAY_VISIBLE_WORDS) {
		if (m_dsp.halfclock) {
			UINT16 word1 = double_bits[word / 256];
			UINT16 word2 = double_bits[word % 256];
			/* mixing with the cursor */
			if (x == m_dsp.curword + 0)
				word1 ^= m_dsp.curdata >> 16;
			if (x == m_dsp.curword + 1)
				word1 ^= m_dsp.curdata & 0177777;
			if (word1 != scanline[x]) {
				scanline[x] = word1;
				update_bitmap_word(16 * x, y, word1);
			}
			x++;
			if (x < ALTO2_DISPLAY_VISIBLE_WORDS) {
				/* mixing with the cursor */
				if (x == m_dsp.curword + 0)
					word2 ^= m_dsp.curdata >> 16;
				if (x == m_dsp.curword + 1)
					word2 ^= m_dsp.curdata & 0177777;
				if (word2 != scanline[x]) {
					scanline[x] = word2;
					update_bitmap_word(16 * x, y, word2);
				}
				x++;
			}
		} else {
			/* mixing with the cursor */
			if (x == m_dsp.curword + 0)
				word ^= m_dsp.curdata >> 16;
			if (x == m_dsp.curword + 1)
				word ^= m_dsp.curdata & 0177777;
			if (word != scanline[x]) {
				scanline[x] = word;
				update_bitmap_word(16 * x, y, word);
			}
			x++;
		}
	}
	if (x < ALTO2_DISPLAY_VISIBLE_WORDS) {
		m_unload_time += ALTO2_DISPLAY_BITTIME(m_dsp.halfclock ? 32 : 16);
		m_unload_word = x;
	} else {
		m_unload_time = -1;
	}
}


/**
 * @brief function called by the CPU to enter the next display state
 *
 * There are 32 states per scanline and 875 scanlines per frame.
 *
 * @param arg the current displ_a63 PROM address
 * @result returns the next state of the display state machine
 */
void alto2_cpu_device::display_state_machine()
{
	LOG((LOG_DISPL,5,"DSP%03o:", m_dsp.state));
	if (020 == m_dsp.state) {
		LOG((LOG_DISPL,2," HLC=%d", m_dsp.hlc));
	}

	UINT8 a63 = m_disp_a63[m_dsp.state];
	if (A63_HLCGATE(a63)) {
		/* reset or count horizontal line counters */
		if (m_dsp.hlc == ALTO2_DISPLAY_HLC_END)
			m_dsp.hlc = ALTO2_DISPLAY_HLC_START;
		else
			m_dsp.hlc++;
		/* start the refresh task _twice_ on each scanline */
		m_task_wakeup |= 1 << task_mrt;
		if (m_ewfct) {
			/* The Ether task wants a wakeup, too */
			m_task_wakeup |= 1 << task_ether;
		}
	}
	// PROM a66 is disabled, if any of HLC256 or HLC512 are high
	UINT8 a66 = (HLC256 || HLC512) ? 017 : m_disp_a66[m_dsp.hlc & 0377];

	// next address from PROM a63, use A4 from HLC1
	UINT8 next = ((HLC1 ^ 1) << 4) | A63_NEXT(a63);

	if (A66_VBLANK(a66)) {
		/* VBLANK: remember hlc */
		m_dsp.vblank = m_dsp.hlc & ~02000;

		LOG((LOG_DISPL,1, " VBLANK"));

		// VSYNC is always within VBLANK, thus we handle it only here
		if (A66_VSYNC(a66)) {
			if (!A66_VSYNC(m_dsp.a66)) {
				LOG((LOG_DISPL,1, " VSYNC↗ (wake DVT)"));
				/*
				 * The display vertical task DVT is awakened once per field,
				 * at the beginning of vertical retrace.
				 */
				m_task_wakeup |= 1 << task_dvt;
				// TODO: upade odd or even field of the internal bitmap
			} else {
				LOG((LOG_DISPL,1, " VSYNC"));
			}
		}
	} else {
		if (A66_VBLANK(m_dsp.a66)) {
			/*
			 * VBLANKPULSE:
			 * The display horizontal task DHT is awakened once at the
			 * beginning of each field, and thereafter whenever the
			 * display word task blocks.
			 * The DHT can block itself, in which case neither it nor
			 * the word task can be awakened until the start of the
			 * next field.
			 */
			LOG((LOG_DISPL,1, " VBLANKPULSE (wake DHT)"));
			m_dsp.dht_blocks = false;
			m_dsp.dwt_blocks = false;
			m_task_wakeup |= 1 << task_dht;
			/*
			 * VBLANKPULSE also resets the cursor task block flip flop,
			 * which is built from two NAND gates a40c and a40d (74H01).
			 */
			m_dsp.curt_blocks = false;
		}
		if (!A63_HBLANK(a63) && A63_HBLANK(m_dsp.a63)) {
			// falling edge of a63 HBLANK starts unloading of FIFO words
			LOG((LOG_DISPL,1, " HBLANK↘ UNLOAD"));
			m_unload_time = ALTO2_DISPLAY_BITTIME(m_dsp.halfclock ? 32 : 16);
			m_unload_word = 0;
#if	DEBUG_DISPLAY_TIMING
			printf("@%lld: first unload_word @%lldns hlc:+%d (id:%d)\n",
				ntime(), ntime() + DISPLAY_BITTIME(m_dsp.halfclock ? 32 : 16),
				m_dsp.hlc - DISPLAY_HLC_START, m_dsp.unload_id);
#endif
		}
	}

	/*
	 * The wakeup request for the display word task (DWT) is controlled by
	 * the state of the 16 word FIFO. If DWT has not executed a BLOCK,
	 * if DHT is not blocked, and if the buffer is not full, DWT wakeups
	 * are generated.
	 */
	UINT8 a38 = m_disp_a38[m_dsp.fifo_rd * 16 + m_dsp.fifo_wr];
	if (!m_dsp.dwt_blocks && !m_dsp.dht_blocks && !FIFO_STOPWAKE(a38)) {
		m_task_wakeup |= 1 << task_dwt;
		LOG((LOG_DISPL,1, " (wake DWT)"));
	}

	// Stop waking the display word task at SCANEND time
	if (A63_SCANEND(a63)) {
		LOG((LOG_DISPL,1, " SCANEND"));
		m_task_wakeup &= ~(1 << task_dwt);
	}

	LOG((LOG_DISPL,1, "%s", A63_HBLANK(a63) ? " HBLANK": ""));

	if (A63_HSYNC(a63)) {
		if (!A63_HSYNC(m_dsp.a63)) {
			LOG((LOG_DISPL,1, " HSYNC↗ (CLRBUF)"));
			/*
			 * The hardware sets the buffer empty and clears the DWT block
			 * flip-flop at the beginning of horizontal retrace for
			 * every scanline.
			 */
			m_dsp.fifo_wr = 0;
			m_dsp.fifo_rd = 0;
			m_dsp.dwt_blocks = false;
			// now take the new values from the last SETMODE←
			m_dsp.inverse = GET_SETMODE_INVERSE(m_dsp.setmode) ? 0xffff : 0x0000;
			m_dsp.halfclock = GET_SETMODE_SPEEDY(m_dsp.setmode);
			// stop the CPU from calling unload_word()
			m_unload_time = -1;
		} else {
			LOG((LOG_DISPL,1, " HSYNC"));
		}
	}
	// FIXME: jiggly cursor issue; try to wake up CURT at the end of HSYNC
	if (A63_HSYNC(m_dsp.a63) && !A63_HSYNC(a63)) {
		/*
		 * CLRBUF' also resets the 2nd cursor task block flip flop,
		 * which is built from two NAND gates a30c and a30d (74H00).
		 * If both flip flops are reset, the NOR gate a20d (74S02)
		 * decodes this as WAKECURT signal.
		 */
		m_dsp.curt_wakeup = true;
	}


	LOG((LOG_DISPL,1, " NEXT:%03o\n", next));

	if (!m_dsp.curt_blocks && m_dsp.curt_wakeup)
		m_task_wakeup |= 1 << task_curt;

	m_dsp.a63 = a63;
	m_dsp.a66 = a66;
	m_dsp.state = next;
}

/**
 * @brief f2_evenfield late: branch on evenfield
 *
 * NEXT(09) = even field ? 1 : 0
 */
void alto2_cpu_device::f2_late_evenfield()
{
	UINT16 r = HLC1024 ^ 1;
	LOG((LOG_DISPL,2,"	evenfield branch on HLC1024 (%#o | %#o)\n", m_next2, r));
	m_next2 |= r;
}

/**
 * @brief initialize the display context to useful values
 *
 * Zap the display context to all 0s.
 * Allocate a bitmap array to save blitting to the screen when
 * there is no change in the data words.
 */
void alto2_cpu_device::init_disp()
{
	memset(&m_dsp, 0, sizeof(m_dsp));
	m_dsp.hlc = ALTO2_DISPLAY_HLC_START;

	m_dsp.raw_bitmap = auto_alloc_array(machine(), UINT16, ALTO2_DISPLAY_HEIGHT * ALTO2_DISPLAY_SCANLINE_WORDS);
	m_dsp.scanline = auto_alloc_array(machine(), UINT8*, ALTO2_DISPLAY_HEIGHT);
	for (int y = 0; y < ALTO2_DISPLAY_HEIGHT; y++)
		m_dsp.scanline[y] = auto_alloc_array(machine(), UINT8, ALTO2_DISPLAY_TOTAL_WIDTH);

	m_dsp.bitmap = auto_bitmap_ind16_alloc(machine(), ALTO2_DISPLAY_WIDTH, ALTO2_DISPLAY_HEIGHT);
	m_dsp.state = 020;
}

void alto2_cpu_device::exit_disp()
{
	// nothing to do yet
}

/* Video update */
UINT32 alto2_cpu_device::screen_update(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect)
{
	pen_t palette_bw[2];
	palette_bw[0] = get_white_pen(machine());
	palette_bw[1] = get_black_pen(machine());
	// copy even or odd field
	for (int y = m_dsp.odd_frame ? 0 : 1; y < ALTO2_DISPLAY_HEIGHT; y += 2)
		draw_scanline8(*m_dsp.bitmap, 0, y, ALTO2_DISPLAY_WIDTH, m_dsp.scanline[y], palette_bw);

	// copy bitmap
	copybitmap(bitmap, *m_dsp.bitmap, 0, 0, 0, 0, cliprect);
	return 0;
}

void alto2_cpu_device::screen_eof(screen_device &screen, bool state)
{
	if (state)
		m_dsp.odd_frame = !m_dsp.odd_frame;
}
