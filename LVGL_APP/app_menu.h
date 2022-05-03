#ifndef _APP_MENU_H_
#define _APP_MENU_H_

#include "lvgl.h"

typedef enum
{
	ICON_ID_COMPASS = 0,
	ICON_ID_SETTING,
	ICON_ID_ABOUT
}ICON_IDTypeDef;

typedef struct
{
	const void *img_buffer;		//图标图像数据
	lv_obj_t *img;			//图标img对象
	ICON_IDTypeDef id;	//图标id
	bool focus;					//图标当前是否被聚焦
}ICON_TypeDef;


void app_menu_create(void);
void app_menu_anim_Vexit(uint8_t id, bool dir);
void app_menu_anim_Hexit(uint8_t id, bool dir);


#endif
