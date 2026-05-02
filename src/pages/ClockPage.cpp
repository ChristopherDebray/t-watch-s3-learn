#include <LilyGoLib.h>
#include "ClockPage.h"

// ─── Constructor ─────────────────────────────────────────────────────────────

ClockPage::ClockPage(PageManager* pm) : pm(pm) {
    hh = conv2d(__TIME__);
    mm = conv2d(__TIME__ + 3);
    ss = conv2d(__TIME__ + 6);
}

// ─── Helpers ─────────────────────────────────────────────────────────────────

uint8_t ClockPage::conv2d(const char* p) {
    uint8_t v = 0;
    if ('0' <= *p && *p <= '9') v = *p - '0';
    return 10 * v + *++p - '0';
}

// Timer callback statique : LVGL ne peut pas appeler une méthode d'instance
// directement, donc on passe le pointeur `this` dans user_data
void ClockPage::timerCb(lv_timer_t* t) {
    ClockPage* self = (ClockPage*)t->user_data;

    self->ss++;
    if (self->ss == 60) { self->ss = 0; self->mm++; }
    if (self->mm == 60) { self->mm = 0; self->hh++; }
    if (self->hh == 24) { self->hh = 0; }

    char buf[12];
    snprintf(buf, sizeof(buf), "%02d:%02d:%02d", self->hh, self->mm, self->ss);
    lv_label_set_text(self->labelTime, buf);
}

// ─── Build ───────────────────────────────────────────────────────────────────

void ClockPage::build() {
    _screen = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(_screen, lv_color_black(), 0);

    // Label heure
    labelTime = lv_label_create(_screen);
    lv_obj_set_style_text_color(labelTime, lv_color_hex(0xFFE000), 0);
    lv_obj_set_style_text_font(labelTime, &lv_font_montserrat_36, 0);
    lv_label_set_text(labelTime, "00:00:00");
    lv_obj_align(labelTime, LV_ALIGN_CENTER, 0, -10);

    // Label titre
    lv_obj_t* title = lv_label_create(_screen);
    lv_obj_set_style_text_color(title, lv_color_hex(0x444444), 0);
    lv_label_set_text(title, "CLOCK");
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 12);

    // Bouton nav → page suivante
    lv_obj_t* btn = lv_btn_create(_screen);
    lv_obj_set_size(btn, 130, 38);
    lv_obj_align(btn, LV_ALIGN_BOTTOM_MID, 0, -10);
    lv_obj_set_style_bg_color(btn, lv_color_hex(0x222222), 0);

    // On passe `this` en user_data du bouton pour y accéder dans le callback
    lv_obj_set_user_data(btn, (void*)pm);
    lv_obj_add_event_cb(btn, [](lv_event_t* e) {
        if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
            PageManager* pm = (PageManager*)lv_obj_get_user_data(lv_event_get_target(e));
            pm->next();
        }
    }, LV_EVENT_CLICKED, NULL);

    lv_obj_t* btnLabel = lv_label_create(btn);
    lv_label_set_text(btnLabel, "Info  >");
    lv_obj_center(btnLabel);

    // Timer : 1 tick par seconde, user_data = this
    timerClock = lv_timer_create(timerCb, 1000, (void*)this);
    lv_timer_pause(timerClock);  // pause jusqu'au onEnter
}

// ─── Lifecycle ───────────────────────────────────────────────────────────────

void ClockPage::onEnter() {
    lv_timer_resume(timerClock);
}

void ClockPage::onExit() {
    lv_timer_pause(timerClock);
}