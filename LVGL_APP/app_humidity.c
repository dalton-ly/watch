#include "app_humidity.h"
#include  "app_anim.h"

//LV_IMG_DECLARE(humidity);   //气压温度图标
LV_IMG_DECLARE(num0);
LV_IMG_DECLARE(num1);
LV_IMG_DECLARE(num2);
LV_IMG_DECLARE(num3);
LV_IMG_DECLARE(num4);
LV_IMG_DECLARE(num5);
LV_IMG_DECLARE(num6);
LV_IMG_DECLARE(num7);
LV_IMG_DECLARE(num8);
LV_IMG_DECLARE(num9);
LV_IMG_DECLARE(Point);


static lv_obj_t* cont;  // 界面容器
static lv_obj_t* label_temper;  //温度标签
static lv_obj_t* label_humidity;    //湿度标签
static lv_obj_t* label_pressure;    //气压标签

static const char* temper_text="temperature:";
static const char* humidity_text="humidity:";
static const char* pressure_text="pressure:";

static const lv_img_dsc_t *dsc[10] = {&num0, &num1, &num2, &num3, &num4, &num5, &num6, &num7, &num8, &num9}; //数字列表

void app_humidity_create() //创建气压温度界面
{
    	lv_obj_t *scr;  //指向屏幕的指针
        scr=lv_scr_act();   //获取当前屏幕
        
    //设置容器属性    
    cont = lv_cont_create(scr, NULL);															//创建容器
	lv_obj_set_size(cont, 220, 220);															//设置容器大小
	lv_obj_align(cont, scr, LV_ALIGN_OUT_TOP_MID, 0, 0);										//设置容器在屏幕外部正上方
	lv_obj_set_style_local_bg_opa(cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP);		//设置背景透明
	lv_obj_set_style_local_border_opa(cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP); //设置边框透明
    lv_cont_set_layout(cont, LV_LAYOUT_OFF); //设置容器的布局方式: 关闭自动布局。根据需要设置是否自动布局,否则程序卡死


    //创建标签
    //三个标签由上到下
	label_temper = lv_label_create(cont, NULL);																   //创建温度标签
	lv_obj_set_style_local_text_font(label_temper, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_32); //设置字体20号
	lv_obj_set_style_local_text_color(label_temper, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_CYAN);		   //设置字体颜色
	lv_label_set_static_text(label_temper, temper_text);    //设置为静态字符串
	lv_obj_align(label_temper, NULL, LV_ALIGN_IN_TOP_MID, 0, 10);   //设置对齐

    label_pressure = lv_label_create(cont, NULL);																   //创建温度标签
	lv_obj_set_style_local_text_font(label_pressure, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_32); //设置字体20号
	lv_obj_set_style_local_text_color(label_pressure, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_CYAN);		   //设置字体颜色
	lv_label_set_static_text(label_pressure, pressure_text);    //设置为静态字符串
	lv_obj_align(label_pressure, label_temper, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);   //设置对齐
    
    label_humidity = lv_label_create(cont, NULL);																   //创建温度标签
	lv_obj_set_style_local_text_font(label_humidity, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_32); //设置字体20号
	lv_obj_set_style_local_text_color(label_humidity, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_CYAN);		   //设置字体颜色
	lv_label_set_static_text(label_humidity, humidity_text);    //设置为静态字符串
	lv_obj_align(label_humidity, label_pressure, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);   //设置对齐

}

void app_humidity_anim_Vexit(bool dir)
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


