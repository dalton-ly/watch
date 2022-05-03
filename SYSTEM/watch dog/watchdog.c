#include "watchdog.h"

static IWDG_HandleTypeDef IWDG_Handler; //独立看门狗句柄

//独立看门狗初始化, 
void IWatchDog_Init(void)
{
	 IWDG_Handler.Instance = IWDG;
    IWDG_Handler.Init.Prescaler = IWDG_PRESCALER_32;	//设置IWDG分频系数32
    IWDG_Handler.Init.Reload = 2000;		//重装载值2000，2S
    HAL_IWDG_Init(&IWDG_Handler);		//初始化IWDG  
	
    HAL_IWDG_Start(&IWDG_Handler);		//开启独立看门狗
}

//喂狗
void IWatch_Feed(void)
{
	HAL_IWDG_Refresh(&IWDG_Handler);
}

