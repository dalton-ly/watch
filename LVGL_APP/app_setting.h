#ifndef _APP_SETTING_H_
#define _APP_SETTING_H_

#include "lvgl.h"
#include "pcf8563.h"

void app_setting_create(void);
void app_setting_change(uint8_t index);
void app_setting_change_roller(uint8_t i);
void app_setting_anim_Vexit(bool dir);
const uint8_t* app_setting_get_roller_max(void);
uint8_t app_setting_get_roller_index(void);
void app_setting_update_roller(RTC_Data_Type *rtc_t);
void app_setting_update_time(RTC_Data_Type *rtc_t);

#endif
