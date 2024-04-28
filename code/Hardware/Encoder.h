#ifndef __ENCODER_H
#define __ENCODER_H
/*
编码器驱动程序，用于对电机测速
E1A:A_Pin6;E1B:A_Pin7

霍尔编码器线数：13ppr
（减速比30，而一个脉冲波会产生四个计数值，因此转一圈是13*30*4=1560个计数值）
*/
void Encoder_Init(void);//编码器初始化
uint16_t Encoder_GetCounter(void);//获取当前计数值
int32_t Encoder_GetOverFlowCnt(void);//获取计数编码器溢出值
void Encoder_Clear(void);//编码器计数值归位
 int32_t Encoder_GetPosition(void);//获取当前位置
double Encoder_GetVelocity(void);//获取当前电机转速(rpm)，存在200ms延时

#endif
