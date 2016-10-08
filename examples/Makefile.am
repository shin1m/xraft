AM_CPPFLAGS = -I$(top_srcdir)/include -DXRAFT_X11 $(X11_CFLAGS) $(DBUS_CFLAGS) $(GL_FLAGS)
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

noinst_PROGRAMS = balls cairo gears dbus ibus
balls_LDADD = $(top_srcdir)/src/libxraft.la
balls_SOURCES = balls.cc
cairo_CPPFLAGS = $(AM_CPPFLAGS) $(CAIRO_CFLAGS)
cairo_LDADD = $(top_srcdir)/src/libxraft.la $(CAIRO_LIBS)
cairo_SOURCES = cairo.cc
gears_LDADD = $(top_srcdir)/src/libxraft.la $(GL_LIBS)
gears_SOURCES = gears.cc
dbus_LDADD = $(top_srcdir)/src/libxraft.la
dbus_SOURCES = dbus.cc
ibus_LDADD = $(top_srcdir)/src/libxraft.la
ibus_SOURCES = ibus.cc
