cmake_minimum_required(VERSION 3.19.0)
project(plugin_manager VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# set(CMAKE_PREFIX_PATH ${TOP_DIR}/lib/static-debug/ftxui/cmake/ftxui)
set(ftxui_DIR "/home/gan/demos_v1/lib/static-debug/ftxui/cmake/ftxui")
# set(CMAKE_PREFIX_PATH ${TOP_DIR}/lib/static-debug/glog/cmake/glog)
find_package(ftxui CONFIG REQUIRED)
# find_package(glog CONFIG REQUIRED)

# Plugin manager library
add_executable(
    calc4
    main.cpp
    calc4.cpp
    ui.cpp
)

target_include_directories(
    calc4
    PRIVATE
    ${TOP_DIR}/lib/ftxui/static-debug/include
)

# target_link_libraries(
#     calc4
#     PRIVATE
#     ${TOP_DIR}/lib/ftxui/static-debug/libftxui-component.a
#     ${TOP_DIR}/lib/ftxui/static-debug/libftxui-dom.a
#     ${TOP_DIR}/lib/ftxui/static-debug/libftxui-screen.a
# )
target_link_libraries(calc4 PRIVATE ftxui::dom ftxui::screen ftxui::component)

target_link_libraries(calc4 PRIVATE glog::glog)