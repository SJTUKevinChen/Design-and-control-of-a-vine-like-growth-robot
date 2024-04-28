#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include <cmath>

int32_t Encoder_OverFlowCnt = 0;//计数编码器溢出
uint32_t Encoder_Cntpr = 1560;//每转计数值
uint32_t Encoder_T = 200;//采样周期（ms）

void Encoder_Init(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_TimeBaseStructInit(&TIM_TimeBaseInitStructure);
    TIM_TimeBaseInitStructure.TIM_Period = 65536 - 1;
    TIM_TimeBaseInitStructure.TIM_Prescaler = 1 - 1;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);
    TIM_ClearFlag(TIM3, TIM_IT_Update);
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
    
    //编码器接口配置
    TIM_ICInitTypeDef TIM_ICInitStructure;
    TIM_ICStructInit(&TIM_ICInitStructure);
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
    TIM_ICInitStructure.TIM_ICFilter = 0;
    TIM_ICInit(TIM3, &TIM_ICInitStructure);
  
    TIM_ICStructInit(&TIM_ICInitStructure);
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
    TIM_ICInitStructure.TIM_ICFilter = 0;
    TIM_ICInit(TIM3, &TIM_ICInitStructure);
    
    TIM_EncoderInterfaceConfig(TIM3, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
    
    //正交编码器输入波形GPIO口初始化(E1A:A_Pin6;E1B:A_Pin7)
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    //NVIC配置
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_Init(&NVIC_InitStructure);
    
    TIM_SetCounter(TIM3, 0);
    TIM_Cmd(TIM3, ENABLE);
}

uint16_t Encoder_GetCounter(void)
{
    return TIM_GetCounter(TIM3);
}

int32_t Encoder_GetOverFlowCnt(void)
{
    return Encoder_OverFlowCnt;
}

void Encoder_Clear(void)
{
    TIM_SetCounter(TIM3, 0);
}

int32_t Encoder_GetPosition(void)
{
    return (Encoder_GetOverFlowCnt() * 65536 + Encoder_GetCounter()) / 1560.0 * 360;
}

double Encoder_GetVelocity(void)
{
    //Encoder_Clear();
    double Encoder_Counter1, Encoder_Counter2;//定义为浮点数，便于后面除法计算
    int32_t Encoder_OverFlowCnt1, Encoder_OverFlowCnt2;
    Encoder_OverFlowCnt1 = Encoder_OverFlowCnt;
    Encoder_Counter1 = Encoder_GetCounter();
    Delay_ms(Encoder_T);
    Encoder_OverFlowCnt2 = Encoder_OverFlowCnt;
    Encoder_Counter2 = Encoder_GetCounter();
    return fabs((Encoder_OverFlowCnt2 * 65536 + Encoder_Counter2) - (Encoder_OverFlowCnt1 * 65536 + Encoder_Counter1)) / Encoder_Cntpr * (1000 / Encoder_T) * 60;
}



void TIM3_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM3, TIM_IT_Update) == SET)
    {
        uint16_t Encoder_Counter = Encoder_GetCounter();
        if(Encoder_Counter < 32768) Encoder_OverFlowCnt ++;

        else Encoder_OverFlowCnt --;
    }
    TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
}
