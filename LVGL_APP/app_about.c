#include "app_about.h"
#include "app_anim.h"

LV_IMG_DECLARE(start);

static lv_obj_t *cont; //页面容器

static lv_obj_t *label_title;  //显示标题
static lv_obj_t *label_mcu;	   //显示mcu信息
static lv_obj_t *label_Date;	   //显示Date信息
static lv_obj_t *label_author; //显示作者信息
static lv_obj_t *logo_img;	   //logo

static const char *title_text = "About";
static const char *mcu_text = "MCU: STM32F411CEU6";
static const char *Date_text = "Date: 2022.5";
static const char *author_text = "AUTHOR: LiYang";

//创建关于界面
void app_about_create(void)
{
	lv_obj_t *scr;

	scr = lv_scr_act(); //获取当前活跃的屏幕

	cont = lv_cont_create(scr, NULL);															//创建容器
	lv_obj_set_size(cont, 220, 220);															//设置容器大小
	lv_obj_align(cont, scr, LV_ALIGN_OUT_TOP_MID, 0, 0);										//设置容器在屏幕外部正上方
	lv_obj_set_style_local_bg_opa(cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP);		//设置背景透明
	lv_obj_set_style_local_border_opa(cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP); //设置边框透明

	lv_cont_set_layout(cont, LV_LAYOUT_OFF); //设置容器的布局方式: 关闭自动布局。根据需要设置是否自动布局,否则程序卡死

	label_title = lv_label_create(cont, NULL);																   //创建标题标签
	lv_obj_set_style_local_text_font(label_title, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_32); //设置字体20号
	lv_obj_set_style_local_text_color(label_title, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_CYAN);		   //设置字体颜色
	lv_label_set_static_text(label_title, title_text);
	lv_obj_align(label_title, NULL, LV_ALIGN_IN_TOP_MID, 0, 10);

	label_mcu = lv_label_create(cont, NULL);														   //创建MCU标签
	lv_obj_set_style_local_text_color(label_mcu, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_YELLOW); //设置字体颜色
	lv_label_set_static_text(label_mcu, mcu_text);
	lv_obj_align(label_mcu, label_title, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

	label_Date = lv_label_create(cont, NULL);														   //创建Date标签
	lv_obj_set_style_local_text_color(label_Date, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_YELLOW); //设置字体颜色
	lv_label_set_static_text(label_Date, Date_text);
	lv_obj_align(label_Date, label_mcu, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);

	label_author = lv_label_create(cont, NULL);															  //创建AUTHOR标签
	lv_obj_set_style_local_text_color(label_author, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_YELLOW); //设置字体颜色
	lv_label_set_static_text(label_author, author_text);
	lv_obj_align(label_author, label_Date, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);

	logo_img = lv_img_create(cont, NULL); //创建logo
	lv_img_set_src(logo_img, &start);	  //logo显示图片
	lv_obj_align(logo_img, cont, LV_ALIGN_IN_BOTTOM_MID, 0, 0);
	lv_obj_set_style_local_image_opa(logo_img, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);
	lv_img_set_auto_size(logo_img, false); //不使能大小自动适配
}

//about垂直方向移动进入或退出中心
void app_about_anim_Vexit(bool dir)
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
