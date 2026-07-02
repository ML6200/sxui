#pragma once

// Umbrella header for the sxui phosphor UI framework.

#include "sxui/Theme.h"
#include "sxui/Style.h"
#include "sxui/Panel.h"
#include "sxui/Toggle.h"
#include "sxui/Gauge.h"
#include "sxui/LineChart.h"
#include "sxui/BarChart.h"
#include "sxui/FileBrowser.h"
#include "sxui/ImageView.h"

class QApplication;

namespace sxui {

// Installs the phosphor style, palette and monospace font application-wide.
// Call once, right after constructing QApplication.
void init(QApplication& app);

} // namespace sxui
