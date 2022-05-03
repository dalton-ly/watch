#ifndef _APP_TIME_H_
#define _APP_TIME_H_

#include "lvgl.h"

//APP时间数据类型定义
typedef struct
{
	uint8_t Hour;
	uint8_t Minute;
	uint8_t Second;
} APP_TimeTypeDef;

//APP日期数据类型定义
typedef struct
{
	uint16_t year;
	uint8_t month;
	uint8_t date;
	uint8_t week;
} APP_DateTypeDef;

//APP时间显示类型定义
typedef struct
{
	lv_obj_t *img_time[4][2]; //时分
	lv_obj_t *img_Point;	  //点
	lv_obj_t *label_sec;	  //秒
	char sec_text[3];		  //秒文本

	lv_anim_t img_anim[4][2]; //动画
	bool dir[4];

	lv_obj_t *time_cont; //时间容器

} Time_ImgTypeDef;

//APP日期显示类型定义
typedef struct
{
	lv_obj_t *date_cont;			 //日期容器
	lv_obj_t *label_year_month_date; //显示日期的标签
	lv_obj_t *label_week;			 //显示星期的标签

	char year_month_date_text[11];
	char week_text[5];
} Date_ImgTypeDef;

typedef struct
{
	lv_obj_t *cont;			//电量容器
	lv_obj_t *bar;			//电量进度条
	lv_obj_t *label;		//电量文本标签
	char bat_value_text[5]; //电量文本内容
} Bat_TypeDef;

typedef struct
{
	lv_obj_t *cont;		//温度容器
	lv_obj_t *temperature_label;	//温度显示label
	char temperature_text[8];			//温度显示文本内容
}Temperature_TypeDef;

void app_show_start(void);
void start_img_anim(void);
void app_del_start(void);
void app_point_clock_create(void);
void app_digital_clock_create(void);
void app_init_time_data(APP_TimeTypeDef *app_time_t);
void app_update_time_data(APP_TimeTypeDef *app_time_t);
void app_update_date_data(APP_DateTypeDef *app_date_t);
void app_time_anim_start(uint8_t obj);
void app_time_point_state(bool state);

void app_update_time(APP_TimeTypeDef *app_time_t);
void app_update_date(APP_DateTypeDef *app_date_t);

void app_update_bat(uint8_t bat_value, uint8_t chg_state);
void app_temperature_update(int16_t T);

void app_time_anim_Hexit(bool dir);


#endif
