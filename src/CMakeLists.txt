qt_add_library(sxui STATIC
    Sxui.h
    Sxui.cpp
    core/Theme.h
    core/Theme.cpp
    core/Style.h
    core/Style.cpp
    widgets/Panel.h
    widgets/Panel.cpp
    widgets/ScrollArea.h
    widgets/ScrollArea.cpp
    widgets/Toggle.h
    widgets/Toggle.cpp
    widgets/Gauge.h
    widgets/Gauge.cpp
    widgets/FileBrowser.h
    widgets/FileBrowser.cpp
    widgets/ImageView.h
    widgets/ImageView.cpp
    widgets/TableEditor.h
    widgets/TableEditor.cpp
    widgets/Workspace.h
    widgets/Workspace.cpp
    charts/LineChart.h
    charts/LineChart.cpp
    charts/BarChart.h
    charts/BarChart.cpp
    media/VideoView.h
    media/VideoView.cpp
    media/Timeline.h
    media/Timeline.cpp
)

# Headers are addressed as <module/Class.h> (or <Sxui.h>) from src/.
target_include_directories(sxui PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_link_libraries(sxui PUBLIC Qt6::Widgets Qt6::OpenGLWidgets)
