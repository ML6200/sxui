#include "widgets/Gauge.h"
#include "core/Theme.h"

#include <QPainter>
#include <QVariantAnimation>
#include <cmath>

namespace sxui {

namespace {
constexpr int kStartAngle = 225 * 16; // QPainter angles: 1/16 deg, CCW from 3 o'clock
constexpr int kSpanAngle = -270 * 16;
} // namespace

Gauge::Gauge(QWidget* parent)
    : QWidget(parent)
    , m_warning(std::nan(""))
    , m_critical(std::nan(""))
    , m_anim(new QVariantAnimation(this))
{
    m_anim->setDuration(350);
    m_anim->setEasingCurve(QEasingCurve::OutCubic);
    connect(m_anim, &QVariantAnimation::valueChanged, this, [this](const QVariant& v) {
        m_displayed = v.toDouble();
        update();
    });
}

void Gauge::setRange(double min, double max)
{
    m_min = min;
    m_max = qMax(max, min + 1e-9);
    update();
}

void Gauge::setLabel(const QString& label) { m_label = label; invalidateBackground(); update(); }
void Gauge::setUnit(const QString& unit) { m_unit = unit; invalidateBackground(); update(); }
void Gauge::setDecimals(int decimals) { m_decimals = qMax(0, decimals); update(); }

void Gauge::setThresholds(double warning, double critical)
{
    m_warning = warning;
    m_critical = critical;
    update();
}

void Gauge::setValue(double value)
{
    m_value = qBound(m_min, value, m_max);
    m_anim->stop();
    m_anim->setStartValue(m_displayed);
    m_anim->setEndValue(m_value);
    m_anim->start();
}

QSize Gauge::sizeHint() const { return { 170, 170 }; }
QSize Gauge::minimumSizeHint() const { return { 90, 90 }; }

QColor Gauge::arcColor() const
{
    const Theme& t = Theme::current();
    if (!std::isnan(m_critical) && m_displayed >= m_critical)
        return t.danger;
    if (!std::isnan(m_warning) && m_displayed >= m_warning)
        return t.warning;
    return t.primary;
}

QRectF Gauge::arcGeometry(qreal& penWidth) const
{
    const qreal side = qMin(width(), height());
    penWidth = qMax(3.0, side * 0.055);
    QRectF arcRect((width() - side) / 2.0, (height() - side) / 2.0, side, side);
    arcRect.adjust(penWidth, penWidth, -penWidth, -penWidth);
    return arcRect;
}

// Rasterize the parts that never change during a value animation — the track
// arc, tick marks, unit and label text — into a size-keyed pixmap. The value
// arc and readout paint over this each frame.
void Gauge::ensureBackground()
{
    const qreal dpr = devicePixelRatioF();
    const QSize px = size() * dpr;
    if (!m_bg.isNull() && m_bg.size() == px)
        return;

    m_bg = QPixmap(px);
    m_bg.setDevicePixelRatio(dpr);
    m_bg.fill(Qt::transparent);

    const Theme& t = Theme::current();
    QPainter p(&m_bg);
    p.setRenderHint(QPainter::Antialiasing, true);

    qreal penW;
    const QRectF arcRect = arcGeometry(penW);
    const qreal side = qMin(width(), height());

    // Track
    p.setPen(QPen(alpha(t.border, 170), penW, Qt::SolidLine, Qt::FlatCap));
    p.drawArc(arcRect, kStartAngle, kSpanAngle);

    // Ticks at 0/25/50/75/100 %
    const QPointF center = arcRect.center();
    const qreal rOuter = arcRect.width() / 2.0 + penW * 0.4;
    const qreal rInner = rOuter - penW * 1.6;
    p.setPen(QPen(t.textDim, 1.0));
    for (int i = 0; i <= 4; ++i) {
        const double a = (225.0 - 270.0 * i / 4.0) * M_PI / 180.0;
        const QPointF dir(std::cos(a), -std::sin(a));
        p.drawLine(center + dir * rInner, center + dir * rOuter);
    }

    p.setFont(t.font(side * 0.065));
    p.setPen(t.textDim);
    if (!m_unit.isEmpty()) {
        QRectF unitRect = arcRect.adjusted(0, side * 0.14, 0, side * 0.14);
        p.drawText(unitRect, Qt::AlignCenter, m_unit);
    }
    if (!m_label.isEmpty()) {
        p.setFont(t.headingFont(side * 0.06, QFont::Normal));
        p.drawText(QRectF(0, height() - side * 0.14, width(), side * 0.12),
                   Qt::AlignHCenter | Qt::AlignVCenter, m_label);
    }
}

void Gauge::paintEvent(QPaintEvent*)
{
    ensureBackground();

    const Theme& t = Theme::current();
    QPainter p(this);
    p.drawPixmap(0, 0, m_bg);
    p.setRenderHint(QPainter::Antialiasing, true);

    qreal penW;
    const QRectF arcRect = arcGeometry(penW);
    const qreal side = qMin(width(), height());

    const double frac = qBound(0.0, (m_displayed - m_min) / (m_max - m_min), 1.0);
    const QColor c = arcColor();

    // Value arc with a soft glow pass underneath
    const int span = int(kSpanAngle * frac);
    if (frac > 0.001) {
        p.setPen(QPen(alpha(c, 70), penW * 2.2, Qt::SolidLine, Qt::FlatCap));
        p.drawArc(arcRect, kStartAngle, span);
        p.setPen(QPen(c, penW, Qt::SolidLine, Qt::FlatCap));
        p.drawArc(arcRect, kStartAngle, span);
    }

    // Readout
    const QString valueText = QString::number(m_displayed, 'f', m_decimals);
    p.setFont(t.font(side * 0.16, QFont::DemiBold));
    p.setPen(t.textBright);
    QRectF valueRect = arcRect.adjusted(0, -side * 0.04, 0, -side * 0.04);
    p.drawText(valueRect, Qt::AlignCenter, valueText);
}

} // namespace sxui
