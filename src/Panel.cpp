#include "sxui/Panel.h"
#include "sxui/Theme.h"

#include <QPaintEvent>
#include <QPainter>

namespace sxui {

Panel::Panel(const QString& title, QWidget* parent)
    : QWidget(parent)
    , m_title(title)
{
    setContentsMargins(14, m_headerHeight + 12, 14, 14);
}

void Panel::setTitle(const QString& title)
{
    m_title = title;
    update();
}

void Panel::setStatus(const QString& text, const QColor& color)
{
    m_status = text;
    m_statusColor = color;
    update();
}

void Panel::paintEvent(QPaintEvent*)
{
    const Theme& t = Theme::current();
    QPainter p(this);
    const QRectF r = QRectF(rect()).adjusted(0.5, 0.5, -0.5, -0.5);

    p.fillRect(rect(), t.surface);
    p.setPen(QPen(t.border, 1.0));
    p.drawRect(r);

    // Header
    const QRectF header(r.x(), r.y(), r.width(), m_headerHeight);
    p.drawLine(header.bottomLeft(), header.bottomRight());

    p.setFont(t.headingFont(9.0));
    p.setPen(t.primary);
    p.drawText(header.adjusted(12, 0, -12, 0), Qt::AlignLeft | Qt::AlignVCenter, m_title);

    if (!m_status.isEmpty()) {
        p.setPen(m_statusColor.isValid() ? m_statusColor : t.textDim);
        p.drawText(header.adjusted(12, 0, -12, 0), Qt::AlignRight | Qt::AlignVCenter, m_status);
    }
}

} // namespace sxui
