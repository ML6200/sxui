#include "sxui/core/Style.h"
#include "sxui/core/Theme.h"

#include <QPainter>
#include <QPainterPath>
#include <QStyleFactory>
#include <QStyleOption>
#include <QStyleOptionComplex>

namespace sxui {

namespace {

void drawBox(QPainter* p, const QRectF& rect, const QColor& fill,
             const QColor& border, qreal radius)
{
    p->save();
    p->setRenderHint(QPainter::Antialiasing, true);
    QPainterPath path;
    path.addRoundedRect(rect.adjusted(0.5, 0.5, -0.5, -0.5), radius, radius);
    if (fill.alpha() > 0)
        p->fillPath(path, fill);
    if (border.alpha() > 0) {
        p->setPen(QPen(border, 1.0));
        p->drawPath(path);
    }
    p->restore();
}

} // namespace

Style::Style()
    : QProxyStyle(QStyleFactory::create(QStringLiteral("Fusion")))
{
}

QPalette Style::standardPalette() const
{
    const Theme& t = Theme::current();
    QPalette pal;
    pal.setColor(QPalette::Window, t.background);
    pal.setColor(QPalette::WindowText, t.text);
    pal.setColor(QPalette::Base, t.surface);
    pal.setColor(QPalette::AlternateBase, QColor(0x09, 0x1c, 0x11));
    pal.setColor(QPalette::Text, t.text);
    pal.setColor(QPalette::Button, t.surface);
    pal.setColor(QPalette::ButtonText, t.text);
    pal.setColor(QPalette::BrightText, t.textBright);
    pal.setColor(QPalette::Highlight, alpha(t.primary, 56));
    pal.setColor(QPalette::HighlightedText, t.textBright);
    pal.setColor(QPalette::PlaceholderText, t.textDim);
    pal.setColor(QPalette::ToolTipBase, t.surfaceRaised);
    pal.setColor(QPalette::ToolTipText, t.text);
    pal.setColor(QPalette::Link, t.primary);
    pal.setColor(QPalette::Light, t.borderBright);
    pal.setColor(QPalette::Midlight, t.border);
    pal.setColor(QPalette::Mid, t.border);
    pal.setColor(QPalette::Dark, t.background);
    pal.setColor(QPalette::Shadow, QColor(0, 0, 0));

    pal.setColor(QPalette::Disabled, QPalette::WindowText, t.textDim);
    pal.setColor(QPalette::Disabled, QPalette::Text, t.textDim);
    pal.setColor(QPalette::Disabled, QPalette::ButtonText, t.textDim);
    pal.setColor(QPalette::Disabled, QPalette::Highlight, alpha(t.border, 120));
    pal.setColor(QPalette::Disabled, QPalette::HighlightedText, t.textDim);
    return pal;
}

void Style::drawPrimitive(PrimitiveElement element, const QStyleOption* option,
                          QPainter* p, const QWidget* widget) const
{
    const Theme& t = Theme::current();
    const bool enabled = option->state & State_Enabled;
    const bool hover = enabled && (option->state & State_MouseOver);
    const bool focus = option->state & State_HasFocus;

    switch (element) {
    case PE_PanelButtonCommand: {
        const bool pressed = option->state & (State_Sunken | State_On);
        QColor fill = t.surface;
        QColor border = enabled ? t.borderBright : t.border;
        if (pressed) {
            fill = t.primary;
            border = t.primary;
        } else if (hover) {
            fill = alpha(t.primary, 26);
            border = t.primary;
        } else if (focus) {
            border = t.primaryDim;
        }
        drawBox(p, option->rect, fill, border, t.radius);
        return;
    }
    case PE_PanelLineEdit: {
        QColor border = focus ? t.primary : (hover ? t.borderBright : t.border);
        if (!enabled)
            border = t.border;
        drawBox(p, option->rect, t.surface, border, t.radius);
        return;
    }
    case PE_IndicatorCheckBox: {
        const QRectF r = QRectF(option->rect).adjusted(1, 1, -1, -1);
        QColor border = (hover || focus) ? t.primary : t.borderBright;
        if (!enabled)
            border = t.border;
        drawBox(p, r, t.surface, border, 1);
        if (option->state & State_On) {
            const QRectF inner = r.adjusted(4, 4, -4, -4);
            p->fillRect(inner, enabled ? t.primary : t.textDim);
        } else if (option->state & State_NoChange) {
            QRectF inner = r.adjusted(4, 4, -4, -4);
            inner.setTop(inner.center().y() - 1.5);
            inner.setHeight(3);
            p->fillRect(inner, t.primaryDim);
        }
        return;
    }
    case PE_IndicatorRadioButton: {
        const QRectF r = QRectF(option->rect).adjusted(1.5, 1.5, -1.5, -1.5);
        p->save();
        p->setRenderHint(QPainter::Antialiasing, true);
        QColor border = (hover || focus) ? t.primary : t.borderBright;
        if (!enabled)
            border = t.border;
        p->setPen(QPen(border, 1.0));
        p->setBrush(t.surface);
        p->drawEllipse(r);
        if (option->state & State_On) {
            p->setPen(Qt::NoPen);
            p->setBrush(enabled ? t.primary : t.textDim);
            p->drawEllipse(r.adjusted(4, 4, -4, -4));
        }
        p->restore();
        return;
    }
    case PE_FrameFocusRect: {
        p->save();
        p->setPen(QPen(alpha(t.primary, 110), 1.0));
        p->setBrush(Qt::NoBrush);
        p->drawRect(QRectF(option->rect).adjusted(0.5, 0.5, -0.5, -0.5));
        p->restore();
        return;
    }
    case PE_PanelTipLabel: {
        drawBox(p, option->rect, t.surfaceRaised, t.primaryDim, t.radius);
        return;
    }
    case PE_Frame:
    case PE_FrameLineEdit: {
        p->save();
        p->setPen(QPen(t.border, 1.0));
        p->setBrush(Qt::NoBrush);
        p->drawRect(QRectF(option->rect).adjusted(0.5, 0.5, -0.5, -0.5));
        p->restore();
        return;
    }
    case PE_FrameTabWidget: {
        drawBox(p, option->rect, t.surface, t.border, 0);
        return;
    }
    default:
        break;
    }
    QProxyStyle::drawPrimitive(element, option, p, widget);
}

void Style::drawControl(ControlElement element, const QStyleOption* option,
                        QPainter* p, const QWidget* widget) const
{
    const Theme& t = Theme::current();

    switch (element) {
    case CE_PushButtonLabel: {
        if (const auto* btn = qstyleoption_cast<const QStyleOptionButton*>(option)) {
            QStyleOptionButton copy = *btn;
            if (btn->state & (State_Sunken | State_On))
                copy.palette.setColor(QPalette::ButtonText, t.background);
            QProxyStyle::drawControl(element, &copy, p, widget);
            return;
        }
        break;
    }
    case CE_ProgressBar: {
        if (const auto* pb = qstyleoption_cast<const QStyleOptionProgressBar*>(option)) {
            const QRectF r = QRectF(pb->rect);
            drawBox(p, r, t.surface, t.border, t.radius);

            const QRectF inner = r.adjusted(4, 4, -4, -4);
            const qreal blockW = 7.0;
            const qreal gap = 3.0;
            const int n = qMax(1, int((inner.width() + gap) / (blockW + gap)));
            const qreal range = qMax(1, pb->maximum - pb->minimum);
            const qreal frac = qBound(0.0, (pb->progress - pb->minimum) / range, 1.0);
            const int lit = qRound(frac * n);

            p->save();
            p->setPen(Qt::NoPen);
            for (int i = 0; i < n; ++i) {
                const QRectF block(inner.x() + i * (blockW + gap), inner.y(),
                                   blockW, inner.height());
                p->fillRect(block, i < lit ? t.primary : alpha(t.border, 90));
            }
            p->restore();
            return;
        }
        break;
    }
    case CE_TabBarTabShape: {
        if (const auto* tab = qstyleoption_cast<const QStyleOptionTab*>(option)) {
            const bool selected = tab->state & State_Selected;
            const QRectF r = QRectF(tab->rect).adjusted(0.5, 0.5, -0.5, -0.5);
            p->save();
            p->fillRect(r, selected ? t.surface : t.background);
            p->setPen(QPen(t.border, 1.0));
            p->drawRect(r);
            if (selected) {
                p->setPen(QPen(t.primary, 2.0));
                p->drawLine(r.topLeft() + QPointF(1, 0.5), r.topRight() + QPointF(-1, 0.5));
            }
            p->restore();
            return;
        }
        break;
    }
    case CE_TabBarTabLabel: {
        if (const auto* tab = qstyleoption_cast<const QStyleOptionTab*>(option)) {
            QStyleOptionTab copy = *tab;
            const bool selected = tab->state & State_Selected;
            copy.palette.setColor(QPalette::WindowText,
                                  selected ? t.textBright : t.textDim);
            QProxyStyle::drawControl(element, &copy, p, widget);
            return;
        }
        break;
    }
    case CE_HeaderSection: {
        p->save();
        p->fillRect(option->rect, t.surfaceRaised);
        p->setPen(QPen(t.border, 1.0));
        const QRectF r = QRectF(option->rect);
        p->drawLine(r.bottomLeft(), r.bottomRight());
        p->drawLine(r.topRight(), r.bottomRight());
        p->restore();
        return;
    }
    default:
        break;
    }
    QProxyStyle::drawControl(element, option, p, widget);
}

void Style::drawComplexControl(ComplexControl control, const QStyleOptionComplex* option,
                               QPainter* p, const QWidget* widget) const
{
    const Theme& t = Theme::current();
    const bool enabled = option->state & State_Enabled;
    const bool hover = enabled && (option->state & State_MouseOver);

    switch (control) {
    case CC_Slider: {
        if (const auto* so = qstyleoption_cast<const QStyleOptionSlider*>(option)) {
            const QRect groove = proxy()->subControlRect(CC_Slider, so, SC_SliderGroove, widget);
            const QRect handle = proxy()->subControlRect(CC_Slider, so, SC_SliderHandle, widget);
            const bool pressed = so->activeSubControls & SC_SliderHandle
                                 && (so->state & State_Sunken);

            p->save();
            p->setPen(Qt::NoPen);
            if (so->orientation == Qt::Horizontal) {
                QRectF track(groove.x(), groove.center().y() - 1, groove.width(), 2);
                p->fillRect(track, alpha(t.border, 200));
                QRectF fill = track;
                fill.setRight(handle.center().x());
                p->fillRect(fill, enabled ? t.primaryDim : t.border);
            } else {
                QRectF track(groove.center().x() - 1, groove.y(), 2, groove.height());
                p->fillRect(track, alpha(t.border, 200));
                QRectF fill = track;
                fill.setTop(handle.center().y());
                p->fillRect(fill, enabled ? t.primaryDim : t.border);
            }
            QRectF h = QRectF(handle).adjusted(2, 2, -2, -2);
            QColor border = (hover || pressed) ? t.primary : t.borderBright;
            if (!enabled)
                border = t.border;
            drawBox(p, h, pressed ? t.primary : t.surfaceRaised, border, 1);
            p->restore();
            return;
        }
        break;
    }
    case CC_ScrollBar: {
        if (const auto* sb = qstyleoption_cast<const QStyleOptionSlider*>(option)) {
            const QRect slider = proxy()->subControlRect(CC_ScrollBar, sb, SC_ScrollBarSlider, widget);
            p->save();
            p->fillRect(sb->rect, alpha(t.surface, 160));
            const bool active = sb->activeSubControls & SC_ScrollBarSlider;
            QColor c = t.border;
            if (active && (sb->state & State_Sunken))
                c = t.primary;
            else if (active && hover)
                c = t.primaryDim;
            else if (hover)
                c = t.borderBright;
            const QRectF h = (sb->orientation == Qt::Horizontal)
                                 ? QRectF(slider).adjusted(1, 3, -1, -3)
                                 : QRectF(slider).adjusted(3, 1, -3, -1);
            p->fillRect(h, c);
            p->restore();
            return;
        }
        break;
    }
    case CC_ComboBox: {
        if (const auto* cb = qstyleoption_cast<const QStyleOptionComboBox*>(option)) {
            const bool open = cb->state & State_On;
            QColor border = t.borderBright;
            QColor fill = t.surface;
            if (!enabled)
                border = t.border;
            else if (open || (cb->state & State_HasFocus))
                border = t.primary;
            else if (hover)
                border = t.primary;
            if (hover && enabled)
                fill = alpha(t.primary, 20);
            drawBox(p, cb->rect, fill, border, t.radius);

            // Arrow
            const QRect arrowRect = proxy()->subControlRect(CC_ComboBox, cb, SC_ComboBoxArrow, widget);
            p->save();
            p->setRenderHint(QPainter::Antialiasing, true);
            const QPointF c = QRectF(arrowRect).center();
            const qreal s = 3.5;
            QPainterPath arrow;
            arrow.moveTo(c + QPointF(-s, -s * 0.5));
            arrow.lineTo(c + QPointF(0, s * 0.7));
            arrow.lineTo(c + QPointF(s, -s * 0.5));
            p->setPen(QPen(enabled ? t.primaryDim : t.textDim, 1.4));
            p->setBrush(Qt::NoBrush);
            p->drawPath(arrow);
            p->restore();
            return;
        }
        break;
    }
    default:
        break;
    }
    QProxyStyle::drawComplexControl(control, option, p, widget);
}

QRect Style::subControlRect(ComplexControl control, const QStyleOptionComplex* option,
                            SubControl subControl, const QWidget* widget) const
{
    if (control == CC_ScrollBar) {
        if (const auto* sb = qstyleoption_cast<const QStyleOptionSlider*>(option)) {
            const QRect r = sb->rect;
            const bool horiz = sb->orientation == Qt::Horizontal;
            const int len = horiz ? r.width() : r.height();
            const int range = sb->maximum - sb->minimum;
            int handleLen = len;
            if (range + sb->pageStep > 0)
                handleLen = qBound(24, int(qint64(len) * sb->pageStep / (range + sb->pageStep)), len);
            const int pos = sliderPositionFromValue(sb->minimum, sb->maximum,
                                                    sb->sliderPosition,
                                                    len - handleLen, sb->upsideDown);
            switch (subControl) {
            case SC_ScrollBarAddLine:
            case SC_ScrollBarSubLine:
                return QRect();
            case SC_ScrollBarGroove:
                return r;
            case SC_ScrollBarSlider:
                return horiz ? QRect(r.x() + pos, r.y(), handleLen, r.height())
                             : QRect(r.x(), r.y() + pos, r.width(), handleLen);
            case SC_ScrollBarSubPage:
                return horiz ? QRect(r.x(), r.y(), pos, r.height())
                             : QRect(r.x(), r.y(), r.width(), pos);
            case SC_ScrollBarAddPage:
                return horiz ? QRect(r.x() + pos + handleLen, r.y(),
                                     len - pos - handleLen, r.height())
                             : QRect(r.x(), r.y() + pos + handleLen,
                                     r.width(), len - pos - handleLen);
            default:
                break;
            }
        }
    }
    return QProxyStyle::subControlRect(control, option, subControl, widget);
}

int Style::pixelMetric(PixelMetric metric, const QStyleOption* option,
                       const QWidget* widget) const
{
    switch (metric) {
    case PM_ScrollBarExtent:
        return 12;
    case PM_IndicatorWidth:
    case PM_IndicatorHeight:
    case PM_ExclusiveIndicatorWidth:
    case PM_ExclusiveIndicatorHeight:
        return 17;
    case PM_SliderThickness:
    case PM_SliderControlThickness:
        return 22;
    case PM_SliderLength:
        return 14;
    case PM_ButtonMargin:
        return 12;
    default:
        break;
    }
    return QProxyStyle::pixelMetric(metric, option, widget);
}

int Style::styleHint(StyleHint hint, const QStyleOption* option, const QWidget* widget,
                     QStyleHintReturn* returnData) const
{
    switch (hint) {
    case SH_EtchDisabledText:
    case SH_DitherDisabledText:
    case SH_UnderlineShortcut:
        return 0;
    default:
        break;
    }
    return QProxyStyle::styleHint(hint, option, widget, returnData);
}

} // namespace sxui
