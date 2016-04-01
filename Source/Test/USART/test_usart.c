#include "usart.h"
#include "unity.h"

#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>
#include <unistd.h>

int fd;

void test_USARTOpen(void)
{
	fd = open("/dev/ttyUSB0", O_RDWR );
	
	/* 0 < fd < 127*/
	TEST_ASSERT_UINT_WITHIN(127, 0, fd);
}

void test_USARTConfig(void)
{
	int status;
	status  = USART_Config(fd);

	TEST_ASSERT_EQUAL_INT(0, status);
}

void test_USARTReadAndWrite(void)
{
	int n;
	const uint8_t write_data[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	uint8_t read_data[10];

	/*写入*/
	n= write(fd, &write_data[0], sizeof(write_data));
	TEST_ASSERT_EQUAL_INT(10, n);

	/*读出*/
	n = read(fd ,&read_data[0], sizeof(write_data));

	TEST_ASSERT_EQUAL_INT(1, n);

	/*读出数据*/
	TEST_ASSERT_EQUAL_MEMORY(write_data, read_data, sizeof(uint8_t));
}

void RUN_USARTTest(void)
{
	RUN_TEST(test_USARTOpen, 4);
	RUN_TEST(test_USARTConfig, 17);
	RUN_TEST(test_USARTReadAndWrite, 26);
}

