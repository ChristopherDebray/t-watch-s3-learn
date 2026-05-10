# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

When doing a commit, never add the "co authored by claude".

## Build & Flash (PlatformIO)

```bash
# Build
pio run -e twatch-s3

# Upload to watch (ensure it's on, port /dev/ttyACM0)
pio run -e twatch-s3 -t upload

# Serial monitor (115200 baud)
pio device monitor

# Build + upload + monitor
pio run -e twatch-s3 -t upload && pio device monitor
```

If upload fails: hold the crown button for 1s until the COM port appears, then upload.

## Hardware

**Target:** LilyGO T-Watch S3 (ESP32-S3, 16MB flash, OPI PSRAM)  
**Display:** ST7789 240×240, driven via TFT_eSPI  
**UI framework:** LVGL 8.4.0  
**Power:** AXP2101 PMU (charging current capped at 125mA — do not exceed)  
**Radio:** SX1262 LoRa (pins defined in `src/utilities.h`)  
**Battery:** 502530 size (5×25×30mm)

Pin definitions for all peripherals (TFT, touch, I2C, radio, mic, GPS) live in `src/utilities.h` under `#elif defined(USING_TWATCH_S3)`.

## Architecture

### Startup flow (`src/main.cpp`)
1. `watch.begin()` — initializes all hardware via `LilyGoLib`
2. `beginLvglHelper()` — sets up LVGL display driver and touch input
3. Pages registered with `PageManager`, then `pm.start()` loads the first screen
4. `loop()` calls `lv_task_handler()` every 5ms — this drives all LVGL timers and events

### Page system (`src/PageManager.h`)
`LvglPage` is the interface all pages implement:
- `build()` — called once at registration; creates LVGL objects on a dedicated `lv_obj_t* screen`
- `onEnter()` / `onExit()` — lifecycle hooks, called on navigation
- `screen()` — returns the LVGL screen object

`PageManager` owns an array of up to 10 pages. `next()`/`prev()` wrap around with slide animations. Navigation buttons pass `PageManager*` via `lv_obj_set_user_data` so lambda callbacks can call `pm->next()` / `pm->prev()`.

### Adding a new page
1. Create `src/pages/MyPage.h` and `src/pages/MyPage.cpp` following the `ClockPage` pattern
2. Store `PageManager*` in the constructor for navigation
3. Use a `static void timerCb(lv_timer_t* t)` with `t->user_data = this` for periodic updates — LVGL callbacks cannot be instance methods
4. Pause timers in `build()`, resume in `onEnter()`, pause again in `onExit()`
5. Register in `main.cpp`: `pm.addPage(new MyPage(&pm))`

### Key libraries
- `LilyGoLib` — thin wrapper aggregating all peripheral drivers; exposes `watch` global
- `LV_Helper` — wires TFT_eSPI + touch driver into LVGL; call `beginLvglHelper()` once
- `XPowersLib` — AXP2101 PMU; use `watch.getBatteryPercent()` etc.
- `RadioLib` — LoRa via SX1262; most unused radio drivers excluded via `RADIOLIB_EXCLUDE_*` flags in `platformio.ini`

### TFT_eSPI configuration
Board-specific pin config is in `extras/Setup212_LilyGo_T_Watch_S3.h`, injected at compile time via `-include` build flag. Do not edit TFT_eSPI's own config files.

### USB CDC note
`-DARDUINO_USB_CDC_ON_BOOT=1` enables serial over USB but blocks startup until a terminal connects. Switch to `-UARDUINO_USB_CDC_ON_BOOT` for battery-powered use without USB.
