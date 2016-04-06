#include "stm32f4xx_conf.h"
#include "modbus_define.h"

/*2000000 8N1 */
void USART_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    /*时钟设置*/
    RCC_APB2PeriphClockCmd(MODBUS_USART_CLOCK, ENABLE);
    RCC_AHB1PeriphClockCmd(MODBUS_USART_GPIO_CLOCK, ENABLE);
	
    /*GPIO设置*/

	GPIO_PinAFConfig(MODBUS_USART_RX_GPIO_PORT, 
			MODBUS_USART_RX_GPIO_PINSOURCE, MODBUS_USART_GPIO_AF);

	GPIO_PinAFConfig(MODBUS_USART_TX_GPIO_PORT, 
			MODBUS_USART_TX_GPIO_PINSOURCE, MODBUS_USART_GPIO_AF);

	/*TX*/
    GPIO_InitStructure.GPIO_Pin     = MODBUS_USART_TX_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType   = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd    = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_100MHz;
    GPIO_Init(MODBUS_USART_TX_GPIO_PORT, &GPIO_InitStructure);

	/*RX*/
    GPIO_InitStructure.GPIO_Pin     = MODBUS_USART_RX_GPIO_PIN;
    GPIO_Init(MODBUS_USART_RX_GPIO_PORT, &GPIO_InitStructure);

	/*DE*/
    GPIO_InitStructure.GPIO_Pin     = MODBUS_DE_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_OUT;
    GPIO_Init(MODBUS_DE_GPIO_PORT, &GPIO_InitStructure);

    /*USART通用设置*/
    USART_InitStructure.USART_BaudRate   = MODBUS_USART_BAUD_RATE;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits   = USART_StopBits_1;
    USART_InitStructure.USART_Parity    = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode       = USART_Mode_Tx | USART_Mode_Rx;
    USART_Init(MODBUS_USART, &USART_InitStructure);

    USART_InitStructure.USART_Mode       = USART_Mode_Tx | USART_Mode_Rx;
    USART_Init(MODBUS_USART, &USART_InitStructure);

	/*串口接收中断*/
	USART_ITConfig(MODBUS_USART, USART_IT_RXNE, ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

    /*使能USART*/
    USART_Cmd(MODBUS_USART, ENABLE);
}

