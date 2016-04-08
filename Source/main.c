#include <stdint.h>

#include "main.h"
#include "usart.h"
#include "USART_ReadTask.h"

#include "stm32f4xx_conf.h"

#include "motor.h"

void IdleTask(void* parameter)
{
	while(1)
	{

	}
}

int main(void)
{
	MOTOR_Init();
	MOTOR_Start();

	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );
	USART_Config();

	/*初始化信号量*/
	frameLengthQueue = xQueueCreate(7, sizeof(uint32_t));
	errorQueue = xQueueCreate(7, sizeof(MODBUSTypeDef*));


	/*初始化环形缓冲区*/
	ptxcbuf = CircularBuffer_Create(56, sizeof(uint32_t));
	prxcbuf = CircularBuffer_Create(56, sizeof(uint32_t));

	/*更新 MODBUS 寄存器*/
	MODBUS_RegisterUpdate();

	if(frameLengthQueue == NULL)
		printf("%s %d: Queue Create Fault!", __FUNCTION__, __LINE__ );

	/*任务栈大小必须大于 configMINIMAL_STACK_SIZE !!!! */
	xTaskCreate(MODBUS_CheckRequestTask, "TASK2", 1024, NULL,  1, NULL);

	/*开始调度*/
	vTaskStartScheduler();
	
	while(1)
	{

	}
}

