#include <stdint.h>

#define IBM_CRC16_PLOY	   0xA001

uint16_t CRC_CalculateByte(uint16_t CRCValue, uint8_t data)
{
    uint8_t j;

    CRCValue ^= (uint16_t)data;

    for(j = 0; j < 8; j++)
    {
        if( (CRCValue & 0x0001) == 0 )
            CRCValue >>= 1;
        else
            CRCValue = (CRCValue >> 1) ^ IBM_CRC16_PLOY;
    }

        return CRCValue;
}

