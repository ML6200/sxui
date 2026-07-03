#include "widgets/Workspace.h"
#include "core/Theme.h"

#include <QMdiSubWindow>

namespace sxui {

Workspace::Workspace(QWidget* parent)
    : QMdiArea(parent)
{
    setBackground(Theme::current().background);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setActivationOrder(QMdiArea::ActivationHistoryOrder);
}

QMdiSubWindow* Workspace::addWindow(QWidget* content, const QString& title,
                                    const QSize& size)
{
    QMdiSubWindow* sub = addSubWindow(content);
    sub->setWindowTitle(title);
    if (size.isValid())
        sub->resize(size);
    return sub;
}

} // namespace sxui
