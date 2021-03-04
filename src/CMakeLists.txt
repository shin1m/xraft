add_library(xraft SHARED
	dbus.cc
	ibus.cc
	application_x11.cc
	graphics_x11.cc
	input_context_x11.cc
	opengl_x11.cc
	pixmap_x11.cc
	timer_x11.cc
	window_x11.cc
	)
target_compile_features(xraft PUBLIC cxx_std_17)
target_include_directories(xraft PUBLIC
	"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
	"$<INSTALL_INTERFACE:include>"
	)
target_compile_definitions(xraft PUBLIC XRAFT_X11)
target_link_libraries(xraft X11::X11 dbus-1 OpenGL::OpenGL OpenGL::GLX)
