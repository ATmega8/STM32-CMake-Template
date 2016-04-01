#define MODBUS_WAIT 0

void USART_ReadTask(void* parameter);
void MODBUS_CheckRequestTask(void* parameter);
void MODBUS_ErrorProcessTask(void* parameter);
void MODBUS_ReplyTask(void* parameter);

