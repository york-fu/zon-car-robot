/*********************************************************************************************
*文件: key.c
*作者: zonesion   2015.12.18
*说明: 按键处理程序
*修改:
*注释:
*********************************************************************************************/
#include "adc_key.h"
#include "stdio.h"
#include "user.h"
#include "SteeringEngine.h"

/*********************************************************************************************
定义全局变量
*********************************************************************************************/
unsigned int key_status = 0;
void key_adc_init(void)
{
    GPIO_InitTypeDef      GPIO_InitStructure;
    
    
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);         //使能GPIOC时钟
    //先初始化ADC2通道0 IO口
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;        //PC2/PC3 ADC通道12/13
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;                  //模拟输入
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;             //不带上下拉
    GPIO_Init(GPIOC, &GPIO_InitStructure);                        //初始化
    
}

u16 get_key_status(u8 key)
{
    //设置指定ADC的规则组通道，一个序列，采样时间
    //ADC2,ADC通道12/13,112个周期,提高采样时间可以提高精确度
    ADC_RegularChannelConfig(ADC1, key, 1, ADC_SampleTime_15Cycles );
    ADC_SoftwareStartConv(ADC1);		                        //使能指定的ADC2的软件转换启动功能
    while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));               //等待转换结束
    return ADC_GetConversionValue(ADC1);	                        //返回最近一次ADC2规则组的转换结果
}


//声明外部进程
extern struct process qrcode;


#if SYSTEM_SUPPORT_CONTIKI

//按键长按判断时间
#define LONG_TIME 100  

process_event_t process_event_key; 

/*********************************************************************************************
定义进程名
*********************************************************************************************/
PROCESS(key, "key");
/*********************************************************************************************
* 名称: PROCESS_THREAD()
* 功能: 按键处理进程
* 参数: sensors, ev, data
* 返回: 无
* 修改:
* 注释:
*********************************************************************************************/
PROCESS_THREAD(key, ev, data)
{
    static struct etimer key_timer;
    static unsigned char key_status_tmp = 0;
    
    PROCESS_BEGIN();
    
    key_adc_init();
    adc_init();
    process_event_key = process_alloc_event();
    
    while (1) 
    {
        //K1，K2检测
        if(get_key_status(KEY1_2) > 2000)
        {
            key_status_tmp = 0;
            for(;;)
            {
                etimer_set(&key_timer, 10);
                PROCESS_YIELD();
                if(get_key_status(KEY1_2) > 2000)
                    key_status_tmp++;
                else break;
            }
            if(key_status_tmp >= LONG_TIME)
            {
                key_status = KEY1_LONG_PRESSED;
            }
            else
            {
                key_status = KEY1_PRESSED;
                process_post(&user,process_event_key,&key_status);
            }
        }       
        else if(get_key_status(KEY1_2) > 1000)
        {
            key_status_tmp = 0;
            for(;;)
            {
                etimer_set(&key_timer, 10);
                PROCESS_YIELD();
                if((get_key_status(KEY1_2) > 1000) && (get_key_status(KEY1_2) < 2000))
                    key_status_tmp++;
                else break;
            }
            if(key_status_tmp >= LONG_TIME)
            {
                key_status = KEY2_LONG_PRESSED;
            }
            else
            {
                key_status = KEY2_PRESSED;
                process_post(&user,process_event_key,&key_status);
            }
        }
        
        //K3，K4检测
        if(get_key_status(KEY3_4) > 2000)
        {
            key_status_tmp = 0;
            for(;;)
            {
                etimer_set(&key_timer, 10);
                PROCESS_YIELD();
                if(get_key_status(KEY3_4) > 2000)
                    key_status_tmp++;
                else break;
            }
            if(key_status_tmp >= LONG_TIME)
            {
                key_status = KEY3_LONG_PRESSED;
            }
            else
            {
                key_status = KEY3_PRESSED;
                process_post(&user,process_event_key,&key_status);
            }
        }
        else if(get_key_status(KEY3_4) > 1000)
        {
            key_status_tmp = 0;
            for(;;)
            {
                etimer_set(&key_timer, 10);
                PROCESS_YIELD();
                if((get_key_status(KEY3_4) > 1000) && (get_key_status(KEY3_4) < 2000))
                    key_status_tmp++;
                else break;
            }
            if(key_status_tmp >= LONG_TIME)
            {
                key_status = KEY4_LONG_PRESSED;
            }
            else
            {
                key_status = KEY4_PRESSED;
                process_post(&user,process_event_key,&key_status);
            }
        }
        etimer_set(&key_timer,20);
        PROCESS_YIELD();
    }
    PROCESS_END();
}
#endif