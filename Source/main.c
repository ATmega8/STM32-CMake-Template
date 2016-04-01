#include <stdint.h>

#include "main.h"
#include "usart.h"
#include "USART_ReadTask.h"

#include "stm32f4xx_conf.h"

void IdleTask(void* parameter)
{
	while(1)
	{

	}
}

	/*写入数据*/
	if( (n = write(fd, &data[0], sizeof(data)) == -1))
	{
		printf("Serial Write Error\n");
		return -1;
	}
	
	/*初始化环形缓冲区*/
	pcbuf = CircularBuffer_Create(32, sizeof(uint32_t));

	/*初始化信号量*/
	Sem_USART = xSemaphoreCreateBinary();

	if(Sem_USART == NULL)
	{
		printf("Semaphore Create Error\n");
		return -1;
	}

	/*创建串口读出任务*/
	//USART_ReadTaskHandle = 
		xTaskCreate(USART_ReadTask, "UASRT Read Task", 100, pcbuf, 1, NULL);

	/*if( USART_ReadTaskHandle == NULL)
	{
		printf("USART_ReadTask Create Fail\n");
		return -1;
	}*/

	//MODBUS_CheckRequestTaskHandle =
		xTaskCreate(MODBUS_CheckRequestTask, "CheckRequest", 100, pcbuf, 
				1, NULL);

	/*if( MODBUS_CheckRequestTaskHandle == NULL)
	{
		printf("MODBUS_CheckRequestTask Create Fail\n");
		return -1;
	}*/

	/*开始调度*/
	vTaskStartScheduler();
}

