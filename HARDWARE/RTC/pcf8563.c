#include "pcf8563.h"
#include "i2c.h"

static I2C_HandleTypeDef rtc_i2c_handle;

//初始化PCF8563的I2C接口
void PCF8563_I2C_Init(void)
{
  I2C_Master_InitTypeDef i2c_m;

  i2c_m.ClockSpeed = PCF8563_SPEED;
  i2c_m.Instance = PCF8563_I2C_Type;

  I2Cx_Master_Init(&rtc_i2c_handle, &i2c_m); //初始化I2C2
}

void PCF8563_Clk_Output_Cmd(FunctionalState state)
{
  I2C_TransferType i2c_t;
  uint8_t data;

  if (state != DISABLE)
    data = 0x80;
  else
    data = 0x00;
  i2c_t.devaddr = PCF8563_ADDR;
  i2c_t.regaddr = 0x0D; //第一个0x0D为CLK输出控制寄存器
  i2c_t.txdata = &data;
  I2Cx_WriteByte(&rtc_i2c_handle, &i2c_t);
}

//启动/停止PCF8563
void PCF8563_Cmd(uint8_t cmd)
{
  I2C_TransferType i2c_t;
  uint8_t data = cmd;

  i2c_t.devaddr = PCF8563_ADDR;
  i2c_t.regaddr = 0x00; //第一个0x00为控制/状态寄存器1
  i2c_t.txdata = &data;
  I2Cx_WriteByte(&rtc_i2c_handle, &i2c_t);
}

//将BCD码转换成二进制数
//参数1 - @bcd: 指向bcd数据
//参数2 - @hex: 指向二进制数
//返回值: 1 - 成功  0 - 错误,非bcd码
uint8_t Convert_Bcd2Hex(uint8_t *bcd, uint8_t *hex)
{
  if ((*bcd & 0x0f) > 0x09)
  {
    return 0;
  }
  if ((*bcd & 0xf0) > 0x90)
  {
    return 0;
  }

  *hex = (*bcd & 0x0f);
  *hex += ((*bcd) >> 4) * 10;

  return 1;
}

//将保存在数组的BCD时间数据转换为RTC_Data_Type类型
//参数1 - @ptr: 指向bcd数据数组
//参数2 - @rtc_t: 指向RTC_Data_Type类型
void Ptr2Rtc(uint8_t *ptr, RTC_Data_Type *rtc_t)
{
  uint8_t i;
  uint8_t time_hex[11];

  //将BCD码转为二进制数据
  for (i = 0; i < 11; i++)
  {
    Convert_Bcd2Hex(ptr + i, time_hex + i);
  }

  //填充rtc_t
  rtc_t->sec = time_hex[0];         //秒
  rtc_t->min = time_hex[1];         //分
  rtc_t->hour = time_hex[2];        //时
  rtc_t->date = time_hex[3];        //日
  rtc_t->week = time_hex[4];        //星期
  rtc_t->month = time_hex[5];       //月
  rtc_t->year += time_hex[6];       //年
  rtc_t->alarm_min = time_hex[7];   //报警分
  rtc_t->alarm_hour = time_hex[8];  //报警时
  rtc_t->alarm_date = time_hex[9];  //报警日
  rtc_t->alarm_week = time_hex[10]; //报警星期
}

//从PCF8563中读取时间数据
//参数 - @rtc_t: 指向RTC时间数据结构体
void PCF8563_ReadTime(RTC_Data_Type *rtc_t)
{
  u8 temp, i;
  uint8_t ptr[11];
  I2C_TransferType i2c_t;

  i2c_t.devaddr = PCF8563_ADDR;
  i2c_t.regaddr = 0x02; //从秒寄存器开始连续读
  i2c_t.rxdata = ptr;
  i2c_t.rxlen = 7; //读实时时间寄存器7个

  /*读7个实时时间寄存器*/
  for (i = 0; i < 2; i++)
  {
    I2Cx_ReadLen(&rtc_i2c_handle, &i2c_t); //读取寄存器数据

    rtc_t->year = (ptr[5] & 0x80) ? 2000 : 1900; //判断月寄存器的世纪标志位

    ptr[0] &= 0x7F; //提取秒的BCD码
    ptr[1] &= 0x7F; //提取分的BCD码
    ptr[2] &= 0x3F; //提取时的BCD码
    ptr[3] &= 0x3F; //提取日的BCD码
    ptr[4] &= 0x07; //提取星期的BCD码
    ptr[5] &= 0x1F; //提取月的BCD码
    /*年的BCD码占8位，不需要提取*/

    i2c_t.regaddr = 0x03; //读分寄存器
    i2c_t.rxdata = &temp;
    I2Cx_ReadByte(&rtc_i2c_handle, &i2c_t);

    if ((temp & 0x7F) == ptr[1]) //两次分不相等再读一次
    {
      break;
    }
  }

  /*读4个报警时间寄存器*/
  i2c_t.regaddr = 0x09; //从分报警寄存器开始连续读
  i2c_t.rxdata = &ptr[7];
  i2c_t.rxlen = 4;                       //读报警时间寄存器4个
  I2Cx_ReadLen(&rtc_i2c_handle, &i2c_t); //读取寄存器数据

  /*设置各个报警标志位*/
  rtc_t->alarm_min_flag = (ptr[7] & 0X80) ? ALARM_SET : ALARM_RESET;
  rtc_t->alarm_hour_flag = (ptr[8] & 0X80) ? ALARM_SET : ALARM_RESET;
  rtc_t->alarm_date_flag = (ptr[9] & 0X80) ? ALARM_SET : ALARM_RESET;
  rtc_t->alarm_week_flag = (ptr[10] & 0X80) ? ALARM_SET : ALARM_RESET;

  ptr[7] &= 0x7F;  //提取报警分的BCD码
  ptr[8] &= 0x3F;  //提取报警时的BCD码
  ptr[9] &= 0x3F;  //提取报警日的BCD码
  ptr[10] &= 0x07; //提取报警星期的BCD码

  Ptr2Rtc(ptr, rtc_t); //将时间数据数组转换为RTC_Data_Type类型
}

//往PCF8563写入实时时间
//参数: @rtc_t - 指向RTC时间数据结构体
void PCF8563_WriteTime(RTC_Data_Type *rtc_t)
{
  uint8_t ptr[7]; //用于设置实时时间的数组
  I2C_TransferType i2c_t;

  /*将RTC_Data_Type类型转为BCD码数组*/
  ptr[0] = ((rtc_t->sec / 10) << 4) + rtc_t->sec % 10;                       //二进制秒转为BCD码
  ptr[1] = ((rtc_t->min / 10) << 4) + rtc_t->min % 10;                       //二进制分转为BCD码
  ptr[2] = ((rtc_t->hour / 10) << 4) + rtc_t->hour % 10;                     //二进制时转为BCD码
  ptr[3] = ((rtc_t->date / 10) << 4) + rtc_t->date % 10;                     //二进制日转为BCD码
  ptr[4] = ((rtc_t->week / 10) << 4) + rtc_t->week % 10;                     //二进制星期转为BCD码
  ptr[5] = ((rtc_t->month / 10) << 4) + rtc_t->month % 10;                   //二进制月转为BCD码
  ptr[5] |= rtc_t->year < 2000 ? 0x00 : 0x80;                                //在月BCD码的最高位加上世纪标志位
  if (rtc_t->year >= 2000)                                                   //20世纪
    ptr[6] = (((rtc_t->year - 2000) / 10) << 4) + (rtc_t->year - 2000) % 10; //二进制年转为BCD码
  else                                                                       //19世纪
    ptr[6] = (((2000 - rtc_t->year) / 10) << 4) + (2000 - rtc_t->year) % 10; //二进制年转为BCD码

  PCF8563_Cmd(PCF8563_CMD_STOP); //让PCF8563时钟芯停止运行

  i2c_t.devaddr = PCF8563_ADDR;
  i2c_t.regaddr = 0x02; //从秒寄存器开始连续写
  i2c_t.txdata = ptr;
  i2c_t.txlen = 7;                        //写实时时间寄存器7个
  I2Cx_WriteLen(&rtc_i2c_handle, &i2c_t); //将实时时间数据写入寄存器

  PCF8563_Cmd(PCF8563_CMD_START); //让PCF8563时钟芯开始运行
}
