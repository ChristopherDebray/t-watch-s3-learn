#include <LilyGoLib.h>
#include <LV_Helper.h>

// ─── Écrans LVGL ────────────────────────────────────────────────────────────
static lv_obj_t *screenClock = NULL;
static lv_obj_t *screenInfo  = NULL;

// ─── Widgets Clock ──────────────────────────────────────────────────────────
static lv_obj_t *labelTime   = NULL;
static lv_obj_t *labelDate   = NULL;

// ─── Widgets Info ───────────────────────────────────────────────────────────
static lv_obj_t *labelUptime = NULL;

// ─── Timer handles ──────────────────────────────────────────────────────────
static lv_timer_t *timerClock  = NULL;
static lv_timer_t *timerInfo   = NULL;

// ─── Compteur de temps (simplifié, depuis compile time) ─────────────────────
static uint8_t hh, mm, ss;

static uint8_t conv2d(const char *p) {
    uint8_t v = 0;
    if ('0' <= *p && *p <= '9') v = *p - '0';
    return 10 * v + *++p - '0';
}

// ─── Navigation ─────────────────────────────────────────────────────────────
static void goToScreen(lv_obj_t *screen) {
    lv_scr_load_anim(screen, LV_SCR_LOAD_ANIM_MOVE_LEFT, 300, 0, false);
}

// ─── Callbacks boutons nav ──────────────────────────────────────────────────
static void btnNextCb(lv_event_t *e) {
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
        goToScreen(screenInfo);
    }
}

static void btnPrevCb(lv_event_t *e) {
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
        lv_scr_load_anim(screenClock, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 300, 0, false);
    }
}

// ─── Timer callbacks ────────────────────────────────────────────────────────
static void updateClock(lv_timer_t *timer) {
    ss++;
    if (ss == 60) { ss = 0; mm++; }
    if (mm == 60) { mm = 0; hh++; }
    if (hh == 24) { hh = 0; }

    char buf[16];
    snprintf(buf, sizeof(buf), "%02d:%02d:%02d", hh, mm, ss);
    lv_label_set_text(labelTime, buf);
}

static void updateInfo(lv_timer_t *timer) {
    uint32_t secs  = millis() / 1000;
    uint32_t mins  = secs / 60;
    uint32_t hours = mins / 60;
    secs %= 60; mins %= 60;

    char buf[32];
    snprintf(buf, sizeof(buf), "Uptime\n%02lu:%02lu:%02lu", hours, mins, secs);
    lv_label_set_text(labelUptime, buf);
}

// ─── Construction des écrans ────────────────────────────────────────────────
static void buildScreenClock() {
    screenClock = lv_obj_create(NULL);  // NULL = écran standalone
    lv_obj_set_style_bg_color(screenClock, lv_color_black(), 0);

    // Heure
    labelTime = lv_label_create(screenClock);
    lv_obj_set_style_text_color(labelTime, lv_color_hex(0xFFE000), 0); // jaune
    lv_obj_set_style_text_font(labelTime, &lv_font_montserrat_36, 0);
    lv_label_set_text(labelTime, "00:00:00");
    lv_obj_align(labelTime, LV_ALIGN_CENTER, 0, -20);

    // Label indicatif
    lv_obj_t *hint = lv_label_create(screenClock);
    lv_obj_set_style_text_color(hint, lv_color_hex(0x555555), 0);
    lv_label_set_text(hint, "Clock");
    lv_obj_align(hint, LV_ALIGN_TOP_MID, 0, 10);

    // Bouton nav → Info
    lv_obj_t *btn = lv_btn_create(screenClock);
    lv_obj_set_size(btn, 120, 36);
    lv_obj_align(btn, LV_ALIGN_BOTTOM_MID, 0, -10);
    lv_obj_set_style_bg_color(btn, lv_color_hex(0x222222), 0);
    lv_obj_add_event_cb(btn, btnNextCb, LV_EVENT_CLICKED, NULL);

    lv_obj_t *btnLabel = lv_label_create(btn);
    lv_label_set_text(btnLabel, "Info  >");
    lv_obj_center(btnLabel);
}

static void buildScreenInfo() {
    screenInfo = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(screenInfo, lv_color_black(), 0);

    // Label uptime
    labelUptime = lv_label_create(screenInfo);
    lv_obj_set_style_text_color(labelUptime, lv_color_hex(0x00FFCC), 0);
    lv_obj_set_style_text_font(labelUptime, &lv_font_montserrat_28, 0);
    lv_label_set_text(labelUptime, "Uptime\n00:00:00");
    lv_obj_set_style_text_align(labelUptime, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(labelUptime, LV_ALIGN_CENTER, 0, -20);

    // Label indicatif
    lv_obj_t *hint = lv_label_create(screenInfo);
    lv_obj_set_style_text_color(hint, lv_color_hex(0x555555), 0);
    lv_label_set_text(hint, "Info");
    lv_obj_align(hint, LV_ALIGN_TOP_MID, 0, 10);

    // Bouton nav ← Clock
    lv_obj_t *btn = lv_btn_create(screenInfo);
    lv_obj_set_size(btn, 120, 36);
    lv_obj_align(btn, LV_ALIGN_BOTTOM_MID, 0, -10);
    lv_obj_set_style_bg_color(btn, lv_color_hex(0x222222), 0);
    lv_obj_add_event_cb(btn, btnPrevCb, LV_EVENT_CLICKED, NULL);

    lv_obj_t *btnLabel = lv_label_create(btn);
    lv_label_set_text(btnLabel, "<  Clock");
    lv_obj_center(btnLabel);
}

// ─── Setup / Loop ────────────────────────────────────────────────────────────
void setup() {
    Serial.begin(115200);

    watch.begin();
    beginLvglHelper();

    // Init heure depuis compile time 
    hh = conv2d(__TIME__);
    mm = conv2d(__TIME__ + 3);
    ss = conv2d(__TIME__ + 6);

    // Construction des écrans
    buildScreenClock();
    buildScreenInfo();

    // Timers LVGL (pas de delay dans le loop !)
    timerClock = lv_timer_create(updateClock, 1000, NULL);
    timerInfo  = lv_timer_create(updateInfo,  1000, NULL);

    // Démarre sur l'horloge
    lv_scr_load(screenClock);
}

void loop() {
    lv_task_handler();
    delay(5);
}