#ifndef _DMA_H_
#define _DMA_H_

#include "sys.h"


typedef struct
{
	DMA_Stream_TypeDef *Instance;		//DMAx数据流y	

	uint32_t Channel;              	//该数据流的通道x
  uint32_t Direction;            	//DMA传输方向
  uint32_t PeriphInc;           	//外设地址增量模式	
  uint32_t MemInc;              	//存储器地址增量模式
  uint32_t PeriphDataAlignment; 	//外设数据长度
  uint32_t MemDataAlignment;      //存储器数据长度
  uint32_t Mode;									//DMA传输模式，普通，循环，外设流控
	uint32_t Priority;							//优先级
}DMA_Transfer_InitTypeDef;


void DMA_Transfer_Init(DMA_HandleTypeDef *dma_handle, DMA_Transfer_InitTypeDef *dma_t);

#endif
