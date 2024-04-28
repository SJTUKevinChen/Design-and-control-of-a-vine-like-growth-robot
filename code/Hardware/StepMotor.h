/*步进电机驱动程序
PU-,DR-,MF+-:COM;DR+:A_Pin1;PU+:A_Pin6
*/

/*******************************************
请不要同时使能步进电机/（直流电机+编码器）
*******************************************/
#ifndef __STEPMOTOR_H
#define __STEPMOTOR_H

#include "stm32f10x.h" 
#define STEP_ANGLE 1.8 // 步距角1.8度
#define VELOCITY_ANGLE 90 // deg/s 可改

typedef enum{Forward = 1, Backward = -1} DirTypedef;
void StepMotor_Init(void);
void Pulse_Control(uint16_t cycle, uint16_t pulse_num);
void StepMotor_Rotate_rel(DirTypedef dir, uint32_t pos);
void StepMotor_Rotate_abs(int32_t pos);
int32_t ss(void);

#endif
