#include <LilyGoLib.h>
#include <LV_Helper.h>
#include <esp_sleep.h>

#include "PageManager.h"
#include "pages/ClockPage.h"
#include "pages/InfoPage.h"
#include "pages/PanicPage.h"

PageManager pm;
static int panicPageIndex = -1;

// Triple-click detection on crown button
static volatile bool pmuIrqFired = false;
static uint8_t  clickCount     = 0;
static uint32_t lastClickTime  = 0;
static const uint32_t TRIPLE_CLICK_WINDOW_MS = 800;

// Deep sleep inactivity tracking
static uint32_t lastActivityMs = 0;
static const uint32_t SLEEP_TIMEOUT_MS = 6000;

void resetActivity() { lastActivityMs = millis(); }

void onPMUIrq() { pmuIrqFired = true; }

void onTripleClick() {
    if (panicPageIndex >= 0) {
        pm.goTo(panicPageIndex, LV_SCR_LOAD_ANIM_FADE_ON);
    }
}

void handleCrownButton() {
    if (!pmuIrqFired) return;
    pmuIrqFired = false;

    uint64_t irq = watch.readPMU();
    watch.clearPMU();

    if (!(irq & XPOWERS_AXP2101_PKEY_SHORT_IRQ)) return;

    resetActivity();

    uint32_t now = millis();
    if (now - lastClickTime > TRIPLE_CLICK_WINDOW_MS) {
        clickCount = 0;
    }
    lastClickTime = now;
    clickCount++;

    if (clickCount >= 3) {
        clickCount = 0;
        onTripleClick();
    }
}

void goToSleep() {
    watch.decrementBrightness(0);
    // Wake on crown button (PMU INT, active LOW) via ext0
    // Wake on screen touch (FT6336 INT, active LOW) via ext1
    esp_sleep_enable_ext0_wakeup((gpio_num_t)BOARD_PMU_INT, 0);
    esp_sleep_enable_ext1_wakeup(_BV(BOARD_TOUCH_INT), ESP_EXT1_WAKEUP_ALL_LOW);
    esp_deep_sleep_start();
}

void setup() {
    Serial.begin(115200);
    watch.begin();
    beginLvglHelper();

    watch.attachPMU(onPMUIrq);
    resetActivity();

    pm.addPage(new ClockPage(&pm));
    pm.addPage(new InfoPage(&pm));
    panicPageIndex = pm.getPageCount();
    pm.addPage(new PanicPage(&pm));

    pm.start();
}

void loop() {
    handleCrownButton();

    if (watch.getTouched()) resetActivity();

    bool onPanicPage = (pm.getCurrentIndex() == panicPageIndex);
    if (!onPanicPage && (millis() - lastActivityMs > SLEEP_TIMEOUT_MS)) {
        goToSleep();
    }

    lv_task_handler();
    delay(5);
}
