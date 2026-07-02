#pragma once

#include <QColor>
#include <QFont>
#include <QString>

namespace sxui {

// Design tokens for the phosphor theme. All framework painting goes through
// Theme::current() so a project can swap the palette in one place.
class Theme {
public:
    // Surfaces, darkest to brightest
    QColor background    { 0x04, 0x0f, 0x09 };
    QColor surface       { 0x07, 0x17, 0x0e };
    QColor surfaceRaised { 0x0b, 0x21, 0x14 };

    // Lines and separators
    QColor border       { 0x1a, 0x4d, 0x30 };
    QColor borderBright { 0x2e, 0x8a, 0x55 };

    // Signal colors
    QColor primary    { 0x3f, 0xff, 0x9f };
    QColor primaryDim { 0x2b, 0xb8, 0x72 };
    QColor text       { 0x8d, 0xf5, 0xbd };
    QColor textBright { 0xd2, 0xff, 0xe9 };
    QColor textDim    { 0x41, 0x8f, 0x63 };
    QColor warning    { 0xff, 0xd2, 0x4a };
    QColor danger     { 0xff, 0x5d, 0x45 };

    // Metrics
    int radius = 2;
    int controlHeight = 28;
    qreal baseFontSize = 10.5;

    // Resolved monospace family (first available from a candidate list,
    // overridable via setFontFamily).
    QString fontFamily() const;
    void setFontFamily(const QString& family);

    QFont font(qreal pointSize, int weight = QFont::Normal) const;
    // Uppercase-intended, letter-spaced font for panel titles and headers.
    QFont headingFont(qreal pointSize, int weight = QFont::DemiBold) const;

    static Theme& current();

private:
    mutable QString m_family;
};

// Convenience: copy of a color with a different alpha (0-255).
QColor alpha(const QColor& c, int a);

} // namespace sxui
