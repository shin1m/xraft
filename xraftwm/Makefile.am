lib_LTLIBRARIES = libxraftwm.la
AM_CPPFLAGS = -I$(top_srcdir)/include -DXRAFT_X11 $(X11_CFLAGS) $(DBUS_CFLAGS)
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
libxraftwm_la_LIBADD = -lXext $(top_srcdir)/src/libxraft.la
libxraftwm_la_SOURCES = \
	common.h \
	client.cc \
	root.cc
