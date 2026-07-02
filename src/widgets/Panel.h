#pragma once

#include <QWidget>

namespace sxui {

// Bordered container with an uppercase title header, in the style of the
// section boxes on the Silo terminals ("TUNNEL", "SECTION-8", ...).
// Give it a layout as with any QWidget; contents margins leave room for
// the header.
class Panel : public QWidget {
    Q_OBJECT
public:
    explicit Panel(const QString& title = QString(), QWidget* parent = nullptr);

    QString title() const { return m_title; }
    void setTitle(const QString& title);

    // Short status text drawn at the right edge of the header,
    // e.g. "ONLINE" or "CLASSIFIED". Invalid color = dim text.
    void setStatus(const QString& text, const QColor& color = QColor());

    int headerHeight() const { return m_headerHeight; }

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    QString m_title;
    QString m_status;
    QColor m_statusColor;
    int m_headerHeight = 28;
};

} // namespace sxui
