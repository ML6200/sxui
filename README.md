# sxui

Pure-C++ Qt Widgets UI framework with a phosphor-green terminal aesthetic
(inspired by the displays in *Silo*). Intended as the shared UI base for
desktop, embedded (Raspberry Pi) and WebAssembly targets. No QML, no
JavaScript engine — everything is QPainter and plain Qt Widgets.

## Layout

- `include/sxui/`, `src/` — the static `sxui` library
  - `Theme` — design tokens (colors, fonts, metrics); single source of truth
    for the palette, swappable at `Theme::current()`
  - `Style` — `QProxyStyle` over Fusion that restyles the *standard* Qt
    widgets (buttons, checkboxes, radios, sliders, scrollbars, progress
    bars, line edits, combo boxes, tabs, headers), so plain Qt code picks
    up the theme with zero sxui-specific classes
  - Custom widgets: `Panel` (titled section frame), `Toggle` (animated
    switch), `Gauge` (270° dial with thresholds + animation)
  - Charts: `LineChart` (multi-series, auto-scaling, live append with
    rolling window, area fill + subtle glow), `BarChart`
- `apps/gallery/` — component gallery / visual regression app

## Usage

```cpp
#include <sxui/Sxui.h>

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    sxui::init(app);   // installs style, palette, monospace font
    // ... any Qt Widgets code is now themed; sxui widgets add the extras
}
```

## Build

```sh
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
./build/apps/gallery/sxui-gallery
```

The gallery doubles as a verification tool:

```sh
./build/apps/gallery/sxui-gallery --screenshot out.png --page 1
```

## Targets

- **Desktop (macOS/Linux/Windows)** — works out of the box with any Qt 6.5+.
- **Raspberry Pi** — build against distro Qt 6; for a dedicated panel run
  fullscreen on KMS/DRM without a windowing system:
  `QT_QPA_PLATFORM=eglfs ./yourapp` (or `linuxfb` without GPU).
- **WebAssembly** — everything is QPainter-based, so it runs under Qt for
  WebAssembly unchanged. Requires the wasm Qt kit (not available via
  Homebrew): install Emscripten (version matching the Qt release) and a
  `wasm_singlethread` Qt build (e.g. via `aqtinstall`), then
  `qt-cmake` from that kit against this same CMakeLists.

## Design rules

- All colors come from `sxui::Theme` tokens — never hardcode a QColor in
  widget code.
- Clean phosphor theming only: no scanline/CRT artifacts; glow is limited
  to a subtle wide-stroke underpass on chart lines and gauge arcs.
- Monospace everywhere; headings are uppercase and letter-spaced
  (`Theme::headingFont`).
- Licensing: only LGPL Qt modules (`Qt6::Widgets`). Qt Charts / Qt Graphs
  are GPL and deliberately not used — charts are painted in-framework.
