lib_LTLIBRARIES = libxraft.la
AM_CPPFLAGS = -I$(top_srcdir)/include -DXRAFT_X11 $(X11_CFLAGS) $(DBUS_CFLAGS) $(GL_FLAGS)
AM_CXXFLAGS = -std=c++17
AM_LDFLAGS =
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
libxraft_la_LIBADD = $(X11_LIBS) $(DBUS_LIBS) $(GL_LIBS)
libxraft_la_SOURCES = \
	dbus.cc \
	ibus.cc \
	application_x11.cc \
	graphics_x11.cc \
	input_context_x11.cc \
	opengl_x11.cc \
	pixmap_x11.cc \
	timer_x11.cc \
	window_x11.cc
