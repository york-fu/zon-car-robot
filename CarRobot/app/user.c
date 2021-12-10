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

/*********************************************************************************************
С�����Ʋ���
*********************************************************************************************/
/*
С���ƶ�ģʽ
D1��0x00-->ֹͣ��0x01-->�ֶ�ģʽ��0x02-->Ѱ��ģʽ��0x04-->����ģʽ
*/
uint8 D1=0;
/*
�ֶ�ģʽ������Ʋ���
V2������ٶȣ�0--84��
D2���˶�����0x00-->ֹͣ��0x01-->ǰ����0x02-->���ˣ�0x04-->��ת��0x08-->��ת��
*/
uint8 V2=0,D2=0;

/*********************************************************************************************
��е�ۿ��Ʋ���
*********************************************************************************************/
uint8 V3=6,V4=10,V5=90,V6=0;

/*
��е���Զ��嶯������--V7
0���޶���
1���Լ�
2����ˮ
3��������
4���Ӷ���
*/
uint8 V7=0;


/*********************************************************************************************
�������
*********************************************************************************************/
//����ģʽ��0-->������1-->gsm
uint8 connect_mode=0;
//����MAC
char RADIO_MAC[32];

process_event_t d2Recv_event;


void config_save_alarm();
void config_save_phone();
void config_save_notify();


extern void uartData_handle();
extern void ble_init(void);

extern char qrcode_flag;


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
    static u16 user_t=0,d2_t=0;
    
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
                if(qrcode_flag==0)
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
        
        if(ev == d2Recv_event)
        {
            d2_t=0;
        }
        
        if(ev == PROCESS_EVENT_TIMER)
        {
            etimer_reset(&user_timer); 
            
//            uartData_handle();

            /*�����ƶ�ģʽ����*/           
            motion_mode= D1;
            
            /*���µ������*/
            motor_direction = D2;
            motor_speed = V2*100;
            
            /*���»�е�۲���*/
            robotArm.x = V3;
            robotArm.y = V4;
            robotArm.angle_3 = V5;
            robotArm.angle_4 = V6;            
            
            /*�ж�V7�����л�е���Զ��嶯��*/
            if(V7!=0)
            {                
                process_post(&robotArm_motion,robotArm_motion_event,&V7);//���ͻ�е�۶����¼�
            }
            
            user_t++;
            d2_t++;
            
            if((user_t%5)==0)
            {
                user_t = 0;
                sensor_poll();                 
            }                                             //��������ѯ����
            
            /*2s��λ,��ʧ��*/
            if(d2_t>19)
            {
                d2_t = 0;
//                D2 = 0;
            }
        }
    }
    
    PROCESS_END();
}

            
          