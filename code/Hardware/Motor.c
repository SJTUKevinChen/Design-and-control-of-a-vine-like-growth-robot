#include "stm32f10x.h"                  // Device header
#include "Encoder.h"
#include "OLED.h"

typedef enum{Forward = 1, Backward = -1, Stop = 0} DirTypedef;
uint16_t Motor_DCycle = 0;//占空比
double Motor_MaxVelocity = 344;//最大转速
DirTypedef Motor_Direction;

void Motor_Init(void)
{
    /*
    TIM计时器初始化，用以输出PWM波形
    =============
    理论均值空载转速情况下电机输出1rps参数参考值:
    频率:72kHz;分辨率:0.1%;Duty_Cycle:16.4%;
    ============
    实际空载转速情况下电机输出1rps参数参考值:
    频率:72kHz;分辨率:0.1%;Duty_Cycle:21.2%;
    =============
    */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    
    TIM_InternalClockConfig(TIM2);
    
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_Period = 1000 - 1;//调节PWM分辨率
    TIM_TimeBaseInitStructure.TIM_Prescaler = 1 - 1;
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);
    //TIM_ClearFlag(TIM2, TIM_IT_Update);
    
    //TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
 
    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_OCStructInit(&TIM_OCInitStructure);
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = Motor_DCycle;//调节PWM占空比
    TIM_OC1Init(TIM2, &TIM_OCInitStructure);
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    //PWM输出GPIO口的初始化(GPIOA/Pin0)
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    //STBY/IN1/IN2输出GPIO口的初始化(GPIOB/Pin12,Pin13,Pin14)
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    //初始停转
    Motor_Direction = Stop;
    GPIO_ResetBits(GPIOB, GPIO_Pin_12);
    
    TIM_Cmd(TIM2, ENABLE);
}

void Motor_Forward(void)
{
    Motor_Direction = Forward;
    GPIO_SetBits(GPIOB, GPIO_Pin_12);
    GPIO_SetBits(GPIOB, GPIO_Pin_13);
    GPIO_ResetBits(GPIOB, GPIO_Pin_14);
}

void Motor_Backward(void)
{
    Motor_Direction = Backward;
    GPIO_SetBits(GPIOB, GPIO_Pin_12);
    GPIO_SetBits(GPIOB, GPIO_Pin_14);
    GPIO_ResetBits(GPIOB, GPIO_Pin_13);
}

void Motor_Stop(void)
{
    Motor_Direction = Stop;
    GPIO_ResetBits(GPIOB, GPIO_Pin_12);
}

void Motor_Rotate(DirTypedef dir, double Velocity)
{
    if(dir == Forward) Motor_Forward();
    else if(dir == Backward) Motor_Backward();
    Motor_DCycle = Velocity;
    TIM_SetCompare1(TIM2, Motor_DCycle);
}

DirTypedef Motor_GetDirction(void)
{
    return Motor_Direction;
}
