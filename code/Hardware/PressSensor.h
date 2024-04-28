/*
压力传感器驱动，用于压力的测量
传感器尖角处开始，逆时针转分别为PIN1,PIN2,PIN3
Out(PIN3):B_Pin0;In(PIN1):5V;GND(PIN2):GND

压力传感器（40PC100G2A）参数：
压力范围：0-100psi
无效电压输出范围：0.5+-0.04V
电压输出极限:4.00+-0.09V
灵敏度：40.0mV/psi

压力传感器初始化：300个周期，要保证初始状态的压力为大气压（保证后续测量值为表压力）
期间PressSenor_GetValue()输出值为0xFFFF
*/
#ifndef __PRESSSENSOR_H
#define __PRESSSENSOR_H

void PressSenor_Init(void);//传感器初始化
uint16_t PressSenor_GetValue(void);//读取ADC转换数据

#endif
