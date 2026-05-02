#include "InfoPage.h"
#include <LilyGoLib.h>

// ─── Constructor ─────────────────────────────────────────────────────────────

InfoPage::InfoPage(PageManager* pm) : pm(pm) {}

// ─── Timer callback ──────────────────────────────────────────────────────────

void InfoPage::timerCb(lv_timer_t* t) {
    InfoPage* self = (InfoPage*)t->user_data;
    char buf[32];

    // Uptime
    uint32_t secs  = millis() / 1000;
    uint32_t mins  = secs / 60;
    uint32_t hours = mins / 60;
    secs %= 60; mins %= 60;
    snprintf(buf, sizeof(buf), "%02lu:%02lu:%02lu", hours, mins, secs);
    lv_label_set_text(self->labelUptime, buf);

    // Batterie
    int batt = watch.getBatteryPercent();
    if (batt < 0) {
        lv_label_set_text(self->labelBatt, "Batt: --");
    } else {
        snprintf(buf, sizeof(buf), "Batt: %d%%", batt);
        lv_label_set_text(self->labelBatt, buf);
        lv_color_t col = batt > 50 ? lv_color_hex(0x00FF00)
                       : batt > 20 ? lv_color_hex(0xFFAA00)
                       :             lv_color_hex(0xFF3333);
        lv_obj_set_style_text_color(self->labelBatt, col, 0);
    }

    // Page index
    snprintf(buf, sizeof(buf), "Page %d / %d",
             self->pm->getCurrentIndex() + 1,
             self->pm->getPageCount());
    lv_label_set_text(self->labelPage, buf);
}

// ─── Build ───────────────────────────────────────────────────────────────────

void InfoPage::build() {
    _screen = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(_screen, lv_color_black(), 0);

    // Titre
    lv_obj_t* title = lv_label_create(_screen);
    lv_obj_set_style_text_color(title, lv_color_hex(0x444444), 0);
    lv_label_set_text(title, "INFO");
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 12);

    // Uptime titre
    lv_obj_t* uptimeTitle = lv_label_create(_screen);
    lv_obj_set_style_text_color(uptimeTitle, lv_color_hex(0x888888), 0);
    lv_label_set_text(uptimeTitle, "Uptime");
    lv_obj_align(uptimeTitle, LV_ALIGN_CENTER, 0, -50);

    // Valeur uptime
    labelUptime = lv_label_create(_screen);
    lv_obj_set_style_text_color(labelUptime, lv_color_hex(0x00FFCC), 0);
    lv_obj_set_style_text_font(labelUptime, &lv_font_montserrat_28, 0);
    lv_label_set_text(labelUptime, "00:00:00");
    lv_obj_align(labelUptime, LV_ALIGN_CENTER, 0, -20);

    // Batterie
    labelBatt = lv_label_create(_screen);
    lv_obj_set_style_text_color(labelBatt, lv_color_hex(0x00FF00), 0);
    lv_obj_set_style_text_font(labelBatt, &lv_font_montserrat_20, 0);
    lv_label_set_text(labelBatt, "Batt: --%");
    lv_obj_align(labelBatt, LV_ALIGN_CENTER, 0, 15);

    // Page index
    labelPage = lv_label_create(_screen);
    lv_obj_set_style_text_color(labelPage, lv_color_hex(0x555555), 0);
    lv_label_set_text(labelPage, "Page - / -");
    lv_obj_align(labelPage, LV_ALIGN_CENTER, 0, 45);

    // Bouton nav ← Clock
    lv_obj_t* btn = lv_btn_create(_screen);
    lv_obj_set_size(btn, 130, 38);
    lv_obj_align(btn, LV_ALIGN_BOTTOM_MID, 0, -10);
    lv_obj_set_style_bg_color(btn, lv_color_hex(0x222222), 0);
    lv_obj_set_user_data(btn, (void*)pm);
    lv_obj_add_event_cb(btn, [](lv_event_t* e) {
        if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
            PageManager* pm = (PageManager*)lv_obj_get_user_data(lv_event_get_target(e));
            pm->prev();
        }
    }, LV_EVENT_CLICKED, NULL);

    lv_obj_t* btnLabel = lv_label_create(btn);
    lv_label_set_text(btnLabel, "<  Clock");
    lv_obj_center(btnLabel);

    // Timer refresh toutes les secondes
    timerInfo = lv_timer_create(timerCb, 1000, (void*)this);
    lv_timer_pause(timerInfo);
}

// ─── Lifecycle ───────────────────────────────────────────────────────────────

void InfoPage::onEnter() {
    lv_timer_resume(timerInfo);
    timerCb(timerInfo);  // update immédiat à l'entrée
}

void InfoPage::onExit() {
    lv_timer_pause(timerInfo);
}