#include "app_menu.h"
#include "app_anim.h"

//显示图片声明
LV_IMG_DECLARE(setting);
LV_IMG_DECLARE(compass);
LV_IMG_DECLARE(about);

static ICON_TypeDef menu[3] =
{
	{.img_buffer = &compass},
	{.img_buffer = &setting},
	{.img_buffer = &about},
};

//创建菜单界面
void app_menu_create(void)
{
	uint8_t i;
	lv_obj_t *scr;

	scr = lv_scr_act(); //获取当前活跃的屏幕

	for (i = 0; i < sizeof(menu) / sizeof(ICON_TypeDef); i++)
	{
		menu[i].img = lv_img_create(scr, NULL);						  //创建img显示菜单
		lv_img_set_src(menu[i].img, menu[i].img_buffer);			  //设置显示图片
		lv_obj_align(menu[i].img, scr, LV_ALIGN_OUT_TOP_MID, 0, -10); //设置图片显示顶端
	}
}

//菜单图标垂直方向移动进入或退出中心
void app_menu_anim_Vexit(uint8_t id, bool dir)
{
	uint16_t start;
	
	start = lv_obj_get_y(menu[id].img);		//获取起始值
	
	obj_add_anim(
								menu[id].img, 		//动画对象
								(lv_anim_exec_xcb_t)lv_obj_set_y,				//动画函数
								lv_anim_speed_to_time(100, 0, 42),			//动画速度
								start,																	//起始值
								start + (dir ? -180 : 180),							//结束值
								lv_anim_path_bounce											//动画特效:模拟弹性物体下落
								);
}

//菜单图标水平方向移动进入或退出中心
void app_menu_anim_Hexit(uint8_t id, bool dir)
{
	uint16_t start;
	
	start = lv_obj_get_x(menu[id].img);		//获取起始值
	
	obj_add_anim(
								menu[id].img, 		//动画对象
								(lv_anim_exec_xcb_t)lv_obj_set_x,				//动画函数
								lv_anim_speed_to_time(100, 0, 42),			//动画速度
								start,																	//起始值
								start + (dir ? -230 : 230),							//结束值
								lv_anim_path_bounce											//动画特效:模拟弹性物体下落
								);
}
