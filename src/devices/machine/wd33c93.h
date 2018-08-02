// license:BSD-3-Clause
// copyright-holders:ElSemi, R. Belmont, Ryan Holtz
/*
 * wd33c93.h
 *
 */

#ifndef MAME_MACHINE_WD33C93_H
#define MAME_MACHINE_WD33C93_H

#pragma once

#include "machine/nscsi_bus.h"

class wd33c93_device : public nscsi_device
{
public:
	// construction/destruction
	wd33c93_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock = 10000000);

	auto irq_cb() { return m_irq_cb.bind(); }
	auto drq_cb() { return m_drq_cb.bind(); }

	void map(address_map &map);

	DECLARE_READ8_MEMBER (adr_r);
	DECLARE_WRITE8_MEMBER(adr_w);
	DECLARE_READ8_MEMBER (data_r);
	DECLARE_WRITE8_MEMBER(data_w);

	uint8_t dma_r();
	void dma_w(uint8_t val);

protected:
	// device-level overrides
	virtual void device_start() override;
	virtual void device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr) override;

	virtual void scsi_ctrl_changed() override;

private:
	enum
	{
		/* WD register names */
		WD_OWN_ID                   = 0x00,
		WD_CONTROL                  = 0x01,
		WD_TIMEOUT_PERIOD           = 0x02,
		WD_CDB_1                    = 0x03,
		WD_CDB_2                    = 0x04,
		WD_CDB_3                    = 0x05,
		WD_CDB_4                    = 0x06,
		WD_CDB_5                    = 0x07,
		WD_CDB_6                    = 0x08,
		WD_CDB_7                    = 0x09,
		WD_CDB_8                    = 0x0a,
		WD_CDB_9                    = 0x0b,
		WD_CDB_10                   = 0x0c,
		WD_CDB_11                   = 0x0d,
		WD_CDB_12                   = 0x0e,
		WD_TARGET_LUN               = 0x0f,
		WD_COMMAND_PHASE            = 0x10,
		WD_SYNCHRONOUS_TRANSFER     = 0x11,
		WD_TRANSFER_COUNT_MSB       = 0x12,
		WD_TRANSFER_COUNT           = 0x13,
		WD_TRANSFER_COUNT_LSB       = 0x14,
		WD_DESTINATION_ID           = 0x15,
		WD_SOURCE_ID                = 0x16,
		WD_SCSI_STATUS              = 0x17,
		WD_COMMAND                  = 0x18,
		WD_DATA                     = 0x19,
		WD_QUEUE_TAG                = 0x1a,
		WD_AUXILIARY_STATUS         = 0x1f,

		/* WD commands */
		WD_CMD_RESET                = 0x00,
		WD_CMD_ABORT                = 0x01,
		WD_CMD_ASSERT_ATN           = 0x02,
		WD_CMD_NEGATE_ACK           = 0x03,
		WD_CMD_DISCONNECT           = 0x04,
		WD_CMD_RESELECT             = 0x05,
		WD_CMD_SEL_ATN              = 0x06,
		WD_CMD_SEL                  = 0x07,
		WD_CMD_SEL_ATN_XFER         = 0x08,
		WD_CMD_SEL_XFER             = 0x09,
		WD_CMD_RESEL_RECEIVE        = 0x0a,
		WD_CMD_RESEL_SEND           = 0x0b,
		WD_CMD_WAIT_SEL_RECEIVE     = 0x0c,
		WD_CMD_SSCC                 = 0x0d,
		WD_CMD_SND_DISC             = 0x0e,
		WD_CMD_SET_IDI              = 0x0f,
		WD_CMD_RCV_CMD              = 0x10,
		WD_CMD_RCV_DATA             = 0x11,
		WD_CMD_RCV_MSG_OUT          = 0x12,
		WD_CMD_RCV                  = 0x13,
		WD_CMD_SND_STATUS           = 0x14,
		WD_CMD_SND_DATA             = 0x15,
		WD_CMD_SND_MSG_IN           = 0x16,
		WD_CMD_SND                  = 0x17,
		WD_CMD_TRANS_ADDR           = 0x18,
		WD_CMD_XFER_PAD             = 0x19,
		WD_CMD_TRANS_INFO           = 0x20,
		WD_CMD_TRANSFER_PAD         = 0x21,
		WD_CMD_SBT_MODE             = 0x80,

		/* ASR register */
		ASR_INT                     = 0x80,
		ASR_LCI                     = 0x40,
		ASR_BSY                     = 0x20,
		ASR_CIP                     = 0x10,
		ASR_PE                      = 0x02,
		ASR_DBR                     = 0x01,

		/* SCSI Bus Phases */
		PHS_DATA_OUT                = 0x00,
		PHS_DATA_IN                 = 0x01,
		PHS_COMMAND                 = 0x02,
		PHS_STATUS                  = 0x03,
		PHS_MESS_OUT                = 0x06,
		PHS_MESS_IN                 = 0x07,


		/* Command Status Register definitions */

		/* reset state interrupts */
		CSR_RESET                   = 0x00,
		CSR_RESET_AF                = 0x01,

		/* successful completion interrupts */
		CSR_RESELECT                = 0x10,
		CSR_SELECT                  = 0x11,
		CSR_SEL_XFER_DONE           = 0x16,
		CSR_XFER_DONE               = 0x18,

		/* paused or aborted interrupts */
		CSR_MSGIN                   = 0x20,
		CSR_SDP                     = 0x21,
		CSR_SEL_ABORT               = 0x22,
		CSR_RESEL_ABORT             = 0x25,
		CSR_RESEL_ABORT_AM          = 0x27,
		CSR_ABORT                   = 0x28,

		/* terminated interrupts */
		CSR_INVALID                 = 0x40,
		CSR_UNEXP_DISC              = 0x41,
		CSR_TIMEOUT                 = 0x42,
		CSR_PARITY                  = 0x43,
		CSR_PARITY_ATN              = 0x44,
		CSR_BAD_STATUS              = 0x45,
		CSR_UNEXP                   = 0x48,

		/* service required interrupts */
		CSR_RESEL                   = 0x80,
		CSR_RESEL_AM                = 0x81,
		CSR_DISC                    = 0x85,
		CSR_SRV_REQ                 = 0x88,

		/* Own ID/CDB Size register */
		OWNID_EAF                   = 0x08,
		OWNID_EHP                   = 0x10,
		OWNID_RAF                   = 0x20,
		OWNID_FS_8                  = 0x00,
		OWNID_FS_12                 = 0x40,
		OWNID_FS_16                 = 0x80,

		/* Control register */
		CTRL_HSP                    = 0x01,
		CTRL_HA                     = 0x02,
		CTRL_IDI                    = 0x04,
		CTRL_EDI                    = 0x08,
		CTRL_HHP                    = 0x10,
		CTRL_POLLED                 = 0x00,
		CTRL_BURST                  = 0x20,
		CTRL_BUS                    = 0x40,
		CTRL_DMA                    = 0x80,

		/* Synchronous Transfer Register */
		STR_FSS                     = 0x80,

		/* Destination ID register */
		DSTID_DPD                   = 0x40,
		DATA_OUT_DIR             =    0,
		DATA_IN_DIR              =    1,
		DSTID_SCC                   = 0x80,

		/* Source ID register */
		SRCID_MASK                  = 0x07,
		SRCID_SIV                   = 0x08,
		SRCID_DSP                   = 0x20,
		SRCID_ES                    = 0x40,
		SRCID_ER                    = 0x80,
	};

	devcb_write_line m_irq_cb;
	devcb_write_line m_drq_cb;
#if 0
	u8           sasr;
	u8           regs[WD_AUXILIARY_STATUS+1];
	u8           fifo[FIFO_SIZE];
	int          fifo_pos;
	u8           temp_input[TEMP_INPUT_LEN];
	int          temp_input_pos;
	u8           busphase;
	u8           identify;
	int          read_pending;
	emu_timer   *cmd_timer;
	emu_timer   *service_req_timer;
	emu_timer   *deassert_cip_timer;
#endif
};

// device type definition
DECLARE_DEVICE_TYPE(WD33C93, wd33c93_device)

#endif // MAME_MACHINE_WD33C93_H
