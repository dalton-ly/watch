#include "lcd_init.h"
#include "delay.h"
#include "spi.h"
#include "dma.h"

static SPI_HandleTypeDef spi_handle; //SPI句柄

static DMA_HandleTypeDef spi_tx_dma_handle; //DMA句柄

void LCD_DeInit(void)
{
	HAL_DMA_DeInit(&spi_tx_dma_handle);
	HAL_SPI_DeInit(&spi_handle);
}

//初始化LCD使用到的一些GPIO
static void LCD_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	LCD_RES_GPIO_RCC_ENABLE(); //使能端口时钟
	LCD_DC_GPIO_RCC_ENABLE();
	LCD_CS_GPIO_RCC_ENABLE();
	LCD_BLK_GPIO_RCC_ENABLE();

	LCD_RES_OUT(1);
	GPIO_InitStructure.Pin = LCD_RES_PIN;			  //RES
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;	  //推挽输出
	GPIO_InitStructure.Speed = GPIO_SPEED_FAST;		  //速度快速
	HAL_GPIO_Init(LCD_RES_GPIO, &GPIO_InitStructure); //初始化GPIO

	LCD_DC_OUT(1);
	GPIO_InitStructure.Pin = LCD_DC_PIN;			 //DC
	HAL_GPIO_Init(LCD_DC_GPIO, &GPIO_InitStructure); //初始化GPIO

	LCD_CS_OUT(1);
	GPIO_InitStructure.Pin = LCD_CS_PIN;			 //CS
	HAL_GPIO_Init(LCD_CS_GPIO, &GPIO_InitStructure); //初始化GPIO

	LCD_DC_OUT(1);
	GPIO_InitStructure.Pin = LCD_BLK_PIN;			  //BLK
	HAL_GPIO_Init(LCD_BLK_GPIO, &GPIO_InitStructure); //初始化GPIO
}

//初始化LCD SPI TX DMA
static void LCD_DMA_Init(void)
{
	DMA_Transfer_InitTypeDef DMA_Transfer_InitStructure;

	__HAL_LINKDMA(&spi_handle, hdmatx, spi_tx_dma_handle); //连接外设句柄和DMA句柄

	DMA_Transfer_InitStructure.Instance = LCD_SPI_TX_DMA;
	DMA_Transfer_InitStructure.Channel = LCD_SPI_TX_DMA_CHANNEL;
	DMA_Transfer_InitStructure.Direction = LCD_SPI_TX_DMA_DIR;
	DMA_Transfer_InitStructure.MemDataAlignment = LCD_SPI_TX_DMA_MDA;
	DMA_Transfer_InitStructure.PeriphDataAlignment = LCD_SPI_TX_DMA_PDA;
	DMA_Transfer_InitStructure.MemInc = LCD_SPI_TX_DMA_MINC;
	DMA_Transfer_InitStructure.PeriphInc = LCD_SPI_TX_DMA_PINC;
	DMA_Transfer_InitStructure.Mode = LCD_SPI_TX_DMA_MODE;
	DMA_Transfer_InitStructure.Priority = LCD_SPI_TX_DMA_PRIORITY;

	DMA_Transfer_Init(&spi_tx_dma_handle, &DMA_Transfer_InitStructure); //初始化DMA

	HAL_NVIC_SetPriority(LCD_SPI_TX_DMA_IRQn, 1, 1); //发送DMAy数据流x中断优先级,抢占1，响应1
	HAL_NVIC_EnableIRQ(LCD_SPI_TX_DMA_IRQn);		 //开启发送DMAy数据x流中断
}

/*
* 功能: DMA中断服务函数,调用DMA中断公共处理函数
* 说明: 这是一个在lcd_init.h中的宏定义，实质是DMA2_Stream2_IRQHandler
*/
void LCD_SPI_TX_DMA_IRQHandler(void)
{
	HAL_DMA_IRQHandler(&spi_tx_dma_handle);
}

//初始化LCD使用的SPI接口
static void LCD_SPI_Init(void)
{
	SPI_Master_InitTypeDef SPI_Master_InitStructure;

	SPI_Master_InitStructure.Instance = LCD_SPI;
	SPI_Master_InitStructure.CLKPhase = LCD_SPI_CPHA;
	SPI_Master_InitStructure.CLKPolarity = LCD_SPI_CPOL;
	SPI_Master_InitStructure.Direction = LCD_SPI_DIR;
	SPI_Master_InitStructure.FirstBit = LCD_SPI_FIRSTBIT;
	SPI_Master_InitStructure.BaudRatePrescaler = LCD_SPI_PRESCALER;

	SPI_Master_Init(&spi_handle, &SPI_Master_InitStructure);
}

static void delay(uint8_t t)
{
	while (t--)
		;
}

//LCD串行数据写入
static void LCD_Writ_Bus(uint8_t dat)
{
	LCD_CS_OUT(0);

	SPIx_WriteByte(&spi_handle, dat); //SPI发送1字节

	delay(3); //令CS延时拉高，大约0.05us

	LCD_CS_OUT(1);
}

//LCD写入8位数据
void LCD_WR_DATA8(uint8_t dat)
{
	LCD_Writ_Bus(dat);
}

//LCD写入16位数据
void LCD_WR_DATA(uint16_t dat)
{
	LCD_Writ_Bus(dat >> 8);
	LCD_Writ_Bus(dat);
}

//LCD写入命令
void LCD_WR_REG(uint8_t dat)
{
	LCD_DC_OUT(0); //写命令
	LCD_Writ_Bus(dat);
	LCD_DC_OUT(1); //写数据
}

//启用SPI DMA连续发送单个16bit数据(存储器地址不自增)
void LCD_DMA_Transfer16Bit(uint8_t *pData, uint16_t size, DMA_MEMINC_STATE state)
{
	LCD_SPI_TX_DMA->CR &= ~(DMA_SxCR_MINC | DMA_SxCR_MSIZE | DMA_SxCR_PSIZE); //清除 存储器地址增量、存储器外设数据长度 的设置

	LCD_SPI_TX_DMA->CR |= DMA_SxCR_MSIZE_0 | DMA_SxCR_PSIZE_0; //存储器地址不自增、存储器和外设数据长度为半字(16bit)

	if (state == DMA_MEMINC_ENABLE)
		LCD_SPI_TX_DMA->CR |= DMA_SxCR_MINC; //存储器地址不自增、存储器和外设数据长度为半字(16bit)

	while (spi_handle.State != HAL_SPI_STATE_READY)
		; //等待SPI空闲

	HAL_SPI_Transmit_DMA(&spi_handle, pData, size); //启用DMA传输
}

//启用SPI DMA连续发送8bit数据(存储器地址自增)
void LCD_DMA_Transfer8Bit(uint8_t *pData, uint16_t size, DMA_MEMINC_STATE state)
{
	LCD_SPI_TX_DMA->CR &= ~(DMA_SxCR_MINC | DMA_SxCR_MSIZE | DMA_SxCR_PSIZE); //清除 存储器地址增量、存储器外设数据长度 的设置

	if (state == DMA_MEMINC_ENABLE)
		LCD_SPI_TX_DMA->CR |= DMA_SxCR_MINC; //存储器地址自增、存储器和外设数据长度为字节(8bit)

	while (spi_handle.State != HAL_SPI_STATE_READY)
		; //等待SPI空闲

	HAL_SPI_Transmit_DMA(&spi_handle, pData, size); //启用DMA传输
}

/*
*功能: 设置起始和结束地址
*参数1: @x1,x2 - 设置列的起始和结束地址
*参数1: @y1,y2 - 设置行的起始和结束地址
*/
void LCD_Address_Set(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
	if (USE_HORIZONTAL == 0)
	{
		LCD_WR_REG(0x2a); //列地址设置
		LCD_WR_DATA(LCD_ADDRESS_OFFEST_0 + x1);
		LCD_WR_DATA(LCD_ADDRESS_OFFEST_0 + x2);
		LCD_WR_REG(0x2b); //行地址设置
		LCD_WR_DATA(LCD_ADDRESS_OFFEST_1 + y1);
		LCD_WR_DATA(LCD_ADDRESS_OFFEST_1 + y2);
		LCD_WR_REG(0x2c); //储存器写
	}
	else if (USE_HORIZONTAL == 1)
	{
		LCD_WR_REG(0x2a); //列地址设置
		LCD_WR_DATA(LCD_ADDRESS_OFFEST_0 + x1);
		LCD_WR_DATA(LCD_ADDRESS_OFFEST_0 + x2);
		LCD_WR_REG(0x2b); //行地址设置
		LCD_WR_DATA(LCD_ADDRESS_OFFEST_1 + y1);
		LCD_WR_DATA(LCD_ADDRESS_OFFEST_1 + y2);
		LCD_WR_REG(0x2c); //储存器写
	}
	else if (USE_HORIZONTAL == 2)
	{
		LCD_WR_REG(0x2a); //列地址设置
		LCD_WR_DATA(LCD_ADDRESS_OFFEST_1 + x1);
		LCD_WR_DATA(LCD_ADDRESS_OFFEST_1 + x2);
		LCD_WR_REG(0x2b); //行地址设置
		LCD_WR_DATA(LCD_ADDRESS_OFFEST_0 + y1);
		LCD_WR_DATA(LCD_ADDRESS_OFFEST_0 + y2);
		LCD_WR_REG(0x2c); //储存器写
	}
	else
	{
		LCD_WR_REG(0x2a); //列地址设置
		LCD_WR_DATA(LCD_ADDRESS_OFFEST_1 + x1);
		LCD_WR_DATA(LCD_ADDRESS_OFFEST_1 + x2);
		LCD_WR_REG(0x2b); //行地址设置
		LCD_WR_DATA(LCD_ADDRESS_OFFEST_0 + y1);
		LCD_WR_DATA(LCD_ADDRESS_OFFEST_0 + y2);
		LCD_WR_REG(0x2c); //储存器写
	}
}

//LCD初始化
void LCD_Init(void)
{
	LCD_GPIO_Init(); //初始化GPIO
	LCD_SPI_Init();	 //初始化SPI1
	LCD_DMA_Init();	 //初始化DMA

	LCD_RES_OUT(0); //复位
	delay_ms(100);
	LCD_RES_OUT(1);
	delay_ms(100);

	LCD_BLK_OUT(1); //打开背光
	delay_ms(100);

#if USE_DRIVER == DRIVER_ST7735S
	LCD_WR_REG(0x11); //Sleep out
	delay_ms(120);	  //Delay 120ms
	LCD_WR_REG(0xB1); //Normal mode
	LCD_WR_DATA8(0x05);
	LCD_WR_DATA8(0x3C);
	LCD_WR_DATA8(0x3C);
	LCD_WR_REG(0xB2); //Idle mode
	LCD_WR_DATA8(0x05);
	LCD_WR_DATA8(0x3C);
	LCD_WR_DATA8(0x3C);
	LCD_WR_REG(0xB3); //Partial mode
	LCD_WR_DATA8(0x05);
	LCD_WR_DATA8(0x3C);
	LCD_WR_DATA8(0x3C);
	LCD_WR_DATA8(0x05);
	LCD_WR_DATA8(0x3C);
	LCD_WR_DATA8(0x3C);
	LCD_WR_REG(0xB4); //Dot inversion
	LCD_WR_DATA8(0x03);
	LCD_WR_REG(0xC0); //AVDD GVDD
	LCD_WR_DATA8(0xAB);
	LCD_WR_DATA8(0x0B);
	LCD_WR_DATA8(0x04);
	LCD_WR_REG(0xC1);	//VGH VGL
	LCD_WR_DATA8(0xC5); //C0
	LCD_WR_REG(0xC2);	//Normal Mode
	LCD_WR_DATA8(0x0D);
	LCD_WR_DATA8(0x00);
	LCD_WR_REG(0xC3); //Idle
	LCD_WR_DATA8(0x8D);
	LCD_WR_DATA8(0x6A);
	LCD_WR_REG(0xC4); //Partial+Full
	LCD_WR_DATA8(0x8D);
	LCD_WR_DATA8(0xEE);
	LCD_WR_REG(0xC5); //VCOM
	LCD_WR_DATA8(0x0F);
	LCD_WR_REG(0xE0); //positive gamma
	LCD_WR_DATA8(0x07);
	LCD_WR_DATA8(0x0E);
	LCD_WR_DATA8(0x08);
	LCD_WR_DATA8(0x07);
	LCD_WR_DATA8(0x10);
	LCD_WR_DATA8(0x07);
	LCD_WR_DATA8(0x02);
	LCD_WR_DATA8(0x07);
	LCD_WR_DATA8(0x09);
	LCD_WR_DATA8(0x0F);
	LCD_WR_DATA8(0x25);
	LCD_WR_DATA8(0x36);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x08);
	LCD_WR_DATA8(0x04);
	LCD_WR_DATA8(0x10);
	LCD_WR_REG(0xE1); //negative gamma
	LCD_WR_DATA8(0x0A);
	LCD_WR_DATA8(0x0D);
	LCD_WR_DATA8(0x08);
	LCD_WR_DATA8(0x07);
	LCD_WR_DATA8(0x0F);
	LCD_WR_DATA8(0x07);
	LCD_WR_DATA8(0x02);
	LCD_WR_DATA8(0x07);
	LCD_WR_DATA8(0x09);
	LCD_WR_DATA8(0x0F);
	LCD_WR_DATA8(0x25);
	LCD_WR_DATA8(0x35);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x09);
	LCD_WR_DATA8(0x04);
	LCD_WR_DATA8(0x10);

	LCD_WR_REG(0xFC);
	LCD_WR_DATA8(0x80);

	LCD_WR_REG(0x3A);
	LCD_WR_DATA8(0x05);
	LCD_WR_REG(0x36);
	if (USE_HORIZONTAL == 0)
		LCD_WR_DATA8(0x08);
	else if (USE_HORIZONTAL == 1)
		LCD_WR_DATA8(0xC8);
	else if (USE_HORIZONTAL == 2)
		LCD_WR_DATA8(0x78);
	else
		LCD_WR_DATA8(0xA8);
	LCD_WR_REG(0x21); //Display inversion
	LCD_WR_REG(0x29); //Display on
	LCD_WR_REG(0x2A); //Set Column Address
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x1A); //26
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x69); //105
	LCD_WR_REG(0x2B);	//Set Page Address
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x01); //1
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0xA0); //160
	LCD_WR_REG(0x2C);

#elif USE_DRIVER == DRIVER_GC9A01

#if LCD_H < 240 || LCD_W < 240
	LCD_WR_REG(0xFE);
	LCD_WR_REG(0xEF);
	LCD_WR_REG(0xEB);
	LCD_WR_DATA8(0x14);
	LCD_WR_REG(0x84);
	LCD_WR_DATA8(0x60);
	LCD_WR_REG(0x85);
	LCD_WR_DATA8(0xF7);
	LCD_WR_REG(0x86);
	LCD_WR_DATA8(0xFC);
	LCD_WR_REG(0x87);
	LCD_WR_DATA8(0x28);
	LCD_WR_REG(0x8E);
	LCD_WR_DATA8(0x0F);
	LCD_WR_REG(0x8F);
	LCD_WR_DATA8(0xFC);
	LCD_WR_REG(0x88);
	LCD_WR_DATA8(0x0A);
	LCD_WR_REG(0x89);
	LCD_WR_DATA8(0x21);
	LCD_WR_REG(0x8A);
	LCD_WR_DATA8(0x00);
	LCD_WR_REG(0x8B);
	LCD_WR_DATA8(0x80);
	LCD_WR_REG(0x8C);
	LCD_WR_DATA8(0x01);
	LCD_WR_REG(0x8D);
	LCD_WR_DATA8(0x01);
	LCD_WR_REG(0xB6);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x20);
	LCD_WR_REG(0x36);
	if (USE_HORIZONTAL == 0)
		LCD_WR_DATA8(0x08);
	else if (USE_HORIZONTAL == 1)
		LCD_WR_DATA8(0xC8);
	else if (USE_HORIZONTAL == 2)
		LCD_WR_DATA8(0x68);
	else
		LCD_WR_DATA8(0xA8);
	LCD_WR_REG(0x3A);
	LCD_WR_DATA8(0x05);
	LCD_WR_REG(0x90);
	LCD_WR_DATA8(0x08);
	LCD_WR_DATA8(0x08);
	LCD_WR_DATA8(0x08);
	LCD_WR_DATA8(0x08);
	LCD_WR_REG(0xBD);
	LCD_WR_DATA8(0x06);
	LCD_WR_REG(0xBC);
	LCD_WR_DATA8(0x00);
	LCD_WR_REG(0xFF);
	LCD_WR_DATA8(0x60);
	LCD_WR_DATA8(0x01);
	LCD_WR_DATA8(0x04);
	LCD_WR_REG(0xC3);
	LCD_WR_DATA8(0x25); //48
	LCD_WR_REG(0xC4);
	LCD_WR_DATA8(0x25);
	LCD_WR_REG(0xC9);
	LCD_WR_DATA8(0x25);
	LCD_WR_REG(0xBE);
	LCD_WR_DATA8(0x11);
	LCD_WR_REG(0xE1);
	LCD_WR_DATA8(0x10);
	LCD_WR_DATA8(0x0E);
	LCD_WR_REG(0xDF);
	LCD_WR_DATA8(0x21);
	LCD_WR_DATA8(0x10);
	LCD_WR_DATA8(0x02);
	LCD_WR_REG(0xf0);
	LCD_WR_DATA8(0x4b);
	LCD_WR_DATA8(0x0f);
	LCD_WR_DATA8(0x0A);
	LCD_WR_DATA8(0x0B);
	LCD_WR_DATA8(0x15);
	LCD_WR_DATA8(0x30);

	LCD_WR_REG(0xf1);
	LCD_WR_DATA8(0x43);
	LCD_WR_DATA8(0x70);
	LCD_WR_DATA8(0x72);
	LCD_WR_DATA8(0x36);
	LCD_WR_DATA8(0x37);
	LCD_WR_DATA8(0x6f);
	LCD_WR_REG(0xf2);
	LCD_WR_DATA8(0x4b);
	LCD_WR_DATA8(0x0f);
	LCD_WR_DATA8(0x0A);
	LCD_WR_DATA8(0x0B);
	LCD_WR_DATA8(0x15);
	LCD_WR_DATA8(0x30);

	LCD_WR_REG(0xf3);
	LCD_WR_DATA8(0x43);
	LCD_WR_DATA8(0x70);
	LCD_WR_DATA8(0x72);
	LCD_WR_DATA8(0x36);
	LCD_WR_DATA8(0x37);
	LCD_WR_DATA8(0x6f);
	LCD_WR_REG(0xED);
	LCD_WR_DATA8(0x1B);
	LCD_WR_DATA8(0x0B);
	LCD_WR_REG(0xAC);
	LCD_WR_DATA8(0x47);
	LCD_WR_REG(0xAE);
	LCD_WR_DATA8(0x77);
	LCD_WR_REG(0xCD);
	LCD_WR_DATA8(0x63);
	LCD_WR_REG(0x70);
	LCD_WR_DATA8(0x07);
	LCD_WR_DATA8(0x09);
	LCD_WR_DATA8(0x04);
	LCD_WR_DATA8(0x0C);
	LCD_WR_DATA8(0x0D);
	LCD_WR_DATA8(0x09);
	LCD_WR_DATA8(0x07);
	LCD_WR_DATA8(0x08);
	LCD_WR_DATA8(0x03);
	LCD_WR_REG(0xE8);
	LCD_WR_DATA8(0x14);
	/////////////////////////////////////////////
	LCD_WR_REG(0x60);
	LCD_WR_DATA8(0x38);
	LCD_WR_DATA8(0x0B);
	LCD_WR_DATA8(0x76);
	LCD_WR_DATA8(0x62);
	LCD_WR_DATA8(0x39);
	LCD_WR_DATA8(0xF0);
	LCD_WR_DATA8(0x76);
	LCD_WR_DATA8(0x62);
	LCD_WR_REG(0x61);
	LCD_WR_DATA8(0x38);
	LCD_WR_DATA8(0xF6);
	LCD_WR_DATA8(0x76);
	LCD_WR_DATA8(0x62);
	LCD_WR_DATA8(0x38);
	LCD_WR_DATA8(0xF7);
	LCD_WR_DATA8(0x76);
	LCD_WR_DATA8(0x62);
	/////////////////////////////////////
	LCD_WR_REG(0x62);
	LCD_WR_DATA8(0x38);
	LCD_WR_DATA8(0x0D);
	LCD_WR_DATA8(0x71);
	LCD_WR_DATA8(0xED);
	LCD_WR_DATA8(0x76);
	LCD_WR_DATA8(0x62);
	LCD_WR_DATA8(0x38);
	LCD_WR_DATA8(0x0F);
	LCD_WR_DATA8(0x71);
	LCD_WR_DATA8(0xEF);
	LCD_WR_DATA8(0x76);
	LCD_WR_DATA8(0x62);
	LCD_WR_REG(0x63);
	LCD_WR_DATA8(0x38);
	LCD_WR_DATA8(0x11);
	LCD_WR_DATA8(0x71);
	LCD_WR_DATA8(0xF1);
	LCD_WR_DATA8(0x76);
	LCD_WR_DATA8(0x62);
	LCD_WR_DATA8(0x38);
	LCD_WR_DATA8(0x13);
	LCD_WR_DATA8(0x71);
	LCD_WR_DATA8(0xF3);
	LCD_WR_DATA8(0x76);
	LCD_WR_DATA8(0x62);

	///////////////////////////////////////////////////////
	LCD_WR_REG(0x64);
	LCD_WR_DATA8(0x3b);
	LCD_WR_DATA8(0x29);
	LCD_WR_DATA8(0xF1);
	LCD_WR_DATA8(0x01);
	LCD_WR_DATA8(0xF1);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x0a);
	LCD_WR_REG(0x66);
	LCD_WR_DATA8(0x3C);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0xCD);
	LCD_WR_DATA8(0x67);
	LCD_WR_DATA8(0x45);
	LCD_WR_DATA8(0x45);
	LCD_WR_DATA8(0x10);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x00);
	LCD_WR_REG(0x67);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x3C);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x01);
	LCD_WR_DATA8(0x54);
	LCD_WR_DATA8(0x10);
	LCD_WR_DATA8(0x32);
	LCD_WR_DATA8(0x98);
	LCD_WR_REG(0xB5);
	LCD_WR_DATA8(0x08);
	LCD_WR_DATA8(0x09);
	LCD_WR_DATA8(0x14);
	LCD_WR_DATA8(0x08);
	LCD_WR_REG(0x74);
	LCD_WR_DATA8(0x10);
	LCD_WR_DATA8(0x85);
	LCD_WR_DATA8(0x80);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x4E);
	LCD_WR_DATA8(0x00);
	LCD_WR_REG(0x98);
	LCD_WR_DATA8(0x3e);
	LCD_WR_DATA8(0x07);
	LCD_WR_REG(0x35);
	LCD_WR_DATA8(0x00);
	LCD_WR_REG(0x21);
	LCD_WR_REG(0x11);
	delay_ms(120);
	LCD_WR_REG(0x29);
	delay_ms(10);
	LCD_WR_REG(0x2A);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x3C);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0xb3);
	LCD_WR_REG(0x2B);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0xEF);
	LCD_WR_REG(0x2C);

#else

	LCD_WR_REG(0xEF);
	LCD_WR_REG(0xEB);
	LCD_WR_DATA8(0x14);

	LCD_WR_REG(0xFE);
	LCD_WR_REG(0xEF);

	LCD_WR_REG(0xEB);
	LCD_WR_DATA8(0x14);

	LCD_WR_REG(0x84);
	LCD_WR_DATA8(0x40);

	LCD_WR_REG(0x85);
	LCD_WR_DATA8(0xFF);

	LCD_WR_REG(0x86);
	LCD_WR_DATA8(0xFF);

	LCD_WR_REG(0x87);
	LCD_WR_DATA8(0xFF);

	LCD_WR_REG(0x88);
	LCD_WR_DATA8(0x0A);

	LCD_WR_REG(0x89);
	LCD_WR_DATA8(0x21);

	LCD_WR_REG(0x8A);
	LCD_WR_DATA8(0x00);

	LCD_WR_REG(0x8B);
	LCD_WR_DATA8(0x80);

	LCD_WR_REG(0x8C);
	LCD_WR_DATA8(0x01);

	LCD_WR_REG(0x8D);
	LCD_WR_DATA8(0x01);

	LCD_WR_REG(0x8E);
	LCD_WR_DATA8(0xFF);

	LCD_WR_REG(0x8F);
	LCD_WR_DATA8(0xFF);

	LCD_WR_REG(0xB6);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x20);

	LCD_WR_REG(0x36);
	if (USE_HORIZONTAL == 0)
		LCD_WR_DATA8(0x08);
	else if (USE_HORIZONTAL == 1)
		LCD_WR_DATA8(0xC8);
	else if (USE_HORIZONTAL == 2)
		LCD_WR_DATA8(0x68);
	else
		LCD_WR_DATA8(0xA8);

	LCD_WR_REG(0x3A);
	LCD_WR_DATA8(0x05);

	LCD_WR_REG(0x90);
	LCD_WR_DATA8(0x08);
	LCD_WR_DATA8(0x08);
	LCD_WR_DATA8(0x08);
	LCD_WR_DATA8(0x08);

	LCD_WR_REG(0xBD);
	LCD_WR_DATA8(0x06);

	LCD_WR_REG(0xBC);
	LCD_WR_DATA8(0x00);

	LCD_WR_REG(0xFF);
	LCD_WR_DATA8(0x60);
	LCD_WR_DATA8(0x01);
	LCD_WR_DATA8(0x04);

	LCD_WR_REG(0xC3);
	LCD_WR_DATA8(0x13);
	LCD_WR_REG(0xC4);
	LCD_WR_DATA8(0x13);

	LCD_WR_REG(0xC9);
	LCD_WR_DATA8(0x22);

	LCD_WR_REG(0xBE);
	LCD_WR_DATA8(0x11);

	LCD_WR_REG(0xE1);
	LCD_WR_DATA8(0x10);
	LCD_WR_DATA8(0x0E);

	LCD_WR_REG(0xDF);
	LCD_WR_DATA8(0x21);
	LCD_WR_DATA8(0x0c);
	LCD_WR_DATA8(0x02);

	LCD_WR_REG(0xF0);
	LCD_WR_DATA8(0x45);
	LCD_WR_DATA8(0x09);
	LCD_WR_DATA8(0x08);
	LCD_WR_DATA8(0x08);
	LCD_WR_DATA8(0x26);
	LCD_WR_DATA8(0x2A);

	LCD_WR_REG(0xF1);
	LCD_WR_DATA8(0x43);
	LCD_WR_DATA8(0x70);
	LCD_WR_DATA8(0x72);
	LCD_WR_DATA8(0x36);
	LCD_WR_DATA8(0x37);
	LCD_WR_DATA8(0x6F);

	LCD_WR_REG(0xF2);
	LCD_WR_DATA8(0x45);
	LCD_WR_DATA8(0x09);
	LCD_WR_DATA8(0x08);
	LCD_WR_DATA8(0x08);
	LCD_WR_DATA8(0x26);
	LCD_WR_DATA8(0x2A);

	LCD_WR_REG(0xF3);
	LCD_WR_DATA8(0x43);
	LCD_WR_DATA8(0x70);
	LCD_WR_DATA8(0x72);
	LCD_WR_DATA8(0x36);
	LCD_WR_DATA8(0x37);
	LCD_WR_DATA8(0x6F);

	LCD_WR_REG(0xED);
	LCD_WR_DATA8(0x1B);
	LCD_WR_DATA8(0x0B);

	LCD_WR_REG(0xAE);
	LCD_WR_DATA8(0x77);

	LCD_WR_REG(0xCD);
	LCD_WR_DATA8(0x63);

	LCD_WR_REG(0x70);
	LCD_WR_DATA8(0x07);
	LCD_WR_DATA8(0x07);
	LCD_WR_DATA8(0x04);
	LCD_WR_DATA8(0x0E);
	LCD_WR_DATA8(0x0F);
	LCD_WR_DATA8(0x09);
	LCD_WR_DATA8(0x07);
	LCD_WR_DATA8(0x08);
	LCD_WR_DATA8(0x03);

	LCD_WR_REG(0xE8);
	LCD_WR_DATA8(0x34);

	LCD_WR_REG(0x62);
	LCD_WR_DATA8(0x18);
	LCD_WR_DATA8(0x0D);
	LCD_WR_DATA8(0x71);
	LCD_WR_DATA8(0xED);
	LCD_WR_DATA8(0x70);
	LCD_WR_DATA8(0x70);
	LCD_WR_DATA8(0x18);
	LCD_WR_DATA8(0x0F);
	LCD_WR_DATA8(0x71);
	LCD_WR_DATA8(0xEF);
	LCD_WR_DATA8(0x70);
	LCD_WR_DATA8(0x70);

	LCD_WR_REG(0x63);
	LCD_WR_DATA8(0x18);
	LCD_WR_DATA8(0x11);
	LCD_WR_DATA8(0x71);
	LCD_WR_DATA8(0xF1);
	LCD_WR_DATA8(0x70);
	LCD_WR_DATA8(0x70);
	LCD_WR_DATA8(0x18);
	LCD_WR_DATA8(0x13);
	LCD_WR_DATA8(0x71);
	LCD_WR_DATA8(0xF3);
	LCD_WR_DATA8(0x70);
	LCD_WR_DATA8(0x70);

	LCD_WR_REG(0x64);
	LCD_WR_DATA8(0x28);
	LCD_WR_DATA8(0x29);
	LCD_WR_DATA8(0xF1);
	LCD_WR_DATA8(0x01);
	LCD_WR_DATA8(0xF1);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x07);

	LCD_WR_REG(0x66);
	LCD_WR_DATA8(0x3C);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0xCD);
	LCD_WR_DATA8(0x67);
	LCD_WR_DATA8(0x45);
	LCD_WR_DATA8(0x45);
	LCD_WR_DATA8(0x10);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x00);

	LCD_WR_REG(0x67);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x3C);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x01);
	LCD_WR_DATA8(0x54);
	LCD_WR_DATA8(0x10);
	LCD_WR_DATA8(0x32);
	LCD_WR_DATA8(0x98);

	LCD_WR_REG(0x74);
	LCD_WR_DATA8(0x10);
	LCD_WR_DATA8(0x85);
	LCD_WR_DATA8(0x80);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x4E);
	LCD_WR_DATA8(0x00);

	LCD_WR_REG(0x98);
	LCD_WR_DATA8(0x3e);
	LCD_WR_DATA8(0x07);

	LCD_WR_REG(0x35);
	LCD_WR_REG(0x21);

	LCD_WR_REG(0x11);
	delay_ms(120);
	LCD_WR_REG(0x29);
	delay_ms(20);

#endif

#endif
}
