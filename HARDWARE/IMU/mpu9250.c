#include "mpu9250.h"
#include "i2c.h"
#include "delay.h"

static I2C_HandleTypeDef mpu_i2c_handle;

static void MPU_IIC_Init(void); //I2C接口初始化

//其他,错误代码
uint8_t MPU9250_Init(void)
{
	uint8_t res = 0;

	MPU_IIC_Init();											 //初始化IIC总线
	MPU_Write_Byte(MPU9250_ADDR, MPU_PWR_MGMT1_REG, 0X80);	 //复位MPU9250
	delay_ms(100);											 //延时100ms
	MPU_Write_Byte(MPU9250_ADDR, MPU_PWR_MGMT1_REG, 0X00);	 //唤醒MPU9250
	MPU_Set_Gyro_Fsr(3);									 //陀螺仪传感器,±2000dps
	MPU_Set_Accel_Fsr(0);									 //加速度传感器,±2g
	MPU_Set_Rate(1000);										 //设置采样率1KHz
	MPU_Write_Byte(MPU9250_ADDR, MPU_INT_EN_REG, 0x00);		 //关闭数据中断
	MPU_Write_Byte(MPU9250_ADDR, MPU_USER_CTRL_REG, 0X00);	 //I2C主模式关闭
	MPU_Write_Byte(MPU9250_ADDR, MPU_FIFO_EN_REG, 0X00);	 //关闭FIFO
	MPU_Write_Byte(MPU9250_ADDR, MPU_INTBP_CFG_REG, 0X82);	 //INT引脚低电平有效，开启bypass模式，可以直接读取磁力计
	MPU_Write_Byte(MPU9250_ADDR, MPU_I2CMST_CTRL_REG, 0X00); //
	res = MPU_Read_Byte(MPU9250_ADDR, MPU_DEVICE_ID_REG);	 //读取MPU6500的ID
	if (res == MPU6500_ID1 || res == MPU6500_ID2)			 //器件ID正确
	{
		MPU_Write_Byte(MPU9250_ADDR, MPU_PWR_MGMT1_REG, 0X01); //设置CLKSEL,PLL X轴为参考
		MPU_Write_Byte(MPU9250_ADDR, MPU_PWR_MGMT2_REG, 0X00); //加速度与陀螺仪都工作
		MPU_Set_Rate(1000);									   //设置采样率为1KHz
	}
	else
		return 2;

	res = MPU_Read_Byte(AK8963_ADDR, MAG_WIA); //读取AK8963 ID
	if (res == AK8963_ID)
	{
		MPU_Write_Byte(AK8963_ADDR, MAG_CNTL2, 0X01); //复位AK8963
		delay_ms(50);
		MPU_Write_Byte(AK8963_ADDR, MAG_CNTL1, 0X01); //设置AK8963为单次测量
	}
	else
		return 1;

	return 0;
}

//设置MPU9250陀螺仪传感器满量程范围
//fsr:0,±250dps;1,±500dps;2,±1000dps;3,±2000dps
//返回值:0,设置成功
//    其他,设置失败
uint8_t MPU_Set_Gyro_Fsr(uint8_t fsr)
{
	return MPU_Write_Byte(MPU9250_ADDR, MPU_GYRO_CFG_REG, (fsr << 3) | 3); //设置陀螺仪满量程范围
}
//设置MPU9250加速度传感器满量程范围
//fsr:0,±2g;1,±4g;2,±8g;3,±16g
//返回值:0,设置成功
//    其他,设置失败
uint8_t MPU_Set_Accel_Fsr(uint8_t fsr)
{
	return MPU_Write_Byte(MPU9250_ADDR, MPU_ACCEL_CFG_REG, fsr << 3); //设置加速度传感器满量程范围
}

//设置MPU9250的数字低通滤波器
//lpf:数字低通滤波频率(Hz)
//返回值:0,设置成功
//    其他,设置失败
uint8_t MPU_Set_LPF(u16 lpf)
{
	uint8_t data = 0;
	if (lpf >= 188)
		data = 1;
	else if (lpf >= 98)
		data = 2;
	else if (lpf >= 42)
		data = 3;
	else if (lpf >= 20)
		data = 4;
	else if (lpf >= 10)
		data = 5;
	else
		data = 6;
	return MPU_Write_Byte(MPU9250_ADDR, MPU_CFG_REG, data); //设置数字低通滤波器
}

//设置MPU9250的采样率(假定Fs=1KHz)
//rate:4~1000(Hz)
//返回值:0,设置成功
//    其他,设置失败
uint8_t MPU_Set_Rate(u16 rate)
{
	uint8_t data;
	if (rate > 1000)
		rate = 1000;
	if (rate < 4)
		rate = 4;
	data = 1000 / rate - 1;
	data = MPU_Write_Byte(MPU9250_ADDR, MPU_SAMPLE_RATE_REG, data); //设置数字低通滤波器
	return MPU_Set_LPF(rate / 2);									//自动设置LPF为采样率的一半
}

//得到温度值
//返回值:温度值(扩大了100倍)
short MPU_Get_Temperature(void)
{
	uint8_t buf[2];
	short raw;
	float temp;
	MPU_Read_Len(MPU9250_ADDR, MPU_TEMP_OUTH_REG, 2, buf);
	raw = ((u16)buf[0] << 8) | buf[1];
	temp = 21 + ((double)raw) / 333.87;
	return temp * 100;
}
//得到陀螺仪值(原始值)
//gx,gy,gz:陀螺仪x,y,z轴的原始读数(带符号)
//返回值:0,成功
//    其他,错误代码
uint8_t MPU_Get_Gyroscope(short *gx, short *gy, short *gz)
{
	uint8_t buf[6], res;
	res = MPU_Read_Len(MPU9250_ADDR, MPU_GYRO_XOUTH_REG, 6, buf);
	if (res == 0)
	{
		*gx = ((u16)buf[0] << 8) | buf[1];
		*gy = ((u16)buf[2] << 8) | buf[3];
		*gz = ((u16)buf[4] << 8) | buf[5];
	}
	return res;
	;
}
//得到加速度值(原始值)
//gx,gy,gz:陀螺仪x,y,z轴的原始读数(带符号)
//返回值:0,成功
//    其他,错误代码
uint8_t MPU_Get_Accelerometer(short *ax, short *ay, short *az)
{
	uint8_t buf[6], res;
	res = MPU_Read_Len(MPU9250_ADDR, MPU_ACCEL_XOUTH_REG, 6, buf);
	if (res == 0)
	{
		*ax = ((u16)buf[0] << 8) | buf[1];
		*ay = ((u16)buf[2] << 8) | buf[3];
		*az = ((u16)buf[4] << 8) | buf[5];
	}
	return res;
	;
}

//得到磁力计值(原始值)
//mx,my,mz:磁力计x,y,z轴的原始读数(带符号)
//返回值:0,成功
//    其他,错误代码
uint8_t MPU_Get_Magnetometer(short *mx, short *my, short *mz)
{
	uint8_t buf[6], res;
	
	MPU_Write_Byte(MPU9250_ADDR, MPU_INTBP_CFG_REG, 0X82);	 //INT引脚低电平有效，开启bypass模式，可以直接读取磁力计
	delay_ms(10);
	MPU_Write_Byte(AK8963_ADDR, MAG_CNTL1, 0X01); //AK8963设置为单次测量模式
	delay_ms(10);
	
	res = MPU_Read_Len(AK8963_ADDR, MAG_XOUT_L, 6, buf);
	if (res == 0)
	{
		*mx = ((u16)buf[1] << 8) | buf[0];
		*my = ((u16)buf[3] << 8) | buf[2];
		*mz = ((u16)buf[5] << 8) | buf[4];
	}
//	MPU_Write_Byte(AK8963_ADDR, MAG_CNTL1, 0X01); //AK8963每次读完以后都需要重新设置为单次测量模式
	return res;
}

//IIC连续写
//addr:器件地址
//reg:寄存器地址
//len:写入长度
//buf:数据区
//返回值:0,正常
//    其他,错误代码
uint8_t MPU_Write_Len(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *buf)
{
	I2C_TransferType i2c_t;

	i2c_t.devaddr = addr << 1;
	i2c_t.regaddr = reg;   //写入的起始地址
	i2c_t.txdata = buf;	   //数据起始地址
	i2c_t.txlen = len;	   //写入的长度
	I2Cx_WriteLen(&mpu_i2c_handle, &i2c_t); //I2C连续写入

	return 0;
}

//IIC连续读
//addr:器件地址
//reg:要读取的寄存器地址
//len:要读取的长度
//buf:读取到的数据存储区
//返回值:0,正常
//    其他,错误代码
uint8_t MPU_Read_Len(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *buf)
{
	I2C_TransferType i2c_t;

	i2c_t.devaddr = addr << 1;
	i2c_t.regaddr = reg;  //读数据起始地址
	i2c_t.rxdata = buf;	  //读出到RAM的起始地址
	i2c_t.rxlen = len;	  //读出的数据长度
	I2Cx_ReadLen(&mpu_i2c_handle, &i2c_t); //读取寄存器数据

	return 0;
}

//IIC写一个字节
//reg:寄存器地址
//data:数据
//返回值:0,正常
//    其他,错误代码
uint8_t MPU_Write_Byte(uint8_t addr, uint8_t reg, uint8_t data)
{
	I2C_TransferType i2c_t;

	i2c_t.devaddr = addr << 1;
	i2c_t.regaddr = reg;  //写入的地址
	i2c_t.txdata = &data; //写入的数据
	I2Cx_WriteByte(&mpu_i2c_handle, &i2c_t);

	return 0;
}

//IIC读一个字节
//reg:寄存器地址
//返回值:读到的数据
uint8_t MPU_Read_Byte(uint8_t addr, uint8_t reg)
{
	uint8_t data = 0;
	I2C_TransferType i2c_t;

	i2c_t.devaddr = addr << 1;
	i2c_t.regaddr = reg;  //写入的地址
	i2c_t.rxdata = &data; //写入的数据
	I2Cx_ReadByte(&mpu_i2c_handle, &i2c_t);

	return data;
}

//初始化PCF8563的I2C接口
static void MPU_IIC_Init(void)
{
  I2C_Master_InitTypeDef i2c_m;

  i2c_m.ClockSpeed = MPU_SPEED;
  i2c_m.Instance = MPU_I2C_Type;

  I2Cx_Master_Init(&mpu_i2c_handle, &i2c_m); //初始化I2C1
}

