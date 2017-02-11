// license:BSD-3-Clause
// copyright-holders:Olivier Galibert
#ifndef MAME_VIDEO_POWERVR2_H
#define MAME_VIDEO_POWERVR2_H

#pragma once

#define MCFG_POWERVR2_ADD(_tag, _irq_cb)                                \
	MCFG_DEVICE_ADD(_tag, POWERVR2, 0)                                  \
	downcast<powervr2_device *>(device)->set_irq_cb(DEVCB_ ## _irq_cb);

class powervr2_device : public device_t,
						public device_video_interface
{
public:
	enum { NUM_BUFFERS = 4 };
	enum {
		EOXFER_YUV_IRQ,
		EOXFER_OPLST_IRQ,
		EOXFER_OPMV_IRQ,
		EOXFER_TRLST_IRQ,
		EOXFER_TRMV_IRQ,
		EOXFER_PTLST_IRQ,
		VBL_IN_IRQ,
		VBL_OUT_IRQ,
		HBL_IN_IRQ,
		EOR_VIDEO_IRQ,
		EOR_TSP_IRQ,
		EOR_ISP_IRQ,
		DMA_PVR_IRQ,
		ERR_ISP_LIMIT_IRQ,
		ERR_PVRIF_ILL_ADDR_IRQ
	};

	void ta_map(address_map &map);
	void pd_dma_map(address_map &map);

	struct {
		uint32_t pvr_addr;
		uint32_t sys_addr;
		uint32_t size;
		uint8_t sel;
		uint8_t dir;
		uint8_t flag;
		uint8_t start;
	} m_pvr_dma;

	static const int pvr_parconfseq[];
	static const int pvr_wordsvertex[24];
	static const int pvr_wordspolygon[24];
	int pvr_parameterconfig[128];
	uint32_t dilated0[15][1024];
	uint32_t dilated1[15][1024];
	int dilatechose[64];
	float wbuffer[480][640];


	// the real accumulation buffer is a 32x32x8bpp buffer into which tiles get rendered before they get copied to the framebuffer
	//  our implementation is not currently tile based, and thus the accumulation buffer is screen sized
	std::unique_ptr<bitmap_rgb32> fake_accumulationbuffer_bitmap;

	struct texinfo  {
		uint32_t address, vqbase;
		uint32_t nontextured_pal_int;
		uint8_t nontextured_fpal_a,nontextured_fpal_r,nontextured_fpal_g,nontextured_fpal_b;
		int textured, sizex, sizey, stride, sizes, pf, palette, mode, mipmapped, blend_mode, filter_mode;
		int coltype;

		uint32_t (powervr2_device::*r)(struct texinfo *t, float x, float y);
		uint32_t (*blend)(uint32_t s, uint32_t d);
		int (*u_func)(float uv, int size);
		int (*v_func)(float uv, int size);
		int palbase, cd;
	};

	typedef struct
	{
		float x, y, w, u, v;
	} vert;

	struct strip
	{
		int svert, evert;
		texinfo ti;
	};

	struct receiveddata {
		vert verts[65536];
		strip strips[65536];

		int verts_size, strips_size;
		uint32_t ispbase;
		uint32_t fbwsof1;
		uint32_t fbwsof2;
		int busy;
		int valid;
	};

	enum {
		TEX_FILTER_NEAREST = 0,
		TEX_FILTER_BILINEAR,
		TEX_FILTER_TRILINEAR_A,
		TEX_FILTER_TRILINEAR_B
	};

	int tafifo_vertexwords, tafifo_listtype;
	int start_render_received;
	int renderselect;
	int listtype_used;
	int alloc_ctrl_OPB_Mode, alloc_ctrl_PT_OPB, alloc_ctrl_TM_OPB, alloc_ctrl_T_OPB, alloc_ctrl_OM_OPB, alloc_ctrl_O_OPB;
	std::unique_ptr<receiveddata[]> grab;
	int grabsel;
	int grabsellast;
	uint32_t paracontrol,paratype,endofstrip,listtype,global_paratype,parameterconfig;
	uint32_t groupcontrol,groupen,striplen,userclip;
	uint32_t objcontrol,shadow,volume,coltype,texture,offfset,gouraud,uv16bit;
	uint32_t texturesizes,textureaddress,scanorder,pixelformat;
	uint32_t blend_mode, srcselect,dstselect,fogcontrol,colorclamp, use_alpha;
	uint32_t ignoretexalpha,flipuv,clampuv,filtermode,sstexture,mmdadjust,tsinstruction;
	uint32_t depthcomparemode,cullingmode,zwritedisable,cachebypass,dcalcctrl,volumeinstruction,mipmapped,vqcompressed,strideselect,paletteselector;
	uint32_t nontextured_pal_int;
	float nontextured_fpal_a,nontextured_fpal_r,nontextured_fpal_g,nontextured_fpal_b;

	uint64_t *dc_texture_ram;
	uint64_t *dc_framebuffer_ram;

	uint64_t *pvr2_texture_ram;
	uint64_t *pvr2_framebuffer_ram;
	uint64_t *elan_ram;

	uint32_t debug_dip_status;
	emu_timer *vbout_timer;
	emu_timer *vbin_timer;
	emu_timer *hbin_timer;
	emu_timer *endofrender_timer_isp;
	emu_timer *endofrender_timer_tsp;
	emu_timer *endofrender_timer_video;
	emu_timer *yuv_timer_end;
	int scanline;
	int next_y;

	powervr2_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);
	template<class _cb> void set_irq_cb(_cb cb) { irq_cb.set_callback(cb); }

	DECLARE_READ64_MEMBER(  tex64_r );
	DECLARE_WRITE64_MEMBER( tex64_w );
	DECLARE_READ32_MEMBER(  tex32_r );
	DECLARE_WRITE32_MEMBER( tex32_w );

	DECLARE_READ32_MEMBER(  id_r );
	DECLARE_READ32_MEMBER(  revision_r );
	DECLARE_READ32_MEMBER(  softreset_r );
	DECLARE_WRITE32_MEMBER( softreset_w );
	DECLARE_READ32_MEMBER(  startrender_r );
	DECLARE_WRITE32_MEMBER( startrender_w );
	DECLARE_READ32_MEMBER(  test_select_r );
	DECLARE_WRITE32_MEMBER( test_select_w );
	DECLARE_READ32_MEMBER(  param_base_r );
	DECLARE_WRITE32_MEMBER( param_base_w );
	DECLARE_READ32_MEMBER(  region_base_r );
	DECLARE_WRITE32_MEMBER( region_base_w );
	DECLARE_READ32_MEMBER(  span_sort_cfg_r );
	DECLARE_WRITE32_MEMBER( span_sort_cfg_w );
	DECLARE_READ32_MEMBER(  vo_border_col_r );
	DECLARE_WRITE32_MEMBER( vo_border_col_w );
	DECLARE_READ32_MEMBER(  fb_r_ctrl_r );
	DECLARE_WRITE32_MEMBER( fb_r_ctrl_w );
	DECLARE_READ32_MEMBER(  fb_w_ctrl_r );
	DECLARE_WRITE32_MEMBER( fb_w_ctrl_w );
	DECLARE_READ32_MEMBER(  fb_w_linestride_r );
	DECLARE_WRITE32_MEMBER( fb_w_linestride_w );
	DECLARE_READ32_MEMBER(  fb_r_sof1_r );
	DECLARE_WRITE32_MEMBER( fb_r_sof1_w );
	DECLARE_READ32_MEMBER(  fb_r_sof2_r );
	DECLARE_WRITE32_MEMBER( fb_r_sof2_w );
	DECLARE_READ32_MEMBER(  fb_r_size_r );
	DECLARE_WRITE32_MEMBER( fb_r_size_w );
	DECLARE_READ32_MEMBER(  fb_w_sof1_r );
	DECLARE_WRITE32_MEMBER( fb_w_sof1_w );
	DECLARE_READ32_MEMBER(  fb_w_sof2_r );
	DECLARE_WRITE32_MEMBER( fb_w_sof2_w );
	DECLARE_READ32_MEMBER(  fb_x_clip_r );
	DECLARE_WRITE32_MEMBER( fb_x_clip_w );
	DECLARE_READ32_MEMBER(  fb_y_clip_r );
	DECLARE_WRITE32_MEMBER( fb_y_clip_w );
	DECLARE_READ32_MEMBER(  fpu_shad_scale_r );
	DECLARE_WRITE32_MEMBER( fpu_shad_scale_w );
	DECLARE_READ32_MEMBER(  fpu_cull_val_r );
	DECLARE_WRITE32_MEMBER( fpu_cull_val_w );
	DECLARE_READ32_MEMBER(  fpu_param_cfg_r );
	DECLARE_WRITE32_MEMBER( fpu_param_cfg_w );
	DECLARE_READ32_MEMBER(  half_offset_r );
	DECLARE_WRITE32_MEMBER( half_offset_w );
	DECLARE_READ32_MEMBER(  fpu_perp_val_r );
	DECLARE_WRITE32_MEMBER( fpu_perp_val_w );
	DECLARE_READ32_MEMBER(  isp_backgnd_d_r );
	DECLARE_WRITE32_MEMBER( isp_backgnd_d_w );
	DECLARE_READ32_MEMBER(  isp_backgnd_t_r );
	DECLARE_WRITE32_MEMBER( isp_backgnd_t_w );
	DECLARE_READ32_MEMBER(  isp_feed_cfg_r );
	DECLARE_WRITE32_MEMBER( isp_feed_cfg_w );
	DECLARE_READ32_MEMBER(  sdram_refresh_r );
	DECLARE_WRITE32_MEMBER( sdram_refresh_w );
	DECLARE_READ32_MEMBER(  sdram_arb_cfg_r );
	DECLARE_WRITE32_MEMBER( sdram_arb_cfg_w );
	DECLARE_READ32_MEMBER(  sdram_cfg_r );
	DECLARE_WRITE32_MEMBER( sdram_cfg_w );
	DECLARE_READ32_MEMBER(  fog_col_ram_r );
	DECLARE_WRITE32_MEMBER( fog_col_ram_w );
	DECLARE_READ32_MEMBER(  fog_col_vert_r );
	DECLARE_WRITE32_MEMBER( fog_col_vert_w );
	DECLARE_READ32_MEMBER(  fog_density_r );
	DECLARE_WRITE32_MEMBER( fog_density_w );
	DECLARE_READ32_MEMBER(  fog_clamp_max_r );
	DECLARE_WRITE32_MEMBER( fog_clamp_max_w );
	DECLARE_READ32_MEMBER(  fog_clamp_min_r );
	DECLARE_WRITE32_MEMBER( fog_clamp_min_w );
	DECLARE_READ32_MEMBER(  spg_trigger_pos_r );
	DECLARE_WRITE32_MEMBER( spg_trigger_pos_w );
	DECLARE_READ32_MEMBER(  spg_hblank_int_r );
	DECLARE_WRITE32_MEMBER( spg_hblank_int_w );
	DECLARE_READ32_MEMBER(  spg_vblank_int_r );
	DECLARE_WRITE32_MEMBER( spg_vblank_int_w );
	DECLARE_READ32_MEMBER(  spg_control_r );
	DECLARE_WRITE32_MEMBER( spg_control_w );
	DECLARE_READ32_MEMBER(  spg_hblank_r );
	DECLARE_WRITE32_MEMBER( spg_hblank_w );
	DECLARE_READ32_MEMBER(  spg_load_r );
	DECLARE_WRITE32_MEMBER( spg_load_w );
	DECLARE_READ32_MEMBER(  spg_vblank_r );
	DECLARE_WRITE32_MEMBER( spg_vblank_w );
	DECLARE_READ32_MEMBER(  spg_width_r );
	DECLARE_WRITE32_MEMBER( spg_width_w );
	DECLARE_READ32_MEMBER(  text_control_r );
	DECLARE_WRITE32_MEMBER( text_control_w );
	DECLARE_READ32_MEMBER(  vo_control_r );
	DECLARE_WRITE32_MEMBER( vo_control_w );
	DECLARE_READ32_MEMBER(  vo_startx_r );
	DECLARE_WRITE32_MEMBER( vo_startx_w );
	DECLARE_READ32_MEMBER(  vo_starty_r );
	DECLARE_WRITE32_MEMBER( vo_starty_w );
	DECLARE_READ32_MEMBER(  scaler_ctl_r );
	DECLARE_WRITE32_MEMBER( scaler_ctl_w );
	DECLARE_READ32_MEMBER(  pal_ram_ctrl_r );
	DECLARE_WRITE32_MEMBER( pal_ram_ctrl_w );
	DECLARE_READ32_MEMBER(  spg_status_r );
	DECLARE_READ32_MEMBER(  fb_burstctrl_r );
	DECLARE_WRITE32_MEMBER( fb_burstctrl_w );
	DECLARE_READ32_MEMBER(  y_coeff_r );
	DECLARE_WRITE32_MEMBER( y_coeff_w );
	DECLARE_READ32_MEMBER(  pt_alpha_ref_r );
	DECLARE_WRITE32_MEMBER( pt_alpha_ref_w );
	DECLARE_READ32_MEMBER(  ta_ol_base_r );
	DECLARE_WRITE32_MEMBER( ta_ol_base_w );
	DECLARE_READ32_MEMBER(  ta_isp_base_r );
	DECLARE_WRITE32_MEMBER( ta_isp_base_w );
	DECLARE_READ32_MEMBER(  ta_ol_limit_r );
	DECLARE_WRITE32_MEMBER( ta_ol_limit_w );
	DECLARE_READ32_MEMBER(  ta_isp_limit_r );
	DECLARE_WRITE32_MEMBER( ta_isp_limit_w );
	DECLARE_READ32_MEMBER(  ta_next_opb_r );
	DECLARE_READ32_MEMBER(  ta_itp_current_r );
	DECLARE_READ32_MEMBER(  ta_glob_tile_clip_r );
	DECLARE_WRITE32_MEMBER( ta_glob_tile_clip_w );
	DECLARE_READ32_MEMBER(  ta_alloc_ctrl_r );
	DECLARE_WRITE32_MEMBER( ta_alloc_ctrl_w );
	DECLARE_READ32_MEMBER(  ta_list_init_r );
	DECLARE_WRITE32_MEMBER( ta_list_init_w );
	DECLARE_READ32_MEMBER(  ta_yuv_tex_base_r );
	DECLARE_WRITE32_MEMBER( ta_yuv_tex_base_w );
	DECLARE_READ32_MEMBER(  ta_yuv_tex_ctrl_r );
	DECLARE_WRITE32_MEMBER( ta_yuv_tex_ctrl_w );
	DECLARE_READ32_MEMBER(  ta_yuv_tex_cnt_r );
	DECLARE_WRITE32_MEMBER( ta_yuv_tex_cnt_w );
	DECLARE_READ32_MEMBER(  ta_list_cont_r );
	DECLARE_WRITE32_MEMBER( ta_list_cont_w );
	DECLARE_READ32_MEMBER(  ta_next_opb_init_r );
	DECLARE_WRITE32_MEMBER( ta_next_opb_init_w );
	DECLARE_READ32_MEMBER(  fog_table_r );
	DECLARE_WRITE32_MEMBER( fog_table_w );
	DECLARE_READ32_MEMBER(  ta_ol_pointers_1_r );
	DECLARE_READ32_MEMBER(  ta_ol_pointers_2_r );
	DECLARE_READ32_MEMBER(  palette_r );
	DECLARE_WRITE32_MEMBER( palette_w );

	DECLARE_READ32_MEMBER(  sb_pdstap_r );
	DECLARE_WRITE32_MEMBER( sb_pdstap_w );
	DECLARE_READ32_MEMBER(  sb_pdstar_r );
	DECLARE_WRITE32_MEMBER( sb_pdstar_w );
	DECLARE_READ32_MEMBER(  sb_pdlen_r );
	DECLARE_WRITE32_MEMBER( sb_pdlen_w );
	DECLARE_READ32_MEMBER(  sb_pddir_r );
	DECLARE_WRITE32_MEMBER( sb_pddir_w );
	DECLARE_READ32_MEMBER(  sb_pdtsel_r );
	DECLARE_WRITE32_MEMBER( sb_pdtsel_w );
	DECLARE_READ32_MEMBER(  sb_pden_r );
	DECLARE_WRITE32_MEMBER( sb_pden_w );
	DECLARE_READ32_MEMBER(  sb_pdst_r );
	DECLARE_WRITE32_MEMBER( sb_pdst_w );
	DECLARE_READ32_MEMBER(  sb_pdapro_r );
	DECLARE_WRITE32_MEMBER( sb_pdapro_w );

	DECLARE_READ32_MEMBER(  pvr2_ta_r );
	DECLARE_WRITE32_MEMBER( pvr2_ta_w );
	DECLARE_WRITE32_MEMBER( pvrs_ta_w );
	DECLARE_READ32_MEMBER(  elan_regs_r );
	DECLARE_WRITE32_MEMBER( elan_regs_w );
	DECLARE_WRITE64_MEMBER( ta_fifo_poly_w );
	DECLARE_WRITE8_MEMBER(  ta_fifo_yuv_w );
	DECLARE_WRITE64_MEMBER( texture_path0_w );
	DECLARE_WRITE64_MEMBER( texture_path1_w );

	TIMER_CALLBACK_MEMBER(vbin);
	TIMER_CALLBACK_MEMBER(vbout);
	TIMER_CALLBACK_MEMBER(hbin);
	TIMER_CALLBACK_MEMBER(yuv_convert_end);
	TIMER_CALLBACK_MEMBER(endofrender_video);
	TIMER_CALLBACK_MEMBER(endofrender_tsp);
	TIMER_CALLBACK_MEMBER(endofrender_isp);
	TIMER_CALLBACK_MEMBER(transfer_opaque_list_irq);
	TIMER_CALLBACK_MEMBER(transfer_opaque_modifier_volume_list_irq);
	TIMER_CALLBACK_MEMBER(transfer_translucent_list_irq);
	TIMER_CALLBACK_MEMBER(transfer_translucent_modifier_volume_list_irq);
	TIMER_CALLBACK_MEMBER(transfer_punch_through_list_irq);
	TIMER_CALLBACK_MEMBER(pvr_dma_irq);

	void set_texture_path_mode(int channel, bool bits32);

	void pvr_dma_execute(address_space &space);
	void pvr_scanline_timer(int vpos);
	uint32_t screen_update(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect);

protected:
	virtual void device_start() override;
	virtual void device_reset() override;

private:
	enum {
		TRI_G_F32,
		TRI_G_I_GLB,
		TRI_G_I_GLB_DUAL,
		TRI_G_I_PREV,
		TRI_G_I_PREV_DUAL,
		TRI_G_U8,
		TRI_G_U8_DUAL,
		TRI_T_F32,
		TRI_T_F32_CUV,
		TRI_T_I_GLB,
		TRI_T_I_GLB_CUV,
		TRI_T_I_GLB_CUV_DUAL,
		TRI_T_I_GLB_DUAL,
		TRI_T_I_GLB_OFF,
		TRI_T_I_GLB_OFF_CUV,
		TRI_T_I_PREV,
		TRI_T_I_PREV_CUV,
		TRI_T_I_PREV_CUV_DUAL,
		TRI_T_I_PREV_DUAL,
		TRI_T_U8,
		TRI_T_U8_CUV,
		TRI_T_U8_CUV_DUAL,
		TRI_T_U8_DUAL,

		SHADOW,
		SPRITE,
		LINE_SPRITE,

		CMD_END_OF_LIST,
		CMD_USER_TILE_CLIP,
		CMD_OBJECT_LIST_SET,

		VAR_UNKNOWN,
		VAR_COUNT
	};

	enum {
		L_OPAQUE,
		L_OPAQUE_SHADOW,
		L_TRANS,
		L_TRANS_SHADOW,
		L_TRANS_PUNCHTHROUGH
	};

	enum {
		H_TYPE        = 0xe0000000,
		H_EOSTRIP     = 0x10000000,
		H_LTYPE       = 0x07000000,
		H_GROUP       = 0x00800000,
		H_SLEN        = 0x000c0000,
		H_UCLIP       = 0x00030000,
		H_DUAL        = 0x00000040,
		H_COLMODE     = 0x00000030,
		H_TEX         = 0x00000008,
		H_OFF         = 0x00000004,
		H_GOURAUD     = 0x00000002,
		H_COMPACT_UV  = 0x00000001,

		HI_DEPTH      = 0xe0000000,
		HI_CULL       = 0x18000000,
		HI_ZWRITE     = 0x04000000,
		HI_TEX        = 0x02000000,
		HI_OFF        = 0x01000000,
		HI_GOURAUD    = 0x00800000,
		HI_COMPACT_UV = 0x00400000,
		HI_CACHE      = 0x00200000,
		HI_DCOEFF     = 0x00100000,
	};

	enum {
		OL_POINTERS_COUNT = 40*15,
		TA_SORTER_SIZE    = 16
	};

	static const char *ta_packet_type_name[VAR_COUNT];
	static const int ta_packet_len_table[VAR_COUNT*2];

	devcb_write8 irq_cb;
	required_ioport m_mamedebug;

	// SDRAM, 4 pairs of 1M*16 sdrams, total of 16M
	uint32_t *sdram;

	uint8_t zerobuf[480][640][3];

	// setup->isp buffer, seemingly 128K entries 128-bits wide each
	struct isp_buffer_t {
		uint32_t a, b, c, d;
	};

	struct setup_poly_t {
		uint32_t op, ciw, paddr, last_object_pos;
		uint32_t coords[4][3];
		float low_y, high_y;
		float xo, yo;
		int list_id, tri, last_object_list;
		bool isquad, shadow, shadow_2vol;
	};

	isp_buffer_t *isp_buffer;
	uint32_t isp_buffer_counts[5], isp_buffer_pos;

	// Core registers
	uint32_t softreset;
	uint32_t test_select;
	uint32_t param_base, region_base;
	uint32_t span_sort_cfg;
	uint32_t vo_border_col;
	uint32_t fb_r_ctrl, fb_w_ctrl, fb_w_linestride, fb_r_sof1, fb_r_sof2, fb_r_size, fb_w_sof1, fb_w_sof2, fb_x_clip, fb_y_clip;
	uint32_t fpu_shad_scale, fpu_cull_val, fpu_param_cfg, half_offset, fpu_perp_val;
	uint32_t isp_backgnd_d, isp_backgnd_t, isp_feed_cfg;
	uint32_t sdram_refresh, sdram_arb_cfg, sdram_cfg;
	uint32_t fog_col_ram, fog_col_vert, fog_density, fog_clamp_max, fog_clamp_min;
	uint32_t spg_trigger_pos, spg_hblank_int, spg_vblank_int, spg_control, spg_hblank, spg_load, spg_vblank, spg_width;
	uint32_t text_control;
	uint32_t vo_control, vo_startx, vo_starty;
	uint32_t scaler_ctl, pal_ram_ctrl, fb_burstctrl, y_coeff, pt_alpha_ref;

	// TA registers
	uint32_t ta_ol_base, ta_ol_limit, ta_isp_base, ta_isp_limit;
	uint32_t ta_next_opb, ta_itp_current, ta_glob_tile_clip, ta_alloc_ctrl, ta_next_opb_init;
	uint32_t ta_yuv_tex_base, ta_yuv_tex_ctrl, ta_yuv_tex_cnt;
	uint32_t ta_yuv_index;
	int ta_yuv_x,ta_yuv_y;
	int ta_yuv_x_size,ta_yuv_y_size;
	uint8_t yuv_fifo[384];
	uint32_t ta_ol_pointers_1[OL_POINTERS_COUNT];
	uint32_t ta_ol_pointers_2[OL_POINTERS_COUNT];

	// Other registers
	uint32_t fog_table[0x80];
	uint32_t palette[0x400];

	// PD DMA registers
	uint32_t sb_pdstap, sb_pdstar, sb_pdlen, sb_pddir, sb_pdtsel, sb_pden, sb_pdst, sb_pdapro;

	// TA input fifo
	int ta_fifo_pos;
	uint32_t ta_fifo_buf[16];

	// TA lists and packetization
	int ta_packet_len, ta_packet_type, ta_cmd_type, ta_list_idx, ta_list_type;

	// TA command parsing
	uint32_t ta_cmd_header, ta_cmd_instr, ta_cmd_tsp[2], ta_cmd_tex[2];
	float ta_cmd_color_base[2][4], ta_cmd_color_offset[2][4];
	uint32_t ta_cmd_color_sprite_base, ta_cmd_color_sprite_offset;
	int ta_cmd_strip_length, ta_cmd_user_clip_mode;

	// TA user clipping
	uint32_t ta_clip_min_x, ta_clip_max_x, ta_clip_min_y, ta_clip_max_y;

	// TA vertices
	struct ta_vertex {
		uint32_t header;
		uint32_t coords[3];
		uint32_t color_base[2];
		uint32_t color_offset[2];
		uint32_t uv[2][2];
		uint32_t uvc[2];

		int tx() const { float p = u2f(coords[0]); if(p<0) return -1; return int(p)>>5; }
		int ty() const { float p = u2f(coords[1]); if(p<0) return -1; return int(p)>>5; }
	};

	struct ta_bbox {
		int min_x, max_x, min_y, max_y;
	};

	ta_vertex ta_vertex_current, ta_vertex_strip[8];
	int ta_vertex_count;
	bool ta_vertex_odd_tri;
	bool ta_vertex_shadow_first;
	ta_bbox ta_vertex_shadow_bbox;

	// TA output
	uint32_t ta_olist_pos, ta_olist_pos_base, ta_olist_tile, ta_olist_block_size, ta_olist_line_size;

	// Texture path widths
	bool texture_path_width[2];

	// ZBuffers
	struct zbuffer {
		uint32_t tag[32][32];
		float depth[32][32];
		uint32_t valid[32];

		void clear_valid();
		void set_depth_tag(float _depth, uint32_t _tag);
	};

	// ISP poly information
	struct isp_poly_t {
		float edge_ab_a, edge_ab_b, edge_ab_c;
		float edge_bc_a, edge_bc_b, edge_bc_c;
		float edge_ca_a, edge_ca_b, edge_ca_c;
		float edge_cd_a, edge_cd_b, edge_cd_c;
		float edge_da_a, edge_da_b, edge_da_c;
		float face_a,    face_b,    face_c;
		int   y_start,   y_end;
	};

	static uint32_t (*const blend_functions[64])(uint32_t s, uint32_t d);

	static int uv_wrap(float uv, int size);
	static int uv_flip(float uv, int size);
	static int uv_clamp(float uv, int size);

	inline float clampf(uint32_t v) {
		float v1 = u2f(v);
		return v1 < 0 ? 0 : v1 > 1 ? 1 : v1;
	}

	inline float clampf(float v) {
		return v < 0 ? 0 : v > 1 ? 1 : v;
	}

	inline void ta_load_color_f32(float *dest, const uint32_t *src)
	{
		for(int i=0; i<4; i++)
			dest[i] = clampf(src[i]);
	}

	inline uint32_t ta_color_f32_to_u8(const uint32_t *src)
	{
		uint32_t res = 0;
		for(int i=0; i<4; i++)
			res |= uint8_t(255*clampf(src[i])) << (24-8*i);
		return res;
	}

	inline uint32_t ta_intensity(const float *src, uint32_t i)
	{
		uint32_t ii = uint32_t(i*256) & 0x1ff;
		uint32_t res = uint8_t(255*clampf(src[0])) << 24;

		for(int i=1; i<4; i++)
			res |= uint8_t((ii*uint8_t(255*clampf(src[i]))) >> 8) << (24-8*i);
		return res;
	}

	static inline int32_t clamp(int32_t in, int32_t min, int32_t max);
	static inline uint32_t bilinear_filter(uint32_t c0, uint32_t c1, uint32_t c2, uint32_t c3, float u, float v);
	static inline uint32_t bla(uint32_t c, uint32_t a);
	static inline uint32_t blia(uint32_t c, uint32_t a);
	static inline uint32_t blc(uint32_t c1, uint32_t c2);
	static inline uint32_t blic(uint32_t c1, uint32_t c2);
	static inline uint32_t bls(uint32_t c1, uint32_t c2);
	static uint32_t bl00(uint32_t s, uint32_t d);
	static uint32_t bl01(uint32_t s, uint32_t d);
	static uint32_t bl02(uint32_t s, uint32_t d);
	static uint32_t bl03(uint32_t s, uint32_t d);
	static uint32_t bl04(uint32_t s, uint32_t d);
	static uint32_t bl05(uint32_t s, uint32_t d);
	static uint32_t bl06(uint32_t s, uint32_t d);
	static uint32_t bl07(uint32_t s, uint32_t d);
	static uint32_t bl10(uint32_t s, uint32_t d);
	static uint32_t bl11(uint32_t s, uint32_t d);
	static uint32_t bl12(uint32_t s, uint32_t d);
	static uint32_t bl13(uint32_t s, uint32_t d);
	static uint32_t bl14(uint32_t s, uint32_t d);
	static uint32_t bl15(uint32_t s, uint32_t d);
	static uint32_t bl16(uint32_t s, uint32_t d);
	static uint32_t bl17(uint32_t s, uint32_t d);
	static uint32_t bl20(uint32_t s, uint32_t d);
	static uint32_t bl21(uint32_t s, uint32_t d);
	static uint32_t bl22(uint32_t s, uint32_t d);
	static uint32_t bl23(uint32_t s, uint32_t d);
	static uint32_t bl24(uint32_t s, uint32_t d);
	static uint32_t bl25(uint32_t s, uint32_t d);
	static uint32_t bl26(uint32_t s, uint32_t d);
	static uint32_t bl27(uint32_t s, uint32_t d);
	static uint32_t bl30(uint32_t s, uint32_t d);
	static uint32_t bl31(uint32_t s, uint32_t d);
	static uint32_t bl32(uint32_t s, uint32_t d);
	static uint32_t bl33(uint32_t s, uint32_t d);
	static uint32_t bl34(uint32_t s, uint32_t d);
	static uint32_t bl35(uint32_t s, uint32_t d);
	static uint32_t bl36(uint32_t s, uint32_t d);
	static uint32_t bl37(uint32_t s, uint32_t d);
	static uint32_t bl40(uint32_t s, uint32_t d);
	static uint32_t bl41(uint32_t s, uint32_t d);
	static uint32_t bl42(uint32_t s, uint32_t d);
	static uint32_t bl43(uint32_t s, uint32_t d);
	static uint32_t bl44(uint32_t s, uint32_t d);
	static uint32_t bl45(uint32_t s, uint32_t d);
	static uint32_t bl46(uint32_t s, uint32_t d);
	static uint32_t bl47(uint32_t s, uint32_t d);
	static uint32_t bl50(uint32_t s, uint32_t d);
	static uint32_t bl51(uint32_t s, uint32_t d);
	static uint32_t bl52(uint32_t s, uint32_t d);
	static uint32_t bl53(uint32_t s, uint32_t d);
	static uint32_t bl54(uint32_t s, uint32_t d);
	static uint32_t bl55(uint32_t s, uint32_t d);
	static uint32_t bl56(uint32_t s, uint32_t d);
	static uint32_t bl57(uint32_t s, uint32_t d);
	static uint32_t bl60(uint32_t s, uint32_t d);
	static uint32_t bl61(uint32_t s, uint32_t d);
	static uint32_t bl62(uint32_t s, uint32_t d);
	static uint32_t bl63(uint32_t s, uint32_t d);
	static uint32_t bl64(uint32_t s, uint32_t d);
	static uint32_t bl65(uint32_t s, uint32_t d);
	static uint32_t bl66(uint32_t s, uint32_t d);
	static uint32_t bl67(uint32_t s, uint32_t d);
	static uint32_t bl70(uint32_t s, uint32_t d);
	static uint32_t bl71(uint32_t s, uint32_t d);
	static uint32_t bl72(uint32_t s, uint32_t d);
	static uint32_t bl73(uint32_t s, uint32_t d);
	static uint32_t bl74(uint32_t s, uint32_t d);
	static uint32_t bl75(uint32_t s, uint32_t d);
	static uint32_t bl76(uint32_t s, uint32_t d);
	static uint32_t bl77(uint32_t s, uint32_t d);
	static inline uint32_t cv_1555(uint16_t c);
	static inline uint32_t cv_1555z(uint16_t c);
	static inline uint32_t cv_565(uint16_t c);
	static inline uint32_t cv_565z(uint16_t c);
	static inline uint32_t cv_4444(uint16_t c);
	static inline uint32_t cv_4444z(uint16_t c);
	static inline uint32_t cv_yuv(uint16_t c1, uint16_t c2, int x);
	uint32_t tex_r_yuv_n(texinfo *t, float x, float y);
	uint32_t tex_r_yuv_tw(texinfo *t, float x, float y);
//  uint32_t tex_r_yuv_vq(texinfo *t, float x, float y);
	uint32_t tex_r_1555_n(texinfo *t, float x, float y);
	uint32_t tex_r_1555_tw(texinfo *t, float x, float y);
	uint32_t tex_r_1555_vq(texinfo *t, float x, float y);
	uint32_t tex_r_565_n(texinfo *t, float x, float y);
	uint32_t tex_r_565_tw(texinfo *t, float x, float y);
	uint32_t tex_r_565_vq(texinfo *t, float x, float y);
	uint32_t tex_r_4444_n(texinfo *t, float x, float y);
	uint32_t tex_r_4444_tw(texinfo *t, float x, float y);
	uint32_t tex_r_4444_vq(texinfo *t, float x, float y);
	uint32_t tex_r_p4_1555_tw(texinfo *t, float x, float y);
	uint32_t tex_r_p4_1555_vq(texinfo *t, float x, float y);
	uint32_t tex_r_p4_565_tw(texinfo *t, float x, float y);
	uint32_t tex_r_p4_565_vq(texinfo *t, float x, float y);
	uint32_t tex_r_p4_4444_tw(texinfo *t, float x, float y);
	uint32_t tex_r_p4_4444_vq(texinfo *t, float x, float y);
	uint32_t tex_r_p4_8888_tw(texinfo *t, float x, float y);
	uint32_t tex_r_p4_8888_vq(texinfo *t, float x, float y);
	uint32_t tex_r_p8_1555_tw(texinfo *t, float x, float y);
	uint32_t tex_r_p8_1555_vq(texinfo *t, float x, float y);
	uint32_t tex_r_p8_565_tw(texinfo *t, float x, float y);
	uint32_t tex_r_p8_565_vq(texinfo *t, float x, float y);
	uint32_t tex_r_p8_4444_tw(texinfo *t, float x, float y);
	uint32_t tex_r_p8_4444_vq(texinfo *t, float x, float y);
	uint32_t tex_r_p8_8888_tw(texinfo *t, float x, float y);
	uint32_t tex_r_p8_8888_vq(texinfo *t, float x, float y);

	uint32_t tex_r_nt_palint(texinfo *t, float x, float y);
	uint32_t tex_r_nt_palfloat(texinfo *t, float x, float y);

	uint32_t tex_r_default(texinfo *t, float x, float y);

	void tex_get_info(texinfo *t);

	void render_hline(bitmap_rgb32 &bitmap, texinfo *ti, int y, float xl, float xr, float ul, float ur, float vl, float vr, float wl, float wr);
	void render_span(bitmap_rgb32 &bitmap, texinfo *ti,
						float y0, float y1,
						float xl, float xr,
						float ul, float ur,
						float vl, float vr,
						float wl, float wr,
						float dxldy, float dxrdy,
						float duldy, float durdy,
						float dvldy, float dvrdy,
						float dwldy, float dwrdy);
	void sort_vertices(const vert *v, int *i0, int *i1, int *i2);
	void render_tri_sorted(bitmap_rgb32 &bitmap, texinfo *ti, const vert *v0, const vert *v1, const vert *v2);
	void render_tri(bitmap_rgb32 &bitmap, texinfo *ti, const vert *v);
	void render_to_accumulation_buffer(bitmap_rgb32 &bitmap, const rectangle &cliprect);
	void pvr_accumulationbuffer_to_framebuffer(address_space &space, int x, int y);
	void pvr_drawframebuffer(bitmap_rgb32 &bitmap,const rectangle &cliprect);
	static uint32_t dilate0(uint32_t value,int bits);
	static uint32_t dilate1(uint32_t value,int bits);
	void computedilated();
	void pvr_build_parameterconfig();
	void process_ta_fifo();
	void update_screen_format();

	void fb_convert_0555krgb_to_555rgb(address_space &space, int x, int y);
	void fb_convert_0555krgb_to_565rgb(address_space &space, int x, int y);
	void fb_convert_0555krgb_to_888rgb24(address_space &space, int x, int y);
	void fb_convert_0555krgb_to_888rgb32(address_space &space, int x, int y);

	void fb_convert_0565rgb_to_555rgb(address_space &space, int x, int y);
	void fb_convert_0565rgb_to_565rgb(address_space &space, int x, int y);
	void fb_convert_0565rgb_to_888rgb24(address_space &space, int x, int y);
	void fb_convert_0565rgb_to_888rgb32(address_space &space, int x, int y);

	void fb_convert_1555argb_to_555rgb(address_space &space, int x, int y);
	void fb_convert_1555argb_to_565rgb(address_space &space, int x, int y);
	void fb_convert_1555argb_to_888rgb24(address_space &space, int x, int y);
	void fb_convert_1555argb_to_888rgb32(address_space &space, int x, int y);

	void fb_convert_888rgb_to_555rgb(address_space &space, int x, int y);
	void fb_convert_888rgb_to_565rgb(address_space &space, int x, int y);
	void fb_convert_888rgb_to_888rgb24(address_space &space, int x, int y);
	void fb_convert_888rgb_to_888rgb32(address_space &space, int x, int y);

	void fb_convert_8888argb_to_555rgb(address_space &space, int x, int y);
	void fb_convert_8888argb_to_565rgb(address_space &space, int x, int y);
	void fb_convert_8888argb_to_888rgb24(address_space &space, int x, int y);
	void fb_convert_8888argb_to_888rgb32(address_space &space, int x, int y);

	void mem_write32(uint32_t adr, uint32_t val);

	void ta_object_list_extend();
	void ta_add_object(uint32_t adr, bool tail);
	void ta_add_strip(uint32_t adr, uint32_t psize);
	bool ta_bbox_vertex(ta_bbox &bb, const ta_vertex *vtx, int count) const;
	void ta_bbox_merge(ta_bbox &bb, const ta_bbox &src) const;
	uint32_t ta_strip_write(bool single_tile, uint32_t &psize);
	void ta_vertex_write_sprite();
	void ta_vertex_write_shadow();
	void ta_vertex_write();
	void ta_vertex_push();
	void ta_handle_command_draw();
	void ta_handle_vertex();
	void ta_handle_command_end_of_list();
	void ta_handle_command_user_tile_clip();
	void ta_handle_command_object_list_set();

	int ta_get_packet_type();
	void ta_next_list();
	void ta_list_start();
	void ta_process_lists(uint32_t adr, bool cont);

	void texture_path_w(address_space &space, int path, offs_t offset, uint64_t data, uint64_t mem_mask);
	bool list_has_objects(uint32_t adr) const;
	void setup_object(setup_poly_t &poly);
	void setup_poly(setup_poly_t &poly);
	void isp_run(uint32_t x, uint32_t y, bool zclear, bool presort, bool flushacc);
	void isp_load_poly(const isp_buffer_t *p, isp_poly_t &poly) const;
	void isp_coverage_poly(uint32_t *coverage, const isp_buffer_t *p, isp_poly_t &poly) const;
};

DECLARE_DEVICE_TYPE(POWERVR2, powervr2_device)

#endif // MAME_VIDEO_POWERVR2_H
