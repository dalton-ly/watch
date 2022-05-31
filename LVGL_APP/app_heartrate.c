#include "app_heartrate.h"
#include "app_anim.h"


static lv_obj_t *cont; //页面容器

static lv_obj_t *label_hr;  //显示心率
static const char *hr_text = "Heartrate:";
//创建关于界面
void app_heartrate_create(void)
{
	lv_obj_t *scr;

	scr = lv_scr_act(); //获取当前活跃的屏幕

	cont = lv_cont_create(scr, NULL);															//创建容器
	lv_obj_set_size(cont, 220, 220);															//设置容器大小
	lv_obj_align(cont, scr, LV_ALIGN_OUT_TOP_MID, 0, 0);										//设置容器在屏幕外部正上方
	lv_obj_set_style_local_bg_opa(cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP);		//设置背景透明
	lv_obj_set_style_local_border_opa(cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP); //设置边框透明

	lv_cont_set_layout(cont, LV_LAYOUT_OFF); //设置容器的布局方式: 关闭自动布局。根据需要设置是否自动布局,否则程序卡死

	label_hr = lv_label_create(cont, NULL);																   //创建标题标签
	lv_obj_set_style_local_text_font(label_hr, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_32); //设置字体20号
	lv_obj_set_style_local_text_color(label_hr, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_CYAN);		   //设置字体颜色
	lv_label_set_static_text(label_hr, hr_text);
	lv_obj_align(label_hr, NULL, LV_ALIGN_IN_TOP_MID, 0, 20);



}

//about垂直方向移动进入或退出中心
void app_heartrate_anim_Vexit(bool dir)
{
	uint16_t start;
	
	start = lv_obj_get_y(cont);		//获取起始值
	
	obj_add_anim(
								cont, 		//动画对象
								(lv_anim_exec_xcb_t)lv_obj_set_y,				//动画函数
								lv_anim_speed_to_time(100, 0, 42),			//动画速度
								start,																	//起始值
								start + (dir ? -220 : 220),							//结束值
								lv_anim_path_bounce											//动画特效:模拟弹性物体下落
								);
}
