#include "stm32f10x.h"   // Device header
#include "OLED.h"
#include "Delay.h"
#include "Button.h"
#include "PressSensor.h"
#include "Delay.h"
#include "Pump.h"
#include "Serial.h"
#include "string.h"
#include <cmath>
#include "StepMotor.h"

uint16_t Press;// 记录气压
int32_t Position;// 记录位置
uint16_t Press_threshold; // 更改压强阈值

int main(void)
{   
/*各个硬件初始化*/   
    StepMotor_Init();
    OLED_Init();
    Pump_Init();
    PressSenor_Init();
    Serial_Init();
    StepMotor_Rotate_rel(Backward, 360);
    while(1)
	{   
        /*串口命令解析与执行*/
        if (Serial_RxFlag == 1)
		{
            // 显示串口命令内容
            Serial_RxFlag = 0;
			OLED_ShowString(4, 1, "Com:            ");
            OLED_ShowString(4, 5, Serial_RxPacket);
            
            // 解析命令
            char Command[100];
            uint16_t top = 0;
            while(Serial_RxPacket[top] != ' ') Command[top ++] = Serial_RxPacket[top];
            Command[top] = '\0';
            top ++;
            
            // 对于命令具体数值进行分析
            if (strcmp(Command, "to") == 0)// 电机旋转命令
            {
                int32_t temp = 0;
                int8_t sign = 0;
                // 判断符号
                if (Serial_RxPacket[top ++] == '+') sign = 1;
                else sign = -1;
                
                while(Serial_RxPacket[top] != '\0')
                {
                    temp = temp * 10 + (Serial_RxPacket[top] - '0');
                    top ++;
                }
                Position = sign * temp;
            }
            if (strcmp(Command, "setPress") == 0)// 设置阈值命令
            {
                uint16_t temp = 0;
                while(Serial_RxPacket[top] != '\0')
                {
                    temp = temp * 10 + (Serial_RxPacket[top] - '0');
                    top ++;
                }
                Press_threshold = temp;
            }
		}
        
        /*气压和电机位置显示*/
        OLED_ShowString(1, 1, "Press:     kPa");
        OLED_ShowString(2, 1, "PumpState:");
        // 气压显示
        Press = PressSenor_GetValue();
        if(Press == 0xFFFF) OLED_ShowString(1, 7, "Init.");
        //else if(Press == 0) OLED_ShowString(1, 7, "NULL ");
        else OLED_ShowNum(1, 7, Press, 5);    
        // 位置显示
        OLED_ShowString(3, 1, "Positon:");
        OLED_ShowSignedNum(3, 9, StepMotor_GetPosition(), 5);     
        
        /*泵阈值控制*/
        if(Press < Press_threshold)// 气压阈值设置
        {   
            Pump_ENABLE();
            OLED_ShowString(2,11,"ON ");
        }
        else
        {
            Pump_DISABLE();
            OLED_ShowString(2,11,"OFF");
        }

	}
}
