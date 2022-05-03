#ifndef __LCD_INIT_H
#define __LCD_INIT_H

#include "sys.h"

#define DRIVER_ST7735S 0u //控制器为ST7735S的LCD
#define DRIVER_GC9A01 1u  //控制器为GC9A01的LCD

#define USE_HORIZONTAL 0		 //设置横屏或者竖屏显示 0或1为竖屏 2或3为横屏
#define USE_DRIVER DRIVER_GC9A01 //设置LCD的驱动器
#define X_MAX 240				 //LCD排线朝下时的X和Y最大像素点
#define Y_MAX 240

#if USE_HORIZONTAL == 0 || USE_HORIZONTAL == 1
#define LCD_W X_MAX
#define LCD_H Y_MAX

#else
#define LCD_W Y_MAX
#define LCD_H X_MAX
#endif

/**********************************SPI相关**************************************************/
#if USE_DRIVER == DRIVER_GC9A01 || USE_DRIVER == DRIVER_ST7735S

#define LCD_SPI SPI1
#define LCD_SPI_CPHA SPI_PHASE_2EDGE			  //数据在第二个跳变沿采样
#define LCD_SPI_CPOL SPI_POLARITY_HIGH			  //时钟空闲时为高电平
#define LCD_SPI_DIR SPI_DIRECTION_1LINE			  //1线发送
#define LCD_SPI_FIRSTBIT SPI_FIRSTBIT_MSB		  //高位在前
#define LCD_SPI_PRESCALER SPI_BAUDRATEPRESCALER_4 //时钟为外设总线时钟的4分频，如果是SPI2可以2分频

#else

#error "there is a wrong driver of transfer"

#endif
/**********************************SPI相关**************************************************/

/**********************************DMA相关**************************************************/

#define LCD_SPI_TX_DMA DMA2_Stream2					//DMA2数据流2
#define LCD_SPI_TX_DMA_CHANNEL DMA_CHANNEL_2		//通道2
#define LCD_SPI_TX_DMA_DIR DMA_MEMORY_TO_PERIPH		//存储器到外设传输
#define LCD_SPI_TX_DMA_PINC DMA_PINC_DISABLE		//外设地址不增量
#define LCD_SPI_TX_DMA_MINC DMA_MINC_DISABLE		//存储器地址不增量
#define LCD_SPI_TX_DMA_PDA DMA_PDATAALIGN_HALFWORD	//外设数据长度16位
#define LCD_SPI_TX_DMA_MDA DMA_MDATAALIGN_HALFWORD	//存储器数据长度16位
#define LCD_SPI_TX_DMA_MODE DMA_NORMAL				//普通传输模式
#define LCD_SPI_TX_DMA_PRIORITY DMA_PRIORITY_MEDIUM //优先级中等

#define LCD_SPI_TX_DMA_IRQn DMA2_Stream2_IRQn			  //SPI发送DMAx数据流y中断
#define LCD_SPI_TX_DMA_IRQHandler DMA2_Stream2_IRQHandler //SPI发送DMAx数据流y中断服务函数

/**********************************DMA相关**************************************************/

/**********************************普通IO相关**************************************************/
#define LCD_RES_GPIO GPIOA
#define LCD_RES_PIN GPIO_PIN_6
#define LCD_RES_GPIO_RCC_ENABLE() __HAL_RCC_GPIOA_CLK_ENABLE()

#define LCD_DC_GPIO GPIOA
#define LCD_DC_PIN GPIO_PIN_4
#define LCD_DC_GPIO_RCC_ENABLE() __HAL_RCC_GPIOA_CLK_ENABLE()

#define LCD_CS_GPIO GPIOA
#define LCD_CS_PIN GPIO_PIN_3
#define LCD_CS_GPIO_RCC_ENABLE() __HAL_RCC_GPIOA_CLK_ENABLE()

#define LCD_BLK_GPIO GPIOA
#define LCD_BLK_PIN GPIO_PIN_2
#define LCD_BLK_GPIO_RCC_ENABLE() __HAL_RCC_GPIOA_CLK_ENABLE()

#define PIN_OUT(PORT, PIN, STATUS) (PORT)->BSRR = (STATUS) ? (PIN) : (uint32_t)(PIN) << 16

#define LCD_RES_OUT(STATUS) PIN_OUT(LCD_RES_GPIO, LCD_RES_PIN, STATUS)
#define LCD_DC_OUT(STATUS) PIN_OUT(LCD_DC_GPIO, LCD_DC_PIN, STATUS)
#define LCD_CS_OUT(STATUS) PIN_OUT(LCD_CS_GPIO, LCD_CS_PIN, STATUS)
#define LCD_BLK_OUT(STATUS) PIN_OUT(LCD_BLK_GPIO, LCD_BLK_PIN, STATUS)

/**********************************普通IO相关**************************************************/

/**********************************LCD参数相关**************************************************/
#if USE_DRIVER == DRIVER_ST7735S

#define LCD_ADDRESS_OFFEST_0 0x1a
#define LCD_ADDRESS_OFFEST_1 0x01

#elif USE_DRIVER == DRIVER_GC9A01

#if LCD_H < 240 || LCD_W < 240

#define LCD_ADDRESS_OFFEST_0 0x3c
#define LCD_ADDRESS_OFFEST_1 0x00

#else

#define LCD_ADDRESS_OFFEST_0 0
#define LCD_ADDRESS_OFFEST_1 0

#endif

#endif
/**********************************LCD参数相关**************************************************/

typedef enum
{
	DMA_MEMINC_ENABLE = 0,
	DMA_MEMINC_DISABLE
} DMA_MEMINC_STATE;

void LCD_WR_DATA8(uint8_t dat);													   //写入一个字节
void LCD_WR_DATA(uint16_t dat);													   //写入两个字节
void LCD_WR_REG(uint8_t dat);													   //写入一个指令
void LCD_DMA_Transfer16Bit(uint8_t *pData, uint16_t size, DMA_MEMINC_STATE state); //启动DMA连续发送单个16bit数据
void LCD_DMA_Transfer8Bit(uint8_t *pData, uint16_t size, DMA_MEMINC_STATE state);  //启动DMA连续发送8bit数据
void LCD_Address_Set(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);		   //设置坐标函数
void LCD_Init(void);															   //LCD初始化
void LCD_DeInit(void);
#endif
