#include "timer.h"
#include "lvgl.h"


static TIM_HandleTypeDef lvgl_timer_handle;			//��ʱ�����

//��ʼ����ʱ��3��ΪLVGL��������ʱ��
void LVGL_Timer_Init(void)
{
	lvgl_timer_handle.Instance = TIM3;
	lvgl_timer_handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;		
	lvgl_timer_handle.Init.CounterMode = TIM_COUNTERMODE_UP;		//���ϼ���
	lvgl_timer_handle.Init.Period = 1000 - 1;		//��ʱʱ��1ms
	lvgl_timer_handle.Init.Prescaler = 100 - 1;		//100��Ƶ��1MHz
	lvgl_timer_handle.Init.RepetitionCounter = 0;
	
	HAL_TIM_Base_Init(&lvgl_timer_handle);		//��ʼ����ʱ��
	
	HAL_TIM_Base_Start_IT(&lvgl_timer_handle);		//������ʱ�����������ж�
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM3)
	{
		__HAL_RCC_TIM3_CLK_ENABLE();		//������ʱ��3��ʱ��
		
		HAL_NVIC_SetPriority(TIM3_IRQn, 0, 3);  //��ʱ��3�ж����ȼ�,��ռ0����Ӧ3
		HAL_NVIC_EnableIRQ(TIM3_IRQn);					//������ʱ��3�ж�
	}
}

void TIM3_IRQHandler(void)
{
	if(__HAL_TIM_GET_ITSTATUS(&lvgl_timer_handle, TIM_IT_UPDATE) != RESET)		//�����ж�
	{
		lv_tick_inc(1);//lvgl �� 1ms ����
	}
	
	__HAL_TIM_CLEAR_IT(&lvgl_timer_handle, TIM_IT_UPDATE);		//��������жϱ�־λ
}

