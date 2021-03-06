#include "app_anim.h"


void obj_add_anim(lv_obj_t *obj, lv_anim_exec_xcb_t exec_cb, uint16_t time, lv_coord_t start, lv_coord_t end, lv_anim_path_cb_t path_cb)
{
	lv_anim_t a;

	lv_anim_init(&a);
	lv_anim_set_var(&a, obj); //动画对象

	lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)exec_cb); //动画函数
	lv_anim_set_time(&a, time);

	a.start = start;		  //起始值
	a.end = end; //结束值
	a.path.cb = path_cb;	  //动画计算方法
	a.playback_time = 0;				  //回放时间设为0不执行动画回放

	lv_anim_start(&a); //开启动画
}

//图片渐变动画函数
void img_anim_exec_xcb(void *obj, lv_anim_value_t value)
{
	lv_obj_set_style_local_image_opa(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, value); //图片透明度设置
}



