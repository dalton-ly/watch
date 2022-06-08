#ifndef _APP_HUMIDITY_H_
#define _APP_HUMIDITY_H_
#include"BME280.h"

#include "lvgl.h"


void app_humidity_create(void);
void app_humidity_anim_Vexit(bool dir);
void app_update_humidity(struct bme280_data* bmedata);	
#endif