#include <stdio.h>
#include <stdlib.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "USART_ReadTask.h"
#include "CRC.h"
#include "modbus_define.h"

extern QueueHandle_t frameLengthQueue;

extern CircularBufferTypeDef* ptxcbuf;
extern CircularBufferTypeDef* prxcbuf;

extern QueueHandle_t errorQueue;

void MODBUS_ErrorSend(MODBUSTypeDef* pmodbus, MODBUSStatusTypeDef status, QueueHandle_t queue);

void MODBUS_CheckRequestTask(void* parameter)
{
	uint32_t length;

	printf("%s %d: Enter MODBUS_CheckRequestTask \n", __FUNCTION__, __LINE__);

	while(1)
	{
		if(xQueueReceive(frameLengthQueue, &length, (TickType_t)MODBUS_WAIT) == pdTRUE )
		{

			MODBUSTypeDef* const pmodbus = MODBUS_Create();
			MODBUS_SetModbusFrame(ptxcbuf, pmodbus, length);
			MODBUS_ProcessRequest(pmodbus, prxcbuf);
		}
		else /*Queue*/
		{ 

		} /* end queue */

		MODBUS_Reply(prxcbuf);

		taskYIELD();
	} /*end while(1) */

} /*end function */

void MODBUS_ErrorSend(MODBUSTypeDef* pmodbus, MODBUSStatusTypeDef status, QueueHandle_t queue)
{
	/*设置错误状态*/
	MODBUS_SetModbusStatus(pmodbus, status);

	/*发送至错误处理任务*/
	xQueueSend(queue, &pmodbus, (TickType_t)MODBUS_WAIT);

	/*创建错误处理任务*/
	xTaskCreate(MODBUS_ErrorProcessTask, "TASK3", 1024, NULL, 3, NULL);

	taskYIELD();
}

void MODBUS_ErrorProcessTask(void* parameter)
{
	MODBUSTypeDef* pmodbus;
	uint32_t buffer[5];
	uint32_t CRCValue = 0xFFFF;

	buffer[0] = 0x02;

	while(1)
	{
		if(xQueueReceive(errorQueue, &pmodbus, (TickType_t)MODBUS_WAIT) == pdTRUE)
		{
			
			buffer[1] = 0x80 | MODBUS_FunctionCode(pmodbus);

			switch(MODBUS_ModbusStatus(pmodbus))
			{
				case MODBUS_CRCCheckError:

					buffer[1] = 0x01;

					CRCValue = CRC_CalculateByte(CRCValue, buffer[0]);	
					CRCValue = CRC_CalculateByte(CRCValue, buffer[1]);

					CircularBuffer_Write(prxcbuf, &buffer, 4);
						
					free(pmodbus);
					break;

				case MODBUS_AddressError:

					buffer[2] = 0x02;

					CRCValue = CRC_CalculateByte(CRCValue, buffer[0]);	
					CRCValue = CRC_CalculateByte(CRCValue, buffer[1]);
					CRCValue = CRC_CalculateByte(CRCValue, buffer[2]);

					buffer[3] = (uint32_t)(CRCValue & 0x00FF);
					buffer[4] = (uint32_t)(CRCValue >> 8);

					CircularBuffer_Write(prxcbuf, &buffer, 5);

					free(MODBUS_DataPointer(pmodbus));
					free(pmodbus);
					break;

				case MODBUS_FunctionCodeError:
					printf("ERROR: Function Code ERROR!!!!\n");
					free(pmodbus);
					break;

				default:
					break;
			}

		}

		vTaskDelete(NULL);
	}
}

