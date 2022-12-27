link_libraries(xraft xemmai)
function(add_module name)
	add_library(${name}-xemmai MODULE ${ARGN})
	set_target_properties(${name}-xemmai PROPERTIES OUTPUT_NAME ${name} PREFIX "")
endfunction()
add_module(xraft
	application.cc
	drawable.cc
	geometry.cc
	graphics.cc
	input_context.cc
	object.cc
	opengl.cc
	pixmap.cc
	region.cc
	symbol.cc
	timer.cc
	window.cc
	xraft.cc
)
add_module(xraftdbus xraftdbus.cc)
add_module(xraftcairo xraftcairo.cc)
target_compile_options(xraftcairo-xemmai PRIVATE ${CAIRO_CFLAGS})
target_link_libraries(xraftcairo-xemmai ${CAIRO_LIBRARIES})
add_module(xraftwm xraftwm.cc)
target_include_directories(xraftwm-xemmai PRIVATE ${PROJECT_SOURCE_DIR})
target_link_libraries(xraftwm-xemmai xraftwm)
