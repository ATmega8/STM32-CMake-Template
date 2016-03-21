#include <stdint.h>

#define IBM_CRC16_PLOY	   0xA001
#define IBM_CRC16_INIT_VALUE 0xFFFF

uint16_t CRC_CalculateFrame(uint8_t* data, uint8_t length);
uint16_t CRC_CalculateByte(uint16_t CRCValue, uint8_t data);

