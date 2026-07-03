cmake_minimum_required(VERSION 3.21)
project(sxui VERSION 0.3.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Widgets)
qt_standard_project_setup()

add_subdirectory(src)

# Local, untracked demo apps (e.g. the component gallery).
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/apps/gallery/CMakeLists.txt)
    add_subdirectory(apps/gallery)
endif()
