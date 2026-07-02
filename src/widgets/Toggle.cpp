#include "widgets/Toggle.h"
#include "core/Theme.h"

#include <QPainter>
#include <QVariantAnimation>

namespace sxui {

Toggle::Toggle(QWidget* parent)
    : QAbstractButton(parent)
    , m_anim(new QVariantAnimation(this))
{
    setCheckable(true);
    setCursor(Qt::PointingHandCursor);
    setFocusPolicy(Qt::TabFocus);

    m_anim->setDuration(120);
    m_anim->setEasingCurve(QEasingCurve::InOutQuad);
    connect(m_anim, &QVariantAnimation::valueChanged, this, [this](const QVariant& v) {
        m_pos = v.toReal();
        update();
    });
}

QSize Toggle::sizeHint() const
{
    return { 46, 24 };
}

void Toggle::checkStateSet()
{
    animateTo(isChecked() ? 1.0 : 0.0);
}

void Toggle::nextCheckState()
{
    QAbstractButton::nextCheckState();
    animateTo(isChecked() ? 1.0 : 0.0);
}

void Toggle::animateTo(qreal target)
{
    m_anim->stop();
    m_anim->setStartValue(m_pos);
    m_anim->setEndValue(target);
    m_anim->start();
}

void Toggle::paintEvent(QPaintEvent*)
{
    const Theme& t = Theme::current();
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    const qreal trackW = 42, trackH = 20;
    const QRectF track(QPointF((width() - trackW) / 2.0, (height() - trackH) / 2.0),
                       QSizeF(trackW, trackH));

    const bool on = m_pos > 0.5;
    QColor border = on ? t.primary : t.borderBright;
    QColor fill = on ? alpha(t.primary, 40) : t.surface;
    if (!isEnabled()) {
        border = t.border;
        fill = t.surface;
    }

    p.setPen(QPen(border, 1.0));
    p.setBrush(fill);
    p.drawRoundedRect(track.adjusted(0.5, 0.5, -0.5, -0.5), 2, 2);

    // Knob
    const qreal knobS = trackH - 7;
    const qreal x0 = track.x() + 3.5;
    const qreal x1 = track.right() - knobS - 3.5;
    const QRectF knob(x0 + (x1 - x0) * m_pos, track.y() + 3.5, knobS, knobS);
    QColor knobColor = on ? t.primary : t.textDim;
    if (!isEnabled())
        knobColor = t.border;
    p.setPen(Qt::NoPen);
    p.setBrush(knobColor);
    p.drawRect(knob);

    if (hasFocus()) {
        p.setPen(QPen(alpha(t.primary, 110), 1.0));
        p.setBrush(Qt::NoBrush);
        p.drawRoundedRect(track.adjusted(-2, -2, 2, 2), 3, 3);
    }
}

} // namespace sxui
