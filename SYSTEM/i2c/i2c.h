#ifndef _I2C_H_
#define _I2C_H_

#include "sys.h"

#define I2C1_GPIO_RCC_ENABLE() __HAL_RCC_GPIOB_CLK_ENABLE()
#define I2C1_SCL_SDA_Pin (GPIO_PIN_6 | GPIO_PIN_7)
#define I2C1_GPIO GPIOB

#define I2C2_GPIO_RCC_ENABLE() __HAL_RCC_GPIOB_CLK_ENABLE()
#define I2C2_SCL_Pin (GPIO_PIN_10)
#define I2C2_SDA_Pin (GPIO_PIN_3)
#define I2C2_SDA_AF GPIO_AF9_I2C2
#define I2C2_SCL_AF GPIO_AF4_I2C2

#define I2C2_GPIO GPIOB

//I2C主机模式初始化
typedef struct
{
	I2C_TypeDef *Instance;		//使用的I2C
	uint32_t ClockSpeed;			//I2C通信速率
}I2C_Master_InitTypeDef;

typedef struct
{
  uint8_t devaddr;    //从机地址
  uint8_t regaddr;    //要读写的起始地址
  uint8_t *rxdata;    //指向接收缓冲区
  uint8_t *txdata;    //指向发送缓冲区
  uint16_t rxlen;     //多字节读的长度 - 单字节读时无用
  uint16_t txlen;     //多字节写的长度 - 单字节写时无用
}I2C_TransferType;    //I2C传输结构类型


void I2Cx_Master_Init(I2C_HandleTypeDef *i2c_handle, I2C_Master_InitTypeDef *i2c_t);
void I2Cx_ReadByte(I2C_HandleTypeDef *i2c_handle, I2C_TransferType *i2c_t);
void I2Cx_ReadLen(I2C_HandleTypeDef *i2c_handle, I2C_TransferType *i2c_t);
void I2Cx_WriteByte(I2C_HandleTypeDef *i2c_handle, I2C_TransferType *i2c_t);
void I2Cx_WriteLen(I2C_HandleTypeDef *i2c_handle, I2C_TransferType *i2c_t);

#endif
