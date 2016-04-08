#include "modbus_define.h"
#include "modbus_callback.h"
#include "CRC.h"
#include "motor.h"

#include <stdlib.h>

static uint32_t lastHead = 0;

inline MODBUSTypeDef* MODBUS_Create(void)
{
	return (MODBUSTypeDef*)malloc(sizeof(MODBUSTypeDef));
}

inline void MODBUS_SetAddress(MODBUSTypeDef* const pmodbus, const uint32_t address)
{
	pmodbus->address = address;
}

inline void MODBUS_SetFunctionCode(MODBUSTypeDef* const pmodbus, const uint32_t functionCode)
{
	pmodbus->functionCode = functionCode;
}

inline void MODBUS_SetDataPointer(MODBUSTypeDef* const pmodbus, uint32_t* pdata)
{
	pmodbus->data = pdata;
}

inline void MODBUS_SetDataLength(MODBUSTypeDef* const pmodbus, const uint32_t dataLength)
{
	pmodbus->dataLength = dataLength;
}

inline void MODBUS_SetCRC(MODBUSTypeDef* const pmodbus, const uint32_t CRCL, const uint32_t CRCH)
{
	pmodbus->CRCL = CRCL;
	pmodbus->CRCH = CRCH;
}

inline void MODBUS_SetModbusStatus(MODBUSTypeDef* const pmodbus, const MODBUSStatusTypeDef status)
{
	pmodbus->status = status;
}

inline uint32_t MODBUS_Address(MODBUSTypeDef* const pmodbus)
{
	return pmodbus->address;
}

inline uint32_t MODBUS_FunctionCode(MODBUSTypeDef* const pmodbus)
{
	return pmodbus->functionCode;
}

inline uint32_t MODBUS_DataLength(MODBUSTypeDef* const pmodbus)
{
	return pmodbus->dataLength;
}

inline uint32_t* MODBUS_DataPointer(MODBUSTypeDef* const pmodbus)
{
	return pmodbus->data;
}

inline uint16_t MODBUS_CRC(MODBUSTypeDef* const pmodbus)
{
	return (uint16_t)((pmodbus->CRCH << 8) | (pmodbus->CRCL));
}

inline MODBUSStatusTypeDef MODBUS_ModbusStatus(MODBUSTypeDef* const pmodbus)
{
	return pmodbus->status;
}

void MODBUS_EnableDriver(void)
{
	GPIO_SetBits(MODBUS_DE_GPIO_PORT, MODBUS_DE_GPIO_PIN);
}

void MODBUS_DisableDriver(void)
{
	GPIO_ResetBits(MODBUS_DE_GPIO_PORT, MODBUS_DE_GPIO_PIN);
}

uint16_t CRC_Calculate(MODBUSTypeDef* const pmodbus)
{
	uint16_t i, CRCValue = IBM_CRC16_INIT_VALUE;
	const uint32_t* pdata = MODBUS_DataPointer(pmodbus);

	CRCValue = CRC_CalculateByte(CRCValue, MODBUS_Address(pmodbus));
	CRCValue = CRC_CalculateByte(CRCValue, MODBUS_FunctionCode(pmodbus));

	for(i = 0; i < MODBUS_DataLength(pmodbus); i++)
		CRCValue = CRC_CalculateByte(CRCValue, (uint8_t)*(pdata + i));

	return CRCValue;
}

inline void MODBUS_RegisterUpdate(void)
{
	modbusRegisters[0] = 0;
	modbusRegisters[1] = ADDRESS;
	modbusRegisters[2] = 0;
}

inline uint32_t MODBUS_RegisterAddress(MODBUSTypeDef* const pmodbus)
{
	uint32_t* pdata;
	uint32_t registerAddress;

	pdata = MODBUS_DataPointer(pmodbus);

	/*高位*/
	registerAddress =  *(pdata + 0) << 8;

	/*低位*/
	registerAddress |= *(pdata + 1);

	return registerAddress;
}

inline uint32_t MODBUS_ReadRegisterLength(MODBUSTypeDef* const pmodbus)
{
	uint32_t* pdata;
	uint32_t registerLength;

	pdata = MODBUS_DataPointer(pmodbus);

	/*高位*/
	registerLength  =  *(pdata + 2) << 8;

	/*低位*/
	registerLength |= *(pdata + 3);

	return registerLength;
}

inline uint32_t MODBUS_WriteRegisterData(MODBUSTypeDef* const pmodbus)
{
	uint32_t* pdata;
	uint32_t registerData;

	pdata = MODBUS_DataPointer(pmodbus);

	/*高位*/
	registerData   =  *(pdata + 2) << 8;

	/*低位*/
	registerData  |= *(pdata + 3);

	return registerData;
}

void MODBUS_SetModbusFrame(CircularBufferTypeDef* pcbuf, MODBUSTypeDef* const pmodbus, uint32_t length)
{
	uint32_t data, data2;

	/*帧长度*/
	MODBUS_SetDataLength(pmodbus, length - 4);
	MODBUS_SetDataPointer(pmodbus, (uint32_t*)malloc(sizeof(uint32_t)*(length - 4)));

	/*从机地址*/
	CircularBuffer_Read(pcbuf, &data, 1);
	MODBUS_SetAddress(pmodbus, data);

	/*功能代码*/
	CircularBuffer_Read(pcbuf, &data, 1);
	MODBUS_SetFunctionCode(pmodbus, data);

	/*数据*/
	CircularBuffer_Read(pcbuf, MODBUS_DataPointer(pmodbus), MODBUS_DataLength(pmodbus));

	/*CRC*/
	CircularBuffer_Read(pcbuf, &data, 1);
	CircularBuffer_Read(pcbuf, &data2, 1);
	MODBUS_SetCRC(pmodbus, data, data2);

	/*Call Back Function*/
	pmodbus->readCallBack[0] = MODBUS_ReadSlaveAddress;
	pmodbus->readCallBack[1] = MODBUS_ReadDepthStatus;
	pmodbus->readCallBack[2] = MODBUS_ReadDepthSpeed;

	pmodbus->writeCallBack[0] = NULL;
	pmodbus->writeCallBack[1] = MODBUS_SetDepthStatus;
	pmodbus->writeCallBack[2] = MODBUS_SetDepthSpeed;

	/*MODBUS 状态*/
	MODBUS_SetModbusStatus(pmodbus, MODBUS_OK);
}

void MODBUS_ProcessRequest(MODBUSTypeDef* pmodbus, CircularBufferTypeDef* prxcbuf)
{
	uint16_t CRCValue;

	/*检查CRC*/
	CRCValue = CRC_Calculate(pmodbus);

	/*对比 CRC */
	if( MODBUS_CRC(pmodbus) != CRCValue )
	{
		/*CRC 校验失败*/
		return;
	}

	/*检查请求地址*/
	if((MODBUS_Address(pmodbus) == ADDRESS) 
			|| (MODBUS_Address(pmodbus) == 0x00))
	{
		/*检查功能代码*/	
		switch(MODBUS_FunctionCode(pmodbus))
		{
			case 0x03: /*Read Hold Register*/
				MODBUS_ProcessReadHoldRegister(pmodbus, prxcbuf);
				break;

			case 0x06: /*Write Single Register*/
				MODBUS_ProcessWriteSingleRegister(pmodbus, prxcbuf);
				break;

			default:  /*不支持的功能代码, 异常代码：01*/
				/*error handler*/
				break;

		}/* end function code*/
	}
	else/*else address*/
	{
		/*从机地址错误*/
		/*无响应*/

	}/*end address*/
}

void MODBUS_ProcessReadHoldRegister(MODBUSTypeDef* pmodbus, CircularBufferTypeDef* prxcbuf)
{
	uint32_t  registerAddress;
	uint32_t  registerLength;
	uint32_t  data, i;
	uint16_t  CRCValue;
	uint32_t* pdata; 

	/*寄存器地址*/
	registerAddress = MODBUS_RegisterAddress(pmodbus);

	/*数据长度*/
	registerLength = MODBUS_ReadRegisterLength(pmodbus);

	/*校验数据长度*/
	if(registerLength >= 0x7D)
	{
		/*异常代码：03*/
		return;
	}

	/*校验起始地址*/
	if((registerAddress < START_ADDRESS) || 
			(registerAddress > ADDRESS_LENGTH + START_ADDRESS) || 
			(registerAddress + registerLength > ADDRESS_LENGTH + START_ADDRESS))
	{
		/*异常代码：02*/
		return;
	}

	/*分配数据空间，最高位为读出数据长度×2*/
	pdata = malloc(sizeof(uint32_t)*registerLength*2 + 1);
	*pdata = registerLength*2;

	/*读出数据*/
	for(i = 0; i < registerLength; i++)
	{
		data  = (*(pmodbus->readCallBack[registerAddress+i]))();

		if(data == 0xFFFFFFFF)
		{
			/*异常代码：04*/
			return;
		}

		/*高位*/
		*(pdata + 2*i + 1) = (0x0000FF00 & data) >> 8;

		/*低位*/
		*(pdata + 2*i + 2) = (0x000000FF & data);
	}

	/*释放之前内存空间*/
	free(MODBUS_DataPointer(pmodbus));

	/*构造回复帧*/
	MODBUS_SetDataPointer(pmodbus, pdata);
	MODBUS_SetDataLength(pmodbus, registerLength*2 + 1);

	/*计算CRC*/
	CRCValue = CRC_Calculate(pmodbus);

	MODBUS_SetCRC(pmodbus, (CRCValue & 0x00FF), (CRCValue >> 8));

	/*写入缓冲区*/
	data = MODBUS_Address(pmodbus);
	CircularBuffer_Write(prxcbuf, &data, 1);

	data = MODBUS_FunctionCode(pmodbus);
	CircularBuffer_Write(prxcbuf, &data, 1);
	
	CircularBuffer_Write(prxcbuf, MODBUS_DataPointer(pmodbus), MODBUS_DataLength(pmodbus));

	data = CRCValue & 0x00FF;
	CircularBuffer_Write(prxcbuf, &data, 1);

	data = CRCValue  >> 8;
	CircularBuffer_Write(prxcbuf, &data, 1);

	/*释放内存*/
	free(MODBUS_DataPointer(pmodbus));
	free(pmodbus);
}

void MODBUS_ProcessWriteSingleRegister(MODBUSTypeDef* pmodbus, CircularBufferTypeDef* prxcbuf)
{
	uint32_t  registerAddress;
	uint32_t  registerValue;
	uint32_t  CRCValue;
	uint32_t  data;

	registerAddress = MODBUS_RegisterAddress(pmodbus);
	registerValue = MODBUS_WriteRegisterData(pmodbus);

	if(registerValue > 0xFFFF)
	{
		/*异常代码：03*/
		return;
	}

	/*地址寄存器0x00只读不可写*/
	if((registerAddress <= START_ADDRESS) || 
			(registerAddress > START_ADDRESS + ADDRESS_LENGTH))
	{
		/*异常代码：02*/
		return;
	}

	/*写入数据*/
	(*(pmodbus->writeCallBack[registerAddress])) (registerValue);

	/*构造回复PDU*/
	/*回复数据与请求相同*/
	/*写入缓冲区*/
	data = MODBUS_Address(pmodbus);
	CircularBuffer_Write(prxcbuf, &data, 1);

	data = MODBUS_FunctionCode(pmodbus);
	CircularBuffer_Write(prxcbuf, &data, 1);
	
	CircularBuffer_Write(prxcbuf, MODBUS_DataPointer(pmodbus), MODBUS_DataLength(pmodbus));

	CRCValue = MODBUS_CRC(pmodbus);

	data = CRCValue & 0x00FF;
	CircularBuffer_Write(prxcbuf, &data, 1);

	data = CRCValue  >> 8;
	CircularBuffer_Write(prxcbuf, &data, 1);

	free(MODBUS_DataPointer(pmodbus));
	free(pmodbus);

}

void MODBUS_Reply(CircularBufferTypeDef* prxcbuf)
{
	uint32_t i, data, len;

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
