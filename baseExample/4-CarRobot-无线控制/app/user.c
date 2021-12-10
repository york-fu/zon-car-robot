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
#include "ble.h"


//LED���Ʋ�����0-->�صƣ�1-->����
uint8 D1=0;


/*********************************************************************************************
�������
*********************************************************************************************/
//����ģʽ��0-->������1-->gsm
uint8 connect_mode=0;
//����MAC
char RADIO_MAC[32];


extern char qrcodeShow_flag;


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
        
        if(ev == process_event_key) //�ж��Ƿ��������¼�
        {                               
            if((*(int*)data)==KEY1_PRESSED)//�¼������ֵ
            {
                if(qrcodeShow_flag==0)
                {
                    connect_mode = !connect_mode;
                    
                    if(connect_mode==0)                             //��������ģʽ
                    {                
                        ble_init();
                        config_save_mode();
                    }
                    else                                            //GSM����ģʽ
                    {
                        mg2639_init();
                        config_save_mode();
                    }
                }
            }            
        }
        
        if(ev == PROCESS_EVENT_TIMER)
        {
            etimer_reset(&user_timer); 
            
            sensor_poll();  
            led2 = !D1;                                         //����LED״̬
            led3 = !D1;                                         //����LED״̬
            led4 = !D1;                                         //����LED״̬
        }
    }
    
    PROCESS_END();
}

            
          