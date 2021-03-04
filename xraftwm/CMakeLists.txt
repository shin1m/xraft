add_library(xraftwm SHARED client.cc root.cc)
target_link_libraries(xraftwm xraft X11::Xext)
