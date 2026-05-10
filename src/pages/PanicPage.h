#pragma once
#include "../PageManager.h"

class PanicPage : public LvglPage {
public:
    PanicPage(PageManager* pm);
    void      build()   override;
    lv_obj_t* screen()  override { return _screen; }
    void      onEnter() override;
    void      onExit()  override;

private:
    PageManager* pm;
    lv_obj_t*    _screen    = nullptr;
    lv_obj_t*    labelMain  = nullptr;
    lv_obj_t*    labelSub   = nullptr;
    lv_obj_t*    labelStep  = nullptr;
    lv_timer_t*  timerBuzz  = nullptr;

    uint8_t step = 0;

    void showStep(uint8_t s);
    static void onTap(lv_event_t* e);
    static void stopBuzz(lv_timer_t* t);
};
