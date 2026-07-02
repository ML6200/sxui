#pragma once

#include <QWidget>
#include <QVector>
#include <functional>

namespace sxui {

// Multi-series line/area chart painted with QPainter. Axes auto-scale to
// the data by default ("nice" tick values); ranges can be pinned. Suited
// for live telemetry via appendPoint() with a rolling window.
class LineChart : public QWidget {
    Q_OBJECT
public:
    using Formatter = std::function<QString(double)>;

    explicit LineChart(QWidget* parent = nullptr);

    // Returns the series index. Invalid color = next color from the theme cycle.
    int addSeries(const QString& name = QString(), const QColor& color = QColor());
    void setSeriesData(int index, QVector<QPointF> points);
    // Appends and, if maxPoints > 0, drops oldest points beyond the window.
    void appendPoint(int index, const QPointF& point, int maxPoints = 0);
    void clearSeries();

    void setXRange(double min, double max);
    void setYRange(double min, double max);
    void setAutoXRange();
    void setAutoYRange();

    void setXFormatter(Formatter formatter);
    void setYFormatter(Formatter formatter);
    void setFillEnabled(bool enabled);
    void setLegendVisible(bool visible);

    QSize sizeHint() const override { return { 440, 240 }; }
    QSize minimumSizeHint() const override { return { 200, 120 }; }

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    struct Series {
        QString name;
        QColor color;
        QVector<QPointF> points;
    };

    void dataRange(double& xMin, double& xMax, double& yMin, double& yMax) const;

    QVector<Series> m_series;
    bool m_autoX = true, m_autoY = true;
    double m_xMin = 0, m_xMax = 1, m_yMin = 0, m_yMax = 1;
    bool m_fill = true;
    bool m_legend = true;
    Formatter m_xFmt, m_yFmt;
};

} // namespace sxui
