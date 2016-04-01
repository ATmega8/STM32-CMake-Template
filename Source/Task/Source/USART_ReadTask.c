#include <stdio.h>
#include <stdlib.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "circularbuffer.h"

extern int fd;
extern xSemaphoreHandle Sem_USART;

void USART_ReadTask(void* parameter)
{
	int n;
	int count = 0;
	uint8_t data;
	uint32_t cirbufData;

	CircularBufferTypeDef* pcbuf =
		(CircularBufferTypeDef*)parameter;

	printf("enter USART_ReadTask fd:%d\n", fd);

	while(1)
	{
		if((n = read(fd, &data, 1)  != -1))	
		{
			/*转换为uint32_t*/
			cirbufData = (uint32_t)data;

			CircularBuffer_Write(pcbuf, &cirbufData, 1);
			count += n;
		}

		if(count == 10)
		{
			xSemaphoreGive(Sem_USART);

			while(1)
			{

			}
		}
	}
}

void MODBUS_CheckRequestTask(void* parameter)
{
	int i = 0, data;

	CircularBufferTypeDef* pcbuf = 
		(CircularBufferTypeDef*)parameter;

	if(xSemaphoreTake(Sem_USART, (TickType_t)10) == pdTRUE )
	{
		printf("\n");
		printf("Start check receive data\n");
		printf("\n");
		printf("CircularBuffer Size:   %d\n", CircularBuffer_Size(pcbuf));
		printf("CircularBuffer Length: %d\n", CircularBuffer_Length(pcbuf));
		printf("CircularBuffer Head:   %d\n", CircularBuffer_HeadPosition(pcbuf));
		printf("CircularBuffer Tail:   %d\n", CircularBuffer_TailPosition(pcbuf));
		printf("\n");

		while(CircularBuffer_Status(pcbuf) != CircularBuffer_Empty)
		{
			CircularBuffer_Read(pcbuf, &data, 1);
			printf("CircularBuffer Count: %d Data: %d\n", i, data);
		}

		xSemaphoreGive( Sem_USART );
	}
	else
	{
		printf("Sem take timeout\n");
	}

	vTaskEndScheduler();
}

