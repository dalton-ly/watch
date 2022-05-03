#include "bat_adc.h"
#include "dma.h"

static ADC_HandleTypeDef adc_handle;	  //ADC句柄
static DMA_HandleTypeDef adc_dma_handle;  //DMA句柄
static TIM_HandleTypeDef adc_timer_hanle; //定时器句柄

static uint16_t adc_data[1]; //保存ADC数值

//初始化ADC触发的定时器
static void ADC_TIM_Init(void)
{
	TIM_OC_InitTypeDef TIM_OC_InitStrucuture;

	adc_timer_hanle.Instance = TIM2;
	adc_timer_hanle.Init.ClockDivision = TIM_CLEARINPUTPRESCALER_DIV1;
	adc_timer_hanle.Init.CounterMode = TIM_COUNTERMODE_UP;
	adc_timer_hanle.Init.Prescaler = 10000 - 1; //10kHz
	adc_timer_hanle.Init.Period = 100 - 1;		//10ms触发1次
	adc_timer_hanle.Init.RepetitionCounter = 0;

	HAL_TIM_PWM_Init(&adc_timer_hanle);

	TIM_OC_InitStrucuture.Pulse = 50;
	TIM_OC_InitStrucuture.OCMode = TIM_OCMODE_PWM1;			//CNT小于CCRx有效电平
	TIM_OC_InitStrucuture.OCPolarity = TIM_OCPOLARITY_HIGH; //有效电平高电平

	HAL_TIM_PWM_ConfigChannel(&adc_timer_hanle, &TIM_OC_InitStrucuture, TIM_CHANNEL_4); //初始化PWM模式

	HAL_TIM_PWM_Start(&adc_timer_hanle, TIM_CHANNEL_4); //开启PWM模式,产生ADC触发信号
}

//初始化ADC DMA
static void ADC_DMA_Init(void)
{
	DMA_Transfer_InitTypeDef DMA_Transfer_InitStructure;

	__HAL_LINKDMA(&adc_handle, DMA_Handle, adc_dma_handle); //连接外设句柄和DMA句柄

	DMA_Transfer_InitStructure.Instance = DMA2_Stream0;						  //DMA2数据流0
	DMA_Transfer_InitStructure.Channel = DMA_CHANNEL_0;						  //通道0
	DMA_Transfer_InitStructure.Direction = DMA_PERIPH_TO_MEMORY;			  //传输方向
	DMA_Transfer_InitStructure.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;	  //存储器数据宽度
	DMA_Transfer_InitStructure.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD; //外设数据宽度
	DMA_Transfer_InitStructure.MemInc = DMA_MINC_ENABLE;					  //存储器地址自增
	DMA_Transfer_InitStructure.PeriphInc = DMA_PINC_DISABLE;				  //外设地址不自增
	DMA_Transfer_InitStructure.Mode = DMA_CIRCULAR;							  //循环模式
	DMA_Transfer_InitStructure.Priority = DMA_PRIORITY_MEDIUM;				  //优先级中等

	DMA_Transfer_Init(&adc_dma_handle, &DMA_Transfer_InitStructure); //初始化DMA
}

//初始化ADC
void Bat_ADC_Init(void)
{
	ADC_ChannelConfTypeDef ADC_ChannelConfStructure;

	adc_handle.Instance = ADC1;
	adc_handle.Init.ClockPrescaler = ADC_CLOCKPRESCALER_PCLK_DIV4;			 //ADC时钟25M
	adc_handle.Init.ContinuousConvMode = DISABLE;							 //关闭连续转换
	adc_handle.Init.DataAlign = ADC_DATAALIGN_RIGHT;						 //数据右对齐
	adc_handle.Init.DiscontinuousConvMode = DISABLE;						 //关闭不连续采样模式
	adc_handle.Init.DMAContinuousRequests = ENABLE;							 //开启DMA请求
	adc_handle.Init.EOCSelection = DISABLE;									 //关闭EOC转换完成中断
	adc_handle.Init.ExternalTrigConv = ADC_EXTERNALTRIG3_T2_CC4;			 //定时器2通道4触发转换
	adc_handle.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_FALLING; //触发极性:下降沿触发
	adc_handle.Init.NbrOfConversion = 1;									 //规则转换序列个数1
	adc_handle.Init.NbrOfDiscConversion = 0;								 //不连续采样转换通道个数0
	adc_handle.Init.Resolution = ADC_RESOLUTION_12B;						 //分辨率12Bit
	adc_handle.Init.ScanConvMode = DISABLE;									 //关闭扫描模式

	HAL_ADC_Init(&adc_handle); //初始化ADC

	ADC_ChannelConfStructure.Channel = ADC_CHANNEL_8;				 //通道
	ADC_ChannelConfStructure.Offset = 0;							 //偏移，针对注入通道，规则通道设为0
	ADC_ChannelConfStructure.Rank = 1;								 //在规则序列的第1个位置
	ADC_ChannelConfStructure.SamplingTime = ADC_SAMPLETIME_28CYCLES; //ADC采样时间

	HAL_ADC_ConfigChannel(&adc_handle, &ADC_ChannelConfStructure); //配置ADC规则通道

	adc_handle.Instance->CR2 |= ADC_CR2_DMA; //开启ADC的DMA传输模式

	__HAL_ADC_ENABLE(&adc_handle); //开启ADC

	ADC_DMA_Init(); //初始化DMA

	HAL_DMA_Start(adc_handle.DMA_Handle, (uint32_t)&adc_handle.Instance->DR, (uint32_t)&adc_data, 1); //开启DMA

	ADC_TIM_Init(); //ADC触发定时器初始化
}

//获取电池电压(扩大1000倍)
uint16_t Bat_GetValue(void)
{
	uint16_t temp;

	temp = adc_data[0] * 3.3f * 1000 * 2 / 4096.0f; //计算电压(扩大1000倍)，电阻分压，因此乘以2

	return temp;
}

//PWM初始化回调函数
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM2)
	{
		__HAL_RCC_TIM2_CLK_ENABLE(); //开启时钟
	}
}

//ADC初始化回调函数
void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	if (hadc->Instance == ADC1)
	{
		__HAL_RCC_ADC1_CLK_ENABLE(); //开启ADC1时钟
		BAT_ADC_GPIO_CLK_ENABLE();	 //开启GPIO时钟

		GPIO_InitStructure.Mode = GPIO_MODE_ANALOG; //模拟输入
		GPIO_InitStructure.Pin = BAT_ADC_PIN;
		GPIO_InitStructure.Pull = GPIO_NOPULL;

		HAL_GPIO_Init(BAT_ADC_GPIO, &GPIO_InitStructure); //初始化GPIO
	}
}
