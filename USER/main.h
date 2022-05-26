#ifndef __MAIN_H
#define __MAIN_H

#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "includes.h"
#include "lcd_init.h"
#include "lcd.h"
#include "timer.h"
#include "bat_adc.h"
#include "pcf8563.h"
#include "mpu9250.h"
#include "Kalman.h"
#include "watchdog.h"

#include "app_time.h"
#include "app_menu.h"
#include "app_about.h"
#include "app_setting.h"
#include "app_compass.h"
#include "app_humidity.h"

#include "lvgl.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"

#include "BME280.h"
typedef enum
{
	Disp_Menu = -1,
	Disp_Compass,
	Disp_Setting,
	Disp_About,
	Disp_Humidity
} Display_TypeDef; //当前界面枚举

typedef enum
{
	MOVE_NONE = 0,
	MOVE_LEFT,
	MOVE_RIGHT,
	MOVE_UP,
	MOVE_DOWN
} Move_DirTypeDef;

static Move_DirTypeDef which_key(void);

#define Power_Off() HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0 | GPIO_PIN_1, GPIO_PIN_RESET)
#define Power_On() HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0 | GPIO_PIN_1, GPIO_PIN_SET)

#define Charging_State() HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1)

//任务优先级
#define START_TASK_PRIO 3
//任务堆栈大小
#define START_STK_SIZE 512
//任务控制块
OS_TCB StartTaskTCB;
//任务堆栈
CPU_STK START_TASK_STK[START_STK_SIZE];
//任务函数
void start_task(void *p_arg);

//任务优先级
#define IMU_TASK_PRIO 5
//任务堆栈大小
#define IMU_STK_SIZE 512
//任务控制块
OS_TCB ImuTaskTCB;
//任务堆栈
CPU_STK IMU_TASK_STK[IMU_STK_SIZE];
void imu_task(void *p_arg);

//任务优先级
#define LVGL_TASK_PRIO 6
//任务堆栈大小
#define LVGL_STK_SIZE 1024 //8K
//任务控制块
OS_TCB LvglTaskTCB;
//任务堆栈
CPU_STK LVGL_TASK_STK[LVGL_STK_SIZE];
void lvgl_task(void *p_arg);

//任务优先级
#define COMPASS_TASK_PRIO 7
//任务堆栈大小
#define COMPASS_STK_SIZE 512
//任务控制块
OS_TCB CompassTaskTCB;
//任务堆栈
CPU_STK COMPASS_TASK_STK[COMPASS_STK_SIZE];
void compass_task(void *p_arg);

//任务优先级
#define TEMPERATURE_TASK_PRIO 8
//任务堆栈大小
#define TEMPERATURE_STK_SIZE 512
//任务控制块
OS_TCB TemperatureTaskTCB;
//任务堆栈
CPU_STK TEMPERATURE_TASK_STK[TEMPERATURE_STK_SIZE];
void temperature_task(void *p_arg);

//任务优先级
#define BAT_TASK_PRIO 9
//任务堆栈大小
#define BAT_STK_SIZE 128
//任务控制块
OS_TCB BatTaskTCB;
//任务堆栈
CPU_STK BAT_TASK_STK[BAT_STK_SIZE];
void bat_task(void *p_arg);

//任务优先级
#define MOVE_TASK_PRIO 10
//任务堆栈大小
#define MOVE_STK_SIZE 512
//任务控制块
OS_TCB MoveTaskTCB;
//任务堆栈
CPU_STK MOVE_TASK_STK[MOVE_STK_SIZE];
void move_task(void *p_arg);

//任务优先级
#define TIME_TASK_PRIO 12
//任务堆栈大小
#define TIME_STK_SIZE 512
//任务控制块
OS_TCB TimeTaskTCB;
//任务堆栈
CPU_STK TIME_TASK_STK[TIME_STK_SIZE];
void time_task(void *p_arg);


//定义湿度更新任务优先级
#define HUMIDITY_TASK_PRIO 13
//堆栈大小
#define HUMIDITY_STK_SIZE 512
//控制块
OS_TCB HumidityTaskTCB;
//堆栈
CPU_STK HUMIDITY_TASK_STK[HUMIDITY_STK_SIZE];
void humidity_task(void* p_arg);




//任务优先级
#define LED0_TASK_PRIO 15
//任务堆栈大小
#define LED0_STK_SIZE 128
//任务控制块
OS_TCB Led0TaskTCB;
//任务堆栈
CPU_STK LED0_TASK_STK[LED0_STK_SIZE];
void led0_task(void *p_arg);

//息屏软件定时器
OS_TMR display_timer;
void display_tim_callback(void *p_tmr, void *p_arg);

#endif
