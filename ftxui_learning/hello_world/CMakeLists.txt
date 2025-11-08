cmake_minimum_required(VERSION 3.19.0)
project(plugin_manager VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
# set(RELATIVE_DIR "ftxui_learing/hello_world")

# Plugin manager library
add_executable(
    hello_ftxui
    main.cpp
)

target_include_directories(
    hello_ftxui
    PRIVATE
    ${TOP_DIR}/lib/ftxui/static-debug/include
)

target_link_libraries(
    hello_ftxui
    PRIVATE
    ${TOP_DIR}/lib/ftxui/static-debug/libftxui-component.a
    ${TOP_DIR}/lib/ftxui/static-debug/libftxui-dom.a
    ${TOP_DIR}/lib/ftxui/static-debug/libftxui-screen.a
)
