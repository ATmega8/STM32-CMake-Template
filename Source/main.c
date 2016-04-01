#include <stdint.h>

#include "main.h"
#include "usart.h"
#include "circularbuffer.h"
#include "USART_ReadTask.h"

int main(void)
{
	int n;
	const uint8_t data[10] = {1, 2, 3, 4, 5,
								6, 7, 8, 9, 10};

	//TaskHandle_t USART_ReadTaskHandle;
	//TaskHandle_t MODBUS_CheckRequestTaskHandle;
	CircularBufferTypeDef* pcbuf;

	/*初始化串口*/
	if((fd = open("/dev/ttyUSB0", O_RDWR )) == -1)
	{
		printf("Serial Port Open Fail\n");
		return -1;
	}

	if(USART_Config(fd) == -1)
	{
		printf("Serial Port Configure Fail\n");
		return -1;
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

