// license:BSD-3-Clause
// copyright-holders:Aaron Giles
/***************************************************************************

    clifront.h

    Command-line interface frontend for MAME.

***************************************************************************/

#pragma once

#ifndef __CLIFRONT_H__
#define __CLIFRONT_H__

#include "emu.h"
#include "cliopts.h"

// don't include osd_interface in header files
class osd_interface;

//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************


// cli_frontend handles command-line processing and emulator execution
class cli_frontend
{
public:
	// construction/destruction
	cli_frontend(cli_options &options, osd_interface &osd);
	~cli_frontend();

	// execute based on the incoming argc/argv
	int execute(int argc, char **argv);

	// direct access to the command operations
	void listxml(std::string gamename = "*");
	void listfull(std::string gamename = "*");
	void listsource(std::string gamename = "*");
	void listclones(std::string gamename = "*");
	void listbrothers(std::string gamename = "*");
	void listcrc(std::string gamename = "*");
	void listroms(std::string gamename = "*");
	void listsamples(std::string gamename = "*");
	static int compare_devices(const void *i1, const void *i2);
	void listdevices(std::string gamename = "*");
	void listslots(std::string gamename = "*");
	void listmedia(std::string gamename = "*");
	void listsoftware(std::string gamename = "*");
	void verifysoftware(std::string gamename = "*");
	void verifyroms(std::string gamename = "*");
	void verifysamples(std::string gamename = "*");
	void romident(std::string filename);
	void getsoftlist(std::string gamename = "*");
	void verifysoftlist(std::string gamename = "*");

private:
	// internal helpers
	void execute_commands(std::string exename);
	void display_help();
	void display_suggestions(std::string gamename);
	void output_single_softlist(FILE *out, software_list_device &swlist);

	// internal state
	cli_options &       m_options;
	osd_interface &     m_osd;
	int                 m_result;
};




#endif  /* __CLIFRONT_H__ */
