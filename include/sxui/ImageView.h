#pragma once

#include <QImage>
#include <QWidget>

namespace sxui {

// Image viewer with drag-pan, wheel-zoom (anchored at the cursor) and an
// optional phosphor mode that remaps the image through a green luminance
// ramp so photos sit on-theme. Double-click resets to fit.
class ImageView : public QWidget {
    Q_OBJECT
public:
    explicit ImageView(QWidget* parent = nullptr);

    bool load(const QString& path);
    void setImage(const QImage& image, const QString& name = QString());

    bool phosphor() const { return m_phosphor; }
    void setPhosphor(bool on);

    qreal zoom() const { return m_zoom; }

public slots:
    void zoomToFit();

signals:
    void zoomChanged(qreal zoom);

protected:
    void paintEvent(QPaintEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;

private:
    void rebuildDisplayImage();
    void setZoomAnchored(qreal zoom, const QPointF& anchor);
    qreal fitZoom() const;
    QPointF clampCenter(QPointF center) const;

    QImage m_source;
    QImage m_display;
    QString m_name;
    bool m_phosphor = false;
    bool m_fit = true;
    qreal m_zoom = 1.0;
    QPointF m_center;     // source-image point shown at the viewport center
    QPointF m_dragStart;
    QPointF m_dragCenter;
    bool m_dragging = false;
};

} // namespace sxui
