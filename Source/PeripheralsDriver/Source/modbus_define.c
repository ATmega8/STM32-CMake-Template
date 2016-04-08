#include "modbus_define.h"
#include "CRC.h"
#include "motor.h"

#include <stdlib.h>

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

	/*MODBUS 状态*/
	MODBUS_SetModbusStatus(pmodbus, MODBUS_OK);
}

int MODBUS_ReadRegister(MODBUSTypeDef* const pmodbus, CircularBufferTypeDef* prxcbuf)
{
	uint32_t  registerAddress;
	uint32_t  registerLength;
	uint32_t  CRCValue;
	uint32_t  data;
	uint32_t  i;
	uint32_t* pdata;

	pdata = MODBUS_DataPointer(pmodbus);

	/*高位*/
	registerAddress =  *(pdata + 0) << 8;

	/*低位*/
	registerAddress |= *(pdata + 1);

	/*寄存器长度高位必为零*/
	/*低位*/
	registerLength  =  *(pdata + 3);

	if(registerLength + registerAddress - 1 > REGISTERSLENGTH)
	{
		return -1;
	}

	/*释放PDU*/
	free(MODBUS_DataPointer(pmodbus));

	/*构造回复PDU*/
	/*计算PDU长度*/
	MODBUS_SetDataLength(pmodbus, registerLength*2 + 1);
	/*分配PDU储存空间*/
	MODBUS_SetDataPointer(pmodbus, (uint32_t*)malloc(sizeof(uint32_t)*(registerLength*2 + 1)));

	/*填充内容*/
	pdata = MODBUS_DataPointer(pmodbus);

	for( i = 0; i < registerLength; i++)
	{
		*(pdata + 2*i + 1) = modbusRegisters[registerAddress-1 + i] & 0x0000FF00; /*高位*/
		*(pdata + 2*i + 2) = modbusRegisters[registerAddress-1 + i] & 0x000000FF; /*低位*/
	}

	*pdata = registerLength*2;

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

	free(MODBUS_DataPointer(pmodbus));
	free(pmodbus);

	return 0;
}

int MODBUS_WriteRegister(MODBUSTypeDef* const pmodbus, CircularBufferTypeDef* prxcbuf)
{
	uint32_t  registerAddress;
	uint32_t  registerValue;
	uint32_t  CRCValue;
	uint32_t  data;
	uint32_t* pdata;

	pdata = MODBUS_DataPointer(pmodbus);

	/*写入寄存器地址*/
	/*高位*/
	registerAddress =  *(pdata + 0) << 8;

	/*低位*/
	registerAddress |= *(pdata + 1);

	/*寄存器长度高位必为零*/
	/*高位*/
	registerValue  =  *(pdata + 2) << 8;

	/*低位*/
	registerValue |=  *(pdata + 3);

	if(registerAddress > REGISTERSLENGTH)
	{
		return -1;
	}

	/*写入数据*/
	if(registerAddress == 0x02)
	{
		if(registerValue == 0x000F)
			MOTOR_Forward();
		else if(registerValue == 0x00F0)
			MOTOR_Backward();
	}
	else if(registerAddress == 0x03)
	{
		MOTOR_SetMotorSpeed(registerValue);
	}

	modbusRegisters[registerAddress - 1] = registerValue;

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

	return 0;
}

