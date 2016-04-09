#include "stm32f4xx_conf.h"
#include "modbus_define.h"
#include "motor.h"

uint32_t MODBUS_ReadSlaveAddress(void)
{
	return ADDRESS;
}

uint32_t MODBUS_ReadDepthStatus(void)
{
	if(GPIO_ReadOutputDataBit(MOTOR_DEPTH_LEFT_LEN_GPIO_PORT,
				MOTOR_DEPTH_LEFT_LEN_GPIO_PIN) &&
		!GPIO_ReadOutputDataBit(MOTOR_DEPTH_LEFT_REN_GPIO_PORT,
			MOTOR_DEPTH_LEFT_REN_GPIO_PIN))
	{
		return 0x0F; /*Forward*/
	}
	else if(!GPIO_ReadOutputDataBit(MOTOR_DEPTH_LEFT_LEN_GPIO_PORT,
				MOTOR_DEPTH_LEFT_LEN_GPIO_PIN) &&
		GPIO_ReadOutputDataBit(MOTOR_DEPTH_LEFT_REN_GPIO_PORT,
				MOTOR_DEPTH_LEFT_REN_GPIO_PIN))
	{
		return 0xF0; /*Backward*/
	}
	else
	{
		return 0xFF; /*unknow status*/
	}
}

uint32_t MODBUS_ReadDepthSpeed(void)
{
	return TIM_GetCapture4(MOTOR_TIMER);
}

void MODBUS_SetDepthStatus(uint32_t status)
{
	if(status == 0x000F)
		MOTOR_Forward();
	else if(status == 0x00F0)
		MOTOR_Backward();
}

void MODBUS_SetDepthSpeed(uint32_t speed)
{
	MOTOR_SetMotorSpeed(speed);
}

