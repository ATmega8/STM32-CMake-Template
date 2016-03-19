#include "main.h"

uint32_t idleCount = 0UL;

void vTask1(void *pvParameters )
{
	while(1)
	{

		printf("Task1\n");
		vTaskDelay(250);
	}
}

void vTask2(void *pvParameters )
{
	while(1)
	{
		printf("Task2\n");
		vTaskDelay(250);
	}
}

void vApplicationIdleHook(void)
{
	idleCount++;
}

void test_unity(void)
{
	TEST_ASSERT_EQUAL_INT(1, 2);	
}

int main(void)
{

#ifdef CROSS_COMPILE
	USART_Config();
#endif 
	
	UnityBegin("Uinty Test\n");
	RUN_TEST(test_unity, 29);
	UnityEnd();


	printf("Start\r\n");

	xTaskCreate( vTask1, 
			     "Task 1", 
			     1000, 
			     NULL, 
			     1, 
			     NULL);


	xTaskCreate( vTask2, 
			     "Task 2", 
			     1000, 
			     NULL, 
			     1, 
			     NULL);

	//vTaskStartScheduler();
			

}

