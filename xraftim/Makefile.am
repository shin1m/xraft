componentdir = $(datadir)/ibus/component

AM_CPPFLAGS = -I$(top_srcdir)/include $(DBUS_CFLAGS)
AM_CXXFLAGS = -std=c++11
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
libexec_PROGRAMS = xraftim
xraftim_LDADD = $(DBUS_LIBS)
xraftim_SOURCES = \
	../src/dbus.cc \
	../src/ibus.cc \
	engine.cc \
	basic_dictionary.cc \
	main.cc
component_DATA = xraftim.xml
CLEANFILES = xraftim.xml
EXTRA_DIST = xraftim.xml.in
xraftim.xml: xraftim.xml.in
	$(AM_V_GEN) sed \
		-e 's|@VERSION[@]|$(VERSION)|g' \
		-e 's|@libexecdir[@]|$(libexecdir)|g' $< > $@.tmp && \
		mv $@.tmp $@
