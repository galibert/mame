/*****************************************************************************
 *
 *   Portable Xerox AltoII RAM related functions
 *
 *   Copyright: Juergen Buchmueller <pullmoll@t-online.de>
 *
 *   Licenses: MAME, GPLv2
 *
 *****************************************************************************/
#include "alto2cpu.h"

/**
 * @brief bs_read_sreg early: drive bus by S register or M (MYL), if rsel is = 0
 *
 * Note: RSEL == 0 can't be read, because it is decoded as
 * access to the M register (MYL latch access, LREF' in the schematics)
 */
void alto2_cpu_device::bs_early_read_sreg()
{
	UINT16 r;

	if (m_d_rsel) {
		UINT8 bank = m_s_reg_bank[m_task];
		r = m_s[bank][m_d_rsel];
		LOG((LOG_RAM,2,"	←S%02o; bus &= S[%o][%02o] (%#o)\n", reg, bank, reg, r));
	} else {
		r = m_m;
		LOG((LOG_RAM,2,"	←S%02o; bus &= M (%#o)\n", reg, r));
	}
	m_bus &= r;
}

/**
 * @brief bs_load_sreg early: load S register puts garbage on the bus
 */
void alto2_cpu_device::bs_early_load_sreg()
{
	int r = 0;	/* ??? */
	LOG((LOG_RAM,2,"	S%02o← BUS &= garbage (%#o)\n", MIR_RSEL(m_mir), r));
	m_bus &= r;
}

/**
 * @brief bs_load_sreg late: load S register from M
 */
void alto2_cpu_device::bs_late_load_sreg()
{
	UINT8 bank = m_s_reg_bank[m_task];
	m_s[bank][m_d_rsel] = m_m;
	LOG((LOG_RAM,2,"	S%02o← S[%o][%02o] := %#o\n", reg, bank, reg, m_m));
}

/**
 * @brief branch to ROM page
 */
void alto2_cpu_device::branch_ROM(const char *from, int page)
{
	(void)from;
	m_next2 = (m_next2 & ALTO2_UCODE_PAGE_MASK) + page * ALTO2_UCODE_PAGE_SIZE;
	LOG((LOG_RAM,2,"	SWMODE: branch from %s to ROM%d (%#o)\n", from, page, m_next2));
}

/**
 * @brief branch to RAM page
 */
void alto2_cpu_device::branch_RAM(const char *from, int page)
{
	(void)from;
	m_next2 = (m_next2 & ALTO2_UCODE_PAGE_MASK) + ALTO2_UCODE_RAM_BASE + page * ALTO2_UCODE_PAGE_SIZE;
	LOG((LOG_RAM,2,"	SWMODE: branch from %s to RAM%d\n", from, page, m_next2));
}

/**
 * @brief f1_swmode early: switch to micro program counter BUS[6-15] in other bank
 *
 * Note: Jumping to uninitialized CRAM
 *
 * When jumping to uninitialized RAM, which, because of the inverted bits of the
 * microcode words F1(0), F2(0) and LOADL, it is then read as F1=010 (SWMODE),
 * F2=010 (BUSODD) and LOADL=1, loading the M register (MYL latch), too.
 * This causes control to go back to the Emulator task at 0, because the
 * NEXT[0-9] of uninitialized RAM is 0.
 *
 */
void alto2_cpu_device::f1_late_swmode()
{
	/* currently executing in what page? */
	UINT16 current = m_mpc / ALTO2_UCODE_PAGE_SIZE;

#if	(ALTO2_UCODE_ROM_PAGES == 1 && ALTO2_UCODE_RAM_PAGES == 1)
	switch (current) {
	case 0:
		branch_RAM("ROM0", 0);
		break;
	case 1:
		branch_ROM("RAM0", 0);
		break;
	default:
		fatal(1, "Impossible current mpc %d\n", current);
	}
#endif
#if (ALTO2_UCODE_ROM_PAGES == 2 && ALTO2_UCODE_RAM_PAGES == 1)
	UINT16 next = X_RDBITS(m_next2,10,1,1);

	switch (current) {
	case 0: /* ROM0 to RAM0 or ROM1 */
		switch (next) {
		case 0:
			branch_RAM("ROM0", 0);
			break;
		case 1:
			branch_ROM("ROM0", 1);
			break;
		default:
			fatal(1, "Impossible next %d\n", next);
		}
		break;
	case 1: /* ROM1 to ROM0 or RAM0 */
		switch (next) {
		case 0:
			branch_ROM("ROM1", 0);
			break;
		case 1:
			branch_RAM("ROM1", 0);
			break;
		default:
			fatal(1, "Impossible next %d\n", next);
		}
		break;
	case 2: /* RAM0 to ROM0 or ROM1 */
		switch (next) {
		case 0:
			branch_ROM("RAM0", 0);
			break;
		case 1:
			branch_ROM("RAM0", 1);
			break;
		default:
			fatal(1, "Impossible next %d\n", next);
		}
		break;
	default:
		fatal(1, "Impossible current mpc %d\n", current);
	}
#endif
#if	(ALTO2_UCODE_ROM_PAGES == 1 && ALTO2_UCODE_RAM_PAGES == 3)
	UINT16 next = X_RDBITS(m_next2,10,1,2);

	switch (current) {
	case 0: /* ROM0 to RAM0, RAM2, RAM1, RAM0 */
		switch (next) {
		case 0:
			branch_RAM("ROM0", 0);
			break;
		case 1:
			branch_RAM("ROM0", 2);
			break;
		case 2:
			branch_RAM("ROM0", 1);
			break;
		case 3:
			branch_RAM("ROM0", 0);
			break;
		default:
			fatal(1, "Impossible next %d\n", next);
		}
		break;
	case 1: /* RAM0 to ROM0, RAM2, RAM1, RAM1 */
		switch (next) {
		case 0:
			branch_ROM("RAM0", 0);
			break;
		case 1:
			branch_RAM("RAM0", 2);
			break;
		case 2:
			branch_RAM("RAM0", 1);
			break;
		case 3:
			branch_RAM("RAM0", 1);
			break;
		default:
			fatal(1, "Impossible next %d\n", next);
		}
		break;
	case 2: /* RAM1 to ROM0, RAM2, RAM0, RAM0 */
		switch (next) {
		case 0:
			branch_ROM("RAM1", 0);
			break;
		case 1:
			branch_RAM("RAM1", 2);
			break;
		case 2:
			branch_RAM("RAM1", 0);
			break;
		case 3:
			branch_RAM("RAM1", 0);
			break;
		default:
			fatal(1, "Impossible next %d\n", next);
		}
		break;
	case 3: /* RAM2 to ROM0, RAM1, RAM0, RAM0 */
		switch (next) {
		case 0:
			branch_ROM("RAM2", 0);
			break;
		case 1:
			branch_RAM("RAM2", 1);
			break;
		case 2:
			branch_RAM("RAM2", 0);
			break;
		case 3:
			branch_RAM("RAM2", 0);
			break;
		default:
			fatal(1, "Impossible next %d\n", next);
		}
		break;
	default:
		fatal(1, "Impossible current mpc %d\n", current);
	}
#else
	fatal(1, "Impossible control ROM/RAM combination %d/%d\n", ALTO2_UCODE_ROM_PAGES, ALTO2_UCODE_RAM_PAGES);
#endif
}

/**
 * @brief f1_wrtram late: start WRTRAM cycle
 */
void alto2_cpu_device::f1_late_wrtram()
{
	m_wrtram_flag = true;
	LOG((LOG_RAM,2,"	WRTRAM\n"));
}

/**
 * @brief f1_rdram late: start RDRAM cycle
 */
void alto2_cpu_device::f1_late_rdram()
{
	m_rdram_flag = true;
	LOG((LOG_RAM,2,"	RDRAM\n"));
}

#if	(ALTO2_UCODE_RAM_PAGES == 3)

/**
 * @brief f1_load_rmr late: load the reset mode register
 *
 * F1=013 corresponds to RMR← in the emulator. In Altos with the 3K
 * RAM option, F1=013 performs RMR← in all RAM-related tasks, including
 * the emulator.
 */
void alto2_cpu_device::f1_late_load_rmr()
{
	LOG((LOG_RAM,2,"	RMR←; BUS (%#o)\n", m_bus));
	m_reset_mode = m_bus;
}
#else	// ALTO2_UCODE_RAM_PAGES != 3
/**
 * @brief f1_load_srb late: load the S register bank from BUS[12-14]
 */
void alto2_cpu_device::f1_late_load_srb()
{
	m_s_reg_bank[m_task] = X_RDBITS(m_bus,16,12,14) % ALTO2_SREG_BANKS;
	LOG((LOG_RAM,2,"	SRB←; srb[%d] := %#o\n", m_task, m_s_reg_bank[m_task]));
}
#endif

/**
 * @brief RAM related task slots initialization
 */
void alto2_cpu_device::init_ram(int task)
{
	m_ram_related[task] = true;

	set_bs(task, bs_ram_read_slocation,	&alto2_cpu_device::bs_early_read_sreg, 0);
	set_bs(task, bs_ram_load_slocation,	&alto2_cpu_device::bs_early_load_sreg, &alto2_cpu_device::bs_late_load_sreg);

	set_f1(task, f1_ram_swmode,			0, &alto2_cpu_device::f1_late_swmode);
	set_f1(task, f1_ram_wrtram,			0, &alto2_cpu_device::f1_late_wrtram);
	set_f1(task, f1_ram_rdram,			0, &alto2_cpu_device::f1_late_rdram);
#if	(ALTO2_UCODE_RAM_PAGES == 3)
	set_f1(task, f1_ram_load_rmr,		0, &alto2_cpu_device::f1_load_rmr_1);
#else	// ALTO2_UCODE_RAM_PAGES != 3
	set_f1(task, f1_ram_load_srb,		0, &alto2_cpu_device::f1_late_load_srb);
#endif
}

void alto2_cpu_device::exit_ram()
{
	// nothing to do yet
}

