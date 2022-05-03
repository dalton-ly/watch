#ifndef _SPI_H_
#define _SPI_H_

#include "sys.h"

#define SPI1_GPIO_RCC_ENABLE() __HAL_RCC_GPIOA_CLK_ENABLE()
#define SPI1_SCK_MOMI_Pin (GPIO_PIN_5 | GPIO_PIN_7)
#define SPI1_GPIO GPIOA

#define SPI2_GPIO_RCC_ENABLE() __HAL_RCC_GPIOB_CLK_ENABLE()
#define SPI2_SCK_MOMI_Pin (GPIO_PIN_13 | GPIO_PIN_15)
#define SPI2_GPIO GPIOB

typedef struct
{
	SPI_TypeDef *Instance;
	uint32_t Direction;
	uint32_t CLKPolarity;
	uint32_t CLKPhase;
	uint32_t FirstBit;
	uint32_t BaudRatePrescaler;
	
}SPI_Master_InitTypeDef;


void SPI_Master_Init(SPI_HandleTypeDef *spi_handle, SPI_Master_InitTypeDef *spi_t);
void SPIx_WriteByte(SPI_HandleTypeDef *spi_handle, uint8_t data);
void SPIx_WriteLen(SPI_TypeDef *spix, uint8_t *data, uint16_t len);

#endif
