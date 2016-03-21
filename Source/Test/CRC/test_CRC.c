#include "CRC.h"
#include "unity.h"

void test_CRCCalcularByte(void)
{
	uint8_t data = 0x0A;
	uint16_t CRCValue;

	CRCValue = CRC_CalculateByte(0xFFFF, data);

	TEST_ASSERT_EQUAL_HEX16(0x473F, CRCValue);
}

void test_CRCCalcularFrame(void)
{
	uint8_t data[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};;
	uint16_t CRCValue;

	CRCValue = CRC_CalculateFrame(&data[0], 9);

	TEST_ASSERT_EQUAL_HEX16(0xB20E, CRCValue);
}

void RUN_CRCTest(void)
{
	RUN_TEST(test_CRCCalcularByte, 4);
	RUN_TEST(test_CRCCalcularFrame, 14);
}

