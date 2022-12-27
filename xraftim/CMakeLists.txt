add_executable(xraftim
	"${PROJECT_SOURCE_DIR}/src/dbus.cc"
	"${PROJECT_SOURCE_DIR}/src/ibus.cc"
	engine.cc
	basic_dictionary.cc
	main.cc
)
target_compile_features(xraftim PRIVATE cxx_std_17)
target_include_directories(xraftim PRIVATE "${PROJECT_SOURCE_DIR}/include")
target_link_libraries(xraftim dbus-1)
install(CODE "execute_process(
	COMMAND sed
		-e \"s|@VERSION[@]|${PROJECT_VERSION}|g\"
		-e \"s|@libexecdir[@]|\${CMAKE_INSTALL_PREFIX}/libexec|g\"
		\"${CMAKE_CURRENT_SOURCE_DIR}/xraftim.xml.in\"
	OUTPUT_FILE \"${CMAKE_CURRENT_BINARY_DIR}/xraftim.xml\"
	)
")
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/xraftim.xml DESTINATION share/ibus/component)
