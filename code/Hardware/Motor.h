/*
电机驱动程序，用以控制电机的正转、反转和停转
PWM输出(PWMA):A_Pin0;STBY:A_Pin1;IN1:A_Pin2;IN2:A_Pin3
PWM输出(PWMA):A_Pin0;STBY:B_Pin12;IN1:B_Pin13;IN2:B_Pin14
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              
MG513P30_12V电机参数:
空载转速:366±26rpm（标定:344rpm）
额定转速:293±21rpm（代标定）
额定扭矩:1kg.cm
堵转扭矩:4.5kg.cm
功率:约4W
*/
/*******************************************
请不要同时使能步进电机/（直流电机+编码器）
*******************************************/
#ifndef __MOTOR_H
#define __MOTOR_H

typedef enum{Forward = 1, Backward = -1} DirTypedef;
void Motor_Init(void);//电机初始化
void Motor_Forward(void);//电机正转
void Motor_Backward(void);//电机反转
void Motor_Stop(void);//电机停转

void Motor_Rotate(DirTypedef dir, double DCycle);//根据输入正反转,设置转速（用于PID位置）
//void Motor_SetVelocity(double Velocity);//（用于PID转速）
DirTypedef Motor_GetDirction(void);//获取电机转向
#endif
