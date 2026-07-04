#pragma once

#include <QPixmap>
#include <QRectF>
#include <QWidget>

class QVariantAnimation;

namespace sxui {

// Circular gauge: 270-degree arc with tick marks and a large value readout.
// Value changes animate. Optional warning/critical thresholds recolor the
// arc (amber/red).
class Gauge : public QWidget {
    Q_OBJECT
public:
    explicit Gauge(QWidget* parent = nullptr);

    void setRange(double min, double max);
    double value() const { return m_value; }
    void setLabel(const QString& label);
    void setUnit(const QString& unit);
    void setDecimals(int decimals);
    // Pass NaN to disable a threshold.
    void setThresholds(double warning, double critical);

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

public slots:
    void setValue(double value);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    QColor arcColor() const;
    // Shared geometry so the cached background and the live value arc align.
    QRectF arcGeometry(qreal& penWidth) const;
    // Static layer (track, ticks, unit, label) rasterized once per size; only
    // the value arc and readout are repainted per animation frame.
    void ensureBackground();
    void invalidateBackground() { m_bg = QPixmap(); }

    double m_min = 0.0, m_max = 100.0;
    double m_value = 0.0;      // target value
    double m_displayed = 0.0;  // animated value
    double m_warning, m_critical;
    QString m_label, m_unit;
    int m_decimals = 0;
    QVariantAnimation* m_anim;
    QPixmap m_bg;              // cached static layer, keyed on device-pixel size
};

} // namespace sxui
