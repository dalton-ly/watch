#include "i2c.h"

//模拟I2C BME相关的函数
/*******************************************************************************
* 函 数 名         : i2c_start
* 函数功能         : iic开始信号,SCL高电平时，SDA出现一个下跳沿表示启动信号 
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void i2c_Start(void)
{
    SDA_OUT();  
    I2C_SDA_UP;
    I2C_SCL_UP;
    delay_us(4);
    I2C_SDA_LOW;        //启动开始信号
    delay_us(4);
    I2C_SCL_LOW;      //钳住I2C总线，准备发送或接收数据
}


/*******************************************************************************
* 函 数 名         : i2c_stop
* 函数功能         : iic停止信号,SCL高电平时，SDA出现一个上跳沿表示停止信号
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void i2c_Stop(void)
{
    SDA_OUT();            //sda线输出
    I2C_SDA_LOW;         //发送结束条件的数据信号
    I2C_SCL_UP;         //拉高时钟信号
    delay_us(4);         //结束条件建立时间大于4μ
    I2C_SDA_UP;         //发送I2C总线结束信号 scl为高时sda有个上升沿
    delay_us(4);
    I2C_SCL_LOW; 
}


/*******************************************************************************
* 函 数 名         : i2c_send
* 函数功能         : iic发送数据
* 输    入         : uint8_t dat,要发送的数据
* 输    出         : 无
*******************************************************************************/
void i2c_SendByte(uint8_t dat)
{

     unsigned char temp;
     SDA_OUT();
   for(temp=0x80;temp!=0;temp>>=1)
   {
       if((temp & dat)== 0)
       {
          I2C_SDA_LOW;
       }
       else
       {
          I2C_SDA_UP;
       }
       delay_us(1);
       I2C_SCL_UP;
       delay_us(4);
       I2C_SCL_LOW;
   }
}




/*******************************************************************************
* 函 数 名         : i2c_read
* 函数功能         : iic接收数据
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
uint8_t i2c_read(void)//只读取一个字节，换成四个字节
{
   unsigned char temp;
   unsigned char dat;
   I2C_SDA_UP;                     //释放总线准备接收
   SDA_IN();
   for(temp=0x80;temp!=0;temp>>=1)//0x80:1000 0000 逐位右移在函数中挨个比较1
   {
      delay_us(1);
      I2C_SCL_UP;
      if(I2C_SDA==1)
      {
         dat|=temp;   //位或，dat初始任意，与1位或保留自身 sda读取到1则保留自身
      }
      else
      {
         dat&=~temp; //sda读取到0则与0111 1111 做位与，其他位不变，第一位值零
      }
      I2C_SCL_LOW;
   }
   return dat;
}


/*******************************************************************************
* 函 数 名         : i2c_wait_ack
* 函数功能         : iic等待应答
* 输    入         : 无
* 输    出         : 0/1，返回1表示无应答信号，返回0表示应答
*******************************************************************************/
uint8_t i2c_WaitAck(void)
{
    
    uint8_t con=0;
    
    I2C_SDA_UP;       //释放数据线，准备接收应答
    delay_us(1);
    I2C_SCL_UP;                //CPU驱动SCL = 1, 此时器件会返回ACK应答
    SDA_IN(); 
    delay_us(1);
    
  while(I2C_SDA)    //CPU读取SDA口线状态
  {
        con++;
        if(con>255)
        {
            i2c_stop();
      		return 1;     //无应答信号
        }
  }

  I2C_SCL_LOW; 
  delay_us(1);
  return 0;             //有应答
}

/*******************************************************************************
* 函 数 名         : i2c_ack
* 函数功能         : iic应答信号
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void i2c_ack(void)
{
    SDA_OUT();
    I2C_SDA_LOW;
    delay_us(1);
    I2C_SCL_UP;            //CPU产生1个时钟
    delay_us(4);         //时钟低电平周期大于4μ
    I2C_SCL_LOW;         //清时钟线，钳住I2C总线以便继续接收
    delay_us(4);
    I2C_SDA_UP;            //CPU释放SDA总线
}


/*******************************************************************************
* 函 数 名         : I2C_No_ack
* 函数功能         : iic非应答信号
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void I2C_No_ack(void)
{
   SDA_OUT();
   I2C_SDA_UP;
   delay_us(1);
   I2C_SCL_UP;
   delay_us(4);
   I2C_SCL_LOW; 
   delay_us(4);
}


/*******************************************************************************
* 函 数 名         : SDA_OUT
* 函数功能         : SDA 设置为输出
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void SDA_OUT(void)
{ 
    
    /*    HAL库使用，HAL库注意要把初始化函数的静态标记去掉    */
  GPIO_InitTypeDef GPIO_InitStruct;
  GPIO_InitStruct.Pin = GPIO_PIN_8;                
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}


/*******************************************************************************
* 函 数 名         : SDA_IN
* 函数功能         : SDA 设置为输入
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void SDA_IN(void)  
{
    /*    HAL库使用, HAL库注意要把初始化函数的静态标记去掉    */
  GPIO_InitTypeDef GPIO_InitStruct;
  GPIO_InitStruct.Pin = GPIO_PIN_8;                 
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
} 


/*******************************************************************************
* 函 数 名         : delay_us
* 函数功能         : 延时函数，延时us
* 输    入         : i
* 输    出         : 无
*******************************************************************************/
void delay_us(uint32_t i)
{
    uint32_t temp;
    SysTick->LOAD=9*i;                     //设置重装数值, 72MHZ时
    SysTick->CTRL=0X01;                 //使能，减到零是无动作，采用外部时钟源
    SysTick->VAL=0;                 //清零计数器
    
    do
    {
        temp=SysTick->CTRL;                            //读取当前倒计数值
    }
    while((temp&0x01)&&(!(temp&(1<<16))));     //等待时间到达
        
    SysTick->CTRL=0;         //关闭计数器
    SysTick->VAL=0;        //清空计数器
}





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

