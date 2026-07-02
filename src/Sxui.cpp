#include "sxui/Sxui.h"

#include <QApplication>

namespace sxui {

void init(QApplication& app)
{
    auto* style = new Style(); // QApplication takes ownership
    app.setStyle(style);
    app.setPalette(style->standardPalette());
    app.setFont(Theme::current().font(Theme::current().baseFontSize));
}

} // namespace sxui
