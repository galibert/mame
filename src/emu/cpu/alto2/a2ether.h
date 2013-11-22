/*****************************************************************************
 *
 *   Xerox AltoII ethernet task (ETHER)
 *
 *   Copyright © Jürgen Buchmüller <pullmoll@t-online.de>
 *
 *   Licenses: MAME, GPLv2
 *
 *****************************************************************************/
#ifdef  ALTO2_DEFINE_CONSTANTS

#define ALTO2_ETHER_FIFO_SIZE	16              //!< number of words in the ethernet FIFO
#define ALTO2_ETHER_PACKET_SIZE 0400            //!< size of a packet in words

#else   // ALTO2_DEFINE_CONSTANTS
#ifndef _A2ETHER_H_
#define _A2ETHER_H_
//! BUS source for ethernet task
enum {
	bs_ether_eidfct		= bs_task_3				//!< ethernet task: Ethernet input data function
};

//! F1 functions for ethernet task
enum {
	f1_ether_eilfct		= f1_task_13,			//!< f1 (1011): ethernet input look function
	f1_ether_epfct		= f1_task_14,			//!< f1 (1100): ethernet post function
	f1_ether_ewfct		= f1_task_15			//!< f1 (1101): ethernet countdown wakeup function
};

//! F2 functions for ethernet task
enum {
	f2_ether_eodfct		= f2_task_10,			//!< f2 (1000): ethernet output data function
	f2_ether_eosfct		= f2_task_11,			//!< f2 (1001): ethernet output start function
	f2_ether_erbfct		= f2_task_12,			//!< f2 (1010): ethernet reset branch function
	f2_ether_eefct		= f2_task_13,			//!< f2 (1011): ethernet end of transmission function
	f2_ether_ebfct		= f2_task_14,			//!< f2 (1100): ethernet branch function
	f2_ether_ecbfct		= f2_task_15,			//!< f2 (1101): ethernet countdown branch function
	f2_ether_eisfct		= f2_task_16			//!< f2 (1110): ethernet input start function
												//!< f2 (1111): undefined
};

UINT8* m_ether_a41;								//!< BPROM; P3601-1; 256x4; enet.a41 "PE1"
UINT8* m_ether_a42;								//!< BPROM; P3601-1; 256x4; enet.a42 "PE2"
UINT8* m_ether_a49;								//!< BPROM; P3601-1; 265x4 enet.a49 "AFIFO"
enum {
	ether_a49_BE	= (1 << 0),					//!< buffer empty
	ether_a49_BNE	= (1 << 1),					//!< buffer next empty
	ether_a49_BNNE	= (1 << 2),					//!< buffer next next empty
	ether_a49_BF	= (1 << 3)					//!< buffer full
};

static const int m_duckbreath_sec = 15;			//!< send duckbreath every 15 seconds

struct {
	UINT16 fifo[ALTO2_ETHER_FIFO_SIZE];			//!< FIFO buffer
	UINT16 fifo_rd;								//!< FIFO input pointer
	UINT16 fifo_wr;								//!< FIFO output pointer
	UINT16 status;								//!< status word
	UINT32 rx_crc;								//!< receiver CRC
	UINT32 tx_crc;								//!< transmitter CRC
	UINT32 rx_count;							//!< received words count
	UINT32 tx_count;							//!< transmitted words count
	UINT16* rx_packet;							//!< buffer to collect received words
	UINT16* tx_packet;							//!< buffer to collect transmitted words
	emu_timer* rx_timer;						//!< receiver timer
	emu_timer* tx_timer;						//!< transmitter timer
	int duckbreath;								//!< if non-zero, interval in seconds at which to broadcast the duckbreath
}	m_eth;

TIMER_CALLBACK_MEMBER( rx_duckbreath );			//!< HACK: pull the next word from the duckbreath in the fifo
TIMER_CALLBACK_MEMBER( tx_packet );				//!< transmit data from the FIFO to <nirvana for now>
void eth_wakeup();								//!< check for the various reasons to wakeup the Ethernet task
void eth_startf();								//!< start input or output depending on m_bus
void bs_early_eidfct();							//!< bus source: Ethernet input data function
void f1_early_eth_block();						//!< F1 func: block the Ether task
void f1_early_eilfct();							//!< F1 func: Ethernet input look function
void f1_early_epfct();							//!< F1 func: Ethernet post function
void f1_late_ewfct();							//!< F1 func: Ethernet countdown wakeup function
void f2_late_eodfct();							//!< F2 func: Ethernet output data function
void f2_late_eosfct();							//!< F2 func: Ethernet output start function
void f2_late_erbfct();							//!< F2 func: Ethernet reset branch function
void f2_late_eefct();							//!< F2 func: Ethernet end of transmission function
void f2_late_ebfct();							//!< F2 func: Ethernet branch function
void f2_late_ecbfct();							//!< F2 func: Ethernet countdown branch function
void f2_late_eisfct();							//!< F2 func: Ethernet input start function
void activate_eth();							//!< called by the CPU when the Ethernet task becomes active
void init_ether(int task = task_ether);			//!< initialize the ethernet task
void exit_ether();								//!< deinitialize the ethernet task
void reset_ether();								//!< reset the ethernet task
#endif // _A2ETHER_H_
#endif	// ALTO2_DEFINE_CONSTANTS
