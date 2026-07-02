#include "sxui/BarChart.h"
#include "sxui/Theme.h"

#include <QPainter>
#include <cmath>

namespace sxui {

BarChart::BarChart(QWidget* parent)
    : QWidget(parent)
{
}

void BarChart::setData(const QStringList& categories, const QVector<double>& values)
{
    m_categories = categories;
    m_values = values;
    update();
}

void BarChart::setBarColor(const QColor& color)
{
    m_color = color;
    update();
}

void BarChart::setValueSuffix(const QString& suffix)
{
    m_suffix = suffix;
    update();
}

void BarChart::paintEvent(QPaintEvent*)
{
    const Theme& t = Theme::current();
    QPainter p(this);

    const int n = m_values.size();
    if (n == 0)
        return;

    double maxV = 0;
    for (double v : m_values)
        maxV = qMax(maxV, v);
    if (maxV <= 0)
        maxV = 1;
    maxV *= 1.15; // headroom for value labels

    const QColor color = m_color.isValid() ? m_color : t.primary;
    const QFont labelFont = t.font(8.0);
    const QFontMetricsF fm(labelFont);
    const QRectF plot(12, 8 + fm.height(), width() - 24,
                      height() - 8 - fm.height() * 2 - 14);
    if (plot.width() < 20 || plot.height() < 20)
        return;

    // Horizontal gridlines
    p.setPen(QPen(alpha(t.border, 110), 1.0));
    for (int i = 1; i <= 3; ++i) {
        const qreal y = plot.bottom() - plot.height() * i / 4.0;
        p.drawLine(QPointF(plot.x(), y), QPointF(plot.right(), y));
    }
    // Baseline
    p.setPen(QPen(t.border, 1.0));
    p.drawLine(QPointF(plot.x(), plot.bottom() + 0.5),
               QPointF(plot.right(), plot.bottom() + 0.5));

    const qreal slot = plot.width() / n;
    const qreal barW = qMax(6.0, slot * 0.55);
    p.setFont(labelFont);

    for (int i = 0; i < n; ++i) {
        const double v = qMax(0.0, m_values[i]);
        const qreal h = plot.height() * (v / maxV);
        const QRectF bar(plot.x() + slot * i + (slot - barW) / 2,
                         plot.bottom() - h, barW, h);

        p.setPen(Qt::NoPen);
        p.fillRect(bar, alpha(color, 55));
        p.setPen(QPen(alpha(color, 180), 1.0));
        p.setBrush(Qt::NoBrush);
        p.drawRect(bar.adjusted(0.5, 0.5, -0.5, -0.5));
        // Bright cap
        p.fillRect(QRectF(bar.x(), bar.y(), bar.width(), 2.0), color);

        // Value label
        const QString valueText = QString::number(v, 'f', 0) + m_suffix;
        p.setPen(t.text);
        p.drawText(QRectF(bar.x() - slot / 2, bar.y() - fm.height() - 3,
                          bar.width() + slot, fm.height()),
                   Qt::AlignHCenter, valueText);

        // Category label
        if (i < m_categories.size()) {
            p.setPen(t.textDim);
            p.drawText(QRectF(plot.x() + slot * i, plot.bottom() + 6, slot, fm.height()),
                       Qt::AlignHCenter, m_categories[i]);
        }
    }
}

} // namespace sxui
