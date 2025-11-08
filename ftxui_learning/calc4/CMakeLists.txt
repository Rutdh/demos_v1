cmake_minimum_required(VERSION 3.19.0)
project(plugin_manager VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

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

target_link_libraries(
    calc4
    PRIVATE
    ${TOP_DIR}/lib/ftxui/static-debug/libftxui-component.a
    ${TOP_DIR}/lib/ftxui/static-debug/libftxui-dom.a
    ${TOP_DIR}/lib/ftxui/static-debug/libftxui-screen.a
)
