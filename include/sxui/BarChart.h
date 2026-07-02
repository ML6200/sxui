#pragma once

#include <QWidget>
#include <QStringList>
#include <QVector>

namespace sxui {

// Vertical bar chart: dim fill, bright outline, solid cap on top of each
// bar, value labels above. Y axis starts at zero.
class BarChart : public QWidget {
    Q_OBJECT
public:
    explicit BarChart(QWidget* parent = nullptr);

    void setData(const QStringList& categories, const QVector<double>& values);
    void setBarColor(const QColor& color);
    void setValueSuffix(const QString& suffix);

    QSize sizeHint() const override { return { 440, 220 }; }
    QSize minimumSizeHint() const override { return { 180, 120 }; }

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    QStringList m_categories;
    QVector<double> m_values;
    QColor m_color;
    QString m_suffix;
};

} // namespace sxui
