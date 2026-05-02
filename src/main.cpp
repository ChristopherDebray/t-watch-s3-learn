#include <LilyGoLib.h>
#include <LV_Helper.h>

#include "PageManager.h"
#include "pages/ClockPage.h"
#include "pages/InfoPage.h"

PageManager pm;

void setup() {
    Serial.begin(115200);
    watch.begin();
    beginLvglHelper();

    pm.addPage(new ClockPage(&pm));
    pm.addPage(new InfoPage(&pm));

    pm.start();
}

void loop() {
    lv_task_handler();
    delay(5);
}
