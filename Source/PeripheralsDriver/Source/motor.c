#include "stm32f4xx_conf.h"
#include "motor.h"

/*INIT VALUE*/

uint32_t CCR1_Val = 2500; /*2625/5*/
uint32_t CCR2_Val = 1050;
uint32_t CCR3_Val = 1575;
uint32_t CCR4_Val = 1050;

/*
 *INH1 PWM1 PA11 CH4
 *INH2 PWM2 PA10 CH3
 *INH3 PWM3 PA9  CH2
 *INH4 PWM4 PA8  CH1
 */

void MOTOR_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_BDTRInitTypeDef TIM_BDTRInitStructure;

    /*时钟设置*/
    RCC_APB2PeriphClockCmd(MOTOR_TIMER_CLOCK, ENABLE);
    RCC_AHB1PeriphClockCmd(MOTOR_TIMER_GPIO_CLOCK, ENABLE);

	/*IO设置*/
	/*AF*/
	GPIO_PinAFConfig(MOTOR_DEPTH_LEFT_PWM_GPIO_PORT, 
			MOTOR_DEPTH_LEFT_PWM_GPIO_PINSOURCE, MOTOR_TIMER_AF);

	GPIO_PinAFConfig(MOTOR_DIRECTION_LEFT_PWM_GPIO_PORT, 
			MOTOR_DIRECTION_LEFT_PWM_GPIO_PINSOURCE, MOTOR_TIMER_AF);

	GPIO_PinAFConfig(MOTOR_DEPTH_RIGHT_PWM_GPIO_PORT, 
			MOTOR_DEPTH_RIGHT_PWM_GPIO_PINSOURCE, MOTOR_TIMER_AF);

	GPIO_PinAFConfig(MOTOR_DIRECTION_RIGHT_PWM_GPIO_PORT, 
			MOTOR_DIRECTION_RIGHT_PWM_GPIO_PINSOURCE, MOTOR_TIMER_AF);

	/*PWM*/

	GPIO_InitStructure.GPIO_Pin     = MOTOR_DEPTH_LEFT_PWM_GPIO_PIN |
                                      MOTOR_DIRECTION_LEFT_PWM_GPIO_PIN |
                                      MOTOR_DEPTH_RIGHT_PWM_GPIO_PIN |
                                      MOTOR_DIRECTION_RIGHT_PWM_GPIO_PIN;

    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType   = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd    = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_100MHz;
    GPIO_Init(MOTOR_DEPTH_LEFT_PWM_GPIO_PORT, &GPIO_InitStructure);

	/*EN*/
	GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_OUT;

    GPIO_InitStructure.GPIO_Pin     = MOTOR_DEPTH_LEFT_LEN_GPIO_PIN;
    GPIO_Init(MOTOR_DEPTH_LEFT_LEN_GPIO_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin     = MOTOR_DEPTH_LEFT_REN_GPIO_PIN;
    GPIO_Init(MOTOR_DEPTH_LEFT_REN_GPIO_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin     = MOTOR_DEPTH_RIGHT_LEN_GPIO_PIN;
    GPIO_Init(MOTOR_DEPTH_RIGHT_LEN_GPIO_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin     = MOTOR_DEPTH_RIGHT_REN_GPIO_PIN;
    GPIO_Init(MOTOR_DEPTH_RIGHT_REN_GPIO_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin     = MOTOR_DIRECTION_LEFT_LEN_GPIO_PIN;
    GPIO_Init(MOTOR_DIRECTION_LEFT_LEN_GPIO_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin     = MOTOR_DIRECTION_LEFT_REN_GPIO_PIN;
    GPIO_Init(MOTOR_DIRECTION_LEFT_REN_GPIO_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin     = MOTOR_DIRECTION_RIGHT_LEN_GPIO_PIN;
    GPIO_Init(MOTOR_DIRECTION_RIGHT_LEN_GPIO_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin     = MOTOR_DIRECTION_RIGHT_REN_GPIO_PIN;
    GPIO_Init(MOTOR_DIRECTION_RIGHT_REN_GPIO_PORT, &GPIO_InitStructure);

	/* Current Feedback*/
	GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd    = GPIO_PuPd_DOWN;

    GPIO_InitStructure.GPIO_Pin     = MOTOR_DEPTH_LEFT_CFB_GPIO_PIN;
    GPIO_Init(MOTOR_DEPTH_LEFT_CFB_GPIO_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin     = MOTOR_DEPTH_RIGHT_CFB_GPIO_PIN;
    GPIO_Init(MOTOR_DEPTH_RIGHT_CFB_GPIO_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin     = MOTOR_DIRECTION_LEFT_CFB_GPIO_PIN;
    GPIO_Init(MOTOR_DIRECTION_LEFT_CFB_GPIO_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin     = MOTOR_DIRECTION_RIGHT_CFB_GPIO_PIN;
    GPIO_Init(MOTOR_DIRECTION_RIGHT_CFB_GPIO_PORT, &GPIO_InitStructure);

	/*定时器设置*/
	/* Time Base configuration 84MHz/32KHz = 2625*/
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_CenterAligned1;
	TIM_TimeBaseStructure.TIM_Period = 2625;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(MOTOR_TIMER, &TIM_TimeBaseStructure);

	/* PWM1 Mode configuration: Channel1 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = CCR1_Val;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC1Init(MOTOR_TIMER, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(MOTOR_TIMER, TIM_OCPreload_Enable);

	/* PWM1 Mode configuration: Channel2 */
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = CCR2_Val;
	TIM_OC2Init(MOTOR_TIMER, &TIM_OCInitStructure);
	TIM_OC2PreloadConfig(MOTOR_TIMER, TIM_OCPreload_Enable);

	/* PWM1 Mode configuration: Channel3 */
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = CCR3_Val;
	TIM_OC3Init(MOTOR_TIMER, &TIM_OCInitStructure);
	TIM_OC3PreloadConfig(MOTOR_TIMER, TIM_OCPreload_Enable);

	/* PWM1 Mode configuration: Channel4 */
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = CCR4_Val;
	TIM_OC4Init(MOTOR_TIMER, &TIM_OCInitStructure);
	TIM_OC4PreloadConfig(MOTOR_TIMER, TIM_OCPreload_Enable);

	TIM_ARRPreloadConfig(MOTOR_TIMER, ENABLE);

	/* Automatic Output enable, Break, dead time and lock configuration*/
	TIM_BDTRInitStructure.TIM_OSSRState = TIM_OSSRState_Enable;
	TIM_BDTRInitStructure.TIM_OSSIState = TIM_OSSIState_Enable;
	TIM_BDTRInitStructure.TIM_LOCKLevel = TIM_LOCKLevel_OFF;
	TIM_BDTRInitStructure.TIM_DeadTime = 1;
	TIM_BDTRInitStructure.TIM_Break = TIM_Break_Enable;
	TIM_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_High;
	TIM_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Enable;

	TIM_BDTRConfig(MOTOR_TIMER, &TIM_BDTRInitStructure);
}

void MOTOR_Start(void)
{
	TIM_Cmd(MOTOR_TIMER, ENABLE);
	TIM_CtrlPWMOutputs(MOTOR_TIMER, ENABLE);
}

void MOTOR_Stop(void)
{
	TIM_CtrlPWMOutputs(MOTOR_TIMER, DISABLE);
	TIM_Cmd(MOTOR_TIMER, DISABLE);
}

void MOTOR_Forward(void)
{
	GPIO_SetBits(MOTOR_DEPTH_LEFT_LEN_GPIO_PORT, MOTOR_DEPTH_LEFT_LEN_GPIO_PIN);	
	GPIO_ResetBits(MOTOR_DEPTH_LEFT_REN_GPIO_PORT, MOTOR_DEPTH_LEFT_REN_GPIO_PIN);
}

void MOTOR_Backward(void)
{
	GPIO_ResetBits(MOTOR_DEPTH_LEFT_LEN_GPIO_PORT, MOTOR_DEPTH_LEFT_LEN_GPIO_PIN);	
	GPIO_SetBits(MOTOR_DEPTH_LEFT_REN_GPIO_PORT, MOTOR_DEPTH_LEFT_REN_GPIO_PIN);
}

void MOTOR_SetMotorSpeed(uint32_t speed)
{
	TIM_SetCompare4(MOTOR_TIMER, speed);
}

