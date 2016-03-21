#include "CRC.h"

uint16_t CRC_CalculateFrame(uint8_t* data, uint8_t length)
{
    uint8_t i;
	uint16_t CRCValue = IBM_CRC16_INIT_VALUE;

    //CRCValue = CRC_CalculateByte(
            //CRC_calculate_Byte_CRC(CRCValue, frame->slaveAddr), frame->command);

    for(i = 0; i < length; i++)
        CRCValue = CRC_CalculateByte(CRCValue, *(data + i));

	return CRCValue;
}

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

