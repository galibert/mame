// license:BSD-3-Clause
// copyright-holders:Olivier Galibert
/*
  Dreamcast video emulation
*/

#include "emu.h"
#include "powervr2.h"
#include "includes/dc.h"

#include "cpu/sh/sh4.h"
#include "video/rgbutil.h"
#include "rendutil.h"


DEFINE_DEVICE_TYPE(POWERVR2, powervr2_device, "powervr2", "PowerVR 2")

void powervr2_device::ta_map(address_map &map)
{
	map(0x0000, 0x0003).r (this, FUNC(powervr2_device::id_r));
	map(0x0004, 0x0007).r (this, FUNC(powervr2_device::revision_r));
	map(0x0008, 0x000b).rw(this, FUNC(powervr2_device::softreset_r),         FUNC(powervr2_device::softreset_w));
	map(0x0014, 0x0017).rw(this, FUNC(powervr2_device::startrender_r),       FUNC(powervr2_device::startrender_w));
	map(0x0018, 0x001f).rw(this, FUNC(powervr2_device::test_select_r),       FUNC(powervr2_device::test_select_w));
	map(0x0020, 0x0023).rw(this, FUNC(powervr2_device::param_base_r),        FUNC(powervr2_device::param_base_w));
	map(0x002c, 0x002f).rw(this, FUNC(powervr2_device::region_base_r),       FUNC(powervr2_device::region_base_w));
	map(0x0030, 0x0034).rw(this, FUNC(powervr2_device::span_sort_cfg_r),     FUNC(powervr2_device::span_sort_cfg_w));
	map(0x0040, 0x0043).rw(this, FUNC(powervr2_device::vo_border_col_r),     FUNC(powervr2_device::vo_border_col_w));
	map(0x0044, 0x0047).rw(this, FUNC(powervr2_device::fb_r_ctrl_r),         FUNC(powervr2_device::fb_r_ctrl_w));
	map(0x0048, 0x004b).rw(this, FUNC(powervr2_device::fb_w_ctrl_r),         FUNC(powervr2_device::fb_w_ctrl_w));
	map(0x004c, 0x004f).rw(this, FUNC(powervr2_device::fb_w_linestride_r),   FUNC(powervr2_device::fb_w_linestride_w));
	map(0x0050, 0x0053).rw(this, FUNC(powervr2_device::fb_r_sof1_r),         FUNC(powervr2_device::fb_r_sof1_w));
	map(0x0054, 0x0057).rw(this, FUNC(powervr2_device::fb_r_sof2_r),         FUNC(powervr2_device::fb_r_sof2_w));
	map(0x005c, 0x005f).rw(this, FUNC(powervr2_device::fb_r_size_r),         FUNC(powervr2_device::fb_r_size_w));
	map(0x0060, 0x0063).rw(this, FUNC(powervr2_device::fb_w_sof1_r),         FUNC(powervr2_device::fb_w_sof1_w));
	map(0x0064, 0x0067).rw(this, FUNC(powervr2_device::fb_w_sof2_r),         FUNC(powervr2_device::fb_w_sof2_w));
	map(0x0068, 0x006b).rw(this, FUNC(powervr2_device::fb_x_clip_r),         FUNC(powervr2_device::fb_x_clip_w));
	map(0x006c, 0x006f).rw(this, FUNC(powervr2_device::fb_y_clip_r),         FUNC(powervr2_device::fb_y_clip_w));
	map(0x0074, 0x0077).rw(this, FUNC(powervr2_device::fpu_shad_scale_r),    FUNC(powervr2_device::fpu_shad_scale_w));
	map(0x0078, 0x007b).rw(this, FUNC(powervr2_device::fpu_cull_val_r),      FUNC(powervr2_device::fpu_cull_val_w));
	map(0x007c, 0x007f).rw(this, FUNC(powervr2_device::fpu_param_cfg_r),     FUNC(powervr2_device::fpu_param_cfg_w));
	map(0x0080, 0x0083).rw(this, FUNC(powervr2_device::half_offset_r),       FUNC(powervr2_device::half_offset_w));
	map(0x0084, 0x0087).rw(this, FUNC(powervr2_device::fpu_perp_val_r),      FUNC(powervr2_device::fpu_perp_val_w));
	map(0x0088, 0x008b).rw(this, FUNC(powervr2_device::isp_backgnd_d_r),     FUNC(powervr2_device::isp_backgnd_d_w));
	map(0x008c, 0x008f).rw(this, FUNC(powervr2_device::isp_backgnd_t_r),     FUNC(powervr2_device::isp_backgnd_t_w));
	map(0x0098, 0x009b).rw(this, FUNC(powervr2_device::isp_feed_cfg_r),      FUNC(powervr2_device::isp_feed_cfg_w));
	map(0x00a0, 0x00a3).rw(this, FUNC(powervr2_device::sdram_refresh_r),     FUNC(powervr2_device::sdram_refresh_w));
	map(0x00a4, 0x00a7).rw(this, FUNC(powervr2_device::sdram_arb_cfg_r),     FUNC(powervr2_device::sdram_arb_cfg_w));
	map(0x00a8, 0x00ab).rw(this, FUNC(powervr2_device::sdram_cfg_r),         FUNC(powervr2_device::sdram_cfg_w));
	map(0x00b0, 0x00b3).rw(this, FUNC(powervr2_device::fog_col_ram_r),       FUNC(powervr2_device::fog_col_ram_w));
	map(0x00b4, 0x00b7).rw(this, FUNC(powervr2_device::fog_col_vert_r),      FUNC(powervr2_device::fog_col_vert_w));
	map(0x00b8, 0x00bb).rw(this, FUNC(powervr2_device::fog_density_r),       FUNC(powervr2_device::fog_density_w));
	map(0x00bc, 0x00bf).rw(this, FUNC(powervr2_device::fog_clamp_max_r),     FUNC(powervr2_device::fog_clamp_max_w));
	map(0x00c0, 0x00c3).rw(this, FUNC(powervr2_device::fog_clamp_min_r),     FUNC(powervr2_device::fog_clamp_min_w));
	map(0x00c4, 0x00c7).rw(this, FUNC(powervr2_device::spg_trigger_pos_r),   FUNC(powervr2_device::spg_trigger_pos_w));
	map(0x00c8, 0x00cb).rw(this, FUNC(powervr2_device::spg_hblank_int_r),    FUNC(powervr2_device::spg_hblank_int_w));
	map(0x00cc, 0x00cf).rw(this, FUNC(powervr2_device::spg_vblank_int_r),    FUNC(powervr2_device::spg_vblank_int_w));
	map(0x00d0, 0x00d3).rw(this, FUNC(powervr2_device::spg_control_r),       FUNC(powervr2_device::spg_control_w));
	map(0x00d4, 0x00d7).rw(this, FUNC(powervr2_device::spg_hblank_r),        FUNC(powervr2_device::spg_hblank_w));
	map(0x00d8, 0x00db).rw(this, FUNC(powervr2_device::spg_load_r),          FUNC(powervr2_device::spg_load_w));
	map(0x00dc, 0x00df).rw(this, FUNC(powervr2_device::spg_vblank_r),        FUNC(powervr2_device::spg_vblank_w));
	map(0x00e0, 0x00e3).rw(this, FUNC(powervr2_device::spg_width_r),         FUNC(powervr2_device::spg_width_w));
	map(0x00e4, 0x00e7).rw(this, FUNC(powervr2_device::text_control_r),      FUNC(powervr2_device::text_control_w));
	map(0x00e8, 0x00eb).rw(this, FUNC(powervr2_device::vo_control_r),        FUNC(powervr2_device::vo_control_w));
	map(0x00ec, 0x00ef).rw(this, FUNC(powervr2_device::vo_startx_r),         FUNC(powervr2_device::vo_startx_w));
	map(0x00f0, 0x00f3).rw(this, FUNC(powervr2_device::vo_starty_r),         FUNC(powervr2_device::vo_starty_w));
	map(0x00f4, 0x00f7).rw(this, FUNC(powervr2_device::scaler_ctl_r),        FUNC(powervr2_device::scaler_ctl_w));
	map(0x0108, 0x010b).rw(this, FUNC(powervr2_device::pal_ram_ctrl_r),      FUNC(powervr2_device::pal_ram_ctrl_w));
	map(0x010c, 0x010f).r (this, FUNC(powervr2_device::spg_status_r));
	map(0x0110, 0x0113).rw(this, FUNC(powervr2_device::fb_burstctrl_r),      FUNC(powervr2_device::fb_burstctrl_w));
	map(0x0118, 0x011b).rw(this, FUNC(powervr2_device::y_coeff_r),           FUNC(powervr2_device::y_coeff_w));
	map(0x011c, 0x011f).rw(this, FUNC(powervr2_device::pt_alpha_ref_r),      FUNC(powervr2_device::pt_alpha_ref_w));
	map(0x0124, 0x0127).rw(this, FUNC(powervr2_device::ta_ol_base_r),        FUNC(powervr2_device::ta_ol_base_w));
	map(0x0128, 0x012b).rw(this, FUNC(powervr2_device::ta_isp_base_r),       FUNC(powervr2_device::ta_isp_base_w));
	map(0x012c, 0x012f).rw(this, FUNC(powervr2_device::ta_ol_limit_r),       FUNC(powervr2_device::ta_ol_limit_w));
	map(0x0130, 0x0133).rw(this, FUNC(powervr2_device::ta_isp_limit_r),      FUNC(powervr2_device::ta_isp_limit_w));
	map(0x0134, 0x0137).r (this, FUNC(powervr2_device::ta_next_opb_r));
	map(0x0138, 0x013b).r (this, FUNC(powervr2_device::ta_itp_current_r));
	map(0x013c, 0x013f).rw(this, FUNC(powervr2_device::ta_glob_tile_clip_r), FUNC(powervr2_device::ta_glob_tile_clip_w));
	map(0x0140, 0x0143).rw(this, FUNC(powervr2_device::ta_alloc_ctrl_r),     FUNC(powervr2_device::ta_alloc_ctrl_w));
	map(0x0144, 0x0147).rw(this, FUNC(powervr2_device::ta_list_init_r),      FUNC(powervr2_device::ta_list_init_w));
	map(0x0148, 0x014b).rw(this, FUNC(powervr2_device::ta_yuv_tex_base_r),   FUNC(powervr2_device::ta_yuv_tex_base_w));
	map(0x014c, 0x014f).rw(this, FUNC(powervr2_device::ta_yuv_tex_ctrl_r),   FUNC(powervr2_device::ta_yuv_tex_ctrl_w));
	map(0x0150, 0x0153).r (this, FUNC(powervr2_device::ta_yuv_tex_cnt_r));
	map(0x0160, 0x0163).rw(this, FUNC(powervr2_device::ta_list_cont_r),      FUNC(powervr2_device::ta_list_cont_w));
	map(0x0164, 0x0167).rw(this, FUNC(powervr2_device::ta_next_opb_init_r),  FUNC(powervr2_device::ta_next_opb_init_w));
	
	map(0x0200, 0x03ff).rw(this, FUNC(powervr2_device::fog_table_r),         FUNC(powervr2_device::fog_table_w));
	map(0x0600, 0x0f5f).r (this, FUNC(powervr2_device::ta_ol_pointers_1_r));
	map(0x1000, 0x1fff).rw(this, FUNC(powervr2_device::palette_r),           FUNC(powervr2_device::palette_w));
	map(0x2000, 0x295f).r (this, FUNC(powervr2_device::ta_ol_pointers_2_r));
}

void powervr2_device::pd_dma_map(address_map &map)
{
	map(0x00,   0x03)  .rw(this, FUNC(powervr2_device::sb_pdstap_r),         FUNC(powervr2_device::sb_pdstap_w));
	map(0x04,   0x07)  .rw(this, FUNC(powervr2_device::sb_pdstar_r),         FUNC(powervr2_device::sb_pdstar_w));
	map(0x08,   0x0b)  .rw(this, FUNC(powervr2_device::sb_pdlen_r),          FUNC(powervr2_device::sb_pdlen_w));
	map(0x0c,   0x0f)  .rw(this, FUNC(powervr2_device::sb_pddir_r),          FUNC(powervr2_device::sb_pddir_w));
	map(0x10,   0x13)  .rw(this, FUNC(powervr2_device::sb_pdtsel_r),         FUNC(powervr2_device::sb_pdtsel_w));
	map(0x14,   0x17)  .rw(this, FUNC(powervr2_device::sb_pden_r),           FUNC(powervr2_device::sb_pden_w));
	map(0x18,   0x1b)  .rw(this, FUNC(powervr2_device::sb_pdst_r),           FUNC(powervr2_device::sb_pdst_w));
	map(0x80,   0x83)  .rw(this, FUNC(powervr2_device::sb_pdapro_r),         FUNC(powervr2_device::sb_pdapro_w));
}

const int powervr2_device::pvr_parconfseq[] = {1,2,3,2,3,4,5,6,5,6,7,8,9,10,11,12,13,14,13,14,15,16,17,16,17,0,0,0,0,0,18,19,20,19,20,21,22,23,22,23};
const int powervr2_device::pvr_wordsvertex[24]  = {8,8,8,8,8,16,16,8,8,8, 8, 8,8,8,8,8,16,16, 8,16,16,8,16,16};
const int powervr2_device::pvr_wordspolygon[24] = {8,8,8,8,8, 8, 8,8,8,8,16,16,8,8,8,8, 8, 8,16,16,16,8, 8, 8};

#define DEBUG_FIFO_POLY (0)
#define DEBUG_PVRTA 0
#define DEBUG_PVRDLIST  (0)
#define DEBUG_PALRAM (0)
#define DEBUG_PVRCTRL   (0)

inline int32_t powervr2_device::clamp(int32_t in, int32_t min, int32_t max)
{
	if(in < min) return min;
	if(in > max) return max;
	return in;
}

// Perform a standard bilinear filter across four pixels
inline uint32_t powervr2_device::bilinear_filter(uint32_t c0, uint32_t c1, uint32_t c2, uint32_t c3, float u, float v)
{
	const uint32_t ui = (u * 256.0f);
	const uint32_t vi = (v * 256.0f);
	return rgbaint_t::bilinear_filter(c0, c1, c3, c2, ui, vi);
}

// Multiply with alpha value in bits 31-24
inline uint32_t powervr2_device::bla(uint32_t c, uint32_t a)
{
	a = a >> 24;
	return ((((c & 0xff00ff)*a) & 0xff00ff00) >> 8) | ((((c >> 8) & 0xff00ff)*a) & 0xff00ff00);
}

// Multiply with 1-alpha value in bits 31-24
inline uint32_t powervr2_device::blia(uint32_t c, uint32_t a)
{
	a = 0x100 - (a >> 24);
	return ((((c & 0xff00ff)*a) & 0xff00ff00) >> 8) | ((((c >> 8) & 0xff00ff)*a) & 0xff00ff00);
}

// Per-component multiply with color value
inline uint32_t powervr2_device::blc(uint32_t c1, uint32_t c2)
{
	uint32_t cr =
		(((c1 & 0x000000ff)*(c2 & 0x000000ff) & 0x0000ff00) >> 8)  |
		(((c1 & 0x0000ff00)*(c2 & 0x0000ff00) & 0xff000000) >> 16);
	c1 >>= 16;
	c2 >>= 16;
	cr |=
		(((c1 & 0x000000ff)*(c2 & 0x000000ff) & 0x0000ff00) << 8)  |
		(((c1 & 0x0000ff00)*(c2 & 0x0000ff00) & 0xff000000));
	return cr;
}

// Per-component multiply with 1-color value
inline uint32_t powervr2_device::blic(uint32_t c1, uint32_t c2)
{
	uint32_t cr =
		(((c1 & 0x000000ff)*(0x00100-(c2 & 0x000000ff)) & 0x0000ff00) >> 8)  |
		(((c1 & 0x0000ff00)*(0x10000-(c2 & 0x0000ff00)) & 0xff000000) >> 16);
	c1 >>= 16;
	c2 >>= 16;
	cr |=
		(((c1 & 0x000000ff)*(0x00100-(c2 & 0x000000ff)) & 0x0000ff00) << 8)  |
		(((c1 & 0x0000ff00)*(0x10000-(c2 & 0x0000ff00)) & 0xff000000));
	return cr;
}

// Add two colors with saturation
inline uint32_t powervr2_device::bls(uint32_t c1, uint32_t c2)
{
	uint32_t cr1, cr2;
	cr1 = (c1 & 0x00ff00ff) + (c2 & 0x00ff00ff);
	if(cr1 & 0x0000ff00)
		cr1 = (cr1 & 0xffff00ff) | 0x000000ff;
	if(cr1 & 0xff000000)
		cr1 = (cr1 & 0x00ffffff) | 0x00ff0000;

	cr2 = ((c1 >> 8) & 0x00ff00ff) + ((c2 >> 8) & 0x00ff00ff);
	if(cr2 & 0x0000ff00)
		cr2 = (cr2 & 0xffff00ff) | 0x000000ff;
	if(cr2 & 0xff000000)
		cr2 = (cr2 & 0x00ffffff) | 0x00ff0000;
	return cr1|(cr2 << 8);
}

// All 64 blending modes, 3 top bits are source mode, 3 bottom bits are destination mode
uint32_t powervr2_device::bl00(uint32_t s, uint32_t d) { return 0; }
uint32_t powervr2_device::bl01(uint32_t s, uint32_t d) { return d; }
uint32_t powervr2_device::bl02(uint32_t s, uint32_t d) { return blc(d, s); }
uint32_t powervr2_device::bl03(uint32_t s, uint32_t d) { return blic(d, s); }
uint32_t powervr2_device::bl04(uint32_t s, uint32_t d) { return bla(d, s); }
uint32_t powervr2_device::bl05(uint32_t s, uint32_t d) { return blia(d, s); }
uint32_t powervr2_device::bl06(uint32_t s, uint32_t d) { return bla(d, d); }
uint32_t powervr2_device::bl07(uint32_t s, uint32_t d) { return blia(d, d); }
uint32_t powervr2_device::bl10(uint32_t s, uint32_t d) { return s; }
uint32_t powervr2_device::bl11(uint32_t s, uint32_t d) { return bls(s, d); }
uint32_t powervr2_device::bl12(uint32_t s, uint32_t d) { return bls(s, blc(s, d)); }
uint32_t powervr2_device::bl13(uint32_t s, uint32_t d) { return bls(s, blic(s, d)); }
uint32_t powervr2_device::bl14(uint32_t s, uint32_t d) { return bls(s, bla(d, s)); }
uint32_t powervr2_device::bl15(uint32_t s, uint32_t d) { return bls(s, blia(d, s)); }
uint32_t powervr2_device::bl16(uint32_t s, uint32_t d) { return bls(s, bla(d, d)); }
uint32_t powervr2_device::bl17(uint32_t s, uint32_t d) { return bls(s, blia(d, d)); }
uint32_t powervr2_device::bl20(uint32_t s, uint32_t d) { return blc(d, s); }
uint32_t powervr2_device::bl21(uint32_t s, uint32_t d) { return bls(blc(d, s), d); }
uint32_t powervr2_device::bl22(uint32_t s, uint32_t d) { return bls(blc(d, s), blc(s, d)); }
uint32_t powervr2_device::bl23(uint32_t s, uint32_t d) { return bls(blc(d, s), blic(s, d)); }
uint32_t powervr2_device::bl24(uint32_t s, uint32_t d) { return bls(blc(d, s), bla(d, s)); }
uint32_t powervr2_device::bl25(uint32_t s, uint32_t d) { return bls(blc(d, s), blia(d, s)); }
uint32_t powervr2_device::bl26(uint32_t s, uint32_t d) { return bls(blc(d, s), bla(d, d)); }
uint32_t powervr2_device::bl27(uint32_t s, uint32_t d) { return bls(blc(d, s), blia(d, d)); }
uint32_t powervr2_device::bl30(uint32_t s, uint32_t d) { return blic(d, s); }
uint32_t powervr2_device::bl31(uint32_t s, uint32_t d) { return bls(blic(d, s), d); }
uint32_t powervr2_device::bl32(uint32_t s, uint32_t d) { return bls(blic(d, s), blc(s, d)); }
uint32_t powervr2_device::bl33(uint32_t s, uint32_t d) { return bls(blic(d, s), blic(s, d)); }
uint32_t powervr2_device::bl34(uint32_t s, uint32_t d) { return bls(blic(d, s), bla(d, s)); }
uint32_t powervr2_device::bl35(uint32_t s, uint32_t d) { return bls(blic(d, s), blia(d, s)); }
uint32_t powervr2_device::bl36(uint32_t s, uint32_t d) { return bls(blic(d, s), bla(d, d)); }
uint32_t powervr2_device::bl37(uint32_t s, uint32_t d) { return bls(blic(d, s), blia(d, d)); }
uint32_t powervr2_device::bl40(uint32_t s, uint32_t d) { return bla(s, s); }
uint32_t powervr2_device::bl41(uint32_t s, uint32_t d) { return bls(bla(s, s), d); }
uint32_t powervr2_device::bl42(uint32_t s, uint32_t d) { return bls(bla(s, s), blc(s, d)); }
uint32_t powervr2_device::bl43(uint32_t s, uint32_t d) { return bls(bla(s, s), blic(s, d)); }
uint32_t powervr2_device::bl44(uint32_t s, uint32_t d) { return bls(bla(s, s), bla(d, s)); }
uint32_t powervr2_device::bl45(uint32_t s, uint32_t d) { return bls(bla(s, s), blia(d, s)); }
uint32_t powervr2_device::bl46(uint32_t s, uint32_t d) { return bls(bla(s, s), bla(d, d)); }
uint32_t powervr2_device::bl47(uint32_t s, uint32_t d) { return bls(bla(s, s), blia(d, d)); }
uint32_t powervr2_device::bl50(uint32_t s, uint32_t d) { return blia(s, s); }
uint32_t powervr2_device::bl51(uint32_t s, uint32_t d) { return bls(blia(s, s), d); }
uint32_t powervr2_device::bl52(uint32_t s, uint32_t d) { return bls(blia(s, s), blc(s, d)); }
uint32_t powervr2_device::bl53(uint32_t s, uint32_t d) { return bls(blia(s, s), blic(s, d)); }
uint32_t powervr2_device::bl54(uint32_t s, uint32_t d) { return bls(blia(s, s), bla(d, s)); }
uint32_t powervr2_device::bl55(uint32_t s, uint32_t d) { return bls(blia(s, s), blia(d, s)); }
uint32_t powervr2_device::bl56(uint32_t s, uint32_t d) { return bls(blia(s, s), bla(d, d)); }
uint32_t powervr2_device::bl57(uint32_t s, uint32_t d) { return bls(blia(s, s), blia(d, d)); }
uint32_t powervr2_device::bl60(uint32_t s, uint32_t d) { return bla(s, d); }
uint32_t powervr2_device::bl61(uint32_t s, uint32_t d) { return bls(bla(s, d), d); }
uint32_t powervr2_device::bl62(uint32_t s, uint32_t d) { return bls(bla(s, d), blc(s, d)); }
uint32_t powervr2_device::bl63(uint32_t s, uint32_t d) { return bls(bla(s, d), blic(s, d)); }
uint32_t powervr2_device::bl64(uint32_t s, uint32_t d) { return bls(bla(s, d), bla(d, s)); }
uint32_t powervr2_device::bl65(uint32_t s, uint32_t d) { return bls(bla(s, d), blia(d, s)); }
uint32_t powervr2_device::bl66(uint32_t s, uint32_t d) { return bls(bla(s, d), bla(d, d)); }
uint32_t powervr2_device::bl67(uint32_t s, uint32_t d) { return bls(bla(s, d), blia(d, d)); }
uint32_t powervr2_device::bl70(uint32_t s, uint32_t d) { return blia(s, d); }
uint32_t powervr2_device::bl71(uint32_t s, uint32_t d) { return bls(blia(s, d), d); }
uint32_t powervr2_device::bl72(uint32_t s, uint32_t d) { return bls(blia(s, d), blc(s, d)); }
uint32_t powervr2_device::bl73(uint32_t s, uint32_t d) { return bls(blia(s, d), blic(s, d)); }
uint32_t powervr2_device::bl74(uint32_t s, uint32_t d) { return bls(blia(s, d), bla(d, s)); }
uint32_t powervr2_device::bl75(uint32_t s, uint32_t d) { return bls(blia(s, d), blia(d, s)); }
uint32_t powervr2_device::bl76(uint32_t s, uint32_t d) { return bls(blia(s, d), bla(d, d)); }
uint32_t powervr2_device::bl77(uint32_t s, uint32_t d) { return bls(blia(s, d), blia(d, d)); }

uint32_t (*const powervr2_device::blend_functions[64])(uint32_t s, uint32_t d) = {
	bl00, bl01, bl02, bl03, bl04, bl05, bl06, bl07,
	bl10, bl11, bl12, bl13, bl14, bl15, bl16, bl17,
	bl20, bl21, bl22, bl23, bl24, bl25, bl26, bl27,
	bl30, bl31, bl32, bl33, bl34, bl35, bl36, bl37,
	bl40, bl41, bl42, bl43, bl44, bl45, bl46, bl47,
	bl50, bl51, bl52, bl53, bl54, bl55, bl56, bl57,
	bl60, bl61, bl62, bl63, bl64, bl65, bl66, bl67,
	bl70, bl71, bl72, bl73, bl74, bl75, bl76, bl77,
};

inline uint32_t powervr2_device::cv_1555(uint16_t c)
{
	return
		(c & 0x8000 ? 0xff000000 : 0) |
		((c << 9) & 0x00f80000) | ((c << 4) & 0x00070000) |
		((c << 6) & 0x0000f800) | ((c << 1) & 0x00000700) |
		((c << 3) & 0x000000f8) | ((c >> 2) & 0x00000007);
}

inline uint32_t powervr2_device::cv_1555z(uint16_t c)
{
	return
		(c & 0x8000 ? 0xff000000 : 0) |
		((c << 9) & 0x00f80000) |
		((c << 6) & 0x0000f800) |
		((c << 3) & 0x000000f8);
}

inline uint32_t powervr2_device::cv_565(uint16_t c)
{
	return
		0xff000000 |
		((c << 8) & 0x00f80000) | ((c << 3) & 0x00070000) |
		((c << 5) & 0x0000fc00) | ((c >> 1) & 0x00000300) |
		((c << 3) & 0x000000f8) | ((c >> 2) & 0x00000007);
}

inline uint32_t powervr2_device::cv_565z(uint16_t c)
{
	return
		0xff000000 |
		((c << 8) & 0x00f80000) |
		((c << 5) & 0x0000fc00) |
		((c << 3) & 0x000000f8);
}

inline uint32_t powervr2_device::cv_4444(uint16_t c)
{
	return
		((c << 16) & 0xf0000000) | ((c << 12) & 0x0f000000) |
		((c << 12) & 0x00f00000) | ((c <<  8) & 0x000f0000) |
		((c <<  8) & 0x0000f000) | ((c <<  4) & 0x00000f00) |
		((c <<  4) & 0x000000f0) | ((c      ) & 0x0000000f);
}

inline uint32_t powervr2_device::cv_4444z(uint16_t c)
{
	return
		((c << 16) & 0xf0000000) |
		((c << 12) & 0x00f00000) |
		((c <<  8) & 0x0000f000) |
		((c <<  4) & 0x000000f0);
}

inline uint32_t powervr2_device::cv_yuv(uint16_t c1, uint16_t c2, int x)
{
	int u = 11*((c1 & 0xff) - 128);
	int v = 11*((c2 & 0xff) - 128);
	int y = (x & 1 ? c2 : c1) >> 8;
	int r = y + v/8;
	int g = y - u/32 - v/16;
	int b = y + (3*u)/16;
	r = r < 0 ? 0 : r > 255 ? 255 : r;
	g = g < 0 ? 0 : g > 255 ? 255 : g;
	b = b < 0 ? 0 : b > 255 ? 255 : b;
	return 0xff000000 | (r << 16) | (g << 8) | b;
}

int powervr2_device::uv_wrap(float uv, int size)
{
	int iuv = (int)uv;
	return iuv & (size - 1);
}
int powervr2_device::uv_flip(float uv, int size)
{
	int iuv = (int)uv;
	return ((iuv & size) ? ~iuv : iuv) & (size - 1);
}
int powervr2_device::uv_clamp(float uv, int size)
{
	int iuv = (int)uv;
	return (iuv > 0) ? (iuv < size) ? iuv : (size - 1) : 0;
}

uint32_t powervr2_device::tex_r_yuv_n(texinfo *t, float x, float y)
{
	int xt = t->u_func(x, t->sizex);
	int yt = t->v_func(y, t->sizey);
	int addrp = t->address + (t->stride*yt + (xt & ~1))*2;
	uint16_t c1 = *(uint16_t *)((reinterpret_cast<uint8_t *>(dc_texture_ram)) + WORD_XOR_LE(addrp));
	uint16_t c2 = *(uint16_t *)((reinterpret_cast<uint8_t *>(dc_texture_ram)) + WORD_XOR_LE(addrp+2));
	return cv_yuv(c1, c2, xt);
}

uint32_t powervr2_device::tex_r_yuv_tw(texinfo *t, float x, float y)
{
	int xt = t->u_func(x, t->sizex);
	int yt = t->v_func(y, t->sizey);
	int addrp = t->address + (dilated1[t->cd][xt & ~1] + dilated0[t->cd][yt]) * 2;
	uint16_t c1 = *(uint16_t *)((reinterpret_cast<uint8_t *>(dc_texture_ram)) + WORD_XOR_LE(addrp));
	uint16_t c2 = *(uint16_t *)((reinterpret_cast<uint8_t *>(dc_texture_ram)) + WORD_XOR_LE(addrp+4));
	return cv_yuv(c1, c2, xt);
}

#if 0
uint32_t powervr2_device::tex_r_yuv_vq(texinfo *t, float x, float y)
{
	int xt = t->u_func(x, t->sizex);
	int yt = t->v_func(y, t->sizey);
	int idx = (reinterpret_cast<uint8_t *>(dc_texture_ram))[BYTE_XOR_LE(t->address + dilated1[t->cd][xt >> 1] + dilated0[t->cd][yt >> 1])];
	int addrp = t->vqbase + 8*idx + (dilated1[t->cd][xt & 1] + dilated0[t->cd][yt & 1])*2;
	uint16_t c1 = *(uint16_t *)((reinterpret_cast<uint8_t *>(dc_texture_ram)) + WORD_XOR_LE(addrp));
	uint16_t c2 = *(uint16_t *)((reinterpret_cast<uint8_t *>(dc_texture_ram)) + WORD_XOR_LE(addrp+4));
	return cv_yuv(c1, c2, xt);
}
#endif

uint32_t powervr2_device::tex_r_1555_n(texinfo *t, float x, float y)
{
	int xt = t->u_func(x, t->sizex);
	int yt = t->v_func(y, t->sizey);
	int addrp = t->address + (t->stride*yt + xt) * 2;
	return cv_1555z(*(uint16_t *)((reinterpret_cast<uint8_t *>(dc_texture_ram)) + WORD_XOR_LE(addrp)));
}

uint32_t powervr2_device::tex_r_1555_tw(texinfo *t, float x, float y)
{
	int xt = t->u_func(x, t->sizex);
	int yt = t->v_func(y, t->sizey);
	int addrp = t->address + (dilated1[t->cd][xt] + dilated0[t->cd][yt]) * 2;
	return cv_1555(*(uint16_t *)((reinterpret_cast<uint8_t *>(dc_texture_ram)) + WORD_XOR_LE(addrp)));
}

uint32_t powervr2_device::tex_r_1555_vq(texinfo *t, float x, float y)
{
	int xt = t->u_func(x, t->sizex);
	int yt = t->v_func(y, t->sizey);
	int idx = (reinterpret_cast<uint8_t *>(dc_texture_ram))[BYTE_XOR_LE(t->address + dilated1[t->cd][xt >> 1] + dilated0[t->cd][yt >> 1])];
	int addrp = t->vqbase + 8*idx + (dilated1[t->cd][xt & 1] + dilated0[t->cd][yt & 1])*2;
	return cv_1555(*(uint16_t *)((reinterpret_cast<uint8_t *>(dc_texture_ram)) + WORD_XOR_LE(addrp)));
}

uint32_t powervr2_device::tex_r_565_n(texinfo *t, float x, float y)
{
	int xt = t->u_func(x, t->sizex);
	int yt = t->v_func(y, t->sizey);
	int addrp = t->address + (t->stride*yt + xt) * 2;
	return cv_565z(*(uint16_t *)((reinterpret_cast<uint8_t *>(dc_texture_ram)) + WORD_XOR_LE(addrp)));
}

uint32_t powervr2_device::tex_r_565_tw(texinfo *t, float x, float y)
{
	int xt = t->u_func(x, t->sizex);
	int yt = t->v_func(y, t->sizey);
	int addrp = t->address + (dilated1[t->cd][xt] + dilated0[t->cd][yt]) * 2;
	return cv_565(*(uint16_t *)((reinterpret_cast<uint8_t *>(dc_texture_ram)) + WORD_XOR_LE(addrp)));
}

uint32_t powervr2_device::tex_r_565_vq(texinfo *t, float x, float y)
{
	int xt = t->u_func(x, t->sizex);
	int yt = t->v_func(y, t->sizey);
	int idx = (reinterpret_cast<uint8_t *>(dc_texture_ram))[BYTE_XOR_LE(t->address + dilated1[t->cd][xt >> 1] + dilated0[t->cd][yt >> 1])];
	int addrp = t->vqbase + 8*idx + (dilated1[t->cd][xt & 1] + dilated0[t->cd][yt & 1])*2;
	return cv_565(*(uint16_t *)((reinterpret_cast<uint8_t *>(dc_texture_ram)) + WORD_XOR_LE(addrp)));
}

uint32_t powervr2_device::tex_r_4444_n(texinfo *t, float x, float y)
{
	int xt = t->u_func(x, t->sizex);
	int yt = t->v_func(y, t->sizey);
	int addrp = t->address + (t->stride*yt + xt) * 2;
	return cv_4444z(*(uint16_t *)((reinterpret_cast<uint8_t *>(dc_texture_ram)) + WORD_XOR_LE(addrp)));
}

uint32_t powervr2_device::tex_r_4444_tw(texinfo *t, float x, float y)
{
	int xt = t->u_func(x, t->sizex);
	int yt = t->v_func(y, t->sizey);
	int addrp = t->address + (dilated1[t->cd][xt] + dilated0[t->cd][yt]) * 2;
	return cv_4444(*(uint16_t *)((reinterpret_cast<uint8_t *>(dc_texture_ram)) + WORD_XOR_LE(addrp)));
}

uint32_t powervr2_device::tex_r_4444_vq(texinfo *t, float x, float y)
{
	int xt = t->u_func(x, t->sizex);
	int yt = t->v_func(y, t->sizey);
	int idx = (reinterpret_cast<uint8_t *>(dc_texture_ram))[BYTE_XOR_LE(t->address + dilated1[t->cd][xt >> 1] + dilated0[t->cd][yt >> 1])];
	int addrp = t->vqbase + 8*idx + (dilated1[t->cd][xt & 1] + dilated0[t->cd][yt & 1])*2;
	return cv_4444(*(uint16_t *)((reinterpret_cast<uint8_t *>(dc_texture_ram)) + WORD_XOR_LE(addrp)));
}

uint32_t powervr2_device::tex_r_nt_palint(texinfo *t, float x, float y)
{
	return t->nontextured_pal_int;
}

uint32_t powervr2_device::tex_r_nt_palfloat(texinfo *t, float x, float y)
{
	return (t->nontextured_fpal_a << 24) | (t->nontextured_fpal_r << 16) | (t->nontextured_fpal_g << 8) | (t->nontextured_fpal_b);
}

uint32_t powervr2_device::tex_r_p4_1555_tw(texinfo *t, float x, float y)
{
	int xt = t->u_func(x, t->sizex);
	int yt = t->v_func(y, t->sizey);
	int off = dilated1[t->cd][xt] + dilated0[t->cd][yt];
	int addrp = t->address + (off >> 1);
	int c = ((reinterpret_cast<uint8_t *>(dc_texture_ram))[BYTE_XOR_LE(addrp)] >> ((off & 1) << 2)) & 0xf;
	return cv_1555(palette[t->palbase + c]);
}

uint32_t powervr2_device::tex_r_p4_1555_vq(texinfo *t, float x, float y)
{
	int xt = t->u_func(x, t->sizex);
	int yt = t->v_func(y, t->sizey);
	int idx = (reinterpret_cast<uint8_t *>(dc_texture_ram))[BYTE_XOR_LE(t->address + dilated1[t->cd][xt >> 1] + dilated0[t->cd][yt >> 1])];
	int addrp = t->vqbase + 8*idx + dilated1[t->cd][xt & 1] + dilated0[t->cd][yt & 3];
	int c = (reinterpret_cast<uint8_t *>(dc_texture_ram))[BYTE_XOR_LE(addrp)] & 0xf;
	return cv_1555(palette[t->palbase + c]);
}

uint32_t powervr2_device::tex_r_p4_565_tw(texinfo *t, float x, float y)
{
	int xt = t->u_func(x, t->sizex);
	int yt = t->v_func(y, t->sizey);
	int off = dilated1[t->cd][xt] + dilated0[t->cd][yt];
	int addrp = t->address + (off >> 1);
	int c = ((reinterpret_cast<uint8_t *>(dc_texture_ram))[BYTE_XOR_LE(addrp)] >> ((off & 1) << 2)) & 0xf;
	return cv_565(palette[t->palbase + c]);
}

uint32_t powervr2_device::tex_r_p4_565_vq(texinfo *t, float x, float y)
{
	int xt = t->u_func(x, t->sizex);
	int yt = t->v_func(y, t->sizey);
	int idx = (reinterpret_cast<uint8_t *>(dc_texture_ram))[BYTE_XOR_LE(t->address + dilated1[t->cd][xt >> 1] + dilated0[t->cd][yt >> 1])];
	int addrp = t->vqbase + 8*idx + dilated1[t->cd][xt & 1] + dilated0[t->cd][yt & 3];
	int c = (reinterpret_cast<uint8_t *>(dc_texture_ram))[BYTE_XOR_LE(addrp)] & 0xf;
	return cv_565(palette[t->palbase + c]);
}

uint32_t powervr2_device::tex_r_p4_4444_tw(texinfo *t, float x, float y)
{
	int xt = t->u_func(x, t->sizex);
	int yt = t->v_func(y, t->sizey);
	int off = dilated1[t->cd][xt] + dilated0[t->cd][yt];
	int addrp = t->address + (off >> 1);
	int c = ((reinterpret_cast<uint8_t *>(dc_texture_ram))[BYTE_XOR_LE(addrp)] >> ((off & 1) << 2)) & 0xf;
	return cv_4444(palette[t->palbase + c]);
}

uint32_t powervr2_device::tex_r_p4_4444_vq(texinfo *t, float x, float y)
{
	int xt = t->u_func(x, t->sizex);
	int yt = t->v_func(y, t->sizey);
	int idx = (reinterpret_cast<uint8_t *>(dc_texture_ram))[BYTE_XOR_LE(t->address + dilated1[t->cd][xt >> 1] + dilated0[t->cd][yt >> 1])];
	int addrp = t->vqbase + 8*idx + dilated1[t->cd][xt & 1] + dilated0[t->cd][yt & 3];
	int c = (reinterpret_cast<uint8_t *>(dc_texture_ram))[BYTE_XOR_LE(addrp)] & 0xf;
	return cv_4444(palette[t->palbase + c]);
}

uint32_t powervr2_device::tex_r_p4_8888_tw(texinfo *t, float x, float y)
{
	int xt = t->u_func(x, t->sizex);
	int yt = t->v_func(y, t->sizey);
	int off = dilated1[t->cd][xt] + dilated0[t->cd][yt];
	int addrp = t->address + (off >> 1);
	int c = ((reinterpret_cast<uint8_t *>(dc_texture_ram))[BYTE_XOR_LE(addrp)] >> ((off & 1) << 2)) & 0xf;
	return palette[t->palbase + c];
}

uint32_t powervr2_device::tex_r_p4_8888_vq(texinfo *t, float x, float y)
{
	int xt = t->u_func(x, t->sizex);
	int yt = t->v_func(y, t->sizey);
	int idx = (reinterpret_cast<uint8_t *>(dc_texture_ram))[BYTE_XOR_LE(t->address + dilated1[t->cd][xt >> 1] + dilated0[t->cd][yt >> 1])];
	int addrp = t->vqbase + 8*idx + dilated1[t->cd][xt & 1] + dilated0[t->cd][yt & 3];
	int c = (reinterpret_cast<uint8_t *>(dc_texture_ram))[BYTE_XOR_LE(addrp)] & 0xf;
	return palette[t->palbase + c];
}

uint32_t powervr2_device::tex_r_p8_1555_tw(texinfo *t, float x, float y)
{
	int xt = t->u_func(x, t->sizex);
	int yt = t->v_func(y, t->sizey);
	int addrp = t->address + dilated1[t->cd][xt] + dilated0[t->cd][yt];
	int c = (reinterpret_cast<uint8_t *>(dc_texture_ram))[BYTE_XOR_LE(addrp)];
	return cv_1555(palette[t->palbase + c]);
}

uint32_t powervr2_device::tex_r_p8_1555_vq(texinfo *t, float x, float y)
{
	int xt = t->u_func(x, t->sizex);
	int yt = t->v_func(y, t->sizey);
	int idx = (reinterpret_cast<uint8_t *>(dc_texture_ram))[BYTE_XOR_LE(t->address + dilated1[t->cd][xt >> 1] + dilated0[t->cd][yt >> 1])];
	int addrp = t->vqbase + 8*idx + dilated1[t->cd][xt & 1] + dilated0[t->cd][yt & 3];
	int c = (reinterpret_cast<uint8_t *>(dc_texture_ram))[BYTE_XOR_LE(addrp)];
	return cv_1555(palette[t->palbase + c]);
}

uint32_t powervr2_device::tex_r_p8_565_tw(texinfo *t, float x, float y)
{
	int xt = t->u_func(x, t->sizex);
	int yt = t->v_func(y, t->sizey);
	int addrp = t->address + dilated1[t->cd][xt] + dilated0[t->cd][yt];
	int c = (reinterpret_cast<uint8_t *>(dc_texture_ram))[BYTE_XOR_LE(addrp)];
	return cv_565(palette[t->palbase + c]);
}

uint32_t powervr2_device::tex_r_p8_565_vq(texinfo *t, float x, float y)
{
	int xt = t->u_func(x, t->sizex);
	int yt = t->v_func(y, t->sizey);
	int idx = (reinterpret_cast<uint8_t *>(dc_texture_ram))[BYTE_XOR_LE(t->address + dilated1[t->cd][xt >> 1] + dilated0[t->cd][yt >> 1])];
	int addrp = t->vqbase + 8*idx + dilated1[t->cd][xt & 1] + dilated0[t->cd][yt & 3];
	int c = (reinterpret_cast<uint8_t *>(dc_texture_ram))[BYTE_XOR_LE(addrp)];
	return cv_565(palette[t->palbase + c]);
}

uint32_t powervr2_device::tex_r_p8_4444_tw(texinfo *t, float x, float y)
{
	int xt = t->u_func(x, t->sizex);
	int yt = t->v_func(y, t->sizey);
	int addrp = t->address + dilated1[t->cd][xt] + dilated0[t->cd][yt];
	int c = (reinterpret_cast<uint8_t *>(dc_texture_ram))[BYTE_XOR_LE(addrp)];
	return cv_4444(palette[t->palbase + c]);
}

uint32_t powervr2_device::tex_r_p8_4444_vq(texinfo *t, float x, float y)
{
	int xt = t->u_func(x, t->sizex);
	int yt = t->v_func(y, t->sizey);
	int idx = (reinterpret_cast<uint8_t *>(dc_texture_ram))[BYTE_XOR_LE(t->address + dilated1[t->cd][xt >> 1] + dilated0[t->cd][yt >> 1])];
	int addrp = t->vqbase + 8*idx + dilated1[t->cd][xt & 1] + dilated0[t->cd][yt & 3];
	int c = (reinterpret_cast<uint8_t *>(dc_texture_ram))[BYTE_XOR_LE(addrp)];
	return cv_4444(palette[t->palbase + c]);
}

uint32_t powervr2_device::tex_r_p8_8888_tw(texinfo *t, float x, float y)
{
	int xt = t->u_func(x, t->sizex);
	int yt = t->v_func(y, t->sizey);
	int addrp = t->address + dilated1[t->cd][xt] + dilated0[t->cd][yt];
	int c = (reinterpret_cast<uint8_t *>(dc_texture_ram))[BYTE_XOR_LE(addrp)];
	return palette[t->palbase + c];
}

uint32_t powervr2_device::tex_r_p8_8888_vq(texinfo *t, float x, float y)
{
	int xt = t->u_func(x, t->sizex);
	int yt = t->v_func(y, t->sizey);
	int idx = (reinterpret_cast<uint8_t *>(dc_texture_ram))[BYTE_XOR_LE(t->address + dilated1[t->cd][xt >> 1] + dilated0[t->cd][yt >> 1])];
	int addrp = t->vqbase + 8*idx + dilated1[t->cd][xt & 1] + dilated0[t->cd][yt & 3];
	int c = (reinterpret_cast<uint8_t *>(dc_texture_ram))[BYTE_XOR_LE(addrp)];
	return palette[t->palbase + c];
}


uint32_t powervr2_device::tex_r_default(texinfo *t, float x, float y)
{
	return ((int)x ^ (int)y) & 4 ? 0xffffff00 : 0xff0000ff;
}

void powervr2_device::tex_get_info(texinfo *t)
{
	int miptype = 0;

	t->textured    = texture;

	// not textured, abort.
//  if (!t->textured) return;

	t->address     = textureaddress;
	t->pf          = pixelformat;
	t->palette     = 0;

	t->mode = (vqcompressed<<1);

	// scanorder is ignored for palettized textures (palettized textures are ALWAYS twiddled)
	// (the same bits are used for palette select instead)
	if ((t->pf == 5) || (t->pf == 6))
	{
		t->palette = paletteselector;
	}
	else
	{
		t->mode |= scanorder;
	}

	/* When scan order is 1 (non-twiddled) mipmap is ignored */
	t->mipmapped  = t->mode & 1 ? 0 : mipmapped;

	// Mipmapped textures are always square, ignore v size
	if (t->mipmapped)
	{
		t->sizes = (texturesizes & 0x38) | ((texturesizes & 0x38) >> 3);
	}
	else
	{
		t->sizes = texturesizes;
	}

	t->sizex = 1 << (3+((t->sizes >> 3) & 7));
	t->sizey = 1 << (3+(t->sizes & 7));


	/* Stride select is used only in the non-twiddled case */
	t->stride = (t->mode & 1) && strideselect ? (text_control & 0x1f) << 5 : t->sizex;

	t->blend_mode  = blend_mode;
	t->filter_mode = filtermode;

	t->u_func = (clampuv & 2) ? &powervr2_device::uv_clamp : ((flipuv & 2) ? &powervr2_device::uv_flip : &powervr2_device::uv_wrap);
	t->v_func = (clampuv & 1) ? &powervr2_device::uv_clamp : ((flipuv & 1) ? &powervr2_device::uv_flip : &powervr2_device::uv_wrap);

	t->r = &powervr2_device::tex_r_default;
	t->cd = dilatechose[t->sizes];
	t->palbase = 0;
	t->vqbase = t->address;
	t->blend = use_alpha ? blend_functions[t->blend_mode] : bl10;

//  fprintf(stderr, "tex %d %d %d %d\n", t->pf, t->mode, pal_ram_ctrl, t->mipmapped);
	if(!t->textured)
	{
		t->coltype = coltype;
		switch(t->coltype) {
			case 0: // packed color
				t->nontextured_pal_int = nontextured_pal_int;
				t->r = &powervr2_device::tex_r_nt_palint;
				break;
			case 1: // floating color
				/* TODO: might be converted even earlier I believe */
				t->nontextured_fpal_a = (uint8_t)(nontextured_fpal_a * 255.0f);
				t->nontextured_fpal_r = (uint8_t)(nontextured_fpal_r * 255.0f);
				t->nontextured_fpal_g = (uint8_t)(nontextured_fpal_g * 255.0f);
				t->nontextured_fpal_b = (uint8_t)(nontextured_fpal_b * 255.0f);
				t->r = &powervr2_device::tex_r_nt_palfloat;
				break;
		}
	}
	else
	{
	switch(t->pf) {
	case 0: // 1555
		switch(t->mode) {
		case 0:  t->r = &powervr2_device::tex_r_1555_tw; miptype = 2; break;
		case 1:  t->r = &powervr2_device::tex_r_1555_n;  miptype = 2; break;
		case 2:
		case 3:  t->r = &powervr2_device::tex_r_1555_vq; miptype = 3; t->address += 0x800; break;

		default:
			//
			break;
		}
		break;

	case 1: // 565
		switch(t->mode) {
		case 0:  t->r = &powervr2_device::tex_r_565_tw; miptype = 2; break;
		case 1:  t->r = &powervr2_device::tex_r_565_n;  miptype = 2; break;
		case 2:
		case 3:  t->r = &powervr2_device::tex_r_565_vq; miptype = 3; t->address += 0x800; break;

		default:
			//
			break;
		}
		break;

	case 2: // 4444
		switch(t->mode) {
		case 0:  t->r = &powervr2_device::tex_r_4444_tw; miptype = 2; break;
		case 1:  t->r = &powervr2_device::tex_r_4444_n;  miptype = 2; break;
		case 2:
		case 3:  t->r = &powervr2_device::tex_r_4444_vq; miptype = 3; t->address += 0x800; break;

		default:
			//
			break;
		}
		break;

	case 3: // yuv422
		switch(t->mode) {
		case 0:  t->r = &powervr2_device::tex_r_yuv_tw; miptype = -1; break;
		case 1:  t->r = &powervr2_device::tex_r_yuv_n; miptype = -1; break;
		//default: t->r = &powervr2_device::tex_r_yuv_vq; miptype = -1; break;
		}
		break;

	case 4: // bumpmap
		break;

	case 5: // 4bpp palette
		t->palbase = (t->palette & 0x3f) << 4;
		switch(t->mode) {
		case 0: case 1:
			miptype = 0;

			switch(pal_ram_ctrl) {
			case 0: t->r = &powervr2_device::tex_r_p4_1555_tw; break;
			case 1: t->r = &powervr2_device::tex_r_p4_565_tw;  break;
			case 2: t->r = &powervr2_device::tex_r_p4_4444_tw; break;
			case 3: t->r = &powervr2_device::tex_r_p4_8888_tw; break;
			}
			break;
		case 2: case 3:
			miptype = 3; // ?
			switch(pal_ram_ctrl) {
			case 0: t->r = &powervr2_device::tex_r_p4_1555_vq; t->address += 0x800; break;
			case 1: t->r = &powervr2_device::tex_r_p4_565_vq;  t->address += 0x800; break;
			case 2: t->r = &powervr2_device::tex_r_p4_4444_vq; t->address += 0x800; break;
			case 3: t->r = &powervr2_device::tex_r_p4_8888_vq; t->address += 0x800; break;
			}
			break;

		default:
			//
			break;
		}
		break;

	case 6: // 8bpp palette
		t->palbase = (t->palette & 0x30) << 4;
		switch(t->mode) {
		case 0: case 1:
			miptype = 1;

			switch(pal_ram_ctrl) {
			case 0: t->r = &powervr2_device::tex_r_p8_1555_tw; break;
			case 1: t->r = &powervr2_device::tex_r_p8_565_tw; break;
			case 2: t->r = &powervr2_device::tex_r_p8_4444_tw; break;
			case 3: t->r = &powervr2_device::tex_r_p8_8888_tw; break;
			}
			break;
		case 2: case 3:
			miptype = 3; // ?
			switch(pal_ram_ctrl) {
			case 0: t->r = &powervr2_device::tex_r_p8_1555_vq; t->address += 0x800; break;
			case 1: t->r = &powervr2_device::tex_r_p8_565_vq;  t->address += 0x800; break;
			case 2: t->r = &powervr2_device::tex_r_p8_4444_vq; t->address += 0x800; break;
			case 3: t->r = &powervr2_device::tex_r_p8_8888_vq; t->address += 0x800; break;
			}
			break;

		default:
			//
			break;
		}
		break;

	case 9: // reserved
		break;
	}
	}

	if (t->mipmapped)
	{
		// full offset tables for reference,
		// we don't do mipmapping, so don't use anything < 8x8
		// first table is half-bytes

		// 4BPP palette textures
		// Texture size _4-bit_ offset value for starting address
		// 1x1          0x00003
		// 2x2          0x00004
		// 4x4          0x00008
		// 8x8          0x00018
		// 16x16        0x00058
		// 32x32        0x00158
		// 64x64        0x00558
		// 128x128      0x01558
		// 256x256      0x05558
		// 512x512      0x15558
		// 1024x1024    0x55558

		// 8BPP palette textures
		// Texture size Byte offset value for starting address
		// 1x1          0x00003
		// 2x2          0x00004
		// 4x4          0x00008
		// 8x8          0x00018
		// 16x16        0x00058
		// 32x32        0x00158
		// 64x64        0x00558
		// 128x128      0x01558
		// 256x256      0x05558
		// 512x512      0x15558
		// 1024x1024    0x55558

		// Non-palette textures
		// Texture size Byte offset value for starting address
		// 1x1          0x00006
		// 2x2          0x00008
		// 4x4          0x00010
		// 8x8          0x00030
		// 16x16        0x000B0
		// 32x32        0x002B0
		// 64x64        0x00AB0
		// 128x128      0x02AB0
		// 256x256      0x0AAB0
		// 512x512      0x2AAB0
		// 1024x1024    0xAAAB0

		// VQ textures
		// Texture size Byte offset value for starting address
		// 1x1          0x00000
		// 2x2          0x00001
		// 4x4          0x00002
		// 8x8          0x00006
		// 16x16        0x00016
		// 32x32        0x00056
		// 64x64        0x00156
		// 128x128      0x00556
		// 256x256      0x01556
		// 512x512      0x05556
		// 1024x1024    0x15556

		static const int mipmap_4_8_offset[8] = { 0x00018, 0x00058, 0x00158, 0x00558, 0x01558, 0x05558, 0x15558, 0x55558 };  // 4bpp (4bit offset) / 8bpp (8bit offset)
		static const int mipmap_np_offset[8] =  { 0x00030, 0x000B0, 0x002B0, 0x00AB0, 0x02AB0, 0x0AAB0, 0x2AAB0, 0xAAAB0 };  // nonpalette textures
		static const int mipmap_vq_offset[8] =  { 0x00006, 0x00016, 0x00056, 0x00156, 0x00556, 0x01556, 0x05556, 0x15556 };  // vq textures

		switch (miptype)
		{
			case 0: // 4bpp
				//printf("4bpp\n");
				t->address += mipmap_4_8_offset[(t->sizes)&7]>>1;
				break;

			case 1: // 8bpp
				//printf("8bpp\n");
				t->address += mipmap_4_8_offset[(t->sizes)&7];
				break;

			case 2: // nonpalette
				//printf("np\n");
				t->address += mipmap_np_offset[(t->sizes)&7];
				break;

			case 3: // vq
				//printf("vq\n");
				t->address += mipmap_vq_offset[(t->sizes)&7];
				break;
		}
	}

}

READ64_MEMBER(  powervr2_device::tex64_r )
{
	if(offset & 0x100000)
		offset ^= 0x300000;
	return (uint64_t(tex32_r(space, offset | 0x100000, mem_mask >> 32)) << 32) | tex32_r(space, offset, mem_mask);
}

WRITE64_MEMBER( powervr2_device::tex64_w )
{
	if(offset & 0x100000)
		offset ^= 0x300000;
	if(ACCESSING_BITS_0_31)
		tex32_w(space, offset           , data      , mem_mask      );
	if(ACCESSING_BITS_32_63)
		tex32_w(space, offset | 0x100000, data >> 32, mem_mask >> 32);
}

READ32_MEMBER(  powervr2_device::tex32_r )
{
	return sdram[offset];
}

WRITE32_MEMBER( powervr2_device::tex32_w )
{
	COMBINE_DATA(sdram+offset);
	//	logerror("%s: rec tex32_w %06x %08x\n", tag(), offset, sdram[offset]);
}

READ32_MEMBER( powervr2_device::id_r )
{
	return 0x17fd11db;
}

READ32_MEMBER( powervr2_device::revision_r )
{
	return 0x00000011;
}

READ32_MEMBER( powervr2_device::softreset_r )
{
	return softreset;
}

WRITE32_MEMBER( powervr2_device::softreset_w )
{
	COMBINE_DATA(&softreset);
	if (softreset & 1) {
#if DEBUG_PVRTA
		logerror("%s: TA soft reset\n", tag());
#endif
		listtype_used=0;
	}
	if (softreset & 2) {
#if DEBUG_PVRTA
		logerror("%s: Core Pipeline soft reset\n", tag());
#endif
		if (start_render_received == 1) {
			for (int a=0;a < NUM_BUFFERS;a++)
				if (grab[a].busy == 1)
					grab[a].busy = 0;

			start_render_received = 0;
		}
	}
	if (softreset & 4) {
#if DEBUG_PVRTA
		logerror("%s: sdram I/F soft reset\n", tag());
#endif
	}
}

void powervr2_device::zbuffer::clear_valid()
{
	memset(valid, 0, sizeof(valid));
}

void powervr2_device::zbuffer::set_depth_tag(float _depth, uint32_t _tag)
{
	for(int y=0; y<32; y++)
		for(int x=0; x<32; x++)
			depth[y][x] = _depth;
	for(int y=0; y<32; y++)
		for(int x=0; x<32; x++)
			tag[y][x] = _tag;
	memset(valid, 0xff, sizeof(valid));
}

void powervr2_device::isp_load_poly(const isp_buffer_t *p, isp_poly_t &poly) const
{
	poly.y_start = (p[0].a >> 19) & 0x1f;
	poly.y_end   = (p[0].a >> 24) & 0x1f;

	poly.edge_ab_a = u2f(((p[0].b << 16) | (p[0].c >> 16)) & 0xfffffff0);
	poly.edge_ab_b = u2f(((p[0].c << 12) | (p[0].d >> 20)) & 0xfffffff0);
	poly.edge_ab_c = u2f(((p[0].d <<  8) | (p[1].a >> 24)) & 0xfffffff0);

	poly.edge_bc_a = u2f(( p[1].a <<  4                  ) & 0xfffffff0);
	poly.edge_bc_b = u2f(( p[1].b                        ) & 0xfffffff0);
	poly.edge_bc_c = u2f(((p[1].b << 28) | (p[1].c >>  4)) & 0xfffffff0);

	if(p[0].a & 0x40000000) {
		poly.edge_cd_a = u2f(((p[1].c << 24) | (p[1].d >>  8)) & 0xfffffff0);
		poly.edge_cd_b = u2f(((p[1].d << 20) | (p[2].a >> 12)) & 0xfffffff0);
		poly.edge_cd_c = u2f(((p[2].a << 16) | (p[2].b >> 16)) & 0xfffffff0);

		poly.edge_da_a = u2f(((p[2].b << 12) | (p[2].c >> 20)) & 0xfffffff0);
		poly.edge_da_b = u2f(((p[2].c <<  8) | (p[2].d >> 28)) & 0xfffffff0);
		poly.edge_da_c = u2f(( p[2].d <<  4                  ) & 0xfffffff0);

		poly.face_a    = u2f(  p[3].a                          & 0xfffffff0);
		poly.face_b    = u2f(((p[3].a << 28) | (p[3].b >>  4)) & 0xfffffff0);
		poly.face_c    = u2f(((p[3].b << 24) | (p[3].c >>  8)) & 0xfffffff0);

	} else {
		poly.edge_ca_a = u2f(((p[1].c << 24) | (p[1].d >>  8)) & 0xfffffff0);
		poly.edge_ca_b = u2f(((p[1].d << 20) | (p[2].a >> 12)) & 0xfffffff0);
		poly.edge_ca_c = u2f(((p[2].a << 16) | (p[2].b >> 16)) & 0xfffffff0);

		poly.face_a    = u2f(((p[2].b << 12) | (p[2].c >> 20)) & 0xfffffff0);
		poly.face_b    = u2f(((p[2].c <<  8) | (p[2].d >> 28)) & 0xfffffff0);
		poly.face_c    = u2f(( p[2].d <<  4                  ) & 0xfffffff0);
	}

	logerror("%s: ab_a=%f ab_b=%f ab_c=%f bc_a=%f bc_b=%f bc_c=%f ca_a=%f ca_b=%f ca_c=%f f_a=%f f_b=%f f_c=%f\n", tag(),
			 poly.edge_ab_a, poly.edge_ab_b, poly.edge_ab_c,
			 poly.edge_bc_a, poly.edge_bc_b, poly.edge_bc_c,
			 poly.edge_ca_a, poly.edge_ca_b, poly.edge_ca_c,
			 poly.face_a, poly.face_b, poly.face_c);

}

void powervr2_device::isp_coverage_poly(uint32_t *coverage, const isp_buffer_t *p, isp_poly_t &poly) const
{
	memset(coverage, 0xff, 4*32);
}

void powervr2_device::isp_run(uint32_t x, uint32_t y, bool zclear, bool presort, bool flushacc)
{
	logerror("%s: isp_run(%x, %x) opaque=%d punch=%d opaquemod=%d trans=%d transmod=%d Z=%d P=%d F=%d\n",
			 tag(), x, y, isp_buffer_counts[0], isp_buffer_counts[4], isp_buffer_counts[1], isp_buffer_counts[2], isp_buffer_counts[3], zclear, presort, flushacc);
	for(int i=0; i<isp_buffer_pos; i++)
		logerror("%s: %02d: %08x %08x %08x %08x\n", tag(), i,
				 isp_buffer[i].a, isp_buffer[i].b, isp_buffer[i].c, isp_buffer[i].d);

	if(zclear && !flushacc && !isp_buffer_counts[0] && !isp_buffer_counts[4] && !isp_buffer_counts[1] && !isp_buffer_counts[2] && !isp_buffer_counts[3]) {
		logerror("%s: Need dummy tag\n", tag());
		return;
	}

	{
		uint8_t *dst = zerobuf[y*32][x*32];
#define mm(a) dst[(a)] = dst[(a)+1] = 0
		mm(0);
		mm(31*3);
		mm(31*640*3);
		mm(31*640*3+31*3);
#undef mm
	}

	zbuffer zb1;
	if(zclear)
		zb1.clear_valid();
	else {
		logerror("%s: bgtag=%08x, bgdepth=%f\n", tag(), isp_backgnd_t, u2f(isp_backgnd_d));
		zb1.set_depth_tag(u2f(isp_backgnd_d), isp_backgnd_t);
	}


	// Process opaque and if no punch-through, opaque-mod

	isp_buffer_t *p;
	p = isp_buffer;
	for(;;) {
		isp_poly_t poly;
		uint32_t coverage[32];

		isp_load_poly(p, poly);
		isp_coverage_poly(coverage, p, poly);
		break;
	}
	exit(0);
}

READ32_MEMBER( powervr2_device::startrender_r )
{
	return 0;
}

bool powervr2_device::list_has_objects(uint32_t adr) const
{
	for(;;) {
		uint32_t op = sdram[adr];
		logerror("%s:  - %08x/%08x %08x\n", tag(), adr, adr << 2, op);
		if((op & 0xf0000000) == 0xf0000000)
			return false;
		if((op & 0xfe000000) == 0x00000000)
			adr = (adr + 1) & 0x3fffff;
		else if((op & 0xe0000000) == 0xe0000000)
			adr = (op & 0xfffffc) >> 2;
		else
			return true;
	}	
}

void powervr2_device::setup_poly(setup_poly_t &poly)
{
	logerror("%s: setup_poly ciw=%08x\n", tag(), poly.ciw);
	for(int i=0; i < (poly.isquad ? 4 : 3); i++)
		logerror("%s:  %d: %08x %08x %08x %f %f %f\n", tag(), i,
				 poly.coords[i][0],
				 poly.coords[i][1],
				 poly.coords[i][2],
				 u2f(poly.coords[i][0]),
				 u2f(poly.coords[i][1]),
				 u2f(poly.coords[i][2]));

	float a00 = u2f(poly.coords[1][1]) - u2f(poly.coords[2][1]);
	float a01 = u2f(poly.coords[2][0]) - u2f(poly.coords[1][0]);
	float a02 = u2f(poly.coords[1][0])*u2f(poly.coords[2][1]) - u2f(poly.coords[2][0])*u2f(poly.coords[1][1]);

	float a10 = u2f(poly.coords[2][1]) - u2f(poly.coords[0][1]);
	float a11 = u2f(poly.coords[0][0]) - u2f(poly.coords[2][0]);
	float a12 = u2f(poly.coords[2][0])*u2f(poly.coords[0][1]) - u2f(poly.coords[0][0])*u2f(poly.coords[2][1]);

	float a20 = u2f(poly.coords[0][1]) - u2f(poly.coords[1][1]);
	float a21 = u2f(poly.coords[1][0]) - u2f(poly.coords[0][0]);
	float a22 = u2f(poly.coords[0][0])*u2f(poly.coords[1][1]) - u2f(poly.coords[1][0])*u2f(poly.coords[0][1]);

	float det = a02 + a12 + a22;
	float adet = fabsf(det);

	float ymin = u2f(poly.coords[0][1]);
	float ymax = ymin;
	for(int i=1; i < (poly.isquad ? 4 : 3); i++) {
		float y = u2f(poly.coords[i][1]);
		if(ymin > y)
			ymin = y;
		if(ymax < y + 0.5)
			ymax = y + 0.5;
	}
	if(ymin < poly.low_y)
		ymin = poly.low_y;
	if(ymax > poly.high_y)
		ymax = poly.high_y;

	uint32_t ymin1 = uint32_t(ymin) & 31;
	uint32_t ymax1 = uint32_t(ymax) & 31;

	logerror("%s:  adj %f %f %f\n", tag(), a00, a01, a02);
	logerror("%s:      %f %f %f\n", tag(), a10, a11, a12);
	logerror("%s:      %f %f %f\n", tag(), a20, a21, a22);
	logerror("%s:      %f\n", tag(), det);
	logerror("%s:      %f - %f (%f %f) -> %02x-%02x\n", tag(), ymin, ymax, poly.low_y, poly.high_y, ymin1, ymax1);

	bool culled;
	switch((poly.ciw >> 27) & 3) {
	case 0: culled = false; break;
	case 1: culled = adet <= u2f(fpu_cull_val); break;
	case 2: culled = det <= 0 || adet <= u2f(fpu_cull_val); break;
	case 3: culled = det >= 0 || adet <= u2f(fpu_cull_val); break;
	}

	if(culled)
		return;

	// Patch in the end-of-list flag
	//
	// Transiting from opaque to opaque mod and from trans
	// to trans mod does not set the end-of-list flag
	if(poly.last_object_list != poly.list_id) {
		if(!(poly.last_object_list == -1 ||
			 (poly.list_id == 3 && poly.last_object_list == 2) ||
			 (poly.list_id == 0 && poly.last_object_list == 1)))
			isp_buffer[poly.last_object_pos].a |= 0x40000000;
	}

	poly.last_object_list = poly.list_id;
	poly.last_object_pos = isp_buffer_pos;

	float edge_ab_a = a20;
	float edge_ab_b = a21;
	float edge_ab_c = poly.xo*a20 + poly.yo*a21 + a22;

	float edge_bc_a = a00;
	float edge_bc_b = a01;
	float edge_bc_c = poly.xo*a00 + poly.yo*a01 + a02;

	if(det < 0) {
		edge_ab_a = -edge_ab_a;
		edge_ab_b = -edge_ab_b;
		edge_ab_c = -edge_ab_c;

		edge_bc_a = -edge_bc_a;
		edge_bc_b = -edge_bc_b;
		edge_bc_c = -edge_bc_c;
	}


	float face_a, face_b, face_c;
	if(adet <= u2f(fpu_perp_val)) {
		face_a = 0;
		face_b = 0;
		face_c = (poly.coords[0][2] + poly.coords[1][2] + poly.coords[2][2])/3;
	} else {
		float pd = a00*u2f(poly.coords[0][2]) + a10*u2f(poly.coords[1][2]) + a20*u2f(poly.coords[2][2]);
		float qd = a01*u2f(poly.coords[0][2]) + a11*u2f(poly.coords[1][2]) + a21*u2f(poly.coords[2][2]);
		float rd = a02*u2f(poly.coords[0][2]) + a12*u2f(poly.coords[1][2]) + a22*u2f(poly.coords[2][2]);
		face_a = pd/det;
		face_b = qd/det;
		face_c = (poly.xo*pd + poly.yo*qd + rd)/det;
	}

	isp_buffer[isp_buffer_pos].a =
		                                                // last in region / last in list patched afterwards
		(poly.isquad ? 0x40000000 : 0) |                // quad
		(ymax1 << 24) |                                 // ymax in tile
		(ymin1 << 19) |                                 // ymin in tile
		((poly.list_id & 3) << 17) |                    // bottom two bits of list id
		((poly.ciw & 0xe0000000) >> (29-14)) |          // Depth comparison mode
		((poly.ciw & 0x00000001) << (12-0)) |           // Cache hint
		(poly.shadow ? 0x00000800 : 0) |                // Shadow
		((poly.op & 0x00e00000) >> (21-8)) |            // Skip
		((poly.paddr & 0x1fe000) >> 13);                // Parameter address, high bits
	if(!(poly.list_id & 1))
		isp_buffer[isp_buffer_pos].a |=
			((poly.ciw & 0x00000002) << (13-1));        // Z write disable

	isp_buffer[isp_buffer_pos].b =
		((poly.paddr & 0x001fff) << 19) |               // Parameter address, high bits
		(poly.tri << 16) |                              // Tri id in strip
		((f2u(edge_ab_a) & 0xffff0000) >> 16);          // Edge AB A high

	isp_buffer[isp_buffer_pos].c =
		((f2u(edge_ab_a) & 0x0000fff0) << (20-4)) |     // Edge AB A low
		((f2u(edge_ab_b) & 0xfffff000) >> 12);          // Edge AB B high

	isp_buffer[isp_buffer_pos].d =
		((f2u(edge_ab_b) & 0x00000ff0) << (24-4)) |     // Edge AB B low
		((f2u(edge_ab_c) & 0xffffff00) >> 8);           // Edge AB C high

	isp_buffer_pos++;

	isp_buffer[isp_buffer_pos].a =
		((f2u(edge_ab_c) & 0x000000f0) << (28-4)) |     // Edge AB C low
		((f2u(edge_bc_a) & 0xfffffff0) >> 4);           // Edge BC A

	isp_buffer[isp_buffer_pos].b =
		(f2u(edge_bc_b) & 0xfffffff0) |                 // Edge BC B
		((f2u(edge_bc_c) & 0xf0000000) >> 28);          // Edge BC C high

	isp_buffer[isp_buffer_pos].c =
		((f2u(edge_bc_c) & 0x0ffffff0) << (8-4));       // Edge BC C low

	if(poly.isquad) {
		float cd_a = poly.coords[2][1] - poly.coords[3][1];
		float cd_b = poly.coords[3][0] - poly.coords[2][0];
		float cd_c = poly.coords[2][0]*poly.coords[3][1] - poly.coords[3][0]*poly.coords[2][1];

		float edge_cd_a = cd_a;
		float edge_cd_b = cd_b;
		float edge_cd_c = poly.xo*cd_a + poly.yo*cd_b + cd_c;

		float da_a = poly.coords[3][1] - poly.coords[0][1];
		float da_b = poly.coords[0][0] - poly.coords[3][0];
		float da_c = poly.coords[3][0]*poly.coords[0][1] - poly.coords[0][0]*poly.coords[3][1];

		float edge_da_a = da_a;
		float edge_da_b = da_b;
		float edge_da_c = poly.xo*da_a + poly.yo*da_b + da_c;
		
		if(det < 0) {
			edge_cd_a = -edge_cd_a;
			edge_cd_b = -edge_cd_b;
			edge_cd_c = -edge_cd_c;

			edge_da_a = -edge_da_a;
			edge_da_b = -edge_da_b;
			edge_da_c = -edge_da_c;
		}

		isp_buffer[isp_buffer_pos].c |=
			((f2u(edge_cd_a) & 0xff000000) >> 24);      // Edge CA A high

		isp_buffer[isp_buffer_pos].d =
			((f2u(edge_cd_a) & 0x00fffff0) << 8) |      // Edge CA A low
			((f2u(edge_cd_b) & 0xfff00000) >> 20);      // Edge CA B high

		isp_buffer_pos++;

		isp_buffer[isp_buffer_pos].a =
			((f2u(edge_cd_b) & 0x000ffff0) << 12) |     // Edge CD B low
			((f2u(edge_cd_c) & 0xffff0000) >> 16);      // Edge CD C high

		isp_buffer[isp_buffer_pos].b =
			((f2u(edge_cd_c) & 0x0000fff0) << 16) |     // Edge CD C low
			((f2u(edge_da_a) & 0xfffff000) >> 12);      // Edge DA A high

		isp_buffer[isp_buffer_pos].c =
			((f2u(edge_da_a) & 0x00000ff0) << 20) |     // Edge DA A low
			((f2u(edge_da_b) & 0xffffff00) >> 8);       // Edge DA B high

		isp_buffer[isp_buffer_pos].d =
			((f2u(edge_da_b) & 0x000000f0) << 28) |     // Edge DA B low
			((f2u(edge_da_c) & 0xfffffff0) >> 4);       // Edge DA C

		isp_buffer_pos++;

		isp_buffer[isp_buffer_pos].a =
			( f2u(face_a) & 0xfffffff0       ) |        // Face A
			((f2u(face_b) & 0xf0000000) >> 28);         // Face B high

		isp_buffer[isp_buffer_pos].b =
			((f2u(face_b) & 0x0ffffff0) <<  4) |        // Face B low
			((f2u(face_c) & 0xff000000) >> 24);         // Face C high

		isp_buffer[isp_buffer_pos].c =
			((f2u(face_c) & 0x00fffff0) <<  8);         // Face C low

		isp_buffer[isp_buffer_pos].d =
			0;

		isp_buffer_pos++;

	} else {
		float edge_ca_a = a10;
		float edge_ca_b = a11;
		float edge_ca_c = poly.xo*a10 + poly.yo*a11 + a12;
		
		if(det < 0) {
			edge_ca_a = -edge_ca_a;
			edge_ca_b = -edge_ca_b;
			edge_ca_c = -edge_ca_c;
		}

		logerror("%s: ab_a=%f ab_b=%f ab_c=%f bc_a=%f bc_b=%f bc_c=%f ca_a=%f ca_b=%f ca_c=%f f_a=%f f_b=%f f_c=%f\n", tag(),
				 edge_ab_a, edge_ab_b, edge_ab_c,
				 edge_bc_a, edge_bc_b, edge_bc_c,
				 edge_ca_a, edge_ca_b, edge_ca_c,
				 face_a, face_b, face_c);

		isp_buffer[isp_buffer_pos].c |=
			((f2u(edge_ca_a) & 0xff000000) >> 24);      // Edge CA A high

		isp_buffer[isp_buffer_pos].d =
			((f2u(edge_ca_a) & 0x00fffff0) << 8) |      // Edge CA A low
			((f2u(edge_ca_b) & 0xfff00000) >> 20);      // Edge CA B high

		isp_buffer_pos++;

		isp_buffer[isp_buffer_pos].a =
			((f2u(edge_ca_b) & 0x000ffff0) << 12) |     // Edge CA B low
			((f2u(edge_ca_c) & 0xffff0000) >> 16);      // Edge CA C high

		isp_buffer[isp_buffer_pos].b =
			((f2u(edge_ca_c) & 0x0000fff0) << 16) |     // Edge CA C low
			((f2u(face_a) & 0xfffff000) >> 12);         // Face A high

		isp_buffer[isp_buffer_pos].c =
			((f2u(face_a) & 0x00000ff0) << 20) |        // Face A low
			((f2u(face_b) & 0xffffff00) >> 8);          // Face B high

		isp_buffer[isp_buffer_pos].d =
			((f2u(face_b) & 0x000000f0) << 28) |        // Face B low
			((f2u(face_c) & 0xfffffff0) >> 4);          // Face C

		isp_buffer_pos++;
	}
}

void powervr2_device::setup_object(setup_poly_t &poly)
{
	uint32_t paddr = poly.op & 0x1fffff;
	poly.shadow = poly.op & 0x01000000;
	poly.shadow_2vol = poly.shadow && (fpu_shad_scale & 0x00000100);
	poly.paddr = paddr;

	logerror("%s:  object %08x param %08x\n", tag(), poly.op, paddr);
	if(!(poly.op & 0x80000000)) {
		// Strip
		poly.ciw = sdram[paddr++];
		poly.isquad = false;
		uint32_t vsize = 3 + (poly.shadow_2vol ? 2 : 1)*((poly.op >> 21) & 7);
		uint32_t mask = (poly.op >> 25) & 0x3f;
		paddr += poly.shadow_2vol ? 4 : 2;
		paddr &= 0x1fffff;

		for(int tri=0; tri<6; tri++)
			if(mask & (0x20 >> tri)) {
				poly.tri = tri;
				uint32_t pbase = paddr + tri*vsize;
				if(tri & 1) {
					poly.coords[1][0] = sdram[ pbase            & 0x3fffff];
					poly.coords[1][1] = sdram[(pbase        +1) & 0x3fffff];
					poly.coords[1][2] = sdram[(pbase        +2) & 0x3fffff];
					poly.coords[0][0] = sdram[(pbase+  vsize)   & 0x3fffff];
					poly.coords[0][1] = sdram[(pbase+  vsize+1) & 0x3fffff];
					poly.coords[0][2] = sdram[(pbase+  vsize+2) & 0x3fffff];
					poly.coords[2][0] = sdram[(pbase+2*vsize)   & 0x3fffff];
					poly.coords[2][1] = sdram[(pbase+2*vsize+1) & 0x3fffff];
					poly.coords[2][2] = sdram[(pbase+2*vsize+2) & 0x3fffff];					
				} else {
					poly.coords[0][0] = sdram[ pbase            & 0x3fffff];
					poly.coords[0][1] = sdram[(pbase        +1) & 0x3fffff];
					poly.coords[0][2] = sdram[(pbase        +2) & 0x3fffff];
					poly.coords[1][0] = sdram[(pbase+  vsize)   & 0x3fffff];
					poly.coords[1][1] = sdram[(pbase+  vsize+1) & 0x3fffff];
					poly.coords[1][2] = sdram[(pbase+  vsize+2) & 0x3fffff];
					poly.coords[2][0] = sdram[(pbase+2*vsize)   & 0x3fffff];
					poly.coords[2][1] = sdram[(pbase+2*vsize+1) & 0x3fffff];
					poly.coords[2][2] = sdram[(pbase+2*vsize+2) & 0x3fffff];
				}
				setup_poly(poly);
			}

	} else {
		logerror("%s: array\n", tag());
		//		exit(0);
	}
}

WRITE32_MEMBER( powervr2_device::startrender_w )
{
	static const int list_order[5] = { 0, 4, 1, 2, 3 };
	logerror("%s: rec startrender_w %08x\n", tag(), data);

	{
		uint8_t *dest = zerobuf[0][0];
		for(int i=0; i<640*480; i++) {
			*dest++ = 0xff;
			*dest++ = 0xff;
			*dest++ = 0xff;
		}
	}		

	uint32_t region_data = (region_base >> 2) & 0x3fffff;
	bool alt_mode = !(fpu_param_cfg & 0x00200000);
	int num_list = alt_mode ? 4 : 5;
	for(;;) {
		setup_poly_t poly;
		uint32_t head = sdram[region_data];
		int xtile = (head & 0xfc) >> 2;
		int ytile = (head & 0x3f00) >> 8;
		bool zclear = alt_mode ? false : head & 0x40000000;
		bool presort = alt_mode ? isp_feed_cfg & 0x00000001 : head & 0x20000000;
		bool flushacc = alt_mode ? false : head & 0x10000000;
		bool empty_region = true;

		poly.low_y = ytile << 5;
		poly.high_y = poly.low_y + 31;

		poly.xo = xtile << 5;
		poly.yo = poly.low_y;
		if(half_offset) {
			poly.xo += 0.5;
			poly.yo += 0.5;
		}

		for(int i=0; i<5; i++)
			isp_buffer_counts[i] = 0;
		isp_buffer_pos = 0;

		logerror("%s: xt %d yt %d zc %d ps %d fa %d %s\n",
				 tag(), xtile, ytile, zclear, presort, flushacc, head & 0x80000000 ? "end" : "next");

		region_data++;
		bool has_objects[5] = { false, false, false, false, false };
		for(int list_idx = 0; list_idx < num_list; list_idx++) {
			int list_id = alt_mode ? list_idx : list_order[list_idx];
			uint32_t base_pointer = sdram[region_data + list_id];
			has_objects[list_id] = base_pointer & 0x80000000 ? false : list_has_objects(base_pointer >> 2);
			empty_region = empty_region && !has_objects[list_id];
			logerror("%s:  %d: %08x (%08x) %s\n", tag(), list_id, sdram[region_data + list_id], 4*(region_data + list_id), has_objects[list_id] ? "YES" : "NO");
		}

		poly.last_object_pos = 0;
		poly.last_object_list = -1;

		for(int list_idx = 0; list_idx < num_list; list_idx++) {
			poly.list_id = alt_mode ? list_idx : list_order[list_idx];
			uint32_t base_pointer = sdram[region_data + poly.list_id];
			if(base_pointer & 0x80000000)
				continue;

			uint32_t start_pos = isp_buffer_pos;
			uint32_t adr = base_pointer >> 2;
			for(;;) {
				poly.op = sdram[adr];
				if((poly.op & 0xf0000000) == 0xf0000000)
					break;
				if((poly.op & 0xe0000000) == 0xe0000000) {
					adr = (poly.op & 0xfffffc) >> 2;
					continue;
				}
				if((poly.op & 0xfe000000) == 0x00000000) {
					adr = (adr + 1) & 0x3fffff;
					continue;
				}

				setup_object(poly);
				adr = (adr + 1) & 0x3fffff;
			}
			isp_buffer_counts[poly.list_id] += isp_buffer_pos - start_pos;
		}

		// Set end-of-list and end-of-region on the last object
		if(poly.last_object_list != -1)
			isp_buffer[poly.last_object_pos].a |= 0xc0000000;

		isp_run(xtile, ytile, zclear, presort, flushacc);

		region_data += num_list;

		if(head & 0x80000000)
			break;
	}

//          printf("ISP START %d %d\n",sanitycount,m_screen->vpos());
			/* Fire ISP irq after a set amount of time TODO: timing of this */
	//			endofrender_timer_isp->adjust(state->m_maincpu->cycles_to_attotime(sanitycount*25 + 500000));	// hacky end of render delay for Capcom games, otherwise they works at ~1/10 speed
	endofrender_timer_isp->adjust(machine().driver_data<dc_state>()->m_maincpu->cycles_to_attotime(2500));
}

READ32_MEMBER( powervr2_device::test_select_r )
{
	return test_select;
}

WRITE32_MEMBER( powervr2_device::test_select_w )
{
	logerror("%s: rec test_select_w %08x\n", tag(), data);
	COMBINE_DATA(&test_select);
}

READ32_MEMBER( powervr2_device::param_base_r )
{
	return param_base;
}

WRITE32_MEMBER( powervr2_device::param_base_w )
{
	logerror("%s: rec param_base_w %08x\n", tag(), data);
	COMBINE_DATA(&param_base);
}

READ32_MEMBER( powervr2_device::region_base_r )
{
	return region_base;
}

WRITE32_MEMBER( powervr2_device::region_base_w )
{
	logerror("%s: rec region_base_w %08x\n", tag(), data);
	COMBINE_DATA(&region_base);
}

READ32_MEMBER( powervr2_device::span_sort_cfg_r )
{
	return span_sort_cfg;
}

WRITE32_MEMBER( powervr2_device::span_sort_cfg_w )
{
	logerror("%s: rec span_sort_cfg_w %08x\n", tag(), data);
	COMBINE_DATA(&span_sort_cfg);
}

READ32_MEMBER( powervr2_device::vo_border_col_r )
{
	return vo_border_col;
}

WRITE32_MEMBER( powervr2_device::vo_border_col_w )
{
	logerror("%s: rec vo_border_col_w %08x\n", tag(), data);
	COMBINE_DATA(&vo_border_col);
}

READ32_MEMBER( powervr2_device::fb_r_ctrl_r )
{
	return fb_r_ctrl;
}

WRITE32_MEMBER( powervr2_device::fb_r_ctrl_w )
{
	logerror("%s: rec fb_r_ctrl_w %08x\n", tag(), data);
	COMBINE_DATA(&fb_r_ctrl);
}

READ32_MEMBER( powervr2_device::fb_w_ctrl_r )
{
	return fb_w_ctrl;
}

WRITE32_MEMBER( powervr2_device::fb_w_ctrl_w )
{
	logerror("%s: rec fb_w_ctrl_w %08x\n", tag(), data);
	COMBINE_DATA(&fb_w_ctrl);
}

READ32_MEMBER( powervr2_device::fb_w_linestride_r )
{
	return fb_w_linestride;
}

WRITE32_MEMBER( powervr2_device::fb_w_linestride_w )
{
	logerror("%s: rec fb_w_linestride_w %08x\n", tag(), data);
	COMBINE_DATA(&fb_w_linestride);
}

READ32_MEMBER( powervr2_device::fb_r_sof1_r )
{
	return fb_r_sof1;
}

WRITE32_MEMBER( powervr2_device::fb_r_sof1_w )
{
	logerror("%s: rec fb_r_sof1_w %08x\n", tag(), data);
	COMBINE_DATA(&fb_r_sof1);
}

READ32_MEMBER( powervr2_device::fb_r_sof2_r )
{
	return fb_r_sof2;
}

WRITE32_MEMBER( powervr2_device::fb_r_sof2_w )
{
	logerror("%s: rec fb_r_sof2_w %08x\n", tag(), data);
	COMBINE_DATA(&fb_r_sof2);
}

READ32_MEMBER( powervr2_device::fb_r_size_r )
{
	return fb_r_size;
}

WRITE32_MEMBER( powervr2_device::fb_r_size_w )
{
	logerror("%s: rec fb_r_size_w %08x\n", tag(), data);
	COMBINE_DATA(&fb_r_size);
}

READ32_MEMBER( powervr2_device::fb_w_sof1_r )
{
	return fb_w_sof1;
}

WRITE32_MEMBER( powervr2_device::fb_w_sof1_w )
{
	logerror("%s: rec fb_w_sof1_w %08x\n", tag(), data);
	COMBINE_DATA(&fb_w_sof1);
}

READ32_MEMBER( powervr2_device::fb_w_sof2_r )
{
	return fb_w_sof2;
}

WRITE32_MEMBER( powervr2_device::fb_w_sof2_w )
{
	logerror("%s: rec fb_w_sof2_w %08x\n", tag(), data);
	COMBINE_DATA(&fb_w_sof2);
}

READ32_MEMBER( powervr2_device::fb_x_clip_r )
{
	return fb_x_clip;
}

WRITE32_MEMBER( powervr2_device::fb_x_clip_w )
{
	logerror("%s: rec fb_x_clip_w %08x\n", tag(), data);
	COMBINE_DATA(&fb_x_clip);
}

READ32_MEMBER( powervr2_device::fb_y_clip_r )
{
	return fb_y_clip;
}

WRITE32_MEMBER( powervr2_device::fb_y_clip_w )
{
	logerror("%s: rec fb_y_clip_w %08x\n", tag(), data);
	COMBINE_DATA(&fb_y_clip);
}

READ32_MEMBER( powervr2_device::fpu_shad_scale_r )
{
	return fpu_shad_scale;
}

WRITE32_MEMBER( powervr2_device::fpu_shad_scale_w )
{
	logerror("%s: rec fpu_shad_scale_w %08x\n", tag(), data);
	COMBINE_DATA(&fpu_shad_scale);
}

READ32_MEMBER( powervr2_device::fpu_cull_val_r )
{
	return fpu_cull_val;
}

WRITE32_MEMBER( powervr2_device::fpu_cull_val_w )
{
	logerror("%s: rec fpu_cull_val_w %08x\n", tag(), data);
	COMBINE_DATA(&fpu_cull_val);
}

READ32_MEMBER( powervr2_device::fpu_param_cfg_r )
{
	return fpu_param_cfg;
}

WRITE32_MEMBER( powervr2_device::fpu_param_cfg_w )
{
	logerror("%s: rec fpu_param_cfg_w %08x\n", tag(), data);
	COMBINE_DATA(&fpu_param_cfg);
}

READ32_MEMBER( powervr2_device::half_offset_r )
{
	return half_offset;
}

WRITE32_MEMBER( powervr2_device::half_offset_w )
{
	logerror("%s: rec half_offset_w %08x\n", tag(), data);
	COMBINE_DATA(&half_offset);
}

READ32_MEMBER( powervr2_device::fpu_perp_val_r )
{
	return fpu_perp_val;
}

WRITE32_MEMBER( powervr2_device::fpu_perp_val_w )
{
	logerror("%s: rec fpu_perp_val_w %08x\n", tag(), data);
	COMBINE_DATA(&fpu_perp_val);
}

READ32_MEMBER( powervr2_device::isp_backgnd_d_r )
{
	return isp_backgnd_d;
}

WRITE32_MEMBER( powervr2_device::isp_backgnd_d_w )
{
	logerror("%s: rec isp_backgnd_d_w %08x\n", tag(), data);
	COMBINE_DATA(&isp_backgnd_d);
}

READ32_MEMBER( powervr2_device::isp_backgnd_t_r )
{
	return isp_backgnd_t;
}

WRITE32_MEMBER( powervr2_device::isp_backgnd_t_w )
{
	logerror("%s: rec isp_backgnd_t_w %08x\n", tag(), data);
	COMBINE_DATA(&isp_backgnd_t);
}

READ32_MEMBER( powervr2_device::isp_feed_cfg_r )
{
	return isp_feed_cfg;
}

WRITE32_MEMBER( powervr2_device::isp_feed_cfg_w )
{
	logerror("%s: rec isp_feed_cfg_w %08x\n", tag(), data);
	COMBINE_DATA(&isp_feed_cfg);
}

READ32_MEMBER( powervr2_device::sdram_refresh_r )
{
	return sdram_refresh;
}

WRITE32_MEMBER( powervr2_device::sdram_refresh_w )
{
	logerror("%s: rec sdram_refresh_w %08x\n", tag(), data);
	COMBINE_DATA(&sdram_refresh);
}

READ32_MEMBER( powervr2_device::sdram_arb_cfg_r )
{
	return sdram_arb_cfg;
}

WRITE32_MEMBER( powervr2_device::sdram_arb_cfg_w )
{
	logerror("%s: rec sdram_arb_cfg_w %08x\n", tag(), data);
	COMBINE_DATA(&sdram_arb_cfg);
}

READ32_MEMBER( powervr2_device::sdram_cfg_r )
{
	return sdram_cfg;
}

WRITE32_MEMBER( powervr2_device::sdram_cfg_w )
{
	logerror("%s: rec sdram_cfg_w %08x\n", tag(), data);
	COMBINE_DATA(&sdram_cfg);
}

READ32_MEMBER( powervr2_device::fog_col_ram_r )
{
	return fog_col_ram;
}

WRITE32_MEMBER( powervr2_device::fog_col_ram_w )
{
	logerror("%s: rec fog_col_ram_w %08x\n", tag(), data);
	COMBINE_DATA(&fog_col_ram);
}

READ32_MEMBER( powervr2_device::fog_col_vert_r )
{
	return fog_col_vert;
}

WRITE32_MEMBER( powervr2_device::fog_col_vert_w )
{
	logerror("%s: rec fog_col_vert_w %08x\n", tag(), data);
	COMBINE_DATA(&fog_col_vert);
}

READ32_MEMBER( powervr2_device::fog_density_r )
{
	return fog_density;
}

WRITE32_MEMBER( powervr2_device::fog_density_w )
{
	logerror("%s: rec fog_density_w %08x\n", tag(), data);
	COMBINE_DATA(&fog_density);
}

READ32_MEMBER( powervr2_device::fog_clamp_max_r )
{
	return fog_clamp_max;
}

WRITE32_MEMBER( powervr2_device::fog_clamp_max_w )
{
	logerror("%s: rec fog_clamp_max_w %08x\n", tag(), data);
	COMBINE_DATA(&fog_clamp_max);
}

READ32_MEMBER( powervr2_device::fog_clamp_min_r )
{
	return fog_clamp_min;
}

WRITE32_MEMBER( powervr2_device::fog_clamp_min_w )
{
	logerror("%s: rec fog_clamp_min_w %08x\n", tag(), data);
	COMBINE_DATA(&fog_clamp_min);
}

READ32_MEMBER( powervr2_device::spg_trigger_pos_r )
{
	return spg_trigger_pos;
}

WRITE32_MEMBER( powervr2_device::spg_trigger_pos_w )
{
	logerror("%s: rec spg_trigger_pos_w %08x\n", tag(), data);
	COMBINE_DATA(&spg_trigger_pos);
}

READ32_MEMBER( powervr2_device::spg_hblank_int_r )
{
	return spg_hblank_int;
}

WRITE32_MEMBER( powervr2_device::spg_hblank_int_w )
{
	logerror("%s: rec spg_hblank_int_w %08x\n", tag(), data);
	COMBINE_DATA(&spg_hblank_int);
	/* TODO: timer adjust */
}

READ32_MEMBER( powervr2_device::spg_vblank_int_r )
{
	return spg_vblank_int;
}

WRITE32_MEMBER( powervr2_device::spg_vblank_int_w )
{
	logerror("%s: rec spg_vblank_int_w %08x\n", tag(), data);
	COMBINE_DATA(&spg_vblank_int);

	/* clear pending irqs and modify them with the updated ones */
//  vbin_timer->adjust(attotime::never);
//  vbout_timer->adjust(attotime::never);

//  vbin_timer->adjust(screen().time_until_pos(spg_vblank_int & 0x3ff));
//  vbout_timer->adjust(screen().time_until_pos((spg_vblank_int >> 16) & 0x3ff));
}

READ32_MEMBER( powervr2_device::spg_control_r )
{
	return spg_control;
}

WRITE32_MEMBER( powervr2_device::spg_control_w )
{
	logerror("%s: rec spg_control_w %08x\n", tag(), data);
	COMBINE_DATA(&spg_control);
	update_screen_format();

	if((spg_control & 0xc0) == 0xc0)
		popmessage("SPG undocumented pixel clock mode 11, contact MAME/MESSdev");

	if((spg_control & 0xd0) == 0x10)
		popmessage("SPG enabled VGA mode with interlace, contact MAME/MESSdev");
}

READ32_MEMBER( powervr2_device::spg_hblank_r )
{
	return spg_hblank;
}

WRITE32_MEMBER( powervr2_device::spg_hblank_w )
{
	logerror("%s: rec spg_hblank_w %08x\n", tag(), data);
	COMBINE_DATA(&spg_hblank);
	update_screen_format();
}

READ32_MEMBER( powervr2_device::spg_load_r )
{
	return spg_load;
}

WRITE32_MEMBER( powervr2_device::spg_load_w )
{
	logerror("%s: rec spg_load_w %08x\n", tag(), data);
	COMBINE_DATA(&spg_load);
	update_screen_format();
}

READ32_MEMBER( powervr2_device::spg_vblank_r )
{
	return spg_vblank;
}

WRITE32_MEMBER( powervr2_device::spg_vblank_w )
{
	logerror("%s: rec spg_vblank_w %08x\n", tag(), data);
	COMBINE_DATA(&spg_vblank);
	update_screen_format();
}

READ32_MEMBER( powervr2_device::spg_width_r )
{
	return spg_width;
}

WRITE32_MEMBER( powervr2_device::spg_width_w )
{
	logerror("%s: rec spg_width_w %08x\n", tag(), data);
	COMBINE_DATA(&spg_width);
	update_screen_format();
}

READ32_MEMBER( powervr2_device::text_control_r )
{
	return text_control;
}

WRITE32_MEMBER( powervr2_device::text_control_w )
{
	logerror("%s: rec text_control_w %08x\n", tag(), data);
	COMBINE_DATA(&text_control);
}

READ32_MEMBER( powervr2_device::vo_control_r )
{
	return vo_control;
}

WRITE32_MEMBER( powervr2_device::vo_control_w )
{
	logerror("%s: rec vo_control_w %08x\n", tag(), data);
	COMBINE_DATA(&vo_control);
}

READ32_MEMBER( powervr2_device::vo_startx_r )
{
	return vo_startx;
}

WRITE32_MEMBER( powervr2_device::vo_startx_w )
{
	logerror("%s: rec vo_startx_w %08x\n", tag(), data);
	COMBINE_DATA(&vo_startx);
	update_screen_format();
}

READ32_MEMBER( powervr2_device::vo_starty_r )
{
	return vo_starty;
}

WRITE32_MEMBER( powervr2_device::vo_starty_w )
{
	logerror("%s: rec vo_starty_w %08x\n", tag(), data);
	COMBINE_DATA(&vo_starty);
	update_screen_format();
}

READ32_MEMBER( powervr2_device::scaler_ctl_r )
{
	return scaler_ctl;
}

WRITE32_MEMBER( powervr2_device::scaler_ctl_w )
{
	logerror("%s: rec scaler_ctl_w %08x\n", tag(), data);
	COMBINE_DATA(&scaler_ctl);
}

READ32_MEMBER( powervr2_device::pal_ram_ctrl_r )
{
	return pal_ram_ctrl;
}

WRITE32_MEMBER( powervr2_device::pal_ram_ctrl_w )
{
	logerror("%s: rec pal_ram_ctrl_w %08x\n", tag(), data);
	COMBINE_DATA(&pal_ram_ctrl);
}

READ32_MEMBER( powervr2_device::spg_status_r )
{
	uint32_t fieldnum = (screen().frame_number() & 1) ? 1 : 0;
	int32_t spg_hbstart = spg_hblank & 0x3ff;
	int32_t spg_hbend = (spg_hblank >> 16) & 0x3ff;
	int32_t spg_vbstart = spg_vblank & 0x3ff;
	int32_t spg_vbend = (spg_vblank >> 16) & 0x3ff;

	uint32_t vsync = ((screen().vpos() >= spg_vbstart) || (screen().vpos() < spg_vbend)) ? 0 : 1;
	uint32_t hsync = ((screen().hpos() >= spg_hbstart) || (screen().hpos() < spg_hbend)) ? 0 : 1;
	/* FIXME: following is just a wild guess */
	uint32_t blank = ((screen().vpos() >= spg_vbstart) || (screen().vpos() < spg_vbend) |
					(screen().hpos() >= spg_hbstart) || (screen().hpos() < spg_hbend)) ? 0 : 1;
	if(vo_control & 4) { blank^=1; }
	if(vo_control & 2) { vsync^=1; }
	if(vo_control & 1) { hsync^=1; }

	return (vsync << 13) | (hsync << 12) | (blank << 11) | (fieldnum << 10) | (screen().vpos() & 0x3ff);
}

READ32_MEMBER( powervr2_device::fb_burstctrl_r )
{
	return fb_burstctrl;
}

WRITE32_MEMBER( powervr2_device::fb_burstctrl_w )
{
	logerror("%s: rec fb_burstctrl_w %08x\n", tag(), data);
	COMBINE_DATA(&fb_burstctrl);
}

READ32_MEMBER( powervr2_device::y_coeff_r )
{
	return y_coeff;
}

WRITE32_MEMBER( powervr2_device::y_coeff_w )
{
	logerror("%s: rec y_coeff_w %08x\n", tag(), data);
	COMBINE_DATA(&y_coeff);
}

READ32_MEMBER( powervr2_device::pt_alpha_ref_r )
{
	return pt_alpha_ref;
}

WRITE32_MEMBER( powervr2_device::pt_alpha_ref_w )
{
	logerror("%s: rec pt_alpha_ref_w %08x\n", tag(), data);
	COMBINE_DATA(&pt_alpha_ref);
}

READ32_MEMBER( powervr2_device::ta_ol_base_r )
{
	return ta_ol_base;
}

WRITE32_MEMBER( powervr2_device::ta_ol_base_w )
{
	logerror("%s: rec ta_ol_base_w %08x\n", tag(), data);
	COMBINE_DATA(&ta_ol_base);
}

READ32_MEMBER( powervr2_device::ta_isp_base_r )
{
	return ta_isp_base;
}

WRITE32_MEMBER( powervr2_device::ta_isp_base_w )
{
	logerror("%s: rec ta_isp_base_w %08x\n", tag(), data);
	COMBINE_DATA(&ta_isp_base);
}

READ32_MEMBER( powervr2_device::ta_ol_limit_r )
{
	return ta_ol_limit;
}

WRITE32_MEMBER( powervr2_device::ta_ol_limit_w )
{
	logerror("%s: rec ta_ol_limit_w %08x\n", tag(), data);
	COMBINE_DATA(&ta_ol_limit);
}

READ32_MEMBER( powervr2_device::ta_isp_limit_r )
{
	return ta_isp_limit;
}

WRITE32_MEMBER( powervr2_device::ta_isp_limit_w )
{
	logerror("%s: rec ta_isp_limit_w %08x\n", tag(), data);
	COMBINE_DATA(&ta_isp_limit);
}

READ32_MEMBER( powervr2_device::ta_next_opb_r )
{
	return ta_next_opb;
}

READ32_MEMBER( powervr2_device::ta_itp_current_r )
{
	return ta_itp_current;
}

READ32_MEMBER( powervr2_device::ta_glob_tile_clip_r )
{
	return ta_glob_tile_clip;
}

WRITE32_MEMBER( powervr2_device::ta_glob_tile_clip_w )
{
	logerror("%s: rec ta_glob_tile_clip_w %08x\n", tag(), data);
	COMBINE_DATA(&ta_glob_tile_clip);
}

READ32_MEMBER( powervr2_device::ta_alloc_ctrl_r )
{
	return ta_alloc_ctrl;
}

WRITE32_MEMBER( powervr2_device::ta_alloc_ctrl_w )
{
	logerror("%s: rec ta_alloc_ctrl_w %08x\n", tag(), data);
	COMBINE_DATA(&ta_alloc_ctrl);
}

// Dummy read required after write for internal powervr reasons says the manual
READ32_MEMBER( powervr2_device::ta_list_init_r )
{
	return 0;
}

void powervr2_device::ta_list_start()
{
	ta_list_idx = -1;
	ta_next_list();
	ta_olist_pos_base = ta_olist_pos = ta_ol_base;
	memset(ta_ol_pointers_1, 0, sizeof(ta_ol_pointers_1));
}

WRITE32_MEMBER( powervr2_device::ta_list_init_w )
{
	logerror("%s: rec ta_list_init_w %08x\n", tag(), data);
	if(data & 0x80000000) {
		ta_next_opb = ta_next_opb_init;
		ta_itp_current = ta_isp_base;
		ta_list_start();
	}

#if 0
	if(data & 0x80000000) {
		tafifo_pos=0;
		tafifo_mask=7;
		tafifo_vertexwords=8;
		tafifo_listtype= -1;
#if DEBUG_PVRTA
		logerror("%s: list init ol=(%08x, %08x) isp=(%08x, %08x), alloc=%08x obp=%08x\n",
					tag(), ta_ol_base, ta_ol_limit, ta_isp_base, ta_isp_limit, ta_alloc_ctrl, ta_next_opb_init);
#endif
		ta_next_opb = ta_next_opb_init;
		ta_itp_current = ta_isp_base;
		alloc_ctrl_OPB_Mode = ta_alloc_ctrl & 0x100000; // 0 up 1 down
		alloc_ctrl_PT_OPB = (4 << ((ta_alloc_ctrl >> 16) & 3)) & 0x38; // number of 32 bit words (0,8,16,32)
		alloc_ctrl_TM_OPB = (4 << ((ta_alloc_ctrl >> 12) & 3)) & 0x38;
		alloc_ctrl_T_OPB = (4 << ((ta_alloc_ctrl >> 8) & 3)) & 0x38;
		alloc_ctrl_OM_OPB = (4 << ((ta_alloc_ctrl >> 4) & 3)) & 0x38;
		alloc_ctrl_O_OPB = (4 << ((ta_alloc_ctrl >> 0) & 3)) & 0x38;
		listtype_used |= (1+4);
		// use ta_isp_base and select buffer for grab data
		grabsel = -1;
		// try to find already used buffer but not busy
		for (int a=0;a < NUM_BUFFERS;a++)
			if ((grab[a].ispbase == ta_isp_base) && (grab[a].busy == 0) && (grab[a].valid == 1)) {
				grabsel=a;
				break;
			}

		// try a buffer not used yet
		if (grabsel < 0)
			for (int a=0;a < NUM_BUFFERS;a++)
				if (grab[a].valid == 0) {
					grabsel=a;
					break;
				}

		// find a non busy buffer starting from the last one used
		if (grabsel < 0)
			for (int a=0;a < 3;a++)
				if (grab[(grabsellast+1+a) & 3].busy == 0) {
					grabsel=a;
					break;
				}

		if (grabsel < 0)
			assert_always(0, "TA grabber error B!\n");
		grabsellast=grabsel;
		grab[grabsel].ispbase=ta_isp_base;
		grab[grabsel].busy=0;
		grab[grabsel].valid=1;
		grab[grabsel].verts_size=0;
		grab[grabsel].strips_size=0;

		g_profiler.stop();
	}
#endif
}


READ32_MEMBER( powervr2_device::ta_yuv_tex_base_r )
{
	return ta_yuv_tex_base;
}

WRITE32_MEMBER( powervr2_device::ta_yuv_tex_base_w )
{
	logerror("%s: rec ta_yuv_tex_base_w %08x\n", tag(), data);
	COMBINE_DATA(&ta_yuv_tex_base);

	ta_yuv_index = 0;
	ta_yuv_x = 0;
	ta_yuv_y = 0;

}

READ32_MEMBER( powervr2_device::ta_yuv_tex_ctrl_r )
{
	return ta_yuv_tex_ctrl;
}

WRITE32_MEMBER( powervr2_device::ta_yuv_tex_ctrl_w )
{
	logerror("%s: rec ta_yuv_tex_ctrl_w %08x\n", tag(), data);
	COMBINE_DATA(&ta_yuv_tex_ctrl);
	ta_yuv_x_size = ((ta_yuv_tex_ctrl & 0x3f)+1)*16;
	ta_yuv_y_size = (((ta_yuv_tex_ctrl>>8) & 0x3f)+1)*16;
	if(ta_yuv_tex_ctrl & 0x01010000)
		fatalerror("YUV with setting %08x",ta_yuv_tex_ctrl);
}

#include "debugger.h"
/* TODO */
READ32_MEMBER( powervr2_device::ta_yuv_tex_cnt_r )
{
	machine().debug_break();
	return ta_yuv_tex_cnt;
}

// Dummy read required after write for internal powervr reasons says the manual
READ32_MEMBER( powervr2_device::ta_list_cont_r )
{
	return 0;
}

WRITE32_MEMBER( powervr2_device::ta_list_cont_w )
{
	logerror("%s: rec ta_list_cont_w %08x\n", tag(), data);
	if(data & 0x80000000) {
		tafifo_listtype= -1; // no list being received
		listtype_used |= (1+4);
	}
}

READ32_MEMBER( powervr2_device::ta_next_opb_init_r )
{
	return ta_next_opb_init;
}

WRITE32_MEMBER( powervr2_device::ta_next_opb_init_w )
{
	logerror("%s: rec ta_next_opb_init_w %08x\n", tag(), data);
	COMBINE_DATA(&ta_next_opb_init);
}


READ32_MEMBER( powervr2_device::fog_table_r )
{
	return fog_table[offset];
}

WRITE32_MEMBER( powervr2_device::fog_table_w )
{
	COMBINE_DATA(fog_table+offset);
}

READ32_MEMBER( powervr2_device::ta_ol_pointers_1_r )
{
	return ta_ol_pointers_1[offset];
}

READ32_MEMBER( powervr2_device::ta_ol_pointers_2_r )
{
	return ta_ol_pointers_2[offset];
}

READ32_MEMBER( powervr2_device::palette_r )
{
	return palette[offset];
}

WRITE32_MEMBER( powervr2_device::palette_w )
{
	COMBINE_DATA(palette+offset);
}

void powervr2_device::update_screen_format()
{
	/*                        00=VGA    01=NTSC   10=PAL,   11=illegal/undocumented */
	const int spg_clks[4] = { 26944080, 13458568, 13462800, 26944080 };
	int32_t spg_hsize = spg_load & 0x3ff;
	int32_t spg_vsize = (spg_load >> 16) & 0x3ff;
	int32_t spg_hbstart = spg_hblank & 0x3ff;
	int32_t spg_hbend = (spg_hblank >> 16) & 0x3ff;
	int32_t spg_vbstart = spg_vblank & 0x3ff;
	int32_t spg_vbend = (spg_vblank >> 16) & 0x3ff;
	//int32_t vo_horz_start_pos = vo_startx & 0x3ff;
	//int32_t vo_vert_start_pos_f1 = vo_starty & 0x3ff;
	int pclk = spg_clks[(spg_control >> 6) & 3] * (((spg_control & 0x10) >> 4)+1);

	attoseconds_t refresh = HZ_TO_ATTOSECONDS(pclk) * spg_hsize * spg_vsize;

	rectangle visarea = screen().visible_area();

	visarea.min_x = spg_hbend;
	visarea.max_x = spg_hbstart - 1;
	visarea.min_y = spg_vbend;
	visarea.max_y = spg_vbstart - 1;

	// Sanitize
	if(visarea.max_x >= spg_hsize)
		visarea.max_x = spg_hsize-1;
	if(visarea.max_y >= spg_vsize)
		visarea.max_y = spg_vsize-1;
	if(visarea.min_x > visarea.max_x)
		visarea.min_x = visarea.max_x;
	if(visarea.min_y > visarea.max_y)
		visarea.min_y = visarea.max_y;

	screen().configure(spg_hsize, spg_vsize, visarea, refresh );
}


READ32_MEMBER( powervr2_device::sb_pdstap_r )
{
	return sb_pdstap;
}

WRITE32_MEMBER( powervr2_device::sb_pdstap_w )
{
	COMBINE_DATA(&sb_pdstap);
	m_pvr_dma.pvr_addr = sb_pdstap;
}

READ32_MEMBER( powervr2_device::sb_pdstar_r )
{
	return sb_pdstar;
}

WRITE32_MEMBER( powervr2_device::sb_pdstar_w )
{
	COMBINE_DATA(&sb_pdstar);
	m_pvr_dma.sys_addr = sb_pdstar;
}

READ32_MEMBER( powervr2_device::sb_pdlen_r )
{
	return sb_pdlen;
}

WRITE32_MEMBER( powervr2_device::sb_pdlen_w )
{
	COMBINE_DATA(&sb_pdlen);
	m_pvr_dma.size = sb_pdlen;
}

READ32_MEMBER( powervr2_device::sb_pddir_r )
{
	return sb_pddir;
}

WRITE32_MEMBER( powervr2_device::sb_pddir_w )
{
	COMBINE_DATA(&sb_pddir);
	m_pvr_dma.dir = sb_pddir;
}

READ32_MEMBER( powervr2_device::sb_pdtsel_r )
{
	return sb_pdtsel;
}

WRITE32_MEMBER( powervr2_device::sb_pdtsel_w )
{
	COMBINE_DATA(&sb_pdtsel);
	m_pvr_dma.sel = sb_pdtsel & 1;
}

READ32_MEMBER( powervr2_device::sb_pden_r )
{
	return sb_pden;
}

WRITE32_MEMBER( powervr2_device::sb_pden_w )
{
	COMBINE_DATA(&sb_pden);
	m_pvr_dma.flag = sb_pden & 1;
}

READ32_MEMBER( powervr2_device::sb_pdst_r )
{
	return sb_pdst;
}

WRITE32_MEMBER( powervr2_device::sb_pdst_w )
{
	COMBINE_DATA(&sb_pdst);

	uint32_t old = m_pvr_dma.start & 1;
	m_pvr_dma.start = sb_pdst & 1;

	if(((old & 1) == 0) && m_pvr_dma.flag && m_pvr_dma.start && ((m_pvr_dma.sel & 1) == 0)) // 0 -> 1
		pvr_dma_execute(space);
}

READ32_MEMBER( powervr2_device::sb_pdapro_r )
{
	return sb_pdapro;
}

WRITE32_MEMBER( powervr2_device::sb_pdapro_w )
{
	COMBINE_DATA(&sb_pdapro);
}


TIMER_CALLBACK_MEMBER(powervr2_device::transfer_opaque_list_irq)
{
//  printf("OPLST %d\n",screen().vpos());

	irq_cb(EOXFER_OPLST_IRQ);
}

TIMER_CALLBACK_MEMBER(powervr2_device::transfer_opaque_modifier_volume_list_irq)
{
//  printf("OPMV %d\n",screen().vpos());

	irq_cb(EOXFER_OPMV_IRQ);
}

TIMER_CALLBACK_MEMBER(powervr2_device::transfer_translucent_list_irq)
{
//  printf("TRLST %d\n",screen().vpos());

	irq_cb(EOXFER_TRLST_IRQ);
}

TIMER_CALLBACK_MEMBER(powervr2_device::transfer_translucent_modifier_volume_list_irq)
{
//  printf("TRMV %d\n",screen().vpos());

	irq_cb(EOXFER_TRMV_IRQ);
}

TIMER_CALLBACK_MEMBER(powervr2_device::transfer_punch_through_list_irq)
{
//  printf("PTLST %d\n",screen().vpos());

	irq_cb(EOXFER_PTLST_IRQ);
}

void powervr2_device::process_ta_fifo()
{
#if 0
	/* first byte in the buffer is the Parameter Control Word

	 pppp pppp gggg gggg oooo oooo oooo oooo

	 p = para control
	 g = group control
	 o = object control

	*/

	receiveddata *rd = &grab[grabsel];

	// Para Control
	paracontrol=(ta_fifo_buf[0] >> 24) & 0xff;
	// 0 end of list
	// 1 user tile clip
	// 2 object list set
	// 3 reserved
	// 4 polygon/modifier volume
	// 5 sprite
	// 6 reserved
	// 7 vertex
	paratype=(paracontrol >> 5) & 7;
	endofstrip=(paracontrol >> 4) & 1;
	listtype=(paracontrol >> 0) & 7;
	if ((paratype >= 4) && (paratype <= 6))
	{
		global_paratype = paratype;
		// Group Control
		groupcontrol=(ta_fifo_buf[0] >> 16) & 0xff;
		groupen=(groupcontrol >> 7) & 1;
		striplen=(groupcontrol >> 2) & 3;
		userclip=(groupcontrol >> 0) & 3;
		// Obj Control
		objcontrol=(ta_fifo_buf[0] >> 0) & 0xffff;
		shadow=(objcontrol >> 7) & 1;
		volume=(objcontrol >> 6) & 1;
		coltype=(objcontrol >> 4) & 3;
		texture=(objcontrol >> 3) & 1;
		offfset=(objcontrol >> 2) & 1;
		gouraud=(objcontrol >> 1) & 1;
		uv16bit=(objcontrol >> 0) & 1;
	}

	// check if we need 8 words more
	if (tafifo_mask == 7)
	{
		parameterconfig = pvr_parameterconfig[objcontrol & 0x3d];
		// decide number of words per vertex
		if (paratype == 7)
		{
			if ((global_paratype == 5) || (tafifo_listtype == 1) || (tafifo_listtype == 3))
				tafifo_vertexwords = 16;
			if (tafifo_vertexwords == 16)
			{
				tafifo_mask = 15;
				tafifo_pos = 8;
				return;
			}
		}
		// decide number of words when not a vertex
		tafifo_vertexwords=pvr_wordsvertex[parameterconfig];
		if ((paratype == 4) && ((listtype != 1) && (listtype != 3)))
			if (pvr_wordspolygon[parameterconfig] == 16)
			{
				tafifo_mask = 15;
				tafifo_pos = 8;
				return;
			}
	}
	tafifo_mask = 7;

	// now we heve all the needed words
	// here we should generate the data for the various tiles
	// for now, just interpret their meaning
	if (paratype == 0)
	{ // end of list
		#if DEBUG_PVRDLIST
		osd_printf_verbose("Para Type 0 End of List\n");
		#endif
		/* Process transfer FIFO done irqs here */
		/* FIXME: timing of these */
		//printf("%d %d\n",tafifo_listtype,screen().vpos());
		switch (tafifo_listtype)
		{
		case 0: machine().scheduler().timer_set(attotime::from_usec(100), timer_expired_delegate(FUNC(powervr2_device::transfer_opaque_list_irq), this)); break;
		case 1: machine().scheduler().timer_set(attotime::from_usec(100), timer_expired_delegate(FUNC(powervr2_device::transfer_opaque_modifier_volume_list_irq), this)); break;
		case 2: machine().scheduler().timer_set(attotime::from_usec(100), timer_expired_delegate(FUNC(powervr2_device::transfer_translucent_list_irq), this)); break;
		case 3: machine().scheduler().timer_set(attotime::from_usec(100), timer_expired_delegate(FUNC(powervr2_device::transfer_translucent_modifier_volume_list_irq), this)); break;
		case 4: machine().scheduler().timer_set(attotime::from_usec(100), timer_expired_delegate(FUNC(powervr2_device::transfer_punch_through_list_irq), this)); break;
		}
		tafifo_listtype= -1; // no list being received
		listtype_used |= (2+8);
	}
	else if (paratype == 1)
	{ // user tile clip
		#if DEBUG_PVRDLIST
		osd_printf_verbose("Para Type 1 User Tile Clip\n");
		osd_printf_verbose(" (%d , %d)-(%d , %d)\n", ta_fifo_buf[4], ta_fifo_buf[5], ta_fifo_buf[6], ta_fifo_buf[7]);
		#endif
	}
	else if (paratype == 2)
	{ // object list set
		#if DEBUG_PVRDLIST
		osd_printf_verbose("Para Type 2 Object List Set at %08x\n", ta_fifo_buf[1]);
		osd_printf_verbose(" (%d , %d)-(%d , %d)\n", ta_fifo_buf[4], ta_fifo_buf[5], ta_fifo_buf[6], ta_fifo_buf[7]);
		#endif
	}
	else if (paratype == 3)
	{
		#if DEBUG_PVRDLIST
		osd_printf_verbose("Para Type %x Unknown!\n", ta_fifo_buf[0]);
		#endif
	}
	else
	{ // global parameter or vertex parameter
		#if DEBUG_PVRDLIST
		osd_printf_verbose("Para Type %d", paratype);
		if (paratype == 7)
			osd_printf_verbose(" End of Strip %d", endofstrip);
		if (listtype_used & 3)
			osd_printf_verbose(" List Type %d", listtype);
		osd_printf_verbose("\n");
		#endif

		// set type of list currently being received
		if ((paratype == 4) || (paratype == 5) || (paratype == 6))
		{
			if (tafifo_listtype < 0)
			{
				tafifo_listtype = listtype;
			}
		}
		listtype_used = listtype_used ^ (listtype_used & 3);

		if ((paratype == 4) || (paratype == 5))
		{ // quad or polygon
			depthcomparemode=(ta_fifo_buf[1] >> 29) & 7;
			cullingmode=(ta_fifo_buf[1] >> 27) & 3;
			zwritedisable=(ta_fifo_buf[1] >> 26) & 1;
			cachebypass=(ta_fifo_buf[1] >> 21) & 1;
			dcalcctrl=(ta_fifo_buf[1] >> 20) & 1;
			volumeinstruction=(ta_fifo_buf[1] >> 29) & 7;

			//textureusize=1 << (3+((ta_fifo_buf[2] >> 3) & 7));
			//texturevsize=1 << (3+(ta_fifo_buf[2] & 7));
			texturesizes=ta_fifo_buf[2] & 0x3f;
			blend_mode = ta_fifo_buf[2] >> 26;
			srcselect=(ta_fifo_buf[2] >> 25) & 1;
			dstselect=(ta_fifo_buf[2] >> 24) & 1;
			fogcontrol=(ta_fifo_buf[2] >> 22) & 3;
			colorclamp=(ta_fifo_buf[2] >> 21) & 1;
			use_alpha = (ta_fifo_buf[2] >> 20) & 1;
			ignoretexalpha=(ta_fifo_buf[2] >> 19) & 1;
			flipuv=(ta_fifo_buf[2] >> 17) & 3;
			clampuv=(ta_fifo_buf[2] >> 15) & 3;
			filtermode=(ta_fifo_buf[2] >> 13) & 3;
			sstexture=(ta_fifo_buf[2] >> 12) & 1;
			mmdadjust=(ta_fifo_buf[2] >> 8) & 1;
			tsinstruction=(ta_fifo_buf[2] >> 6) & 3;
			if (texture == 1)
			{
				textureaddress=(ta_fifo_buf[3] & 0x1FFFFF) << 3;
				scanorder=(ta_fifo_buf[3] >> 26) & 1;
				pixelformat=(ta_fifo_buf[3] >> 27) & 7;
				mipmapped=(ta_fifo_buf[3] >> 31) & 1;
				vqcompressed=(ta_fifo_buf[3] >> 30) & 1;
				strideselect=(ta_fifo_buf[3] >> 25) & 1;
				paletteselector=(ta_fifo_buf[3] >> 21) & 0x3F;
				#if DEBUG_PVRDLIST
				osd_printf_verbose(" Texture at %08x format %d\n", (ta_fifo_buf[3] & 0x1FFFFF) << 3, pixelformat);
				#endif
			}
			if (paratype == 4)
			{ // polygon or mv
				if ((tafifo_listtype == 1) || (tafifo_listtype == 3))
				{
				#if DEBUG_PVRDLIST
					osd_printf_verbose(" Modifier Volume\n");
				#endif
				}
				else
				{
				#if DEBUG_PVRDLIST
					osd_printf_verbose(" Polygon\n");
				#endif
				}
			}
			if (paratype == 5)
			{ // quad
				#if DEBUG_PVRDLIST
				osd_printf_verbose(" Sprite\n");
				#endif
			}
		}

		if (paratype == 7)
		{ // vertex
			if ((tafifo_listtype == 1) || (tafifo_listtype == 3))
			{
				#if DEBUG_PVRDLIST
				osd_printf_verbose(" Vertex modifier volume");
				osd_printf_verbose(" A(%f,%f,%f) B(%f,%f,%f) C(%f,%f,%f)", u2f(ta_fifo_buf[1]), u2f(ta_fifo_buf[2]),
					u2f(ta_fifo_buf[3]), u2f(ta_fifo_buf[4]), u2f(ta_fifo_buf[5]), u2f(ta_fifo_buf[6]), u2f(ta_fifo_buf[7]),
					u2f(ta_fifo_buf[8]), u2f(ta_fifo_buf[9]));
				osd_printf_verbose("\n");
				#endif
			}
			else if (global_paratype == 5)
			{
				#if DEBUG_PVRDLIST
				osd_printf_verbose(" Vertex sprite");
				osd_printf_verbose(" A(%f,%f,%f) B(%f,%f,%f) C(%f,%f,%f) D(%f,%f,)", u2f(ta_fifo_buf[1]), u2f(ta_fifo_buf[2]),
					u2f(ta_fifo_buf[3]), u2f(ta_fifo_buf[4]), u2f(ta_fifo_buf[5]), u2f(ta_fifo_buf[6]), u2f(ta_fifo_buf[7]),
					u2f(ta_fifo_buf[8]), u2f(ta_fifo_buf[9]), u2f(ta_fifo_buf[10]), u2f(ta_fifo_buf[11]));
				osd_printf_verbose("\n");
				#endif
				if (texture == 1)
				{
					if (rd->verts_size <= 65530)
					{
						strip *ts;
						vert *tv = &rd->verts[rd->verts_size];
						tv[0].x = u2f(ta_fifo_buf[0x1]);
						tv[0].y = u2f(ta_fifo_buf[0x2]);
						tv[0].w = u2f(ta_fifo_buf[0x3]);
						tv[1].x = u2f(ta_fifo_buf[0x4]);
						tv[1].y = u2f(ta_fifo_buf[0x5]);
						tv[1].w = u2f(ta_fifo_buf[0x6]);
						tv[3].x = u2f(ta_fifo_buf[0x7]);
						tv[3].y = u2f(ta_fifo_buf[0x8]);
						tv[3].w = u2f(ta_fifo_buf[0x9]);
						tv[2].x = u2f(ta_fifo_buf[0xa]);
						tv[2].y = u2f(ta_fifo_buf[0xb]);
						tv[2].w = tv[0].w+tv[3].w-tv[1].w;
						tv[0].u = u2f(ta_fifo_buf[0xd] & 0xffff0000);
						tv[0].v = u2f(ta_fifo_buf[0xd] << 16);
						tv[1].u = u2f(ta_fifo_buf[0xe] & 0xffff0000);
						tv[1].v = u2f(ta_fifo_buf[0xe] << 16);
						tv[3].u = u2f(ta_fifo_buf[0xf] & 0xffff0000);
						tv[3].v = u2f(ta_fifo_buf[0xf] << 16);
						tv[2].u = tv[0].u+tv[3].u-tv[1].u;
						tv[2].v = tv[0].v+tv[3].v-tv[1].v;

						ts = &rd->strips[rd->strips_size++];
						tex_get_info(&ts->ti);
						ts->svert = rd->verts_size;
						ts->evert = rd->verts_size + 3;

						rd->verts_size += 4;
					}
				}
			}
			else if (global_paratype == 4)
			{
				#if DEBUG_PVRDLIST
				osd_printf_verbose(" Vertex polygon");
				osd_printf_verbose(" V(%f,%f,%f) T(%f,%f)", u2f(ta_fifo_buf[1]), u2f(ta_fifo_buf[2]), u2f(ta_fifo_buf[3]), u2f(ta_fifo_buf[4]), u2f(ta_fifo_buf[5]));
				osd_printf_verbose("\n");
				#endif
				if (rd->verts_size <= 65530)
				{
					/* add a vertex to our list */
					/* this is used for 3d stuff, ie most of the graphics (see guilty gear, confidential mission, maze of the kings etc.) */
					/* -- this is also wildly inaccurate! */
					vert *tv = &rd->verts[rd->verts_size];

					tv->x=u2f(ta_fifo_buf[1]);
					tv->y=u2f(ta_fifo_buf[2]);
					tv->w=u2f(ta_fifo_buf[3]);
					tv->u=u2f(ta_fifo_buf[4]);
					tv->v=u2f(ta_fifo_buf[5]);
					if (texture == 0)
					{
						if(coltype == 0)
							nontextured_pal_int=ta_fifo_buf[6];
						else if(coltype == 1)
						{
							nontextured_fpal_a=u2f(ta_fifo_buf[4]);
							nontextured_fpal_r=u2f(ta_fifo_buf[5]);
							nontextured_fpal_g=u2f(ta_fifo_buf[6]);
							nontextured_fpal_b=u2f(ta_fifo_buf[7]);
						}
					}

					if((!rd->strips_size) ||
						rd->strips[rd->strips_size-1].evert != -1)
					{
						strip *ts = &rd->strips[rd->strips_size++];
						tex_get_info(&ts->ti);
						ts->svert = rd->verts_size;
						ts->evert = -1;
					}
					if(endofstrip)
						rd->strips[rd->strips_size-1].evert = rd->verts_size;
					rd->verts_size++;
				}
			}
		}
	}
#endif
}

const int powervr2_device::ta_packet_len_table[VAR_COUNT*2] = {
	// Command packets
	8, 8, 16, 8, 8, 8,  8,  8,  8, 8, 8,  8,  8, 16, 16, 8, 8,  8,  8, 8, 8,  8,  8,  8,  8,  8, 8, 8, 8, 8,
	// Vertex packets
	8, 8,  8, 8, 8, 8, 16, 16, 16, 8, 8, 16, 16,  8,  8, 8, 8, 16, 16, 8, 8, 16, 16, 16, 16, 16, 8, 8, 8, 8
};

int powervr2_device::ta_get_packet_type()
{
	uint32_t h = ta_fifo_buf[0];
	switch(h >> 29) {
	case 0: return CMD_END_OF_LIST;
	case 1: return CMD_USER_TILE_CLIP;
	case 2: return CMD_OBJECT_LIST_SET;
	case 4: case 5: {
		if(ta_list_type == -1) {
			ta_list_type = (h >> 24) & 7;
			if(ta_list_type >= 5) {
				ta_list_type = -1;
				return VAR_UNKNOWN;
			}
		}
		if(ta_list_type == L_OPAQUE_SHADOW || ta_list_type == L_TRANS_SHADOW)
			return SHADOW;

		bool vol = h & H_DUAL;
		int  col = (h & H_COLMODE) >> 4;
		bool tex = h & H_TEX;
		bool off = h & H_OFF;
		bool cuv = h & H_COMPACT_UV;

		if((h >> 29) == 5)
			return tex ? SPRITE : LINE_SPRITE;

		if(vol)
			if(tex)
				if(cuv)
					switch(col) {
					case 0: return TRI_T_U8_CUV_DUAL;
					case 2: return TRI_T_I_GLB_CUV_DUAL;
					case 3: return TRI_T_I_PREV_CUV_DUAL;
					}
				else
					switch(col) {
					case 0: return TRI_T_U8_DUAL;
					case 2: return TRI_T_I_GLB_DUAL;
					case 3: return TRI_T_I_PREV_DUAL;
					}
			else
				switch(col) {
				case 0: return TRI_G_U8_DUAL;
				case 2: return TRI_G_I_GLB_DUAL;
				case 3: return TRI_G_I_PREV_DUAL;
				}
		else
			if(tex)
				if(cuv)
					switch(col) {
					case 0: return TRI_T_U8_CUV;
					case 1: return TRI_T_F32_CUV;
					case 2: return off ? TRI_T_I_GLB_OFF_CUV : TRI_T_I_GLB_CUV;
					case 3: return TRI_T_I_PREV_CUV;
					}
				else
					switch(col) {
					case 0: return TRI_T_U8;
					case 1: return TRI_T_F32;
					case 2: return off ? TRI_T_I_GLB_OFF : TRI_T_I_GLB;
					case 3: return TRI_T_I_PREV;
					}
			else
				switch(col) {
				case 0: return TRI_G_U8;
				case 1: return TRI_G_F32;
				case 2: return TRI_G_I_GLB;
				case 3: return TRI_G_I_PREV;
				}
		return VAR_UNKNOWN;
	}
	case 7: return ta_cmd_type + VAR_COUNT;
	}
	return VAR_UNKNOWN;
}

const char *powervr2_device::ta_packet_type_name[VAR_COUNT] = {
	"tri_g_f32",
	"tri_g_i_glb",
	"tri_g_i_glb_dual",
	"tri_g_i_prev",
	"tri_g_i_prev_dual",
	"tri_g_u8",
	"tri_g_u8_dual",
	"tri_t_f32",
	"tri_t_f32_cuv",
	"tri_t_i_glb",
	"tri_t_i_glb_cuv",
	"tri_t_i_glb_cuv_dual",
	"tri_t_i_glb_dual",
	"tri_t_i_glb_off",
	"tri_t_i_glb_off_cuv",
	"tri_t_i_prev",
	"tri_t_i_prev_cuv",
	"tri_t_i_prev_cuv_dual",
	"tri_t_i_prev_dual",
	"tri_t_u8",
	"tri_t_u8_cuv",
	"tri_t_u8_cuv_dual",
	"tri_t_u8_dual",

	"shadow",
	"sprite",
	"line_sprite",

	"cmd_end_of_list",
	"cmd_user_tile_clip",
	"cmd_object_list_set",

	"var_unknown"
};

void powervr2_device::mem_write32(uint32_t adr, uint32_t val)
{
	sdram[adr >> 2] = val;
	logerror("%s: write32 %08x, %08x (%g)\n", tag(), adr/4, val, u2f(val));
}

void powervr2_device::ta_object_list_extend()
{
	uint32_t ptr = ta_ol_pointers_1[ta_olist_tile];
	switch(ta_olist_block_size) {
	case 32:  if((ptr & 0x1c) != 0x1c) return; break;
	case 64:  if((ptr & 0x3c) != 0x3c) return; break;
	case 128: if((ptr & 0x7c) != 0x7c) return; break;
	}

	logerror("%s: list extend\n", tag());
	exit(0);
}

void powervr2_device::ta_add_object(uint32_t adr, bool tail)
{
	uint32_t ptr = ta_ol_pointers_1[ta_olist_tile];
	if(ptr & 0x80000000) {
		if(!tail) {
			ta_object_list_extend();
			ptr = ta_ol_pointers_1[ta_olist_tile];
		}
		mem_write32(ptr & 0x7fffff, adr);
		ta_ol_pointers_1[ta_olist_tile] = (ptr & 0xff800000) | ((ptr + 4) & 0x7fffff);

		if(tail)
			ta_object_list_extend();

	} else {
		mem_write32(ta_olist_pos, adr);
		ta_ol_pointers_1[ta_olist_tile] = ((ta_olist_pos + 4) & 0x7fffff) | (ptr & 0x7f800000) | 0x80000000;
	}
	//	logerror("%s: ta_ol_pointers_1[%d] = %08x\n", tag(), ta_olist_tile, ta_ol_pointers_1[ta_olist_tile]);
}

void powervr2_device::ta_handle_command_draw()
{
	logerror("%s: command %s %08x %08x\n",
			 tag(),
			 ta_packet_type_name[ta_packet_type],
			 ta_fifo_buf[0],
			 ta_fifo_buf[1]
			 );

	ta_cmd_header = ta_fifo_buf[0];
	ta_cmd_instr  = ta_fifo_buf[1];

	if(ta_packet_type == SPRITE) {
		ta_cmd_header = (ta_cmd_header & ~(H_DUAL|H_COLMODE|H_GOURAUD)) | (H_TEX|H_COMPACT_UV);
		ta_cmd_instr  = (ta_cmd_instr  & ~HI_GOURAUD) | (HI_TEX|HI_COMPACT_UV);

	} else if(ta_packet_type == LINE_SPRITE) {
		ta_cmd_header = ta_cmd_header & ~(H_DUAL|H_COLMODE|H_GOURAUD|H_TEX|H_OFF|H_COMPACT_UV);
		ta_cmd_instr  = ta_cmd_instr  & ~(HI_GOURAUD|HI_TEX|HI_COMPACT_UV);

	} else if(ta_packet_type == SHADOW)
		ta_cmd_header = ta_cmd_header & ~(H_DUAL|H_TEX|H_OFF|H_COMPACT_UV);

	if(!(ta_cmd_header & H_TEX)) {
		ta_cmd_header &= ~H_OFF;
		ta_cmd_instr  &= ~HI_OFF;
	}

	if(ta_cmd_header & H_GROUP) {
		switch((ta_cmd_header & H_GROUP) >> 18) {
		case 0: ta_cmd_strip_length = 1; break;
		case 1: ta_cmd_strip_length = 2; break;
		case 2: ta_cmd_strip_length = 4; break;
		case 3: ta_cmd_strip_length = 6; break;
		}
	}

	ta_cmd_user_clip_mode = (ta_cmd_header & H_UCLIP) >> 16;

	if(ta_packet_type == SHADOW) {
		ta_cmd_tsp[0] = 0;
		ta_cmd_tex[0] = 0;
	} else {
		ta_cmd_tsp[0] = ta_fifo_buf[2];
		ta_cmd_tex[0] = ta_fifo_buf[3];
	}

	if(ta_packet_type == TRI_G_I_GLB_DUAL ||
	   ta_packet_type == TRI_G_I_PREV_DUAL ||
	   ta_packet_type == TRI_G_U8_DUAL ||
	   ta_packet_type == TRI_T_I_GLB_CUV_DUAL ||
	   ta_packet_type == TRI_T_I_GLB_DUAL ||
	   ta_packet_type == TRI_T_I_PREV_CUV_DUAL ||
	   ta_packet_type == TRI_T_I_PREV_DUAL ||
	   ta_packet_type == TRI_T_U8_CUV_DUAL ||
	   ta_packet_type == TRI_T_U8_DUAL) {

		ta_cmd_tsp[1] = ta_fifo_buf[4];
		ta_cmd_tex[1] = ta_fifo_buf[5];

	} else {
		ta_cmd_tsp[1] = 0;
		ta_cmd_tex[1] = 0;
	}

	if(!(ta_cmd_header & H_TEX) && (ta_packet_type != SPRITE) && (ta_packet_type != LINE_SPRITE)) {
		ta_cmd_tex[0] = 0;
		ta_cmd_tex[1] = 0;
	}

	switch(ta_packet_type) {
	case TRI_G_I_GLB:
	case TRI_T_I_GLB:
	case TRI_T_I_GLB_CUV:
		ta_load_color_f32(ta_cmd_color_base[0], ta_fifo_buf+4);
		break;
	case TRI_T_I_GLB_OFF:
	case TRI_T_I_GLB_OFF_CUV:
		ta_load_color_f32(ta_cmd_color_base[0], ta_fifo_buf+8);
		ta_load_color_f32(ta_cmd_color_offset[0], ta_fifo_buf+12);
		break;
	case TRI_G_I_GLB_DUAL:
	case TRI_T_I_GLB_DUAL:
	case TRI_T_I_GLB_CUV_DUAL:
		ta_load_color_f32(ta_cmd_color_base[0], ta_fifo_buf+8);
		ta_load_color_f32(ta_cmd_color_base[1], ta_fifo_buf+12);
		break;
	case SPRITE:
	case LINE_SPRITE:
		ta_cmd_color_sprite_base   = ta_fifo_buf[4];
		ta_cmd_color_sprite_offset = ta_fifo_buf[5];
		break;
	}
}

void powervr2_device::ta_next_list()
{
	for(ta_list_idx++; ta_list_idx < 5; ta_list_idx++)
		if((ta_alloc_ctrl >> (4*ta_list_idx)) & 3) {
			ta_olist_block_size = 16 << ((ta_alloc_ctrl >> (4*ta_list_idx)) & 3);
			ta_olist_line_size = ta_olist_block_size*((ta_glob_tile_clip & 0x3f)+1);
			return;
		}
	ta_olist_block_size = 0;
	ta_vertex_count = 0;
	ta_vertex_odd_tri = false;
	ta_list_type = -1;
	ta_vertex_shadow_first = true;
}

void powervr2_device::ta_bbox_merge(ta_bbox &bb, const ta_bbox &src) const
{
	if(bb.min_x > src.min_x)
		bb.min_x = src.min_x;
	if(bb.min_y > src.min_y)
		bb.min_y = src.min_y;
	if(bb.max_x < src.max_x)
		bb.max_x = src.max_x;
	if(bb.max_y < src.max_y)
		bb.max_y = src.max_y;
}

bool powervr2_device::ta_bbox_vertex(ta_bbox &bb, const ta_vertex *vtx, int count) const
{
	bb.min_x = bb.max_x = vtx->tx();
	bb.min_y = bb.max_y = vtx->ty();
	for(int i=1; i<count; i++) {
		int xx = vtx[i].tx();
		int yy = vtx[i].ty();
		if(bb.min_x > xx)
			bb.min_x = xx;
		if(bb.min_y > yy)
			bb.min_y = yy;
		if(bb.max_x < xx)
			bb.max_x = xx;
		if(bb.max_y < yy)
			bb.max_y = yy;
	}

	int sx = ta_glob_tile_clip & 0x3f;
	int sy = (ta_glob_tile_clip >> 16) & 0xf;

	bool clipped = false;
	if(bb.min_x <= sx && bb.max_x >= 0) {
		if(bb.min_x <= 0)
			bb.min_x = 0;
		if(bb.max_x > sx)
			bb.max_x = sx;
	} else {
		clipped = true;
		if(bb.max_x < 0)
			bb.min_x = bb.max_x = 0;
		else
			bb.min_x = bb.max_x = sx;
	}
	if(bb.min_y <= sy && bb.max_y >= 0) {
		if(bb.min_y <= 0)
			bb.min_y = 0;
		if(bb.max_y > sy)
			bb.max_y = sy;
	} else {
		clipped = true;
		if(bb.max_y < 0)
			bb.min_y = bb.max_y = 0;
		else
			bb.min_y = bb.max_y = sy;
	}

	return clipped;
}

uint32_t powervr2_device::ta_strip_write(bool single_tile, uint32_t &psize)
{
	bool shadow = ta_list_type == L_OPAQUE_SHADOW || ta_list_type == L_TRANS_SHADOW;

	uint32_t base_adr = ta_itp_current;
	uint32_t adr = base_adr;

	psize = 0;
	if(ta_cmd_header & H_TEX)
		psize += ta_cmd_header & H_COMPACT_UV ? 4 : 8;
	if(!shadow)
		psize += 4;
	if(ta_cmd_header & H_DUAL)
		psize *= 2;
	psize = (psize + 12)*ta_vertex_count + 12;
	if(ta_cmd_header & H_DUAL)
		psize += 8;
	if(adr+psize > ta_isp_limit)
		return 0xffffffff;

	mem_write32(adr, ta_cmd_instr | (single_tile ? 0x00200000 : 0x00000000)); adr += 4;
	mem_write32(adr, ta_cmd_tsp[0]); adr += 4;
	mem_write32(adr, ta_cmd_tex[0]); adr += 4;

	if(ta_cmd_header & H_DUAL) {
		mem_write32(adr, ta_cmd_tsp[1]); adr += 4;
		mem_write32(adr, ta_cmd_tex[1]); adr += 4;
	}

	for(int i=0; i<ta_vertex_count; i++) {
		mem_write32(adr, ta_vertex_strip[i].coords[0]); adr += 4;
		mem_write32(adr, ta_vertex_strip[i].coords[1]); adr += 4;
		mem_write32(adr, ta_vertex_strip[i].coords[2]); adr += 4;		
		if(ta_cmd_header & H_TEX) {
			if(ta_cmd_header & H_COMPACT_UV) {
				mem_write32(adr, ta_vertex_strip[i].uvc[0]); adr += 4;
			} else {
				mem_write32(adr, f2u(ta_vertex_strip[i].uv[0][0])); adr += 4;
				mem_write32(adr, f2u(ta_vertex_strip[i].uv[0][1])); adr += 4;
			}
		}
		if(!shadow) {
			mem_write32(adr, ta_vertex_strip[i].color_base[0]); adr += 4;
		}
		if(ta_cmd_header & H_OFF) {
			mem_write32(adr, ta_vertex_strip[i].color_offset[0]); adr += 4;
		}
		if(ta_cmd_header & H_DUAL) {
			if(ta_cmd_header & H_TEX) {
				if(ta_cmd_header & H_COMPACT_UV) {
					mem_write32(adr, ta_vertex_strip[i].uvc[1]); adr += 4;
				} else {
					mem_write32(adr, f2u(ta_vertex_strip[i].uv[1][0])); adr += 4;
					mem_write32(adr, f2u(ta_vertex_strip[i].uv[1][1])); adr += 4;
				}
			}
			if(!shadow) {
				mem_write32(adr, ta_vertex_strip[i].color_base[1]); adr += 4;
			}
			if(ta_cmd_header & H_OFF) {
				mem_write32(adr, ta_vertex_strip[i].color_offset[1]); adr += 4;
			}
		}
	}
	ta_itp_current = adr;
	return base_adr - (ta_isp_base & 0xfff00000);
}


void powervr2_device::ta_add_strip(uint32_t adr, uint32_t psize)
{
	logerror("%s: add strip %08x\n", tag(), adr);
	if(adr & 0x80000000) { // not strip, may concatenate into an array
		logerror("%s: tri or quad\n", tag());
		if(ta_ol_pointers_1[ta_olist_tile] & 0x40000000) {
			uint32_t cache = ta_ol_pointers_2[ta_olist_tile];
			bool conc = (adr & 0xc0000000) == 0x80000000; // tri or quad
			logerror("%s: a %d %08x\n", tag(), conc, adr);
			conc = conc && ((adr & 0xe1e00000) == (cache & 0xe1e00000)); // skip, shadow and tri/quad match
			logerror("%s: b %d %08x %08x\n", tag(), conc, adr, cache);
			conc = conc && ((cache & 0x1e000000) != 0x1e000000); // Max size not reached
			logerror("%s: c %d %08x\n", tag(), conc, cache);
			conc = conc && ((adr & 0x1fffff) == (cache & 0x1fffff) + ((((cache >> 25) & 0xf)+1)*psize >> 2)); // Data actually consecutive
			logerror("%s: d %d %08x %08x\n", tag(), conc, (adr - cache) & 0x1fffff, (((cache >> 25) & 0xf)+1)*psize >> 2);
			if(conc)
				ta_ol_pointers_2[ta_olist_tile] += 0x2000000; // Increment the count
			else {
				ta_add_object(ta_ol_pointers_2[ta_olist_tile], true);
				ta_ol_pointers_2[ta_olist_tile] = adr;
			}
			
		} else {
			ta_ol_pointers_2[ta_olist_tile] = adr;
			ta_ol_pointers_1[ta_olist_tile] |= 0x40000000;
			ta_object_list_extend();
		}
	} else { // strip, no concatenation
		if(ta_ol_pointers_1[ta_olist_tile] & 0x40000000) {
			ta_add_object(ta_ol_pointers_2[ta_olist_tile], true);
			ta_ol_pointers_1[ta_olist_tile] &= ~0x40000000;
		}
		ta_add_object(adr, false);
	}
}

void powervr2_device::ta_vertex_write()
{
	ta_bbox prim_bbox[6], full_bbox;
	bool prim_clipped[6], full_clipped;
	int prims;
	uint32_t op_tag;
	bool use_mask = false;
	switch(ta_cmd_type) {
	case SPRITE:
	case LINE_SPRITE: {
		prims = 1;
		full_clipped = prim_clipped[0] = ta_bbox_vertex(prim_bbox[0], ta_vertex_strip, 4);
		full_bbox = prim_bbox[0];
		uint32_t skip =
			ta_cmd_header & H_TEX ?
			ta_cmd_header & H_OFF ? 27 : 23
			: 19;
		op_tag = 0xa0000000 | (skip << 21);
		break;
	}

	case SHADOW: {
		prims = 1;
		full_clipped = prim_clipped[0] = ta_bbox_vertex(prim_bbox[0], ta_vertex_strip, 3);

		if(!full_clipped && ta_vertex_shadow_first) {
			ta_vertex_shadow_bbox = prim_bbox[0];
			ta_vertex_shadow_first = false;
		} else
			ta_bbox_merge(ta_vertex_shadow_bbox, prim_bbox[0]);

		if(!ta_vertex_shadow_first && (ta_cmd_instr & 0xe0000000)) {
			prim_bbox[0] = ta_vertex_shadow_bbox;
			full_clipped = false;
			ta_vertex_shadow_first = true;
		}
		full_bbox = prim_bbox[0];
		op_tag = 0x80000000;
		break;
	}

	default: {
		if(ta_vertex_count < 3)
			return;
		prims = ta_vertex_count - 2;
		full_clipped = true;
		for(int i=0; i<prims; i++) {
			prim_clipped[i] = ta_bbox_vertex(prim_bbox[i], ta_vertex_strip+i, 3);
			if(!prim_clipped[i])
				full_clipped = false;
			if(i)
				ta_bbox_merge(full_bbox, prim_bbox[i]);
			else
				full_bbox = prim_bbox[i];
		}
		uint32_t skip =
			ta_cmd_header & H_TEX ?
			ta_cmd_header & H_OFF ?
			ta_cmd_header & H_COMPACT_UV ? 3 : 4
			: ta_cmd_header & H_COMPACT_UV ? 2 : 3
			: 1;
		op_tag = skip << 21;
		if(prims == 1)
			op_tag |= 0x80000000;
		else
			use_mask = true;
		break;
	}
	}

	if(full_clipped)
		return;

	switch(ta_cmd_user_clip_mode) {
	case 2: // inside
		if(full_bbox.min_x < ta_clip_min_x ||
		   full_bbox.max_x > ta_clip_max_x ||
		   full_bbox.min_y < ta_clip_min_y ||
		   full_bbox.max_y > ta_clip_max_y)
			return;
		break;

	case 3: // outside
		if(full_bbox.min_x >= ta_clip_min_x &&
		   full_bbox.max_x <= ta_clip_max_x &&
		   full_bbox.min_y >= ta_clip_min_y &&
		   full_bbox.max_y <= ta_clip_max_y)
			return;
		break;
	}

	bool single_tile = full_bbox.min_x == full_bbox.max_x && full_bbox.min_y == full_bbox.max_y;
	int sx = (ta_glob_tile_clip & 0x3f) + 1;
	uint32_t params_address = 0xffffffff;
	uint32_t lbase =  ta_olist_pos_base + full_bbox.min_x*ta_olist_block_size + full_bbox.min_y*ta_olist_line_size;
	uint32_t tbase = full_bbox.min_x + full_bbox.min_y*sx;
	uint32_t psize = 0;

	logerror("%s: full_bbox (%d, %d) - (%d, %d)\n", tag(),
			 full_bbox.min_x, full_bbox.min_y,
			 full_bbox.max_x, full_bbox.max_y);
	for(int y = full_bbox.min_y; y <= full_bbox.max_y; y++) {
		bool clip = false;
		switch(ta_cmd_user_clip_mode) {
		case 2: // inside
			clip = y < ta_clip_min_y || y > ta_clip_max_y;
			break;

		case 3: // outside
			clip = y >= ta_clip_min_y && y <= ta_clip_max_y;
			break;
		}

		if(!clip) {
			ta_olist_pos = lbase;
			ta_olist_tile = tbase;
			for(int x = full_bbox.min_x; x <= full_bbox.max_x; x++) {
				clip = ta_olist_tile >= OL_POINTERS_COUNT;
				switch(ta_cmd_user_clip_mode) {
				case 2: // inside
					clip = x < ta_clip_min_x || x > ta_clip_max_x;
					break;
					
				case 3: // outside
					clip = x >= ta_clip_min_x && x <= ta_clip_max_x;
					break;
				}

				if(!clip) {
					uint32_t mask = 0;
					if(use_mask)
						for(int i=0; i<prims; i++)
							if(!prim_clipped[i] &&
							   x >= prim_bbox[i].min_x &&
							   x <= prim_bbox[i].max_x &&
							   y >= prim_bbox[i].min_y &&
							   y <= prim_bbox[i].max_y)
								mask |= 0x40000000 >> i;
					if(!use_mask || mask) {
						if(params_address == 0xffffffff) {
							params_address = ta_strip_write(single_tile, psize);
							if(params_address == 0xffffffff)
								return;
							logerror("%s: params_address = %08x\n", tag(), params_address);
						}
						uint32_t op = ((params_address >> 2) & 0x1fffff) | op_tag | mask;
						ta_add_strip(op, psize);
					}
				}
				ta_olist_pos += ta_olist_block_size;
				ta_olist_tile++;
			}
		}
		tbase += sx;
		lbase += ta_olist_line_size;
	}
}
	
void powervr2_device::ta_vertex_push()
{
	int strip_size;
	switch(ta_cmd_type) {
	case SPRITE:
	case LINE_SPRITE:
		strip_size = 4;
		break;
	case SHADOW:
		strip_size = 3;
		break;
	default:
		strip_size = ta_cmd_strip_length + 2;
		break;
	}

	ta_vertex_strip[ta_vertex_count++] = ta_vertex_current;

	if(ta_vertex_count == strip_size || (ta_vertex_current.header & H_EOSTRIP)) {
		ta_vertex_write();
		if(!(ta_vertex_current.header & H_EOSTRIP)) {
			if(ta_cmd_strip_length == 1) {
				if(ta_vertex_odd_tri) {
					ta_vertex_strip[0] = ta_vertex_strip[ta_vertex_count-3];
					ta_vertex_strip[1] = ta_vertex_strip[ta_vertex_count-1];
				} else {
					ta_vertex_strip[0] = ta_vertex_strip[ta_vertex_count-1];
					ta_vertex_strip[1] = ta_vertex_strip[ta_vertex_count-2];
				}
				ta_vertex_odd_tri = !ta_vertex_odd_tri;
			} else {
				ta_vertex_strip[0] = ta_vertex_strip[ta_vertex_count-2];
				ta_vertex_strip[1] = ta_vertex_strip[ta_vertex_count-1];
			}
			ta_vertex_count = 2;
		} else {
			ta_vertex_count = 0;
			ta_vertex_odd_tri = false;
		}
	}
}

void powervr2_device::ta_handle_vertex()
{
	if(ta_cmd_type != SHADOW && ta_cmd_type != SPRITE && ta_cmd_type != LINE_SPRITE) {
		ta_vertex_current.header = ta_fifo_buf[0];
		ta_vertex_current.coords[0] = ta_fifo_buf[1];
		ta_vertex_current.coords[1] = ta_fifo_buf[2];
		ta_vertex_current.coords[2] = ta_fifo_buf[3];
	}

	switch(ta_cmd_type) {
	case TRI_G_U8:
		ta_vertex_current.color_base[0] = ta_fifo_buf[6];
		ta_vertex_push();
		break;

	case TRI_G_F32:
		ta_vertex_current.color_base[0] = ta_color_f32_to_u8(ta_fifo_buf+4);
		ta_vertex_push();
		break;

	case TRI_G_I_GLB:
	case TRI_G_I_PREV:
		ta_vertex_current.color_base[0] = ta_intensity(ta_cmd_color_base[0], ta_fifo_buf[6]);
		ta_vertex_push();
		break;

	case TRI_G_U8_DUAL:
		ta_vertex_current.color_base[0] = ta_fifo_buf[4];
		ta_vertex_current.color_base[1] = ta_fifo_buf[5];
		ta_vertex_push();
		break;

	case TRI_G_I_GLB_DUAL:
	case TRI_G_I_PREV_DUAL:
		ta_vertex_current.color_base[0] = ta_intensity(ta_cmd_color_base[0], ta_fifo_buf[4]);
		ta_vertex_current.color_base[1] = ta_intensity(ta_cmd_color_base[1], ta_fifo_buf[5]);
		ta_vertex_push();
		break;

	case TRI_T_U8:
		ta_vertex_current.uv[0][0] = ta_fifo_buf[4];
		ta_vertex_current.uv[0][1] = ta_fifo_buf[5];
		ta_vertex_current.color_base[0] = ta_fifo_buf[6];
		ta_vertex_current.color_offset[0] = ta_fifo_buf[7];
		ta_vertex_push();
		break;

	case TRI_T_U8_CUV:
		ta_vertex_current.uvc[0] = ta_fifo_buf[4];
		ta_vertex_current.color_base[0] = ta_fifo_buf[6];
		ta_vertex_current.color_offset[0] = ta_fifo_buf[7];
		ta_vertex_push();
		break;

	case TRI_T_F32:
		ta_vertex_current.uv[0][0] = ta_fifo_buf[4];
		ta_vertex_current.uv[0][1] = ta_fifo_buf[5];
		ta_vertex_current.color_base[0] = ta_color_f32_to_u8(ta_fifo_buf+8);
		ta_vertex_current.color_offset[0] = ta_color_f32_to_u8(ta_fifo_buf+12);
		ta_vertex_push();
		break;

	case TRI_T_F32_CUV:
		ta_vertex_current.uvc[0] = ta_fifo_buf[4];
		ta_vertex_current.color_base[0] = ta_color_f32_to_u8(ta_fifo_buf+8);
		ta_vertex_current.color_offset[0] = ta_color_f32_to_u8(ta_fifo_buf+12);
		ta_vertex_push();
		break;

	case TRI_T_I_GLB:
	case TRI_T_I_GLB_OFF:
	case TRI_T_I_PREV:
		ta_vertex_current.uv[0][0] = ta_fifo_buf[4];
		ta_vertex_current.uv[0][1] = ta_fifo_buf[5];
		ta_vertex_current.color_base[0] = ta_intensity(ta_cmd_color_base[0], ta_fifo_buf[6]);
		ta_vertex_current.color_offset[0] = ta_intensity(ta_cmd_color_offset[0], ta_fifo_buf[7]);
		ta_vertex_push();
		break;

	case TRI_T_I_GLB_CUV:
	case TRI_T_I_GLB_OFF_CUV:
	case TRI_T_I_PREV_CUV:
		ta_vertex_current.uvc[0] = ta_fifo_buf[4];
		ta_vertex_current.color_base[0] = ta_intensity(ta_cmd_color_base[0], ta_fifo_buf[6]);
		ta_vertex_current.color_offset[0] = ta_intensity(ta_cmd_color_offset[0], ta_fifo_buf[7]);
		ta_vertex_push();
		break;

	case TRI_T_U8_DUAL:
		ta_vertex_current.uv[0][0] = ta_fifo_buf[4];
		ta_vertex_current.uv[0][1] = ta_fifo_buf[5];
		ta_vertex_current.color_base[0] = ta_fifo_buf[6];
		ta_vertex_current.color_offset[0] = ta_fifo_buf[7];
		ta_vertex_current.uv[1][0] = ta_fifo_buf[8];
		ta_vertex_current.uv[1][1] = ta_fifo_buf[9];
		ta_vertex_current.color_base[1] = ta_fifo_buf[10];
		ta_vertex_current.color_offset[1] = ta_fifo_buf[11];
		ta_vertex_push();
		break;

	case TRI_T_U8_CUV_DUAL:
		ta_vertex_current.uvc[0] = ta_fifo_buf[4];
		ta_vertex_current.color_base[0] = ta_fifo_buf[6];
		ta_vertex_current.color_offset[0] = ta_fifo_buf[7];
		ta_vertex_current.uvc[1] = ta_fifo_buf[8];
		ta_vertex_current.color_base[1] = ta_fifo_buf[10];
		ta_vertex_current.color_offset[1] = ta_fifo_buf[11];
		ta_vertex_push();
		break;

	case TRI_T_I_GLB_DUAL:
	case TRI_T_I_PREV_DUAL:
		ta_vertex_current.uv[0][0] = ta_fifo_buf[4];
		ta_vertex_current.uv[0][1] = ta_fifo_buf[5];
		ta_vertex_current.color_base[0] = ta_intensity(ta_cmd_color_base[0], ta_fifo_buf[6]);
		ta_vertex_current.color_offset[0] = ta_intensity(ta_cmd_color_offset[0], ta_fifo_buf[7]);
		ta_vertex_current.uv[1][0] = ta_fifo_buf[8];
		ta_vertex_current.uv[1][1] = ta_fifo_buf[9];
		ta_vertex_current.color_base[1] = ta_intensity(ta_cmd_color_base[1], ta_fifo_buf[10]);
		ta_vertex_current.color_offset[1] = ta_intensity(ta_cmd_color_offset[1], ta_fifo_buf[11]);
		ta_vertex_push();
		break;

	case TRI_T_I_GLB_CUV_DUAL:
	case TRI_T_I_PREV_CUV_DUAL:
		ta_vertex_current.uvc[0] = ta_fifo_buf[4];
		ta_vertex_current.color_base[0] = ta_intensity(ta_cmd_color_base[0], ta_fifo_buf[6]);
		ta_vertex_current.color_offset[0] = ta_intensity(ta_cmd_color_offset[0], ta_fifo_buf[7]);
		ta_vertex_current.uvc[1] = ta_fifo_buf[8];
		ta_vertex_current.color_base[1] = ta_intensity(ta_cmd_color_base[1], ta_fifo_buf[10]);
		ta_vertex_current.color_offset[1] = ta_intensity(ta_cmd_color_offset[1], ta_fifo_buf[11]);
		ta_vertex_push();
		break;

	case SPRITE:
		ta_vertex_current.header = 0xe0000000;
		ta_vertex_current.color_base[0] = 0;
		ta_vertex_current.color_offset[0] = 0;
		ta_vertex_current.coords[0] = ta_fifo_buf[1];
		ta_vertex_current.coords[1] = ta_fifo_buf[2];
		ta_vertex_current.coords[2] = ta_fifo_buf[3];
		ta_vertex_current.uvc[0] = ta_fifo_buf[13];
		ta_vertex_push();

		ta_vertex_current.header = 0xe0000000;
		ta_vertex_current.color_base[0] = 0;
		ta_vertex_current.color_offset[0] = 0;
		ta_vertex_current.coords[0] = ta_fifo_buf[4];
		ta_vertex_current.coords[1] = ta_fifo_buf[5];
		ta_vertex_current.coords[2] = ta_fifo_buf[6];
		ta_vertex_current.uvc[0] = ta_fifo_buf[14];
		ta_vertex_push();

		ta_vertex_current.header = 0xe0000000;
		ta_vertex_current.color_base[0] = ta_cmd_color_sprite_base;
		ta_vertex_current.color_offset[0] = ta_cmd_color_sprite_offset;
		ta_vertex_current.coords[0] = ta_fifo_buf[7];
		ta_vertex_current.coords[1] = ta_fifo_buf[8];
		ta_vertex_current.coords[2] = ta_fifo_buf[9];
		ta_vertex_current.uvc[0] = ta_fifo_buf[15];
		ta_vertex_push();

		ta_vertex_current.header = 0xe0000000 | H_EOSTRIP;
		ta_vertex_current.color_base[0] = 0;
		ta_vertex_current.color_offset[0] = 0;
		ta_vertex_current.coords[0] = ta_fifo_buf[10];
		ta_vertex_current.coords[1] = ta_fifo_buf[11];
		ta_vertex_current.coords[2] = 0;
		ta_vertex_current.uvc[0] = 0;
		ta_vertex_push();
		break;

	case LINE_SPRITE:
		ta_vertex_current.header = 0xe0000000;
		ta_vertex_current.color_base[0] = 0;
		ta_vertex_current.coords[0] = ta_fifo_buf[1];
		ta_vertex_current.coords[1] = ta_fifo_buf[2];
		ta_vertex_current.coords[2] = ta_fifo_buf[3];
		ta_vertex_push();

		ta_vertex_current.header = 0xe0000000;
		ta_vertex_current.color_base[0] = 0;
		ta_vertex_current.coords[0] = ta_fifo_buf[4];
		ta_vertex_current.coords[1] = ta_fifo_buf[5];
		ta_vertex_current.coords[2] = ta_fifo_buf[6];
		ta_vertex_push();

		ta_vertex_current.header = 0xe0000000;
		ta_vertex_current.color_base[0] = ta_cmd_color_sprite_base;
		ta_vertex_current.coords[0] = ta_fifo_buf[7];
		ta_vertex_current.coords[1] = ta_fifo_buf[8];
		ta_vertex_current.coords[2] = ta_fifo_buf[9];
		ta_vertex_push();

		ta_vertex_current.header = 0xf0000000;
		ta_vertex_current.color_base[0] = 0;
		ta_vertex_current.coords[0] = ta_fifo_buf[10];
		ta_vertex_current.coords[1] = ta_fifo_buf[11];
		ta_vertex_current.coords[2] = 0;
		ta_vertex_push();
		break;

	case SHADOW:
		ta_vertex_current.header = 0xe0000000;
		ta_vertex_current.coords[0] = ta_fifo_buf[1];
		ta_vertex_current.coords[1] = ta_fifo_buf[2];
		ta_vertex_current.coords[2] = ta_fifo_buf[3];
		ta_vertex_push();

		ta_vertex_current.header = 0xe0000000;
		ta_vertex_current.coords[0] = ta_fifo_buf[4];
		ta_vertex_current.coords[1] = ta_fifo_buf[5];
		ta_vertex_current.coords[2] = ta_fifo_buf[6];
		ta_vertex_push();

		ta_vertex_current.header = 0xf0000000;
		ta_vertex_current.coords[0] = ta_fifo_buf[7];
		ta_vertex_current.coords[1] = ta_fifo_buf[8];
		ta_vertex_current.coords[2] = ta_fifo_buf[9];
		ta_vertex_push();
		break;
	}
}

void powervr2_device::ta_handle_command_end_of_list()
{
	int sx = ta_glob_tile_clip & 0x3f;
	int sy = (ta_glob_tile_clip >> 16) & 0xf;
	ta_olist_tile = 0;
	ta_olist_pos = ta_olist_pos_base;
	for(int y=0; y<=sy; y++)
		for(int x=0; x<=sx; x++) {
			if(ta_olist_tile >= OL_POINTERS_COUNT)
				break;
			uint32_t ptr = ta_ol_pointers_1[ta_olist_tile];
			if(ptr & 0x40000000) {
				ta_add_object(ta_ol_pointers_2[ta_olist_tile], false);
				ptr = ta_ol_pointers_1[ta_olist_tile];
			}
			if(!(ptr & 0x80000000))
				ptr = ta_olist_pos;
			else
				ptr &= 0x007fffff;
			mem_write32(ptr, 0xf0000000);
			
			ta_ol_pointers_1[ta_olist_tile] = 0;

			ta_olist_tile++;
			ta_olist_pos += ta_olist_block_size;
		}
	ta_olist_pos_base = ta_olist_pos;
	irq_cb(EOXFER_OPLST_IRQ + ta_list_idx);
	ta_next_list();
}

void powervr2_device::ta_handle_command_user_tile_clip()
{
	ta_clip_min_x = ta_fifo_buf[4];
	ta_clip_min_y = ta_fifo_buf[5];
	ta_clip_max_x = ta_fifo_buf[6];
	ta_clip_max_y = ta_fifo_buf[7];
}

void powervr2_device::ta_handle_command_object_list_set()
{
	logerror("%s: object lists unhandled\n", tag());
}


WRITE64_MEMBER( powervr2_device::ta_fifo_poly_w )
{
	if (mem_mask != 0xffffffffffffffffU)    // 64 bit
		fatalerror("ta_fifo_poly_w:  Only 64 bit writes supported!\n");

	logerror("%s: rec ta_fifo_poly_w %08x\n", tag(), uint32_t(data));
	logerror("%s: rec ta_fifo_poly_w %08x\n", tag(), uint32_t(data >> 32));

	if(ta_list_idx == 5)
		return;

	ta_fifo_buf[ta_fifo_pos]   = data;
	ta_fifo_buf[ta_fifo_pos+1] = data >> 32;

	if(!ta_fifo_pos) {
		ta_packet_type = ta_get_packet_type();
		if(ta_packet_type < VAR_COUNT)
			ta_cmd_type = ta_packet_type;
		ta_packet_len = ta_packet_len_table[ta_packet_type];
	}	

	ta_fifo_pos += 2;

	if (ta_fifo_pos == ta_packet_len) {
		if(ta_packet_type == CMD_END_OF_LIST)
			ta_handle_command_end_of_list();
		else if(ta_packet_type == CMD_USER_TILE_CLIP)
			ta_handle_command_user_tile_clip();
		else if(ta_packet_type == CMD_OBJECT_LIST_SET)
			ta_handle_command_object_list_set();
		else if(ta_packet_type >= TRI_G_F32 && ta_packet_type <= LINE_SPRITE)
			ta_handle_command_draw();
		else if(ta_packet_type >= TRI_G_F32+VAR_COUNT && ta_packet_type <= LINE_SPRITE+VAR_COUNT)
			ta_handle_vertex();

		ta_fifo_pos = 0;
		ta_packet_len = 0;
	}
}

TIMER_CALLBACK_MEMBER(powervr2_device::yuv_convert_end)
{
	irq_cb(EOXFER_YUV_IRQ);
	yuv_timer_end->adjust(attotime::never);
}


WRITE8_MEMBER( powervr2_device::ta_fifo_yuv_w )
{
	dc_state *state = machine().driver_data<dc_state>();
	//printf("%08x %08x\n",ta_yuv_index++,ta_yuv_tex_ctrl);

	//popmessage("YUV fifo write %08x %08x",ta_yuv_index,ta_yuv_tex_ctrl);

	yuv_fifo[ta_yuv_index] = data;
	ta_yuv_index++;

	if(ta_yuv_index == 0x180)
	{
		ta_yuv_index = 0;
		for(int y=0;y<16;y++)
		{
			for(int x=0;x<16;x+=2)
			{
				int dst_addr;
				int u,v,y0,y1;

				dst_addr = ta_yuv_tex_base;
				dst_addr+= (ta_yuv_x+x)*2;
				dst_addr+= ((ta_yuv_y+y)*320*2);

				u = yuv_fifo[0x00+(x>>1)+((y>>1)*8)];
				v = yuv_fifo[0x40+(x>>1)+((y>>1)*8)];
				y0 = yuv_fifo[0x80+((x&8) ? 0x40 : 0x00)+((y&8) ? 0x80 : 0x00)+(x&6)+((y&7)*8)];
				y1 = yuv_fifo[0x80+((x&8) ? 0x40 : 0x00)+((y&8) ? 0x80 : 0x00)+(x&6)+((y&7)*8)+1];

				*(uint8_t *)((reinterpret_cast<uint8_t *>(dc_texture_ram)) + BYTE8_XOR_LE(dst_addr)) = u;
				*(uint8_t *)((reinterpret_cast<uint8_t *>(dc_texture_ram)) + BYTE8_XOR_LE(dst_addr+1)) = y0;
				*(uint8_t *)((reinterpret_cast<uint8_t *>(dc_texture_ram)) + BYTE8_XOR_LE(dst_addr+2)) = v;
				*(uint8_t *)((reinterpret_cast<uint8_t *>(dc_texture_ram)) + BYTE8_XOR_LE(dst_addr+3)) = y1;
			}
		}

		ta_yuv_x+=16;
		if(ta_yuv_x == ta_yuv_x_size)
		{
			ta_yuv_x = 0;
			ta_yuv_y+=16;
			if(ta_yuv_y == ta_yuv_y_size)
			{
				ta_yuv_y = 0;
				/* TODO: timing */
				yuv_timer_end->adjust(state->m_maincpu->cycles_to_attotime((ta_yuv_x_size/16)*(ta_yuv_y_size/16)*0x180));
			}
		}
	}
}


WRITE64_MEMBER(powervr2_device::texture_path0_w )
{
	texture_path_w(space, 0, offset, data, mem_mask);
}

WRITE64_MEMBER(powervr2_device::texture_path1_w )
{
	texture_path_w(space, 1, offset, data, mem_mask);
}

void powervr2_device::texture_path_w(address_space &space, int path, offs_t offset, uint64_t data, uint64_t mem_mask)
{
	if(texture_path_width[path]) {
		tex32_w(space, offset*2, data, mem_mask);
		tex32_w(space, offset*2+1, data >> 32, mem_mask >> 32);
	} else
		tex64_w(space, offset, data, mem_mask);
}

void powervr2_device::set_texture_path_mode(int channel, bool bits32)
{
	texture_path_width[channel] = bits32;
}

/* test video start */
uint32_t powervr2_device::dilate0(uint32_t value,int bits) // dilate first "bits" bits in "value"
{
	uint32_t x,m1,m2,m3;
	int a;

	x = value;
	for (a=0;a < bits;a++)
	{
		m2 = 1 << (a << 1);
		m1 = m2 - 1;
		m3 = (~m1) << 1;
		x = (x & m1) + (x & m2) + ((x & m3) << 1);
	}
	return x;
}

uint32_t powervr2_device::dilate1(uint32_t value,int bits) // dilate first "bits" bits in "value"
{
	uint32_t x,m1,m2,m3;
	int a;

	x = value;
	for (a=0;a < bits;a++)
	{
		m2 = 1 << (a << 1);
		m1 = m2 - 1;
		m3 = (~m1) << 1;
		x = (x & m1) + ((x & m2) << 1) + ((x & m3) << 1);
	}
	return x;
}

void powervr2_device::computedilated()
{
	int a,b;

	for (b=0;b <= 14;b++)
		for (a=0;a < 1024;a++) {
			dilated0[b][a]=dilate0(a,b);
			dilated1[b][a]=dilate1(a,b);
		}
	for (b=0;b <= 7;b++)
		for (a=0;a <= 7;a++)
			dilatechose[(b << 3) + a]=3+(a < b ? a : b);
}

void powervr2_device::render_hline(bitmap_rgb32 &bitmap, texinfo *ti, int y, float xl, float xr, float ul, float ur, float vl, float vr, float wl, float wr)
{
	int xxl, xxr;
	float dx, ddx, dudx, dvdx, dwdx;
	uint32_t *tdata;
	float *wbufline;

	// untextured cases aren't handled
//  if (!ti->textured) return;

	if(xr < 0 || xl >= 640)
		return;

	xxl = round(xl);
	xxr = round(xr);

	if(xxl == xxr)
		return;

	dx = xr-xl;
	dudx = (ur-ul)/dx;
	dvdx = (vr-vl)/dx;
	dwdx = (wr-wl)/dx;

	if(xxl < 0)
		xxl = 0;
	if(xxr > 640)
		xxr = 640;

	// Target the pixel center
	ddx = xxl + 0.5f - xl;
	ul += ddx*dudx;
	vl += ddx*dvdx;
	wl += ddx*dwdx;


	tdata = &bitmap.pix32(y, xxl);
	wbufline = &wbuffer[y][xxl];

	while(xxl < xxr) {
		if((wl >= *wbufline)) {
			uint32_t c;
			float u = ul/wl;
			float v = vl/wl;

			c = (this->*(ti->r))(ti, u, v);

			// debug dip to turn on/off bilinear filtering, it's slooooow
			if (debug_dip_status&0x1)
			{
				if(ti->filter_mode >= TEX_FILTER_BILINEAR)
				{
					uint32_t c1 = (this->*(ti->r))(ti, u+1.0f, v);
					uint32_t c2 = (this->*(ti->r))(ti, u+1.0f, v+1.0f);
					uint32_t c3 = (this->*(ti->r))(ti, u, v+1.0f);
					c = bilinear_filter(c, c1, c2, c3, u, v);
				}
			}

			if(c & 0xff000000) {
				*tdata = ti->blend(c, *tdata);
				*wbufline = wl;
			}
		}
		wbufline++;
		tdata++;

		ul += dudx;
		vl += dvdx;
		wl += dwdx;
		xxl ++;
	}
}

void powervr2_device::render_span(bitmap_rgb32 &bitmap, texinfo *ti,
									float y0, float y1,
									float xl, float xr,
									float ul, float ur,
									float vl, float vr,
									float wl, float wr,
									float dxldy, float dxrdy,
									float duldy, float durdy,
									float dvldy, float dvrdy,
									float dwldy, float dwrdy)
{
	float dy;
	int yy0, yy1;

	if(y1 <= 0)
		return;
	if(y1 > 480)
		y1 = 480;

	if(y0 < 0) {
		xl += -dxldy*y0;
		xr += -dxrdy*y0;
		ul += -duldy*y0;
		ur += -durdy*y0;
		vl += -dvldy*y0;
		vr += -dvrdy*y0;
		wl += -dwldy*y0;
		wr += -dwrdy*y0;
		y0 = 0;
	}

	yy0 = round(y0);
	yy1 = round(y1);

	if((yy0 < 0 && y0 > 0) || (yy1 < 0 && y1 > 0)) //temp handling of int32 overflow, needed by hotd2/totd
		return;

	dy = yy0+0.5f-y0;

	if(0)
		fprintf(stderr, "%f %f %f %f -> %f %f | %f %f -> %f %f\n",
				(double) y0,
				(double) dy, (double) dxldy, (double) dxrdy, (double) (dy*dxldy), (double) (dy*dxrdy),
				(double) xl, (double) xr, (double) (xl + dy*dxldy), (double) (xr + dy*dxrdy));
	xl += dy*dxldy;
	xr += dy*dxrdy;
	ul += dy*duldy;
	ur += dy*durdy;
	vl += dy*dvldy;
	vr += dy*dvrdy;
	wl += dy*dwldy;
	wr += dy*dwrdy;

	while(yy0 < yy1) {
		render_hline(bitmap, ti, yy0, xl, xr, ul, ur, vl, vr, wl, wr);

		xl += dxldy;
		xr += dxrdy;
		ul += duldy;
		ur += durdy;
		vl += dvldy;
		vr += dvrdy;
		wl += dwldy;
		wr += dwrdy;
		yy0 ++;
	}
}

void powervr2_device::sort_vertices(const vert *v, int *i0, int *i1, int *i2)
{
	float miny, maxy;
	int imin, imax, imid;
	miny = maxy = v[0].y;
	imin = imax = 0;

	if(miny > v[1].y) {
		miny = v[1].y;
		imin = 1;
	} else if(maxy < v[1].y) {
		maxy = v[1].y;
		imax = 1;
	}

	if(miny > v[2].y) {
		miny = v[2].y;
		imin = 2;
	} else if(maxy < v[2].y) {
		maxy = v[2].y;
		imax = 2;
	}

	imid = (imin == 0 || imax == 0) ? (imin == 1 || imax == 1) ? 2 : 1 : 0;

	*i0 = imin;
	*i1 = imid;
	*i2 = imax;
}


void powervr2_device::render_tri_sorted(bitmap_rgb32 &bitmap, texinfo *ti, const vert *v0, const vert *v1, const vert *v2)
{
	float dy01, dy02, dy12;

	float dx01dy, dx02dy, dx12dy, du01dy, du02dy, du12dy, dv01dy, dv02dy, dv12dy, dw01dy, dw02dy, dw12dy;

	if(v0->y >= 480 || v2->y < 0)
		return;

	dy01 = v1->y - v0->y;
	dy02 = v2->y - v0->y;
	dy12 = v2->y - v1->y;

	dx01dy = dy01 ? (v1->x-v0->x)/dy01 : 0;
	dx02dy = dy02 ? (v2->x-v0->x)/dy02 : 0;
	dx12dy = dy12 ? (v2->x-v1->x)/dy12 : 0;

	du01dy = dy01 ? (v1->u-v0->u)/dy01 : 0;
	du02dy = dy02 ? (v2->u-v0->u)/dy02 : 0;
	du12dy = dy12 ? (v2->u-v1->u)/dy12 : 0;

	dv01dy = dy01 ? (v1->v-v0->v)/dy01 : 0;
	dv02dy = dy02 ? (v2->v-v0->v)/dy02 : 0;
	dv12dy = dy12 ? (v2->v-v1->v)/dy12 : 0;

	dw01dy = dy01 ? (v1->w-v0->w)/dy01 : 0;
	dw02dy = dy02 ? (v2->w-v0->w)/dy02 : 0;
	dw12dy = dy12 ? (v2->w-v1->w)/dy12 : 0;

	if(!dy01) {
		if(!dy12)
			return;

		if(v1->x > v0->x)
			render_span(bitmap, ti, v1->y, v2->y, v0->x, v1->x, v0->u, v1->u, v0->v, v1->v, v0->w, v1->w, dx02dy, dx12dy, du02dy, du12dy, dv02dy, dv12dy, dw02dy, dw12dy);
		else
			render_span(bitmap, ti, v1->y, v2->y, v1->x, v0->x, v1->u, v0->u, v1->v, v0->v, v1->w, v0->w, dx12dy, dx02dy, du12dy, du02dy, dv12dy, dv02dy, dw12dy, dw02dy);

	} else if(!dy12) {
		if(v2->x > v1->x)
			render_span(bitmap, ti, v0->y, v1->y, v0->x, v0->x, v0->u, v0->u, v0->v, v0->v, v0->w, v0->w, dx01dy, dx02dy, du01dy, du02dy, dv01dy, dv02dy, dw01dy, dw02dy);
		else
			render_span(bitmap, ti, v0->y, v1->y, v0->x, v0->x, v0->u, v0->u, v0->v, v0->v, v0->w, v0->w, dx02dy, dx01dy, du02dy, du01dy, dv02dy, dv01dy, dw02dy, dw01dy);

	} else {
		if(dx01dy < dx02dy) {
			render_span(bitmap, ti, v0->y, v1->y,
						v0->x, v0->x, v0->u, v0->u, v0->v, v0->v, v0->w, v0->w,
						dx01dy, dx02dy, du01dy, du02dy, dv01dy, dv02dy, dw01dy, dw02dy);
			render_span(bitmap, ti, v1->y, v2->y,
						v1->x, v0->x + dx02dy*dy01, v1->u, v0->u + du02dy*dy01, v1->v, v0->v + dv02dy*dy01, v1->w, v0->w + dw02dy*dy01,
						dx12dy, dx02dy, du12dy, du02dy, dv12dy, dv02dy, dw12dy, dw02dy);
		} else {
			render_span(bitmap, ti, v0->y, v1->y,
						v0->x, v0->x, v0->u, v0->u, v0->v, v0->v, v0->w, v0->w,
						dx02dy, dx01dy, du02dy, du01dy, dv02dy, dv01dy, dw02dy, dw01dy);
			render_span(bitmap, ti, v1->y, v2->y,
						v0->x + dx02dy*dy01, v1->x, v0->u + du02dy*dy01, v1->u, v0->v + dv02dy*dy01, v1->v, v0->w + dw02dy*dy01, v1->w,
						dx02dy, dx12dy, du02dy, du12dy, dv02dy, dv12dy, dw02dy, dw12dy);
		}
	}
}

void powervr2_device::render_tri(bitmap_rgb32 &bitmap, texinfo *ti, const vert *v)
{
	int i0, i1, i2;

	sort_vertices(v, &i0, &i1, &i2);
	render_tri_sorted(bitmap, ti, v+i0, v+i1, v+i2);
}

void powervr2_device::render_to_accumulation_buffer(bitmap_rgb32 &bitmap,const rectangle &cliprect)
{
	dc_state *state = machine().driver_data<dc_state>();
	address_space &space = state->m_maincpu->space(AS_PROGRAM);
#if 0
	int stride;
	uint16_t *bmpaddr16;
	uint32_t k;
#endif


	if (renderselect < 0)
		return;

	//printf("drawtest!\n");

	int rs=renderselect;
	uint32_t c=space.read_dword(0x05000000+((isp_backgnd_t & 0xfffff8)>>1)+(3+3)*4);
	bitmap.fill(c, cliprect);


	int ns=grab[rs].strips_size;
	if(ns)
		memset(wbuffer, 0x00, sizeof(wbuffer));

	for (int cs=0;cs < ns;cs++)
	{
		strip *ts = &grab[rs].strips[cs];
		int sv = ts->svert;
		int ev = ts->evert;
		int i;
		if(ev == -1)
			continue;

		for(i=sv; i <= ev; i++)
		{
			vert *tv = grab[rs].verts + i;
			tv->u = tv->u * ts->ti.sizex * tv->w;
			tv->v = tv->v * ts->ti.sizey * tv->w;
		}

		for(i=sv; i <= ev-2; i++)
		{
			if (!(debug_dip_status&0x2))
				render_tri(bitmap, &ts->ti, grab[rs].verts + i);

		}
	}
	grab[rs].busy=0;
}

// copies the accumulation buffer into the framebuffer, converting to the specified format
// not accurate, ignores field stuff and just uses SOF1 for now
// also ignores scale effects (can scale accumulation buffer to half size with filtering etc.)
// also can specify dither etc.
// basically, just a crude implementation!

/* 0555KRGB = 0 */
void powervr2_device::fb_convert_0555krgb_to_555rgb(address_space &space, int x,int y)
{
	int xcnt,ycnt;
	for (ycnt=0;ycnt<32;ycnt++)
	{
		uint32_t realwriteoffs = 0x05000000 + fb_w_sof1 + (y+ycnt) * (fb_w_linestride<<3) + (x*2);
		uint32_t *src = &fake_accumulationbuffer_bitmap->pix32(y+ycnt, x);

		for (xcnt=0;xcnt<32;xcnt++)
		{
			// data starts in 8888 format, downsample it
			uint32_t data = src[xcnt];
			uint16_t newdat = ((((data & 0x000000f8) >> 3)) << 0)   |
							((((data & 0x0000f800) >> 11)) << 5)  |
							((((data & 0x00f80000) >> 19)) << 10);

			space.write_word(realwriteoffs+xcnt*2, newdat);
		}
	}
}

void powervr2_device::fb_convert_0555krgb_to_565rgb(address_space &space, int x,int y)
{
	int xcnt,ycnt;
	for (ycnt=0;ycnt<32;ycnt++)
	{
		uint32_t realwriteoffs = 0x05000000 + fb_w_sof1 + (y+ycnt) * (fb_w_linestride<<3) + (x*2);
		uint32_t *src = &fake_accumulationbuffer_bitmap->pix32(y+ycnt, x);

		for (xcnt=0;xcnt<32;xcnt++)
		{
			// data starts in 8888 format, downsample it
			uint32_t data = src[xcnt];
			uint16_t newdat = ((((data & 0x000000f8) >> 3)) << 0)   |
							((((data & 0x0000f800) >> 11)) << 5)  |
							((((data & 0x00f80000) >> 19)) << 11);

			space.write_word(realwriteoffs+xcnt*2, newdat);
		}
	}
}

void powervr2_device::fb_convert_0555krgb_to_888rgb24(address_space &space, int x,int y)
{
	int xcnt,ycnt;
	for (ycnt=0;ycnt<32;ycnt++)
	{
		uint32_t realwriteoffs = 0x05000000 + fb_w_sof1 + (y+ycnt) * (fb_w_linestride<<3) + (x*3);
		uint32_t *src = &fake_accumulationbuffer_bitmap->pix32(y+ycnt, x);

		for (xcnt=0;xcnt<32;xcnt++)
		{
			// data starts in 8888 format, downsample it
			uint32_t data = src[xcnt];
			uint32_t newdat = (data & 0xf8f8f8);

			space.write_byte(realwriteoffs+xcnt*3+0, newdat >> 0);
			space.write_byte(realwriteoffs+xcnt*3+1, newdat >> 8);
			space.write_byte(realwriteoffs+xcnt*3+2, newdat >> 16);
		}
	}
}

void powervr2_device::fb_convert_0555krgb_to_888rgb32(address_space &space, int x,int y)
{
	int xcnt,ycnt;
	for (ycnt=0;ycnt<32;ycnt++)
	{
		uint32_t realwriteoffs = 0x05000000 + fb_w_sof1 + (y+ycnt) * (fb_w_linestride<<3) + (x*4);
		uint32_t *src = &fake_accumulationbuffer_bitmap->pix32(y+ycnt, x);

		for (xcnt=0;xcnt<32;xcnt++)
		{
			// data starts in 8888 format, downsample it
			uint32_t data = src[xcnt];
			uint32_t newdat = (data & 0xf8f8f8);

			space.write_dword(realwriteoffs+xcnt*4, newdat);
		}
	}
}

/* 0565RGB = 1 */
void powervr2_device::fb_convert_0565rgb_to_555rgb(address_space &space, int x,int y)
{
	int xcnt,ycnt;
	for (ycnt=0;ycnt<32;ycnt++)
	{
		uint32_t realwriteoffs = 0x05000000 + fb_w_sof1 + (y+ycnt) * (fb_w_linestride<<3) + (x*2);
		uint32_t *src = &fake_accumulationbuffer_bitmap->pix32(y+ycnt, x);

		for (xcnt=0;xcnt<32;xcnt++)
		{
			// data starts in 8888 format, downsample it
			uint32_t data = src[xcnt];
			uint16_t newdat = ((((data & 0x000000f8) >> 3)) << 0)   |
							((((data & 0x0000fc00) >> 10)) << 5)  |
							((((data & 0x00f80000) >> 19)) << 10);

			space.write_word(realwriteoffs+xcnt*2, newdat);
		}
	}
}

void powervr2_device::fb_convert_0565rgb_to_565rgb(address_space &space, int x,int y)
{
	int xcnt,ycnt;
	for (ycnt=0;ycnt<32;ycnt++)
	{
		uint32_t realwriteoffs = 0x05000000 + fb_w_sof1 + (y+ycnt) * (fb_w_linestride<<3) + (x*2);
		uint32_t *src = &fake_accumulationbuffer_bitmap->pix32(y+ycnt, x);

		for (xcnt=0;xcnt<32;xcnt++)
		{
			// data starts in 8888 format, downsample it
			uint32_t data = src[xcnt];
			uint16_t newdat = ((((data & 0x000000f8) >> 3)) << 0)   |
							((((data & 0x0000fc00) >> 10)) << 5)  |
							((((data & 0x00f80000) >> 19)) << 11);

			space.write_word(realwriteoffs+xcnt*2, newdat);
		}
	}
}

void powervr2_device::fb_convert_0565rgb_to_888rgb24(address_space &space, int x,int y)
{
	int xcnt,ycnt;
	for (ycnt=0;ycnt<32;ycnt++)
	{
		uint32_t realwriteoffs = 0x05000000 + fb_w_sof1 + (y+ycnt) * (fb_w_linestride<<3) + (x*3);
		uint32_t *src = &fake_accumulationbuffer_bitmap->pix32(y+ycnt, x);

		for (xcnt=0;xcnt<32;xcnt++)
		{
			// data starts in 8888 format, downsample it
			uint32_t data = src[xcnt];
			uint32_t newdat = (data & 0xf8fcf8);

			space.write_byte(realwriteoffs+xcnt*3+0, newdat >> 0);
			space.write_byte(realwriteoffs+xcnt*3+1, newdat >> 8);
			space.write_byte(realwriteoffs+xcnt*3+2, newdat >> 16);
		}
	}
}

void powervr2_device::fb_convert_0565rgb_to_888rgb32(address_space &space, int x,int y)
{
	int xcnt,ycnt;
	for (ycnt=0;ycnt<32;ycnt++)
	{
		uint32_t realwriteoffs = 0x05000000 + fb_w_sof1 + (y+ycnt) * (fb_w_linestride<<3) + (x*4);
		uint32_t *src = &fake_accumulationbuffer_bitmap->pix32(y+ycnt, x);

		for (xcnt=0;xcnt<32;xcnt++)
		{
			// data starts in 8888 format, downsample it
			uint32_t data = src[xcnt];
			uint32_t newdat = (data & 0xf8fcf8);

			space.write_dword(realwriteoffs+xcnt*4, newdat);
		}
	}
}

/* 1555ARGB = 3 */
void powervr2_device::fb_convert_1555argb_to_555rgb(address_space &space, int x,int y)
{
	int xcnt,ycnt;
	for (ycnt=0;ycnt<32;ycnt++)
	{
		uint32_t realwriteoffs = 0x05000000 + fb_w_sof1 + (y+ycnt) * (fb_w_linestride<<3) + (x*2);
		uint32_t *src = &fake_accumulationbuffer_bitmap->pix32(y+ycnt, x);

		for (xcnt=0;xcnt<32;xcnt++)
		{
			// data starts in 8888 format, downsample it
			uint32_t data = src[xcnt];
			uint16_t newdat = ((((data & 0x000000f8) >> 3)) << 0)   |
							((((data & 0x0000f800) >> 11)) << 5)  |
							((((data & 0x00f80000) >> 19)) << 10);

			space.write_word(realwriteoffs+xcnt*2, newdat);
		}
	}
}

void powervr2_device::fb_convert_1555argb_to_565rgb(address_space &space, int x,int y)
{
	int xcnt,ycnt;
	for (ycnt=0;ycnt<32;ycnt++)
	{
		uint32_t realwriteoffs = 0x05000000 + fb_w_sof1 + (y+ycnt) * (fb_w_linestride<<3) + (x*2);
		uint32_t *src = &fake_accumulationbuffer_bitmap->pix32(y+ycnt, x);

		for (xcnt=0;xcnt<32;xcnt++)
		{
			// data starts in 8888 format, downsample it
			uint32_t data = src[xcnt];
			uint16_t newdat = ((((data & 0x000000f8) >> 3)) << 0)   |
							((((data & 0x0000f800) >> 11)) << 5)  |
							((((data & 0x00f80000) >> 19)) << 11);

			space.write_word(realwriteoffs+xcnt*2, newdat);
		}
	}
}

void powervr2_device::fb_convert_1555argb_to_888rgb24(address_space &space, int x,int y)
{
	int xcnt,ycnt;
	for (ycnt=0;ycnt<32;ycnt++)
	{
		uint32_t realwriteoffs = 0x05000000 + fb_w_sof1 + (y+ycnt) * (fb_w_linestride<<3) + (x*3);
		uint32_t *src = &fake_accumulationbuffer_bitmap->pix32(y+ycnt, x);

		for (xcnt=0;xcnt<32;xcnt++)
		{
			// data starts in 8888 format, downsample it
			uint32_t data = src[xcnt];
			uint32_t newdat = (data & 0xf8f8f8);

			space.write_byte(realwriteoffs+xcnt*3+0, newdat >> 0);
			space.write_byte(realwriteoffs+xcnt*3+1, newdat >> 8);
			space.write_byte(realwriteoffs+xcnt*3+2, newdat >> 16);
		}
	}
}

void powervr2_device::fb_convert_1555argb_to_888rgb32(address_space &space, int x,int y)
{
	int xcnt,ycnt;
	for (ycnt=0;ycnt<32;ycnt++)
	{
		uint32_t realwriteoffs = 0x05000000 + fb_w_sof1 + (y+ycnt) * (fb_w_linestride<<3) + (x*4);
		uint32_t *src = &fake_accumulationbuffer_bitmap->pix32(y+ycnt, x);

		for (xcnt=0;xcnt<32;xcnt++)
		{
			// data starts in 8888 format, downsample it
			uint32_t data = src[xcnt];
			uint32_t newdat = (data & 0xf8f8f8);

			space.write_dword(realwriteoffs+xcnt*4, newdat);
		}
	}
}

/* 888RGB = 4 */
void powervr2_device::fb_convert_888rgb_to_555rgb(address_space &space, int x,int y)
{
	int xcnt,ycnt;
	for (ycnt=0;ycnt<32;ycnt++)
	{
		uint32_t realwriteoffs = 0x05000000 + fb_w_sof1 + (y+ycnt) * (fb_w_linestride<<3) + (x*2);
		uint32_t *src = &fake_accumulationbuffer_bitmap->pix32(y+ycnt, x);

		for (xcnt=0;xcnt<32;xcnt++)
		{
			// data starts in 8888 format, downsample it
			uint32_t data = src[xcnt];
			uint16_t newdat = ((((data & 0x000000f8) >> 3)) << 0)   |
							((((data & 0x0000f800) >> 11)) << 5)  |
							((((data & 0x00f80000) >> 16)) << 10);

			space.write_word(realwriteoffs+xcnt*2, newdat);
		}
	}
}

void powervr2_device::fb_convert_888rgb_to_565rgb(address_space &space, int x,int y)
{
	int xcnt,ycnt;
	for (ycnt=0;ycnt<32;ycnt++)
	{
		uint32_t realwriteoffs = 0x05000000 + fb_w_sof1 + (y+ycnt) * (fb_w_linestride<<3) + (x*2);
		uint32_t *src = &fake_accumulationbuffer_bitmap->pix32(y+ycnt, x);

		for (xcnt=0;xcnt<32;xcnt++)
		{
			// data starts in 8888 format, downsample it
			uint32_t data = src[xcnt];
			uint16_t newdat = ((((data & 0x000000f8) >> 3)) << 0)   |
							((((data & 0x0000fc00) >> 11)) << 5)  |
							((((data & 0x00f80000) >> 16)) << 11);

			space.write_word(realwriteoffs+xcnt*2, newdat);
		}
	}
}

void powervr2_device::fb_convert_888rgb_to_888rgb24(address_space &space, int x,int y)
{
	int xcnt,ycnt;
	for (ycnt=0;ycnt<32;ycnt++)
	{
		uint32_t realwriteoffs = 0x05000000 + fb_w_sof1 + (y+ycnt) * (fb_w_linestride<<3) + (x*3);
		uint32_t *src = &fake_accumulationbuffer_bitmap->pix32(y+ycnt, x);

		for (xcnt=0;xcnt<32;xcnt++)
		{
			// data starts in 8888 format, downsample it
			uint32_t data = src[xcnt];
			uint32_t newdat = (data & 0xffffff);

			space.write_byte(realwriteoffs+xcnt*3+0, newdat >> 0);
			space.write_byte(realwriteoffs+xcnt*3+1, newdat >> 8);
			space.write_byte(realwriteoffs+xcnt*3+2, newdat >> 16);
		}
	}
}

void powervr2_device::fb_convert_888rgb_to_888rgb32(address_space &space, int x,int y)
{
	int xcnt,ycnt;
	for (ycnt=0;ycnt<32;ycnt++)
	{
		uint32_t realwriteoffs = 0x05000000 + fb_w_sof1 + (y+ycnt) * (fb_w_linestride<<3) + (x*4);
		uint32_t *src = &fake_accumulationbuffer_bitmap->pix32(y+ycnt, x);

		for (xcnt=0;xcnt<32;xcnt++)
		{
			// data starts in 8888 format, downsample it
			uint32_t data = src[xcnt];
			uint32_t newdat = (data & 0xffffff);

			space.write_dword(realwriteoffs+xcnt*4, newdat);
		}
	}
}


/* 8888ARGB = 6 */
void powervr2_device::fb_convert_8888argb_to_555rgb(address_space &space, int x,int y)
{
	int xcnt,ycnt;
	for (ycnt=0;ycnt<32;ycnt++)
	{
		uint32_t realwriteoffs = 0x05000000 + fb_w_sof1 + (y+ycnt) * (fb_w_linestride<<3) + (x*2);
		uint32_t *src = &fake_accumulationbuffer_bitmap->pix32(y+ycnt, x);

		for (xcnt=0;xcnt<32;xcnt++)
		{
			// data starts in 8888 format, downsample it
			uint32_t data = src[xcnt];
			uint16_t newdat = ((((data & 0x000000f8) >> 3)) << 0)   |
							((((data & 0x0000f800) >> 11)) << 5)  |
							((((data & 0x00f80000) >> 16)) << 10);

			space.write_word(realwriteoffs+xcnt*2, newdat);
		}
	}
}

void powervr2_device::fb_convert_8888argb_to_565rgb(address_space &space, int x,int y)
{
	int xcnt,ycnt;
	for (ycnt=0;ycnt<32;ycnt++)
	{
		uint32_t realwriteoffs = 0x05000000 + fb_w_sof1 + (y+ycnt) * (fb_w_linestride<<3) + (x*2);
		uint32_t *src = &fake_accumulationbuffer_bitmap->pix32(y+ycnt, x);

		for (xcnt=0;xcnt<32;xcnt++)
		{
			// data starts in 8888 format, downsample it
			uint32_t data = src[xcnt];
			uint16_t newdat = ((((data & 0x000000f8) >> 3)) << 0)   |
							((((data & 0x0000fc00) >> 11)) << 5)  |
							((((data & 0x00f80000) >> 16)) << 11);

			space.write_word(realwriteoffs+xcnt*2, newdat);
		}
	}
}

void powervr2_device::fb_convert_8888argb_to_888rgb24(address_space &space, int x,int y)
{
	int xcnt,ycnt;
	for (ycnt=0;ycnt<32;ycnt++)
	{
		uint32_t realwriteoffs = 0x05000000 + fb_w_sof1 + (y+ycnt) * (fb_w_linestride<<3) + (x*3);
		uint32_t *src = &fake_accumulationbuffer_bitmap->pix32(y+ycnt, x);

		for (xcnt=0;xcnt<32;xcnt++)
		{
			// data starts in 8888 format, downsample it
			uint32_t data = src[xcnt];
			uint32_t newdat = (data & 0xffffff);

			space.write_byte(realwriteoffs+xcnt*3+0, newdat >> 0);
			space.write_byte(realwriteoffs+xcnt*3+1, newdat >> 8);
			space.write_byte(realwriteoffs+xcnt*3+2, newdat >> 16);
		}
	}
}

void powervr2_device::fb_convert_8888argb_to_888rgb32(address_space &space, int x,int y)
{
	int xcnt,ycnt;
	for (ycnt=0;ycnt<32;ycnt++)
	{
		uint32_t realwriteoffs = 0x05000000 + fb_w_sof1 + (y+ycnt) * (fb_w_linestride<<3) + (x*4);
		uint32_t *src = &fake_accumulationbuffer_bitmap->pix32(y+ycnt, x);

		for (xcnt=0;xcnt<32;xcnt++)
		{
			// data starts in 8888 format, downsample it
			uint32_t data = src[xcnt];
			uint32_t newdat = (data & 0xffffff);

			space.write_dword(realwriteoffs+xcnt*4, newdat);
		}
	}
}


/*

0x0 0555 KRGB 16 bit (default) Bit 15 is the value of fb_kval 7.
0x1 565 RGB 16 bit
0x2 4444 ARGB 16 bit
0x3 1555 ARGB 16 bit The alpha value is determined by comparison with the value of fb_alpha_threshold.
0x4 888 RGB 24 bit packed
0x5 0888 KRGB 32 bit K is the value of fk_kval.
0x6 8888 ARGB 32 bit
0x7 Setting prohibited.

*/

void powervr2_device::pvr_accumulationbuffer_to_framebuffer(address_space &space, int x,int y)
{
	// the accumulation buffer is always 8888
	//
	// the standard format for the framebuffer appears to be 565
	// yes, this means colour data is lost in the conversion

	uint8_t packmode = fb_w_ctrl & 0x7;
	uint8_t unpackmode = (fb_r_ctrl & 0x0000000c) >>2;  // aka fb_depth

//  popmessage("%02x %02x",packmode,unpackmode);

	switch (packmode)
	{
		// used by ringout
		case 0x00: //0555 KRGB
		{
			switch(unpackmode)
			{
				case 0x00: fb_convert_0555krgb_to_555rgb(space,x,y); break;
				case 0x01: fb_convert_0555krgb_to_565rgb(space,x,y); break;
				case 0x02: fb_convert_0555krgb_to_888rgb24(space,x,y); break;
				case 0x03: fb_convert_0555krgb_to_888rgb32(space,x,y); break;
			}
		}
		break;

		// used by cleoftp
		case 0x01: //565 RGB 16 bit
		{
			switch(unpackmode)
			{
				case 0x00: fb_convert_0565rgb_to_555rgb(space,x,y); break;
				case 0x01: fb_convert_0565rgb_to_565rgb(space,x,y); break;
				case 0x02: fb_convert_0565rgb_to_888rgb24(space,x,y); break;
				case 0x03: fb_convert_0565rgb_to_888rgb32(space,x,y); break;
			}
		}
		break;

		case 0x02:
			printf("pvr_accumulationbuffer_to_framebuffer buffer to tile at %d,%d - unsupported pack mode %02x (4444 ARGB)\n",x,y,packmode);
			break;

		case 0x03: // 1555 ARGB 16 bit
		{
			switch(unpackmode)
			{
				case 0x00: fb_convert_1555argb_to_555rgb(space,x,y); break;
				case 0x01: fb_convert_1555argb_to_565rgb(space,x,y); break;
				case 0x02: fb_convert_1555argb_to_888rgb24(space,x,y); break;
				case 0x03: fb_convert_1555argb_to_888rgb32(space,x,y); break;
			}
		}
		break;

		// used by Suchie3
		case 0x04: // 888 RGB 24-bit
		{
			switch(unpackmode)
			{
				case 0x00: fb_convert_888rgb_to_555rgb(space,x,y); break;
				case 0x01: fb_convert_888rgb_to_565rgb(space,x,y); break;
				case 0x02: fb_convert_888rgb_to_888rgb24(space,x,y); break;
				case 0x03: fb_convert_888rgb_to_888rgb32(space,x,y); break;
			}
		}
		break;

		case 0x05:
			printf("pvr_accumulationbuffer_to_framebuffer buffer to tile at %d,%d - unsupported pack mode %02x (0888 KGB 32-bit)\n",x,y,packmode);
			break;

		case 0x06: // 8888 ARGB 32 bit
		{
			switch(unpackmode)
			{
				case 0x00: fb_convert_8888argb_to_555rgb(space,x,y); break;
				case 0x01: fb_convert_8888argb_to_565rgb(space,x,y); break;
				case 0x02: fb_convert_8888argb_to_888rgb24(space,x,y); break;
				case 0x03: fb_convert_8888argb_to_888rgb32(space,x,y); break;
			}
		}
		break;

		case 0x07:
			printf("pvr_accumulationbuffer_to_framebuffer buffer to tile at %d,%d - unsupported pack mode %02x (Reserved! Don't Use!)\n",x,y,packmode);
			break;
	}


}

void powervr2_device::pvr_drawframebuffer(bitmap_rgb32 &bitmap,const rectangle &cliprect)
{
#if 0
	int x,y,dy,xi;
	uint32_t addrp;
	uint32_t *fbaddr;
	uint32_t c;
	uint32_t r,g,b;
	uint8_t interlace_on = ((spg_control & 0x10) >> 4);
	int32_t ystart_f1 = (vo_starty & 0x3ff) << interlace_on;
	//int32_t ystart_f2 = (vo_starty >> 16) & 0x3ff;
	int32_t hstart = (vo_startx & 0x3ff);
	int res_x,res_y;
//  rectangle fbclip;

	uint8_t unpackmode = (fb_r_ctrl & 0x0000000c) >>2;  // aka fb_depth
	uint8_t enable = (fb_r_ctrl & 0x00000001);

	// ??
	if (!enable) return;

	// only for rgb565 framebuffer
	xi=((fb_r_size & 0x3ff)+1) << 1;
	dy=((fb_r_size >> 10) & 0x3ff)+1;

	dy++;
	dy*=2; // probably depends on interlace mode, fields etc...

//  popmessage("%d %d %d %d %d",ystart_f1,ystart_f2,interlace_on,spg_vblank & 0x3ff,(spg_vblank >> 16) & 0x3ff);

#if 0
	fbclip.min_x = hstart;
	fbclip.min_y = ystart_f1;
	fbclip.max_x = hstart + (xi << ((vo_control & 0x100) >> 8));
	fbclip.max_y = ystart_f1 + dy;

	popmessage("%d %d %d %d",fbclip.min_x,fbclip.min_y,fbclip.max_x,fbclip.max_y);
#endif

	switch (unpackmode)
	{
		case 0x00: // 0555 RGB 16-bit, Cleo Fortune Plus
			// should upsample back to 8-bit output using fb_concat
			for (y=0;y <= dy;y++)
			{
				addrp = fb_r_sof1+y*xi*2;
				if(vo_control & 0x100)
				{
					for (x=0;x < xi;x++)
					{
						res_x = x*2+0 + hstart;
						res_y = y + ystart_f1;

						fbaddr=&bitmap.pix32(res_y, res_x);
						c=*((reinterpret_cast<uint16_t *>(dc_framebuffer_ram)) + (WORD2_XOR_LE(addrp) >> 1));

						b = (c & 0x001f) << 3;
						g = (c & 0x03e0) >> 2;
						r = (c & 0x7c00) >> 7;

						if (cliprect.contains(res_x, res_y))
							*fbaddr = b | (g<<8) | (r<<16);

						res_x = x*2+1 + hstart;

						fbaddr=&bitmap.pix32(res_y, res_x);
						if (cliprect.contains(res_x, res_y))
							*fbaddr = b | (g<<8) | (r<<16);
						addrp+=2;
					}
				}
				else
				{
					for (x=0;x < xi;x++)
					{
						res_x = x + hstart;
						res_y = y + ystart_f1;

						fbaddr=&bitmap.pix32(res_y, res_x);
						c=*((reinterpret_cast<uint16_t *>(dc_framebuffer_ram)) + (WORD2_XOR_LE(addrp) >> 1));

						b = (c & 0x001f) << 3;
						g = (c & 0x03e0) >> 2;
						r = (c & 0x7c00) >> 7;

						if (cliprect.contains(res_x, res_y))
							*fbaddr = b | (g<<8) | (r<<16);
						addrp+=2;
					}
				}
			}

			break;
		case 0x01: // 0565 RGB 16-bit
			// should upsample back to 8-bit output using fb_concat
			for (y=0;y <= dy;y++)
			{
				addrp = fb_r_sof1+y*xi*2;
				if(vo_control & 0x100)
				{
					for (x=0;x < xi;x++)
					{
						res_x = x*2+0 + hstart;
						res_y = y + ystart_f1;
						fbaddr=&bitmap.pix32(res_y, res_x);
						c=*((reinterpret_cast<uint16_t *>(dc_framebuffer_ram)) + (WORD2_XOR_LE(addrp) >> 1));

						b = (c & 0x001f) << 3;
						g = (c & 0x07e0) >> 3;
						r = (c & 0xf800) >> 8;

						if (cliprect.contains(res_x, res_y))
							*fbaddr = b | (g<<8) | (r<<16);

						res_x = x*2+1 + hstart;
						//res_y = y + ystart_f1;
						fbaddr=&bitmap.pix32(res_y, res_x);

						if (cliprect.contains(res_x, res_y))
							*fbaddr = b | (g<<8) | (r<<16);

						addrp+=2;
					}
				}
				else
				{
					for (x=0;x < xi;x++)
					{
						res_x = x + hstart;
						res_y = y + ystart_f1;
						fbaddr=&bitmap.pix32(res_y, res_x);
						c=*((reinterpret_cast<uint16_t *>(dc_framebuffer_ram)) + (WORD2_XOR_LE(addrp) >> 1));

						b = (c & 0x001f) << 3;
						g = (c & 0x07e0) >> 3;
						r = (c & 0xf800) >> 8;

						if (cliprect.contains(res_x, res_y))
							*fbaddr = b | (g<<8) | (r<<16);
						addrp+=2;
					}
				}
			}
			break;

		case 0x02: ; // 888 RGB 24-bit - suchie3, Soul Calibur
			for (y=0;y <= dy;y++)
			{
				addrp = fb_r_sof1+y*xi*2;

				if(vo_control & 0x100)
				{
					for (x=0;x < xi;x++)
					{
						res_x = x*2+0 + hstart;
						res_y = y + ystart_f1;

						fbaddr=&bitmap.pix32(res_y, res_x);

						c =*(uint8_t *)((reinterpret_cast<uint8_t *>(dc_framebuffer_ram)) + BYTE8_XOR_LE(addrp));
						b = c;

						c =*(uint8_t *)((reinterpret_cast<uint8_t *>(dc_framebuffer_ram)) + BYTE8_XOR_LE(addrp+1));
						g = c;

						c =*(uint8_t *)((reinterpret_cast<uint8_t *>(dc_framebuffer_ram)) + BYTE8_XOR_LE(addrp+2));
						r = c;

						if (cliprect.contains(res_x, res_y))
							*fbaddr = b | (g<<8) | (r<<16);

						res_x = x*2+1 + hstart;

						fbaddr=&bitmap.pix32(res_y, res_x);
						if (cliprect.contains(res_x, res_y))
							*fbaddr = b | (g<<8) | (r<<16);

						addrp+=3;
					}
				}
				else
				{
					for (x=0;x < xi;x++)
					{
						res_x = x + hstart;
						res_y = y + ystart_f1;
						fbaddr=&bitmap.pix32(res_y, res_x);

						c =*(uint8_t *)((reinterpret_cast<uint8_t *>(dc_framebuffer_ram)) + BYTE8_XOR_LE(addrp));
						b = c;

						c =*(uint8_t *)((reinterpret_cast<uint8_t *>(dc_framebuffer_ram)) + BYTE8_XOR_LE(addrp+1));
						g = c;

						c =*(uint8_t *)((reinterpret_cast<uint8_t *>(dc_framebuffer_ram)) + BYTE8_XOR_LE(addrp+2));
						r = c;

						if (cliprect.contains(res_x, res_y))
							*fbaddr = b | (g<<8) | (r<<16);

						addrp+=3;
					}
				}
			}
			break;

		case 0x03:        // 0888 ARGB 32-bit
			for (y=0;y <= dy;y++)
			{
				addrp = fb_r_sof1+y*xi*2;

				if(vo_control & 0x100)
				{
					for (x=0;x < xi;x++)
					{
						res_x = x*2+0 + hstart;
						res_y = y + ystart_f1;

						fbaddr=&bitmap.pix32(res_y, res_x);
						c =*((reinterpret_cast<uint32_t *>(dc_framebuffer_ram)) + (WORD2_XOR_LE(addrp) >> 2));

						b = (c & 0x0000ff) >> 0;
						g = (c & 0x00ff00) >> 8;
						r = (c & 0xff0000) >> 16;

						if (cliprect.contains(res_x, res_y))
							*fbaddr = b | (g<<8) | (r<<16);

						res_x = x*2+1 + hstart;

						fbaddr=&bitmap.pix32(res_y, res_x);
						if (cliprect.contains(res_x, res_y))
							*fbaddr = b | (g<<8) | (r<<16);

						addrp+=4;
					}
				}
				else
				{
					for (x=0;x < xi;x++)
					{
						res_x = x + hstart;
						res_y = y + ystart_f1;
						fbaddr=&bitmap.pix32(res_y, res_x);
						c =*((reinterpret_cast<uint32_t *>(dc_framebuffer_ram)) + (WORD2_XOR_LE(addrp) >> 2));

						b = (c & 0x0000ff) >> 0;
						g = (c & 0x00ff00) >> 8;
						r = (c & 0xff0000) >> 16;

						if (cliprect.contains(res_x, res_y))
							*fbaddr = b | (g<<8) | (r<<16);

						addrp+=4;
					}
				}
			}
			break;
	}
#endif
}


#if DEBUG_PALRAM
void powervr2_device::debug_paletteram()
{
	uint64_t pal;
	uint32_t r,g,b;
	int i;

	//popmessage("%02x",pal_ram_ctrl);

	for(i=0;i<0x400;i++)
	{
		pal = palette[i];
		switch(pal_ram_ctrl)
		{
			case 0: //argb1555 <- guilty gear uses this mode
			{
				//a = (pal & 0x8000)>>15;
				r = (pal & 0x7c00)>>10;
				g = (pal & 0x03e0)>>5;
				b = (pal & 0x001f)>>0;
				//a = a ? 0xff : 0x00;
				m_palette->set_pen_color(i, pal5bit(r), pal5bit(g), pal5bit(b));
			}
			break;
			case 1: //rgb565
			{
				//a = 0xff;
				r = (pal & 0xf800)>>11;
				g = (pal & 0x07e0)>>5;
				b = (pal & 0x001f)>>0;
				m_palette->set_pen_color(i, pal5bit(r), pal6bit(g), pal5bit(b));
			}
			break;
			case 2: //argb4444
			{
				//a = (pal & 0xf000)>>12;
				r = (pal & 0x0f00)>>8;
				g = (pal & 0x00f0)>>4;
				b = (pal & 0x000f)>>0;
				m_palette->set_pen_color(i, pal4bit(r), pal4bit(g), pal4bit(b));
			}
			break;
			case 3: //argb8888
			{
				//a = (pal & 0xff000000)>>20;
				r = (pal & 0x00ff0000)>>16;
				g = (pal & 0x0000ff00)>>8;
				b = (pal & 0x000000ff)>>0;
				m_palette->set_pen_color(i, r, g, b);
			}
			break;
		}
	}
}
#endif

/* test video end */

void powervr2_device::pvr_build_parameterconfig()
{
	int a,b,c,d,e,p;

	for (a = 0;a <= 63;a++)
		pvr_parameterconfig[a] = -1;
	p=0;
	// volume,col_type,texture,offset,16bit_uv
	for (a = 0;a <= 1;a++)
		for (b = 0;b <= 3;b++)
			for (c = 0;c <= 1;c++)
				if (c == 0)
				{
					for (d = 0;d <= 1;d++)
						for (e = 0;e <= 1;e++)
							pvr_parameterconfig[(a << 6) | (b << 4) | (c << 3) | (d << 2) | (e << 0)] = pvr_parconfseq[p];
					p++;
				}
				else
					for (d = 0;d <= 1;d++)
						for (e = 0;e <= 1;e++)
						{
							pvr_parameterconfig[(a << 6) | (b << 4) | (c << 3) | (d << 2) | (e << 0)] = pvr_parconfseq[p];
							p++;
						}
	for (a = 1;a <= 63;a++)
		if (pvr_parameterconfig[a] < 0)
			pvr_parameterconfig[a] = pvr_parameterconfig[a-1];
}

TIMER_CALLBACK_MEMBER(powervr2_device::vbin)
{
	irq_cb(VBL_IN_IRQ);

	//popmessage("VII %d VOI %d VI %d VO %d VS %d",spg_vblank_int & 0x3ff,(spg_vblank_int >> 16) & 0x3ff,spg_vblank & 0x3ff,(spg_vblank >> 16) & 0x3ff,(spg_load >> 16) & 0x3ff);
//  vbin_timer->adjust(screen().time_until_pos(spg_vblank_int & 0x3ff));
}

TIMER_CALLBACK_MEMBER(powervr2_device::vbout)
{
	irq_cb(VBL_OUT_IRQ);

//  vbout_timer->adjust(screen().time_until_pos((spg_vblank_int >> 16) & 0x3ff));
}

TIMER_CALLBACK_MEMBER(powervr2_device::hbin)
{
	if(spg_hblank_int & 0x1000)
	{
		if(scanline == next_y)
		{
			irq_cb(HBL_IN_IRQ);
			next_y += spg_hblank_int & 0x3ff;
		}
	}
	else if((scanline == (spg_hblank_int & 0x3ff)) || (spg_hblank_int & 0x2000))
	{
		irq_cb(HBL_IN_IRQ);
	}

//  printf("hbin on scanline %d\n",scanline);

	scanline++;

	if(scanline >= ((spg_load >> 16) & 0x3ff))
	{
		scanline = 0;
		next_y = spg_hblank_int & 0x3ff;
	}

	hbin_timer->adjust(screen().time_until_pos(scanline, ((spg_hblank_int >> 16) & 0x3ff)-1));
}



TIMER_CALLBACK_MEMBER(powervr2_device::endofrender_video)
{
	printf("VIDEO END %d\n",screen().vpos());
//  endofrender_timer_video->adjust(attotime::never);
}

TIMER_CALLBACK_MEMBER(powervr2_device::endofrender_tsp)
{
	printf("TSP END %d\n",screen().vpos());

//  endofrender_timer_tsp->adjust(attotime::never);
//  endofrender_timer_video->adjust(attotime::from_usec(500) );
}

TIMER_CALLBACK_MEMBER(powervr2_device::endofrender_isp)
{
	irq_cb(EOR_ISP_IRQ); // ISP end of render
	irq_cb(EOR_TSP_IRQ); // TSP end of render
	irq_cb(EOR_VIDEO_IRQ); // VIDEO end of render

//  printf("ISP END %d\n",screen().vpos());

	endofrender_timer_isp->adjust(attotime::never);
//  endofrender_timer_tsp->adjust(attotime::from_usec(500) );
}

uint32_t powervr2_device::screen_update(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect)
{
	/******************
	  MAME note
	*******************

	The video update function should NOT be generating interrupts, setting timers or doing _anything_ the game might be able to detect
	as it will be called at different times depending on frameskip etc.

	Rendering should happen when the hardware requests it, to the framebuffer(s)

	Everything else should depend on timers.

	******************/

//  static int useframebuffer=1;
//  const rectangle &visarea = screen.visible_area();
//  int y,x;

#if DEBUG_PALRAM
	debug_paletteram();
#endif

	// copy our fake framebuffer bitmap (where things have been rendered) to the screen
#if 0
	for (y = visarea->min_y ; y <= visarea->max_y ; y++)
	{
		for (x = visarea->min_x ; x <= visarea->max_x ; x++)
		{
			uint32_t* src = &fake_accumulationbuffer_bitmap->pix32(y, x);
			uint32_t* dst = &bitmap.pix32(y, x);
			dst[0] = src[0];
		}
	}

	bitmap.fill(rgb_t(0xff,
							(vo_border_col >> 16) & 0xff,
							(vo_border_col >> 8 ) & 0xff,
							(vo_border_col      ) & 0xff), cliprect); //FIXME: Chroma bit?

	if(!(vo_control & 8))
		pvr_drawframebuffer(bitmap, cliprect);

	// update this here so we only do string lookup once per frame
	debug_dip_status = m_mamedebug->read();
#endif

	int ly = cliprect.min_y + 479;
	if(ly > cliprect.max_y)
		ly = cliprect.max_y;
	int lx1 = cliprect.min_x + 32;
	if(lx1 > cliprect.max_x)
		lx1 = cliprect.max_x;
	int lx2 = lx1+639;
	if(lx2 > cliprect.max_x)
		lx2 = cliprect.max_x;

	for(int y=cliprect.min_y; y<=ly; y++) {
		const uint8_t *src = zerobuf[y-cliprect.min_y][0];
		uint32_t *dst = &bitmap.pix32(y, lx1);
		for(int x=lx1; x<=lx2; x++) {
			*dst++ = (src[0] << 16) | (src[1] << 8) | (src[2] << 0);
			src += 3;
		}
	}
	return 0;
}


/* Naomi 2 attempts (TBD) */

READ32_MEMBER( powervr2_device::pvr2_ta_r )
{
	printf("PVR2 %08x R\n", offset);

	return 0;
}

WRITE32_MEMBER( powervr2_device::pvr2_ta_w )
{
//  int reg;
//  uint64_t shift;
//  uint32_t dat;

//  reg = decode_reg_64(offset, mem_mask, &shift);
//  dat = (uint32_t)(data >> shift);

	//printf("PVR2 %08x %08x\n",reg,dat);
}

// TODO: move to specific device
READ32_MEMBER( powervr2_device::elan_regs_r )
{
	switch(offset)
	{
		case 0x00/4: // ID chip (TODO: BIOS crashes / gives a black screen with this as per now!)
			return 0xe1ad0000;
		case 0x04/4: // REVISION
			return 0x12; //or 0x01?
		case 0x10/4: // SH4 interface control (???)
			/* ---- -x-- enable second PVR */
			/* ---- --x- elan has channel 2 */
			/* ---- ---x broadcast on cs1 (?) */
			return 6;
		case 0x14/4: // SDRAM refresh register
			return 0x2029; //default 0x1429
		case 0x1c/4: // SDRAM CFG
			return 0xa7320961; //default 0xa7320961
		case 0x30/4: // Macro tiler configuration, bit 0 is enable
			return 0;
		case 0x74/4: // IRQ STAT
			return 0;
		case 0x78/4: // IRQ MASK
			return 0;
		default:
			logerror("%s %08x\n", machine().describe_context(),offset*4);
			break;
	}

	return 0;
}

WRITE32_MEMBER( powervr2_device::elan_regs_w )
{
	switch(offset)
	{
		default:
			logerror("%s %08x %08x W\n", machine().describe_context(),offset*4,data);
			break;
	}
}


WRITE32_MEMBER( powervr2_device::pvrs_ta_w )
{
	//  pvr_ta_w(space,offset,data,mem_mask);
	//  pvr2_ta_w(space,offset,data,mem_mask);
	//printf("PVR2 %08x %08x\n",reg,dat);
}

TIMER_CALLBACK_MEMBER(powervr2_device::pvr_dma_irq)
{
	m_pvr_dma.start = sb_pdst = 0;
	irq_cb(DMA_PVR_IRQ);
}

void powervr2_device::pvr_dma_execute(address_space &space)
{
	dc_state *state = machine().driver_data<dc_state>();
	uint32_t src,dst,size;
	dst = m_pvr_dma.pvr_addr;
	src = m_pvr_dma.sys_addr;
	size = 0;

	/* used so far by usagui and sprtjam*/
	printf("PVR-DMA start\n");
	printf("%08x %08x %08x\n",m_pvr_dma.pvr_addr,m_pvr_dma.sys_addr,m_pvr_dma.size);
	printf("src %s dst %08x\n",m_pvr_dma.dir ? "->" : "<-",m_pvr_dma.sel);

	/* Throw illegal address set */
	#if 0
	if((m_pvr_dma.sys_addr & 0x1c000000) != 0x0c000000)
	{
		/* TODO: timing */
		irq_cb(ERR_PVRIF_ILL_ADDR_IRQ);
		m_pvr_dma.start = sb_pdst = 0;
		printf("Illegal PVR DMA set\n");
		return;
	}
	#endif

	/* 0 rounding size = 16 Mbytes */
	if(m_pvr_dma.size == 0) { m_pvr_dma.size = 0x100000; }

	if(m_pvr_dma.dir == 0)
	{
		for(;size<m_pvr_dma.size;size+=4)
		{
			space.write_dword(dst,space.read_dword(src));
			src+=4;
			dst+=4;
		}
	}
	else
	{
		for(;size<m_pvr_dma.size;size+=4)
		{
			space.write_dword(src,space.read_dword(dst));
			src+=4;
			dst+=4;
		}
	}
	/* Note: do not update the params, since this DMA type doesn't support it. */
	/* TODO: timing of this */
	machine().scheduler().timer_set(state->m_maincpu->cycles_to_attotime(m_pvr_dma.size/4), timer_expired_delegate(FUNC(powervr2_device::pvr_dma_irq), this));
}

powervr2_device::powervr2_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
	: device_t(mconfig, POWERVR2, tag, owner, clock),
		device_video_interface(mconfig, *this),
		irq_cb(*this),
		m_mamedebug(*this, ":MAMEDEBUG")
{
}

void powervr2_device::device_start()
{
	irq_cb.resolve_safe();

	grab = make_unique_clear<receiveddata[]>(NUM_BUFFERS);

	pvr_build_parameterconfig();

	computedilated();

//  vbout_timer = machine().scheduler().timer_alloc(timer_expired_delegate(FUNC(powervr2_device::vbout),this));
//  vbin_timer = machine().scheduler().timer_alloc(timer_expired_delegate(FUNC(powervr2_device::vbin),this));
	hbin_timer = machine().scheduler().timer_alloc(timer_expired_delegate(FUNC(powervr2_device::hbin),this));
	yuv_timer_end = machine().scheduler().timer_alloc(timer_expired_delegate(FUNC(powervr2_device::yuv_convert_end),this));

	endofrender_timer_isp = machine().scheduler().timer_alloc(timer_expired_delegate(FUNC(powervr2_device::endofrender_isp),this));
	endofrender_timer_tsp = machine().scheduler().timer_alloc(timer_expired_delegate(FUNC(powervr2_device::endofrender_tsp),this));
	endofrender_timer_video = machine().scheduler().timer_alloc(timer_expired_delegate(FUNC(powervr2_device::endofrender_video),this));

	fake_accumulationbuffer_bitmap = std::make_unique<bitmap_rgb32>(2048,2048);

	softreset = 0;
	test_select = 0;
	param_base = 0;
	region_base = 0;
	span_sort_cfg = 0;
	vo_border_col = 0;
	fb_r_ctrl = 0;
	fb_w_ctrl = 0;
	fb_w_linestride = 0;
	fb_r_sof1 = 0;
	fb_r_sof2 = 0;
	fb_r_size = 0;
	fb_w_sof1 = 0;
	fb_w_sof2 = 0;
	fb_x_clip = 0;
	fb_y_clip = 0;
	fpu_shad_scale = 0;
	fpu_cull_val = 0;
	fpu_param_cfg = 0;
	half_offset = 0;
	fpu_perp_val = 0;
	isp_backgnd_d = 0;
	isp_backgnd_t = 0;
	isp_feed_cfg = 0;
	sdram_refresh = 0;
	sdram_arb_cfg = 0;
	sdram_cfg = 0;
	fog_col_ram = 0;
	fog_col_vert = 0;
	fog_density = 0;
	fog_clamp_max = 0;
	fog_clamp_min = 0;
	spg_trigger_pos = 0;
	spg_hblank_int = 0;
	spg_vblank_int = 0;
	spg_hblank = 0;
	spg_load = 0;
	spg_vblank = 0;
	spg_width = 0;
	spg_control = 0;
	text_control = 0;
	vo_control = 0;
	vo_startx = 0;
	vo_starty = 0;
	scaler_ctl = 0;
	pal_ram_ctrl = 0;
	fb_burstctrl = 0;
	y_coeff = 0;
	pt_alpha_ref = 255;
	ta_ol_base = 0;
	ta_ol_limit = 0;
	ta_isp_base = 0;
	ta_isp_limit = 0;
	ta_next_opb = 0;
	ta_itp_current = 0;
	ta_alloc_ctrl = 0;
	ta_next_opb_init = 0;
	ta_yuv_tex_base = 0;
	ta_yuv_tex_ctrl = 0;
	ta_yuv_tex_cnt = 0;
	memset(fog_table, 0, sizeof(fog_table));
	memset(palette, 0, sizeof(palette));
	memset(&m_pvr_dma, 0x00, sizeof(m_pvr_dma));
	memset(ta_ol_pointers_1, 0, sizeof(ta_ol_pointers_1));
	memset(ta_ol_pointers_2, 0, sizeof(ta_ol_pointers_2));

	ta_clip_min_x = 0;
	ta_clip_max_x = 0;
	ta_clip_min_y = 0;
	ta_clip_max_y = 0;

	sb_pdstap = 0;
	sb_pdstar = 0;
	sb_pdlen = 0;
	sb_pddir = 0;
	sb_pdtsel = 0;
	sb_pden = 0;
	sb_pdst = 0;
	sb_pdapro = 0;

	sdram = auto_alloc_array(machine(), uint32_t, 4*1024*1024);
	isp_buffer = auto_alloc_array(machine(), isp_buffer_t, 128*1024);

	save_pointer(NAME(sdram),      4*1024*1024);
	// Don't same isp_buffer or isp_buffer_counts, they're transcient

	save_item(NAME(softreset));
	save_item(NAME(test_select));
	save_item(NAME(param_base));
	save_item(NAME(region_base));
	save_item(NAME(span_sort_cfg));
	save_item(NAME(vo_border_col));
	save_item(NAME(fb_r_ctrl));
	save_item(NAME(fb_w_ctrl));
	save_item(NAME(fb_w_linestride));
	save_item(NAME(fb_r_sof1));
	save_item(NAME(fb_r_sof2));
	save_item(NAME(fb_r_size));
	save_item(NAME(fb_w_sof1));
	save_item(NAME(fb_w_sof2));
	save_item(NAME(fb_x_clip));
	save_item(NAME(fb_y_clip));
	save_item(NAME(fpu_shad_scale));
	save_item(NAME(fpu_cull_val));
	save_item(NAME(fpu_param_cfg));
	save_item(NAME(half_offset));
	save_item(NAME(fpu_perp_val));
	save_item(NAME(isp_backgnd_d));
	save_item(NAME(isp_backgnd_t));
	save_item(NAME(isp_feed_cfg));
	save_item(NAME(sdram_refresh));
	save_item(NAME(sdram_arb_cfg));
	save_item(NAME(sdram_cfg));
	save_item(NAME(fog_col_ram));
	save_item(NAME(fog_col_vert));
	save_item(NAME(fog_density));
	save_item(NAME(fog_clamp_max));
	save_item(NAME(fog_clamp_min));
	save_item(NAME(spg_trigger_pos));
	save_item(NAME(spg_hblank_int));
	save_item(NAME(spg_vblank_int));
	save_item(NAME(spg_hblank));
	save_item(NAME(spg_load));
	save_item(NAME(spg_vblank));
	save_item(NAME(spg_width));
	save_item(NAME(text_control));
	save_item(NAME(vo_control));
	save_item(NAME(vo_startx));
	save_item(NAME(vo_starty));
	save_item(NAME(scaler_ctl));
	save_item(NAME(pal_ram_ctrl));
	save_item(NAME(fb_burstctrl));
	save_item(NAME(y_coeff));
	save_item(NAME(pt_alpha_ref));
	save_item(NAME(ta_ol_base));
	save_item(NAME(ta_ol_limit));
	save_item(NAME(ta_isp_base));
	save_item(NAME(ta_isp_limit));
	save_item(NAME(ta_next_opb));
	save_item(NAME(ta_itp_current));
	save_item(NAME(ta_alloc_ctrl));
	save_item(NAME(ta_next_opb_init));
	save_item(NAME(ta_yuv_tex_base));
	save_item(NAME(ta_yuv_tex_ctrl));
	save_item(NAME(ta_yuv_tex_cnt));
	save_item(NAME(fog_table));
	save_item(NAME(ta_ol_pointers_1));
	save_item(NAME(ta_ol_pointers_2));
	save_item(NAME(palette));

	save_item(NAME(sb_pdstap));
	save_item(NAME(sb_pdstar));
	save_item(NAME(sb_pdlen));
	save_item(NAME(sb_pddir));
	save_item(NAME(sb_pdtsel));
	save_item(NAME(sb_pden));
	save_item(NAME(sb_pdst));
	save_item(NAME(sb_pdapro));

	save_item(NAME(ta_fifo_buf));
	save_item(NAME(ta_fifo_pos));
	save_item(NAME(ta_packet_len));
	save_item(NAME(ta_packet_type));
	save_item(NAME(ta_cmd_type));
	save_item(NAME(ta_list_idx));
	save_item(NAME(ta_list_type));

	save_item(NAME(ta_cmd_header));
	save_item(NAME(ta_cmd_instr));
	save_item(NAME(ta_cmd_tsp));
	save_item(NAME(ta_cmd_tex));
	save_item(NAME(ta_cmd_color_base));
	save_item(NAME(ta_cmd_color_offset));
	save_item(NAME(ta_cmd_color_sprite_base));
	save_item(NAME(ta_cmd_color_sprite_offset));
	save_item(NAME(ta_cmd_strip_length));
	save_item(NAME(ta_cmd_user_clip_mode));

	save_item(NAME(ta_clip_min_x));
	save_item(NAME(ta_clip_max_x));
	save_item(NAME(ta_clip_min_y));
	save_item(NAME(ta_clip_max_y));

	save_item(NAME(ta_vertex_current.header));
	save_item(NAME(ta_vertex_current.coords));
	save_item(NAME(ta_vertex_current.color_base));
	save_item(NAME(ta_vertex_current.color_offset));
	save_item(NAME(ta_vertex_current.uv));
	save_item(NAME(ta_vertex_current.uvc));
	for(int i=0; i<8; i++) {
		save_item(NAME(ta_vertex_strip[i].header), i);
		save_item(NAME(ta_vertex_strip[i].coords), i);
		save_item(NAME(ta_vertex_strip[i].color_base), i);
		save_item(NAME(ta_vertex_strip[i].color_offset), i);
		save_item(NAME(ta_vertex_strip[i].uv), i);
		save_item(NAME(ta_vertex_strip[i].uvc), i);
	}
	save_item(NAME(ta_vertex_count));
	save_item(NAME(ta_vertex_odd_tri));
	save_item(NAME(ta_vertex_shadow_first));
	save_item(NAME(ta_vertex_shadow_bbox.min_x));
	save_item(NAME(ta_vertex_shadow_bbox.min_y));
	save_item(NAME(ta_vertex_shadow_bbox.max_x));
	save_item(NAME(ta_vertex_shadow_bbox.max_y));

	save_item(NAME(ta_olist_pos));
	save_item(NAME(ta_olist_pos_base));
	save_item(NAME(ta_olist_tile));
	save_item(NAME(ta_olist_block_size));

	save_item(NAME(texture_path_width));

	save_item(NAME(m_pvr_dma.pvr_addr));
	save_item(NAME(m_pvr_dma.sys_addr));
	save_item(NAME(m_pvr_dma.size));
	save_item(NAME(m_pvr_dma.sel));
	save_item(NAME(m_pvr_dma.dir));
	save_item(NAME(m_pvr_dma.flag));
	save_item(NAME(m_pvr_dma.start));
	save_item(NAME(debug_dip_status));

	save_item(NAME(scanline));
	save_item(NAME(next_y));
}

void powervr2_device::device_reset()
{
	memset(sdram, 0, 4*1024*1024*4);

	softreset =                 0x00000007;
	vo_control =                0x00000108;
	vo_startx =                 0x0000009d;
	vo_starty =                 0x00150015;
	spg_hblank =                0x007e0345;
	spg_load =                  0x01060359;
	spg_vblank =                0x01500104;
	spg_hblank_int =            0x031d0000;
	spg_vblank_int =            0x01500104;

	ta_fifo_pos = 0;
	memset(ta_fifo_buf, 0, sizeof(ta_fifo_buf));
	ta_packet_len = 0;
	ta_packet_type = VAR_UNKNOWN;
	ta_cmd_type = VAR_UNKNOWN;
	ta_list_idx = 0;
	ta_list_type = -1;

	ta_cmd_header = 0;
	ta_cmd_instr = 0;
	memset(ta_cmd_tsp, 0, sizeof(ta_cmd_tsp));
	memset(ta_cmd_tex, 0, sizeof(ta_cmd_tex));
	memset(ta_cmd_color_base, 0, sizeof(ta_cmd_color_base));
	memset(ta_cmd_color_offset, 0, sizeof(ta_cmd_color_offset));
	ta_cmd_color_sprite_base = 0;
	ta_cmd_color_sprite_offset = 0;
	ta_cmd_strip_length = 0;
	ta_cmd_user_clip_mode = 0;

	memset(&ta_vertex_current, 0, sizeof(ta_vertex_current));
	memset(ta_vertex_strip, 0, sizeof(ta_vertex_strip));
	ta_vertex_count = 0;
	ta_vertex_odd_tri = false;
	ta_vertex_shadow_first = true;
	memset(&ta_vertex_shadow_bbox, 0, sizeof(ta_vertex_shadow_bbox));

	ta_olist_pos = 0;
	ta_olist_pos_base = 0;
	ta_olist_tile = 0;
	ta_olist_block_size = 0;
	ta_olist_line_size = 0;

	texture_path_width[0] = false;
	texture_path_width[1] = false;

	start_render_received=0;
	renderselect= -1;
	grabsel=0;

//  vbout_timer->adjust(screen().time_until_pos((spg_vblank_int >> 16) & 0x3ff));
//  vbin_timer->adjust(screen().time_until_pos(spg_vblank_int & 0x3ff));
	hbin_timer->adjust(screen().time_until_pos(0, ((spg_hblank_int >> 16) & 0x3ff)-1));

	scanline = 0;
	next_y = 0;

	endofrender_timer_isp->adjust(attotime::never);
	endofrender_timer_tsp->adjust(attotime::never);
	endofrender_timer_video->adjust(attotime::never);
	yuv_timer_end->adjust(attotime::never);

	dc_texture_ram = NULL;
	dc_framebuffer_ram = NULL;
}

/* called by TIMER_ADD_PERIODIC, in driver sections (controlled by SPG, that's a PVR sub-device) */
void powervr2_device::pvr_scanline_timer(int vpos)
{
	int vbin_line = spg_vblank_int & 0x3ff;
	int vbout_line = (spg_vblank_int >> 16) & 0x3ff;
	uint8_t interlace_on = ((spg_control & 0x10) >> 4);
	dc_state *state = machine().driver_data<dc_state>();

	vbin_line <<= interlace_on;
	vbout_line <<= interlace_on;

	if(vbin_line-(1+interlace_on) == vpos)
		state->m_maple->maple_hw_trigger();

	if(vbin_line == vpos)
		irq_cb(VBL_IN_IRQ);

	if(vbout_line == vpos)
		irq_cb(VBL_OUT_IRQ);
}
