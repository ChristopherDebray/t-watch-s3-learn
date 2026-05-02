#pragma once
#include "../PageManager.h"

class ClockPage : public LvglPage {
public:
    ClockPage(PageManager* pm);
    void      build()   override;
    lv_obj_t* screen()  override { return _screen; }
    void      onEnter() override;
    void      onExit()  override;

private:
    PageManager* pm;
    lv_obj_t*    _screen    = nullptr;
    lv_obj_t*    labelTime  = nullptr;
    lv_timer_t*  timerClock = nullptr;

    uint8_t hh, mm, ss;

    static uint8_t conv2d(const char* p);
    static void    timerCb(lv_timer_t* t);  // callback statique → appelle la méthode instance
};