#include "emu.h"
#include "cpu/i8085/i8085.h"

class dandelion_state : public driver_device
{
public:
	dandelion_state(const machine_config &mconfig, device_type type, const char *tag) :
		driver_device(mconfig, type, tag),
		iop(*this, "io_cpu")
	{}

	DECLARE_READ8_MEMBER (myst_85_r);
	DECLARE_WRITE8_MEMBER(myst_85_w);
	DECLARE_READ8_MEMBER (myst_86_r);
	DECLARE_WRITE8_MEMBER(myst_86_w);
	DECLARE_READ8_MEMBER (myst_87_r);
	DECLARE_WRITE8_MEMBER(myst_87_w);
	DECLARE_READ8_MEMBER (dma_status_r);
	DECLARE_WRITE8_MEMBER(dma_mode_w);
	DECLARE_WRITE8_MEMBER(rs366_reg_w);
	DECLARE_READ8_MEMBER (dma_test_reg_r);
	DECLARE_WRITE8_MEMBER(dma_test_reg_w);
	DECLARE_WRITE8_MEMBER(fdc_state_w);
	DECLARE_WRITE8_MEMBER(misc_clocks_1_w);
	DECLARE_READ8_MEMBER (kbdata_r);
	DECLARE_WRITE8_MEMBER(todclr_w);
	DECLARE_READ8_MEMBER (cp_in_r);
	DECLARE_WRITE8_MEMBER(cp_out_w);
	DECLARE_READ8_MEMBER (cp_status_r);
	DECLARE_WRITE8_MEMBER(cp_control_w);
	DECLARE_READ8_MEMBER (mousex_r);
	DECLARE_WRITE8_MEMBER(mouseclr_w);
	DECLARE_READ8_MEMBER (mousey_r);
	DECLARE_WRITE8_MEMBER(cp_dma_clr_w);
	DECLARE_READ8_MEMBER (misc_input_1_r);
	DECLARE_WRITE8_MEMBER(misc_control_1_w);
	DECLARE_READ8_MEMBER (cs_r);
	DECLARE_WRITE8_MEMBER(cs_w);
	DECLARE_READ8_MEMBER (tpc_high_r);
	DECLARE_WRITE8_MEMBER(tpc_high_w);
	DECLARE_READ8_MEMBER (tpc_low_r);
	DECLARE_WRITE8_MEMBER(tpc_low_w);

	void dandelion(machine_config &config);

protected:
	required_device<i8085a_cpu_device> iop;
	uint16_t maintenance_panel_counter;
	uint16_t tpc[8];
	uint8_t cs[4096][6];
	uint8_t dma_test_reg, myst_85, myst_86, myst_87;
	uint8_t tpc_high, tpc_low;
	uint8_t misc_control_1, misc_clocks_1;

	/*
	  cs read:
	  0: prA.0-3, prB.0-3
	  1: paS.0-2, paF.0-2, paD.0-1
	  2: pEP, pCIN-SE.wrSU, pEnableSU, pmem, pfS.0-3
	  3: pfY.0-3, INIA.0-3
	  4: pfX.0-3, INIA.4-7
	  5: pfZ.0-3, INIA.8-11
	  6: TC.0-3, TPC.0-3
	  7: TPC.4-11

	  cs write:
	  *: CSin.0-7
	  6: Nt.0-2,NIAX.0-4
	  7: 0,NIAX.5-11

	  TPC.0-11[Nt.0-2] = NIAX.0-11 when writeTPC'
	*/

	void dlion_iop_mem(address_map &map);
	void dlion_iop_io(address_map &map);
};


READ8_MEMBER(dandelion_state::myst_85_r)
{
	if(iop->pc() != 0x188e)
		logerror("myst_85_r (%04x)\n", iop->pc());
	return myst_85;
}

WRITE8_MEMBER(dandelion_state::myst_85_w)
{
	myst_85 = data;
	if(iop->pc() != 0x188b)
		logerror("myst_85_w %02x (%04x)\n", data, iop->pc());
}

READ8_MEMBER(dandelion_state::myst_86_r)
{
	if(iop->pc() != 0x189c)
		logerror("myst_86_r (%04x)\n", iop->pc());
	return myst_86;
}

WRITE8_MEMBER(dandelion_state::myst_86_w)
{
	myst_86 = data;
	if(iop->pc() != 0x1899)
		logerror("myst_86_w %02x (%04x)\n", data, iop->pc());
}

READ8_MEMBER(dandelion_state::myst_87_r)
{
	if(iop->pc() != 0x18aa)
		logerror("myst_87_r (%04x)\n", iop->pc());
	return myst_87;
}

WRITE8_MEMBER(dandelion_state::myst_87_w)
{
	myst_87 = data;
	if(iop->pc() != 0x18a7)
		logerror("myst_87_w %02x (%04x)\n", data, iop->pc());
}

READ8_MEMBER(dandelion_state::dma_status_r)
{
	logerror("dma_status_r (%04x)\n", iop->pc());
	return 0x00;
}

WRITE8_MEMBER(dandelion_state::dma_mode_w)
{
	logerror("dma_mode_w %02x (%04x)\n", data, iop->pc());
}

WRITE8_MEMBER(dandelion_state::rs366_reg_w)
{
	logerror("rs366_reg_w %02x (%04x)\n", data, iop->pc());
}

READ8_MEMBER(dandelion_state::dma_test_reg_r)
{
	return dma_test_reg;
}

WRITE8_MEMBER(dandelion_state::dma_test_reg_w)
{
	dma_test_reg = data;
}

WRITE8_MEMBER(dandelion_state::fdc_state_w)
{
	logerror("fdc_state_w %02x (%04x)\n", data, iop->pc());
}

WRITE8_MEMBER(dandelion_state::misc_clocks_1_w)
{
	uint8_t delta = data ^ misc_clocks_1;
	misc_clocks_1 = data;
	if((delta & 0x20) && !(misc_clocks_1 & 0x20))
		maintenance_panel_counter++;
	if((delta & 0x40) && !(misc_clocks_1 & 0x40))
		maintenance_panel_counter = 0;

	if(delta & (0xff^0x60))
		logerror("misc_clocks_1_w %02x (%02x) (%04x)\n", data, data^0xff, iop->pc());
}

READ8_MEMBER(dandelion_state::kbdata_r)
{
	logerror("kbdata_r (%04x)\n", iop->pc());
	return 0x00;
}

WRITE8_MEMBER(dandelion_state::todclr_w)
{
	logerror("cp_control_w %02x (%04x)\n", data, iop->pc());
}

READ8_MEMBER(dandelion_state::cp_in_r)
{
	logerror("cp_in_r (%04x)\n", iop->pc());
	return 0x00;
}

WRITE8_MEMBER(dandelion_state::cp_out_w)
{
	logerror("cp_out_w %02x (%04x)\n", data, iop->pc());
}

READ8_MEMBER(dandelion_state::cp_status_r)
{
	logerror("cp_status_r (%04x)\n", iop->pc());
	return 0x00;
}

WRITE8_MEMBER(dandelion_state::cp_control_w)
{
	logerror("cp_control_w %02x (%04x)\n", data, iop->pc());
}

READ8_MEMBER(dandelion_state::mousex_r)
{
	logerror("mousey_r (%04x)\n", iop->pc());
	return 0x00;
}

WRITE8_MEMBER(dandelion_state::mouseclr_w)
{
	logerror("cp_control_w %02x (%04x)\n", data, iop->pc());
}

READ8_MEMBER(dandelion_state::mousey_r)
{
	logerror("mousey_r (%04x)\n", iop->pc());
	return 0x00;
}

WRITE8_MEMBER(dandelion_state::cp_dma_clr_w)
{
	logerror("cp_dma_clr_w %02x (%04x)\n", data, iop->pc());
}

READ8_MEMBER(dandelion_state::misc_input_1_r)
{
	logerror("misc_input_1_r (%04x)\n", iop->pc());
	return 0x7f;
}

WRITE8_MEMBER(dandelion_state::misc_control_1_w)
{
	uint8_t delta = data ^ misc_control_1;
	misc_control_1 = data;
	if((delta & 0x08) && !(misc_control_1 & 0x08))
		logerror("maintenance panel: %04d\n", maintenance_panel_counter);
	if(delta & (0xff^0x08))
		logerror("misc_control_1_w %02x (%04x)\n", data, iop->pc());
}

READ8_MEMBER(dandelion_state::cs_r)
{
	logerror("cs_r %d.%03x.%d %02x (%04x)\n", tpc_high >> 5, tpc[tpc_high >> 5], offset, cs[tpc[tpc_high >> 5]][offset], iop->pc());
	return ~cs[tpc[tpc_high >> 5]][offset];
}

WRITE8_MEMBER(dandelion_state::cs_w)
{
	logerror("cs_w %d.%03x.%d, %02x (%04x)\n", tpc_high >> 5, tpc[tpc_high >> 5], offset, data, iop->pc());
	cs[tpc[tpc_high >> 5]][offset] = data;
}

READ8_MEMBER(dandelion_state::tpc_high_r)
{
	logerror("tpc_high_r %d, %03x (%04x)\n", tpc_high >> 5, tpc[tpc_high >> 5], iop->pc());
	// Missing TC
	return tpc[tpc_high >> 5] >> 8;
}

WRITE8_MEMBER(dandelion_state::tpc_high_w)
{
	tpc_high = data;
	//	logerror("tpc_high_w %02x (%04x)\n", data, iop->pc());
}

READ8_MEMBER(dandelion_state::tpc_low_r)
{
	logerror("tpc_low_r (%04x)\n", iop->pc());
	return tpc[tpc_high >> 5];
}

WRITE8_MEMBER(dandelion_state::tpc_low_w)
{
	tpc_low = data;
	tpc[tpc_high >> 5] = ((tpc_high & 0x1f) << 7) | (tpc_low & 0x7f);
	//	logerror("tpc_low_w %02x (%04x)\n", data, iop->pc());
}

void dandelion_state::dlion_iop_mem(address_map &map)
{
	map(0x0000, 0x1fff).rom();
	map(0x2000, 0x5fff).ram();
	map(0x80f8, 0x80fd).rw(this, FUNC(dandelion_state::cs_r), FUNC(dandelion_state::cs_w));
	map(0x80fe, 0x80fe).rw(this, FUNC(dandelion_state::tpc_high_r), FUNC(dandelion_state::tpc_high_w));
	map(0x80ff, 0x80ff).rw(this, FUNC(dandelion_state::tpc_low_r), FUNC(dandelion_state::tpc_low_w));
}

void dandelion_state::dlion_iop_io(address_map &map)
{
	map(0x85, 0x85).rw(this, FUNC(dandelion_state::myst_85_r), FUNC(dandelion_state::myst_85_w));
	map(0x86, 0x86).rw(this, FUNC(dandelion_state::myst_86_r), FUNC(dandelion_state::myst_86_w));
	map(0x87, 0x87).rw(this, FUNC(dandelion_state::myst_87_r), FUNC(dandelion_state::myst_87_w));
	map(0xa8, 0xa8).rw(this, FUNC(dandelion_state::dma_status_r), FUNC(dandelion_state::dma_mode_w));
	map(0x9c, 0x9c).w (this, FUNC(dandelion_state::rs366_reg_w));
	map(0xd0, 0xd0).rw(this, FUNC(dandelion_state::dma_test_reg_r), FUNC(dandelion_state::dma_test_reg_w));
	map(0xe8, 0xe8).w (this, FUNC(dandelion_state::fdc_state_w));
	map(0xe9, 0xe9).w (this, FUNC(dandelion_state::misc_clocks_1_w));
	map(0xea, 0xea).rw(this, FUNC(dandelion_state::kbdata_r), FUNC(dandelion_state::todclr_w));
	map(0xeb, 0xeb).rw(this, FUNC(dandelion_state::cp_in_r), FUNC(dandelion_state::cp_out_w));
	map(0xec, 0xec).rw(this, FUNC(dandelion_state::cp_status_r), FUNC(dandelion_state::cp_control_w));
	map(0xed, 0xed).rw(this, FUNC(dandelion_state::mousex_r), FUNC(dandelion_state::mouseclr_w));
	map(0xee, 0xee).rw(this, FUNC(dandelion_state::mousey_r), FUNC(dandelion_state::cp_dma_clr_w));
	map(0xef, 0xef).rw(this, FUNC(dandelion_state::misc_input_1_r), FUNC(dandelion_state::misc_control_1_w));
	map(0xf8, 0xfd).rw(this, FUNC(dandelion_state::cs_r), FUNC(dandelion_state::cs_w));
	map(0xfe, 0xfe).rw(this, FUNC(dandelion_state::tpc_high_r), FUNC(dandelion_state::tpc_high_w));
	map(0xff, 0xff).rw(this, FUNC(dandelion_state::tpc_low_r), FUNC(dandelion_state::tpc_low_w));
}

static INPUT_PORTS_START(dandelion)
INPUT_PORTS_END

MACHINE_CONFIG_START(dandelion_state::dandelion)
	MCFG_CPU_ADD("io_cpu", I8085A, 1000000)
	MCFG_CPU_PROGRAM_MAP(dlion_iop_mem)
	MCFG_CPU_IO_MAP(dlion_iop_io)
MACHINE_CONFIG_END

ROM_START(dlion)
	ROM_REGION(0x2000, "io_cpu", 0)
	ROM_DEFAULT_BIOS("24")
	ROM_SYSTEM_BIOS( 0, "24", "IOP firmware version 2.4" )
	ROMX_LOAD ("537p02563.bin", 0x0000, 0x0800, CRC(1661a418) SHA1(6b9f0eb14fd97593cfe5253f288d0abd9e8db26a), ROM_BIOS(1))
	ROMX_LOAD ("537p02564.bin", 0x0800, 0x0800, CRC(97374b60) SHA1(0d712ebc053d9374426a6e61fd8bc8f90a617196), ROM_BIOS(1))
	ROMX_LOAD ("537p02565.bin", 0x1000, 0x0800, CRC(3f55d17f) SHA1(e6333e53570fb05a841a7f141872c8bd14143f9c), ROM_BIOS(1))
	ROMX_LOAD ("537p02278.bin", 0x1800, 0x0800, CRC(ea201974) SHA1(e89a9421a9b55ba18ba5fd45f03cb826245e0f7e), ROM_BIOS(1))

	ROM_SYSTEM_BIOS( 1, "30", "IOP firmware version 3.0 (incomplete dump)" )
	ROMX_LOAD ("537p03029.bin", 0x0000, 0x0800, CRC(5ec5c762) SHA1(4e0dde739d1f2a1afee6f8bb1ce8742f0585e91b), ROM_BIOS(2))
	ROMX_LOAD ("537p03030.bin", 0x0800, 0x0800, CRC(c91137f1) SHA1(35aa8d01028ed122acc868e8d23a51dc3e773fb3), ROM_BIOS(2))
	ROMX_LOAD ("537p03031.bin", 0x1000, 0x0800, NO_DUMP, ROM_BIOS(2))
	ROMX_LOAD ("537p03032.bin", 0x1800, 0x0800, CRC(293b6aa5) SHA1(e1c6b675902795390be41c6bd1e2c4ff81f5e465), ROM_BIOS(2))

	ROM_SYSTEM_BIOS( 2, "31", "IOP firmware version 3.1" )
	ROMX_LOAD ("537p03029.bin", 0x0000, 0x0800, CRC(5ec5c762) SHA1(4e0dde739d1f2a1afee6f8bb1ce8742f0585e91b), ROM_BIOS(3))
	ROMX_LOAD ("537p03030.bin", 0x0800, 0x0800, CRC(c91137f1) SHA1(35aa8d01028ed122acc868e8d23a51dc3e773fb3), ROM_BIOS(3))
	ROMX_LOAD ("537p03700.bin", 0x1000, 0x0800, CRC(50b21fcd) SHA1(923d0dbb4482e55c6573b3cb226d210dedeb68a0), ROM_BIOS(3))
	ROMX_LOAD ("537p03032.bin", 0x1800, 0x0800, CRC(293b6aa5) SHA1(e1c6b675902795390be41c6bd1e2c4ff81f5e465), ROM_BIOS(3))
ROM_END


// 8090 Daylight disk server

ROM_START(dlight)
	ROM_REGION(0x2000, "io_cpu", 0)
	ROM_DEFAULT_BIOS("85581")
	ROM_SYSTEM_BIOS( 0, "85331", "IOP firmware version 85331 - SCSI?" )
	ROMX_LOAD ("85331.8090.bin", 0, 0x2000, CRC(e8b6be2c) SHA1(f3f56e77b0ec1d377dd1c2835db4746c013e95f1), ROM_BIOS(1))
	ROM_SYSTEM_BIOS( 1, "85581", "IOP firmware version 85581" )
	ROMX_LOAD ("85581.8090.bin", 0, 0x2000, CRC(0a99b611) SHA1(0689ffe1b962016f71b43273776307e6acbf4373), ROM_BIOS(2))
ROM_END


// 1109 Dandetiger, upgraded Dandelion
ROM_START(dtiger)
	ROM_REGION(0x2000, "io_cpu", 0)
	ROM_LOAD ("1109_80mb_129.bin", 0x0000, 0x0800, CRC(5ec5c762) SHA1(4e0dde739d1f2a1afee6f8bb1ce8742f0585e91b))
	ROM_LOAD ("1109_80mb_130.bin", 0x0800, 0x0800, CRC(d534694c) SHA1(52f95ce333b39b58e4abacc4cf544d5d9b8148fa))
	ROM_LOAD ("1109_80mb_131.bin", 0x1000, 0x0800, CRC(1932b163) SHA1(cdcb925404f29bb9ad31b5f28cd4a10d9f0ae23e))
	ROM_LOAD ("1109_80mb_132.bin", 0x1800, 0x0800, CRC(293b6aa5) SHA1(e1c6b675902795390be41c6bd1e2c4ff81f5e465))
ROM_END

COMP( ????, dlion,  0, 0, dandelion, dandelion, dandelion_state, 0, "Xerox", "Xerox 1108/8010 Dandelion", MACHINE_NOT_WORKING|MACHINE_NO_SOUND)
COMP( ????, dlight, 0, 0, dandelion, dandelion, dandelion_state, 0, "Xerox", "Xerox 8090 Daylight Disk Server", MACHINE_NOT_WORKING|MACHINE_NO_SOUND)
COMP( ????, dtiger, 0, 0, dandelion, dandelion, dandelion_state, 0, "Xerox", "Xerox 1109 Dandetiger", MACHINE_NOT_WORKING|MACHINE_NO_SOUND)

