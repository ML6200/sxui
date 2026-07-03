#pragma once

#include <QDateTime>
#include <QVector>
#include <QWidget>

namespace sxui {

// Recording timeline: a zoomable time ruler with recorded segments,
// event markers and a scrubbable playhead.
//
// Designed around event-first navigation (the thing NVR clients get
// wrong): markers are first-class regardless of their source — camera
// motion events today, ML-derived tags later — and jumpToNextMarker()/
// jumpToPreviousMarker() (also PgDown/PgUp) step the playhead through
// them directly.
//
// Interaction: click or drag to seek (positionRequested), wheel to zoom
// the time scale around the cursor, arrow keys to nudge.
class Timeline : public QWidget {
    Q_OBJECT
public:
    enum MarkerKind { Motion, Alarm, Custom };
    struct Segment {
        QDateTime start;
        QDateTime end;
    };
    struct Marker {
        QDateTime time;
        MarkerKind kind = Motion;
    };

    explicit Timeline(QWidget* parent = nullptr);

    // Visible window. Zooming/panning changes it and emits rangeChanged.
    void setRange(const QDateTime& start, const QDateTime& end);
    void setSegments(const QVector<Segment>& segments);
    void setMarkers(const QVector<Marker>& markers);
    QDateTime playhead() const;

    QSize sizeHint() const override { return { 600, 66 }; }
    QSize minimumSizeHint() const override { return { 240, 48 }; }

public slots:
    void setPlayhead(const QDateTime& time);
    void jumpToNextMarker();
    void jumpToPreviousMarker();

signals:
    // User asked to seek (click, scrub, key, marker jump).
    void positionRequested(const QDateTime& time);
    void rangeChanged(const QDateTime& start, const QDateTime& end);

protected:
    void paintEvent(QPaintEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private:
    struct MarkerMs {
        qint64 ms;
        MarkerKind kind;
    };

    qreal xFromMs(qint64 ms) const;
    qint64 msFromX(qreal x) const;
    void seekTo(qint64 ms, bool ensureVisible = false);
    void setRangeMs(qint64 start, qint64 end);

    qint64 m_t0 = 0, m_t1 = 1; // visible window, msecs since epoch
    QVector<QPair<qint64, qint64>> m_segments;
    QVector<MarkerMs> m_markers; // sorted by time
    qint64 m_playhead = 0;
    qint64 m_hover = -1;
    bool m_scrubbing = false;
};

} // namespace sxui
