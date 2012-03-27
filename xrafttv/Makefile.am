EXTRA_DIST = \
	gapped \
	text_model.h \
	wrapped_view.h

AM_CPPFLAGS = -I$(top_srcdir)/include -DXRAFT_X11 $(X11_CFLAGS) $(DBUS_CFLAGS) $(GL_FLAGS)
AM_CXXFLAGS = -g
LDADD = $(top_srcdir)/src/libxraft.la
AM_DEFAULT_SOURCE_EXT = .cc
check_PROGRAMS = test_replace test_attribute test_wrapped_view
TESTS = $(check_PROGRAMS)

bin_PROGRAMS = editor

xrafttvdir = $(XEMMAI_MODULE_PATH)

xrafttv_LTLIBRARIES = xrafttv.la
xrafttv_la_CPPFLAGS = $(AM_CPPFLAGS) $(XEMMAI_CFLAGS) -I$(top_srcdir)/xemmai
xrafttv_la_CXXFLAGS =
xrafttv_la_LDFLAGS = -module -avoid-version
if DEBUG
xrafttv_la_CXXFLAGS += -g
else
xrafttv_la_CPPFLAGS += -DNDEBUG
xrafttv_la_CXXFLAGS += -O3
endif
if PROFILE
xrafttv_la_CXXFLAGS += -pg
xrafttv_la_LDFLAGS += -pg
endif
xrafttv_la_LIBADD = $(LDADD) $(XEMMAI_LIBS)
xrafttv_la_SOURCES = \
	xrafttv.h \
	xrafttv.cc
