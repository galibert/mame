// license:BSD-3-Clause
// copyright-holders:Wilbert Pol
/*********************************************************************

    softlist.h

    Software and software list information.

*********************************************************************/

#ifndef __SOFTLIST_H_
#define __SOFTLIST_H_

//**************************************************************************
//  CONSTANTS
//**************************************************************************

#define SOFTWARE_SUPPORTED_YES      0
#define SOFTWARE_SUPPORTED_PARTIAL  1
#define SOFTWARE_SUPPORTED_NO       2

enum softlist_type
{
	SOFTWARE_LIST_ORIGINAL_SYSTEM,
	SOFTWARE_LIST_COMPATIBLE_SYSTEM
};



//**************************************************************************
//  MACROS
//**************************************************************************

#define MCFG_SOFTWARE_LIST_CONFIG(_list,_list_type) \
	software_list_device::static_set_type(*device, _list, _list_type);

#define MCFG_SOFTWARE_LIST_ADD( _tag, _list ) \
	MCFG_DEVICE_ADD( _tag, SOFTWARE_LIST, 0 ) \
	MCFG_SOFTWARE_LIST_CONFIG(_list, SOFTWARE_LIST_ORIGINAL_SYSTEM)

#define MCFG_SOFTWARE_LIST_COMPATIBLE_ADD( _tag, _list ) \
	MCFG_DEVICE_ADD( _tag, SOFTWARE_LIST, 0 ) \
	MCFG_SOFTWARE_LIST_CONFIG(_list, SOFTWARE_LIST_COMPATIBLE_SYSTEM)

#define MCFG_SOFTWARE_LIST_MODIFY( _tag, _list ) \
	MCFG_DEVICE_MODIFY( _tag ) \
	MCFG_SOFTWARE_LIST_CONFIG(_list, SOFTWARE_LIST_ORIGINAL_SYSTEM)

#define MCFG_SOFTWARE_LIST_COMPATIBLE_MODIFY( _tag, _list ) \
	MCFG_DEVICE_MODIFY( _tag ) \
	MCFG_SOFTWARE_LIST_CONFIG(_list, SOFTWARE_LIST_COMPATIBLE_SYSTEM)

#define MCFG_SOFTWARE_LIST_FILTER( _tag, _filter ) \
	MCFG_DEVICE_MODIFY( _tag ) \
	software_list_device::static_set_filter(*device, _filter);

#define MCFG_SOFTWARE_LIST_REMOVE( _tag ) \
	MCFG_DEVICE_REMOVE( _tag )



//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> feature_list_item

// an item in a list of name/value pairs
class feature_list_item
{
	friend class simple_list<feature_list_item>;

public:
	// construction/destruction
	feature_list_item(std::string name = nullptr, std::string value = nullptr)
		: m_next(nullptr),
			m_name(name),
			m_value(value) { }

	// getters
	feature_list_item *next() const { return m_next; }
	std::string name() const { return m_name; }
	std::string value() const { return m_value; }

private:
	// internal state
	feature_list_item * m_next;
	std::string         m_name;
	std::string         m_value;
};


// ======================> software_part

// a single part of a software item
class software_part
{
	friend class softlist_parser;
	friend class simple_list<software_part>;

public:
	// construction/destruction
	software_part(software_info &info, std::string name = nullptr, std::string interface = nullptr);

	// getters
	software_part *next() const { return m_next; }
	software_info &info() const { return m_info; }
	std::string name() const { return m_name; }
	std::string interface() const { return m_interface; }
	feature_list_item *featurelist() const { return m_featurelist.first(); }
	rom_entry *romdata(unsigned int index = 0) { return (index < m_romdata.size()) ? &m_romdata[index] : nullptr; }

	// helpers
	bool is_compatible(const software_list_device &swlist) const;
	bool matches_interface(std::string interface) const;
	std::string feature(std::string feature_name) const;

private:
	// internal state
	software_part *     m_next;
	software_info &     m_info;
	std::string         m_name;
	std::string         m_interface;
	simple_list<feature_list_item> m_featurelist;
	std::vector<rom_entry>   m_romdata;
};


// ======================> software_info

// a single software item
class software_info
{
	friend class softlist_parser;
	friend class simple_list<software_info>;

public:
	// construction/destruction
	software_info(software_list_device &list, std::string name, std::string parent, std::string supported);

	// getters
	software_info *next() const { return m_next; }
	software_list_device &list() const { return m_list; }
	std::string shortname() const { return m_shortname; }
	std::string longname() const { return m_longname; }
	std::string parentname() const { return m_parentname; }
	std::string year() const { return m_year; }
	std::string publisher() const { return m_publisher; }
	feature_list_item *other_info() const { return m_other_info.first(); }
	feature_list_item *shared_info() const { return m_shared_info.first(); }
	UINT32 supported() const { return m_supported; }
	int num_parts() const { return m_partdata.count(); }
	software_part *first_part() const { return m_partdata.first(); }
	software_part *last_part() const { return m_partdata.last(); }

	// additional operations
	software_part *find_part(std::string partname, std::string interface = nullptr);
	bool has_multiple_parts(std::string interface) const;

private:
	// internal state
	software_info *         m_next;
	software_list_device &  m_list;
	UINT32                  m_supported;
	std::string             m_shortname;
	std::string             m_longname;
	std::string             m_parentname;
	std::string             m_year;           // Copyright year on title screen, actual release dates can be tracked in external resources
	std::string             m_publisher;
	simple_list<feature_list_item> m_other_info;   // Here we store info like developer, serial #, etc. which belong to the software entry as a whole
	simple_list<feature_list_item> m_shared_info;  // Here we store info like TV standard compatibility, or add-on requirements, etc. which get inherited
												// by each part of this software entry (after loading these are stored in partdata->featurelist)
	simple_list<software_part> m_partdata;
};


// ======================> software_list_device

// device representing a software list
class software_list_device : public device_t
{
	friend class softlist_parser;

public:
	// construction/destruction
	software_list_device(const machine_config &mconfig, std::string tag, device_t *owner, UINT32 clock);

	// inline configuration helpers
	static void static_set_type(device_t &device, std::string list, softlist_type list_type);
	static void static_set_filter(device_t &device, std::string filter);

	// getters
	std::string list_name() const { return m_list_name; }
	softlist_type list_type() const { return m_list_type; }
	std::string filter() const { return m_filter; }
	std::string filename() { return m_file.filename(); }

	// getters that may trigger a parse
	std::string description() { if (!m_parsed) parse(); return m_description; }
	bool valid() { if (!m_parsed) parse(); return m_infolist.count() > 0; }
	std::string errors_string() { if (!m_parsed) parse(); return m_errors; }

	// operations
	software_info *find(std::string look_for, software_info *prev = nullptr);
	software_info *first_software_info() { if (!m_parsed) parse(); return m_infolist.first(); }
	void find_approx_matches(std::string name, int matches, software_info **list, std::string interface);
	void release();

	// static helpers
	static software_list_device *find_by_name(const machine_config &mconfig, std::string name);
	static void display_matches(const machine_config &config, std::string interface, std::string name);

protected:
	// internal helpers
	void parse();
	void internal_validity_check(validity_checker &valid) ATTR_COLD;

	// device-level overrides
	virtual void device_start() override;
	virtual void device_validity_check(validity_checker &valid) const override ATTR_COLD;

	// configuration state
	std::string                 m_list_name;
	softlist_type               m_list_type;
	std::string                 m_filter;

	// internal state
	bool                        m_parsed;
	emu_file                    m_file;
	std::string                 m_description;
	std::string                 m_errors;
	simple_list<software_info>  m_infolist;
};


// device type definition
extern const device_type SOFTWARE_LIST;

// device type iterator
typedef device_type_iterator<&device_creator<software_list_device>, software_list_device> software_list_device_iterator;


#endif
