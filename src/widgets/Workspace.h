#pragma once

#include <QMdiArea>

class QMdiSubWindow;

namespace sxui {

// In-app window manager: a themed QMdiArea. Sub-windows get phosphor
// title bars (drawn by sxui::Style), can be dragged, resized, tiled and
// cascaded. The full QMdiArea API remains available, including
// setViewMode(QMdiArea::TabbedView) for a tabbed workspace.
class Workspace : public QMdiArea {
    Q_OBJECT
public:
    explicit Workspace(QWidget* parent = nullptr);

    // Wraps content in a sub-window. Default position cascades; pass a
    // size to override the content's size hint.
    QMdiSubWindow* addWindow(QWidget* content, const QString& title,
                             const QSize& size = QSize());
};

} // namespace sxui
