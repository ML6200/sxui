#include "charts/LineChart.h"
#include "core/Theme.h"

#include <QPainter>
#include <QPainterPath>
#include <cmath>

namespace sxui {

namespace {

// Rounds to 1/2/5 * 10^n for readable axis steps.
double niceStep(double range, int targetTicks)
{
    if (range <= 0 || targetTicks < 1)
        return 1.0;
    const double rough = range / targetTicks;
    const double mag = std::pow(10.0, std::floor(std::log10(rough)));
    const double norm = rough / mag;
    double step;
    if (norm < 1.5)
        step = 1.0;
    else if (norm < 3.5)
        step = 2.0;
    else if (norm < 7.5)
        step = 5.0;
    else
        step = 10.0;
    return step * mag;
}

QString defaultFormat(double v)
{
    if (std::abs(v) >= 10000 || (std::abs(v) < 0.01 && v != 0.0))
        return QString::number(v, 'g', 3);
    QString s = QString::number(v, 'f', 2);
    while (s.endsWith(QLatin1Char('0')))
        s.chop(1);
    if (s.endsWith(QLatin1Char('.')))
        s.chop(1);
    return s;
}

const QColor& cycleColor(int index)
{
    const Theme& t = Theme::current();
    static const QColor* cycle[] = { &t.primary, &t.warning, &t.textBright, &t.danger };
    return *cycle[index % 4];
}

} // namespace

LineChart::LineChart(QWidget* parent)
    : QWidget(parent)
{
}

int LineChart::addSeries(const QString& name, const QColor& color)
{
    Series s;
    s.name = name;
    s.color = color.isValid() ? color : cycleColor(m_series.size());
    m_series.append(s);
    update();
    return m_series.size() - 1;
}

void LineChart::setSeriesData(int index, QVector<QPointF> points)
{
    if (index < 0 || index >= m_series.size())
        return;
    m_series[index].points = std::move(points);
    update();
}

void LineChart::appendPoint(int index, const QPointF& point, int maxPoints)
{
    if (index < 0 || index >= m_series.size())
        return;
    auto& pts = m_series[index].points;
    pts.append(point);
    if (maxPoints > 0 && pts.size() > maxPoints)
        pts.remove(0, pts.size() - maxPoints);
    update();
}

void LineChart::clearSeries()
{
    m_series.clear();
    update();
}

void LineChart::setXRange(double min, double max) { m_autoX = false; m_xMin = min; m_xMax = max; update(); }
void LineChart::setYRange(double min, double max) { m_autoY = false; m_yMin = min; m_yMax = max; update(); }
void LineChart::setAutoXRange() { m_autoX = true; update(); }
void LineChart::setAutoYRange() { m_autoY = true; update(); }
void LineChart::setXFormatter(Formatter formatter) { m_xFmt = std::move(formatter); update(); }
void LineChart::setYFormatter(Formatter formatter) { m_yFmt = std::move(formatter); update(); }
void LineChart::setFillEnabled(bool enabled) { m_fill = enabled; update(); }
void LineChart::setLegendVisible(bool visible) { m_legend = visible; update(); }

void LineChart::dataRange(double& xMin, double& xMax, double& yMin, double& yMax) const
{
    bool any = false;
    for (const Series& s : m_series) {
        for (const QPointF& pt : s.points) {
            if (!any) {
                xMin = xMax = pt.x();
                yMin = yMax = pt.y();
                any = true;
            } else {
                xMin = qMin(xMin, pt.x());
                xMax = qMax(xMax, pt.x());
                yMin = qMin(yMin, pt.y());
                yMax = qMax(yMax, pt.y());
            }
        }
    }
    if (!any) {
        xMin = yMin = 0;
        xMax = yMax = 1;
    }
    if (xMax - xMin < 1e-12)
        xMax = xMin + 1;
    if (yMax - yMin < 1e-12) {
        yMin -= 0.5;
        yMax += 0.5;
    }
}

void LineChart::paintEvent(QPaintEvent*)
{
    const Theme& t = Theme::current();
    QPainter p(this);

    double xMin = m_xMin, xMax = m_xMax, yMin = m_yMin, yMax = m_yMax;
    if (m_autoX || m_autoY) {
        double dxMin, dxMax, dyMin, dyMax;
        dataRange(dxMin, dxMax, dyMin, dyMax);
        if (m_autoX) {
            xMin = dxMin;
            xMax = dxMax;
        }
        if (m_autoY) {
            const double pad = (dyMax - dyMin) * 0.08;
            yMin = dyMin - pad;
            yMax = dyMax + pad;
        }
    }
    if (xMax - xMin < 1e-12)
        xMax = xMin + 1;
    if (yMax - yMin < 1e-12)
        yMax = yMin + 1;

    const QFont axisFont = t.font(8.0);
    const QFontMetricsF fm(axisFont);
    const qreal left = 10 + fm.horizontalAdvance(QStringLiteral("00000")) + 6;
    const QRectF plot(left, 10, width() - left - 12, height() - 10 - fm.height() - 14);
    if (plot.width() < 20 || plot.height() < 20)
        return;

    const auto mapX = [&](double x) {
        return plot.x() + (x - xMin) / (xMax - xMin) * plot.width();
    };
    const auto mapY = [&](double y) {
        return plot.bottom() - (y - yMin) / (yMax - yMin) * plot.height();
    };
    const auto fmtX = m_xFmt ? m_xFmt : Formatter(defaultFormat);
    const auto fmtY = m_yFmt ? m_yFmt : Formatter(defaultFormat);

    // Grid + labels
    p.setFont(axisFont);
    const double yStep = niceStep(yMax - yMin, 4);
    for (double y = std::ceil(yMin / yStep) * yStep; y <= yMax + yStep * 0.01; y += yStep) {
        const qreal py = mapY(y);
        p.setPen(QPen(alpha(t.border, 110), 1.0));
        p.drawLine(QPointF(plot.x(), py), QPointF(plot.right(), py));
        p.setPen(t.textDim);
        p.drawText(QRectF(0, py - fm.height() / 2, plot.x() - 6, fm.height()),
                   Qt::AlignRight | Qt::AlignVCenter, fmtY(y));
    }
    const double xStep = niceStep(xMax - xMin, qMax(2, int(plot.width() / 90)));
    for (double x = std::ceil(xMin / xStep) * xStep; x <= xMax + xStep * 0.01; x += xStep) {
        const qreal px = mapX(x);
        p.setPen(QPen(alpha(t.border, 70), 1.0));
        p.drawLine(QPointF(px, plot.y()), QPointF(px, plot.bottom()));
        p.setPen(t.textDim);
        p.drawText(QRectF(px - 45, plot.bottom() + 6, 90, fm.height()),
                   Qt::AlignHCenter, fmtX(x));
    }

    // Frame
    p.setPen(QPen(t.border, 1.0));
    p.setBrush(Qt::NoBrush);
    p.drawRect(plot.adjusted(0.5, 0.5, -0.5, -0.5));

    // Series
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setClipRect(plot.adjusted(0, -2, 0, 0));
    for (const Series& s : m_series) {
        if (s.points.size() < 2)
            continue;
        QPolygonF poly;
        poly.reserve(s.points.size());
        for (const QPointF& pt : s.points)
            poly.append(QPointF(mapX(pt.x()), mapY(pt.y())));

        if (m_fill) {
            QPainterPath area;
            area.addPolygon(poly);
            area.lineTo(poly.last().x(), plot.bottom());
            area.lineTo(poly.first().x(), plot.bottom());
            area.closeSubpath();
            QLinearGradient grad(plot.topLeft(), plot.bottomLeft());
            grad.setColorAt(0.0, alpha(s.color, 46));
            grad.setColorAt(1.0, alpha(s.color, 0));
            p.fillPath(area, grad);
        }
        // Subtle glow pass, then the crisp line
        p.setPen(QPen(alpha(s.color, 60), 3.2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        p.drawPolyline(poly);
        p.setPen(QPen(s.color, 1.4, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        p.drawPolyline(poly);
    }
    p.setClipping(false);

    // Legend
    if (m_legend) {
        int named = 0;
        for (const Series& s : m_series)
            if (!s.name.isEmpty())
                ++named;
        if (named > 0) {
            p.setFont(t.font(8.0));
            qreal x = plot.right() - 10;
            for (int i = m_series.size() - 1; i >= 0; --i) {
                const Series& s = m_series[i];
                if (s.name.isEmpty())
                    continue;
                const qreal w = fm.horizontalAdvance(s.name);
                x -= w;
                p.setPen(t.text);
                p.drawText(QPointF(x, plot.y() + 6 + fm.ascent()), s.name);
                x -= 16;
                p.setPen(QPen(s.color, 2.0));
                p.drawLine(QPointF(x, plot.y() + 6 + fm.height() / 2),
                           QPointF(x + 12, plot.y() + 6 + fm.height() / 2));
                x -= 18;
            }
        }
    }
}

} // namespace sxui
