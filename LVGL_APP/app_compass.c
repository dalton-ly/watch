#include "app_compass.h"
#include "app_anim.h"

LV_IMG_DECLARE(compass_needle); //指南针-针图片声明

static lv_obj_t *cont;		 //页面容器
static lv_obj_t *gauge;		 //指南针表盘
static lv_obj_t *needle_img; //指南针-针img

static lv_obj_t *diplay_label; //上方显示方位的label

static lv_obj_t *azimuth_label[8]; //表盘角度标签

static const char azimuth_text[][3] = //表盘标签文本
	{
		"N", "NE", "E", "SE", "S", "SW", "W", "NW"};

static char display_text[] = "360\'";

static lv_color_t needle_color[1]; //gauge自带指针的颜色，设为黑色与背景融合。使用img图片指针
		
//创建指南针界面
void app_compass_create(void)
{
	uint8_t i;
	lv_obj_t *scr;

	scr = lv_scr_act(); //获取当前活跃的屏幕

	cont = lv_cont_create(scr, NULL);															//创建容器
	lv_obj_set_size(cont, 240, 240);															//设置容器大小
	lv_obj_align(cont, scr, LV_ALIGN_OUT_TOP_MID, 0, 0);										//设置容器在屏幕外部正上方
	lv_obj_set_style_local_bg_opa(cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP);		//设置背景颜色透明
	lv_obj_set_style_local_border_opa(cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP); //设置边框透明

	lv_cont_set_layout(cont, LV_LAYOUT_OFF); //设置容器的布局方式: 关闭自动布局。根据需要设置是否自动布局,否则程序卡死

	gauge = lv_gauge_create(cont, NULL);														   //创建指南针表盘
	lv_obj_set_size(gauge, 240, 240);															   //设置表盘尺寸
	lv_obj_align(gauge, cont, LV_ALIGN_CENTER, 0, 0);											   //设置表盘中央对齐
	lv_gauge_set_range(gauge, 0, 3600);															   //设置表盘数值范围
	lv_gauge_set_critical_value(gauge, 0);														   //设置关键数值点0
	lv_gauge_set_scale(gauge, 360, 13, 13);														   //设置表盘角度360°，刻度数量13，标签数量13
	lv_obj_set_style_local_bg_color(gauge, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);	   //设置背景颜色为黑色
	lv_obj_set_style_local_border_width(gauge, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 5);			   //设置边框宽度
	lv_obj_set_style_local_border_color(gauge, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GRAY); //设置边框颜色为灰色
	lv_obj_set_style_local_text_opa(gauge, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP);	   //设置标签透明

	needle_color[0] = LV_COLOR_BLACK;
	lv_gauge_set_needle_count(gauge, 1, needle_color); //设置指针数量为1，颜色黑色，便于与背景融合
	lv_gauge_set_value(gauge, 0, 3550);				   //设置默认指针角度，防止挡刻度线

	for (i = 0; i < 8; i++)
	{
		lv_obj_t *label = lv_label_create(cont, NULL);
		azimuth_label[i] = label;
		lv_label_set_static_text(azimuth_label[i], azimuth_text[i]);
		lv_obj_set_style_local_text_color(azimuth_label[i], LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_CYAN); //设置字体颜色

		if (!(i % 2)) //东西南北20号字体
			lv_obj_set_style_local_text_font(azimuth_label[i], LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_20);
		else //其余14号字体
			lv_obj_set_style_local_text_font(azimuth_label[i], LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_14);
	}

	//设置标签坐标
	lv_obj_align(azimuth_label[0], cont, LV_ALIGN_IN_TOP_MID, 0, 20);		  //N
	lv_obj_align(azimuth_label[1], cont, LV_ALIGN_IN_TOP_RIGHT, -50, 50);	  //NE
	lv_obj_align(azimuth_label[2], cont, LV_ALIGN_IN_RIGHT_MID, -20, 0);	  //E
	lv_obj_align(azimuth_label[3], cont, LV_ALIGN_IN_BOTTOM_RIGHT, -50, -50); //SE
	lv_obj_align(azimuth_label[4], cont, LV_ALIGN_IN_BOTTOM_MID, 0, -20);	  //S
	lv_obj_align(azimuth_label[5], cont, LV_ALIGN_IN_BOTTOM_LEFT, 50, -50);	  //SW
	lv_obj_align(azimuth_label[6], cont, LV_ALIGN_IN_LEFT_MID, 20, 0);		  //W
	lv_obj_align(azimuth_label[7], cont, LV_ALIGN_IN_TOP_LEFT, 50, 50);		  //NW

	diplay_label = lv_label_create(cont, NULL);																	//创建显示角度标签
	lv_label_set_static_text(diplay_label, display_text);														//设置显示文本
	lv_obj_set_style_local_text_font(diplay_label, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_32); //设置字体32字号
	lv_obj_set_style_local_text_color(diplay_label, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);		//设置字体颜色
	lv_obj_align(diplay_label, cont, LV_ALIGN_CENTER, 0, -58);													//设置标签对齐
	lv_obj_set_auto_realign(diplay_label, true);																//使能文本重新排列，自动对齐

	needle_img = lv_img_create(cont, NULL);					//创建指南针-针img
	lv_img_set_src(needle_img, &compass_needle);			//设置显示图片
	lv_obj_align(needle_img, gauge, LV_ALIGN_CENTER, 0, 0); //设置中央对齐
	lv_img_set_angle(needle_img, 0);
	
}

//获取一个十进制的位数
static uint8_t dec_get_bits(int16_t dec)
{
	uint8_t i = 0;

	if (dec == 0)
		return 1;

	while (dec)
	{
		i++;
		dec /= 10;
	}

	return i;
}

//更新指南针-针的角度
void app_compass_update_angle(int16_t angle)
{
	uint8_t i, max_bits;

	max_bits = dec_get_bits(angle); //计算角度数值有几位

	lv_img_set_angle(needle_img, angle * 10); //设置指南针-针角度

	for (i = 0; i < max_bits; i++)
	{
		display_text[max_bits - i - 1] = angle % 10 + '0';
		angle /= 10;
	}
	display_text[i] = '\'';
	display_text[i + 1] = '\0';

	lv_label_set_static_text(diplay_label, display_text); //设置显示文本: 角度
}

//compass垂直方向移动进入或退出中心
void app_compass_anim_Vexit(bool dir)
{
	uint16_t start;
	
	start = lv_obj_get_y(cont);		//获取起始值
	
	obj_add_anim(
								cont, 		//动画对象
								(lv_anim_exec_xcb_t)lv_obj_set_y,				//动画函数
								lv_anim_speed_to_time(100, 0, 42),			//动画速度
								start,																	//起始值
								start + (dir ? -240 : 240),							//结束值
								lv_anim_path_bounce											//动画特效:模拟弹性物体下落
								);
}
