#include "widgets/ImageView.h"
#include "core/Theme.h"

#include <QFileInfo>
#include <QImageReader>
#include <QMouseEvent>
#include <QPainter>
#include <QWheelEvent>
#include <cmath>

namespace sxui {

namespace {

// Luminance -> phosphor ramp: near-black green through primary to a pale
// bright green for highlights.
const QRgb* phosphorLut()
{
    static QRgb lut[256];
    static bool built = false;
    if (!built) {
        const Theme& t = Theme::current();
        struct Stop { double pos; QColor color; };
        const Stop stops[] = {
            { 0.00, QColor(3, 10, 7) },
            { 0.55, t.primaryDim },
            { 0.85, t.primary },
            { 1.00, t.textBright },
        };
        for (int i = 0; i < 256; ++i) {
            const double v = i / 255.0;
            int s = 0;
            while (s < 2 && v > stops[s + 1].pos)
                ++s;
            const Stop& a = stops[s];
            const Stop& b = stops[s + 1];
            const double f = (v - a.pos) / (b.pos - a.pos);
            lut[i] = qRgb(int(a.color.red() + (b.color.red() - a.color.red()) * f),
                          int(a.color.green() + (b.color.green() - a.color.green()) * f),
                          int(a.color.blue() + (b.color.blue() - a.color.blue()) * f));
        }
        built = true;
    }
    return lut;
}

} // namespace

ImageView::ImageView(QWidget* parent)
    : QWidget(parent)
{
    setMinimumSize(160, 120);
    setCursor(Qt::OpenHandCursor);
}

bool ImageView::load(const QString& path)
{
    QImageReader reader(path);
    reader.setAutoTransform(true);
    const QImage img = reader.read();
    if (img.isNull())
        return false;
    setImage(img, QFileInfo(path).fileName());
    return true;
}

void ImageView::setImage(const QImage& image, const QString& name)
{
    m_source = image.convertToFormat(QImage::Format_ARGB32);
    m_name = name;
    m_fit = true;
    rebuildDisplayImage();
    update();
}

void ImageView::setPhosphor(bool on)
{
    if (m_phosphor == on)
        return;
    m_phosphor = on;
    rebuildDisplayImage();
    update();
}

void ImageView::rebuildDisplayImage()
{
    if (m_source.isNull()) {
        m_display = QImage();
        return;
    }
    if (!m_phosphor) {
        m_display = m_source;
        return;
    }
    const QRgb* lut = phosphorLut();
    m_display = QImage(m_source.size(), QImage::Format_ARGB32);
    for (int y = 0; y < m_source.height(); ++y) {
        const QRgb* src = reinterpret_cast<const QRgb*>(m_source.constScanLine(y));
        QRgb* dst = reinterpret_cast<QRgb*>(m_display.scanLine(y));
        for (int x = 0; x < m_source.width(); ++x) {
            const QRgb mapped = lut[qGray(src[x])];
            dst[x] = qRgba(qRed(mapped), qGreen(mapped), qBlue(mapped), qAlpha(src[x]));
        }
    }
}

qreal ImageView::fitZoom() const
{
    if (m_source.isNull())
        return 1.0;
    return qMin(qreal(width()) / m_source.width(),
                qreal(height() - 24) / m_source.height());
}

void ImageView::zoomToFit()
{
    m_fit = true;
    update();
    emit zoomChanged(fitZoom());
}

QPointF ImageView::clampCenter(QPointF center) const
{
    center.setX(qBound(0.0, center.x(), qreal(m_source.width())));
    center.setY(qBound(0.0, center.y(), qreal(m_source.height())));
    return center;
}

void ImageView::setZoomAnchored(qreal zoom, const QPointF& anchor)
{
    if (m_source.isNull())
        return;
    if (m_fit) {
        m_zoom = fitZoom();
        m_center = QPointF(m_source.width() / 2.0, m_source.height() / 2.0);
        m_fit = false;
    }
    zoom = qBound(0.02, zoom, 40.0);
    const QPointF widgetCenter(width() / 2.0, height() / 2.0);
    const QPointF imgAtAnchor = (anchor - widgetCenter) / m_zoom + m_center;
    m_zoom = zoom;
    m_center = clampCenter(imgAtAnchor - (anchor - widgetCenter) / m_zoom);
    update();
    emit zoomChanged(m_zoom);
}

void ImageView::paintEvent(QPaintEvent*)
{
    const Theme& t = Theme::current();
    QPainter p(this);
    p.fillRect(rect(), t.background);

    if (m_display.isNull()) {
        p.setPen(QPen(t.border, 1.0));
        p.drawRect(QRectF(rect()).adjusted(0.5, 0.5, -0.5, -0.5));
        p.setFont(t.headingFont(10.0));
        p.setPen(t.textDim);
        p.drawText(rect(), Qt::AlignCenter, QStringLiteral("NO IMAGE"));
        return;
    }

    const qreal zoom = m_fit ? fitZoom() : m_zoom;
    const QPointF center = m_fit
        ? QPointF(m_source.width() / 2.0, m_source.height() / 2.0)
        : m_center;

    p.save();
    p.setClipRect(rect().adjusted(1, 1, -1, -23));
    p.setRenderHint(QPainter::SmoothPixmapTransform, zoom < 1.0);
    p.translate(width() / 2.0, (height() - 22) / 2.0);
    p.scale(zoom, zoom);
    p.translate(-center);
    p.drawImage(0, 0, m_display);
    p.restore();

    // Info strip
    const QRectF strip(0, height() - 22, width(), 22);
    p.fillRect(strip, t.surface);
    p.setPen(QPen(t.border, 1.0));
    p.drawLine(strip.topLeft(), strip.topRight());
    p.setFont(t.font(8.0));
    p.setPen(t.textDim);
    const QString info = QStringLiteral("%1x%2  //  ZOOM %3%  %4")
                             .arg(m_source.width())
                             .arg(m_source.height())
                             .arg(qRound(zoom * 100))
                             .arg(m_phosphor ? QStringLiteral("//  PHOSPHOR") : QString());
    p.drawText(strip.adjusted(10, 0, -10, 0), Qt::AlignLeft | Qt::AlignVCenter,
               m_name.toUpper());
    p.drawText(strip.adjusted(10, 0, -10, 0), Qt::AlignRight | Qt::AlignVCenter, info);

    // Frame
    p.setPen(QPen(t.border, 1.0));
    p.setBrush(Qt::NoBrush);
    p.drawRect(QRectF(rect()).adjusted(0.5, 0.5, -0.5, -0.5));
}

void ImageView::wheelEvent(QWheelEvent* event)
{
    if (m_source.isNull())
        return;
    const qreal base = m_fit ? fitZoom() : m_zoom;
    const qreal factor = std::pow(1.0015, event->angleDelta().y());
    setZoomAnchored(base * factor, event->position());
}

void ImageView::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && !m_source.isNull()) {
        m_dragging = true;
        m_dragStart = event->position();
        if (m_fit) {
            m_zoom = fitZoom();
            m_center = QPointF(m_source.width() / 2.0, m_source.height() / 2.0);
            m_fit = false;
        }
        m_dragCenter = m_center;
        setCursor(Qt::ClosedHandCursor);
    }
}

void ImageView::mouseMoveEvent(QMouseEvent* event)
{
    if (m_dragging) {
        m_center = clampCenter(m_dragCenter - (event->position() - m_dragStart) / m_zoom);
        update();
    }
}

void ImageView::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragging = false;
        setCursor(Qt::OpenHandCursor);
    }
}

void ImageView::mouseDoubleClickEvent(QMouseEvent*)
{
    zoomToFit();
}

} // namespace sxui
