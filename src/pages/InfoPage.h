#pragma once
#include "../PageManager.h"

class InfoPage : public LvglPage {
public:
    InfoPage(PageManager* pm);
    void      build()   override;
    lv_obj_t* screen()  override { return _screen; }
    void      onEnter() override;
    void      onExit()  override;

private:
    PageManager* pm;
    lv_obj_t*    _screen     = nullptr;
    lv_obj_t*    labelUptime = nullptr;
    lv_obj_t*    labelBatt   = nullptr;  // ← nouveau
    lv_obj_t*    labelPage   = nullptr;
    lv_timer_t*  timerInfo   = nullptr;

    static void timerCb(lv_timer_t* t);
};