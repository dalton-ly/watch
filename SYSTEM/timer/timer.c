#include "timer.h"
#include "lvgl.h"


static TIM_HandleTypeDef lvgl_timer_handle;			//定时器句柄

//初始化定时器3作为LVGL的心跳定时器
void LVGL_Timer_Init(void)
{
	lvgl_timer_handle.Instance = TIM3;
	lvgl_timer_handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;		
	lvgl_timer_handle.Init.CounterMode = TIM_COUNTERMODE_UP;		//向上计数
	lvgl_timer_handle.Init.Period = 1000 - 1;		//定时时间1ms
	lvgl_timer_handle.Init.Prescaler = 100 - 1;		//100分频，1MHz
	lvgl_timer_handle.Init.RepetitionCounter = 0;
	
	HAL_TIM_Base_Init(&lvgl_timer_handle);		//初始化定时器
	
	HAL_TIM_Base_Start_IT(&lvgl_timer_handle);		//开启定时器，并开启中断
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM3)
	{
		__HAL_RCC_TIM3_CLK_ENABLE();		//开启定时器3的时钟
		
		HAL_NVIC_SetPriority(TIM3_IRQn, 0, 3);  //定时器3中断优先级,抢占0，响应3
		HAL_NVIC_EnableIRQ(TIM3_IRQn);					//开启定时器3中断
	}
}

void TIM3_IRQHandler(void)
{
	if(__HAL_TIM_GET_ITSTATUS(&lvgl_timer_handle, TIM_IT_UPDATE) != RESET)		//更新中断
	{
		lv_tick_inc(1);//lvgl 的 1ms 心跳
	}
	
	__HAL_TIM_CLEAR_IT(&lvgl_timer_handle, TIM_IT_UPDATE);		//清除更新中断标志位
}

