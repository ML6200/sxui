#pragma once

#include <QDateTime>
#include <QElapsedTimer>
#include <QImage>
#include <QWidget>

namespace sxui {

// Video display surface. Deliberately decoder-agnostic: any source
// (RTSP/FFmpeg pipeline, file playback, test generator) pushes frames
// through presentFrame() — from the GUI thread directly or from a worker
// via a queued connection. Draws letterboxed video with a phosphor OSD:
// timestamp, camera name, LIVE/PLAYBACK badge, resolution and measured
// FPS. Without frames it shows a NO SIGNAL state.
class VideoView : public QWidget {
    Q_OBJECT
public:
    explicit VideoView(QWidget* parent = nullptr);

    void setCameraName(const QString& name);
    QString cameraName() const { return m_name; }

    // Live shows a bright LIVE badge; otherwise PLAYBACK (dim).
    void setLive(bool live);

    // Bright border, e.g. the selected cell in a camera grid.
    void setActive(bool active);

    QSize sizeHint() const override { return { 480, 270 }; }
    QSize minimumSizeHint() const override { return { 160, 90 }; }

public slots:
    // Invalid timestamp = now. Ownership of the image data is implicitly
    // shared; the source may reuse its buffer after the call.
    void presentFrame(const QImage& frame, const QDateTime& timestamp = QDateTime());
    void clear();

signals:
    void doubleClicked();

protected:
    void paintEvent(QPaintEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;

private:
    QImage m_frame;
    QString m_name;
    QDateTime m_timestamp;
    bool m_live = true;
    bool m_active = false;

    QElapsedTimer m_fpsClock;
    int m_framesSinceMark = 0;
    double m_fps = 0.0;
};

} // namespace sxui
