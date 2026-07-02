qt_add_library(sxui STATIC
    Sxui.h
    Sxui.cpp
    core/Theme.h
    core/Theme.cpp
    core/Style.h
    core/Style.cpp
    widgets/Panel.h
    widgets/Panel.cpp
    widgets/Toggle.h
    widgets/Toggle.cpp
    widgets/Gauge.h
    widgets/Gauge.cpp
    widgets/FileBrowser.h
    widgets/FileBrowser.cpp
    widgets/ImageView.h
    widgets/ImageView.cpp
    charts/LineChart.h
    charts/LineChart.cpp
    charts/BarChart.h
    charts/BarChart.cpp
)

# Headers are addressed as <module/Class.h> (or <Sxui.h>) from src/.
target_include_directories(sxui PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_link_libraries(sxui PUBLIC Qt6::Widgets)
