#pragma once

#include <QScrollArea>

namespace sxui {

// QScrollArea with touch scrolling wired up. Plain QScrollArea only follows
// the wheel and the scrollbars, which leaves touch screens (phones, the wasm
// build) with nothing to drag but the bars. This grabs the viewport's touch
// gesture so drag/flick scrolls; taps still reach child widgets, and mouse
// behavior is unchanged. Comes frameless and widget-resizable, the way every
// sxui app was configuring it by hand.
class ScrollArea : public QScrollArea {
    Q_OBJECT
public:
    explicit ScrollArea(QWidget* parent = nullptr);
};

} // namespace sxui
