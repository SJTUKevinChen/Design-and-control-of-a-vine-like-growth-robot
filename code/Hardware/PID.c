#include "stm32f10x.h"                  // Device header
#include "Encoder.h"
#include "Motor.h"
#include "Delay.h"
#include <cmath>

//定义一个pid结构体，存储pid控制的相关变量
typedef struct
{
	double target;   //目标值
    double actual;   //实际值
	double err;          //偏差值
	double err_last;     //上一个偏差值
	double Kp,Ki,Kd;     //比例、积分、微分系数
	double integral;     //积分值
	double output;   //输出值
} PID_StructTypedef;

PID_StructTypedef PID_VelocityStructure;
PID_StructTypedef PID_PositionStructure;
uint8_t PID_VelocityFlag = 0;//用于记录是否初始化
uint8_t PID_PositionFlag = 0;

double PID_Velocitydt = 0.2;//速度采样周期为200ms
double PID_Positiondt = 0.01;//位置采样周期为10ms

void PID_VelocityInit(double Kp, double Ki, double Kd)
{
    PID_VelocityStructure.target = 0;
    PID_VelocityStructure.actual = 0;
    PID_VelocityStructure.err = PID_VelocityStructure.target - PID_VelocityStructure.actual;
    PID_VelocityStructure.err_last = PID_VelocityStructure.target - PID_VelocityStructure.actual;
    PID_VelocityStructure.Kp = Kp;
    PID_VelocityStructure.Ki = Ki;
    PID_VelocityStructure.Kd = Kd;
    PID_VelocityStructure.integral = 0;
    PID_VelocityStructure.output = 0;
    PID_VelocityFlag = 1;
} 

void PID_VelocityRealize(DirTypedef dir, double target)
{
    if(!PID_VelocityFlag) return;
    else
    {
        PID_VelocityStructure.target = target;
        
        /*计算目标值与实际值的误差*/
        PID_VelocityStructure.actual = Encoder_GetVelocity();//测速存在200ms延时
        PID_VelocityStructure.err = PID_VelocityStructure.target - PID_VelocityStructure.actual;
        
        /*积分项*/
        PID_VelocityStructure.integral += PID_VelocityStructure.err * PID_Velocitydt;
        
        /*pid控制器输出参数*/
        PID_VelocityStructure.output = PID_VelocityStructure.Kp * PID_VelocityStructure.err +
                     PID_VelocityStructure.Ki * PID_VelocityStructure.integral +
                     PID_VelocityStructure.Kd * ((PID_VelocityStructure.err - PID_VelocityStructure.err_last) / PID_Velocitydt);
        
        /*误差传递*/
        PID_VelocityStructure.err_last = PID_VelocityStructure.err;
        
        Motor_Rotate(dir, PID_VelocityStructure.output);
    }
}

double PID_GetVelocity(void)
{
    return PID_VelocityStructure.actual;
}

void PID_PositionInit(double Kp, double Ki, double Kd)
{
    PID_PositionStructure.target = 0;
    PID_PositionStructure.actual = Encoder_GetPosition();
    PID_PositionStructure.err = PID_PositionStructure.target - PID_PositionStructure.actual;
    PID_PositionStructure.err_last = PID_PositionStructure.target - PID_PositionStructure.actual;
    PID_PositionStructure.Kp = Kp;
    PID_PositionStructure.Ki = Ki;
    PID_PositionStructure.Kd = Kd;
    PID_PositionStructure.integral = 0;
    PID_PositionStructure.output = 0;
    PID_PositionFlag = 1;
}

void PID_PositionRealize(double target)
{
    if(!PID_PositionFlag) return;
    else
    {
        Delay_ms(PID_Positiondt * 1000);
        PID_PositionStructure.actual = Encoder_GetPosition();
        PID_PositionStructure.target = target;
        PID_PositionStructure.err =  PID_PositionStructure.target - PID_PositionStructure.actual;
       
        /*积分项*/
        PID_PositionStructure.integral += PID_PositionStructure.err * PID_Positiondt;
        
        /*pid控制器输出参数*/
        PID_PositionStructure.output = PID_PositionStructure.Kp * PID_PositionStructure.err +
                                       PID_PositionStructure.Ki * PID_PositionStructure.integral +
                                       PID_PositionStructure.Kd * ((PID_PositionStructure.err - PID_PositionStructure.err_last) / PID_Positiondt);
        /*误差传递*/
        PID_PositionStructure.err_last = PID_PositionStructure.err;
        
        if(PID_PositionStructure.output < 0) Motor_Rotate(Backward, fabs(PID_PositionStructure.output));
        else if(PID_PositionStructure.output > 0) Motor_Rotate(Forward, fabs(PID_PositionStructure.output));
        else Motor_Stop();
    }
}
