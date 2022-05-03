#ifndef _PCF8563_H
#define _PCF8563_H

#include "sys.h"

#define PCF8563_ADDR  0xA2     //10100010
#define PCF8563_I2C_Type I2C2
#define PCF8563_SPEED 400000		//通信速率

#define PCF8563_CMD_START 0X00
#define PCF8563_CMD_STOP 0X20

enum{ALARM_RESET = 0, ALARM_SET}; //报警标志

typedef struct 
{
  uint16_t year;  //年
  uint8_t month;  //月
  uint8_t date;   //日
  uint8_t hour;   //时
  uint8_t min;    //分
  uint8_t sec;    //秒
  uint8_t week;   //星期
  uint8_t alarm_min;  //分报警
  uint8_t alarm_hour; //时报警
  uint8_t alarm_date; //日报警
  uint8_t alarm_week; //星期报警
  uint8_t alarm_min_flag; //分报警标志位
  uint8_t alarm_hour_flag; //分报警标志位
  uint8_t alarm_date_flag; //分报警标志位
  uint8_t alarm_week_flag; //分报警标志位
}RTC_Data_Type; //RTC时间数据结构类型

void PCF8563_I2C_Init(void);
void PCF8563_Start(uint8_t cmd);
void PCF8563_Clk_Output_Cmd(FunctionalState state);
uint8_t Convert_Bcd2Hex(uint8_t *bcd, uint8_t *hex);
void Ptr2Rtc(uint8_t *ptr, RTC_Data_Type *rtc_t);
void PCF8563_ReadTime(RTC_Data_Type *rtc_t);
void PCF8563_WriteTime(RTC_Data_Type *rtc_t);

#endif
