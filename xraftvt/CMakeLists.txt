link_libraries(xraft -lutil)
add_executable(xraftvt window.cc main.cc)
add_executable(xrafttt window.cc main.cc)
target_compile_definitions(xrafttt PRIVATE XRAFT_TRANSPARENT)
