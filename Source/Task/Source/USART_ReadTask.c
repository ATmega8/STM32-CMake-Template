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

void MODBUS_Reply(CircularBufferTypeDef* prxcbuf);

void MODBUS_CheckRequestTask(void* parameter)
{

	uint16_t CRCValue;
	uint32_t length;

	printf("%s %d: Enter MODBUS_CheckRequestTask \n", __FUNCTION__, __LINE__);

	while(1)
	{
		if(xQueueReceive(frameLengthQueue, &length, (TickType_t)MODBUS_WAIT) == pdTRUE )
		{

			MODBUSTypeDef* const pmodbus = MODBUS_Create();

			printf("check receive data CRC \n");

			MODBUS_SetModbusFrame(ptxcbuf, pmodbus, length);

			CRCValue = CRC_Calculate(pmodbus);

			/*对比 CRC */
			if( MODBUS_CRC(pmodbus) != CRCValue )
			{
				/*CRC 校验错误*/
				MODBUS_ErrorSend(pmodbus, MODBUS_CRCCheckError, errorQueue);
			}
			else /*CRC*/
			{
				if(MODBUS_Address(pmodbus) != ADDRESS)
				{
					/*从机地址错误*/
					MODBUS_ErrorSend(pmodbus, MODBUS_AddressError, errorQueue);
				}
				else /* ADDRESS */
				{
					switch(MODBUS_FunctionCode(pmodbus))
					{
						case 0x03: /*读寄存器*/
							if(MODBUS_ReadRegister(pmodbus, prxcbuf) == -1)
							{
								MODBUS_ErrorSend(pmodbus, MODBUS_AddressError, errorQueue);
							}
							break;

						case 0x06: /*写寄存器*/
							if(MODBUS_WriteRegister(pmodbus, prxcbuf) == -1)
							{
								MODBUS_ErrorSend(pmodbus, MODBUS_AddressError, errorQueue);
							}
							break;

						default:   /*Function Code Error*/
							MODBUS_ErrorSend(pmodbus, MODBUS_FunctionCodeError, errorQueue);
							break;
					} /*end switch*/
				} /* end address */
			} /*end CRC */
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

void MODBUS_Reply(CircularBufferTypeDef* prxcbuf)
{
	uint32_t i, data, len;
	uint32_t lastHead = 0;

	/*检查发送缓冲区状态*/
	if( CircularBuffer_Status(prxcbuf)  != CircularBuffer_Empty )
	{

		/*计算帧长*/
		len = (CircularBuffer_Length(prxcbuf) -  
			(lastHead - CircularBuffer_HeadPosition(prxcbuf)))%CircularBuffer_Length(prxcbuf);

		lastHead = CircularBuffer_HeadPosition(prxcbuf);

		/*使能驱动器*/
		MODBUS_EnableDriver();

		/*发送数据*/
		for( i = 0; i < len; i++)
		{
			/*读数据*/
			CircularBuffer_Read(prxcbuf, &data ,1);

			/*写数据*/
			USART_SendData(USART1, (uint8_t)data);

			/*等待完成*/
			while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
		}

		/*关闭驱动器*/
		MODBUS_DisableDriver();

	} /* end if */
}


