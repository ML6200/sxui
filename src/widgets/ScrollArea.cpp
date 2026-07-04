#include "ScrollArea.h"

#include <QScroller>
#include <QScrollerProperties>

namespace sxui {

ScrollArea::ScrollArea(QWidget* parent)
    : QScrollArea(parent)
{
    setFrameShape(QFrame::NoFrame);
    setWidgetResizable(true);

    // TouchGesture, not LeftMouseButtonGesture: a mouse grab would hijack
    // drags meant for child widgets (sliders, chart scrubbing) on desktop.
    // Touch-only means QScroller engages exclusively where it's needed.
    QScroller::grabGesture(viewport(), QScroller::TouchGesture);

    // No rubber-band overshoot: the phosphor panels bouncing past the edge
    // reads as a glitch rather than an affordance.
    QScroller* scroller = QScroller::scroller(viewport());
    QScrollerProperties props = scroller->scrollerProperties();
    props.setScrollMetric(
        QScrollerProperties::VerticalOvershootPolicy,
        QVariant::fromValue(QScrollerProperties::OvershootAlwaysOff));
    props.setScrollMetric(
        QScrollerProperties::HorizontalOvershootPolicy,
        QVariant::fromValue(QScrollerProperties::OvershootAlwaysOff));
    scroller->setScrollerProperties(props);
}

} // namespace sxui
