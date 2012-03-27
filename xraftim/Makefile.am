AM_CPPFLAGS = -I$(top_srcdir)/include $(DBUS_CFLAGS)
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
bin_PROGRAMS = xraftim
xraftim_LDADD = $(DBUS_LIBS)
xraftim_SOURCES = \
	../src/dbus.cc \
	../src/ibus.cc \
	engine.cc \
	basic_dictionary.cc \
	main.cc
