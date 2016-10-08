AM_CPPFLAGS = -I$(top_srcdir)/include -DXRAFT_X11 $(X11_CFLAGS) $(DBUS_CFLAGS)
AM_CXXFLAGS =
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
bin_PROGRAMS = xraftvt xrafttt
#xraftvt_CPPFLAGS = $(AM_CPPFLAGS)
xraftvt_LDADD = -lutil $(top_srcdir)/src/libxraft.la
xraftvt_SOURCES = \
	buffer.cc \
	window.cc \
	terminal.cc \
	main.cc
xrafttt_CPPFLAGS = $(AM_CPPFLAGS) -DXRAFT_TRANSPARENT
xrafttt_LDADD = -lutil $(top_srcdir)/src/libxraft.la
xrafttt_SOURCES = \
	buffer.cc \
	window.cc \
	terminal.cc \
	main.cc
