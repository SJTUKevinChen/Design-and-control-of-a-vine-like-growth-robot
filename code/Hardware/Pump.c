#include "stm32f10x.h"                  // Device header

uint16_t Pump_DCycle = 10;//占空比

void Pump_Init(void)
{
	//1.结构体
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//2.开时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
 
	//3.配置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		
	//4.初始化
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//1.结构体,时基结构体及输出比较结构体
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	
	//2.开时钟，TIM1的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	
	//3.配置及初始化
	/*--------------------时基结构体初始化-------------------------*/
	// PWM 信号的频率 F = TIM_CLK/{(ARR+1)*(PSC+1)}
	
	TIM_TimeBaseStructure.TIM_Period = 9;	                      //ARR寄存器的值
	TIM_TimeBaseStructure.TIM_Prescaler = 7199;	                  //PSC分频的值
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;   //向上计数
	TIM_TimeBaseStructure.TIM_RepetitionCounter=0;                //重复计数器为0
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
	
	/*--------------------输出比较结构体初始化-------------------*/
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;             //输出模式：PWM1
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_Pulse = 10;			                  //设置占空比
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;     //有效电平
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;  //空闲时比较输出状态
	TIM_OC1Init(TIM1, &TIM_OCInitStructure);
 
	//4.使能，此处使能为将内部时钟作为TIM1时钟源的使能
	TIM_Cmd(TIM1, ENABLE);									      //使能计数器
	TIM_CtrlPWMOutputs(TIM1, ENABLE);				              //使能输出通道
    
    //STBY/IN1/IN2输出GPIO口的初始化(GPIOA/Pin11,Pin10,Pin9)
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    //初始停转
    GPIO_ResetBits(GPIOA, GPIO_Pin_11);
    
    TIM_Cmd(TIM2, ENABLE);
}

void Pump_ENABLE(void)
{
    GPIO_SetBits(GPIOA, GPIO_Pin_11);
    GPIO_SetBits(GPIOA, GPIO_Pin_9);
    GPIO_ResetBits(GPIOA, GPIO_Pin_10);
}

void Pump_DISABLE(void)
{
    GPIO_ResetBits(GPIOA, GPIO_Pin_11);
}

void Pump_SetDCycle(uint16_t DCycle)
{
    Pump_DCycle = DCycle;
    TIM_SetCompare1(TIM2, Pump_DCycle);
}
