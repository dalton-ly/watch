#include "main.h"

//优先级0：中断服务服务管理任务 OS_IntQTask()
//优先级1：时钟节拍任务 OS_TickTask()
//优先级2：定时任务 OS_TmrTask()
//优先级OS_CFG_PRIO_MAX-2：统计任务 OS_StatTask()
//优先级OS_CFG_PRIO_MAX-1：空闲任务 OS_IdleTask()

//一些函数的声明
static void move_task_menu(Move_DirTypeDef dir);
static void move_task_menu_change_display(Display_TypeDef disp_t);
static void move_task_about(Move_DirTypeDef dir);
static void move_task_setting(Move_DirTypeDef dir);
static void move_task_compass(Move_DirTypeDef dir);
static void move_task_humidity(Move_DirTypeDef dir);
static Move_DirTypeDef which_key(void);
static Move_DirTypeDef Move_Scan(void);


static Display_TypeDef Disp = Disp_Menu; //当前界面 菜单一个界面，包含几个图片的选择。
static int8_t id = -1;					 //菜单id，默认为时间界面

struct bme280_data bmedata;	//保存从bme280读取的数据
struct bme280_dev dev;
static AngleGyro_TypeDef *ag_t;

//定义按键
#define SW1 GPIO_PIN_10
#define SW2 GPIO_PIN_14
#define SW3 GPIO_PIN_15
#define SW1_PORT GPIOA
#define SW23_PORT GPIOB

//按键检测函数

GPIO_PinState Key_Scan(GPIO_TypeDef *GPIOx,uint16_t GPIO_PIN)
{
    if (HAL_GPIO_ReadPin(GPIOx,GPIO_PIN) == GPIO_PIN_RESET)
    {
		delay_ms(10);
		if (HAL_GPIO_ReadPin(GPIOx,GPIO_PIN) == GPIO_PIN_RESET)
		{	
			return GPIO_PIN_RESET;
		}
		else
		return GPIO_PIN_SET;
    }
    else
    {
        return GPIO_PIN_SET;
    }   
}

void Power_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
	GPIO_InitStructure.Pull = GPIO_PULLUP;
	GPIO_InitStructure.Pin = GPIO_PIN_0 | GPIO_PIN_1; //3.3V电源保持控制接口
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.Pin = GPIO_PIN_1; //充电检测接口
	GPIO_InitStructure.Pull = GPIO_PULLDOWN;
	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);

	Power_On();//PA0 和PA1设置为1
}


void SW_Init(void)//按键引脚初始化
{
	GPIO_InitTypeDef GPIO_InitStructure_SW1={0};
	GPIO_InitTypeDef GPIO_InitStructure_SW2={0};

	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	GPIO_InitStructure_SW1.Mode=GPIO_MODE_INPUT;
	GPIO_InitStructure_SW1.Pin = GPIO_PIN_10;
	GPIO_InitStructure_SW1.Pull=GPIO_PULLUP;
	//GPIO_InitStructure_SW1.Speed = GPIO_SPEED_HIGH;

	
	GPIO_InitStructure_SW2.Mode=GPIO_MODE_INPUT;
	GPIO_InitStructure_SW2.Pin = GPIO_PIN_14 | GPIO_PIN_15;	
	GPIO_InitStructure_SW2.Pull=GPIO_PULLUP;
	//GPIO_InitStructure_SW2.Speed = GPIO_SPEED_HIGH;
	HAL_GPIO_Init(SW1_PORT, &GPIO_InitStructure_SW1);
	HAL_GPIO_Init(SW23_PORT, &GPIO_InitStructure_SW2);
}

//bme初始化函数

 void BME280_Init()
{
	int8_t rslt = BME280_OK;
	uint8_t dev_addr = BME280_I2C_ADDR_PRIM;
	dev.intf_ptr = &dev_addr;
	dev.intf = BME280_I2C_INTF;
	dev.read = user_i2c_read;
	dev.write = user_i2c_write;
	dev.delay_us = user_delay_us;
	rslt = bme280_init(&dev);
} 



int main(void)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	Stm32_Clock_Init(200, 25, 2, 4); //设置时钟,100Mhz
	HAL_Init();						 //初始化HAL库
	delay_init(100);				 //初始化延时函数
	Power_Init();					 //初始化电源控制
	LED_Init();						 //初始化LED 或许可以去除？
	LCD_Init();						 //初始化LCD
	SW_Init();	
	Bat_ADC_Init();					 //初始化ADC
	PCF8563_I2C_Init();				 //PCF8563初始化 在其中定义了通信速率和i2c实例
	Kalman_Init();					 //卡尔曼参数初始化
	MPU9250_Init();					 //MPU9250初始化
	BME280_Init();					 //BME280的初始化
	LVGL_Timer_Init(); //初始化LVGL的心跳定时器

	lv_init();			 //lvgl 系统初始化
	lv_port_disp_init(); //lvgl 显示接口初始化,放在 lv_init()的后面

	OSInit(&err);		 //初始化UCOSIII
	OS_CRITICAL_ENTER(); //进入临界区
	//创建开始任务
	OSTaskCreate((OS_TCB *)&StartTaskTCB,												  //任务控制块
				 (CPU_CHAR *)"start task",												  //任务名字
				 (OS_TASK_PTR)start_task,												  //任务函数
				 (void *)0,																  //传递给任务函数的参数
				 (OS_PRIO)START_TASK_PRIO,												  //任务优先级3
				 (CPU_STK *)&START_TASK_STK[0],											  //任务堆栈基地址
				 (CPU_STK_SIZE)START_STK_SIZE / 10,										  //任务堆栈深度限位
				 (CPU_STK_SIZE)START_STK_SIZE,											  //任务堆栈大小
				 (OS_MSG_QTY)0,															  //任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
				 (OS_TICK)0,															  //当使能时间片轮转时的时间片长度，为0时为默认长度，
				 (void *)0,																  //用户补充的存储区
				 (OS_OPT)OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR | OS_OPT_TASK_SAVE_FP, //任务选项,为了保险起见，所有任务都保存浮点寄存器的值
				 (OS_ERR *)&err);														  //存放该函数错误时的返回值
	OS_CRITICAL_EXIT();																	  //退出临界区

	OSStart(&err); //开启UCOSIII 发生一次调度 即执行开始任务
	while (1)
		;
}



//开始任务函数
void start_task(void *p_arg)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	p_arg = p_arg;

	CPU_Init();
#if OS_CFG_STAT_TASK_EN > 0u
	OSStatTaskCPUUsageInit(&err); //统计任务
#endif

#ifdef CPU_CFG_INT_DIS_MEAS_EN //如果使能了测量中断关闭时间
	CPU_IntDisMeasMaxCurReset();
#endif

#if OS_CFG_SCHED_ROUND_ROBIN_EN //当使用时间片轮转的时候
								//使能时间片轮转调度功能,设置默认的时间片长度s
	OSSchedRoundRobinCfg(DEF_ENABLED, 10, &err);
#endif

	OS_CRITICAL_ENTER(); //进入临界区

	//创建LVGL任务
	OSTaskCreate((OS_TCB *)&LvglTaskTCB,
				 (CPU_CHAR *)"lvgl task",
				 (OS_TASK_PTR)lvgl_task,
				 (void *)0,
				 (OS_PRIO)LVGL_TASK_PRIO,		//优先级6
				 (CPU_STK *)&LVGL_TASK_STK[0],
				 (CPU_STK_SIZE)LVGL_STK_SIZE / 10,
				 (CPU_STK_SIZE)LVGL_STK_SIZE,
				 (OS_MSG_QTY)0,
				 (OS_TICK)0,
				 (void *)0,
				 (OS_OPT)OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR | OS_OPT_TASK_SAVE_FP,
				 (OS_ERR *)&err);
	OS_CRITICAL_EXIT(); //退出临界区

	OS_TaskSuspend((OS_TCB *)&StartTaskTCB, &err); //挂起开始任务 发生调度进入lvgltask，后续没有执行resume的话开始任务不会再被执行
}

//创建一些跟APP有关的任务
void APP_TaskCreate(void)
{
	OS_ERR err;
	CPU_SR_ALLOC();

	OS_CRITICAL_ENTER(); //进入临界区
	//创建MOVE任务
	OSTaskCreate((OS_TCB *)&MoveTaskTCB,
				 (CPU_CHAR *)"move task",
				 (OS_TASK_PTR)move_task,
				 (void *)0,
				 (OS_PRIO)MOVE_TASK_PRIO,		//10
				 (CPU_STK *)&MOVE_TASK_STK[0],
				 (CPU_STK_SIZE)MOVE_STK_SIZE / 10,
				 (CPU_STK_SIZE)MOVE_STK_SIZE,
				 (OS_MSG_QTY)0,
				 (OS_TICK)0,
				 (void *)0,
				 (OS_OPT)OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR | OS_OPT_TASK_SAVE_FP,
				 (OS_ERR *)&err);
	//创建TIME任务
	OSTaskCreate((OS_TCB *)&TimeTaskTCB,
				 (CPU_CHAR *)"time task",
				 (OS_TASK_PTR)time_task,
				 (void *)0,
				 (OS_PRIO)TIME_TASK_PRIO,		//12
				 (CPU_STK *)&TIME_TASK_STK[0],
				 (CPU_STK_SIZE)TIME_STK_SIZE / 10,
				 (CPU_STK_SIZE)TIME_STK_SIZE,
				 (OS_MSG_QTY)0,
				 (OS_TICK)0,
				 (void *)0,
				 (OS_OPT)OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR | OS_OPT_TASK_SAVE_FP,
				 (OS_ERR *)&err);
	//创建LED0任务
	OSTaskCreate((OS_TCB *)&Led0TaskTCB,
				 (CPU_CHAR *)"led0 task",
				 (OS_TASK_PTR)led0_task,
				 (void *)0,
				 (OS_PRIO)LED0_TASK_PRIO,		//15
				 (CPU_STK *)&LED0_TASK_STK[0],
				 (CPU_STK_SIZE)LED0_STK_SIZE / 10,
				 (CPU_STK_SIZE)LED0_STK_SIZE,
				 (OS_MSG_QTY)0,
				 (OS_TICK)0,
				 (void *)0,
				 (OS_OPT)OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR | OS_OPT_TASK_SAVE_FP,
				 (OS_ERR *)&err);
	//创建BAT任务
	OSTaskCreate((OS_TCB *)&BatTaskTCB,
				 (CPU_CHAR *)"bat task",
				 (OS_TASK_PTR)bat_task,
				 (void *)0,
				 (OS_PRIO)BAT_TASK_PRIO,		//9
				 (CPU_STK *)&BAT_TASK_STK[0],
				 (CPU_STK_SIZE)BAT_STK_SIZE / 10,
				 (CPU_STK_SIZE)BAT_STK_SIZE,
				 (OS_MSG_QTY)0,
				 (OS_TICK)0,
				 (void *)0,
				 (OS_OPT)OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR | OS_OPT_TASK_SAVE_FP,
				 (OS_ERR *)&err);
	//创建IMU任务
	OSTaskCreate((OS_TCB *)&ImuTaskTCB,
				 (CPU_CHAR *)"imu task",
				 (OS_TASK_PTR)imu_task,
				 (void *)0,
				 (OS_PRIO)IMU_TASK_PRIO,		//5
				 (CPU_STK *)&IMU_TASK_STK[0],
				 (CPU_STK_SIZE)IMU_STK_SIZE / 10,
				 (CPU_STK_SIZE)IMU_STK_SIZE,
				 (OS_MSG_QTY)0,
				 (OS_TICK)0,
				 (void *)0,
				 (OS_OPT)OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR | OS_OPT_TASK_SAVE_FP,
				 (OS_ERR *)&err);
	//创建COMPASS任务
	OSTaskCreate((OS_TCB *)&CompassTaskTCB,
				 (CPU_CHAR *)"compass task",
				 (OS_TASK_PTR)compass_task,
				 (void *)0,
				 (OS_PRIO)COMPASS_TASK_PRIO,		//7
				 (CPU_STK *)&COMPASS_TASK_STK[0],
				 (CPU_STK_SIZE)COMPASS_STK_SIZE / 10,
				 (CPU_STK_SIZE)COMPASS_STK_SIZE,
				 (OS_MSG_QTY)0,
				 (OS_TICK)0,
				 (void *)0,
				 (OS_OPT)OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR | OS_OPT_TASK_SAVE_FP,
				 (OS_ERR *)&err);
	//创建TEMPERATURE任务
	OSTaskCreate((OS_TCB *)&TemperatureTaskTCB,
				 (CPU_CHAR *)"temperature task",
				 (OS_TASK_PTR)temperature_task,
				 (void *)0,
				 (OS_PRIO)TEMPERATURE_TASK_PRIO,		//8
				 (CPU_STK *)&TEMPERATURE_TASK_STK[0],
				 (CPU_STK_SIZE)TEMPERATURE_STK_SIZE / 10,
				 (CPU_STK_SIZE)TEMPERATURE_STK_SIZE,
				 (OS_MSG_QTY)0,
				 (OS_TICK)0,
				 (void *)0,
				 (OS_OPT)OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR | OS_OPT_TASK_SAVE_FP,
				 (OS_ERR *)&err);
	//创建bme280任务
	OSTaskCreate((OS_TCB *)&HumidityTaskTCB,
				 (CPU_CHAR *)"humidity task",
				 (OS_TASK_PTR)humidity_task,
				 (void *)0,
				 (OS_PRIO)HUMIDITY_TASK_PRIO,		//13
				 (CPU_STK *)&HUMIDITY_TASK_STK[0],
				 (CPU_STK_SIZE)HUMIDITY_STK_SIZE / 10,
				 (CPU_STK_SIZE)HUMIDITY_STK_SIZE,
				 (OS_MSG_QTY)0,
				 (OS_TICK)0,
				 (void *)0,
				 (OS_OPT)OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR | OS_OPT_TASK_SAVE_FP,
				 (OS_ERR *)&err);

	//创建息屏控制软件定时器
	OSTmrCreate(&display_timer, "display timer", 900, 0, OS_OPT_TMR_ONE_SHOT, display_tim_callback, NULL, &err);
	OS_TaskSuspend((OS_TCB *)&CompassTaskTCB, &err); //挂起COMPASS任务 发生调度 
	IWatchDog_Init();		//独立看门狗初始化
	OS_CRITICAL_EXIT(); //退出临界区
}

//息屏控制软件定时器
void display_tim_callback(void *p_tmr, void *p_arg)
{
	Power_Off();
}

//IMU任务，优先级3
void imu_task(void *p_arg)
{
	OS_ERR err;

	p_arg = p_arg;
	
	while(1)
	{
		Update_Angle_Gyrox(); //更新姿态信息
		OSTimeDlyHMSM(0, 0, 0, 100, OS_OPT_TIME_HMSM_STRICT, &err); //延时100ms
	}
}

//LVGL任务函数，优先级4
void lvgl_task(void *p_arg)
{
	OS_ERR err;

	p_arg = p_arg;

	app_show_start(); //显示开机图片
	start_img_anim(); //开启开机图片动画

	lv_task_handler(); //LVGL更新显示

	OSTimeDlyHMSM(0, 0, 1, 0, OS_OPT_TIME_HMSM_STRICT, &err); //延时1S 这里是不是会发生一次调度？ 但是此时没有别的任务不影响
	app_del_start();										  //删除开机图片

	app_digital_clock_create(); //创建数字表盘
	app_menu_create();			//创建菜单界面
	app_about_create();			//创建"关于"界面
	app_setting_create();		//创建"设置"界面
	app_compass_create();		//创建"指南针"界面
	app_humidity_create();		//创建气压温度界面
	

	ag_t = Get_Angle_GyroxStructure();
	
	APP_TaskCreate(); //创建一些任务
	
	OSTmrStart(&display_timer, &err);

	while (1)
	{
		lv_task_handler();
		OSTimeDlyHMSM(0, 0, 0, 10, OS_OPT_TIME_HMSM_STRICT, &err); //延时10ms
	}
}

//指南针任务，优先级7
void compass_task(void *p_arg)
{
	OS_ERR err;
	CPU_SR_ALLOC();

	p_arg = p_arg;
	
	while(1)
	{
		OS_CRITICAL_ENTER(); //进入临界区
		app_compass_update_angle(ag_t->yaw);
		OS_CRITICAL_EXIT(); //退出临界区
		OSTimeDlyHMSM(0, 0, 0, 120, OS_OPT_TIME_HMSM_STRICT, &err); //延时120ms
	}
}

//温度更新任务，优先级8
void temperature_task(void *p_arg)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	uint8_t i = 0;
	int16_t real_temperature;		//实时温度数值
	int16_t min_temperature = 0x7fff;		//历史温度最小值

	p_arg = p_arg;
	
	while(1)
	{
		OS_CRITICAL_ENTER(); //进入临界区
		real_temperature = MPU_Get_Temperature() / 10;		//读1次温度
		if(min_temperature > real_temperature)
			min_temperature = real_temperature;		//保存最小值
		OS_CRITICAL_EXIT(); //退出临界区
		
		if(++i == 11)
		{
			i = 0;
			app_temperature_update(min_temperature);		//将10次采集的最小温度值更新到显示
			min_temperature = 0x7fff;		//重置历史最小值
		}
		
		OSTimeDlyHMSM(0, 0, 0, 200, OS_OPT_TIME_HMSM_STRICT, &err); //延时200ms
	}
}

//电量更新任务，优先级9
void bat_task(void *p_arg)
{
	OS_ERR err;
	uint16_t voltage;
	uint8_t chg_state;

	p_arg = p_arg;

	while (1)
	{
		chg_state = Charging_State(); //查询是否在充电
		voltage = Bat_GetValue();		 //获取电压值
		if (chg_state)
			voltage -= 2;										  //如果在充电需要减去
		voltage = (voltage - 3400) / 8;						  //计算电压百分比
		app_update_bat(voltage > 100 ? 100 : voltage, chg_state);						  //更新电量显示
		
		OSTimeDlyHMSM(0, 0, 2, 0, OS_OPT_TIME_HMSM_STRICT, &err); //延时2S
	}
}

//MOVE任务，优先级10
void move_task(void *p_arg)
{
	OS_ERR err;
	Move_DirTypeDef dir;
	p_arg = p_arg;

	while (1)
	{
		//dir=Move_Scan(); //姿态扫描
		dir=which_key();
		if (dir == MOVE_NONE)
			OSTmrStart(&display_timer, &err); //重新启动定时器计时

		switch (Disp)
		{
		case Disp_Menu:
			move_task_menu(dir);//
			break;
		case Disp_About:
			move_task_about(dir);
			break;
		case Disp_Compass:
			move_task_compass(dir);
			break;
		case Disp_Setting:
			move_task_setting(dir);
			break;
		case Disp_Humidity:
			move_task_humidity(dir);
			break;
		default:
			break;
		}

		OSTimeDlyHMSM(0, 0, 0, 300, OS_OPT_TIME_HMSM_STRICT, &err); //延时300ms
	}
}

//time时间获取与更新任务，优先级12
void time_task(void *p_arg)
{
	OS_ERR err;
	RTC_Data_Type rtc_t;
	APP_TimeTypeDef time;
	APP_DateTypeDef date;

	p_arg = p_arg;

	PCF8563_ReadTime(&rtc_t);
	time.Hour = rtc_t.hour;
	time.Minute = rtc_t.min;
	time.Second = rtc_t.sec;

	app_init_time_data(&time); //这里需要先获取一次RTC时间，初始化显示

	while(1)
	{
		//获取时间间隔为500ms
		PCF8563_ReadTime(&rtc_t);
		time.Hour = rtc_t.hour;
		time.Minute = rtc_t.min;
		time.Second = rtc_t.sec;
		date.date = rtc_t.date;
		date.year = rtc_t.year;
		date.month = rtc_t.month;
		date.week = rtc_t.week;

		app_update_time(&time);										//更新时间
		app_update_date(&date);										//更新日期
		if(Disp != Disp_Setting)		//不在setting界面时更新roller
			app_setting_update_roller(&rtc_t);					//更新时间设置roller
		IWatch_Feed();		//喂狗
		OSTimeDlyHMSM(0, 0, 0, 500, OS_OPT_TIME_HMSM_STRICT, &err); //延时500ms
		
	}
}

//更新bme280数据
void humidity_task(void* p_arg)
{
	OS_ERR err;
	p_arg = p_arg;
	CPU_SR_ALLOC();	//需要用到临界区

	while(1)
	{
	OS_CRITICAL_ENTER();
	stream_sensor_data_normal_mode(&dev,&bmedata);	
	OS_CRITICAL_EXIT(); //退出临界区
	OSTimeDlyHMSM(0, 0, 0, 200, OS_OPT_TIME_HMSM_STRICT, &err); //延时200ms
	}
}


//led0任务函数，优先级15
void led0_task(void *p_arg)
{
	OS_ERR err;

	p_arg = p_arg;

	while (1)
	{
		app_time_point_state(true);									//显示时间的 :
		OSTimeDlyHMSM(0, 0, 0, 500, OS_OPT_TIME_HMSM_STRICT, &err); //延时500ms
		app_time_point_state(false);								//隐藏时间的 :
		OSTimeDlyHMSM(0, 0, 0, 500, OS_OPT_TIME_HMSM_STRICT, &err); //延时500ms
	}
}

//MOVE任务菜单界面时改变界面
static void move_task_menu_change_display(Display_TypeDef disp_t)
{
	OS_ERR err;
	
	switch (disp_t)
	{
	case Disp_Compass:
		app_compass_anim_Vexit(false); //将compass界面显示
		OSTaskResume((OS_TCB *)&CompassTaskTCB, &err);		//解挂COMPASS任务,开始更新指南针角度
		break;
	case Disp_Setting:
		app_setting_anim_Vexit(false); //将setting界面显示
		break;
	case Disp_About:
		app_about_anim_Vexit(false); //将about界面显示
		break;
	case Disp_Humidity:
		app_humidity_anim_Vexit(false);//显示温湿度气压信息
		break;
	default:
		break;
	} 

	app_menu_anim_Hexit(id, true); //移出菜单界面
}

//当前界面是菜单时MOVE任务要做的事情
static void move_task_menu(Move_DirTypeDef dir)
{
	switch (dir)
	{
	case MOVE_UP:	 //手臂往上移动
		if (id == 0) //当前界面是菜单第一个，向上之后是主界面 move_up为sw1
		{
			app_time_anim_Hexit(true);				//主界面进入
			app_menu_anim_Vexit((uint8_t)id, true); //菜单第一个向上(退出中心)
		}
		else if (id > 0)
		{
			app_menu_anim_Vexit((uint8_t)id - 1, true); //菜单第上一个向上(进入中心)
			app_menu_anim_Vexit((uint8_t)id, true);		//菜单当前向上(退出中心)
		}
		if (id == -1 && ag_t->imu_d.az > -4000)
			Power_Off();

		id = id <= 0 ? -1 : id - 1; //移动后id减一
		break;
	case MOVE_DOWN:	  //手臂往下移动
		if (id == -1) //当前界面是主菜单，下一个是菜单第一个
		{
			app_time_anim_Hexit(false);					   //主菜单退出
			app_menu_anim_Vexit((uint8_t)(id + 1), false); //下一个向上(进入中心)
		}
		else if (id <= 1)
		{
			app_menu_anim_Vexit((uint8_t)id, false);
			app_menu_anim_Vexit((uint8_t)(id + 1), false);
		}
		id = id >= 3 ? 3 : id + 1;
		break;
	case MOVE_RIGHT:
		if (id != -1)
		{
			Disp = (Display_TypeDef)id;
			move_task_menu_change_display(Disp); //根据下一个要显示的界面改变界面
		}
		break;
	default:
		break;
	}
}

//当前界面是about界面时MOVE任务要做的事情
static void move_task_about(Move_DirTypeDef dir)
{
	switch (dir)
	{
	case MOVE_LEFT:
		Disp = Disp_Menu;
		app_about_anim_Vexit(true);		//将about界面退出
		app_menu_anim_Hexit(id, false); //移入菜单界面
		break;
	default:
		break;
	}
}

//当前界面是setting界面时MOVE任务要做的事情
static void move_task_setting(Move_DirTypeDef dir)
{
	OS_ERR err;
	RTC_Data_Type rtc_t;
	static uint8_t index = 0;
	static uint8_t roller_index = 0;
	const uint8_t *roller_max = app_setting_get_roller_max();

	switch (dir)
	{
	case MOVE_LEFT:
		Disp = Disp_Menu;
		app_setting_anim_Vexit(true);	//将setting界面退出
		app_menu_anim_Hexit(id, false); //移入菜单界面
		break;
	case MOVE_UP:
		if(roller_index != 7)
		{
			app_setting_change(index);		//设置当前选中roller的项(递增)
			index = index >= roller_max[roller_index] - 1 ? 0 : index + 1;
		}
		else
		{
			if(ag_t->imu_d.az > -4000)		//当朝右倾斜，且z轴较接近水平时设置时间
			{
				app_setting_update_time(&rtc_t);		//将roller的数据更新到时间结构体
				PCF8563_WriteTime(&rtc_t);					//往PCF8563写入新的时间
				Disp = Disp_Menu;
				app_setting_anim_Vexit(true);	//将setting界面退出
				app_menu_anim_Hexit(id, false); //移入菜单界面
			}
		}
		break;
	case MOVE_DOWN:
		if(roller_index != 7)
		{
			app_setting_change(index);		//设置当前选中roller的项(递减)
			index = index < 1 ? roller_max[roller_index] - 1 : index - 1;
		}
		break;
	case MOVE_RIGHT:
		roller_index = roller_index >= 7  ? 0 : roller_index + 1;
		app_setting_change_roller(roller_index);		//设置当前选中的roller
		if(roller_index != 7)
			index = app_setting_get_roller_index();		//更新当前选中roller的当前选中项
	case MOVE_NONE:
			OSTmrStart(&display_timer, &err);		//重复开启息屏定时器, compass界面不息屏
			break;
	default:
		break;
	}
}

//当前界面是compass界面时MOVE任务要做的事情
static void move_task_compass(Move_DirTypeDef dir)
{
	OS_ERR err;
	
	switch (dir)
	{
		case MOVE_LEFT:
			Disp = Disp_Menu;
			OS_TaskSuspend((OS_TCB *)&CompassTaskTCB, &err); //挂起COMPASS任务
			app_compass_anim_Vexit(true);	//将aompass界面退出
			app_menu_anim_Hexit(id, false); //移入菜单界面
			break;
		case MOVE_NONE:
			OSTmrStart(&display_timer, &err);		//重复开启息屏定时器, compass界面不息屏
			break;
		default:
			break;
	}
}

static void move_task_humidity(Move_DirTypeDef dir)
{
	switch (dir)
	{
	case MOVE_LEFT:
		Disp = Disp_Menu;
		app_humidity_anim_Vexit(true);		//将about界面退出
		app_menu_anim_Hexit(id, false); //移入菜单界面
		break;
	default:
		break;
	}
}

//动作扫描函数
static Move_DirTypeDef Move_Scan(void)
{
	AngleGyro_TypeDef *ag_t;

	ag_t = Get_Angle_GyroxStructure();

	if ((ag_t->pitch < 160.0f) && (ag_t->pitch > 0))
		return MOVE_DOWN;
	else if ((ag_t->pitch > -160.0f) && (ag_t->pitch < 0))
		return MOVE_UP;
	else if ((ag_t->roll < 160.0f) && (ag_t->roll > 0))
		return MOVE_LEFT;
	else if ((ag_t->roll > -160.0f) && (ag_t->roll < 0))
		return MOVE_RIGHT;
	else
		return MOVE_NONE;
}
//按键读取按键 返回哪个按键被按下
static Move_DirTypeDef which_key(void)
{
	if (Key_Scan(SW1_PORT,SW1)==GPIO_PIN_RESET)//SW1向上移动
	{
		return MOVE_UP;
	}
	else if (Key_Scan(SW23_PORT,SW2)==GPIO_PIN_RESET)//SW2向下
	{
		return MOVE_DOWN;
	}
	else if (Key_Scan(SW23_PORT,SW3)==GPIO_PIN_RESET)//sw3向右，23同时按下向左（退出）
	{
		delay_ms(100);
		if(Key_Scan(SW23_PORT,SW2)==GPIO_PIN_RESET)
			return MOVE_LEFT;
		else
		return MOVE_RIGHT;
	}
	else 
	return MOVE_NONE;		
}


