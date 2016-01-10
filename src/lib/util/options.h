// license:BSD-3-Clause
// copyright-holders:Aaron Giles
/***************************************************************************

    options.h

    Core options code code

***************************************************************************/

#ifndef __OPTIONS_H__
#define __OPTIONS_H__

#include "corefile.h"
#include <unordered_map>



//**************************************************************************
//  CONSTANTS
//**************************************************************************

// option types
const UINT32 OPTION_TYPE_MASK       = 0x0007;       // up to 8 different types
enum
{
	OPTION_INVALID,         // invalid
	OPTION_HEADER,          // a header item
	OPTION_COMMAND,         // a command
	OPTION_BOOLEAN,         // boolean option
	OPTION_INTEGER,         // integer option
	OPTION_FLOAT,           // floating-point option
	OPTION_STRING           // string option
};

// option priorities
const int OPTION_PRIORITY_DEFAULT   = 0;            // defaults are at 0 priority
const int OPTION_PRIORITY_LOW       = 50;           // low priority
const int OPTION_PRIORITY_NORMAL    = 100;          // normal priority
const int OPTION_PRIORITY_HIGH      = 150;          // high priority
const int OPTION_PRIORITY_MAXIMUM   = 255;          // maximum priority

const UINT32 OPTION_FLAG_INTERNAL = 0x40000000;


//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// static structure describing a single option with its description and default value
struct options_entry
{
	const char *        name;               // name on the command line
	const char *        defvalue;           // default value of this argument
	UINT32              flags;              // flags to describe the option
	const char *        description;        // description for -showusage
};


// structure holding information about a collection of options
class core_options
{
	static const int MAX_UNADORNED_OPTIONS = 16;

public:
	// information about a single entry in the options
	class entry
	{
		friend class core_options;
		friend class simple_list<entry>;

		// construction/destruction
		entry(std::string name, std::string description, UINT32 flags = 0, std::string defvalue = nullptr);

	public:
		// getters
		entry *next() const { return m_next; }
		std::string name(int index = 0) const { return (index < ARRAY_LENGTH(m_name) && !m_name[index].empty()) ? m_name[index] : ""; }
		std::string description() const { return m_description; }
		std::string value() const { return m_data; }
		std::string default_value() const { return m_defdata; }
		std::string minimum() const { return m_minimum; }
		std::string maximum() const { return m_maximum; }
		UINT32 seqid() const { return m_seqid; }
		int type() const { return (m_flags & OPTION_TYPE_MASK); }
		UINT32 flags() const { return m_flags; }
		bool is_header() const { return type() == OPTION_HEADER; }
		bool is_command() const { return type() == OPTION_COMMAND; }
		bool is_internal() const { return m_flags & OPTION_FLAG_INTERNAL; }
		bool has_range() const { return (!m_minimum.empty() && !m_maximum.empty()); }
		int priority() const { return m_priority; }

		// setters
		void set_value(std::string newvalue, int priority);
		void set_default_value(std::string defvalue);
		void set_description(std::string description);
		void set_flag(UINT32 mask, UINT32 flag);
		void revert(int priority);

	private:
		// internal state
		entry *                 m_next;             // link to the next data
		UINT32                  m_flags;            // flags from the entry
		UINT32                  m_seqid;            // sequence ID; bumped on each change
		bool                    m_error_reported;   // have we reported an error on this option yet?
		int                     m_priority;         // priority of the data set
		std::string             m_description;      // description for this item
		std::string             m_name[4];          // up to 4 names for the item
		std::string             m_data;             // data for this item
		std::string             m_defdata;          // default data for this item
		std::string             m_minimum;          // minimum value
		std::string             m_maximum;          // maximum value
	};

	// construction/destruction
	core_options();
	core_options(const options_entry *entrylist);
	core_options(const options_entry *entrylist1, const options_entry *entrylist2);
	core_options(const options_entry *entrylist1, const options_entry *entrylist2, const options_entry *entrylist3);
	core_options(const core_options &src);
	virtual ~core_options();

	// operators
	core_options &operator=(const core_options &rhs);
	bool operator==(const core_options &rhs);
	bool operator!=(const core_options &rhs);

	// getters
	entry *first() const { return m_entrylist.first(); }
	std::string command() const { return m_command; }

	// configuration
	void add_entry(std::string name, std::string description, UINT32 flags = 0, std::string defvalue = nullptr, bool override_existing = false);
	void add_entry(const options_entry &data, bool override_existing = false) { add_entry(data.name, data.description, data.flags, data.defvalue, override_existing); }
	void add_entries(const options_entry *entrylist, bool override_existing = false);
	void set_default_value(std::string name, std::string defvalue);
	void set_description(std::string name, std::string description);
	void remove_entry(entry &delentry);

	// parsing/input
	bool parse_command_line(int argc, char **argv, int priority, std::string &error_string);
	bool parse_ini_file(core_file &inifile, int priority, int ignore_priority, std::string &error_string);

	// reverting
	void revert(int priority = OPTION_PRIORITY_MAXIMUM);

	// output
	std::string output_ini(const core_options *diff = nullptr);
	std::string output_help();

	// reading
	std::string value(std::string option) const;
	std::string description(std::string option) const;
	int priority(std::string option) const;
	bool bool_value(std::string name) const { return (atoi(value(name).c_str()) != 0); }
	int int_value(std::string name) const { return atoi(value(name).c_str()); }
	float float_value(std::string name) const { return atof(value(name).c_str()); }
	UINT32 seqid(std::string name) const;
	bool exists(std::string name) const;

	// setting
	void set_command(std::string command);
	bool set_value(std::string name, std::string value, int priority, std::string &error_string);
	bool set_value(std::string name, int value, int priority, std::string &error_string);
	bool set_value(std::string name, float value, int priority, std::string &error_string);
	void set_flag(std::string name, UINT32 mask, UINT32 flags);

	// misc
	static const char *unadorned(int x = 0) { return s_option_unadorned[MIN(x, MAX_UNADORNED_OPTIONS)]; }
	int options_count() const { return m_entrylist.count(); }

private:
	// internal helpers
	void reset();
	void append_entry(entry &newentry);
	void copyfrom(const core_options &src);
	bool validate_and_set_data(entry &curentry, std::string newdata, int priority, std::string &error_string);

	// internal state
	simple_list<entry>      m_entrylist;            // head of list of entries
	std::unordered_map<std::string,entry *>       m_entrymap;             // map for fast lookup
	std::string             m_command;              // command found
	static const char *const s_option_unadorned[];  // array of unadorned option "names"
};



#endif /* __OPTIONS_H__ */
