#define MOTOR_TIMER TIM1

#define MOTOR_TIMER_CLOCK RCC_APB2Periph_TIM1

#define MOTOR_TIMER_GPIO_CLOCK \
	(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC)

#define MOTOR_TIMER_AF GPIO_AF_TIM1

/*
 * 1: DEPTH_LEFT                      * 1:DEPTH_LEFT
 * 2: DEPTH_RIGHT                * 3:DIRECTION_LEFT
 *                      --->
 * 3: DIRECTION_LEFT             * 4:DIRECTION_RIGHT
 * 4: DIRECTION_RIGTH                 * 2:DEPTH_RIGHT
 */

/*PWM*/

#define MOTOR_DEPTH_LEFT_PWM_GPIO_PIN		     GPIO_Pin_11
#define MOTOR_DEPTH_LEFT_PWM_GPIO_PORT		     GPIOA
#define MOTOR_DEPTH_LEFT_PWM_GPIO_PINSOURCE      GPIO_PinSource11

#define MOTOR_DEPTH_RIGHT_PWM_GPIO_PIN		     GPIO_Pin_10
#define MOTOR_DEPTH_RIGHT_PWM_GPIO_PORT          GPIOA
#define MOTOR_DEPTH_RIGHT_PWM_GPIO_PINSOURCE     GPIO_PinSource10

#define MOTOR_DIRECTION_LEFT_PWM_GPIO_PIN        GPIO_Pin_9
#define MOTOR_DIRECTION_LEFT_PWM_GPIO_PORT       GPIOA
#define MOTOR_DIRECTION_LEFT_PWM_GPIO_PINSOURCE  GPIO_PinSource9

#define MOTOR_DIRECTION_RIGHT_PWM_GPIO_PIN	     GPIO_Pin_8
#define MOTOR_DIRECTION_RIGHT_PWM_GPIO_PORT	     GPIOA
#define MOTOR_DIRECTION_RIGHT_PWM_GPIO_PINSOURCE GPIO_PinSource8

/*EN*/

#define MOTOR_DEPTH_LEFT_LEN_GPIO_PIN          GPIO_Pin_9
#define MOTOR_DEPTH_LEFT_LEN_GPIO_PORT         GPIOC

#define MOTOR_DEPTH_LEFT_REN_GPIO_PIN          GPIO_Pin_8
#define MOTOR_DEPTH_LEFT_REN_GPIO_PORT         GPIOC

#define MOTOR_DEPTH_RIGHT_LEN_GPIO_PIN	       GPIO_Pin_6
#define MOTOR_DEPTH_RIGHT_LEN_GPIO_PORT	       GPIOC

#define MOTOR_DEPTH_RIGHT_REN_GPIO_PIN         GPIO_Pin_15
#define MOTOR_DEPTH_RIGHT_REN_GPIO_PORT	       GPIOB

#define MOTOR_DIRECTION_LEFT_LEN_GPIO_PIN      GPIO_Pin_13
#define MOTOR_DIRECTION_LEFT_LEN_GPIO_PORT	   GPIOB

#define MOTOR_DIRECTION_LEFT_REN_GPIO_PIN	   GPIO_Pin_12
#define MOTOR_DIRECTION_LEFT_REN_GPIO_PORT	   GPIOB

#define MOTOR_DIRECTION_RIGHT_LEN_GPIO_PIN	   GPIO_Pin_10
#define MOTOR_DIRECTION_RIGHT_LEN_GPIO_PORT	   GPIOB

#define MOTOR_DIRECTION_RIGHT_REN_GPIO_PIN	   GPIO_Pin_2
#define MOTOR_DIRECTION_RIGHT_REN_GPIO_PORT	   GPIOB

/* Current Feedback */

#define MOTOR_DEPTH_LEFT_CFB_GPIO_PIN          GPIO_Pin_7
#define MOTOR_DEPTH_LEFT_CFB_GPIO_PORT         GPIOC

#define MOTOR_DEPTH_RIGHT_CFB_GPIO_PIN         GPIO_Pin_14
#define MOTOR_DEPTH_RIGHT_CFB_GPIO_PORT        GPIOB

#define MOTOR_DIRECTION_LEFT_CFB_GPIO_PIN      GPIO_Pin_11
#define MOTOR_DIRECTION_LEFT_CFB_GPIO_PORT     GPIOB

#define MOTOR_DIRECTION_RIGHT_CFB_GPIO_PIN     GPIO_Pin_1
#define MOTOR_DIRECTION_RIGHT_CFB_GPIO_PORT	   GPIOB

void MOTOR_Init(void);
void MOTOR_Start(void);
void MOTOR_Stop(void);

