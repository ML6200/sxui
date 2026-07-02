cmake_minimum_required(VERSION 3.21)
project(sxui VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Widgets)
qt_standard_project_setup()

qt_add_library(sxui STATIC
    include/sxui/Sxui.h
    include/sxui/Theme.h
    include/sxui/Style.h
    include/sxui/Panel.h
    include/sxui/Toggle.h
    include/sxui/Gauge.h
    include/sxui/LineChart.h
    include/sxui/BarChart.h
    src/Sxui.cpp
    src/Theme.cpp
    src/Style.cpp
    src/Panel.cpp
    src/Toggle.cpp
    src/Gauge.cpp
    src/LineChart.cpp
    src/BarChart.cpp
)

target_include_directories(sxui PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(sxui PUBLIC Qt6::Widgets)

add_subdirectory(apps/gallery)
