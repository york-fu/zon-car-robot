/*********************************************************************************************
*�ļ�: key.c
*����: zonesion   2015.12.18
*˵��: �����������
*�޸�:
*ע��:
*********************************************************************************************/
#include "adc_key.h"
#include "stdio.h"
#include "user.h"
#include "SteeringEngine.h"

/*********************************************************************************************
����ȫ�ֱ���
*********************************************************************************************/
unsigned int key_status = 0;


void key_adc_init(void)
{
    GPIO_InitTypeDef      GPIO_InitStructure;
    
    
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);         //ʹ��GPIOCʱ��
    //�ȳ�ʼ��ADC2ͨ��0 IO��
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;        //PC2/PC3 ADCͨ��12/13
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;                  //ģ������
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;             //����������
    GPIO_Init(GPIOC, &GPIO_InitStructure);                        //��ʼ��
    
}

u16 get_key_status(u8 key)
{
    //����ָ��ADC�Ĺ�����ͨ����һ�����У�����ʱ��
    //ADC2,ADCͨ��12/13,112������,��߲���ʱ�������߾�ȷ��
    ADC_RegularChannelConfig(ADC1, key, 1, ADC_SampleTime_15Cycles );
    ADC_SoftwareStartConv(ADC1);		                        //ʹ��ָ����ADC2�����ת����������
    while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));               //�ȴ�ת������
    return ADC_GetConversionValue(ADC1);	                        //�������һ��ADC2�������ת�����
}


//�����ⲿ����
extern struct process qrcode;


#if     SYSTEM_SUPPORT_CONTIKI

//���������ж�ʱ��
#define LONG_TIME 100       
/*********************************************************************************************
���������
*********************************************************************************************/
PROCESS(key, "key");
process_event_t process_event_key;
/*********************************************************************************************
* ����: PROCESS_THREAD()
* ����: �����������
* ����: sensors, ev, data
* ����: ��
* �޸�:
* ע��:
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
        if(get_key_status(KEY1_2) > 2000){
            key_status_tmp = 0;
            for(;;){
                etimer_set(&key_timer, 10);
                PROCESS_YIELD();
                if(get_key_status(KEY1_2) > 2000)
                    key_status_tmp++;
                else break;
            }
            if(key_status_tmp >= LONG_TIME){
                key_status = KEY1_LONG_PRESSED;
                process_post(&qrcode,process_event_key,&key_status);
            }
            else{
                key_status = KEY1_PRESSED;
                process_post(&user,process_event_key,&key_status);
            }
        }
        
        else if(get_key_status(KEY1_2) > 1000){
            key_status_tmp = 0;
            for(;;){
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
                process_post(&qrcode,process_event_key,&key_status);
            }
        }
        
        if(get_key_status(KEY3_4) > 2000){
            key_status_tmp = 0;
            for(;;){
                etimer_set(&key_timer, 10);
                PROCESS_YIELD();
                if(get_key_status(KEY3_4) > 2000)
                    key_status_tmp++;
                else break;
            }
            if(key_status_tmp >= LONG_TIME){
                key_status = KEY3_LONG_PRESSED;
            }
            else{
                key_status = KEY3_PRESSED;
            }
        }
        
        else if(get_key_status(KEY3_4) > 1000){
            key_status_tmp = 0;
            for(;;){
                etimer_set(&key_timer, 10);
                PROCESS_YIELD();
                if((get_key_status(KEY3_4) > 1000) && (get_key_status(KEY3_4) < 2000))
                    key_status_tmp++;
                else break;
            }
            if(key_status_tmp >= LONG_TIME){
                key_status = KEY4_LONG_PRESSED;
            }
            else{
                key_status = KEY4_PRESSED;
            }
        }
        etimer_set(&key_timer,20);
        PROCESS_YIELD();
    }
    PROCESS_END();
}
#endif