#include <stdint.h>
#include "circularbuffer.h"
#include "stm32f4xx_conf.h"

#define IBM_CRC16_INIT_VALUE 0xFFFF

#define ADDRESS 0x01
#define START_ADDRESS 0x00
#define ADDRESS_LENGTH 0x03

#define REGISTERSLENGTH 0x03

#define MODBUS_USART_BAUD_RATE 115200

#define MODBUS_USART USART1
#define MODBUS_USART_CLOCK RCC_APB2Periph_USART1
#define MODBUS_USART_GPIO_CLOCK RCC_AHB1Periph_GPIOB

#define MODBUS_USART_RX_GPIO_PIN  GPIO_Pin_7
#define MODBUS_USART_RX_GPIO_PORT GPIOB

#define MODBUS_USART_TX_GPIO_PIN  GPIO_Pin_6
#define MODBUS_USART_TX_GPIO_PORT GPIOB

#define MODBUS_DE_GPIO_PIN        GPIO_Pin_5
#define MODBUS_DE_GPIO_PORT       GPIOB

#define MODBUS_USART_GPIO_AF GPIO_AF_USART1

#define MODBUS_USART_TX_GPIO_PINSOURCE GPIO_PinSource6
#define MODBUS_USART_RX_GPIO_PINSOURCE GPIO_PinSource7

#define MODBUS_USART_IRQn USART1_IRQn

uint32_t modbusRegisters[REGISTERSLENGTH];

typedef enum
{
	MODBUS_OK,
	MODBUS_FunctionCodeError,
	MODBUS_RegisterLengthError,
	MODBUS_AddressError,
	MODBUS_RegisterReadError,
	MODBUS_CRCCheckError
} MODBUSStatusTypeDef;

typedef struct
{
	uint32_t  address;
	uint32_t  functionCode;
	uint32_t* data;
	uint32_t  dataLength;
	uint32_t  CRCL;
	uint32_t  CRCH;
	uint32_t  (*readCallBack[3])(void);
	void      (*writeCallBack[3])(uint32_t);
	MODBUSStatusTypeDef status;
} MODBUSTypeDef;

extern MODBUSTypeDef* MODBUS_Create(void);

extern void MODBUS_SetAddress(MODBUSTypeDef* const pmodbus, const uint32_t address);
extern void MODBUS_SetFunctionCode(MODBUSTypeDef* const pmodbus, const uint32_t functionCode);
extern void MODBUS_SetDataLength(MODBUSTypeDef* const pmodbus, const uint32_t dataLength);
extern void MODBUS_SetDataPointer(MODBUSTypeDef* const pmodbus, uint32_t* pdata);
extern void MODBUS_SetCRC(MODBUSTypeDef* const pmodbus, const uint32_t CRCL, const uint32_t CRCH);
extern void MODBUS_SetModbusStatus(MODBUSTypeDef* const pmodbus, 
		const MODBUSStatusTypeDef status);

extern uint32_t  MODBUS_Address(MODBUSTypeDef* const pmodbus);
extern uint32_t  MODBUS_FunctionCode(MODBUSTypeDef* const pmodbus);
extern uint32_t  MODBUS_DataLength(MODBUSTypeDef* const pmodbus);
extern uint32_t* MODBUS_DataPointer(MODBUSTypeDef* const pmodbus);
extern uint16_t  MODBUS_CRC(MODBUSTypeDef* const pmodbus);
extern MODBUSStatusTypeDef MODBUS_ModbusStatus(MODBUSTypeDef* const pmodbus);

void MODBUS_EnableDriver(void);
void MODBUS_DisableDriver(void);

uint16_t CRC_Calculate(MODBUSTypeDef*const pmodbus);
void MODBUS_SetModbusFrame(CircularBufferTypeDef* pcbuf, 
		MODBUSTypeDef* const pmodbus, uint32_t length);

int MODBUS_ReadRegister(MODBUSTypeDef* const pmodbus, CircularBufferTypeDef* prxcbuf);
int MODBUS_WriteRegister(MODBUSTypeDef* const pmodbus, CircularBufferTypeDef* prxcbuf);

extern void MODBUS_RegisterUpdate(void);

extern uint32_t MODBUS_RegisterAddress(MODBUSTypeDef* const pmodbus);
extern uint32_t MODBUS_ReadRegisterLength(MODBUSTypeDef* const pmodbus);
extern uint32_t MODBUS_WriteRegisterData(MODBUSTypeDef* const pmodbus);

void MODBUS_Reply(CircularBufferTypeDef* prxcbuf);
void MODBUS_ProcessRequest(MODBUSTypeDef* pmodbus, CircularBufferTypeDef* prxcbuf);
void MODBUS_ProcessWriteSingleRegister(MODBUSTypeDef* pmodbus, CircularBufferTypeDef* prxcbuf);
void MODBUS_ProcessReadHoldRegister(MODBUSTypeDef* pmodbus, CircularBufferTypeDef* prxcbuf);

