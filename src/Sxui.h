#pragma once

// Umbrella header for the sxui phosphor UI framework.

#include "core/Theme.h"
#include "core/Style.h"
#include "widgets/Panel.h"
#include "widgets/Toggle.h"
#include "widgets/Gauge.h"
#include "charts/LineChart.h"
#include "charts/BarChart.h"
#include "widgets/FileBrowser.h"
#include "widgets/ImageView.h"
#include "widgets/TableEditor.h"
#include "widgets/Workspace.h"

class QApplication;

namespace sxui {

// Installs the phosphor style, palette and monospace font application-wide.
// Call once, right after constructing QApplication.
void init(QApplication& app);

} // namespace sxui
