#ifndef _DMA_H_
#define _DMA_H_

#include "sys.h"


typedef struct
{
	DMA_Stream_TypeDef *Instance;		//DMAx������y	

	uint32_t Channel;              	//����������ͨ��x
  uint32_t Direction;            	//DMA���䷽��
  uint32_t PeriphInc;           	//�����ַ����ģʽ	
  uint32_t MemInc;              	//�洢����ַ����ģʽ
  uint32_t PeriphDataAlignment; 	//�������ݳ���
  uint32_t MemDataAlignment;      //�洢�����ݳ���
  uint32_t Mode;									//DMA����ģʽ����ͨ��ѭ������������
	uint32_t Priority;							//���ȼ�
}DMA_Transfer_InitTypeDef;


void DMA_Transfer_Init(DMA_HandleTypeDef *dma_handle, DMA_Transfer_InitTypeDef *dma_t);

#endif
