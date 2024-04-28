#ifndef __PID_H
#define __PID_H
/*
电机转速和位置的PID控制
*/
typedef struct//定义一个pid结构体，存储pid控制的相关变量
{
	double target;   //目标值
    double actual;   //实际值
	double err;          //偏差值
	double err_last;     //上一个偏差值
	double Kp,Ki,Kd;     //比例、积分、微分系数
	double integral;     //积分值
	double output;   //输出值
} PID_StructTypedef;
void PID_VelocityInit(double Kp, double Ki, double Kd);//转速PID初始化
void PID_VelocityRealize(DirTypedef dir, double target);//输入转向和转速进行PID调速
double PID_GetVelocity(void);//对PID控制中获取的实际转速直接读取，避免外部读取再次延时
void PID_PositionInit(double Kp, double Ki, double Kd);//位置PID初始化
void PID_PositionRealize(double target);////输入位置和实际位置进行PID控制
#endif
