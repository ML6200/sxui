#include "media/VideoView.h"
#include "core/Theme.h"

#include <QMouseEvent>
#include <QPainter>

namespace sxui {

namespace {

// OSD text with a translucent backing strip for legibility over video.
void drawOsd(QPainter* p, const QPointF& corner, const QString& text,
             const QFont& font, const QColor& color, Qt::Alignment align)
{
    const QFontMetricsF fm(font);
    const QSizeF size(fm.horizontalAdvance(text) + 12, fm.height() + 4);
    QPointF topLeft = corner;
    if (align & Qt::AlignRight)
        topLeft.rx() -= size.width();
    if (align & Qt::AlignBottom)
        topLeft.ry() -= size.height();
    const QRectF r(topLeft, size);
    p->fillRect(r, QColor(0, 0, 0, 110));
    p->setFont(font);
    p->setPen(color);
    p->drawText(r, Qt::AlignCenter, text);
}

} // namespace

VideoView::VideoView(QWidget* parent)
    : QWidget(parent)
{
    m_fpsClock.start();
}

void VideoView::setCameraName(const QString& name)
{
    m_name = name;
    update();
}

void VideoView::setLive(bool live)
{
    m_live = live;
    update();
}

void VideoView::setActive(bool active)
{
    m_active = active;
    update();
}

void VideoView::presentFrame(const QImage& frame, const QDateTime& timestamp)
{
    m_frame = frame;
    m_timestamp = timestamp.isValid() ? timestamp : QDateTime::currentDateTime();

    ++m_framesSinceMark;
    const qint64 elapsed = m_fpsClock.elapsed();
    if (elapsed >= 1000) {
        m_fps = m_framesSinceMark * 1000.0 / elapsed;
        m_framesSinceMark = 0;
        m_fpsClock.restart();
    }
    update();
}

void VideoView::clear()
{
    m_frame = QImage();
    m_fps = 0.0;
    m_framesSinceMark = 0;
    update();
}

void VideoView::mouseDoubleClickEvent(QMouseEvent*)
{
    emit doubleClicked();
}

void VideoView::paintEvent(QPaintEvent*)
{
    const Theme& t = Theme::current();
    QPainter p(this);
    p.fillRect(rect(), t.background);

    const QRectF frameRect = QRectF(rect()).adjusted(1, 1, -1, -1);

    if (m_frame.isNull()) {
        p.setFont(t.headingFont(10.0));
        p.setPen(t.textDim);
        p.drawText(rect(), Qt::AlignCenter, QStringLiteral("NO SIGNAL"));
        if (!m_name.isEmpty())
            drawOsd(&p, frameRect.bottomLeft() + QPointF(6, -6), m_name,
                    t.headingFont(8.0), t.textDim, Qt::AlignLeft | Qt::AlignBottom);
    } else {
        // Letterboxed video
        QSizeF target = QSizeF(m_frame.size());
        target.scale(frameRect.size(), Qt::KeepAspectRatio);
        const QRectF video(frameRect.center() - QPointF(target.width() / 2, target.height() / 2),
                           target);
        p.setRenderHint(QPainter::SmoothPixmapTransform,
                        target.width() < m_frame.width());
        p.drawImage(video, m_frame);

        // OSD
        const QFont osd = t.font(8.0);
        drawOsd(&p, video.topLeft() + QPointF(6, 6),
                m_timestamp.toString(QStringLiteral("MM-dd-yyyy  HH:mm:ss")),
                osd, t.primary, Qt::AlignLeft | Qt::AlignTop);
        if (!m_name.isEmpty())
            drawOsd(&p, video.bottomLeft() + QPointF(6, -6), m_name,
                    t.headingFont(8.0), t.text, Qt::AlignLeft | Qt::AlignBottom);
        drawOsd(&p, video.topRight() + QPointF(-6, 6),
                m_live ? QStringLiteral("● LIVE") : QStringLiteral("PLAYBACK"),
                t.headingFont(7.5), m_live ? t.danger : t.textDim,
                Qt::AlignRight | Qt::AlignTop);
        const QString info = QStringLiteral("%1x%2 // %3 FPS")
                                 .arg(m_frame.width())
                                 .arg(m_frame.height())
                                 .arg(m_fps, 0, 'f', 0);
        drawOsd(&p, video.bottomRight() + QPointF(-6, -6), info,
                osd, t.textDim, Qt::AlignRight | Qt::AlignBottom);
    }

    // Frame border
    p.setRenderHint(QPainter::SmoothPixmapTransform, false);
    p.setPen(QPen(m_active ? t.primary : t.border, 1.0));
    p.setBrush(Qt::NoBrush);
    p.drawRect(QRectF(rect()).adjusted(0.5, 0.5, -0.5, -0.5));
}

} // namespace sxui
