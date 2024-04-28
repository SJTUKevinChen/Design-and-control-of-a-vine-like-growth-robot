#include "stm32f10x.h"                  // Device header
#include "Kalman.h"

void PressSenor_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    RCC_ADCCLKConfig(RCC_PCLK2_Div2);
    
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    ADC_InitTypeDef ADC_InitStructure;
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_Init(ADC1, &ADC_InitStructure);
    
    ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 1, ADC_SampleTime_55Cycles5);
    
    ADC_Cmd(ADC1, ENABLE);
    
    ADC_ResetCalibration(ADC1);
    while (ADC_GetResetCalibrationStatus(ADC1) == SET);
    ADC_StartCalibration(ADC1);
    while (ADC_GetSoftwareStartConvStatus(ADC1));
    
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

uint16_t PressSenor_GetValue(void)
{
    static uint16_t epoch = 0;
    static KalmanFilter EKF = {0.02, 0, 0, 0, 0.001, 0.543};// 初始化Kalman滤波器参数
    static uint8_t Flag = 0;// 表示是否记录了漂移误差
    static uint16_t Zero_Shift = 0;// 记录漂移误差
    uint16_t Press_Value = ADC_GetConversionValue(ADC1);
    kalmanfiter(&EKF, (float)(Press_Value - Zero_Shift) / 4095 * 3.3 * 1000 / 40 * 6.894757);
    
    if (epoch < 300)
    {
        epoch ++;
        return 0xFFFF;//表示正在初始化
    }
    else
    {
        
        if (Flag == 0)
        {
            Zero_Shift = Press_Value;
            Flag = 1;
        }
        //if (Press_Value < 0.5 / 3.3 * 4095) return 0;// 0~0.5V为传感器无效输出范围
        return (uint16_t)(float)EKF.Out;
    }
 }// / 4095 * 3.3 * 1000 / 40 psi
