/*************************************************************************************

Submarine (c) 1985 Sigma

driver by David Haywood & Angelo Salese

TODO:
- finish spriteram hook-up;
- colors;
- complete inputs;
- status bar should go above the sprites;
- a bunch of unemulated writes at 0xe***

======================================================================================

 2 PCBs

 PCB1 - Bottom? (video?) board

|------------------------------------------------------------------- --|
|           B                                                          |
|     A           C     D     E     F     -     H     -    J           |
|                                                                      |
|                                                                      |
|1                                             (rom)                   |
|                                               OBJ1             T     |
|2                           Sony                                e    --
|                            CX23001           (rom)             x    --
|3                                              OBJ2             t    --
|                                                                     --C
|4                           (prom)            (rom)                  --N
|                              E4               OBJ3                  --3
|5                           (prom)                                   --
|                              E5                                     --
|6                           Sony                                     --
|                            CX23001                                  --
|7              (prom)                                                 |
|                 C7                                                   |
|8              (prom)                                                 |
|                 C8                                                   |
|9  (prom)                                                            --
|    A9    R                                                          --
|10 (prom) e                                                          --
|    A10   s                         HM6116P-3                        --C
|11 (prom)                                                            --N
|    A11                             (rom)                            --4
|12                                  VRAM1                            --
|                                                                     --
|13                                  (rom)                            --
|                                    VRAM2                            --
|14                                                                    |
|                                    (rom)                             |
|15                                  VRAM3                             |
|                                                                      |
|                                                                      |
|----------------------------------------------------------------------|

 Text = sigma enterprises, inc.
                 JAPAN F-021BCRT
         (rotated 90 degress left)

  Res = a bunch of resistors (colour weighting?)


PCB2  (Top board, CPU board)

|----------------------------------------------------------------------|
|                                                                      |
|         A       B       C       D      E       F       G      H      |
--|                                                        16000  18.432
  |                                                        OSC1    OSC2|
  |1           - - - - - - - - - - -                                   |
--|           |                                                    T   |
--|2                 NECD780        |                              e  --
--|           |      (z80 CPU)                                     x  --
--|  3                              |                              t  --
--|            - - - - - - - - - - -                                  --C
--|  4                                                                --N
--|                                                                   --3
--|C 5          T         T        T                                  --
  |N            E(rom)    E(rom)   E(rom)                             --
  |2 6          M         M        M                                  --
--|             P         P        P                                  --
|    7          1         2        3                                   |
--|C                                                                   |
  |N 8                                                                 |
  |1                                                                   |
--|  9                                                                --
--|                                                                   --
--| 10                                                                --
--|                                                                   --C
--| 11                                                                --N
--|                                                                   --4
--| 12                             DSW1 DSW2                          --
--|                                                                   --
--| 13                                                                --
  |                                                                   --
  | 14                                                         L       |
--|                                AY8910     R(rom)           H(z80A) |
|          15                                 O                0       |
|                                  AY8910     M                0       |
|          16                                 M                8       |
|                                                              0A      |
|----------------------------------------------------------------------|

 Text = F-020   CPU1 JAPAN   sigma enterprises, inc.

*************************************************************************************/

#include "driver.h"
#include "cpu/z80/z80.h"
#include "sound/ay8910.h"


static UINT8* sub_vid;
static UINT8* sub_attr;
static UINT8* sub_scrolly;

static ADDRESS_MAP_START( subm_map, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0xafff) AM_ROM
	AM_RANGE(0xb000, 0xbfff) AM_RAM
	AM_RANGE(0xc000, 0xc3ff) AM_RAM AM_BASE(&sub_attr)
	AM_RANGE(0xc400, 0xc7ff) AM_RAM AM_BASE(&sub_vid)
	AM_RANGE(0xd000, 0xd03f) AM_RAM AM_BASE(&spriteram)
	AM_RANGE(0xd800, 0xd83f) AM_RAM AM_BASE(&spriteram_2)
	AM_RANGE(0xd840, 0xd85f) AM_RAM AM_BASE(&sub_scrolly)

	AM_RANGE(0xe000, 0xe000) AM_NOP
	AM_RANGE(0xe800, 0xe800) AM_NOP
	AM_RANGE(0xe801, 0xe801) AM_NOP
	AM_RANGE(0xe802, 0xe802) AM_NOP
	AM_RANGE(0xe803, 0xe803) AM_NOP
	AM_RANGE(0xe805, 0xe805) AM_NOP

	AM_RANGE(0xf000, 0xf000) AM_READ_PORT("DSW0") // DSW0?
	AM_RANGE(0xf020, 0xf020) AM_READ_PORT("DSW1") // DSW1?
	AM_RANGE(0xf040, 0xf040) AM_READ_PORT("SYSTEM")
	AM_RANGE(0xf060, 0xf060) AM_READ_PORT("IN0")
ADDRESS_MAP_END

static WRITE8_HANDLER( subm_to_sound_w )
{
	soundlatch_w(space, 0, data & 0xff);
	cputag_set_input_line(space->machine, "soundcpu", 0, HOLD_LINE);
}

static UINT8 nmi_en;

static WRITE8_HANDLER( nmi_mask_w )
{
	nmi_en = data & 1;
}

static ADDRESS_MAP_START( subm_io, ADDRESS_SPACE_IO, 8 )
	ADDRESS_MAP_GLOBAL_MASK(0xff)
	AM_RANGE(0x00, 0x00) AM_READWRITE(soundlatch2_r, subm_to_sound_w) // to/from sound CPU
ADDRESS_MAP_END

static ADDRESS_MAP_START( subm_sound_map, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x3fff) AM_ROM
	AM_RANGE(0x4000, 0x47ff) AM_RAM
	AM_RANGE(0x6000, 0x6000) AM_WRITE(nmi_mask_w)
ADDRESS_MAP_END

static ADDRESS_MAP_START( subm_sound_io, ADDRESS_SPACE_IO, 8 )
	ADDRESS_MAP_GLOBAL_MASK(0xff)
	AM_RANGE(0x00, 0x00) AM_READWRITE(soundlatch_r, soundlatch2_w) // to/from main CPU
	AM_RANGE(0x40, 0x41) AM_DEVREADWRITE("ay1", ay8910_r, ay8910_address_data_w)
	AM_RANGE(0x80, 0x81) AM_DEVREADWRITE("ay2", ay8910_r, ay8910_address_data_w)
ADDRESS_MAP_END


static INPUT_PORTS_START( sub )
	PORT_START("SYSTEM")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_BUTTON1 )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_BUTTON2 )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_START("IN0")
	PORT_DIPNAME( 0x01, 0x01, "DSWC" )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_START2 )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_START1 )

	PORT_START("DSW0")
	PORT_DIPNAME( 0x01, 0x01, "DSWC" )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_START("DSW1")
	PORT_DIPNAME( 0x01, 0x01, "DSWC" )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
INPUT_PORTS_END



static const gfx_layout tiles8x8_layout =
{
	8,8,
	RGN_FRAC(1,3),
	3,
	{ RGN_FRAC(0,3), RGN_FRAC(1,3), RGN_FRAC(2,3) },
	{ 0, 1, 2, 3, 4, 5, 6, 7 },
	{ 7*8, 6*8, 5*8, 4*8, 3*8, 2*8, 1*8, 0*8 },
	8*8
};

static const gfx_layout tiles16x32_layout = {
    16,32,
    RGN_FRAC(1,3),
    3,
    { RGN_FRAC(0,3), RGN_FRAC(1,3), RGN_FRAC(2,3) },
    { 64+0, 64+1, 64+2, 64+3, 64+4, 64+5, 64+6, 64+7, 0, 1, 2, 3, 4, 5, 6, 7 },
    { 55*8, 54*8, 53*8, 52*8, 51*8, 50*8, 49*8, 48*8, 39*8,38*8,37*8,36*8,35*8,34*8,33*8,32*8,23*8,22*8,21*8,20*8,19*8,18*8,17*8,16*8,7*8,
6*8, 5*8, 4*8, 3*8, 2*8, 1*8, 0*8 },
    64*8
};

static GFXDECODE_START( sub )
	GFXDECODE_ENTRY( "gfx1", 0, tiles8x8_layout, 0, 0x40 )
	GFXDECODE_ENTRY( "gfx2", 0, tiles16x32_layout, 0, 0x40 )
GFXDECODE_END



VIDEO_START(sub)
{
}

VIDEO_UPDATE(sub)
{
	const gfx_element *gfx = screen->machine->gfx[0];
	int y,x;
	int count = 0;

	for (y=0;y<32;y++)
	{
		for (x=0;x<32;x++)
		{
			UINT16 tile = sub_vid[count];
			UINT8 col;
			UINT8 y_offs = sub_scrolly[x];

			tile += (sub_attr[count]&0xe0)<<3;
			col = (sub_attr[count]&0x0f);

			count++;

			drawgfx_opaque(bitmap,cliprect,gfx,tile,col,0,0,x*8,(y*8)-y_offs);
			drawgfx_opaque(bitmap,cliprect,gfx,tile,col,0,0,x*8,(y*8)-y_offs+256);
		}
	}

	{
		int x,y,spr_offs,i;

		for(i=0;i<0x40;i+=2)
		{
			spr_offs = spriteram[i+1];
			//x = sub_sprx[i];
			y = 0xf0 - spriteram_2[i+1];
			x = spriteram[i+0];

			drawgfx_transpen(bitmap,cliprect,screen->machine->gfx[1],spr_offs,0,0,0,x,y,0);
		}
	}

	return 0;
}

static PALETTE_INIT(sub)
{
	/* might be wrong */
	int i;

	for (i = 0; i < 0x400; i++)
	{
		UINT8 data;
		UINT8*proms = memory_region(machine, "proms2");
		size_t length = memory_region_length(machine, "proms2");

		data = proms[length/2 + i] | (proms[length/2 + i] << 4);

		palette_set_color_rgb(machine, i,  pal2bit(data >> 6), pal3bit(data >> 3),  pal3bit(data >> 0));
	}

}

static INTERRUPT_GEN( subm_sound_irq )
{
	if(nmi_en)
		cputag_set_input_line(device->machine, "soundcpu", INPUT_LINE_NMI, PULSE_LINE);
}

static MACHINE_DRIVER_START( sub )
	/* basic machine hardware */
	MDRV_CPU_ADD("maincpu", Z80,4000000)		 /* ? MHz */
	MDRV_CPU_PROGRAM_MAP(subm_map)
	MDRV_CPU_IO_MAP(subm_io)
	MDRV_CPU_VBLANK_INT("screen", irq0_line_hold)

	MDRV_CPU_ADD("soundcpu", Z80,4000000)		 /* ? MHz */
	MDRV_CPU_PROGRAM_MAP(subm_sound_map)
	MDRV_CPU_IO_MAP(subm_sound_io)
	MDRV_CPU_PERIODIC_INT(subm_sound_irq, 60) //???


	/* video hardware */
	MDRV_SCREEN_ADD("screen", RASTER)
	MDRV_SCREEN_REFRESH_RATE(60)
	MDRV_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(0))
	MDRV_SCREEN_FORMAT(BITMAP_FORMAT_INDEXED16)
	MDRV_SCREEN_SIZE(256, 256)
	MDRV_SCREEN_VISIBLE_AREA(0, 256-1, 16, 256-16-1)

	MDRV_GFXDECODE(sub)
	MDRV_PALETTE_LENGTH(0x400)
	MDRV_PALETTE_INIT(sub)

	MDRV_VIDEO_START(sub)
	MDRV_VIDEO_UPDATE(sub)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")

	MDRV_SOUND_ADD("ay1", AY8910, 12000000/8) /* ? Mhz */
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.23)

	MDRV_SOUND_ADD("ay2", AY8910, 12000000/8) /* ? Mhz */
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.23)
MACHINE_DRIVER_END


ROM_START( sub )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "temp 1 pos b6 27128.bin",	  0x0000, 0x4000, CRC(6875b31d) SHA1(e7607e53687f1331cc97de939de144a7954ca3c3) )
	ROM_LOAD( "temp 2 pos c6 27128.bin",	  0x4000, 0x4000, CRC(bc7f8f43) SHA1(088156a66acb2214c638d9d1ad18e9836b27eff0) )
	ROM_LOAD( "temp 3 pos d6 2764.bin",		  0x8000, 0x2000, CRC(3546c226) SHA1(35e53c0db75c89e8e222d2139b841e77f5cc282c) )

	ROM_REGION( 0x10000, "soundcpu", 0 )
	ROM_LOAD( "m sound pos f14 2764.bin",	  0x0000, 0x2000, CRC(61536a97) SHA1(84effc2251bf7c91e0bb670a651117503de8940d) )
	ROM_RELOAD( 							  0x2000, 0x2000 )

	ROM_REGION( 0xc000, "gfx1", 0)
	ROM_LOAD( "vram 1 pos f12 27128  version3.bin",	  0x0000, 0x4000, CRC(8d176ba0) SHA1(b0bf4af97e991545d6b38e8159eb909376e6df35) )
	ROM_LOAD( "vram 2 pos f14 27128  version3.bin",	  0x4000, 0x4000, CRC(0677cf3a) SHA1(072e9391f6a230b78124e820da0f0d27ffa45dc3) )
	ROM_LOAD( "vram 3 pos f15 27128  version3.bin",	  0x8000, 0x4000, CRC(9a4cd1a0) SHA1(a321b88386424d73d7d73a7f321317b0f21d2eb6) )

	ROM_REGION( 0xc000, "gfx2", 0 )
	ROM_LOAD( "obj 1 pos h1 27128  version3.bin",	  0x0000, 0x4000, CRC(63173e65) SHA1(2be3776c0e08d2c876cfce842e02345389e1fba0) )
	ROM_LOAD( "obj 2 pos h3 27128  version3.bin",	  0x4000, 0x4000, CRC(3898d1a8) SHA1(acd3d7695a0fe9faa5e4315032c65e131d24a3ce) )
	ROM_LOAD( "obj 3 pos h4 27128  version3.bin",	  0x8000, 0x4000, CRC(304e2145) SHA1(d4eb49b5502872718d64e53f02acd2150f6bf713) )

	ROM_REGION( 0x300, "proms", 0 ) // what are these?
	ROM_LOAD( "prom pos a9 n82s129",	  0x0000, 0x100, CRC(8df9cefe) SHA1(86320eb8135932d79c4478929b9fd90ffba55712) )
	ROM_LOAD( "prom pos a10 n82s129",	  0x0100, 0x100, CRC(3c834094) SHA1(4d681431376a8ed071566d74d4accc737bf965dd) )
	ROM_LOAD( "prom pos a11 n82s129",	  0x0200, 0x100, CRC(339afa95) SHA1(ff4ff712960f41c26419a681e8dcceaeef75d2e3) )

	ROM_REGION( 0x800, "proms2", 0 )
	ROM_LOAD( "prom pos e5 n82s131",	  0x0000, 0x200, CRC(0024b5dd) SHA1(7d623f8e8964336d643820850cef0fb641e52e22) )
	ROM_LOAD( "prom pos c7 n82s129",	  0x0200, 0x100, CRC(9072d259) SHA1(9679fa01372d14a866836c9193204ff6e33cf67c) )
	ROM_LOAD( "prom pos e4 n82s131",	  0x0400, 0x200, CRC(307aa2cf) SHA1(839eccf1d34adaf9a5006bfb30e3524bc19a9b41) )
	ROM_LOAD( "prom pos c8 n82s129",	  0x0600, 0x100, CRC(351e1ef8) SHA1(530c9012ff5abda1c4ba9787ca999ca1ae1a893d) )
ROM_END

GAME( 1985, sub,  0,    sub, sub,  0, ROT270, "Sigma", "Submarine", GAME_NOT_WORKING )
