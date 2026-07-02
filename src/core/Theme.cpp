#include "core/Theme.h"

#include <QFontDatabase>

namespace sxui {

Theme& Theme::current()
{
    static Theme instance;
    return instance;
}

QString Theme::fontFamily() const
{
    if (!m_family.isEmpty())
        return m_family;

    static const char* candidates[] = {
        "JetBrains Mono", "IBM Plex Mono", "SF Mono", "Cascadia Code",
        "Menlo", "DejaVu Sans Mono", "Consolas", "Liberation Mono",
    };
    const QStringList families = QFontDatabase::families();
    for (const char* candidate : candidates) {
        if (families.contains(QLatin1String(candidate))) {
            m_family = QLatin1String(candidate);
            return m_family;
        }
    }
    m_family = QStringLiteral("Monospace");
    return m_family;
}

void Theme::setFontFamily(const QString& family)
{
    m_family = family;
}

QFont Theme::font(qreal pointSize, int weight) const
{
    QFont f(fontFamily());
    f.setPointSizeF(pointSize);
    f.setWeight(QFont::Weight(weight));
    f.setStyleHint(QFont::Monospace);
    return f;
}

QFont Theme::headingFont(qreal pointSize, int weight) const
{
    QFont f = font(pointSize, weight);
    f.setLetterSpacing(QFont::PercentageSpacing, 112);
    f.setCapitalization(QFont::AllUppercase);
    return f;
}

QColor alpha(const QColor& c, int a)
{
    QColor out = c;
    out.setAlpha(a);
    return out;
}

} // namespace sxui
