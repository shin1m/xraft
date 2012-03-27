xemmaidir = $(XEMMAI_MODULE_PATH)

xemmai_LTLIBRARIES = xraft.la xraftcairo.la xraftwm.la
AM_CPPFLAGS = -I$(top_srcdir)/include -DXRAFT_X11 $(X11_CFLAGS) $(DBUS_CFLAGS) $(GL_FLAGS) $(XEMMAI_CFLAGS)
AM_CXXFLAGS =
AM_LDFLAGS = -module -avoid-version
LIBADD = $(top_srcdir)/src/libxraft.la $(XEMMAI_LIBS)
if DEBUG
AM_CXXFLAGS += -g
else
AM_CPPFLAGS += -DNDEBUG
AM_CXXFLAGS += -O3
endif
if PROFILE
AM_CXXFLAGS += -pg
AM_LDFLAGS += -pg
endif
xraft_la_LIBADD = $(LIBADD)
xraft_la_SOURCES = \
	application.h \
	application.cc \
	drawable.h \
	drawable.cc \
	geometry.h \
	geometry.cc \
	graphics.h \
	graphics.cc \
	input_context.h \
	input_context.cc \
	object.h \
	object.cc \
	opengl.h \
	opengl.cc \
	pixmap.h \
	pixmap.cc \
	region.h \
	region.cc \
	symbol.h \
	symbol.cc \
	timer.h \
	timer.cc \
	window.h \
	window_define.h \
	window_override.h \
	window.cc \
	xraft.h \
	xraft.cc
xraftcairo_la_CPPFLAGS = $(AM_CPPFLAGS) $(CAIRO_CFLAGS)
xraftcairo_la_LIBADD = $(LIBADD) $(CAIRO_LIBS)
xraftcairo_la_SOURCES = \
	xraftcairo.h \
	xraftcairo.cc
xraftwm_la_CPPFLAGS = $(AM_CPPFLAGS) -I$(top_srcdir)
xraftwm_la_LIBADD = $(LIBADD) $(top_srcdir)/xraftwm/libxraftwm.la
xraftwm_la_SOURCES = \
	xraftwm.h \
	xraftwm.cc
