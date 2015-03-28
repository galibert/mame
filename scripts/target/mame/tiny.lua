-- license:BSD-3-Clause
-- copyright-holders:MAMEdev Team

---------------------------------------------------------------------------
--
--   tiny.lua
--
--   Small driver-specific example makefile
--   Use make SUBTARGET=tiny to build
--
---------------------------------------------------------------------------


--------------------------------------------------
-- Specify all the CPU cores necessary for the
-- drivers referenced in tiny.c.
--------------------------------------------------

CPUS["M6502"] = true
CPUS["M6805"] = true
CPUS["M680X0"] = true
CPUS["UPD7810"] = true

--------------------------------------------------
-- Specify all the sound cores necessary for the
-- drivers referenced in tiny.c.
--------------------------------------------------

SOUNDS["SAMPLES"] = true
SOUNDS["DAC"] = true
SOUNDS["BEEP"] = true
SOUNDS["WAVE"] = true
SOUNDS["SPEAKER"] = true
SOUNDS["AY8910"] = true
SOUNDS["YM2151"] = true
SOUNDS["POKEY"] = true
SOUNDS["TIA"] = true

--------------------------------------------------
-- specify available video cores
--------------------------------------------------

--------------------------------------------------
-- specify available machine cores
--------------------------------------------------

MACHINES["MOS6551"] = true
MACHINES["6522VIA"] = true
MACHINES["WD_FDC"] = true
MACHINES["RIOT6532"] = true
MACHINES["NSCSI"] = true
MACHINES["UPD765"] = true
MACHINES["STEPPERS"] = true
MACHINES["E05A03"] = true
MACHINES["E05A30"] = true
MACHINES["EEPROMDEV"] = true
MACHINES["MCCS1850"] = true
MACHINES["MB8795"] = true
MACHINES["NCR5390"] = true
MACHINES["8530SCC"] = true


--------------------------------------------------
-- specify available bus cores
--------------------------------------------------

BUSES["ORICEXT"] = true
BUSES["A7800"] = true
BUSES["CENTRONICS"] = true

--------------------------------------------------
-- This is the list of files that are necessary
-- for building all of the drivers referenced
-- in tiny.c
--------------------------------------------------

function createProjects_mame_tiny(_target, _subtarget)
	project ("mame_tiny")
	targetsubdir(_target .."_" .. _subtarget)
	kind "StaticLib"
	uuid (os.uuid("drv-mame-tiny"))
	
	options {
		"ForceCPP",
	}
	
	includedirs {
		MAME_DIR .. "src/osd",
		MAME_DIR .. "src/emu",
		MAME_DIR .. "src/mame",
		MAME_DIR .. "src/mess",
		MAME_DIR .. "src/lib",
		MAME_DIR .. "src/lib/util",
		MAME_DIR .. "3rdparty",
		MAME_DIR .. "3rdparty/zlib",
		GEN_DIR  .. "mame/layout",
	}	

	files{
		MAME_DIR .. "src/mess/drivers/a7800.c",
		MAME_DIR .. "src/mess/drivers/next.c",
		MAME_DIR .. "src/mess/drivers/oric.c",
		MAME_DIR .. "src/mess/video/maria.c", 
        MAME_DIR .. "src/mess/machine/nextkbd.c",
        MAME_DIR .. "src/mess/machine/nextmo.c", 
	}
	
	--------------------------------------------------
	-- layout dependencies
	--------------------------------------------------

	dependency {
	}

	custombuildtask {
	}	
end

function linkProjects_mame_tiny(_target, _subtarget)
	links {
		"mame_tiny",
	}
end