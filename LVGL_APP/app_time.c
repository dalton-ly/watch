#include "app_time.h"
#include "string.h"
#include "app_anim.h"

//声明图片
LV_IMG_DECLARE(start);
LV_IMG_DECLARE(watch_bg);
LV_IMG_DECLARE(hour);
LV_IMG_DECLARE(minute);
LV_IMG_DECLARE(second);
LV_IMG_DECLARE(main_back);
//声明数字图片
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

static const lv_img_dsc_t *dsc[10] = {&num0, &num1, &num2, &num3, &num4, &num5, &num6, &num7, &num8, &num9}; //数字列表
static const char week_list[][5] = {"Sun.", "Mon.", "Tue.", "Wen.", "Thu.", "Fri.", "Sat."};				 //日期列表

static lv_obj_t *start_img; //定义开始背景图片img
static lv_obj_t *main_img;	//定义主背景图片img

static APP_TimeTypeDef app_time = {14, 37, 00};			  //定义时间对象，用于保存时间数据(实时更新)
static APP_DateTypeDef app_date = {2021, 3, 16, (2 - 1)}; //定义日期对象，用于保存日期数据(实时更新)

static Time_ImgTypeDef time; //定义时间控件对象
static Date_ImgTypeDef date; //定义日期控件对象

static Bat_TypeDef bat; //电量控件对象

static Temperature_TypeDef temperature;		//温度显示控件

static int16_t tim_img_y;		//保存是上方的时间img的y坐标, 用于动画起点

//创建开始图片
void app_show_start(void)
{
	lv_obj_t *scr;

	scr = lv_scr_act();																		  //获取当前活跃的屏幕
	lv_obj_set_style_local_bg_color(scr, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK); //设置背景颜色为黑色

	start_img = lv_img_create(scr, NULL);

	lv_obj_set_size(start_img, 184, 84); //设置大小

	lv_obj_align(start_img, scr, LV_ALIGN_CENTER, 0, 0); //设置图片中央对齐

	lv_img_set_src(start_img, &start); //然后显示此图片

	lv_obj_set_style_local_image_opa(start_img, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP); //图片透明
}

//开始图片的渐变动画
void start_img_anim(void)
{
	lv_anim_t anim;

	lv_anim_init(&anim);

	lv_anim_set_var(&anim, start_img); //动画对象

	lv_anim_set_exec_cb(&anim, (lv_anim_exec_xcb_t)img_anim_exec_xcb); //渐变函数
	lv_anim_set_time(&anim, lv_anim_speed_to_time(100, 0, 80));

	anim.start = LV_OPA_TRANSP;			//起始值
	anim.end = LV_OPA_COVER;			//结束值
	anim.path.cb = lv_anim_path_linear; //线性
	anim.playback_time = 0;				//回放时间设为0不执行动画回放

	lv_anim_start(&anim);
}

//删除开始图片
void app_del_start(void)
{
	lv_obj_del(start_img); //删除启动图片对象
}

//创建数字表盘
void app_digital_clock_create(void)
{
	uint8_t i;
	lv_obj_t *scr;

	scr = lv_scr_act(); //获取当前活跃的屏幕

	main_img = lv_img_create(scr, NULL);

	lv_obj_set_pos(main_img, 0, 0);		 //设置位置
	lv_obj_set_size(main_img, 240, 240); //设置大小

	lv_img_set_src(main_img, &main_back); //然后显示此图片

	/***********************创建时间控件****************************/
	time.time_cont = lv_cont_create(scr, NULL);															  //创建时间容器
	lv_obj_set_pos(time.time_cont, 34, 99);																  //设置容器位置
	lv_obj_set_size(time.time_cont, 170, 42);															  //设置容器尺寸
	lv_obj_set_style_local_bg_opa(time.time_cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP);	  //设置背景透明
	lv_obj_set_style_local_border_opa(time.time_cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP); //设置边框透明
	lv_cont_set_layout(time.time_cont, LV_LAYOUT_ROW_MID);												  //设置容器的布局方式: 开启自动布局
	lv_obj_set_style_local_pad_inner(time.time_cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);			  //设置子对象之间的间距

	for (i = 0; i < 2; i++) //创建显示用img图片0~1
	{
		time.img_time[i][0] = lv_img_create(time.time_cont, NULL);
		lv_img_set_src(time.img_time[i][0], &num0);
	}

	time.img_Point = lv_img_create(time.time_cont, NULL); //创建 :
	lv_img_set_src(time.img_Point, &Point);

	for (i = 2; i < 4; i++) //创建显示用img图片2~3
	{
		time.img_time[i][0] = lv_img_create(time.time_cont, NULL);
		lv_img_set_src(time.img_time[i][0], &num0);
	}

	lv_cont_set_layout(time.time_cont, LV_LAYOUT_OFF); //设置容器的布局方式: 取消自动布局

	for (i = 0; i < 4; i++) //创建滚动切换用img
	{
		time.img_time[i][1] = lv_img_create(time.time_cont, NULL);
		lv_obj_align(time.img_time[i][1], time.img_time[i][0], LV_ALIGN_OUT_TOP_MID, 0, -21);
		lv_img_set_src(time.img_time[i][1], &num1);
	}
	
	tim_img_y = lv_obj_get_y(time.img_time[0][1]);		//获取并保存时间的上方的imgy坐标, 即img动画起始地点
	
	lv_obj_set_size(time.time_cont, 195, 42); //设置容器尺寸

	time.label_sec = lv_label_create(time.time_cont, NULL); //创建秒label
	lv_obj_align(time.label_sec, time.time_cont, LV_ALIGN_IN_BOTTOM_RIGHT, 0, 0);
	lv_obj_set_style_local_text_font(time.label_sec, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_14); //设置字体
	lv_obj_set_style_local_text_color(time.label_sec, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);		  //设置颜色
	strcpy(time.sec_text, "00");
	lv_label_set_static_text(time.label_sec, time.sec_text);

	/***********************创建时间控件****************************/

	/***********************创建日期控件****************************/
	date.date_cont = lv_cont_create(scr, NULL);															  //创建日期容器
	lv_obj_set_pos(date.date_cont, 34, 150);															  //设置容器位置
	lv_obj_set_size(date.date_cont, 190, 20);															  //设置容器尺寸
	lv_obj_set_style_local_bg_opa(date.date_cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP);	  //设置背景透明
	lv_obj_set_style_local_border_opa(date.date_cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP); //设置边框透明

	date.label_year_month_date = lv_label_create(date.date_cont, NULL);
	lv_obj_set_style_local_text_font(date.label_year_month_date, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_20); //设置字体
	lv_obj_set_style_local_text_color(date.label_year_month_date, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);		  //设置颜色

	lv_obj_align(date.label_year_month_date, NULL, LV_ALIGN_IN_LEFT_MID, 0, 0); //设置其在日期容器的左边居中
	strcpy(date.year_month_date_text, "2021-03-15");
	lv_label_set_static_text(date.label_year_month_date, date.year_month_date_text);

	date.label_week = lv_label_create(date.date_cont, NULL);
	lv_obj_set_style_local_text_font(date.label_week, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_20); //设置字体
	lv_obj_set_style_local_text_color(date.label_week, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);		   //设置颜色
	lv_label_set_align(date.label_week, LV_ALIGN_IN_RIGHT_MID);													   //设置文本对齐方式右对齐
	lv_obj_align(date.label_week, NULL, LV_ALIGN_IN_RIGHT_MID, -30, 0);											   //设置其在日期容器的右边居中
	strcpy(date.week_text, "Mon.");
	lv_label_set_static_text(date.label_week, date.week_text);

	/***********************创建日期控件****************************/

	/***********************创建电量控件****************************/
	bat.cont = lv_cont_create(scr, NULL);															//创建日期容器
	lv_obj_set_size(bat.cont, 100, 20);																//设置容器尺寸
	lv_obj_align(bat.cont, time.time_cont, LV_ALIGN_OUT_TOP_MID, -10, -15);							//顶部居中对齐
	lv_obj_set_style_local_bg_opa(bat.cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP);		//设置背景透明
	lv_obj_set_style_local_border_opa(bat.cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP); //设置边框透明

	bat.bar = lv_bar_create(bat.cont, NULL);													  //创建电量bar
	lv_bar_set_range(bat.bar, 0, 100);															  //设置上下限
	lv_bar_set_type(bat.bar, LV_BAR_TYPE_NORMAL);												  // 设置控件类型
	lv_obj_set_size(bat.bar, 50, 15);															  //设置尺寸
	lv_obj_set_style_local_bg_color(bat.bar, LV_BAR_PART_BG, LV_STATE_DEFAULT, LV_COLOR_WHITE);	  //设置控件背景色颜色
	lv_obj_set_style_local_bg_color(bat.bar, LV_BAR_PART_INDIC, LV_STATE_DEFAULT, LV_COLOR_CYAN); //设置控件前景色颜色
	lv_obj_set_style_local_pad_all(bat.bar, LV_BAR_PART_BG, LV_STATE_DEFAULT, 2);				  //设指示器和背景的间距
	lv_obj_align(bat.bar, bat.cont, LV_ALIGN_IN_LEFT_MID, 10, 0);								  //左部居中对齐
	lv_bar_set_value(bat.bar, 100, LV_ANIM_ON);													  //设置进度条值

	bat.label = lv_label_create(bat.cont, NULL);
	lv_obj_set_style_local_text_color(bat.label, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE); //设置颜色
	strcpy(bat.bat_value_text, "100%");
	lv_label_set_static_text(bat.label, bat.bat_value_text);
	lv_obj_align(bat.label, bat.bar, LV_ALIGN_OUT_RIGHT_BOTTOM, 0, 0); //外部右下角对齐
	lv_obj_set_auto_realign(bat.label, true);						   //使能自动对齐
	/***********************创建电量控件****************************/
	
	/***********************创建温度控件****************************/
	temperature.cont = lv_cont_create(scr, NULL);															//创建日期容器
	lv_obj_set_size(temperature.cont, 120, 35);																//设置容器尺寸
	lv_obj_align(temperature.cont, scr, LV_ALIGN_IN_BOTTOM_MID, 0, -30); //底端居中对齐
	lv_obj_set_style_local_bg_opa(temperature.cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP);		//设置背景透明
	lv_obj_set_style_local_border_opa(temperature.cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP); //设置边框透明
	
	temperature.temperature_label = lv_label_create(temperature.cont, NULL);		//创建温度label
	
	lv_obj_set_style_local_text_font(temperature.temperature_label, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_32); //设置字体
	lv_obj_set_style_local_text_color(temperature.temperature_label, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);		  //设置颜色
	strcpy(temperature.temperature_text, "--.-\'C");
	lv_label_set_static_text(temperature.temperature_label, temperature.temperature_text);
	lv_obj_align(temperature.temperature_label, temperature.cont, LV_ALIGN_CENTER, 0, 0); //居中对齐
	lv_obj_set_auto_realign(temperature.temperature_label, true);						   //使能自动对齐
}

//初始化时间显示数据
void app_init_time_data(APP_TimeTypeDef *app_time_t)
{
	uint8_t i;
	uint8_t temp[4];

	temp[0] = app_time_t->Hour / 10; //分离时间的每个位
	temp[1] = app_time_t->Hour % 10;
	temp[2] = app_time_t->Minute / 10;
	temp[3] = app_time_t->Minute % 10;

	for (i = 0; i < 4; i++)
	{
		lv_img_set_src(time.img_time[i][1], dsc[temp[i]]); //设置时间每个位的下一次显示img
	}
}

//更新时间显示数据
void app_update_time_data(APP_TimeTypeDef *app_time_t)
{
	uint8_t i;
	uint8_t temp[4];

	temp[0] = app_time_t->Hour / 10; //分离时间的每个位
	temp[1] = app_time_t->Hour % 10;
	temp[2] = app_time_t->Minute / 10;
	temp[3] = app_time_t->Minute % 10;

	for (i = 0; i < 4; i++)
	{
		lv_img_set_src(time.img_time[i][time.dir[i] ? 1 : 0], dsc[temp[i]]); //设置时间每个位的当前显示img
	}

	for (i = 0; i < 4; i++)
	{
		switch (i) //限制每个位的最大显示值
		{
		case 0:
			temp[0] = temp[0] >= 2 ? 0 : temp[0] + 1;
			break;
		case 1:
			temp[1] = app_time_t->Hour / 10 >= 2 ? (temp[1] >= 3 ? 0 : temp[1] + 1) : (temp[1] >= 9 ? 0 : temp[1] + 1);
			break;
		case 2:
			temp[2] = temp[2] >= 5 ? 0 : temp[2] + 1;
			break;
		case 3:
			temp[3] = temp[3] >= 9 ? 0 : temp[3] + 1;
			break;
		default:
			break;
		}

		lv_img_set_src(time.img_time[i][time.dir[i] ? 0 : 1], dsc[temp[i]]); //设置时间每个位的下一次显示img
	}
}

//更新日期显示数据
void app_update_date_data(APP_DateTypeDef *app_date_t)
{
	/**************更新年月日******************/
	date.year_month_date_text[0] = app_date_t->year / 1000 + '0';
	date.year_month_date_text[1] = app_date_t->year / 100 % 10 + '0';
	date.year_month_date_text[2] = app_date_t->year / 10 % 10 + '0';
	date.year_month_date_text[3] = app_date_t->year % 10 + '0';

	date.year_month_date_text[5] = app_date_t->month / 10 + '0';
	date.year_month_date_text[6] = app_date_t->month % 10 + '0';

	date.year_month_date_text[8] = app_date_t->date / 10 + '0';
	date.year_month_date_text[9] = app_date_t->date % 10 + '0';

	/*****************更新星期******************/
	strcpy(date.week_text, week_list[app_date_t->week]);

	/*****************更新到label******************/
	lv_label_set_static_text(date.label_year_month_date, date.year_month_date_text);
	lv_label_set_static_text(date.label_week, date.week_text);
}

//动画执行完成回调函数
static void time_anim_ready_cb(lv_anim_t *a)
{
	app_update_time_data(&app_time);
}

//设置时间的 : 状态
void app_time_point_state(bool state)
{
	lv_obj_set_style_local_image_opa(time.img_Point, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, state ? LV_OPA_COVER : LV_OPA_TRANSP);
}

//时间显示开启一次动画
void app_time_anim_start(uint8_t obj)
{
	uint8_t i;
	uint8_t temp;

	for (i = 0; i < 2; i++)
	{
		lv_anim_init(&time.img_anim[obj][i]);

		lv_anim_set_var(&time.img_anim[obj][i], time.img_time[obj][i]);				   //动画对象
		lv_anim_set_exec_cb(&time.img_anim[obj][i], (lv_anim_exec_xcb_t)lv_obj_set_y); //Y轴方向移动
		lv_anim_set_time(&time.img_anim[obj][i], lv_anim_speed_to_time(100, 0, 42));
		lv_anim_set_ready_cb(&time.img_anim[obj][i], time_anim_ready_cb); //设置动画结束回调函数
		
		temp = time.dir[obj] ? 1 : 0;		//判断是哪一个img在下面

		time.img_anim[obj][i].start = tim_img_y + (i == temp ? 41 : 0);	//动画起点，上面的img为y，下面的img为y+41
		time.img_anim[obj][i].end = tim_img_y + (i == temp ? 41 : 0) + 41;	//动画终点，上面的img为y+41，下面的img为y+41+41
		time.img_anim[obj][i].path.cb = lv_anim_path_linear;								  //直线运动
		time.img_anim[obj][i].playback_time = 0;											  //回放时间设为0不执行动画回放
	}

	lv_anim_start(&time.img_anim[obj][0]); //开启动画
	lv_anim_start(&time.img_anim[obj][1]); //开启动画

	time.dir[obj] = !time.dir[obj]; //标记下次运动方向改变
}

//更新时间
void app_update_time(APP_TimeTypeDef *app_time_t)
{
	static APP_TimeTypeDef app_time_old;
	app_time.Hour = app_time_t->Hour;
	app_time.Minute = app_time_t->Minute;
	app_time.Second = app_time_t->Second;

	time.sec_text[0] = app_time.Second / 10 + '0';
	time.sec_text[1] = app_time.Second % 10 + '0';

	if (app_time.Hour != app_time_old.Hour) //若时与旧的时不相等
	{
		if (app_time.Hour / 10 != app_time_old.Hour / 10) //更新时十位动画
			app_time_anim_start(0);
		if (app_time.Hour % 10 != app_time_old.Hour % 10) //更新时个位动画
			app_time_anim_start(1);

		app_time_old.Hour = app_time.Hour; //更新旧的时
	}
	if (app_time.Minute != app_time_old.Minute) //若分钟与旧的分钟不相等
	{
		if (app_time.Minute / 10 != app_time_old.Minute / 10) //更新分钟十位动画
			app_time_anim_start(2);
		if (app_time.Minute % 10 != app_time_old.Minute % 10) //更新分钟个位动画
			app_time_anim_start(3);

		app_time_old.Minute = app_time.Minute; //更新新的时
	}

	lv_label_set_static_text(time.label_sec, time.sec_text); //更新秒
}

//更新日期
void app_update_date(APP_DateTypeDef *app_date_t)
{
	app_date.year = app_date_t->year;
	app_date.month = app_date_t->month;
	app_date.date = app_date_t->date;
	app_date.week = app_date_t->week;

	app_update_date_data(&app_date);
}

//获取一个十进制数的位数
static uint8_t dev_get_bits(uint16_t dec)
{
	uint8_t bits = 0;
	
	if(!dec)
		return 1;
	
	while(dec)
	{
		bits++;
		dec /= 10;
	}
	
	return bits;
}

//更新电量显示
void app_update_bat(uint8_t bat_value, uint8_t chg_state)
{
	uint8_t i;
	uint8_t bits;
	
	bits = dev_get_bits(bat_value);
	
	lv_bar_set_value(bat.bar, bat_value, LV_ANIM_ON);		 //更新进度条
	
	if (bat_value < 10)		//电量不足
	{
		lv_obj_set_style_local_bg_color(bat.bar, LV_BAR_PART_INDIC, LV_STATE_DEFAULT, LV_COLOR_RED); //设置控件前景色颜色: 红色
	}
	else
	{
		lv_obj_set_style_local_bg_color(bat.bar, LV_BAR_PART_INDIC, LV_STATE_DEFAULT, LV_COLOR_CYAN); //设置控件前景色颜色: 青色
	}
	
	for(i = 0; i < bits; i++)			//计算文本内容
	{
		bat.bat_value_text[bits - i - 1] = bat_value % 10 + '0';
		bat_value /= 10;
	}
	bat.bat_value_text[i] = '%';
	bat.bat_value_text[i + 1] = '\0';
	

	if (chg_state)																					   //是否在充电
		lv_obj_set_style_local_bg_color(bat.bar, LV_BAR_PART_INDIC, LV_STATE_DEFAULT, LV_COLOR_GREEN); //设置控件前景色颜色: 绿色

	lv_label_set_static_text(bat.label, bat.bat_value_text); //更新文本
}

//更新温度数值(扩大10倍的值)
void app_temperature_update(int16_t T)
{
	uint8_t i;
	uint8_t bits;
	uint8_t last_bits;
	
	bits = dev_get_bits(T);
	
	if(T < 0)
		T = -T;
	
	for(i = 0; i < bits; i++)			//计算文本内容
	{
		temperature.temperature_text[bits - i - 1] = T % 10 + '0';
		T /= 10;
	}
	last_bits = temperature.temperature_text[i - 1];
	temperature.temperature_text[i - 1] = '.';
	
	temperature.temperature_text[i] = last_bits;
	temperature.temperature_text[i + 1] = 'C';
	temperature.temperature_text[i + 2] = '\0';
	
	lv_label_set_static_text(temperature.temperature_label, temperature.temperature_text);	//更新温度label显示内容
}

//创建并开始界面进入或退出动画(true：进入，false：退出)
void app_time_anim_Hexit(bool dir)
{
	uint8_t i;
	uint16_t start;

	for (i = 0; i < 2; i++)
	{
		start = lv_obj_get_x(i ? time.time_cont : date.date_cont);		//获取起始值
		obj_add_anim(
								i ? time.time_cont : date.date_cont, 		//动画对象
								(lv_anim_exec_xcb_t)lv_obj_set_x,				//动画函数
								lv_anim_speed_to_time(100, 0, 42),			//动画速度
								start,																	//起始值
								start + (dir ? 220 : -220),							//结束值
								lv_anim_path_bounce											//动画特效:模拟弹性物体下落
								);
	}

	start = lv_obj_get_x(bat.cont);		//获取起始值
	
	obj_add_anim(
								bat.cont, 		//动画对象
								(lv_anim_exec_xcb_t)lv_obj_set_x,				//动画函数
								lv_anim_speed_to_time(100, 0, 42),			//动画速度
								start,																	//起始值
								start + (dir ? -220 : 220),							//结束值
								lv_anim_path_bounce											//动画特效:模拟弹性物体下落
								);
	
	start = lv_obj_get_y(temperature.cont);		//获取起始值
	
	obj_add_anim(
								temperature.cont, 		//动画对象
								(lv_anim_exec_xcb_t)lv_obj_set_y,				//动画函数
								lv_anim_speed_to_time(100, 0, 42),			//动画速度
								start,																	//起始值
								start + (dir ? -80 : 80),							//结束值
								lv_anim_path_bounce											//动画特效:模拟弹性物体下落
								);
}

