/*
气泵驱动程序
PWMA:A_Pin8;AIN2:A_Pin9;AIN1:A_Pin10;STBY:A_Pin11;
电机正：AO1;电机负：AO2
*/
#ifndef __PUMP_H
#define __PUMP_H

void Pump_Init(void);// 气泵初始化
void Pump_ENABLE(void);// 气泵使能
void Pump_DISABLE(void);// 气泵失能
void Pump_SetDCycle(uint16_t DCycle);// 设置PWM波占空比

#endif
