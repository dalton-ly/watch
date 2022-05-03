#include "spi.h"


//初始化SPIx为主机模式，8位数据
void SPI_Master_Init(SPI_HandleTypeDef *spi_handle, SPI_Master_InitTypeDef *spi_t)
{
	uint8_t data[1] = {0xff};
	
	spi_handle->Instance = spi_t->Instance;
	spi_handle->Init.Mode = SPI_MODE_MASTER;
	spi_handle->Init.BaudRatePrescaler = spi_t->BaudRatePrescaler;
	spi_handle->Init.CLKPhase = spi_t->CLKPhase;
	spi_handle->Init.CLKPolarity = spi_t->CLKPolarity;
	spi_handle->Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	spi_handle->Init.CRCPolynomial = 7;
	spi_handle->Init.DataSize = SPI_DATASIZE_8BIT;
	spi_handle->Init.Direction = spi_t->Direction;
	spi_handle->Init.FirstBit = spi_t->FirstBit;
	spi_handle->Init.NSS = SPI_NSS_SOFT;
	spi_handle->Init.TIMode = SPI_TIMODE_DISABLE;
	
	HAL_SPI_Init(spi_handle);
	HAL_SPI_Transmit(spi_handle, data, 1, 1000);
}

//SPIx发送一byte数据
void SPIx_WriteByte(SPI_HandleTypeDef *spi_handle, uint8_t data)
{
	//HAL_SPI_Transmit(&spi_handle, &data, 1, 1000);		//HAL库字节之间间隔2.3us
	
	
	while((spi_handle->Instance->SR & 1 << 1 ) == 0);//等待发送区空	 
	spi_handle->Instance->DR = data;	 	  //发送一个byte
	while((spi_handle->Instance->SR & 1 << 1 ) == 0);//等待发送区空	 	
}

//SPI初始化回调函数
void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	if(hspi->Instance == SPI1)
	{
		__HAL_RCC_SPI1_CLK_ENABLE();
		SPI1_GPIO_RCC_ENABLE();
		
		GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStructure.Pin = SPI1_SCK_MOMI_Pin;
		GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
		GPIO_InitStructure.Pull = GPIO_PULLUP;
		GPIO_InitStructure.Alternate = GPIO_AF5_SPI1;
		HAL_GPIO_Init(SPI1_GPIO, &GPIO_InitStructure);
	}
	else if(hspi->Instance == SPI2)
	{
		__HAL_RCC_SPI2_CLK_ENABLE();
		SPI2_GPIO_RCC_ENABLE();
		
		GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStructure.Pin = SPI2_SCK_MOMI_Pin;
		GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
		GPIO_InitStructure.Pull = GPIO_PULLUP;
		GPIO_InitStructure.Alternate = GPIO_AF5_SPI2;
		HAL_GPIO_Init(SPI2_GPIO, &GPIO_InitStructure);
	}
}




