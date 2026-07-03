#include "media/Timeline.h"
#include "core/Theme.h"

#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QWheelEvent>

#include <algorithm>
#include <cmath>

namespace sxui {

namespace {

constexpr qint64 kMinSpanMs = 10 * 1000;                    // 10 s
constexpr qint64 kMaxSpanMs = 60ll * 24 * 3600 * 1000;      // 60 days

// Ruler steps, seconds. Picked so adjacent labels stay readable.
constexpr qint64 kSteps[] = {
    1, 2, 5, 10, 15, 30,
    60, 120, 300, 600, 900, 1800,
    3600, 7200, 10800, 21600, 43200,
    86400, 172800, 604800,
};

QString tickLabel(qint64 ms, qint64 stepSec)
{
    const QDateTime dt = QDateTime::fromMSecsSinceEpoch(ms);
    if (stepSec < 60)
        return dt.toString(QStringLiteral("HH:mm:ss"));
    if (stepSec < 86400)
        return dt.toString(QStringLiteral("HH:mm"));
    return dt.toString(QStringLiteral("MMM dd")).toUpper();
}

} // namespace

Timeline::Timeline(QWidget* parent)
    : QWidget(parent)
{
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);
    const qint64 now = QDateTime::currentMSecsSinceEpoch();
    m_t0 = now - 3600 * 1000;
    m_t1 = now;
    m_playhead = now;
}

void Timeline::setRange(const QDateTime& start, const QDateTime& end)
{
    if (!start.isValid() || !end.isValid())
        return;
    setRangeMs(start.toMSecsSinceEpoch(), end.toMSecsSinceEpoch());
}

void Timeline::setRangeMs(qint64 start, qint64 end)
{
    if (end - start < kMinSpanMs)
        end = start + kMinSpanMs;
    if (end - start > kMaxSpanMs)
        end = start + kMaxSpanMs;
    if (start == m_t0 && end == m_t1)
        return;
    m_t0 = start;
    m_t1 = end;
    update();
    emit rangeChanged(QDateTime::fromMSecsSinceEpoch(m_t0),
                      QDateTime::fromMSecsSinceEpoch(m_t1));
}

void Timeline::setSegments(const QVector<Segment>& segments)
{
    m_segments.clear();
    m_segments.reserve(segments.size());
    for (const Segment& s : segments)
        if (s.start.isValid() && s.end.isValid())
            m_segments.append({ s.start.toMSecsSinceEpoch(), s.end.toMSecsSinceEpoch() });
    update();
}

void Timeline::setMarkers(const QVector<Marker>& markers)
{
    m_markers.clear();
    m_markers.reserve(markers.size());
    for (const Marker& m : markers)
        if (m.time.isValid())
            m_markers.append({ m.time.toMSecsSinceEpoch(), m.kind });
    std::sort(m_markers.begin(), m_markers.end(),
              [](const MarkerMs& a, const MarkerMs& b) { return a.ms < b.ms; });
    update();
}

QDateTime Timeline::playhead() const
{
    return QDateTime::fromMSecsSinceEpoch(m_playhead);
}

void Timeline::setPlayhead(const QDateTime& time)
{
    if (!time.isValid())
        return;
    m_playhead = time.toMSecsSinceEpoch();
    update();
}

void Timeline::seekTo(qint64 ms, bool ensureVisible)
{
    m_playhead = ms;
    if (ensureVisible && (ms < m_t0 || ms > m_t1)) {
        const qint64 span = m_t1 - m_t0;
        setRangeMs(ms - span / 2, ms + span / 2);
    }
    update();
    emit positionRequested(QDateTime::fromMSecsSinceEpoch(m_playhead));
}

void Timeline::jumpToNextMarker()
{
    for (const MarkerMs& m : m_markers) {
        if (m.ms > m_playhead) {
            seekTo(m.ms, true);
            return;
        }
    }
}

void Timeline::jumpToPreviousMarker()
{
    for (auto it = m_markers.crbegin(); it != m_markers.crend(); ++it) {
        if (it->ms < m_playhead) {
            seekTo(it->ms, true);
            return;
        }
    }
}

qreal Timeline::xFromMs(qint64 ms) const
{
    return (ms - m_t0) * qreal(width()) / qreal(m_t1 - m_t0);
}

qint64 Timeline::msFromX(qreal x) const
{
    return m_t0 + qint64(x / qreal(width()) * (m_t1 - m_t0));
}

void Timeline::paintEvent(QPaintEvent*)
{
    const Theme& t = Theme::current();
    QPainter p(this);
    p.fillRect(rect(), t.surface);

    const QFont labelFont = t.font(7.5);
    const QFontMetricsF fm(labelFont);
    const qreal rulerH = fm.height() + 6;
    const QRectF band(0, 14, width(), height() - 14 - rulerH);

    // Recorded segments
    p.setPen(Qt::NoPen);
    for (const auto& seg : m_segments) {
        const qreal x0 = qBound(0.0, xFromMs(seg.first), qreal(width()));
        const qreal x1 = qBound(0.0, xFromMs(seg.second), qreal(width()));
        if (x1 <= x0)
            continue;
        p.fillRect(QRectF(x0, band.y(), x1 - x0, band.height()), alpha(t.primaryDim, 70));
        p.fillRect(QRectF(x0, band.y(), x1 - x0, 2), alpha(t.primaryDim, 160));
    }

    // Ruler
    const qint64 spanSec = (m_t1 - m_t0) / 1000;
    qint64 stepSec = kSteps[std::size(kSteps) - 1];
    for (qint64 candidate : kSteps) {
        if (width() * candidate / qMax<qint64>(1, spanSec) >= 72) {
            stepSec = candidate;
            break;
        }
    }
    const qint64 stepMs = stepSec * 1000;
    p.setFont(labelFont);
    for (qint64 ms = (m_t0 / stepMs) * stepMs; ms <= m_t1; ms += stepMs) {
        if (ms < m_t0)
            continue;
        const qreal x = xFromMs(ms);
        p.setPen(QPen(alpha(t.border, 130), 1.0));
        p.drawLine(QPointF(x, band.y()), QPointF(x, band.bottom() + 4));
        p.setPen(t.textDim);
        p.drawText(QRectF(x - 50, band.bottom() + 4, 100, fm.height() + 2),
                   Qt::AlignHCenter | Qt::AlignVCenter, tickLabel(ms, stepSec));
    }

    // Markers
    for (const MarkerMs& m : m_markers) {
        if (m.ms < m_t0 || m.ms > m_t1)
            continue;
        const qreal x = xFromMs(m.ms);
        const QColor c = m.kind == Alarm ? t.danger
                       : m.kind == Custom ? t.warning
                                          : t.primary;
        p.setPen(QPen(c, 1.2));
        p.drawLine(QPointF(x, band.y()), QPointF(x, band.bottom()));
        // Flag on top
        p.setPen(Qt::NoPen);
        p.setBrush(c);
        QPolygonF flag;
        flag << QPointF(x - 3.5, band.y() - 8) << QPointF(x + 3.5, band.y() - 8)
             << QPointF(x, band.y() - 1);
        p.drawPolygon(flag);
    }

    // Hover cursor
    if (m_hover >= m_t0 && m_hover <= m_t1 && !m_scrubbing) {
        const qreal x = xFromMs(m_hover);
        p.setPen(QPen(alpha(t.textBright, 90), 1.0));
        p.drawLine(QPointF(x, band.y()), QPointF(x, band.bottom()));
    }

    // Playhead
    if (m_playhead >= m_t0 && m_playhead <= m_t1) {
        const qreal x = xFromMs(m_playhead);
        p.setPen(QPen(t.textBright, 1.5));
        p.drawLine(QPointF(x, 2), QPointF(x, band.bottom()));
        const QString label = QDateTime::fromMSecsSinceEpoch(m_playhead)
                                  .toString(QStringLiteral("HH:mm:ss"));
        p.setFont(t.font(7.5));
        const qreal w = fm.horizontalAdvance(label) + 10;
        const bool flip = x + 6 + w > width();
        const QRectF bubble(flip ? x - 6 - w : x + 6, 1, w, fm.height() + 3);
        p.fillRect(bubble, t.surfaceRaised);
        p.setPen(QPen(t.borderBright, 1.0));
        p.setBrush(Qt::NoBrush);
        p.drawRect(bubble.adjusted(0.5, 0.5, -0.5, -0.5));
        p.setPen(t.textBright);
        p.drawText(bubble, Qt::AlignCenter, label);
    }

    // Frame
    p.setPen(QPen(t.border, 1.0));
    p.setBrush(Qt::NoBrush);
    p.drawRect(QRectF(rect()).adjusted(0.5, 0.5, -0.5, -0.5));
}

void Timeline::wheelEvent(QWheelEvent* event)
{
    const qreal factor = std::pow(1.0015, -event->angleDelta().y());
    const qint64 span = m_t1 - m_t0;
    const qint64 newSpan = qBound(kMinSpanMs, qint64(span * factor), kMaxSpanMs);
    const qint64 anchor = msFromX(event->position().x());
    const qreal frac = (anchor - m_t0) / qreal(span);
    setRangeMs(anchor - qint64(newSpan * frac), anchor + qint64(newSpan * (1.0 - frac)));
}

void Timeline::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        m_scrubbing = true;
        seekTo(qBound(m_t0, msFromX(event->position().x()), m_t1));
    }
}

void Timeline::mouseMoveEvent(QMouseEvent* event)
{
    if (m_scrubbing) {
        seekTo(qBound(m_t0, msFromX(event->position().x()), m_t1));
    } else {
        m_hover = msFromX(event->position().x());
        update();
    }
}

void Timeline::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
        m_scrubbing = false;
}

void Timeline::leaveEvent(QEvent*)
{
    m_hover = -1;
    update();
}

void Timeline::keyPressEvent(QKeyEvent* event)
{
    const qint64 nudge = qMax<qint64>(1000, (m_t1 - m_t0) / 100);
    switch (event->key()) {
    case Qt::Key_Left:
        seekTo(m_playhead - nudge, true);
        break;
    case Qt::Key_Right:
        seekTo(m_playhead + nudge, true);
        break;
    case Qt::Key_PageUp:
        jumpToPreviousMarker();
        break;
    case Qt::Key_PageDown:
        jumpToNextMarker();
        break;
    default:
        QWidget::keyPressEvent(event);
    }
}

} // namespace sxui
