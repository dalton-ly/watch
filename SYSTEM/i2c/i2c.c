#include "i2c.h"

//初始化I2C为主机模式
void I2Cx_Master_Init(I2C_HandleTypeDef *i2c_handle, I2C_Master_InitTypeDef *i2c_t)
{
	i2c_handle->Instance = i2c_t->Instance;
	i2c_handle->Init.ClockSpeed = i2c_t->ClockSpeed;
	i2c_handle->Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;		//7位器件地址
	i2c_handle->Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	i2c_handle->Init.DutyCycle = I2C_DUTYCYCLE_16_9;
	i2c_handle->Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	i2c_handle->Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	i2c_handle->Init.OwnAddress1 = 0;
	i2c_handle->Init.OwnAddress2 = 0;
	
	HAL_I2C_Init(i2c_handle); 
}

//I2C写1字节函数
void I2Cx_WriteByte(I2C_HandleTypeDef *i2c_handle, I2C_TransferType *i2c_t)
{
	uint8_t data[2];
	
	data[0] = i2c_t->regaddr;			//写寄存器地址
	data[1] = *(i2c_t->txdata);		//写数据
	
	HAL_I2C_Master_Transmit(i2c_handle, i2c_t->devaddr, data, 2, 1000);
}

//I2C读1字节函数
void I2Cx_ReadByte(I2C_HandleTypeDef *i2c_handle, I2C_TransferType *i2c_t)
{
	HAL_I2C_Mem_Read(i2c_handle, i2c_t->devaddr, i2c_t->regaddr, I2C_MEMADD_SIZE_8BIT, i2c_t->rxdata, 1, 1000);
}

//I2C连续读函数
void I2Cx_ReadLen(I2C_HandleTypeDef *i2c_handle, I2C_TransferType *i2c_t)
{
	HAL_I2C_Mem_Read(i2c_handle, i2c_t->devaddr, i2c_t->regaddr, I2C_MEMADD_SIZE_8BIT, i2c_t->rxdata, i2c_t->rxlen, 1000);
}

//I2C连续写函数
void I2Cx_WriteLen(I2C_HandleTypeDef *i2c_handle, I2C_TransferType *i2c_t)
{
	HAL_I2C_Mem_Write(i2c_handle, i2c_t->devaddr, i2c_t->regaddr, I2C_MEMADD_SIZE_8BIT, i2c_t->txdata, i2c_t->txlen, 1000);		//I2C连续写
}

//I2C初始化回调函数
void HAL_I2C_MspInit(I2C_HandleTypeDef *hi2c)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	if(hi2c->Instance == I2C1)
	{
		I2C1_GPIO_RCC_ENABLE();
		__HAL_RCC_I2C1_CLK_ENABLE();
		
		GPIO_InitStructure.Mode = GPIO_MODE_AF_OD;		//开漏复用输出
		GPIO_InitStructure.Pin = I2C1_SCL_SDA_Pin;
		GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
		GPIO_InitStructure.Pull = GPIO_PULLUP;
		GPIO_InitStructure.Alternate = GPIO_AF4_I2C1;
		HAL_GPIO_Init(I2C1_GPIO, &GPIO_InitStructure);
	}
	else if(hi2c->Instance == I2C2)
	{
		I2C2_GPIO_RCC_ENABLE();
		
		__HAL_RCC_I2C2_CLK_ENABLE();
		
		GPIO_InitStructure.Mode = GPIO_MODE_AF_OD;		//开漏复用输出
		GPIO_InitStructure.Pin = I2C2_SCL_Pin;
		GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
		GPIO_InitStructure.Pull = GPIO_PULLUP;
		GPIO_InitStructure.Alternate = I2C2_SCL_AF;
		HAL_GPIO_Init(I2C2_GPIO, &GPIO_InitStructure);
		
		GPIO_InitStructure.Pin = I2C2_SDA_Pin;
		GPIO_InitStructure.Alternate = I2C2_SDA_AF;
		HAL_GPIO_Init(I2C2_GPIO, &GPIO_InitStructure);
	}
}

