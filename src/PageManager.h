#pragma once
#include <lvgl.h>

// Interface que chaque page doit implémenter
class LvglPage {
public:
    virtual void build()    = 0;  // construit les widgets LVGL
    virtual lv_obj_t* screen() = 0;  // retourne l'écran LVGL
    virtual void onEnter()  {}    // appelé quand on arrive (optionnel)
    virtual void onExit()   {}    // appelé quand on part (optionnel)
    virtual ~LvglPage() {}
};

class PageManager {
public:
    PageManager() : currentIndex(0), pageCount(0) {}

    void addPage(LvglPage* page) {
        if (pageCount < MAX_PAGES) {
            pages[pageCount++] = page;
            page->build();
        }
    }

    void goTo(int index, lv_scr_load_anim_t anim = LV_SCR_LOAD_ANIM_MOVE_LEFT) {
        if (index < 0 || index >= pageCount) return;
        pages[currentIndex]->onExit();
        currentIndex = index;
        pages[currentIndex]->onEnter();
        lv_scr_load_anim(pages[currentIndex]->screen(), anim, 300, 0, false);
    }

    void next() {
        goTo((currentIndex + 1) % pageCount, LV_SCR_LOAD_ANIM_MOVE_LEFT);
    }

    void prev() {
        goTo((currentIndex - 1 + pageCount) % pageCount, LV_SCR_LOAD_ANIM_MOVE_RIGHT);
    }

    void start() {
        if (pageCount > 0) {
            pages[0]->onEnter();
            lv_scr_load(pages[0]->screen());
        }
    }

    int getCurrentIndex() const { return currentIndex; }
    int getPageCount()    const { return pageCount; }

private:
    static const int MAX_PAGES = 10;
    LvglPage* pages[MAX_PAGES];
    int pageCount;
    int currentIndex;
};