#include "PanicPage.h"
#include <LilyGoLib.h>

static const char* COLORS[]     = { "ROUGE", "BLEU", "VERT", "JAUNE", "ORANGE" };
static const uint32_t COLOR_HEX[] = { 0xFF3333, 0x3399FF, 0x33CC66, 0xFFDD00, 0xFF8800 };
static const uint8_t  COLOR_COUNT = 5;

PanicPage::PanicPage(PageManager* pm) : pm(pm) {}

void PanicPage::stopBuzz(lv_timer_t* t) {
    watch.stop();
    lv_timer_del(t);
}

void PanicPage::showStep(uint8_t s) {
    lv_label_set_text(labelSub, "");

    switch (s) {
    case 0:
        lv_label_set_text(labelMain, "STOP.\nASSIS.");
        lv_obj_set_style_text_color(labelMain, lv_color_hex(0xFFFFFF), 0);
        break;
    case 1: {
        uint8_t c = random(COLOR_COUNT);
        char buf[40];
        snprintf(buf, sizeof(buf), "COMPTE\n5 OBJETS");
        lv_label_set_text(labelMain, buf);
        lv_obj_set_style_text_color(labelMain, lv_color_hex(0xFFFFFF), 0);
        lv_label_set_text(labelSub, COLORS[c]);
        lv_obj_set_style_text_color(labelSub, lv_color_hex(COLOR_HEX[c]), 0);
        break;
    }
    case 2:
        lv_label_set_text(labelMain, "4 RESPIRATIONS\nPROFONDES");
        lv_obj_set_style_text_color(labelMain, lv_color_hex(0xFFFFFF), 0);
        break;
    }

    char stepBuf[8];
    snprintf(stepBuf, sizeof(stepBuf), "%d / 3", s + 1);
    lv_label_set_text(labelStep, stepBuf);
}

void PanicPage::onTap(lv_event_t* e) {
    if (lv_event_get_code(e) != LV_EVENT_CLICKED) return;
    PanicPage* self = (PanicPage*)lv_event_get_user_data(e);

    self->step++;
    if (self->step >= 3) {
        self->step = 0;
        self->pm->goTo(0, LV_SCR_LOAD_ANIM_FADE_ON);
    } else {
        self->showStep(self->step);
    }
}

void PanicPage::build() {
    _screen = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(_screen, lv_color_hex(0x0A0A1A), 0);
    lv_obj_add_event_cb(_screen, onTap, LV_EVENT_CLICKED, (void*)this);

    labelMain = lv_label_create(_screen);
    lv_obj_set_style_text_font(labelMain, &lv_font_montserrat_28, 0);
    lv_obj_set_style_text_color(labelMain, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_align(labelMain, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_long_mode(labelMain, LV_LABEL_LONG_WRAP);
    lv_obj_set_width(labelMain, 200);
    lv_obj_align(labelMain, LV_ALIGN_CENTER, 0, -20);

    labelSub = lv_label_create(_screen);
    lv_obj_set_style_text_font(labelSub, &lv_font_montserrat_36, 0);
    lv_obj_set_style_text_align(labelSub, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(labelSub, "");
    lv_obj_align(labelSub, LV_ALIGN_CENTER, 0, 45);

    labelStep = lv_label_create(_screen);
    lv_obj_set_style_text_color(labelStep, lv_color_hex(0x444444), 0);
    lv_label_set_text(labelStep, "");
    lv_obj_align(labelStep, LV_ALIGN_BOTTOM_MID, 0, -8);
}

void PanicPage::onEnter() {
    step = 0;
    showStep(0);

    // Vibrate 2s: effect 47 = long buzz, stop via one-shot timer
    watch.setWaveform(0, 47);
    watch.setWaveform(1, 0);
    watch.run();
    timerBuzz = lv_timer_create(stopBuzz, 2000, nullptr);
    lv_timer_set_repeat_count(timerBuzz, 1);
}

void PanicPage::onExit() {
    watch.stop();
    if (timerBuzz) {
        lv_timer_del(timerBuzz);
        timerBuzz = nullptr;
    }
}
