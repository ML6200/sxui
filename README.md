# sxui

Pure-C++ Qt Widgets UI framework with a phosphor-green terminal aesthetic
(inspired by the displays in *Silo*). Intended as the shared UI base for
desktop, embedded (Raspberry Pi) and WebAssembly targets. No QML, no
JavaScript engine — everything is QPainter and plain Qt Widgets.

## Layout

The library is organized by module; each class keeps its header and
source together, and headers are included as `<module/Class.h>` (or
everything at once via `<Sxui.h>`) — the `sxui::` namespace carries the
prefix.

```
src/                     the static sxui library
├── Sxui.h / Sxui.cpp    umbrella header + sxui::init()
├── core/
│   ├── Theme            design tokens (colors, fonts, metrics); single
│   │                    source of truth, swappable at Theme::current()
│   └── Style            QProxyStyle over Fusion restyling the standard
│                        Qt widgets (buttons, checkboxes, radios, sliders,
│                        scrollbars, progress bars, line edits, combo
│                        boxes, tabs, headers) — plain Qt code picks up
│                        the theme with zero sxui-specific classes
├── widgets/
│   ├── Panel            titled section frame with status chip
│   ├── Toggle           animated on/off switch
│   ├── Gauge            270° dial with thresholds + animated value
│   ├── FileBrowser      filesystem browser drawn as archive entry cards
│   │                    (outlined icon, name, amber date line, size);
│   │                    dirs descend on activation, files emit
│   │                    fileActivated()
│   └── ImageView        pan/zoom image viewer (cursor-anchored wheel
│                        zoom, drag pan, double-click to fit) with an
│                        optional phosphor luminance-ramp mode
├── charts/
│   ├── LineChart        multi-series, auto-scaling axes, live append
│   │                    with rolling window, area fill + subtle glow
│   └── BarChart         dim fill, bright cap, value labels
apps/gallery/            component gallery / visual regression app
                         (local only, not under version control)
docs/PLAN.md             roadmap and status
```

The `apps/` directory is gitignored; the build includes it only when
present, so the repository stands alone as the library.

## Usage

```cpp
#include <Sxui.h>

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
