#pragma once

// Umbrella header for the sxui phosphor UI framework.

#include "sxui/core/Theme.h"
#include "sxui/core/Style.h"
#include "sxui/widgets/Panel.h"
#include "sxui/widgets/Toggle.h"
#include "sxui/widgets/Gauge.h"
#include "sxui/charts/LineChart.h"
#include "sxui/charts/BarChart.h"
#include "sxui/widgets/FileBrowser.h"
#include "sxui/widgets/ImageView.h"

class QApplication;

namespace sxui {

// Installs the phosphor style, palette and monospace font application-wide.
// Call once, right after constructing QApplication.
void init(QApplication& app);

} // namespace sxui
