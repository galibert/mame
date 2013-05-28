//============================================================
//
//  window.h - Generic window management classes
//
//============================================================
//
//  Copyright Nicola Salmoria and the MAME Team.
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or
//  without modification, are permitted provided that the
//  following conditions are met:
//
//    * Redistributions of source code must retain the above
//      copyright notice, this list of conditions and the
//      following disclaimer.
//    * Redistributions in binary form must reproduce the
//      above copyright notice, this list of conditions and
//      the following disclaimer in the documentation and/or
//      other materials provided with the distribution.
//    * Neither the name 'MAME' nor the names of its
//      contributors may be used to endorse or promote
//      products derived from this software without specific
//      prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY AARON GILES ''AS IS'' AND
//  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
//  FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
//  EVENT SHALL AARON GILES BE LIABLE FOR ANY DIRECT,
//  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
//  DAMAGE (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
//  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
//  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
//  ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
//  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
//  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
//  IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//============================================================

#ifndef __RENDER_WINDOW__
#define __RENDER_WINDOW__

#include "render.h"

//============================================================
//  TYPE DEFINITIONS
//============================================================

namespace render
{

class video_system;
class window_info;
class monitor_info;
class draw_hal;

class window_system
{
public:
	window_system(running_machine &machine, video_system *video) :
		m_machine(machine),
		m_video(video)
		{ }
	virtual ~window_system() { }

	virtual void			process_events_periodic() = 0;

	virtual UINT64			main_threadid() { return m_main_threadid; }
	virtual UINT64			window_threadid() { return m_window_threadid; }

	virtual window_info *	window_alloc(monitor_info *monitor);

	virtual void			toggle_full_screen() = 0;

	virtual window_info *	window_list() { return m_window_list; }

	virtual void			take_snap() = 0;
	virtual void			toggle_fsfx() = 0;
	virtual void			take_video() = 0;

	struct window_config
	{
		float               aspect;	// decoded aspect ratio
		int                 width;	// decoded width
		int                 height;	// decoded height
		int                 refresh;	// decoded refresh
	};

protected:
	running_machine &		m_machine;

	video_system *			m_video;

	window_info *			m_window_list;

	UINT64					m_main_threadid;
	UINT64					m_window_threadid;

	bool					m_multithreading_enabled;
};

class window_info
{
public:
	window_info(running_machine &machine, UINT64 main_threadid, UINT64 window_threadid, window_system *system,
		monitor_info *monitor);
	virtual ~window_info();

	running_machine &		machine() const { return m_machine; }

	virtual window_info *	next() { return m_next; }
	virtual void			set_next(window_info *next) { m_next = next; }

	virtual void			set_maxdims(int width, int height) { m_maxwidth = width; m_maxheight = height; }
	virtual void			set_refresh(int refresh) { m_refresh = refresh; }
	virtual void			set_fullscreen(bool fullscreen) { m_fullscreen = fullscreen; }
	virtual void			set_fullscreen_safe(bool safe) { m_fullscreen_safe = safe; }
	virtual void			toggle_fullscreen(bool fullscreen) = 0;

	virtual void			wait_for_ready();

	virtual void			complete_create() { m_init_state = -1; }

	virtual int				extra_width() { return 0; }
	virtual int				extra_height() { return 0; }

	bool					fullscreen() { return m_fullscreen; }
	virtual void			minimize_window() = 0;
	virtual void			maximize_window() = 0;

	render_primitive_list &	get_primitives();

	render_target *			target() { return m_target; }

	window_info *			window_list() { return m_window_list; }

	void					set_primlist(render_primitive_list *primlist) { m_primlist = primlist; }

	int						targetorient() { return m_targetorient; }

	virtual monitor_info *	monitor() { return m_monitor; }

protected:
	virtual void			set_starting_view(int index, const char *defview, const char *view);

	// monitor info
	monitor_info *			m_monitor;

private:
	draw_hal *			m_hal;
	window_system *		m_system;

	window_info *		m_window_list;

	window_info *   	m_next;
	volatile int        m_init_state;

	bool				m_fullscreen;
	int                 m_fullscreen_safe;
	int                 m_maxwidth, m_maxheight;
	int                 m_refresh;
	float               m_aspect;

	osd_lock *          m_render_lock;
	render_target *     m_target;
	int                 m_targetview;
	int                 m_targetorient;
	render_layer_config m_targetlayerconfig;
	render_primitive_list *m_primlist;

	// drawing data
	void *              m_drawdata;

	running_machine &   m_machine;

	UINT64				m_window_threadid;
	UINT64				m_main_threadid;
};

}; // namespace render

#endif // __RENDER_WINDOW__