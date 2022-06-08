#include "app_humidity.h"
#include  "app_anim.h"
#include "BME280.h"
#include "math.h"

static lv_obj_t* cont;  // 界面容器
static lv_obj_t* label_temper;  //温度标签
static lv_obj_t* label_humidity;    //湿度标签
static lv_obj_t* label_pressure;    //气压标签
static lv_obj_t* label_altitude;


//static const lv_img_dsc_t *dsc[10] = {&num0, &num1, &num2, &num3, &num4, &num5, &num6, &num7, &num8, &num9}; //数字列表

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
	lv_obj_set_style_local_text_font(label_temper, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_20); //设置字体20号
	lv_obj_set_style_local_text_color(label_temper, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_CYAN);		   //设置字体颜色
	//lv_label_set_text_static(label_temper,temper_text);    //
	lv_obj_align(label_temper, NULL, LV_ALIGN_IN_LEFT_MID, 30, -50);   //设置对齐

    label_pressure = lv_label_create(cont, NULL);																   //创建温度标签
	lv_obj_set_style_local_text_font(label_pressure, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_20); //设置字体20号
	lv_obj_set_style_local_text_color(label_pressure, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_CYAN);		   //设置字体颜色
	//lv_label_set_static_text(label_pressure, pressure_text);    //设置为静态字符串
	lv_obj_align(label_pressure, label_temper, LV_ALIGN_OUT_BOTTOM_MID, -25, 30);   //设置对齐
    
    label_humidity = lv_label_create(cont, NULL);																   //创建温度标签
	lv_obj_set_style_local_text_font(label_humidity, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_20); //设置字体20号
	lv_obj_set_style_local_text_color(label_humidity, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_CYAN);		   //设置字体颜色
	lv_obj_align(label_humidity, label_pressure, LV_ALIGN_OUT_BOTTOM_MID, 5, 30);   //设置对齐


    label_altitude = lv_label_create(cont, NULL);																   //创建温度标签
	lv_obj_set_style_local_text_font(label_altitude, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_20); //设置字体20号
	lv_obj_set_style_local_text_color(label_altitude, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_ORANGE);		   //设置字体颜色
	lv_obj_align(label_altitude, label_humidity, LV_ALIGN_OUT_BOTTOM_MID, 10, 15);   //设置对齐

	//lv_label_set_static_text(label_humidity, humidity_text);    //设置为静态字符串
	
	lv_label_set_text_fmt(label_temper,"temper");		// %.2lf deg C",bmedata->temperature);
	lv_label_set_text_fmt(label_pressure,"pressure:");	// %.2lf hPa",0.01*bmedata->pressure);
	lv_label_set_text_fmt(label_humidity,"humidity:");	
	lv_label_set_text_fmt(label_altitude,"altitude:");	//%.2lf deg C",bmedata->humidity);
	

}
void app_update_humidity(struct bme280_data* bmedata)
{
	lv_label_set_text_fmt(label_temper,"temper: %.2lf C",bmedata->temperature);
	lv_label_set_text_fmt(label_pressure,"pressure: %.2lf hPa",0.01*bmedata->pressure);
	lv_label_set_text_fmt(label_humidity,"humidity: %.2lf %%",bmedata->humidity);
	lv_label_set_text_fmt(label_altitude,"altitude: %.2lf m",(pow(101325/bmedata->pressure,(1.0/5.257))-1)*(bmedata->temperature+273.15)/0.0065);
	/*lv_label_set_text_fmt(label_humidity,"humidity:%0.2lf hPa",bmedatahumidity);
	lv_label_set_text_fmt(label_pressure,"pressure:%0.2lf%%",bmedata.pressure);
	*/
//static uint32_t prev_value = 0;

   /* if(prev_value != adc_value) {

        if(lv_obj_get_screen(label_pressure) == lv_scr_act()) {
            char buf[32];
            snprintf(buf, 32, "pressure: %0.2lf hPa", bmedata.pressure);
            lv_label_set_text(label_pressure, buf);
        }
      // prev_value = adc_value;
    //}*/
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


