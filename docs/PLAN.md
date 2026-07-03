# sxui — plan & status

Goal: a pure-C++ Qt Widgets UI framework (no QML/JS engine, LGPL Qt
modules only) with the *Silo* phosphor aesthetic, serving as the UI base
for the home-management system across desktop, Raspberry Pi and
WebAssembly.

## Done

### 0.1.0 — foundation (2026-07-02)
- `Theme`: token-based palette (surfaces, borders, phosphor greens, amber
  warning, red danger), monospace font resolution, heading fonts.
- `Style`: `QProxyStyle` over Fusion restyling standard Qt widgets —
  buttons, checkboxes, radios, sliders, arrowless thin scrollbars,
  segmented-block progress bars, line edits, combo boxes, tabs, headers,
  tooltips. One `sxui::init(app)` call themes plain Qt code.
- Widgets: `Panel`, `Toggle`, `Gauge`.
- Charts (custom QPainter, deliberately not GPL Qt Charts): `LineChart`
  (multi-series, nice-number auto axes, live rolling window, area fill,
  subtle glow), `BarChart`.
- `sxui-gallery` demo app with headless `--screenshot <path> --page <n>`
  for visual verification.

### 0.2.0 — files & media (2026-07-02)
- `FileBrowser`: `QFileSystemModel` + card delegate in the style of the
  show's recovered-drive archive listing (outlined amber icon, bright
  name, amber date, size right-aligned, `DIR` marker). Path bar + UP
  button, name filters, `fileActivated`/`pathChanged` signals.
- `ImageView`: drag pan, cursor-anchored wheel zoom (2%–4000%),
  double-click/`zoomToFit()`, info strip (name, dimensions, zoom,
  mode), optional phosphor luminance remap (LUT-based, cached).
- Gallery page 04 wiring both together: activating an image file in the
  browser opens it in the viewer.

### 0.3.0 — table editor (2026-07-03)
- `TableEditor`: editable data grid on QTableView. Internal
  variant-based model (`setColumns`/`setRows`/`appendRow`/`rows`) with
  an ADD ROW / REMOVE control strip and row-count readout, or any
  external `QAbstractItemModel` via `setModel()`. In-place editing
  emits `cellEdited(row, column, value)`; `setReadOnly()` for display
  use. Style gains SH_Table_GridLineColor so grids draw in the theme
  border color.
- Housekeeping: the gallery app was removed from version control and
  from history entirely — the repository tracks only the library; the
  gallery remains a local untracked verification tool.

### 0.4.0 — in-app windows & tabs (2026-07-03)
- `Workspace`: themed QMdiArea wrapper. Sub-windows added via
  `addWindow(content, title, size)` get phosphor title bars with painted
  min/max/restore/close glyphs, active/inactive states, draggable and
  resizable frames; tile/cascade and QMdiArea's tabbed view mode come
  along for free. Style implements CC_TitleBar and PE_FrameWindow.
- Tab polish: tab bars now use the heading typography (uppercase,
  letter-spaced) via Style::polish, with the bright active underline.

### 0.5.0 — media module (2026-07-03)
- `media/VideoView`: decoder-agnostic video display surface. Sources
  (future RTSP/FFmpeg pipeline, file playback, test generators) push
  frames via `presentFrame(QImage, timestamp)`; the widget handles
  letterboxing, OSD (timestamp, camera name, LIVE/PLAYBACK badge,
  resolution + measured FPS), NO SIGNAL state and an active border for
  grid selection. Stream decoding is deliberately out of scope — it
  belongs to a separate project that feeds this sink.
- `media/Timeline`: zoomable recording timeline addressing the
  motion-search pain of typical NVR clients: recorded segments, event
  markers as first-class citizens (Motion/Alarm/Custom — source
  agnostic, so camera events now and ML-derived tags later plug in the
  same way), click/scrub seeking, cursor-anchored wheel zoom, and
  prev/next-event navigation (PgUp/PgDn, `jumpTo*Marker()`).

## Next

1. **App shell** — fullscreen/windowed application frame with page
   navigation (header, footer status bar, nav rail) extracted from the
   gallery into the library. Basis for the Pi kiosk deployment
   (`QT_QPA_PLATFORM=eglfs`).
2. **WASM kit** — install Emscripten + `wasm_singlethread` Qt via
   `aqtinstall`, add a CI-able `qt-cmake` build; verify gallery in
   browser.
3. **More components as needed by the home system** — log/terminal view,
   notification toasts, modal dialogs, virtual keyboard (touch kiosk).
4. **Theme variants** — amber/red alternative palettes are already
   possible via `Theme::current()`; add presets.

## Related projects (not part of this library)

- **Stream engine**: RTSP client + FFmpeg decode (camera substream for
  grids, mainstream for single view), recording to disk with a segment/
  event index that Timeline can display; camera-side motion events via
  ONVIF/ISAPI subscriptions. Later: ML tagging of recordings (objects,
  faces, plates) emitted as Timeline markers to make archives searchable.

## Deferred / open

- **Window manager**: decision postponed. Current recommendation: no
  custom compositor — run the app fullscreen on KMS/DRM (eglfs) and do
  "window management" in-app via the app shell; if hosting third-party
  apps ever becomes a requirement, use an existing compositor (cage,
  labwc) rather than writing one.
- File browser extras: sorting options, multi-select, context actions.
- Chart extras: sparkline, time-axis label dedup helper, crosshair
  readout.
