#include "stm32f10x.h"   // Device header
#include "OLED.h"
#include "Serial.h"
#include "string.h"
#include <cmath>

void SerialCommand_to(uint16_t top)
{
    uint32_t Position;// 记录位置
    uint32_t temp = 0;
    while(Serial_RxPacket[top] != '\0')
    {
        temp = temp * 10 + (Serial_RxPacket[top] - '0');
        top ++;
    }
    Position = temp;
}

void SerialCommand_exe(void)
{
    if (Serial_RxFlag == 1)
	{
        // 显示串口命令内容
        Serial_RxFlag = 0;
		OLED_ShowString(4, 1, "Com:            ");
        OLED_ShowString(4, 5, Serial_RxPacket);
            
        // 解析命令
        char Command[100];
        uint16_t top = 0; // 字符串指针
        while(Serial_RxPacket[top] != ' ')
        {
            Command[top] = Serial_RxPacket[top];
            top ++;
        }
        Command[top] = '\0';
        top ++;
            
        // 对于命令具体数值进行分析
        if (strcmp(Command, "to") == 0)// 电机旋转命令
        {
            SerialCommand_to(top);
        }
    }
}

