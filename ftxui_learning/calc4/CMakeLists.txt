cmake_minimum_required(VERSION 3.19.0)
project(plugin_manager VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# set(CMAKE_PREFIX_PATH ${TOP_DIR}/lib/static-debug/ftxui/cmake/ftxui)
# set(ftxui_DIR "/home/ganjun/demos_v1/lib/static-debug/ftxui/lib/cmake/ftxui")
# list(APPEND CMAKE_PREFIX_PATH "/home/ganjun/demos_v1/lib/static-debug/glog")

set(
    glog_DIR
    "/home/ganjun/demos_v1/lib/static-debug/glog/lib/cmake/glog"
    CACHE PATH "Custom glog package location" FORCE
)
set(
    gflags_DIR
    "/home/ganjun/demos_v1/lib/static-debug/gflags/lib/cmake/gflags"
    CACHE PATH "Custom gflags package location" FORCE
)
list(APPEND CMAKE_PREFIX_PATH "/home/ganjun/demos_v1/lib/static-debug/ftxui")

set(GFLAGS_USE_TARGET_NAMESPACE ON)
find_package(gflags CONFIG REQUIRED)
find_package(ftxui CONFIG REQUIRED)
find_package(glog CONFIG REQUIRED)

# Plugin manager library
add_executable(
    calc4
    main.cpp
    CalcModel.cpp
)

target_link_libraries(calc4 PRIVATE 
    gflags::gflags
    ftxui::dom 
    ftxui::screen 
    ftxui::component
    glog::glog
)
