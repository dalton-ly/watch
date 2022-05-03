#include "dma.h"

//?????DMA????
void DMA_Transfer_Init(DMA_HandleTypeDef *dma_handle, DMA_Transfer_InitTypeDef *dma_t)
{
	if((uint32_t)(dma_t->Instance) < DMA2_BASE)		//??DMA1
		__HAL_RCC_DMA1_CLK_ENABLE();				
	else																					//??DMA2
		__HAL_RCC_DMA2_CLK_ENABLE();	
	
	dma_handle->Instance = dma_t->Instance;
	
	HAL_DMA_DeInit(dma_handle);		//??¦ËDMAx??????y
	
	dma_handle->Init.Channel = dma_t->Channel;				//???
	dma_handle->Init.Direction = dma_t->Direction;		//??????
	dma_handle->Init.PeriphInc = dma_t->PeriphInc;		//??????????
	dma_handle->Init.MemInc = dma_t->MemInc;					//?›¥?????????
	dma_handle->Init.PeriphDataAlignment = dma_t->PeriphDataAlignment;		//???????????
	dma_handle->Init.MemDataAlignment = dma_t->MemDataAlignment;		//?›¥?????????
	dma_handle->Init.Mode = dma_t->Mode;							//??????
	dma_handle->Init.Priority = dma_t->Priority;			//?????
	dma_handle->Init.FIFOMode = DMA_FIFOMODE_DISABLE;	//FIFO????
	dma_handle->Init.FIFOThreshold = DMA_FIFO_THRESHOLD_1QUARTERFULL;		//FIFO???
	dma_handle->Init.MemBurst = 0;		
	dma_handle->Init.PeriphBurst = 0;
	
	HAL_DMA_Init(dma_handle);		//?????DMAx??????y
	
	
}


