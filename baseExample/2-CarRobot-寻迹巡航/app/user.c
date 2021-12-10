#include "user.h"
#include "zxbee.h"
#include "rtc.h"
#include "config.h"
#include "motor.h"
#include "SteeringEngine.h"
#include "led.h"
#include "adc_key.h"
#include "config.h"
#include "mg2639.h"
#include "sample.h"


//��������
PROCESS(user,"user");

/*********************************************************************************************
* ����: PROCESS_THREAD()
* ����: �������
* ����:
* ����: ��
* �޸�:
* ע��:
*********************************************************************************************/
PROCESS_THREAD(user, ev, data)
{    
    static struct etimer user_timer;
    
    PROCESS_BEGIN();
    
    sensor_init();                                              //��������ʼ��
    etimer_set(&user_timer,100);                                //����etimer��ʱ��
    
    while(1)
    {
        PROCESS_WAIT_EVENT();  
        
        if(ev == process_event_key)                             //�ж��Ƿ��������¼�
        {                               
            if((*(int*)data)==KEY1_PRESSED)                     //�ж��¼������ֵ
            {
                motor_run = !motor_run;
            }   
            else if((*(int*)data)==KEY3_PRESSED)                //�ж��¼������ֵ
            {
                if(motor_speed<8000)
                    motor_speed += 1000;
                    
            }
            else if((*(int*)data)==KEY4_PRESSED)                //�ж��¼������ֵ
            {
                if(motor_speed>5000)
                    motor_speed -= 1000;
            }
        }
        
        if(ev == PROCESS_EVENT_TIMER)
        {
            etimer_reset(&user_timer); 
            
            sensor_poll();    
        }
    }
    
    PROCESS_END();
}

            
          