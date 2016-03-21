#include <stdint.h>

typedef enum
{
	MODBUS_OK,
	MODBUS_FunctionCodeError,
	MODBUS_RegisterLengthError,
	MODBUS_AddressError,
	MODBUS_RegisterReadError
} MODBUSStatusTypeDef;

typedef struct
{
	uint8_t  address;
	uint8_t  functionCode;
	uint8_t* data;
	uint8_t dataLength;
	uint16_t CRC;
} MODBUSTypeDef;

