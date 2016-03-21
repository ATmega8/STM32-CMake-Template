#include "main.h"

#include "test_CRC.h"

int main(void)
{

#ifdef CROSS_COMPILE
	USART_Config();
#endif 
	
	UnityBegin("CRC Test");
	RUN_CRCTest();
	UnityEnd();
}

