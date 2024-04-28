#include "stm32f10x.h" 
#include "StepMotor.h"

int32_t current_position;

void StepMotor_Init(void)
{	
    current_position = 0;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_OCInitTypeDef TIM_OCInitStruct;
	GPIO_InitTypeDef  GPIO_InitStruct;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC, ENABLE);//使能PC,PA端口时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2|RCC_APB1Periph_TIM3,ENABLE);   //使能定时器23的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);						  //使能复用IO时钟	

	//GPIO_PinRemapConfig(GPIO_FullRemap_TIM3, ENABLE);	

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP; //复用推挽输出
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;			
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);	
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;
	GPIO_Init(GPIOA,&GPIO_InitStruct); 

    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;//用于控制DIR，输出1为正转
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;
	GPIO_Init(GPIOA,&GPIO_InitStruct); 
    
	//TIM2工作在单脉冲下
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;//向上计数模式
	TIM_TimeBaseInitStruct.TIM_Prescaler = 7199;				//预分频值,每100us计数一次
	TIM_TimeBaseInitStruct.TIM_Period = 99; 					//重装值，决定单脉冲周期（T=10ms）
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStruct);	

	TIM_SelectOnePulseMode(TIM2,TIM_OPMode_Single);		 //设置TIM2在单脉冲模式，且是单一的脉冲，在下一个更新事件停止	  
	TIM_SelectOutputTrigger(TIM2,TIM_TRGOSource_OC1Ref); //OC1REF signal is used as the trigger output (TRGO).单一模式不需要设置

	TIM_OC1PreloadConfig(TIM2,TIM_OCPreload_Enable);	 //使能定时器2的通道1预装载寄存器
	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM2;		 //在向上计数时，一旦TIMx_CNT>TIMx_CCR1时通道1为有效电平
	//TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;	//OC1输出使能
	TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;//TIM_OCPolarity_High;		//有效电平为高TIM_OCPolarity_Low
	//没什么用
	
	TIM_OCInitStruct.TIM_Pulse = 20;		//比较捕获1的预装载值，决定单脉冲占空比，这个20就是低电平延续的来源
	TIM_OC1Init(TIM2,&TIM_OCInitStruct);		
	TIM_Cmd(TIM2,DISABLE); //Disable the TIM Counter

	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;//向上计数模式
	TIM_TimeBaseInitStruct.TIM_Prescaler = 719; //预分频值，10us计数一次
	TIM_TimeBaseInitStruct.TIM_Period = 29;		//重装值，决定PWM周期
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStruct);

	TIM_SelectSlaveMode(TIM3, TIM_SlaveMode_Gated);//TIM3为门控模式	
	TIM_SelectMasterSlaveMode(TIM3,TIM_MasterSlaveMode_Enable);//使能TIM3的主从模式	
	TIM_SelectInputTrigger(TIM3,TIM_TS_ITR1);//内部触发，TIM_TS_ITR1对应TIM2
	

	TIM_OC1PreloadConfig(TIM3,TIM_OCPreload_Enable);//使能定时器3的通道1预装载寄存器
	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM2;//在向上计数时，一旦TIMx_CNT>TIMx_CCR1时通道1为有效电平
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;//OC1输出使能
	TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;//有效电平为高
	TIM_OCInitStruct.TIM_Pulse = 15;//比较捕获1的预装载值，占空比为50%
	TIM_OC1Init(TIM3,&TIM_OCInitStruct);
	TIM_Cmd(TIM3,ENABLE);//使能TIM3
}

/*********************************************************
名称：Pulse_Control(u16 cycle, u16 pulse_num)
说明：生成指定个数脉冲，每个脉冲周期为cycle微秒，脉冲个数生成的个数
      和单脉冲高电平时间有关系，脉冲个数就由高电平时间来确定
参数cycle：为TIM3一个脉冲周期,单位(us)
参数pulse_num：为脉冲个数，决定步进电机步数
**65516=65536 -20**
要求：cycle * pulse_num < 6551600  us
返回值：无
*********************************************************/
void Pulse_Control(uint16_t cycle, uint16_t pulse_num)
{
	uint16_t arr3 = 0;
	uint32_t time = 0;
	if(pulse_num)
	{ 
		time = cycle * pulse_num / 100;//预分频为720,100us计数一次
		arr3 = cycle / 10;             //预分频为72,10us计数一次 
		TIM_SetAutoreload(TIM2, time+19);//低电平周期加高电平周期
		
		TIM_SetAutoreload(TIM3, arr3 - 1);
		TIM_SetCompare1(TIM3,arr3 / 2); //设置PWM占空比为50%
		TIM_Cmd(TIM2,ENABLE);//使能TIM2
	}
}

void StepMotor_Rotate_rel(DirTypedef dir, uint32_t pos)
{
    uint16_t cycle = 0;
    uint16_t pulse_num = 0;
    if(dir == Forward)
    {
        GPIO_SetBits(GPIOA, GPIO_Pin_1);
        cycle = STEP_ANGLE  / VELOCITY_ANGLE * 1000000;
        pulse_num = pos / STEP_ANGLE;
        Pulse_Control(cycle, pulse_num);
        current_position += pos;
    }
    else if(dir == Backward)
    {
        GPIO_ResetBits(GPIOA, GPIO_Pin_1);
        cycle = STEP_ANGLE  / VELOCITY_ANGLE * 1000000;
        pulse_num = pos / STEP_ANGLE;
        Pulse_Control(cycle, pulse_num);
        current_position -= pos;
    }
}

void StepMotor_Rotate_abs(int32_t pos)
{
    if(pos > current_position)StepMotor_Rotate_rel(Forward, pos - current_position);
    else if(pos < current_position)StepMotor_Rotate_rel(Backward, current_position - pos);
    current_position = pos;
}

int32_t StepMotor_GetPosition(void)
{
    return current_position;
}    
