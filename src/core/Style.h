#pragma once

#include <QProxyStyle>

namespace sxui {

// Phosphor QStyle. Restyles the standard Qt widgets (buttons, checkboxes,
// sliders, scrollbars, progress bars, line edits, combo boxes, tabs, ...) so
// plain Qt code picks up the theme without using sxui widget classes.
class Style : public QProxyStyle {
    Q_OBJECT
public:
    Style();

    void drawPrimitive(PrimitiveElement element, const QStyleOption* option,
                       QPainter* painter, const QWidget* widget) const override;
    void drawControl(ControlElement element, const QStyleOption* option,
                     QPainter* painter, const QWidget* widget) const override;
    void drawComplexControl(ComplexControl control, const QStyleOptionComplex* option,
                            QPainter* painter, const QWidget* widget) const override;
    QRect subControlRect(ComplexControl control, const QStyleOptionComplex* option,
                         SubControl subControl, const QWidget* widget) const override;
    int pixelMetric(PixelMetric metric, const QStyleOption* option,
                    const QWidget* widget) const override;
    int styleHint(StyleHint hint, const QStyleOption* option, const QWidget* widget,
                  QStyleHintReturn* returnData) const override;
    QPalette standardPalette() const override;
    void polish(QWidget* widget) override;
    using QProxyStyle::polish;
};

} // namespace sxui
